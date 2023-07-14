/*
 * Demonstration project for the minimalized version of the LaRoomy Api
 *
 * The current time is saved in a time-keeper ic (mcp 79410) and the Time- and TimeFrame- Selectors are used to control leds.
 * The TIME_SELECTOR_LED is disabled if the current time matches the selected time
 * The TIME_FRAME_SELECTOR_LED is enable if the current time is in the selected time frame
 *
 */
#define F_CPU 9830400UL
#define BAUD 57600UL

#define FALSE 0
#define TRUE 1

#define ON 1
#define OFF 0

// RN487x status pins
#define RN4870_STATUS_1 PORTD6
#define RN4870_STATUS_2 PORTD7

// RN487x status codes:
#define RN4870_STATUS_POWER_ON 1
#define RN4870_STATUS_STANDBY_STATE 2
#define RN4870_STATUS_CONNECTION_ESTABLISHED 3
#define RN4870_STATUS_DATA_SESSION_OPEN 4

// indication LED
#define CONNECTION_STATE_LED_PIN PORTC0
#define TIME_SELECTOR_LED PORTC1
#define TIME_FRAME_SELECTOR_LED PORTC2

// hardware buttons
#define HBUTTON_PIN PORTB2

//	Property ID's
#define PROPERTY_ID_BUTTON 1             // button
#define PROPERTY_ID_TIME_SELECTOR 2      // time selector
#define PROPERTY_ID_TIMEFRAME_SELECTOR 3 // time frame selector

#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>

#include "a328_rn487x_com.h"
#include "atmega328_interrupt.h"
#include "atmega328_timer.h"
#include "mcp79410.h"

#include "LaRoomyApi.h"

// this function will be used by the api to report remote events
void propertyCallback(uint8_t propertyID, uint8_t eventID);

// this function is used to get the current status of the rn487x module
uint8_t get_RN487X_Status();

// get the connection state
uint8_t getConnectionState();

void InitTimeKeeper();

// create the data-holder for the properties
DEVICE_PROPERTY dpButton;
DEVICE_PROPERTY dpTimeSelector;
DEVICE_PROPERTY dpTimeFrameSelector;

// create the data-holder for the complex property states
TIME_SELECTOR_STATE timeSelectorState;
TIME_FRAME_STATE timeFrameState;

uint8_t noSleep;
uint8_t sleepRequested;
uint8_t preSleepCounter;
uint8_t one_second_event;
uint8_t timeRequestResposeControl;

ISR(TIMER0_OVF_vect)
{
    timerCounter++;

    if (timerCounter >= 37) // ~1 second event
    {
        timerEvent = 1;
        timerCounter = 0;
    }
}

ISR(INT0_vect)
{
    one_second_event = 1;
}

ISR(PCINT2_vect)
{
    // wake up
}

