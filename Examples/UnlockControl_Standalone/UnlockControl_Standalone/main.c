/*
 * Demonstration project for the minimalized LaRoomy Api
 *
 * A UnlockControl property is added and executed in standalone mode, the key will be saved permanently to eeprom
 *
 */
#define F_CPU 9830400UL
#define BAUD 57600UL

#define FALSE 0
#define TRUE  1

#define ON  1
#define OFF 0

// RN487x status pins
#define RN4870_STATUS_1 PORTD6
#define RN4870_STATUS_2 PORTD7

// RN487x status codes:
#define RN4870_STATUS_POWER_ON               1
#define RN4870_STATUS_STANDBY_STATE          2
#define RN4870_STATUS_CONNECTION_ESTABLISHED 3
#define RN4870_STATUS_DATA_SESSION_OPEN      4

// indication LED
#define CONNECTION_STATE_LED_PIN PORTC0
#define UNLOCK_LED_LOCKED        PORTC1
#define UNLOCK_LED_UNLOCKED      PORTC2

//	Property ID definitions
#define PROPERTY_ID_UNLOCK_CONTROL 1 // unlock control

#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>

#include "a328_rn487x_com.h"
#include "atmega328_interrupt.h"
#include "atmega328_timer.h"

#include "LaRoomyApi.h"

// this function will be used by the api to report remote events
void propertyCallback(uint8_t propertyID, uint8_t eventID);

// this function is used to get the current status of the rn487x module
uint8_t get_RN487X_Status();

// get the connection state
uint8_t getConnectionState();

// create the data-holder for the property
DEVICE_PROPERTY unlockControl;

// create the data-holder for the property state
UNLOCK_CONTROL_STATE unlockControlState;

uint8_t noSleep;
uint8_t sleepRequested;
uint8_t preSleepCounter;
uint8_t wrongPinCounter;

char eeUnlockControlKey[] EEMEM = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t eeValid EEMEM = 0;

ISR(TIMER0_OVF_vect)
{
    timerCounter++;

    if (timerCounter >= 37) // 1 second event
    {
        timerEvent = 1;
        timerCounter = 0;
    }
}

ISR(PCINT2_vect)
{
    // wake up
}

int main(void)
{
    _delay_ms(800);

    // set data directions
    DDRB = 0b00000000;
    DDRC = 0b00000111; // pc0, pc1 and pc2 as output for control led
    DDRD = 0b00100000; // rx ind pin output pd5

    // set port states
    PORTB = 0b00000000;
    PORTC = 0b00000010; // locked led initially on
    PORTD = 0b00100000; // rx ind high -> rx not responding in sleep mode

    // initialize usart communication controller
    comInit();

    // init parameter
    noSleep = 0;
    sleepRequested = 0;
    preSleepCounter = 0;
    wrongPinCounter = 0;

    // set the usart output gateway for the laroomy api
    // - the internal transmission control uses this function to send data
    setOutGateway(sendBLEData);

    // set the callback to catch remote events
    // every time the user interacts with the laroomy app, this callback will be invoked
    registerRemoteCallback(propertyCallback);

    // set the laroomy api receiver function as usart reception handler
    // - this is the input gateway for the laroomy api. The received data must be passed to the api to get processed.
    setDataReceptionHandler(onDataReceived);

    // init the property structs
    initDevicePropertyStruct(&unlockControl);

    // define the property data:

    unlockControl.ID = PROPERTY_ID_UNLOCK_CONTROL;
    unlockControl.type = PTYPE_UNLOCK_CONTROL;
    unlockControl.imageID = LOCK_CLOSED_021;
    setPropertyDescriptor(&unlockControl, "Access Control");

    // add the unlock control property to the api
    addDeviceProperty(&unlockControl);

    // add the respective state to the api + set default values + load saved key
    unlockControlState.lockState = UC_LOCKED;
    unlockControlState.mode = 0;
    unlockControlState.flags = 0;

    // read the key from eeprom
    char ucKey[11];
    uint8_t isEEKeyValid = eeprom_read_byte(&eeValid);
    eeprom_read_block(ucKey, eeUnlockControlKey, 11);

    // check if the saved key is valid, if not, set a default key
    if (isEEKeyValid == 1)
    {
        strcpy(unlockControlState.key, ucKey);
    }
    else
    {
        memset(unlockControlState.key, 0, sizeof(unlockControlState.key));
        strcpy(unlockControlState.key, "123456");
    }

    addUnlockControlStateForProperty(PROPERTY_ID_UNLOCK_CONTROL, &unlockControlState);

    // activate the standalone mode
    setStandAloneMode(true);

    // activate pin change interrupt to wake up when the rn487x gets connected
    enablePinChangeInterruptOnStatusPin();

    // activate timer
    activateTimer0();

    while (1)
    {
        // sleep if applicable
        if (sleepRequested)
        {
            preSleepCounter = 0;
            sleepRequested = 0;

            set_sleep_mode(SLEEP_MODE_PWR_DOWN);
            sleep_mode();
        }

        // set the connection state in the laroomy api
        // -> this is necessary for internal processing
        setConnectionState(getConnectionState());

        // this function must be called periodically in the loop to fetch the received
        // ble data (if there is any) from the reception controller
        fetchBleData();

        if (sleepRequested == 0)
        {
            // check for incoming transmissions
            fetchBleData();

            // timer event:
            if (timerEvent)
            {
                // the pre-sleep counter implements a delay before the controller goes into sleep
                // (sometimes there is a short delay in the change of the status pins of the rn487x - this could be misinterpreted)
                if (preSleepCounter != 0)
                {
                    preSleepCounter++;

                    if (preSleepCounter >= 3)
                    {
                        sleepRequested = 1;
                    }
                }
                timerEvent = 0;
            }

            if (getConnectionState() == CONNECTED)
            {
                PORTC |= (1 << CONNECTION_STATE_LED_PIN);
                preSleepCounter = 0;
            }
            else
            {
                PORTC &= ~(1 << CONNECTION_STATE_LED_PIN);

                if (preSleepCounter == 0)
                {
                    preSleepCounter = 1;
                }
            }
        }
    }
}

