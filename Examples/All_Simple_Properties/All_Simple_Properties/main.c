/*
 * Demonstration project for the minimalized version of the LaRoomy Api
 *
 * All properties with simple property state are added to the api to demonstrate its usage
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
#define CONNECTION_STATE_LED_PIN   PORTC0
#define BUTTON_TEST_LED_PIN        PORTC1
#define SWITCH_TEST_LED_PIN        PORTC2
#define LEVEL_SELECTOR_LED_PIN     PORTB1

// hardware buttons

//#define HBUTTON_1_PIN PORTB1
//#define HBUTTON_2_PIN PORTB2        

//	Property ID's
#define PROPERTY_ID_BUTTON 1			// button
#define PROPERTY_ID_SWITCH 2			// switch
#define PROPERTY_ID_LEVEL_SELECTOR 3	// level selector
#define PROPERTY_ID_LEVEL_INDICATOR 4	// level indicator
#define PROPERTY_ID_TEXT_DISPLAY 5		// text display
#define PROPERTY_ID_OPTION_SELECTOR 6	// option selector

#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>

#include "a328_rn487x_com.h"
#include "atmega328_interrupt.h"
#include "atmega328_timer.h"

#include "../../../LaRoomyApi/LaRoomyApi.h"

// this function will be used by the api to report remote events
void propertyCallback(uint8_t propertyID, uint8_t eventID);

// this function is used to get the current status of the rn487x module
uint8_t get_RN487X_Status();

// get the connection state
uint8_t getConnectionState();

// create the data-holder for the properties
DEVICE_PROPERTY dpButton;
DEVICE_PROPERTY dpSwitch;
DEVICE_PROPERTY dpLevelSelector;
DEVICE_PROPERTY dpLevelIndicator;
DEVICE_PROPERTY dpTextDisplay;
DEVICE_PROPERTY dpOptionSelector;

uint8_t noSleep;
uint8_t sleepRequested;
uint8_t preSleepCounter;

ISR(TIMER0_OVF_vect)
{
    timerCounter++;

    if (timerCounter >= 37) // ~1 second event
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
    DDRB = 0b00000000; // pb1 as pwm output for level selector          //& pb2 as input from hardware buttons
    DDRC = 0b00001111; // pc0, pc1, pc2 and pc3 as output for control led
    DDRD = 0b00100000; // rx ind pin output pd5

    // set port states
    PORTB = 0b00000000;
    PORTC = 0b00000000; // all leds initially off
    PORTD = 0b00100000; // rx ind high -> rx not responding in sleep mode

    // initialize usart communication controller
    comInit();

    // init parameter
    noSleep = 0;
    sleepRequested = 0;
    preSleepCounter = 0;

    // set the usart output gateway for the laroomy api
    // - the internal transmission control will use this function to send data
    setOutGateway(sendBLEData);

    // set the callback to catch remote events
    // every time the user interacts with the laroomy app, this callback will be invoked
    registerRemoteCallback(propertyCallback);

    // set the laroomy api receiver function as usart reception handler
    // - this is the input gateway for the laroomy api. The received data must be passed to the api to get processed.
    setDataReceptionHandler(onDataReceived);

    // init the property structs
    initDevicePropertyStruct(&dpButton);
    initDevicePropertyStruct(&dpSwitch);
    initDevicePropertyStruct(&dpLevelSelector);
    initDevicePropertyStruct(&dpLevelIndicator);
    initDevicePropertyStruct(&dpTextDisplay);
    initDevicePropertyStruct(&dpOptionSelector);

    // define the property data:

    // button definition
    dpButton.propertyID = PROPERTY_ID_BUTTON;
    dpButton.propertyType = PROPERTY_TYPE_BUTTON;
    dpButton.imageID = LIGHT_BULB_004;
    setPropertyDescriptor(&dpButton, "Press the;;Button"); // dual descriptor: element-text and button-text

    // add the button
    addDeviceProperty(&dpButton);

    // switch definition
    dpSwitch.propertyID = PROPERTY_ID_SWITCH;
    dpSwitch.propertyType = PROPERTY_TYPE_SWITCH;
    dpSwitch.imageID = LIGHT_SETUP_005;
    setPropertyDescriptor(&dpSwitch, "Example Switch");

    // add the switch
    addDeviceProperty(&dpSwitch);

    // level selector definition
    dpLevelSelector.propertyID = PROPERTY_ID_LEVEL_SELECTOR;
    dpLevelSelector.propertyType = PROPERTY_TYPE_LEVEL_SELECTOR;
    dpLevelSelector.imageID = LEVEL_ADJUST_043;
    setPropertyDescriptor(&dpLevelSelector, "Select a Level");

    // add the level selector
    addDeviceProperty(&dpLevelSelector);

    // level indicator definition
    dpLevelIndicator.propertyID = PROPERTY_ID_LEVEL_INDICATOR;
    dpLevelIndicator.propertyType = PROPERTY_TYPE_LEVEL_INDICATOR;
    dpLevelIndicator.imageID = BATTERY_75P_033;
    setPropertyDescriptor(&dpLevelIndicator, "Level Indication");

    // add the level indicator
    addDeviceProperty(&dpLevelIndicator);

    // text display definition
    dpTextDisplay.propertyID = PROPERTY_ID_TEXT_DISPLAY;
    dpTextDisplay.propertyType = PROPERTY_TYPE_TEXT_DISPLAY;
    dpTextDisplay.imageID = NOTIFICATION_067;
    setPropertyDescriptor(&dpTextDisplay, "Status: Ready");

    // add the text display
    addDeviceProperty(&dpTextDisplay);

    // option selector definition
    dpOptionSelector.propertyID = PROPERTY_ID_OPTION_SELECTOR;
    dpOptionSelector.propertyType = PROPERTY_TYPE_OPTION_SELECTOR;
    dpOptionSelector.imageID = OPTION_LIST_170;
    setPropertyDescriptor(&dpOptionSelector, "Select an Option;;Option 1;;Option 2;;Option 3");

    // add the option selector
    addDeviceProperty(&dpOptionSelector);

    // activate pin change interrupt to wake up when the rn487x gets connected
    enablePinChangeInterruptOnStatusPin();

    // activate timer
    activateTimer0();

    // config pwm timer setup
    InitPWMTimer1();

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
        // - this is necessary for internal processing
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

            // check the connection state and set the connection state led accordingly plus control the pre-sleep counter parameter
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
    case REVT_BUTTON_PRESSED:
        // make sure to address the correct property
        if (propertyID == PROPERTY_ID_BUTTON)
        {
            // toggle led
            PORTC ^= (1 << BUTTON_TEST_LED_PIN);
        }
        break;
    case REVT_SWITCH_STATE_CHANGED:
        // make sure to address the correct property
        if (propertyID == PROPERTY_ID_SWITCH)
        {
            // set the led with respect to the new state
            uint8_t newState = getSimplePropertyState(propertyID);
            if (newState == OFF)
            {
                PORTC &= ~(1 << SWITCH_TEST_LED_PIN);
            }
            else
            {
                PORTC |= (1 << SWITCH_TEST_LED_PIN);
            }
        }
        break;
    case REVT_LEVEL_SELECTOR_VALUE_CHANGED:
        // make sure to address the correct property
        if(propertyID == PROPERTY_ID_LEVEL_SELECTOR)
        {
            // get the new value
            uint8_t newValue = getSimplePropertyState(propertyID);

            // set the led with respect to the new value
            if(newValue == 0)
            {
                SetPWMValue(PWM_CHANNEL_A, 0);
                StopPWM(PWM_CHANNEL_A);
            }
            else
            {
                StartPWM(PWM_CHANNEL_A);
                SetPWMValue(PWM_CHANNEL_A, newValue);
            }
        }
        break;
    case REVT_OPTION_SELECTOR_VALUE_CHANGED:
        // make sure to address the correct property
        if(propertyID == PROPERTY_ID_OPTION_SELECTOR)
        {
            // get the new value
            uint8_t newValue = getSimplePropertyState(propertyID);

            // update the text display to show the current option in the app
            char statusText[] = "Option X was selected";
            
            if(newValue == 0)
            {
                statusText[7] = '1';
            }
            else if(newValue == 1)
            {
                statusText[7] = '2';
            }
            else
            {
                statusText[7] = '3';
            }

            PDEVICE_PROPERTY textDisplay = getDevicePropertyFromID(PROPERTY_ID_TEXT_DISPLAY);
            if(textDisplay != NULL)
            {
                setPropertyDescriptor(textDisplay, statusText);
                updateDeviceProperty(textDisplay);
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