int main(void)
{
    _delay_ms(800);

    // set data directions
    DDRB = 0b00000000; // pb2 as input from a hardware button
    DDRC = 0b00001111; // pc0, pc1, pc2 and pc3 as output for control led
    DDRD = 0b00100000; // rx ind pin output pd5

    // set port states
    PORTB = 0b00000000;
    PORTC = 0b00000000; // all leds initially off
    PORTD = 0b00100000; // rx ind high -> rx not responding in sleep mode

    // initialize usart communication controller
    comInit();

    // initialize the mcp79410 timekeeper ic
    InitTimeKeeper();

    // init parameter
    noSleep = 0;
    sleepRequested = 0;
    preSleepCounter = 0;
    timeRequestResposeControl = 0;
    one_second_event = 0;

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
    initDevicePropertyStruct(&dpTimeSelector);
    initDevicePropertyStruct(&dpTimeFrameSelector);

    // define the property data:

    // button definition
    dpButton.ID = PROPERTY_ID_BUTTON;
    dpButton.type = PTYPE_BUTTON;
    dpButton.imageID = LIGHT_BULB_004;
    setPropertyDescriptor(&dpButton, "Time Selector LED;;Toggle"); // dual descriptor: element-text and button-text

    // add the button
    addDeviceProperty(&dpButton);

    // time selector definition
    dpTimeSelector.ID = PROPERTY_ID_TIME_SELECTOR;
    dpTimeSelector.type = PTYPE_TIME_SELECTOR;
    dpTimeSelector.imageID = TIME_012;
    setPropertyDescriptor(&dpTimeSelector, "Select disable time");

    // add the time selector
    addDeviceProperty(&dpTimeSelector);

    // add the state for the time selector (+set default values)
    timeSelectorState.hour = 20;
    timeSelectorState.minute = 0;
    addTimeSelectorStateForProperty(PROPERTY_ID_TIME_SELECTOR, &timeSelectorState);

    // time frame selector definition
    dpTimeFrameSelector.ID = PROPERTY_ID_TIMEFRAME_SELECTOR;
    dpTimeFrameSelector.type = PTYPE_TIME_FRAME_SELECTOR;
    dpTimeFrameSelector.imageID = TIME_SETUP_014;
    setPropertyDescriptor(&dpTimeFrameSelector, "Select a time frame");

    // add the time frame selector
    addDeviceProperty(&dpTimeFrameSelector);

    // add the state for the time frame selector (+set default values)
    timeFrameState.start_hour = 20;
    timeFrameState.start_minute = 0;
    timeFrameState.end_hour = 22;
    timeFrameState.end_minute = 0;
    addTimeFrameSelectorStateForProperty(PROPERTY_ID_TIMEFRAME_SELECTOR, &timeFrameState);

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
        // - this is necessary for internal processing
        setConnectionState(getConnectionState());

        if (sleepRequested == 0)
        {
            // this function must be called periodically to fetch the received
            // ble data (if there is any) from the reception controller
            fetchBleData();

            // timer event:
            if (timerEvent)
            {
                // check if the time request was received, if not, send the request again
                if (timeRequestResposeControl == 1)
                {
                    sendTimeRequest();
                    timeRequestResposeControl = 2;
                }
                else if (timeRequestResposeControl == 2)
                {
                    timeRequestResposeControl = 1;
                }

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

            if (one_second_event)
            {
                // check time and control the leds respectively

                // get system time
                uint16_t time = GetSystemTime();
                uint8_t currentHour = LOBYTE(time);
                uint8_t currentMinute = HIBYTE(time);

                // check time selector time
                PTIME_SELECTOR_STATE pTimeSelectorState = getTimeSelectorState(PROPERTY_ID_TIME_SELECTOR);
                if (pTimeSelectorState != NULL)
                {
                    // if the time equals the current time, disable the led
                    if (pTimeSelectorState->hour == currentHour && pTimeSelectorState->minute == currentMinute)
                    {
                        PORTC &= ~(1 << TIME_SELECTOR_LED);
                    }
                }

                // check time frame
                PTIME_FRAME_STATE pTimeFrameState = getTimeFrameSelectorState(PROPERTY_ID_TIMEFRAME_SELECTOR);
                if (pTimeFrameState != NULL)
                {
                    if (isTimeInFrame(pTimeFrameState, currentHour, currentMinute))
                    {
                        PORTC |= (1 << TIME_FRAME_SELECTOR_LED);
                    }
                    else
                    {
                        PORTC &= ~(1 << TIME_FRAME_SELECTOR_LED);
                    }
                }
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

            // check the hardware button
            if (!(PINB & (1 << HBUTTON_PIN)))
            {
                _delay_ms(20);

                // send a user message to the app
                sendUserMessage("Hello user - this is an example message!", USER_MESSAGE_TYPE_INFO, USER_MESSAGE_DISPLAY_TIME_7P5_SEC);

                // debounce the hardware button
                while (!(PINB & (1 << HBUTTON_PIN)))
                    ;
                _delay_ms(20);
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
            PORTC ^= (1 << TIME_SELECTOR_LED);
        }
        break;
    case REVT_PROPERTY_LOADING_FROM_DEVICE_COMPLETE:
        // when properties are loaded, send a time request to save the current time
        // to the time keeper
        sendTimeRequest();
        timeRequestResposeControl = 1;
        break;
    case REVT_PROPERTY_LOADING_FROM_CACHE_COMPLETE:
        // when properties are loaded, send a time request to save the current time
        // to the time keeper
        sendTimeRequest();
        timeRequestResposeControl = 1;
        break;
    case REVT_TIME_REQUEST_RESPONSE:
    {
        uint8_t hours;
        uint8_t minutes;
        uint8_t seconds;

        // fetch time data
        fetchTimeResponseData(&hours, &minutes, &seconds);
        timeRequestResposeControl = 3;

        // setup rtc
        Timekeeper_SetTime(hours, minutes, seconds);
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

void InitTimeKeeper()
{
    // if the power was lost and no backup battery was connected, the oscillator must be restarted
    if (Timekeeper_CheckFullPowerLoss())
    {
        // enable oscillator
        SetTimeKeeperRTCCRegister(TCREG_SECONDS, ST_OSCILLATOR_ENABLE);
        // enable backup battery
        Timekeeper_EnableBackupBattery();
        // enable 1Hz square wave on INT0 pin
        SetTimekeeperControlRegister(0x40);
    }
    // check if the primary power was lost and erase the fail flag
    if (Timekeeper_CheckPrimaryPowerLoss())
    {
        Timekeeper_ErasePowerFailFlag();
    }
    // setup the interrupt on INT0 to enable a wakeup condition every second to check the current time
    setupINT0_Interrupt();
}