void propertyCallback(uint8_t propertyID, uint8_t eventID)
{
    // catch the event
    switch (eventID)
    {
    case REVT_UNLOCK_CONTROL_STATE_CHANGED:
        // make sure to catch the correct property
        if (propertyID == PROPERTY_ID_UNLOCK_CONTROL)
        {
            PUNLOCK_CONTROL_STATE pState = getUnlockControlState(propertyID);
            if (pState != NULL)
            {
                // check unlock mode
                if (pState->mode == UCMODE_UNLOCK_MODE)
                {
                    if (pState->lockState == UC_LOCKED)
                    {
                        // lock command
                        PORTC |= (1 << UNLOCK_LED_LOCKED);
                        PORTC &= ~(1 << UNLOCK_LED_UNLOCKED);
                    }
                    else
                    {
                        // unlock command
                        PORTC |= (1 << UNLOCK_LED_UNLOCKED);
                        PORTC &= ~(1 << UNLOCK_LED_LOCKED);
                    }
                }
                else
                {
                    // pin was changed - save it permanently
                    eeprom_update_block(pState->key, eeUnlockControlKey, sizeof(pState->key));
                    eeprom_update_byte(&eeValid, 1);
                }
            }
        }
        break;
    case REVT_UNLOCK_CONTROL_INVALID_OPERATION:
        if (getLastInvalidUnlockControlOperation() == UNLOCKCTRL_UNLOCK_REJECTED_WRONG_PIN)
        {
            // a wrong pin was entered 3 times
            if (wrongPinCounter >= 3)
            {
                // close connection
                sendCloseDeviceCommand();
                wrongPinCounter = 0;
            }
            else
            {
                // error blink
                PORTC |= (1 << UNLOCK_LED_LOCKED);
                PORTC |= (1 << UNLOCK_LED_UNLOCKED);
                _delay_ms(100);
                PORTC &= ~(1 << UNLOCK_LED_LOCKED);
                PORTC &= ~(1 << UNLOCK_LED_UNLOCKED);
                _delay_ms(100);
                PORTC |= (1 << UNLOCK_LED_LOCKED);
                PORTC |= (1 << UNLOCK_LED_UNLOCKED);
                _delay_ms(100);
                PORTC &= ~(1 << UNLOCK_LED_LOCKED);
                PORTC &= ~(1 << UNLOCK_LED_UNLOCKED);
                _delay_ms(100);
                PORTC |= (1 << UNLOCK_LED_LOCKED);
                PORTC &= ~(1 << UNLOCK_LED_UNLOCKED);

                wrongPinCounter++;
            }
        }
        break;
    default:
        break;
    }
}

uint8_t get_RN487X_Status()
{
    uint8_t status1 = PIND & (1 << RN4870_STATUS_1);
    uint8_t status2 = PIND & (1 << RN4870_STATUS_2);

    if ((status1 != 0) && (status2 != 0))
    {
        // both are non-zero
        return RN4870_STATUS_POWER_ON;
    }
    else if ((status1 != 0) && (status2 == 0))
    {
        return RN4870_STATUS_STANDBY_STATE;
    }
    else if ((status1 == 0) && (status2 != 0))
    {
        return RN4870_STATUS_DATA_SESSION_OPEN;
    }
    else
    {
        // both are zero
        return RN4870_STATUS_CONNECTION_ESTABLISHED;
    }
}

uint8_t getConnectionState()
{
    uint8_t rn487xStatus = get_RN487X_Status();
    if (rn487xStatus == RN4870_STATUS_CONNECTION_ESTABLISHED || rn487xStatus == RN4870_STATUS_DATA_SESSION_OPEN)
    {
        return CONNECTED;
    }
    else
    {
        return DISCONNECTED;
    }
}
