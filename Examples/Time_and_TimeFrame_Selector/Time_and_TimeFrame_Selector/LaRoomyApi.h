#ifndef LAROOMY_API_H
#define LAROOMY_API_H

/*  The LaRoomy Framework is made to quickly implement Bluetooth remote control capabilities
 *  to your project. It works in conjunction with the LaRoomy App
 *
 *  Read the full documentation: https://api.laroomy.com/
 *
 * Copyright 2023 Hans Philipp Zimmermann
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "imageID.h"

#define FLAG_DEV_INFO_CACHING_PERMISSION 0x01
#define FLAG_DEV_INFO_BINDING_REQUIRED   0x02
#define FLAG_DEV_INFO_STANDALONE_MODE    0x04

#define INSERT_FIRST ((uint16_t)0xfffb)
#define INSERT_LAST  ((uint16_t)0xfffc)

#define CONNECTED    1
#define DISCONNECTED 2

#define DESCRIPTOR_REQUEST_ELEMENT_PROPERTY 0
#define DESCRIPTOR_REQUEST_ELEMENT_GROUP    1

#define PROPERTY_ELEMENT_FLAG_IS_GROUP_MEMBER 0x01
#define PROPERTY_ELEMENT_FLAG_IS_DISABLED     0x02

#define RGBFLAG_HIDE_ON_OFF_SWITCH         0x01
#define RGBFLAG_HIDE_MODE_SELECTOR_BUTTONS 0x02
#define RGBFLAG_HIDE_INTENSITY_SLIDER      0x04
#define RGBFLAG_HIDE_TRANSITION_SWITCH     0x08

#define EXLEVELFLAGS_HIDE_ON_OFF_SWITCH     0x01
#define EXLEVELFLAGS_ONLY_START_END_OFTRACK 0x02

#define TIMEFRAME_NO_CONSENT  0
#define TIMEFRAME_IS_IN_SCOPE 1

#define UC_LOCKED   1
#define UC_UNLOCKED 2

#define DAY_MODE   0
#define NIGHT_MODE 1

#define UCMODE_UNLOCK_MODE     0
#define UCMODE_PIN_CHANGE_MODE 1

#define UCFLAG_SUCCESS                           0
#define UCFLAG_UNLOCK_FAILED_PIN_REJECTED        0x01
#define UCFLAG_PIN_CHANGE_FAILED_OLD_KEY_INVALID 0x02
#define UCFLAG_PIN_CHANGE_FAILED_NO_ACCESS       0x04
#define UCFLAG_CONNECTED_WITH_SHARED_KEY         0x08
#define UCFLAG_FAIL_UNEXPECTED                   0x10

#define UNLOCKCTRL_LOCK_SUCCESS              0
#define UNLOCKCTRL_LOCK_ERROR                1
#define UNLOCKCTRL_UNEXPECTED                2
#define UNLOCKCTRL_UNLOCK_SUCCESS            3
#define UNLOCKCTRL_UNLOCK_REJECTED_WRONG_PIN 4
#define UNLOCKCTRL_UNLOCK_REJECTED_NO_ACCESS 5
#define UNLOCKCTRL_PIN_CHANGE_FAIL_WRONG_PIN 6
#define UNLOCKCTRL_PIN_CHANGE_FAIL_NO_ACCESS 7
#define UNLOCKCTRL_PIN_CHANGE_SUCCESS 8
#define UNLOCKCTRL_NEW_PIN_INVALID 9

#define NAVIGATOR_ACTIONTYPE_NONE          0
#define NAVIGATOR_ACTIONTYPE_TOUCH_DOWN    1
#define NAVIGATOR_ACTIONTYPE_TOUCH_RELEASE 2

#define NCFLAG_VHIDE_RIGHT_BUTTON 0x01
#define NCFLAG_VHIDE_DOWN_BUTTON  0x02
#define NCFLAG_VHIDE_LEFT_BUTTON  0x04
#define NCFLAG_VHIDE_UP_BUTTON    0x08
#define NCFLAG_VHIDE_MID_BUTTON   0x10

#define NCBFLAG_RIGHT_BUTTON 0x01
#define NCBFLAG_DOWN_BUTTON  0x02
#define NCBFLAG_LEFT_BUTTON  0x04
#define NCBFLAG_UP_BUTTON    0x08
#define NCBFLAG_MID_BUTTON   0x10

#define BGFLAG_USE_VALUE_AS_BAR_DESCRIPTOR 0x01
#define BGFLAG_USE_FIXED_MAXIMUM_VALUE     0x02

#define LGFLAG_DRAW_GRID_LINES  0x01
#define LGFLAG_DRAW_AXIS_VALUES 0x02

#define LGTYPE_OVERRIDE_STATE 0
#define LGTYPE_UPDATE_STATE   1

#define STRIRG_FIELD_VISIBILITY_BOTH           0
#define STRIRG_FIELD_VISIBILITY_ONLY_FIELD_ONE 1
#define STRIRG_FIELD_VISIBILITY_ONLY_FIELD_TWO 2

#define STRIRG_INPUT_TYPE_TEXT            0
#define STRIRG_INPUT_TYPE_TEXT_PASSWORD   1
#define STRIRG_INPUT_TYPE_NUMBER          2
#define STRIRG_INPUT_TYPE_NUMBER_PASSWORD 3

#define STRIRG_FLAG_NAVIGATE_BACK_ON_BUTTON_PRESS 0x01
#define STRIRG_FLAG_ACCEPT_NON_ASCII_CHARACTERS   0x02

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

    // Data holder struct for a device property definition
    typedef struct _DEVICE_PROPERTY
    {
        uint8_t type;
        uint8_t ID;
        uint8_t imageID;
        uint8_t groupID;
        uint8_t flags;
        uint8_t state;
        char *descriptor;

    } DEVICE_PROPERTY, *PDEVICE_PROPERTY;

    // data holder struct for a property group definition
    typedef struct _PROPERTY_GROUP
    {
        uint8_t ID;
        uint8_t imageID;
        char *descriptor;

    } PROPERTY_GROUP, *PPROPERTY_GROUP;

    // the sub-type of a transmission
    enum TransmissionSubType
    {
        TST_NONE,
        TST_REQUEST,
        TST_RESPONSE,
        TST_COMMAND,
        TST_UPDATE,
        TST_INSERT,
        TST_REMOVE,
        TST_ENABLE,
        TST_DISABLE
    };

    // type of the property (used in DEVICE_PROPERTY struct
    enum PropertyType
    {
        PTYPE_INVALID,
        PTYPE_BUTTON,
        PTYPE_SWITCH,
        PTYPE_LEVEL_SELECTOR,
        PTYPE_LEVEL_INDICATOR,
        PTYPE_TEXT_DISPLAY,
        PTYPE_OPTION_SELECTOR,
        PTYPE_RGB_SELECTOR,
        PTYPE_EX_LEVEL_SELECTOR,
        PTYPE_TIME_SELECTOR,
        PTYPE_TIME_FRAME_SELECTOR,
        PTYPE_DATE_SELECTOR,
        PTYPE_UNLOCK_CONTROL,
        PTYPE_NAVIGATOR_CONTROL,
        PTYPE_BAR_GRAPH,
        PTYPE_LINE_GRAPH,
        PTYPE_STRING_INTERROGATOR,
        PTYPE_TEXT_LIST_PRESENTER
    };

    // Event IDs of the property callback
    enum Remote_Event
    {
        REVT_BUTTON_PRESSED,
        REVT_SWITCH_STATE_CHANGED,
        REVT_LEVEL_SELECTOR_VALUE_CHANGED,
        REVT_OPTION_SELECTOR_INDEX_CHANGED,
        REVT_RGB_SELECTOR_STATE_CHANGED,
        REVT_EXLEVEL_SELECTOR_STATE_CHANGED,
        REVT_TIME_SELECTOR_STATE_CHANGED,
        REVT_TIME_FRAME_SELECTOR_STATE_CHANGED,
        REVT_DATE_SELECTOR_STATE_CHANGED,
        REVT_UNLOCK_CONTROL_STATE_CHANGED,
        REVT_UNLOCK_CONTROL_INVALID_OPERATION,
        REVT_NAVIGATOR_CONTROL_STATE_CHANGED,
        REVT_STRING_INTERROGATOR_DATA_RECEIVED,
        REVT_BINDING_TRANSMISSION_RECEIVED,
        REVT_PROPERTY_LOADING_FROM_CACHE_COMPLETE,
        REVT_PROPERTY_LOADING_FROM_DEVICE_COMPLETE,
        REVT_FACTORY_RESET_EXECUTED,
        REVT_DEVICE_CONNECTION_RESTORED,
        REVT_BACK_NAVIGATION,
        REVT_COMPLEX_PROPERTY_PAGE_INVOKED,
        REVT_SETTING_PAGE_INVOKED,
        REVT_TIME_REQUEST_RESPONSE,
        REVT_DATE_REQUEST_RESPONSE,
        REVT_LANGUAGE_REQUEST_RESPONSE,
        REVT_UI_MODE_REQUEST_RESPONSE
    };

    // data holder struct for a RGB-Selector state definition
    typedef struct _RGB_SELECTOR_STATE
    {
        uint8_t associatedPropertyID;
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        uint8_t program;
        uint8_t isOn;
        uint8_t hardTransition;
        uint8_t visibilityFlags;

    } RGB_SELECTOR_STATE, *PRGB_SELECTOR_STATE;

    // Set the reference to the data holder for the RGB-State data of the device-property with the given ID
    void addRGBSelectorStateForProperty(uint8_t propertyID, PRGB_SELECTOR_STATE pState);

    // Get the reference to the RGB-State data holder struct for the given property ID
    PRGB_SELECTOR_STATE getRGBSelectorState(uint8_t propertyID);

    // data holder struct for a Extended-Level-Selector state definition
    typedef struct _EX_LEVEL_SELECTOR_STATE
    {
        uint8_t associatedPropertyID;
        uint8_t isOn;
        int16_t level;
        int16_t min;
        int16_t max;
        uint8_t flags;

    } EX_LEVEL_SELECTOR_STATE, *PEX_LEVEL_SELECTOR_STATE;

    // Set the reference to the data holder for the ExLevel-State data of the device-property with the given ID
    void addExLevelSelectorStateForProperty(uint8_t propertyID, PEX_LEVEL_SELECTOR_STATE pState);

    // Get the reference to the Extended-Level-State data holder struct for the given property ID
    PEX_LEVEL_SELECTOR_STATE getExLevelSelectorState(uint8_t propertyID);

    // data holder struct for a Time-Selector state definition
    typedef struct _TIMESELECTORSTATE
    {
        uint8_t associatedPropertyID;
        uint8_t hour;
        uint8_t minute;

    } TIME_SELECTOR_STATE, *PTIME_SELECTOR_STATE;

    // Set the reference to the data holder for the Time-Selector-State data of the device-property with the given ID
    void addTimeSelectorStateForProperty(uint8_t propertyID, PTIME_SELECTOR_STATE pState);

    // Get the reference to the Time-Selector-State data holder struct for the given property ID
    PTIME_SELECTOR_STATE getTimeSelectorState(uint8_t propertyID);

    // data holder struct for a Time-Frame-Selector state definition
    typedef struct _TIME_FRAME_STATE
    {
        uint8_t associatedPropertyID;
        uint8_t start_hour;
        uint8_t start_minute;
        uint8_t end_hour;
        uint8_t end_minute;

    } TIME_FRAME_STATE, *PTIME_FRAME_STATE;

    // Set the reference to the data holder for the Time-Frame-Selector-State data of the device-property with the given ID
    void addTimeFrameSelectorStateForProperty(uint8_t propertyID, PTIME_FRAME_STATE pState);

    // Get the reference to the Time-Frame-Selector-State data holder struct for the given property ID
    PTIME_FRAME_STATE getTimeFrameSelectorState(uint8_t propertyID);

    // data holder struct for a Date-Selector state definition
    typedef struct _DATE_SELECTOR_STATE
    {
        uint8_t associatedPropertyID;
        uint8_t day;
        uint8_t month;
        uint16_t year;

    } DATE_SELECTOR_STATE, *PDATE_SELECTOR_STATE;

    // Set the reference to the data holder for the Date-Selector-State data of the device-property with the given ID
    void addDateSelectorStateForProperty(uint8_t propertyID, PDATE_SELECTOR_STATE pState);

    // Get the reference to the Date-Selector-State data holder struct for the given property ID
    PDATE_SELECTOR_STATE getDateSelectorState(uint8_t propertyID);

    // data holder struct for a Unlock-Control state definition
    typedef struct _UNLOCK_CONTROL_STATE
    {
        uint8_t associatedPropertyID;
        uint8_t lockState;
        uint8_t mode;
        uint8_t flags;
        char key[11];

    } UNLOCK_CONTROL_STATE, *PUNLOCK_CONTROL_STATE;

    // Set the reference to the data holder for the Unlock-Control-State data of the device-property with the given ID
    void addUnlockControlStateForProperty(uint8_t propertyID, PUNLOCK_CONTROL_STATE pState);

    // Get the reference to the Unlock-Control-State data holder struct for the given property ID
    PUNLOCK_CONTROL_STATE getUnlockControlState(uint8_t propertyID);

    // data holder struct for a Navigator-Control state definition
    typedef struct _NAVIGATOR_STATE
    {
        uint8_t associatedPropertyID;
        uint8_t visibilityFlags;
        uint8_t buttonFlags;
        uint8_t type;

    } NAVIGATOR_STATE, *PNAVIGATOR_STATE;

    // Set the reference to the data holder for the Navigator-Control-State data of the device-property with the given ID
    void addNavigatorControlStateForProperty(uint8_t propertyID, PNAVIGATOR_STATE pState);

    // Get the reference to the Navigator-Control-State data holder struct for the given property ID
    PNAVIGATOR_STATE getNavigatorControlState(uint8_t propertyID);

    // data holder struct for a Bar-Graph state definition
    typedef struct _BAR_GRAPH_STATE
    {
        uint8_t associatedPropertyID;
        uint8_t flags;
        float fixedMaximumValue;
        uint8_t numBars;
        char *data;

    } BAR_GRAPH_STATE, *PBAR_GRAPH_STATE;

    // Set the reference to the data holder for the Bar-Graph-State data of the device-property with the given ID
    void addBarGraphStateForProperty(uint8_t propertyID, PBAR_GRAPH_STATE pState);

    // Get the reference to the Bar-Graph-State data holder struct for the given property ID
    PBAR_GRAPH_STATE getBarGraphState(uint8_t propertyID);

    // data holder struct for a Line-Graph state definition
    typedef struct _LINE_GRAPH_STATE
    {
        uint8_t associatedPropertyID;
        uint8_t flags;
        uint8_t type;
        char *data;

    } LINE_GRAPH_STATE, *PLINE_GRAPH_STATE;

    // Set the reference to the data holder for the Line-Graph-State data of the device-property with the given ID
    void addLineGraphStateForProperty(uint8_t propertyID, PLINE_GRAPH_STATE pState);

    // Get the reference to the Line-Graph-State data holder struct for the given property ID
    PLINE_GRAPH_STATE getLineGraphState(uint8_t propertyID);

    // data holder struct for a String-Interrogator state definition
    typedef struct _STRING_INTERROGATOR_STATE
    {
        uint8_t associatedPropertyID;
        uint8_t fieldVisibilities;
        uint8_t fieldOneInputType;
        uint8_t fieldTwoInputType;
        uint8_t flags;
        char *data;
        char *responseData;

    } STRING_INTERROGATOR_STATE, *PSTRING_INTERROGATOR_STATE;

    // Set the reference to the data holder for the String-Interrogator-State data of the device-property with the given ID
    void addStringInterrogatorStateForProperty(uint8_t propertyID, PSTRING_INTERROGATOR_STATE pState);

    // Get the reference to the String-Interrogator-State data holder struct for the given property ID
    PSTRING_INTERROGATOR_STATE getStringInterrogatorState(uint8_t propertyID);

    // data holder struct for a Text-List-Presenter state definition
    typedef struct _TEXT_LIST_PRESENTER_STATE
    {
        uint8_t associatedPropertyID;
        uint8_t elementType;
        uint8_t action;
        uint8_t useBackgroundStack;
        char *data;

    } TEXT_LIST_PRESENTER_STATE, *PTEXT_LIST_PRESENTER_STATE;

    // Set the reference to the data holder for the Text-List-Presenter-State data of the device-property with the given ID
    void addTextListPresenterStateForProperty(uint8_t propertyID, PTEXT_LIST_PRESENTER_STATE pState);

    // Get the reference to the Text-List-Presenter-State data holder struct for the given property ID
    PTEXT_LIST_PRESENTER_STATE getTextListPresenterState(uint8_t propertyID);

    // Event of a binding transmission - reported through the 'bindingCallback' function which was registered by calling 'registerBindingCallback
    enum BindingCallbackEvent
    {
        BINDING_CB_EVENT_ENABLED,
        BINDING_CB_EVENT_RELEASED,
        BINDING_CB_EVENT_AUTHENTICATION
    };

    // Response to a binding transmission
    enum BindingCallbackResult
    {
        BINDING_CB_RESULT_AUTHENTICATION_SUCCESS,
        BINDING_CB_RESULT_AUTHENTICATION_FAIL,
        BINDING_CB_RESULT_ENABLE_SUCCESS,
        BINDING_CB_RESULT_ENABLE_FAIL,
        BINDING_CB_RESULT_RELEASE_SUCCESS,
        BINDING_CB_RESULT_RELEASE_FAIL
    };

    // Type of a user message - used in the 'sendUserMessage' function
    enum UserMessageType
    {
        USER_MESSAGE_TYPE_INFO,
        USER_MESSAGE_TYPE_WARNING,
        USER_MESSAGE_TYPE_ERROR
    };

    // Display time of a user message - used in the 'sendUserMessage' function
    enum UserMessageDisplayTime
    {
        USER_MESSAGE_DISPLAY_TIME_5SEC,
        USER_MESSAGE_DISPLAY_TIME_7P5_SEC,
        USER_MESSAGE_DISPLAY_TIME_10SEC,
        USER_MESSAGE_DISPLAY_TIME_12P5_SEC,
        USER_MESSAGE_DISPLAY_TIME_15SEC,
        USER_MESSAGE_DISPLAY_TIME_17P5_SEC,
        USER_MESSAGE_DISPLAY_TIME_20SEC,
        USER_MESSAGE_DISPLAY_TIME_INFINITE = 9,
    };

    // Type of a text list presenter element - used in the 'addTextListPresenterElement' function
    enum TextListPresenterElementType
    {
        TEXT_LIST_PRESENTER_ELEMENT_TYPE_NONE,
        TEXT_LIST_PRESENTER_ELEMENT_TYPE_INFO,
        TEXT_LIST_PRESENTER_ELEMENT_TYPE_WARNING,
        TEXT_LIST_PRESENTER_ELEMENT_TYPE_ERROR
    };

    // transmission interface

    // set the current connection status - it is critical to always set the correct connection status as soon as it changes
    // otherwise the api will not work correctly since the transmission logic depends on that information
    void setConnectionState(uint8_t state);

    // when data is received from the gateway, this function must be called to process the data
    // - this is critical for the api to work correctly
    void onDataReceived(const char *data);

    // set the function that is used to send data to the gateway - this is a critical feature and must be set before the api can be used
    void setOutGateway(void (*sendData)(const char *));

    // get the current amount of properties that were added to the api
    uint8_t getPropertyCount();

    // get the current amount of groups that were added to the api
    uint8_t getGroupCount();

    // get the amount of members added to a specific group
    uint8_t getGroupMemberCount(uint8_t groupID);

    // remove all properties from the api
    void clearAllProperties();

    // remove all groups from the api
    void clearAllGroups();

    // use this function to init all members of a property struct
    void initDevicePropertyStruct(PDEVICE_PROPERTY pProperty);

    // use this function to init all members of a group struct
    void initGroupStruct(PPROPERTY_GROUP pGroup);

    // use this function to clear all members of a property struct
    void clearDevicePropertyStruct(PDEVICE_PROPERTY pProperty);

    // use this function to clear all members of a group struct
    void clearGroupStruct(PPROPERTY_GROUP pGroup);

    // add a property to the api
    void addDeviceProperty(PDEVICE_PROPERTY pProperty);

    // update an existing property
    void updateDeviceProperty(PDEVICE_PROPERTY pProperty);

    // insert a property at a specific position. The element will be added after the element with the given insert-after ID
    // to insert at the beginning of the list, use INSERT_FIRST as insert-after ID or INSERT_LAST to insert at the end of the list
    void insertDevicePropertyAfter(uint16_t insertAfter, PDEVICE_PROPERTY pProperty);

    // remove a property from the api
    bool removeDeviceProperty(uint8_t ID);

    // add a device property group to the api
    void addPropertyGroup(PPROPERTY_GROUP pGroup);

    // remove a device property group from the api
    bool removePropertyGroup(uint8_t ID);

    // add a property to the group with the given groupID
    void addPropertyToGroup(PDEVICE_PROPERTY pProperty, uint8_t groupID);

    // enable a property - this will enable the property on the device (the user can interact with it)
    void enableProperty(uint8_t propertyID);

    // disable a property - this will disable the property on the device (the user can not interact with it and the element will be grayed out)
    void disableProperty(uint8_t propertyID);

    // get the reference to the property element with the given ID (will return NULL if the property does not exist)
    PDEVICE_PROPERTY getDevicePropertyFromID(uint8_t ID);

    // get the property ID of the property element at the given index
    uint8_t getPropertyIDAtIndex(uint8_t propertyIndex);

    // get the group ID of the group element at the given index
    uint8_t getGroupIDAtIndex(uint8_t groupIndex);

    // get the property index of the property with the given ID
    uint8_t getPropertyIndexForID(uint8_t propertyID);

    // get the reference to the group element with the given ID (will return NULL if the group does not exist)
    PPROPERTY_GROUP getPropertyGroupFromID(uint8_t ID);

    // send a time request to the remote device - the response will be reported through the respective callback event
    void sendTimeRequest();

    // send a date request to the remote device - the response will be reported through the respective callback event
    void sendDateRequest();

    // send a property reload command to the remote device - this will cause the device to reload all properties by starting the property loop
    void sendPropertyReloadCommand();

    // send a property to cache command to the remote device - this will cause the device to save the current properties to cache
    void sendPropertyToCacheCommand();

    // send a language request to the remote device - the response will be reported through the respective callback event
    void sendLanguageRequest();

    // send a refresh all states command to the remote device - this will cause the device to refresh all property states by starting the property state loop
    void sendRefreshAllStatesCommand();

    // send a navigate back command to the remote device - this will force the device to navigate back to the device main page
    void sendNavBackToDeviceMainCommand();

    // send a close device command to the remote device - this will force the device to close the current connection
    void sendCloseDeviceCommand();

    // send a UI mode info request to the remote device - the response will be reported through the respective callback event
    void sendUIModeInfoRequest();

    // send a user message to the remote device - this will cause the device to display the message
    void sendUserMessage(const char *message, uint8_t type, uint8_t displayTime);

    // add an element to the text list presenter with the given ID
    void addTextListPresenterElement(uint8_t textListPresenterID, const char *text, uint8_t elementType, uint8_t useBackgroundStack);

    // clear all elements in the text list presenter with the given ID
    void clearTextListPresenterElements(uint8_t textListPresenterID);

    // when a time response is received through the remote callback, this function can be used to fetch the data from the response
    void fetchTimeResponseData(uint8_t *hours, uint8_t *minutes, uint8_t *seconds);

    // when a date response is received through the remote callback, this function can be used to fetch the data from the response
    void fetchDateResponseData(uint16_t *year, uint8_t *month, uint8_t *day);

    // when a ui mode info response is received through the remote callback, this function can be used to fetch the data from the response
    void fetchUIModeInfoResponseData(uint8_t *mode);

    // set a descriptor to the descriptor field of the given property struct - this is a helper function which handles memory allocation (pseudo-)dynamically
    void setPropertyDescriptor(PDEVICE_PROPERTY pProperty, const char *descriptor);

    // set a descriptor to the descriptor field of the given group struct - this is a helper function which handles memory allocation (pseudo-)dynamically
    void setGroupDescriptor(PPROPERTY_GROUP pGroup, const char *descriptor);

    // set the permission if property caching is permitted or not
    void setCachingPermission(bool enable);

    // set the stand alone mode - this will only work in conjunction with one single device property from complex type
    void setStandAloneMode(bool enable);

    // set if binding is required or not - if so, the binding callback must be processed to implement the binding functionality
    void setBindingRequired(bool enable);

    // register the remote callback function - this function will be called when a remote request or command is received
    void registerRemoteCallback(void (*remoteCallback)(uint8_t, uint8_t));

    // register the descriptor callback function - this function will be called when a descriptor definition is required - if this callback is registered, the descriptor field of the property or group struct will be ignored
    void registerDescriptorCallback(const char *(*descriptorCallback)(uint8_t, uint8_t, const char *));

    // register the binding callback function - this function will be called when a binding transmission is received
    void registerBindingCallback(uint8_t (*bindingCallback)(uint8_t, const char *));

    // get the state of a simple property with the given ID
    uint8_t getSimplePropertyState(uint8_t propertyID);

    // update the state of a simple property with the given ID
    void updateSimplePropertyState(uint8_t propertyID, uint8_t state);

    // check if the given time is inside the given time frame
    uint8_t isTimeInFrame(PTIME_FRAME_STATE timeFrame, uint8_t curTimeHour, uint8_t curTimeMinute);

    // get the current session language - the language which the connected app is currently using - can be NULL if no language is set
    const char *getLanguage();

    // convert a unsigned 8 bit single digit value to a hexadecimal character
    char intValToHexChar(uint8_t val);

    // convert a hexadecimal character to a unsigned 8 bit value
    uint8_t hexCharToIntVal(char h);

    // convert a unsigned 8 bit value to a hexadecimal string with two characters
    // the buffer must be at least [2] fields minimum
    void x8BitValueToHexTwoCharBuffer(uint8_t toConvert, char *buffer_out);

    // convert a signed 16 bit value to a hexadecimal string with four characters
    // the buffer must have at least [4] fields minimum
    void xSigned16BitValueToHex4CharBuffer(int16_t toConvert, char *buffer_out);

    // convert a unsigned 16 bit value to a hexadecimal string with four characters
    // the buffer must have at least [4] fields minimum
    void x16BitValueToHex4CharBuffer(uint16_t toConvert, char *buffer_out);

    // convert a two character string identifying a hexadecimal value to a unsigned 8 bit value
    uint8_t x2CharHexValueTo8BitValue(char left, char right);

    // convert a four character string identifying a hexadecimal value to a signed 16 bit value
    int16_t x4CharHexValueToSigned16BitValue(char hl, char hr, char ll, char lr);

    // convert a four character string identifying a hexadecimal value to a unsigned 16 bit value
    uint16_t x4CharHexValueTo16BitValue(char hl, char hr, char ll, char lr);
    
    // get the last operation code from an invalid unlock control operation
    uint8_t getLastInvalidUnlockControlOperation();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // !LAROOMY_API_H
