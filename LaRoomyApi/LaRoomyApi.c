#include "LaRoomyApi.h"

#include <avr/io.h>

typedef struct _gnrResponseData
{
	uint8_t a;
	uint8_t b;
	uint8_t c;
	uint16_t d;

}gnrRespData, *pGnrRespData;

void insertDevicePropertyAt(uint8_t index, PDEVICE_PROPERTY pProperty);
uint8_t propertyIndexFromID(uint8_t ID);
PDEVICE_PROPERTY findProperty(uint8_t ID, uint8_t* index_out);
void sendData(const char* data);
void removeComplexStateForPropertyID(uint8_t propertyID, uint8_t type);
PDEVICE_PROPERTY getDevicePropertyFromIndex(uint8_t index);
PPROPERTY_GROUP getPropertyGroupFromIndex(uint8_t index);
char* getPropertyDefinitionString(uint8_t index, uint8_t subType, const char* langID);
char* getGroupDefinitionString(uint8_t index, uint8_t subType, const char* langID);
char* getInitString();
char* stringToPtr(const char* string);

// The buffer must have 22 fields minimum
void rgbStateToStateResponseString(PRGB_SELECTOR_STATE pRGBState, char* response_string_out);
void executionStringToRGBStateStruct(const char* data, PRGB_SELECTOR_STATE pState);
bool removeRGBSelectorStateForProperty(uint8_t propertyID);

// the buffer must have 25 fields minimum
void exLevelStateToStateResponseString(PEX_LEVEL_SELECTOR_STATE pState, char* response_string_out);
void executionStringToExLevelStruct(const char* data, PEX_LEVEL_SELECTOR_STATE pState);
bool removeExLevelSelectorStateForProperty(uint8_t propertyID);

// the buffer must have 14 fields minimum
void timeSelectorStateToStateResponseString(PTIME_SELECTOR_STATE pState, char* response_string_out);
void executionStringToTimeSelectorStruct(const char* data, PTIME_SELECTOR_STATE pState);
bool removeTimeSelectorStateForProperty(uint8_t propertyID);

// the buffer must have 18 fields minimum
void timeFrameSelectorStateToStateResponseString(PTIME_FRAME_STATE pState, char* response_string_out);
void executionStringToTimeFrameSelectorStruct(const char* data, PTIME_FRAME_STATE pState);
bool removeTimeFrameSelectorStateForProperty(uint8_t propertyID);

// the buffer must have 18 fields minimum
void dateSelectorStateToStateResponseString(PDATE_SELECTOR_STATE pState, char* response_string_out);
void executionStringToDateSelectorStruct(const char* data, PDATE_SELECTOR_STATE pState);
bool removeDateSelectorStateForProperty(uint8_t propertyID);

// the buffer must have 14 fields minimum
void unlockControlStateToStateResponseString(PUNLOCK_CONTROL_STATE pState, char* response_string_out);
uint8_t executionStringToUnlockControlStruct(const char* data, PUNLOCK_CONTROL_STATE pState);
bool removeUnlockControlStateForProperty(uint8_t propertyID);

// the buffer must have 16 fields minimum
void navigatorControlStateToStateResponseString(PNAVIGATOR_STATE pState, char* response_string_out);
void executionStringToNavigatorControlStruct(const char* data, PNAVIGATOR_STATE pState);
bool removeNavigatorControlStateForProperty(uint8_t propertyID);

// the buffer is an out parameter, it is allocated inside of the function
void barGraphStateToStateResponseString(PBAR_GRAPH_STATE pState, char** response_string_out);
bool removeBarGraphStateForProperty(uint8_t propertyID);

// the buffer is an out parameter, it is allocated inside of the function
void lineGraphStateToStateResponseString(PLINE_GRAPH_STATE pState, char** response_string_out);
bool removeLineGraphStateForProperty(uint8_t propertyID);

// the buffer is an out parameter, it is allocated inside of the function
void stringInterrogatorStateToStateResponseString(PSTRING_INTERROGATOR_STATE pState, char** response_string_out);
void executionStringToStringInterrogatorStruct(const char* data, PSTRING_INTERROGATOR_STATE pState);
bool removeStringInterrogatorStateForProperty(uint8_t propertyID);

// the buffer is an out parameter, it is allocated inside of the function
void textListPresenterStateToStateResponseString(PTEXT_LIST_PRESENTER_STATE pState, char** response_string_out);
bool removeTextListPresenterStateForProperty(uint8_t propertyID);

void onInitRequest();
void onPropertyRequest(const char* data, uint8_t length);
void onGroupRequest(const char* data, uint8_t length);
void onPropertyStateRequest(const char* data, uint8_t length);
void onExecutionCommand(const char* data, uint8_t length);
void onNotificationTransmission(const char* data, uint8_t length);
void onBindingTransmission(const char* data, uint8_t length);

void onRGBStateRequest(uint8_t propertyID);
void onExLevelStateRequest(uint8_t propertyID);
void onTimeSelectorStateRequest(uint8_t propertyID);
void onTimeFrameSelStateRequest(uint8_t propertyID);
void onDateSelectorStateRequest(uint8_t propertyID);
void onUnlockControlStateRequest(uint8_t propertyID);
void onNavigatorControlStateRequest(uint8_t propertyID);
void onBarGraphStateRequest(uint8_t propertyID);
void onLineGraphStateRequest(uint8_t propertyID);
void onStringInterrogatorStateRequest(uint8_t propertyID);
void onTextListPresenterStateRequest(uint8_t propertyID);

void sendPropertyRemovedNotification(uint8_t propertyID);



PDEVICE_PROPERTY* propertyList = NULL;
uint8_t propertyCount = 0;
PPROPERTY_GROUP* groupList = NULL;
uint8_t groupCount = 0;
uint8_t deviceInfoFlags = 0;
char languageIdentifier[3] = { 0 };
uint8_t connection_state = DISCONNECTED;
void (*rmtCallback)(uint8_t propertyID, uint8_t eventID) = NULL;
void (*outGateway)(const char*) = NULL;
const char* (*descriptCallback)(uint8_t elementType, uint8_t ID, const char* langID) = NULL;
uint8_t (*bndgCallback)(uint8_t eventID, const char* key) = NULL;
gnrRespData responseData = { 0 };

uint8_t rgbSelectorStateCount = 0;
PRGB_SELECTOR_STATE* rgbSelectorStateList = NULL;

uint8_t exLevelSelectorStateCount = 0;
PEX_LEVEL_SELECTOR_STATE* exLevelSelectorStateList = NULL;

uint8_t timeSelectorStateCount = 0;
PTIME_SELECTOR_STATE* timeSelectorStateList = NULL;

uint8_t timeFrameSelectorStateCount = 0;
PTIME_FRAME_STATE* timeFrameSelectorStateList = NULL;

uint8_t dateSelectorStateCount = 0;
PDATE_SELECTOR_STATE* dateSelectorStateList = NULL;

uint8_t unlockControlStateCount = 0;
PUNLOCK_CONTROL_STATE* unlockControlStateList = NULL;

uint8_t navigatorControlStateCount = 0;
PNAVIGATOR_STATE* navigatorControlStateList = NULL;

uint8_t barGraphStateCount = 0;
PBAR_GRAPH_STATE* barGraphStateList = NULL;

uint8_t lineGraphStateCount = 0;
PLINE_GRAPH_STATE* lineGraphStateList = NULL;

uint8_t stringInterrogatorStateCount = 0;
PSTRING_INTERROGATOR_STATE* stringInterrogatorStateList = NULL;

uint8_t textListPresenterStateCount = 0;
PTEXT_LIST_PRESENTER_STATE* textListPresenterStateList = NULL;

void setConnectionState(uint8_t state)
{
	connection_state = state;
}

void onDataReceived(const char* data)
{
	if (data != NULL)
	{
		uint8_t length = (uint8_t)strlen(data);
		if (length >= 8)
		{
			switch (data[0])
			{
			case '1':
				// property (request)
				onPropertyRequest(data, length);
				break;
			case '2':
				// group (request)
				onGroupRequest(data, length);
				break;
			case '3':
				// property state (request)
				onPropertyStateRequest(data, length);
				break;
			case '4':
				// execution command
				onExecutionCommand(data, length);
				break;
			case '5':
				// notification or command
				onNotificationTransmission(data, length);
				break;
			case '6':
				// binding transmission
				onBindingTransmission(data, length);
				break;
			case '7':
				// init transmission
				onInitRequest();
				break;
			default:
				break;
			}
		}
	}
}

void setOutGateway(void (*sendData)(const char* data))
{
	outGateway = sendData;
}

uint8_t getPropertyCount()
{
	return propertyCount;
}

uint8_t getGroupCount()
{
	return groupCount;
}

uint8_t getGroupMemberCount(uint8_t groupID)
{
	uint8_t mCount = 0;
	for (uint8_t i = 0; i < propertyCount; i++)
	{
		if (propertyList[i]->groupID == groupID)
		{
			mCount++;
		}
	}
	return mCount;
}

void clearAllProperties()
{
	if (propertyList != NULL)
	{
		free(propertyList);
		propertyList = NULL;
	}
	propertyCount = 0;
}

void clearAllGroups()
{
	if (groupList != NULL)
	{
		free(groupList);
		groupList = NULL;
	}
	groupCount = 0;

	// reset all groupID params of the properties
	for (uint8_t i = 0; i < propertyCount; i++)
	{
		propertyList[i]->groupID = 0;
		propertyList[i]->flags &= ~PROPERTY_ELEMENT_FLAG_IS_GROUP_MEMBER;
	}
}

void initDevicePropertyStruct(PDEVICE_PROPERTY pProperty)
{
	if (pProperty != NULL)
	{
		pProperty->descriptor = NULL;		
		pProperty->flags = 0;
		pProperty->groupID = 0;
		pProperty->ID = 0;
		pProperty->imageID = 0;
		pProperty->state = 0;
		pProperty->type = 0;
	}
}

void initGroupStruct(PPROPERTY_GROUP pGroup)
{
	if (pGroup != NULL)
	{
		pGroup->descriptor = NULL;
		pGroup->ID = 0;
		pGroup->imageID = 0;
	}
}

void clearDevicePropertyStruct(PDEVICE_PROPERTY pProperty)
{
	if (pProperty != NULL)
	{
		if (pProperty->descriptor != NULL)
		{
			free(pProperty->descriptor);
			pProperty->descriptor = NULL;
		}
		pProperty->flags = 0;
		pProperty->groupID = 0;
		pProperty->ID = 0;
		pProperty->imageID = 0;
		pProperty->state = 0;
		pProperty->type = 0;
	}
}

void clearGroupStruct(PPROPERTY_GROUP pGroup)
{
	if (pGroup != NULL)
	{
		if (pGroup->descriptor != NULL)
		{
			free(pGroup->descriptor);
			pGroup->descriptor = NULL;
		}
		pGroup->ID = 0;
		pGroup->imageID = 0;
	}
}

void addDeviceProperty(PDEVICE_PROPERTY pProperty)
{
	if (propertyCount == 0)
	{
		propertyList = (PDEVICE_PROPERTY*)malloc(sizeof(PDEVICE_PROPERTY));
		if (propertyList != NULL)
		{
			propertyList[0] = pProperty;
			propertyCount++;

			if (connection_state == CONNECTED)
			{
				char* propString = getPropertyDefinitionString(0, TST_INSERT, languageIdentifier);
				if(propString != NULL)
				{
					sendData(propString);
					free(propString);
				}
			}
		}
	}
	else
	{
		if (propertyCount < 255)
		{
			PDEVICE_PROPERTY* pHolder = propertyList;

			uint8_t newPCount = propertyCount + 1;
			size_t len = newPCount;
			len *= sizeof(PDEVICE_PROPERTY);

			propertyList = (PDEVICE_PROPERTY*)malloc(len);

			if (propertyList != NULL)
			{
				memset(propertyList, 0, len);

				for (uint8_t i = 0; i < propertyCount; i++)
				{
					propertyList[i] = pHolder[i];
				}
				propertyList[propertyCount] = pProperty;
				propertyCount++;

				if (connection_state == CONNECTED)
				{
					char* propString = getPropertyDefinitionString(propertyCount - 1, TST_INSERT, languageIdentifier);
					if (propString != NULL)
					{
						sendData(propString);
						free(propString);
					}
				}
			}
			free(pHolder);
		}
	}
}

void updateDeviceProperty(PDEVICE_PROPERTY pProperty)
{
	if (pProperty != NULL && connection_state == CONNECTED)
	{
		char* pString = getPropertyDefinitionString(
			propertyIndexFromID(pProperty->ID),
			TST_UPDATE,
			languageIdentifier
		);
		if (pString != NULL)
		{
			sendData(pString);
			free(pString);
		}
	}
}

void insertDevicePropertyAt(uint8_t index, PDEVICE_PROPERTY pProperty)
{
	if (pProperty != NULL)
	{
		if (index >= propertyCount)
		{
			addDeviceProperty(pProperty);
		}
		else
		{
			if (propertyCount < 255)
			{
				PDEVICE_PROPERTY* pHolder = propertyList;

				propertyCount++;

				size_t len = propertyCount;
				len *= sizeof(PDEVICE_PROPERTY);

				propertyList = (PDEVICE_PROPERTY*)malloc(len);

				if (propertyList != NULL)
				{
					memset(propertyList, 0, len);

					uint8_t dstIndex = 0;

					for (uint8_t i = 0; i < propertyCount; i++)
					{
						if (i == index)
						{
							propertyList[i] = pProperty;
						}
						else
						{
							propertyList[i] = pHolder[dstIndex];
							dstIndex++;
						}
					}
					
					if (connection_state == CONNECTED)
					{
						// NOTE: when inserting a property, the remote side will send a complex state request immediately after reception (if the prop is from complex type)
						// -> but the state isn't added here, it must be added immediately after the property was inserted!

						char* pString = getPropertyDefinitionString(index, TST_INSERT, languageIdentifier);
						if (pString != NULL)
						{
							sendData(pString);
							free(pString);
						}
					}
				}
				free(pHolder);
			}
		}
	}
}

void insertDevicePropertyAfter(uint16_t insertAfter, PDEVICE_PROPERTY pProperty)
{
	if (pProperty != NULL)
	{
		if (insertAfter == INSERT_FIRST)
		{
			insertDevicePropertyAt(0, pProperty);
		}
		else if (insertAfter == INSERT_LAST)
		{
			insertDevicePropertyAt(propertyCount, pProperty);
		}
		else
		{
			uint8_t insertIndex = 0;
			for (uint8_t i = 0; i < propertyCount; i++)
			{
				if (propertyList[i]->ID == insertAfter)
				{
					insertIndex = i + 1;
					break;
				}
			}
			insertDevicePropertyAt(insertIndex, pProperty);
		}
	}
}

void sendPropertyRemovedNotification(uint8_t propertyID)
{
	if (connection_state == CONNECTED)
	{
		char data[] = "16xx0000\r\0";
		char twoChars[2];
		x8BitValueToHexTwoCharBuffer(propertyIndexFromID(propertyID), twoChars);
		data[2] = twoChars[0];
		data[3] = twoChars[1];
		sendData(data);
	}
}

bool removeDeviceProperty(uint8_t ID)
{
	for (uint8_t i = 0; i < propertyCount; i++)
	{
		if (ID == propertyList[i]->ID)
		{
			uint8_t type = propertyList[i]->type;

			// if the property was part of a group check if this was the last property
			// in the group and delete the group if so
			if (propertyList[i]->groupID != 0)
			{
				uint8_t mCount = getGroupMemberCount(propertyList[i]->groupID);
				if (mCount <= 1)
				{
					removePropertyGroup(propertyList[i]->groupID);
				}
			}

			if (propertyCount == 1)
			{
				free(propertyList);
				propertyCount = 0;

				sendPropertyRemovedNotification(ID);
				removeComplexStateForPropertyID(ID, type);
			}
			else
			{
				PDEVICE_PROPERTY* pHolder = propertyList;
				uint8_t newPCount = propertyCount - 1;
				size_t len = newPCount;
				len *= sizeof(PDEVICE_PROPERTY);

				propertyList = (PDEVICE_PROPERTY*)malloc(len);

				if (propertyList != NULL)
				{
					memset(propertyList, 0, len);
					uint8_t targetCounter = 0;

					for (uint8_t i = 0; i < propertyCount; i++)
					{
						if (pHolder[i]->ID != ID)
						{
							propertyList[targetCounter] = pHolder[i];
							targetCounter++;
						}
					}
					propertyCount--;

					sendPropertyRemovedNotification(ID);
					removeComplexStateForPropertyID(ID, type);
				}
			}
			return true;
		}
	}	
	return false;
}

PDEVICE_PROPERTY getDevicePropertyFromID(uint8_t ID)
{
	for (uint8_t i = 0; i < propertyCount; i++)
	{
		if (ID == propertyList[i]->ID)
		{
			return propertyList[i];
		}
	}
	return NULL;
}

PDEVICE_PROPERTY getDevicePropertyFromIndex(uint8_t index)
{
	if (index < propertyCount)
	{
		return propertyList[index];
	}
	else
	{
		return NULL;
	}
}

uint8_t getPropertyIDAtIndex(uint8_t propertyIndex)
{
	if (propertyIndex < propertyCount && propertyList != NULL)
	{
		return propertyList[propertyIndex]->ID;
	}
	return 0;
}

uint8_t getGroupIDAtIndex(uint8_t groupIndex)
{
	if (groupIndex < groupCount && groupList != NULL)
	{
		return groupList[groupIndex]->ID;
	}
	return 0;
}

uint8_t getPropertyIndexForID(uint8_t propertyID)
{
	if (propertyCount > 0 && propertyList != NULL)
	{
		for (uint8_t i = 0; i < propertyCount; i++)
		{
			if (propertyID == propertyList[i]->ID)
			{
				return i;
			}
		}
	}
	return 0;
}

PPROPERTY_GROUP getPropertyGroupFromID(uint8_t ID)
{
	for (uint8_t i = 0; i < groupCount; i++)
	{
		if (ID == groupList[i]->ID)
		{
			return groupList[i];
		}
	}
	return NULL;
}

PPROPERTY_GROUP getPropertyGroupFromIndex(uint8_t index)
{
	if (index < groupCount)
	{
		return groupList[index];
	}
	else
	{
		return NULL;
	}
}

void addPropertyGroup(PPROPERTY_GROUP pGroup)
{
	if (groupCount == 0)
	{
		groupList = (PPROPERTY_GROUP*)malloc(sizeof(PPROPERTY_GROUP));
		if (groupList != NULL)
		{
			groupList[0] = pGroup;
			groupCount++;
			sendPropertyReloadCommand();
		}
	}
	else
	{
		if (groupCount < 255)
		{
			PPROPERTY_GROUP* pHolder = groupList;

			uint8_t newGCount = groupCount + 1;
			size_t len = newGCount;
			len *= sizeof(PPROPERTY_GROUP);

			groupList = (PPROPERTY_GROUP*)malloc(len);

			if (groupList != NULL)
			{
				memset(groupList, 0, len);

				for (uint8_t i = 0; i < groupCount; i++)
				{
					groupList[i] = pHolder[i];
				}
				groupList[groupCount] = pGroup;
				groupCount++;
				sendPropertyReloadCommand();
			}
			free(pHolder);
		}
	}
}

bool removePropertyGroup(uint8_t ID)
{
	for (uint8_t i = 0; i < groupCount; i++)
	{
		if (ID == groupList[i]->ID)
		{
			if (groupCount == 1)
			{
				free(groupList);
				groupCount = 0;

				sendPropertyReloadCommand();
			}
			else
			{
				PPROPERTY_GROUP* pHolder = groupList;

				uint8_t newGCount = groupCount - 1;
				size_t len = newGCount;
				len *= sizeof(PPROPERTY_GROUP);

				groupList = (PPROPERTY_GROUP*)malloc(len);

				if (groupList != NULL)
				{
					memset(groupList, 0, len);
					uint8_t targetCounter = 0;

					for (uint8_t i = 0; i < groupCount; i++)
					{
						if (pHolder[i]->ID != ID)
						{
							groupList[targetCounter] = pHolder[i];
							targetCounter++;
						}
					}
					groupCount--;

					sendPropertyReloadCommand();
				}
			}
			// if there are properties which are currently part of the
			// removed group, the groupID params of the property(s) must be reset
			for (uint8_t i = 0; i < propertyCount; i++)
			{
				if (propertyList[i]->groupID == ID)
				{
					propertyList[i]->groupID = 0;
					propertyList[i]->flags &= ~PROPERTY_ELEMENT_FLAG_IS_GROUP_MEMBER;
				}
			}
			return true;
		}
	}
	return false;
}

void addPropertyToGroup(PDEVICE_PROPERTY pProperty, uint8_t groupID)
{
	bool groupExist = false;

	for (uint8_t i = 0; i < groupCount; i++)
	{
		if (groupList[i]->ID == groupID)
		{
			groupExist = true;
			break;
		}
	}
	if (groupExist && pProperty != NULL)
	{
		// search insert point
		uint8_t mark = 0;
		uint8_t insertIndex = 255;

		for (uint8_t i = 0; i < propertyCount; i++)
		{
			if (mark == 0 && propertyList[i]->groupID == groupID)
			{
				mark = 1;
			}
			if (mark == 1 && propertyList[i]->groupID != groupID)
			{
				insertIndex = i;
				break;
			}
		}
		pProperty->groupID = groupID;
		pProperty->flags |= PROPERTY_ELEMENT_FLAG_IS_GROUP_MEMBER;
		insertDevicePropertyAt(insertIndex, pProperty);
	}
}

void enableProperty(uint8_t propertyID)
{
	uint8_t index;

	PDEVICE_PROPERTY prop = findProperty(propertyID, &index);
	if (prop != NULL)
	{
		prop->flags &= ~PROPERTY_ELEMENT_FLAG_IS_DISABLED;
		if (connection_state == CONNECTED)
		{
			char* pString = getPropertyDefinitionString(index, TST_ENABLE, languageIdentifier);
			if (pString != NULL)
			{
				sendData(pString);
			}
		}
	}
}

void disableProperty(uint8_t propertyID)
{
	uint8_t index;

	PDEVICE_PROPERTY prop = findProperty(propertyID, &index);
	if (prop != NULL)
	{
		prop->flags |= PROPERTY_ELEMENT_FLAG_IS_DISABLED;
		if (connection_state == CONNECTED)
		{
			char* pString = getPropertyDefinitionString(index, TST_DISABLE, languageIdentifier);
			if (pString != NULL)
			{
				sendData(pString);
			}
		}
	}
}

void setPropertyDescriptor(PDEVICE_PROPERTY pProperty, const char* descriptor)
{
	if (pProperty != NULL && descriptor != NULL)
	{
		if (pProperty->descriptor != NULL)
		{
			free(pProperty->descriptor);
			pProperty->descriptor = NULL;
		}
		size_t len = strlen(descriptor);
		len += sizeof(char);
		pProperty->descriptor = (char*)malloc(len);
		if (pProperty->descriptor != NULL)
		{
			memset(pProperty->descriptor, 0, len);
			strcpy(pProperty->descriptor, descriptor);
		}
	}
}

void setGroupDescriptor(PPROPERTY_GROUP pGroup, const char* descriptor)
{
	if (pGroup != NULL && descriptor != NULL)
	{
		if (pGroup->descriptor != NULL)
		{
			free(pGroup->descriptor);
			pGroup->descriptor = NULL;
		}
		size_t len = strlen(descriptor);
		len += sizeof(char);
		pGroup->descriptor = (char*)malloc(len);
		if (pGroup->descriptor != NULL)
		{
			memset(pGroup->descriptor, 0, len);
			strcpy(pGroup->descriptor, descriptor);
		}
	}
}

void setCachingPermission(bool enable)
{
	if (enable)
	{
		deviceInfoFlags |= FLAG_DEV_INFO_CACHING_PERMISSION;
	}
	else
	{
		deviceInfoFlags &= ~FLAG_DEV_INFO_CACHING_PERMISSION;
	}
}

void setStandAloneMode(bool enable)
{
	if (enable)
	{
		deviceInfoFlags |= FLAG_DEV_INFO_STANDALONE_MODE;
	}
	else
	{
		deviceInfoFlags &= ~FLAG_DEV_INFO_STANDALONE_MODE;
	}
}

void setBindingRequired(bool enable)
{
	if (enable)
	{
		deviceInfoFlags |= FLAG_DEV_INFO_BINDING_REQUIRED;
	}
	else
	{
		deviceInfoFlags &= ~FLAG_DEV_INFO_BINDING_REQUIRED;
	}
}

void registerRemoteCallback(void (*remoteCallback)(uint8_t propertyID, uint8_t eventID))
{
	rmtCallback = remoteCallback;
}

void registerDescriptorCallback(const char* (*descriptorCallback)(uint8_t elementType, uint8_t ID, const char* langID))
{
	descriptCallback = descriptorCallback;
}

void registerBindingCallback(uint8_t(*bindingCallback)(uint8_t eventID, const char* key))
{
	bndgCallback = bindingCallback;
}



uint8_t getSimplePropertyState(uint8_t propertyID)
{
	PDEVICE_PROPERTY prop = findProperty(propertyID, NULL);
	if (prop != NULL)
	{
		return prop->state;
	}
	return 0;
}

void updateSimplePropertyState(uint8_t propertyID, uint8_t state)
{
	uint8_t index;

	PDEVICE_PROPERTY prop = findProperty(propertyID, &index);
	if (prop != NULL)
	{
		prop->state = state;

		if (connection_state == CONNECTED)
		{
			char stateString[12] = { 0 };
			char twoC[2];

			stateString[0] = '3';						// state transmission
			stateString[1] = '4';						// update sub type

			x8BitValueToHexTwoCharBuffer(index, twoC);
			stateString[2] = twoC[0];					// property index
			stateString[3] = twoC[1];					// property index

			stateString[4] = '0';						// dataSize
			stateString[5] = '3';						// dataSize

			stateString[6] = '0';						// error flag
			stateString[7] = '0';						// status flag

			x8BitValueToHexTwoCharBuffer(state, twoC);
			stateString[8] = twoC[0];					// state
			stateString[9] = twoC[1];					// state

			stateString[10] = '\r';						// delimiter
			stateString[11] = '\0';						// terminator

			sendData(stateString);
		}
	}
}

char numToChar(uint8_t num)
{
	switch (num)
	{
	case 0:
		return '0';
	case 1:
		return '1';
	case 2:
		return '2';
	case 3:
		return '3';
	case 4:
		return '4';
	case 5:
		return '5';
	case 6:
		return '6';
	case 7:
		return '7';
	case 8:
		return '8';
	case 9:
		return '9';
	default:
		return 'X';
	}
}

char* getPropertyDefinitionString(uint8_t index, uint8_t subType, const char* langID)
{
	if (index < propertyCount)
	{
		PDEVICE_PROPERTY prop = getDevicePropertyFromIndex(index);
		if (prop != NULL)
		{
			const char* desc;

			if (descriptCallback != NULL)
			{
				desc = descriptCallback(DESCRIPTOR_REQUEST_ELEMENT_PROPERTY, getPropertyIDAtIndex(index), langID);
			}
			else
			{
				desc = prop->descriptor;
			}

			size_t len = 20;
			if (desc != NULL)
			{
				len += strlen(desc);

				char* defString = (char*)malloc(len);
				if (defString != NULL)
				{
					memset(defString, 0, len);

					char twoC[2];

					defString[0] = '1';						// property data transmission
					defString[1] = numToChar(subType);		// transmission sub type

					x8BitValueToHexTwoCharBuffer(index, twoC);
					defString[2] = twoC[0];					// property index
					defString[3] = twoC[1];					// property index

					if (len > 255)
					{
						twoC[0] = 'f';
						twoC[1] = 'f';
					}
					else
					{
						x8BitValueToHexTwoCharBuffer((uint8_t)len, twoC);
					}
					defString[4] = twoC[0];					// data size
					defString[5] = twoC[1];					// data size
					defString[6] = '0';						// error flag
					defString[7] = (len > 255) ? '5' : '0';	// status flag

					x8BitValueToHexTwoCharBuffer(prop->type, twoC);
					defString[8] = twoC[0];					// property type
					defString[9] = twoC[1];					// property type

					x8BitValueToHexTwoCharBuffer(prop->imageID, twoC);
					defString[10] = twoC[0];				// image ID
					defString[11] = twoC[1];				// image ID

					x8BitValueToHexTwoCharBuffer(prop->groupID, twoC);
					defString[12] = twoC[0];				// group ID
					defString[13] = twoC[1];				// group ID

					x8BitValueToHexTwoCharBuffer(prop->flags, twoC);
					defString[14] = twoC[0];				// flags
					defString[15] = twoC[1];				// flags

					x8BitValueToHexTwoCharBuffer(prop->state, twoC);
					defString[16] = twoC[0];				// state
					defString[17] = twoC[1];				// state

					uint8_t defCounter = 18;
					uint8_t sourceCounter = 0;

					while (desc[sourceCounter] != '\0')
					{
						defString[defCounter] = desc[sourceCounter];
						defCounter++;
						sourceCounter++;
					}
					defString[defCounter] = '\r';

					return defString;
				}
			}			
		}
	}
	return NULL;
}

char* getGroupDefinitionString(uint8_t index, uint8_t subType, const char* langID)
{
	if (index < groupCount)
	{
		PPROPERTY_GROUP group = getPropertyGroupFromIndex(index);
		if (group != NULL)
		{
			const char* desc;

			if (descriptCallback != NULL)
			{
				desc = descriptCallback(DESCRIPTOR_REQUEST_ELEMENT_GROUP, getGroupIDAtIndex(index), langID);
			}
			else
			{
				desc = group->descriptor;
			}

			size_t len = 14;
			if (desc != NULL)
			{
				len += strlen(desc);

				char* defString = (char*)malloc(len);
				if (defString != NULL)
				{
					memset(defString, 0, len);

					char twoC[2];

					defString[0] = '2';						// group data transmission
					defString[1] = numToChar(subType);		// transmission sub type

					x8BitValueToHexTwoCharBuffer(index, twoC);
					defString[2] = twoC[0];					// group index
					defString[3] = twoC[1];					// group index

					if (len > 255)
					{
						twoC[0] = 'f';
						twoC[1] = 'f';
					}
					else
					{
						x8BitValueToHexTwoCharBuffer((uint8_t)len, twoC);
					}
					defString[4] = twoC[0];					// data size
					defString[5] = twoC[1];					// data size
					defString[6] = '0';						// error flag
					defString[7] = (len > 255) ? '5' : '0';	// status flag

					x8BitValueToHexTwoCharBuffer(
						getGroupMemberCount(group->ID),
						twoC
					);
					defString[8] = twoC[0];					// member count
					defString[9] = twoC[1];					// member count

					x8BitValueToHexTwoCharBuffer(group->imageID, twoC);
					defString[10] = twoC[0];				// image ID
					defString[11] = twoC[1];				// image ID

					uint8_t defCounter = 12;
					uint8_t sourceCounter = 0;

					while (desc[sourceCounter] != '\0')
					{
						defString[defCounter] = desc[sourceCounter];
						defCounter++;
						sourceCounter++;
					}
					defString[defCounter] = '\r';

					return defString;
				}
			}
		}
	}
	return NULL;
}

char* getInitString()
{
	size_t len = 20;
	len *= sizeof(char);
	char* initString = (char*)malloc(len);
	if (initString != NULL)
	{
		//memset(initString, 0, len);
		char twoC[2];

		initString[0] = '7';	// init transmission
		initString[1] = '2';	// response
		initString[2] = '0';	// index
		initString[3] = '0';	// index
		initString[4] = '0';	// dataSize
		initString[5] = 'b';	// dataSize
		initString[6] = '0';	// errorFlag
		initString[7] = '0';	// statusFlag

		x8BitValueToHexTwoCharBuffer(propertyCount, twoC);
		initString[8] = twoC[0];	// property amount
		initString[9] = twoC[1];	// property amount

		x8BitValueToHexTwoCharBuffer(groupCount, twoC);
		initString[10] = twoC[0];	// group amount
		initString[11] = twoC[1];	// group amount

		initString[12] = (deviceInfoFlags & FLAG_DEV_INFO_CACHING_PERMISSION) ? '1' : '0'; 	// caching permission
		initString[13] = (deviceInfoFlags & FLAG_DEV_INFO_BINDING_REQUIRED) ? '1' : '0'; 	// binding required
		initString[14] = (deviceInfoFlags & FLAG_DEV_INFO_STANDALONE_MODE) ? '1' : '0'; 	// standalone mode

		initString[15] = '0';	// reserved
		initString[16] = '0';	// reserved
		initString[17] = '0';	// reserved
		initString[18] = '\r';	// delimiter
		initString[19] = '\0';	// terminator

		return initString;
	}
	return NULL;
}

void sendTimeRequest() 
{
	sendData("530002002\r\0");
}

void sendDateRequest() 
{
	sendData("530002009\r\0");
}

void sendPropertyReloadCommand() 
{
	sendData("530002003\r\0");
}

void sendPropertyToCacheCommand() 
{
	sendData("530002004\r\0");
}

void sendLanguageRequest() 
{
	sendData("530002005\r\0");
}

void sendRefreshAllStatesCommand() 
{
	sendData("530002006\r\0");
}

void sendNavBackToDeviceMainCommand()
{
	sendData("530002007\r\0");
}

void sendCloseDeviceCommand() 
{
	sendData("530002008\r\0");
}

void sendUIModeInfoRequest() 
{
	sendData("53000200a\r\0");
}

void sendUserMessage(const char* message, uint8_t type, uint8_t displayTime)
{
	if (message != NULL)
	{
		char header[] = "53000200100\0";
		uint16_t len = (uint16_t)strlen(message);
		len += 13;
		if (len > 255)
		{
			header[4] = 'f';
			header[5] = 'f';
		}
		else
		{
			char twoC[2];
			x8BitValueToHexTwoCharBuffer((uint8_t)len, twoC);
			header[4] = twoC[0];
			header[5] = twoC[1];
		}
		header[9] = numToChar(type);
		header[10] = numToChar(displayTime);

		char* buffer = (char*)malloc(len);
		if (buffer != NULL)
		{
			uint8_t i = 0;
			memset(buffer, 0, len);

			while (header[i] != '\0' && i < len)
			{
				buffer[i] = header[i];
				i++;
			}

			uint8_t j = 0;

			while(message[j] != '\0' && i < len)
			{
				buffer[i] = message[j];
				i++;
				j++;
			}
			sendData(buffer);
			free(buffer);
		}
	}
}

void addTextListPresenterElement(uint8_t textListPresenterID, const char* text, uint8_t elementType, uint8_t useBackgroundStack)
{
	if (text != NULL)
	{
		PDEVICE_PROPERTY tlp = findProperty(textListPresenterID, NULL);
		if (tlp != NULL)
		{
			PTEXT_LIST_PRESENTER_STATE tlpState = getTextListPresenterState(textListPresenterID);
			if(tlpState != NULL)
			{
				size_t len = (size_t)strlen(text);
				len += 13; // text list presenter default header length

				char* msg = (char*)malloc(len);
				if (msg != NULL)
				{
                    memset(msg, 0, len);

					msg[0] = '3';
					msg[1] = '4';

					char twoC[2];
					x8BitValueToHexTwoCharBuffer(textListPresenterID, twoC);
					msg[2] = twoC[0];
					msg[3] = twoC[1];

                    if(len > 255)
                    {
                        msg[4] = 'f';
                        msg[5] = 'f';
                    }
                    else
                    {
                        x8BitValueToHexTwoCharBuffer((uint8_t)len, twoC);
                        msg[4] = twoC[0];
                        msg[5] = twoC[1];
                    }

					msg[6] = '0';
					msg[7] = (len > 255) ? '5' : '0';

					msg[8] = (tlpState->useBackgroundStack != 0) ? '1' : '0';
					msg[9] = '1';

					msg[10] = numToChar(elementType);

					uint8_t i = 0;

                    while (text[i] != '\0' && i < len)
                    {
                        msg[11 + i] = text[i];
                        i++;
                    }
                    msg[11 + i] = '\r';

                    sendData(msg);
                    free(msg);
				}
			}
		}
	}
}

void clearTextListPresenterElements(uint8_t textListPresenterID)
{
    PDEVICE_PROPERTY tlp = findProperty(textListPresenterID, NULL);
    if (tlp != NULL)
    {
        PTEXT_LIST_PRESENTER_STATE tlpState = getTextListPresenterState(textListPresenterID);
        if (tlpState != NULL)
        {
            char msg[13] = { 0 };

            msg[0] = '3';
            msg[1] = '4';

            char twoC[2];
            x8BitValueToHexTwoCharBuffer(textListPresenterID, twoC);
            msg[2] = twoC[0];   // ID
            msg[3] = twoC[1];   // ID

            msg[4] = '0';   // data size
            msg[5] = 'd';   // data size

            msg[6] = '0';   // error flag
            msg[7] = '0';   // status flag

            msg[8] = (tlpState->useBackgroundStack != 0) ? '1' : '0';
            msg[9] = '2';   // action
            msg[10] = '0';  // element type (not used here)
            msg[11] = '\r'; // delimiter

            sendData(msg);
        }
    }
}

void fetchTimeResponseData(uint8_t* hours, uint8_t* minutes, uint8_t* seconds)
{
	*hours = responseData.a;
	*minutes = responseData.b;
	*seconds = responseData.c;
}

void fetchDateResponseData(uint16_t* year, uint8_t* month, uint8_t* day)
{
	*year = responseData.d;
	*month = responseData.b;
	*day = responseData.a;
}

void fetchUIModeInfoResponseData(uint8_t* mode)
{
	*mode = responseData.a;
}

char intValToHexChar(uint8_t val) {
	switch (val) {
	case 0:
		return '0';
	case 1:
		return '1';
	case 2:
		return '2';
	case 3:
		return '3';
	case 4:
		return '4';
	case 5:
		return '5';
	case 6:
		return '6';
	case 7:
		return '7';
	case 8:
		return '8';
	case 9:
		return '9';
	case 10:
		return 'a';
	case 11:
		return 'b';
	case 12:
		return 'c';
	case 13:
		return 'd';
	case 14:
		return 'e';
	case 15:
		return 'f';
	default:
		return '0';
	}
}

uint8_t hexCharToIntVal(char h)
{
	switch (h) {
	case '0':
		return 0;
	case '1':
		return 1;
	case '2':
		return 2;
	case '3':
		return 3;
	case '4':
		return 4;
	case '5':
		return 5;
	case '6':
		return 6;
	case '7':
		return 7;
	case '8':
		return 8;
	case '9':
		return 9;
	case 'a':
		return 10;
	case 'b':
		return 11;
	case 'c':
		return 12;
	case 'd':
		return 13;
	case 'e':
		return 14;
	case 'f':
		return 15;
	default:
		return 0;
	}
}

void x8BitValueToHexTwoCharBuffer(uint8_t toConvert, char* buffer_out)
{
	if (toConvert < 16) {
		buffer_out[0] = '0';
		buffer_out[1] = intValToHexChar(toConvert);
	}
	else {
		uint8_t firstVal = (uint8_t)(toConvert / 16);
		buffer_out[0] = intValToHexChar(firstVal);
		uint8_t secondVal = (uint8_t)(toConvert - (firstVal * 16));
		buffer_out[1] = intValToHexChar(secondVal);
	}
}

uint8_t x2CharHexValueTo8BitValue(char left, char right)
{
	uint8_t lft = hexCharToIntVal(left);
	uint8_t rgt = hexCharToIntVal(right);
	return (uint8_t)((lft * 16) + rgt);
}

int16_t x4CharHexValueToSigned16BitValue(char hl, char hr, char ll, char lr)
{
	uint16_t highLeft = hexCharToIntVal(hl);
	uint16_t highRight = hexCharToIntVal(hr);
	uint16_t lowLeft = hexCharToIntVal(ll);
	uint16_t lowRight = hexCharToIntVal(lr);
	uint16_t result = (highLeft << 12) | (highRight << 8) | (lowLeft << 4) | (lowRight);
	return (int16_t)(result);
}

uint16_t x4CharHexValueTo16BitValue(char hl, char hr, char ll, char lr)
{
	uint16_t highLeft = hexCharToIntVal(hl);
	uint16_t highRight = hexCharToIntVal(hr);
	uint16_t lowLeft = hexCharToIntVal(ll);
	uint16_t lowRight = hexCharToIntVal(lr);
	uint16_t result = (highLeft << 12) | (highRight << 8) | (lowLeft << 4) | (lowRight);
	return (uint16_t)(result);
}

void xSigned16BitValueToHex4CharBuffer(int16_t toConvert, char* buffer_out)
{
	uint16_t v = (uint16_t)(toConvert);
	uint8_t l = (uint8_t)(v & 0x00FF);
	uint8_t h = (uint8_t)((v & 0xFF00) >> 8);
	char tc[2];
	x8BitValueToHexTwoCharBuffer(h, tc);
	buffer_out[0] = tc[0];
	buffer_out[1] = tc[1];
	x8BitValueToHexTwoCharBuffer(l, tc);
	buffer_out[2] = tc[0];
	buffer_out[3] = tc[1];
}

void x16BitValueToHex4CharBuffer(uint16_t toConvert, char* buffer_out)
{
	uint8_t l = (uint8_t)(toConvert & 0x00FF);
	uint8_t h = (uint8_t)((toConvert & 0xFF00) >> 8);
	char tc[2];
	x8BitValueToHexTwoCharBuffer(h, tc);
	buffer_out[0] = tc[0];
	buffer_out[1] = tc[1];
	x8BitValueToHexTwoCharBuffer(l, tc);
	buffer_out[2] = tc[0];
	buffer_out[3] = tc[1];
}

char* stringToPtr(const char* string)
{
	if (string != NULL)
	{
		size_t len = strlen(string) * sizeof(char);
		len += sizeof(char);
		char* data = (char*)malloc(len);
		if (data != NULL)
		{
			memset(data, 0, len);
			strcpy(data, string);
			return data;
		}
	}
	return NULL;
}

uint8_t propertyIndexFromID(uint8_t ID)
{
	for (uint8_t i = 0; i < propertyCount; i++)
	{
		if (ID == propertyList[i]->ID)
		{
			return i;
		}
	}
	return 0;
}

PDEVICE_PROPERTY findProperty(uint8_t ID, uint8_t* index_out)
{
	for (uint8_t i = 0; i < propertyCount; i++)
	{
		if (ID == propertyList[i]->ID)
		{
			if (index_out != NULL)
			{
				*index_out = i;
			}
			return propertyList[i];
		}
	}
	return NULL;
}

void sendData(const char* data)
{
	if (outGateway != NULL && connection_state == CONNECTED)
	{
		outGateway(data);
	}
}

void rgbStateToStateResponseString(PRGB_SELECTOR_STATE pState, char* response_string_out)
{
	if (pState != NULL && response_string_out != NULL)
	{
		char twoStr[2];
		uint8_t propertyIndex = propertyIndexFromID(pState->associatedPropertyID);

		// transmission types
		response_string_out[0] = '3';
		response_string_out[1] = '2';

		// property Index
		x8BitValueToHexTwoCharBuffer(propertyIndex, twoStr);
		response_string_out[2] = twoStr[0];
		response_string_out[3] = twoStr[1];

		// data-size
		response_string_out[4] = '0';
		response_string_out[5] = 'd';

		// header-flags
		response_string_out[6] = '0';
		response_string_out[7] = '0';

		// on-off state
		response_string_out[8] = (pState->isOn == 0) ? '0' : '1';

		// flags:
		x8BitValueToHexTwoCharBuffer(pState->visibilityFlags, twoStr);
		response_string_out[9] = twoStr[0];
		response_string_out[10] = twoStr[1];

		// program value
		x8BitValueToHexTwoCharBuffer(pState->program, twoStr);
		response_string_out[11] = twoStr[0];
		response_string_out[12] = twoStr[1];

		// red value
		x8BitValueToHexTwoCharBuffer(pState->red, twoStr);
		response_string_out[13] = twoStr[0];
		response_string_out[14] = twoStr[1];

		// green value
		x8BitValueToHexTwoCharBuffer(pState->green, twoStr);
		response_string_out[15] = twoStr[0];
		response_string_out[16] = twoStr[1];

		// blue value
		x8BitValueToHexTwoCharBuffer(pState->blue, twoStr);
		response_string_out[17] = twoStr[0];
		response_string_out[18] = twoStr[1];

		// hard transition value
		response_string_out[19] = (pState->hardTransition == 0) ? '0' : '1';

		// delimiter
		response_string_out[20] = '\r';
		response_string_out[21] = '\0';
	}
}

void executionStringToRGBStateStruct(const char* data, PRGB_SELECTOR_STATE pState)
{
	if (data != NULL && pState != NULL)
	{
		pState->isOn = (data[8] == '1') ? 1 : 0;
		pState->visibilityFlags = x2CharHexValueTo8BitValue(data[9], data[10]);
		pState->program = x2CharHexValueTo8BitValue(data[11], data[12]);
		pState->red = x2CharHexValueTo8BitValue(data[13], data[14]);
		pState->green = x2CharHexValueTo8BitValue(data[15], data[16]);
		pState->blue = x2CharHexValueTo8BitValue(data[17], data[18]);
		pState->hardTransition = (data[19] == '1') ? 1 : 0;		
	}
}

void exLevelStateToStateResponseString(PEX_LEVEL_SELECTOR_STATE pState, char* response_string_out)
{
	if (pState != NULL && response_string_out != NULL)
	{
		char twoStr[2];
		char fourStr[4];
		uint8_t propertyIndex = propertyIndexFromID(pState->associatedPropertyID);

		// transmission types
		response_string_out[0] = '3';
		response_string_out[1] = '2';

		// property Index
		x8BitValueToHexTwoCharBuffer(propertyIndex, twoStr);
		response_string_out[2] = twoStr[0];
		response_string_out[3] = twoStr[1];

		// data-size
		response_string_out[4] = '0';
		response_string_out[5] = '4';

		// header-flags
		response_string_out[6] = '0';
		response_string_out[7] = '0';

		// on-off state
		response_string_out[8] = (pState->isOn) ? '1' : '0';

		// level value
		xSigned16BitValueToHex4CharBuffer(pState->level, fourStr);
		response_string_out[9] = fourStr[0];
		response_string_out[10] = fourStr[1];
		response_string_out[11] = fourStr[2];
		response_string_out[12] = fourStr[3];

		// min value
		xSigned16BitValueToHex4CharBuffer(pState->min, fourStr);
		response_string_out[13] = fourStr[0];
		response_string_out[14] = fourStr[1];
		response_string_out[15] = fourStr[2];
		response_string_out[16] = fourStr[3];

		// min value
		xSigned16BitValueToHex4CharBuffer(pState->max, fourStr);
		response_string_out[17] = fourStr[0];
		response_string_out[18] = fourStr[1];
		response_string_out[19] = fourStr[2];
		response_string_out[20] = fourStr[3];

		// flag value
		x8BitValueToHexTwoCharBuffer(pState->flags, twoStr);
		response_string_out[21] = twoStr[0];
		response_string_out[22] = twoStr[1];

		// delimiter
		response_string_out[23] = '\r';
		response_string_out[24] = '\0';
	}
}

void executionStringToExLevelStruct(const char* data, PEX_LEVEL_SELECTOR_STATE pState)
{
	if (data != NULL && pState != NULL)
	{
		pState->isOn = (data[8] == '1') ? 1 : 0;
		pState->level = x4CharHexValueToSigned16BitValue(data[9], data[10], data[11], data[12]);
		// THE EXECUTION STRING DOES NOT CONTAIN THE MIN,MAX AND FLAG VALUE SINCE THEY ARE ONLY FOR CONFIG (ONE DIRECTION!)
	}
}

void timeSelectorStateToStateResponseString(PTIME_SELECTOR_STATE pState, char* response_string_out)
{
	if (pState != NULL && response_string_out != NULL)
	{
		char twoStr[2];
		uint8_t propertyIndex = propertyIndexFromID(pState->associatedPropertyID);

		// transmission types
		response_string_out[0] = '3';
		response_string_out[1] = '2';

		// property Index
		x8BitValueToHexTwoCharBuffer(propertyIndex, twoStr);
		response_string_out[2] = twoStr[0];
		response_string_out[3] = twoStr[1];

		// data-size
		response_string_out[4] = '0';
		response_string_out[5] = '5';

		// header-flags
		response_string_out[6] = '0';
		response_string_out[7] = '0';

		// hour value
		x8BitValueToHexTwoCharBuffer(pState->hour, twoStr);
		response_string_out[8] = twoStr[0];
		response_string_out[9] = twoStr[1];

		// minute value
		x8BitValueToHexTwoCharBuffer(pState->minute, twoStr);
		response_string_out[10] = twoStr[0];
		response_string_out[11] = twoStr[1];

		// delimiter
		response_string_out[12] = '\r';
		response_string_out[13] = '\0';
	}
}

void executionStringToTimeSelectorStruct(const char* data, PTIME_SELECTOR_STATE pState)
{
	if (data != NULL && pState != NULL)
	{
		pState->hour = x2CharHexValueTo8BitValue(data[8], data[9]);
		pState->minute = x2CharHexValueTo8BitValue(data[10], data[11]);
	}
}

void timeFrameSelectorStateToStateResponseString(PTIME_FRAME_STATE pState, char* response_string_out)
{
	if (pState != NULL && response_string_out != NULL)
	{
		char twoStr[2];
		uint8_t propertyIndex = propertyIndexFromID(pState->associatedPropertyID);

		// transmission types
		response_string_out[0] = '3';
		response_string_out[1] = '2';

		// property Index
		x8BitValueToHexTwoCharBuffer(propertyIndex, twoStr);
		response_string_out[2] = twoStr[0];
		response_string_out[3] = twoStr[1];

		// data-size
		response_string_out[4] = '0';
		response_string_out[5] = '7';

		// header-flags
		response_string_out[6] = '0';
		response_string_out[7] = '0';

		// from-hour value
		x8BitValueToHexTwoCharBuffer(pState->start_hour, twoStr);
		response_string_out[8] = twoStr[0];
		response_string_out[9] = twoStr[1];

		// from-minute value
		x8BitValueToHexTwoCharBuffer(pState->start_minute, twoStr);
		response_string_out[10] = twoStr[0];
		response_string_out[11] = twoStr[1];

		// to-hour value
		x8BitValueToHexTwoCharBuffer(pState->end_hour, twoStr);
		response_string_out[12] = twoStr[0];
		response_string_out[13] = twoStr[1];

		// to-minute value
		x8BitValueToHexTwoCharBuffer(pState->end_minute, twoStr);
		response_string_out[14] = twoStr[0];
		response_string_out[15] = twoStr[1];

		// delimiter
		response_string_out[16] = '\r';
		response_string_out[17] = '\0';
	}
}

void executionStringToTimeFrameSelectorStruct(const char* data, PTIME_FRAME_STATE pState) 
{
	if (data != NULL && pState != NULL)
	{
		pState->start_hour = x2CharHexValueTo8BitValue(data[8], data[9]);
		pState->start_minute = x2CharHexValueTo8BitValue(data[10], data[11]);
		pState->end_hour = x2CharHexValueTo8BitValue(data[12], data[13]);
		pState->end_minute = x2CharHexValueTo8BitValue(data[14], data[15]);
	}
}

void dateSelectorStateToStateResponseString(PDATE_SELECTOR_STATE pState, char* response_string_out)
{
	if (pState != NULL && response_string_out != NULL)
	{
		char twoStr[2];
		char fourStr[4];
		uint8_t propertyIndex = propertyIndexFromID(pState->associatedPropertyID);

		// transmission types
		response_string_out[0] = '3';
		response_string_out[1] = '2';

		// property Index
		x8BitValueToHexTwoCharBuffer(propertyIndex, twoStr);
		response_string_out[2] = twoStr[0];
		response_string_out[3] = twoStr[1];

		// data-size
		response_string_out[4] = '0';
		response_string_out[5] = '9';

		// header-flags
		response_string_out[6] = '0';
		response_string_out[7] = '0';

		// day value
		x8BitValueToHexTwoCharBuffer(pState->day, twoStr);
		response_string_out[8] = twoStr[0];
		response_string_out[9] = twoStr[1];

		// month value
		x8BitValueToHexTwoCharBuffer(pState->month, twoStr);
		response_string_out[10] = twoStr[0];
		response_string_out[11] = twoStr[1];

		// year value
		x16BitValueToHex4CharBuffer(pState->year, fourStr);
		response_string_out[12] = fourStr[0];
		response_string_out[13] = fourStr[1];
		response_string_out[14] = fourStr[2];
		response_string_out[15] = fourStr[3];

		// delimiter
		response_string_out[16] = '\r';
		response_string_out[17] = '\0';
	}
}

void executionStringToDateSelectorStruct(const char* data, PDATE_SELECTOR_STATE pState)
{
	if (data != NULL && pState != NULL)
	{
		pState->day = x2CharHexValueTo8BitValue(data[8], data[9]);
		pState->month = x2CharHexValueTo8BitValue(data[10], data[11]);
		pState->year = x4CharHexValueTo16BitValue(data[12], data[13], data[14], data[15]);
	}
}

void unlockControlStateToStateResponseString(PUNLOCK_CONTROL_STATE pState, char* response_string_out)
{
	if (pState != NULL && response_string_out != NULL)
	{
		char twoStr[2];
		uint8_t propertyIndex = propertyIndexFromID(pState->associatedPropertyID);

		// transmission types
		response_string_out[0] = '3';
		response_string_out[1] = '2';

		// property Index
		x8BitValueToHexTwoCharBuffer(propertyIndex, twoStr);
		response_string_out[2] = twoStr[0];
		response_string_out[3] = twoStr[1];

		// data-size
		response_string_out[4] = '0';
		response_string_out[5] = 'f';

		// header-flags
		response_string_out[6] = '0';
		response_string_out[7] = '0';

		// locked/unlocked
		response_string_out[8] = (pState->lockState == UC_LOCKED) ? '1' : '2';

		// mode
		response_string_out[9] = (pState->mode == UCMODE_UNLOCK_MODE) ? '0' : '1';

		// flags
		x8BitValueToHexTwoCharBuffer(pState->flags, twoStr);
		response_string_out[10] = twoStr[0];
		response_string_out[11] = twoStr[1];

		// delimiter
		response_string_out[12] = '\r';
		response_string_out[13] = '\0';
	}
}

uint8_t executionStringToUnlockControlStruct(const char* data, PUNLOCK_CONTROL_STATE pState)
{
	if (data != NULL && pState != NULL)
	{
		pState->flags = x2CharHexValueTo8BitValue(data[10], data[11]);

		if (data[9] == '1') {
			// pin change transmission

			// at first record current pin
			char pin[11] = { 0 };

			uint8_t counter = 12;

			for (uint8_t i = 0; i < 10; i++) {
				if ((data[counter] == '\r') || (data[counter] == '\0') || (data[counter] == ':')) {
					break;
				}
				pin[i] = data[counter];
				counter++;
			}
			//pin[counter - 12] = '\0';

			if (strcmp(pin, pState->key) == 0) {

				if (data[counter] != ':') {
					return UNLOCKCTRL_NEW_PIN_INVALID;
				}
				else {
					counter++;

					if (data[counter] != ':') {
						return UNLOCKCTRL_NEW_PIN_INVALID;
					}
					else {
						counter++;

						// erase old pin
						for (uint8_t i = 0; i < 11; i++) {
							pState->key[i] = '\0';
						}

						// record new pin
						for (uint8_t i = 0; i < 10; i++) {
							if ((data[counter] == '\r') || (data[counter] == '\0')) {
								counter = i;
								break;
							}
							pState->key[i] = data[counter];
							counter++;
						}
						//pin[counter] = '\0';

						return UNLOCKCTRL_PIN_CHANGE_SUCCESS;
					}
				}
			}
			else {
				return UNLOCKCTRL_PIN_CHANGE_FAIL_WRONG_PIN;
			}
		}
		else if (data[8] == '2') {
			// unlock requested -> record pin from transmission:
			char pin[11] = { 0 };

			//pin[19] = '\0';

			uint8_t counter = 12;

			for (uint8_t i = 0; i < 10; i++) {
				if ((data[counter] == '\r') || (data[counter] == '\0')) {
					break;
				}
				pin[i] = data[counter];
				counter++;
			}
			//pin[counter - 12] = '\0';

			// unlock requested -> compare pin
			if (strcmp(pin, pState->key) == 0) {
				// pin accepted
				pState->lockState = UC_UNLOCKED;
				pState->flags = 0;
				pState->mode = 0;

				return UNLOCKCTRL_UNLOCK_SUCCESS;
			}
			else {
				// pin wrong
				return UNLOCKCTRL_UNLOCK_REJECTED_WRONG_PIN;
			}
		}
		else if (data[8] == '1') {
			// lock requested

			pState->lockState = UC_LOCKED;
			pState->flags = 0;
			pState->mode = 0;

			return UNLOCKCTRL_LOCK_SUCCESS;
		}
		return UNLOCKCTRL_UNEXPECTED;
	}
	return UNLOCKCTRL_UNEXPECTED;
}

void navigatorControlStateToStateResponseString(PNAVIGATOR_STATE pState, char* response_string_out)
{
	if (pState != NULL && response_string_out != NULL)
	{
		char twoStr[2];
		uint8_t propertyIndex = propertyIndexFromID(pState->associatedPropertyID);

		// transmission types
		response_string_out[0] = '3';
		response_string_out[1] = '2';

		// property Index
		x8BitValueToHexTwoCharBuffer(propertyIndex, twoStr);
		response_string_out[2] = twoStr[0];
		response_string_out[3] = twoStr[1];

		// data-size
		response_string_out[4] = '0';
		response_string_out[5] = '5';

		// header-flags
		response_string_out[6] = '0';
		response_string_out[7] = '0';

		// visibilities
		response_string_out[8] = (pState->visibilityFlags & NCFLAG_VHIDE_UP_BUTTON) ? '0' : '1';
		response_string_out[9] = (pState->visibilityFlags & NCFLAG_VHIDE_RIGHT_BUTTON) ? '0' : '1';
		response_string_out[10] = (pState->visibilityFlags & NCFLAG_VHIDE_DOWN_BUTTON) ? '0' : '1';
		response_string_out[11] = (pState->visibilityFlags & NCFLAG_VHIDE_LEFT_BUTTON) ? '0' : '1';
		response_string_out[12] = (pState->visibilityFlags & NCFLAG_VHIDE_MID_BUTTON) ? '0' : '1';

		// type (unused in state response)
		response_string_out[13] = '0';

		// delimiter
		response_string_out[14] = '\r';
		response_string_out[15] = '\0';
	}
}

void executionStringToNavigatorControlStruct(const char* data, PNAVIGATOR_STATE pState)
{
	if (data != NULL && pState != NULL)
	{
		pState->type = (data[13] == '1') ? NAVIGATOR_ACTIONTYPE_TOUCH_DOWN : NAVIGATOR_ACTIONTYPE_TOUCH_RELEASE;
		pState->buttonFlags = 0;

		if (data[8] == '1') {
			// up
			pState->buttonFlags |= NCBFLAG_UP_BUTTON;
		}
		if (data[9] == '1') {
			// right
			pState->buttonFlags |= NCBFLAG_RIGHT_BUTTON;
		}
		if (data[10] == '1') {
			// down
			pState->buttonFlags |= NCBFLAG_DOWN_BUTTON;
		}
		if (data[11] == '1') {
			// left
			pState->buttonFlags |= NCBFLAG_LEFT_BUTTON;
		}
		if (data[12] == '1') {
			// mid
			pState->buttonFlags |= NCBFLAG_MID_BUTTON;
		}	
	}
}

void barGraphStateToStateResponseString(PBAR_GRAPH_STATE pState, char** response_string_out)
{
	if (pState != NULL)// && response_string_out != NULL)
	{
		// at first calculate the required string size
		uint16_t bufferSize = 13;
		uint8_t propertyIndex = propertyIndexFromID(pState->associatedPropertyID);

		if (pState->data != NULL)
		{
			bufferSize += (uint16_t)strlen(pState->data);
		}
		size_t len = bufferSize;
		bufferSize *= sizeof(char);

		// then allocate the string
		(*response_string_out) = (char*)malloc(len);
		if ((*response_string_out) != NULL)
		{
			char twoStr[2];
			memset((*response_string_out), 0, len);

			// transmission types
			(*response_string_out)[0] = '3';
			(*response_string_out)[1] = '2';

			// property Index
			x8BitValueToHexTwoCharBuffer(propertyIndex, twoStr);
			(*response_string_out)[2] = twoStr[0];
			(*response_string_out)[3] = twoStr[1];

			// data-size
			if (bufferSize > 255)
			{
				(*response_string_out)[4] = 'f';
				(*response_string_out)[5] = 'f';
			}
			else
			{
				x8BitValueToHexTwoCharBuffer((uint8_t)bufferSize, twoStr);
				(*response_string_out)[4] = twoStr[0];
				(*response_string_out)[5] = twoStr[1];
			}

			// header-flags
			(*response_string_out)[6] = '0';
			(*response_string_out)[7] = (bufferSize > 255) ? '5' : '0';

			// bar graph flags
			x8BitValueToHexTwoCharBuffer(pState->flags, twoStr);
			(*response_string_out)[8] = twoStr[0];
			(*response_string_out)[9] = twoStr[1];

			// number of bars
			(*response_string_out)[10] = numToChar(pState->numBars);

			bufferSize = 11;
			uint16_t j = 0;

			if (pState->data != NULL)
			{
				while (pState->data[j] != '\0')
				{
					(*response_string_out)[bufferSize] = pState->data[j];
					bufferSize++;
					j++;
				}
			}
			(*response_string_out)[bufferSize] = '\r';
		}		
	}
}

void lineGraphStateToStateResponseString(PLINE_GRAPH_STATE pState, char** response_string_out)
{
	if (pState != NULL)// && response_string_out != NULL)
	{
		// at first calculate the required string size
		uint16_t bufferSize = 13;
		uint8_t propertyIndex = propertyIndexFromID(pState->associatedPropertyID);

		if (pState->data != NULL)
		{
			bufferSize += (uint16_t)strlen(pState->data);
		}
		size_t len = bufferSize;
		bufferSize *= sizeof(char);

		// then allocate the string
		(*response_string_out) = (char*)malloc(len);
		if ((*response_string_out) != NULL)
		{
			char twoStr[2];
			memset((*response_string_out), 0, len);

			// transmission types
			(*response_string_out)[0] = '3';
			(*response_string_out)[1] = '2';

			// property Index
			x8BitValueToHexTwoCharBuffer(propertyIndex, twoStr);
			(*response_string_out)[2] = twoStr[0];
			(*response_string_out)[3] = twoStr[1];

			// data-size
			if (bufferSize > 255)
			{
				(*response_string_out)[4] = 'f';
				(*response_string_out)[5] = 'f';
			}
			else
			{
				x8BitValueToHexTwoCharBuffer((uint8_t)bufferSize, twoStr);
				(*response_string_out)[4] = twoStr[0];
				(*response_string_out)[5] = twoStr[1];
			}

			// header-flags
			(*response_string_out)[6] = '0';
			(*response_string_out)[7] = (bufferSize > 255) ? '5' : '0';

			// line graph flags
			x8BitValueToHexTwoCharBuffer(pState->flags, twoStr);
			(*response_string_out)[8] = twoStr[0];
			(*response_string_out)[9] = twoStr[1];

			// number of bars
			(*response_string_out)[10] = numToChar(pState->type);

			bufferSize = 11;
			uint16_t j = 0;

			if (pState->data != NULL)
			{
				while (pState->data[j] != '\0')
				{
					(*response_string_out)[bufferSize] = pState->data[j];
					bufferSize++;
					j++;
				}
			}
			(*response_string_out)[bufferSize] = '\r';
		}
	}
}

void stringInterrogatorStateToStateResponseString(PSTRING_INTERROGATOR_STATE pState, char** response_string_out)
{
	if (pState != NULL)// && response_string_out != NULL)
	{
		// at first calculate the required string size
		uint16_t bufferSize = 15;
		uint8_t propertyIndex = propertyIndexFromID(pState->associatedPropertyID);

		if (pState->data != NULL)
		{
			bufferSize += (uint16_t)strlen(pState->data);
		}
		size_t len = bufferSize;
		bufferSize *= sizeof(char);

		// then allocate the string
		(*response_string_out) = (char*)malloc(len);
		if ((*response_string_out) != NULL)
		{
			char twoStr[2];
			memset((*response_string_out), 0, len);

			// transmission types
			(*response_string_out)[0] = '3';
			(*response_string_out)[1] = '2';

			// property Index
			x8BitValueToHexTwoCharBuffer(propertyIndex, twoStr);
			(*response_string_out)[2] = twoStr[0];
			(*response_string_out)[3] = twoStr[1];

			// data-size
			if (bufferSize > 255)
			{
				(*response_string_out)[4] = 'f';
				(*response_string_out)[5] = 'f';
			}
			else
			{
				x8BitValueToHexTwoCharBuffer((uint8_t)bufferSize, twoStr);
				(*response_string_out)[4] = twoStr[0];
				(*response_string_out)[5] = twoStr[1];
			}

			// header-flags
			(*response_string_out)[6] = '0';
			(*response_string_out)[7] = (bufferSize > 255) ? '5' : '0';

			// field visibilities
			(*response_string_out)[8] = numToChar(pState->fieldVisibilities);

			// field one input type
			(*response_string_out)[9] = numToChar(pState->fieldOneInputType);

			// field two input type
			(*response_string_out)[10] = numToChar(pState->fieldTwoInputType);

			// nav back on button press
			(*response_string_out)[11] = (pState->flags & STRIRG_FLAG_NAVIGATE_BACK_ON_BUTTON_PRESS) ? '1' : '0';

			// accept non ascii character
			(*response_string_out)[12] = (pState->flags & STRIRG_FLAG_ACCEPT_NON_ASCII_CHARACTERS) ? '1' : '0';

			bufferSize = 13;
			uint16_t j = 0;

			if (pState->data != NULL)
			{
				while (pState->data[j] != '\0')
				{
					(*response_string_out)[bufferSize] = pState->data[j];
					bufferSize++;
					j++;
				}
			}
			(*response_string_out)[bufferSize] = '\r';
		}
	}
}

void executionStringToStringInterrogatorStruct(const char* data, PSTRING_INTERROGATOR_STATE pState)
{
	if (data != NULL && pState != NULL)
	{
		// only record the data

		// count
		uint16_t pos = 13;

		while (data[pos] != '\0')
		{
			pos++;
		}
		pos -= 12; // 1 for the terminator
		size_t len = pos;
		len *= sizeof(char);

		if (pState->responseData != NULL)
		{
			free(pState->responseData);
			pState->responseData = NULL;
		}

		pState->responseData = (char*)malloc(len);
		if (pState->responseData != NULL)
		{
			memset(pState->responseData, 0, len);
				
			pos = 13;
			while (data[pos] != '\0')
			{
				pState->responseData[pos - 13] = data[pos];
				pos++;
			}
		}		
	}
}

void textListPresenterStateToStateResponseString(PTEXT_LIST_PRESENTER_STATE pState, char** response_string_out)
{
	if (pState != NULL)// && response_string_out != NULL)
	{
		// at first calculate the required string size
		uint16_t bufferSize = 13;
		uint8_t propertyIndex = propertyIndexFromID(pState->associatedPropertyID);

		if (pState->data != NULL)
		{
			bufferSize += (uint16_t)strlen(pState->data);
		}
		size_t len = bufferSize;
		bufferSize *= sizeof(char);

		// then allocate the string
		(*response_string_out) = (char*)malloc(len);
		if ((*response_string_out) != NULL)
		{
			char twoStr[2];
			memset((*response_string_out), 0, len);

			// transmission types
			(*response_string_out)[0] = '3';
			(*response_string_out)[1] = '2';

			// property Index
			x8BitValueToHexTwoCharBuffer(propertyIndex, twoStr);
			(*response_string_out)[2] = twoStr[0];
			(*response_string_out)[3] = twoStr[1];

			// data-size
			if (bufferSize > 255)
			{
				(*response_string_out)[4] = 'f';
				(*response_string_out)[5] = 'f';
			}
			else
			{
				x8BitValueToHexTwoCharBuffer((uint8_t)bufferSize, twoStr);
				(*response_string_out)[4] = twoStr[0];
				(*response_string_out)[5] = twoStr[1];
			}

			// header-flags
			(*response_string_out)[6] = '0';
			(*response_string_out)[7] = (bufferSize > 255) ? '5' : '0';

			// use background stack
			(*response_string_out)[8] = numToChar(pState->useBackgroundStack);

			// action
			(*response_string_out)[9] = numToChar(pState->action);

			// element type
			(*response_string_out)[10] = numToChar(pState->elementType);

			bufferSize = 11;
			uint16_t j = 0;

			if (pState->data != NULL)
			{
				while (pState->data[j] != '\0')
				{
					(*response_string_out)[bufferSize] = pState->data[j];
					bufferSize++;
					j++;
				}
			}
			(*response_string_out)[bufferSize] = '\r';
		}
	}
}

uint8_t isTimeInFrame(PTIME_FRAME_STATE timeFrame, uint8_t curTimeHour, uint8_t curTimeMinute)
{
	/*
		if the start-hour is below the end-hour, the frame ranges within one day (now overflow to the next day)
	*/
	if (timeFrame->start_hour < timeFrame->end_hour)
	{
		if ((curTimeHour > timeFrame->start_hour) && (curTimeHour < timeFrame->end_hour))
		{
			/*
				The current hour is definitely in the frame range, no need to check for the minutes
			*/
			return TIMEFRAME_IS_IN_SCOPE;
		}
		else if (curTimeHour == timeFrame->start_hour)
		{
			/*
				The current hour equals the start-hour, so the minutes decide
			*/
			if (curTimeMinute >= timeFrame->start_minute)
			{
				/*
					When the start-minute equals or matches, the time must be in frame
				*/
				return TIMEFRAME_IS_IN_SCOPE;
			}
			else
			{
				/*
					Otherwise the current minute is below the start-minute -> time not in frame
				*/
				return TIMEFRAME_NO_CONSENT;
			}
		}
		else if (curTimeHour == timeFrame->end_hour)
		{
			/*
				The current hour equals the end-hour so the minutes decide
			*/
			if (curTimeMinute < timeFrame->end_minute)
			{
				/*
					When the current minute is below the end-minute the time must be in frame
				*/
				return TIMEFRAME_IS_IN_SCOPE;
			}
			else
			{
				/*
					Otherwise the current minute is above or equal the end-minute, so the time is not in frame
				*/
				return TIMEFRAME_NO_CONSENT;
			}
		}
		else
		{
			/*
				The current hour is definitely not in the frame range
			*/
			return TIMEFRAME_NO_CONSENT;
		}
	}
	/*
		If the start-hour is higher than the end-hour, the frame ranges from one day to another
			NOTE: except if the end-hour is midnight (0)
	*/
	else if (timeFrame->start_hour > timeFrame->end_hour)
	{
		/*
			The end hour is above 0 (midnight)
		*/
		if ((curTimeHour > timeFrame->start_hour) || (curTimeHour < timeFrame->end_hour))
		{
			/*
				The current time is definitely in frame
			*/
			return TIMEFRAME_IS_IN_SCOPE;
		}
		else if (curTimeHour == timeFrame->start_hour)
		{
			/*
				If the the start-hour equals the current hour, the minutes decide
			*/
			if (curTimeMinute >= timeFrame->start_minute)
			{
				/*
					When the start-minute equals or matches, the time is in frame
				*/
				return TIMEFRAME_IS_IN_SCOPE;
			}
			else
			{
				/*
					Otherwise the time is not in frame
				*/
				return TIMEFRAME_NO_CONSENT;
			}
		}
		else if (curTimeHour == timeFrame->end_hour)
		{
			/*
				If the end-hour equals the current hour, the minutes decide
			*/
			if (curTimeMinute < timeFrame->end_minute)
			{
				/*
					When the current time minute is lower than the end-minute, the time is in frame
				*/
				return TIMEFRAME_IS_IN_SCOPE;
			}
			else
			{
				/*
					Otherwise the time is not in frame
				*/
				return TIMEFRAME_NO_CONSENT;
			}
		}
		else
		{
			/*
				In all other cases the time is not in frame
			*/
			return TIMEFRAME_NO_CONSENT;
		}
	}
	/*
		If the start-hour equals the end-hour the frame is in the minute range
	*/
	else if (timeFrame->start_hour == timeFrame->end_hour)// && (curTimeHour == timeFrame->start_hour))
	{
		if (timeFrame->start_minute < timeFrame->end_minute)
		{
			/*
				When the start-minute is lower than the end-minute, the frame is minute-based within one hour
				-> if the hour is different, the time cannot be in frame
			*/
			if ((curTimeMinute >= timeFrame->start_minute) && (curTimeMinute < timeFrame->end_minute) && (curTimeHour == timeFrame->start_hour))
			{
				// -> in frame
				return TIMEFRAME_IS_IN_SCOPE;
			}
			else
			{
				// not in frame
				return TIMEFRAME_NO_CONSENT;
			}
		}
		else if (timeFrame->start_minute > timeFrame->end_minute)
		{
			/*
				When the start-minute is higher than the end-minute, the minutes decide, but the frame is within on day
			*/
			if (curTimeHour != timeFrame->start_hour)
			{
				/*
					If the current time hour is different to the start-hour (and to the end-hour, since there are equal)
					The time must be in frame (because the frame is within one day)
				*/
				return TIMEFRAME_IS_IN_SCOPE;
			}
			else
			{
				/*
					Here the current hour equals the start and end hour, so the minutes decide
				*/
				if ((curTimeMinute >= timeFrame->start_minute) || (curTimeMinute < timeFrame->end_minute))
				{
					/*
						This is the part of the hour when the time matches, so it is in frame
					*/
					return TIMEFRAME_IS_IN_SCOPE;
				}
				else
				{
					/*
						This is the short part of the hour (within one day) where the time is NOT in frame
					*/
					return TIMEFRAME_NO_CONSENT;
				}
			}
		}
	}
	return TIMEFRAME_NO_CONSENT;
}

const char* getLanguage()
{
	return languageIdentifier;
}


void addRGBSelectorStateForProperty(uint8_t propertyID, PRGB_SELECTOR_STATE pState)
{
	PDEVICE_PROPERTY prop = findProperty(propertyID, NULL);
	if (prop != NULL)
	{
		if (prop->type == PTYPE_RGB_SELECTOR)
		{
			if (rgbSelectorStateCount == 0)
			{
				rgbSelectorStateList = (PRGB_SELECTOR_STATE*)malloc(sizeof(PRGB_SELECTOR_STATE));
				if (rgbSelectorStateList != NULL)
				{
					pState->associatedPropertyID = propertyID;
					rgbSelectorStateList[0] = pState;
					rgbSelectorStateCount++;
				}
			}
			else
			{
				if (rgbSelectorStateCount < 255)
				{
					PRGB_SELECTOR_STATE* pHolder = rgbSelectorStateList;

					uint8_t newPCount = rgbSelectorStateCount + 1;
					size_t len = newPCount;
					len *= sizeof(PRGB_SELECTOR_STATE);

					rgbSelectorStateList = (PRGB_SELECTOR_STATE*)malloc(len);
					if (rgbSelectorStateList != NULL)
					{
						memset(rgbSelectorStateList, 0, len);

						for (uint8_t i = 0; i < rgbSelectorStateCount; i++)
						{
							rgbSelectorStateList[i] = pHolder[i];
						}
						pState->associatedPropertyID = propertyID;
						rgbSelectorStateList[rgbSelectorStateCount] = pState;
						rgbSelectorStateCount++;
					}
					free(pHolder);
				}
			}
		}
	}
}

bool removeRGBSelectorStateForProperty(uint8_t propertyID)
{
	if (rgbSelectorStateList != NULL)
	{
		for (uint8_t i = 0; i < rgbSelectorStateCount; i++)
		{
			if (rgbSelectorStateList[i]->associatedPropertyID == propertyID)
			{
				if (rgbSelectorStateCount == 1)
				{
					free(rgbSelectorStateList);
					rgbSelectorStateCount = 0;
				}
				else
				{
					PRGB_SELECTOR_STATE* pHolder = rgbSelectorStateList;
					uint8_t newPCount = rgbSelectorStateCount - 1;
					size_t len = newPCount;
					len *= sizeof(PRGB_SELECTOR_STATE);

					rgbSelectorStateList = (PRGB_SELECTOR_STATE*)malloc(len);

					if (rgbSelectorStateList != NULL)
					{
						memset(rgbSelectorStateList, 0, len);
						uint8_t targetCounter = 0;

						for (uint8_t i = 0; i < rgbSelectorStateCount; i++)
						{
							if (pHolder[i]->associatedPropertyID != propertyID)
							{
								rgbSelectorStateList[targetCounter] = pHolder[i];
								targetCounter++;
							}
						}
						rgbSelectorStateCount--;
					}
				}
				return true;
			}
		}
	}
	return false;
}

PRGB_SELECTOR_STATE getRGBSelectorState(uint8_t propertyID)
{
	if (rgbSelectorStateList != NULL)
	{
		for (uint8_t i = 0; i < rgbSelectorStateCount; i++)
		{
			if (rgbSelectorStateList[i]->associatedPropertyID == propertyID)
			{
				return rgbSelectorStateList[i];
			}
		}
	}
	return NULL;
}

void addExLevelSelectorStateForProperty(uint8_t propertyID, PEX_LEVEL_SELECTOR_STATE pState)
{
	PDEVICE_PROPERTY prop = findProperty(propertyID, NULL);
	if (prop != NULL)
	{
		if (prop->type == PTYPE_EX_LEVEL_SELECTOR)
		{
			if (exLevelSelectorStateCount == 0)
			{
				exLevelSelectorStateList = (PEX_LEVEL_SELECTOR_STATE*)malloc(sizeof(PEX_LEVEL_SELECTOR_STATE));
				if (exLevelSelectorStateList != NULL)
				{
					pState->associatedPropertyID = propertyID;
					exLevelSelectorStateList[0] = pState;
					exLevelSelectorStateCount++;
				}
			}
			else
			{
				if (exLevelSelectorStateCount < 255)
				{
					PEX_LEVEL_SELECTOR_STATE* pHolder = exLevelSelectorStateList;

					uint8_t newPCount = exLevelSelectorStateCount + 1;
					size_t len = newPCount;
					len *= sizeof(PEX_LEVEL_SELECTOR_STATE);

					exLevelSelectorStateList = (PEX_LEVEL_SELECTOR_STATE*)malloc(len);
					if (exLevelSelectorStateList != NULL)
					{
						memset(exLevelSelectorStateList, 0, len);

						for (uint8_t i = 0; i < exLevelSelectorStateCount; i++)
						{
							exLevelSelectorStateList[i] = pHolder[i];
						}
						pState->associatedPropertyID = propertyID;
						exLevelSelectorStateList[exLevelSelectorStateCount] = pState;
						exLevelSelectorStateCount++;
					}
					free(pHolder);
				}
			}
		}
	}
}

bool removeExLevelSelectorStateForProperty(uint8_t propertyID)
{
	if (exLevelSelectorStateList != NULL)
	{
		for (uint8_t i = 0; i < exLevelSelectorStateCount; i++)
		{
			if (exLevelSelectorStateList[i]->associatedPropertyID == propertyID)
			{
				if (exLevelSelectorStateCount == 1)
				{
					free(exLevelSelectorStateList);
					exLevelSelectorStateCount = 0;
				}
				else
				{
					PEX_LEVEL_SELECTOR_STATE* pHolder = exLevelSelectorStateList;
					uint8_t newPCount = exLevelSelectorStateCount - 1;
					size_t len = newPCount;
					len *= sizeof(PEX_LEVEL_SELECTOR_STATE);

					exLevelSelectorStateList = (PEX_LEVEL_SELECTOR_STATE*)malloc(len);

					if (exLevelSelectorStateList != NULL)
					{
						memset(exLevelSelectorStateList, 0, len);
						uint8_t targetCounter = 0;

						for (uint8_t i = 0; i < exLevelSelectorStateCount; i++)
						{
							if (pHolder[i]->associatedPropertyID != propertyID)
							{
								exLevelSelectorStateList[targetCounter] = pHolder[i];
								targetCounter++;
							}
						}
						exLevelSelectorStateCount--;
					}
				}
				return true;
			}
		}
	}
	return false;
}

PEX_LEVEL_SELECTOR_STATE getExLevelSelectorState(uint8_t propertyID)
{
	if (exLevelSelectorStateList != NULL)
	{
		for (uint8_t i = 0; i < exLevelSelectorStateCount; i++)
		{
			if (exLevelSelectorStateList[i]->associatedPropertyID == propertyID)
			{
				return exLevelSelectorStateList[i];
			}
		}
	}
	return NULL;
}

void addTimeSelectorStateForProperty(uint8_t propertyID, PTIME_SELECTOR_STATE pState)
{
	PDEVICE_PROPERTY prop = findProperty(propertyID, NULL);
	if (prop != NULL)
	{
		if (prop->type == PTYPE_TIME_SELECTOR)
		{
			if (timeSelectorStateCount == 0)
			{
				timeSelectorStateList = (PTIME_SELECTOR_STATE*)malloc(sizeof(PTIME_SELECTOR_STATE));
				if (timeSelectorStateList != NULL)
				{
					pState->associatedPropertyID = propertyID;
					timeSelectorStateList[0] = pState;
					timeSelectorStateCount++;
				}
			}
			else
			{
				if (timeSelectorStateCount < 255)
				{
					PTIME_SELECTOR_STATE* pHolder = timeSelectorStateList;

					uint8_t newPCount = timeSelectorStateCount + 1;
					size_t len = newPCount;
					len *= sizeof(PTIME_SELECTOR_STATE);

					timeSelectorStateList = (PTIME_SELECTOR_STATE*)malloc(len);
					if (timeSelectorStateList != NULL)
					{
						memset(timeSelectorStateList, 0, len);

						for (uint8_t i = 0; i < timeSelectorStateCount; i++)
						{
							timeSelectorStateList[i] = pHolder[i];
						}
						pState->associatedPropertyID = propertyID;
						timeSelectorStateList[timeSelectorStateCount] = pState;
						timeSelectorStateCount++;
					}
					free(pHolder);
				}
			}
		}
	}
}

bool removeTimeSelectorStateForProperty(uint8_t propertyID)
{
	if (timeSelectorStateList != NULL)
	{
		for (uint8_t i = 0; i < timeSelectorStateCount; i++)
		{
			if (timeSelectorStateList[i]->associatedPropertyID == propertyID)
			{
				if (timeSelectorStateCount == 1)
				{
					free(timeSelectorStateList);
					timeSelectorStateCount = 0;
				}
				else
				{
					PTIME_SELECTOR_STATE* pHolder = timeSelectorStateList;
					uint8_t newPCount = timeSelectorStateCount - 1;
					size_t len = newPCount;
					len *= sizeof(PTIME_SELECTOR_STATE);

					timeSelectorStateList = (PTIME_SELECTOR_STATE*)malloc(len);

					if (timeSelectorStateList != NULL)
					{
						memset(timeSelectorStateList, 0, len);
						uint8_t targetCounter = 0;

						for (uint8_t i = 0; i < timeSelectorStateCount; i++)
						{
							if (pHolder[i]->associatedPropertyID != propertyID)
							{
								timeSelectorStateList[targetCounter] = pHolder[i];
								targetCounter++;
							}
						}
						timeSelectorStateCount--;
					}
				}
				return true;
			}
		}
	}
	return false;
}

PTIME_SELECTOR_STATE getTimeSelectorState(uint8_t propertyID)
{
	if (timeSelectorStateList != NULL)
	{
		for (uint8_t i = 0; i < timeSelectorStateCount; i++)
		{
			if (timeSelectorStateList[i]->associatedPropertyID == propertyID)
			{
				return timeSelectorStateList[i];
			}
		}
	}
	return NULL;
}

void addTimeFrameSelectorStateForProperty(uint8_t propertyID, PTIME_FRAME_STATE pState)
{
	PDEVICE_PROPERTY prop = findProperty(propertyID, NULL);
	if (prop != NULL)
	{
		if (prop->type == PTYPE_TIME_FRAME_SELECTOR)
		{
			if (timeFrameSelectorStateCount == 0)
			{
				timeFrameSelectorStateList = (PTIME_FRAME_STATE*)malloc(sizeof(PTIME_FRAME_STATE));
				if (timeFrameSelectorStateList != NULL)
				{
					pState->associatedPropertyID = propertyID;
					timeFrameSelectorStateList[0] = pState;
					timeFrameSelectorStateCount++;
				}
			}
			else
			{
				if (timeFrameSelectorStateCount < 255)
				{
					PTIME_FRAME_STATE* pHolder = timeFrameSelectorStateList;

					uint8_t newPCount = timeFrameSelectorStateCount + 1;
					size_t len = newPCount;
					len *= sizeof(PTIME_FRAME_STATE);

					timeFrameSelectorStateList = (PTIME_FRAME_STATE*)malloc(len);
					if (timeFrameSelectorStateList != NULL)
					{
						memset(timeFrameSelectorStateList, 0, len);

						for (uint8_t i = 0; i < timeFrameSelectorStateCount; i++)
						{
							timeFrameSelectorStateList[i] = pHolder[i];
						}
						pState->associatedPropertyID = propertyID;
						timeFrameSelectorStateList[timeFrameSelectorStateCount] = pState;
						timeFrameSelectorStateCount++;
					}
					free(pHolder);
				}
			}
		}
	}
}

bool removeTimeFrameSelectorStateForProperty(uint8_t propertyID)
{
	if (timeFrameSelectorStateList != NULL)
	{
		for (uint8_t i = 0; i < timeFrameSelectorStateCount; i++)
		{
			if (timeFrameSelectorStateList[i]->associatedPropertyID == propertyID)
			{
				if (timeFrameSelectorStateCount == 1)
				{
					free(timeFrameSelectorStateList);
					timeFrameSelectorStateCount = 0;
				}
				else
				{
					PTIME_FRAME_STATE* pHolder = timeFrameSelectorStateList;
					uint8_t newPCount = timeFrameSelectorStateCount - 1;
					size_t len = newPCount;
					len *= sizeof(PTIME_FRAME_STATE);

					timeFrameSelectorStateList = (PTIME_FRAME_STATE*)malloc(len);

					if (timeFrameSelectorStateList != NULL)
					{
						memset(timeFrameSelectorStateList, 0, len);
						uint8_t targetCounter = 0;

						for (uint8_t i = 0; i < timeFrameSelectorStateCount; i++)
						{
							if (pHolder[i]->associatedPropertyID != propertyID)
							{
								timeFrameSelectorStateList[targetCounter] = pHolder[i];
								targetCounter++;
							}
						}
						timeFrameSelectorStateCount--;
					}
				}
				return true;
			}
		}
	}
	return false;
}

PTIME_FRAME_STATE getTimeFrameSelectorState(uint8_t propertyID)
{
	if (timeFrameSelectorStateList != NULL)
	{
		for (uint8_t i = 0; i < timeFrameSelectorStateCount; i++)
		{
			if (timeFrameSelectorStateList[i]->associatedPropertyID == propertyID)
			{
				return timeFrameSelectorStateList[i];
			}
		}
	}
	return NULL;
}

void addDateSelectorStateForProperty(uint8_t propertyID, PDATE_SELECTOR_STATE pState)
{
	PDEVICE_PROPERTY prop = findProperty(propertyID, NULL);
	if (prop != NULL)
	{
		if (prop->type == PTYPE_DATE_SELECTOR)
		{
			if (dateSelectorStateCount == 0)
			{
				dateSelectorStateList = (PDATE_SELECTOR_STATE*)malloc(sizeof(PDATE_SELECTOR_STATE));
				if (dateSelectorStateList != NULL)
				{
					pState->associatedPropertyID = propertyID;
					dateSelectorStateList[0] = pState;
					dateSelectorStateCount++;
				}
			}
			else
			{
				if (dateSelectorStateCount < 255)
				{
					PDATE_SELECTOR_STATE* pHolder = dateSelectorStateList;

					uint8_t newPCount = dateSelectorStateCount + 1;
					size_t len = newPCount;
					len *= sizeof(PDATE_SELECTOR_STATE);

					dateSelectorStateList = (PDATE_SELECTOR_STATE*)malloc(len);
					if (dateSelectorStateList != NULL)
					{
						memset(dateSelectorStateList, 0, len);

						for (uint8_t i = 0; i < dateSelectorStateCount; i++)
						{
							dateSelectorStateList[i] = pHolder[i];
						}
						pState->associatedPropertyID = propertyID;
						dateSelectorStateList[dateSelectorStateCount] = pState;
						dateSelectorStateCount++;
					}
					free(pHolder);
				}
			}
		}
	}
}

bool removeDateSelectorStateForProperty(uint8_t propertyID)
{
	if (dateSelectorStateList != NULL)
	{
		for (uint8_t i = 0; i < dateSelectorStateCount; i++)
		{
			if (dateSelectorStateList[i]->associatedPropertyID == propertyID)
			{
				if (dateSelectorStateCount == 1)
				{
					free(dateSelectorStateList);
					dateSelectorStateCount = 0;
				}
				else
				{
					PDATE_SELECTOR_STATE* pHolder = dateSelectorStateList;
					uint8_t newPCount = dateSelectorStateCount - 1;
					size_t len = newPCount;
					len *= sizeof(PDATE_SELECTOR_STATE);

					dateSelectorStateList = (PDATE_SELECTOR_STATE*)malloc(len);

					if (dateSelectorStateList != NULL)
					{
						memset(dateSelectorStateList, 0, len);
						uint8_t targetCounter = 0;

						for (uint8_t i = 0; i < dateSelectorStateCount; i++)
						{
							if (pHolder[i]->associatedPropertyID != propertyID)
							{
								dateSelectorStateList[targetCounter] = pHolder[i];
								targetCounter++;
							}
						}
						dateSelectorStateCount--;
					}
				}
				return true;
			}
		}
	}
	return false;
}

PDATE_SELECTOR_STATE getDateSelectorState(uint8_t propertyID)
{
	if (dateSelectorStateList != NULL)
	{
		for (uint8_t i = 0; i < dateSelectorStateCount; i++)
		{
			if (dateSelectorStateList[i]->associatedPropertyID == propertyID)
			{
				return dateSelectorStateList[i];
			}
		}
	}
	return NULL;
}

void addUnlockControlStateForProperty(uint8_t propertyID, PUNLOCK_CONTROL_STATE pState)
{
	PDEVICE_PROPERTY prop = findProperty(propertyID, NULL);
	if (prop != NULL)
	{
		if (prop->type == PTYPE_UNLOCK_CONTROL)
		{
			if (unlockControlStateCount == 0)
			{
				unlockControlStateList = (PUNLOCK_CONTROL_STATE*)malloc(sizeof(PUNLOCK_CONTROL_STATE));
				if (unlockControlStateList != NULL)
				{
					pState->associatedPropertyID = propertyID;
					unlockControlStateList[0] = pState;
					unlockControlStateCount++;
				}
			}
			else
			{
				if (unlockControlStateCount < 255)
				{
					PUNLOCK_CONTROL_STATE* pHolder = unlockControlStateList;

					uint8_t newPCount = unlockControlStateCount + 1;
					size_t len = newPCount;
					len *= sizeof(PUNLOCK_CONTROL_STATE);

					unlockControlStateList = (PUNLOCK_CONTROL_STATE*)malloc(len);
					if (unlockControlStateList != NULL)
					{
						memset(unlockControlStateList, 0, len);

						for (uint8_t i = 0; i < unlockControlStateCount; i++)
						{
							unlockControlStateList[i] = pHolder[i];
						}
						pState->associatedPropertyID = propertyID;
						unlockControlStateList[unlockControlStateCount] = pState;
						unlockControlStateCount++;
					}
					free(pHolder);
				}
			}
		}
	}
}

bool removeUnlockControlStateForProperty(uint8_t propertyID)
{
	if (unlockControlStateList != NULL)
	{
		for (uint8_t i = 0; i < unlockControlStateCount; i++)
		{
			if (unlockControlStateList[i]->associatedPropertyID == propertyID)
			{
				if (unlockControlStateCount == 1)
				{
					free(unlockControlStateList);
					unlockControlStateCount = 0;
				}
				else
				{
					PUNLOCK_CONTROL_STATE* pHolder = unlockControlStateList;
					uint8_t newPCount = unlockControlStateCount - 1;
					size_t len = newPCount;
					len *= sizeof(PUNLOCK_CONTROL_STATE);

					unlockControlStateList = (PUNLOCK_CONTROL_STATE*)malloc(len);

					if (unlockControlStateList != NULL)
					{
						memset(unlockControlStateList, 0, len);
						uint8_t targetCounter = 0;

						for (uint8_t i = 0; i < unlockControlStateCount; i++)
						{
							if (pHolder[i]->associatedPropertyID != propertyID)
							{
								unlockControlStateList[targetCounter] = pHolder[i];
								targetCounter++;
							}
						}
						unlockControlStateCount--;
					}
				}
				return true;
			}
		}
	}
	return false;
}

PUNLOCK_CONTROL_STATE getUnlockControlState(uint8_t propertyID)
{
	if (unlockControlStateList != NULL)
	{
		for (uint8_t i = 0; i < unlockControlStateCount; i++)
		{
			if (unlockControlStateList[i]->associatedPropertyID == propertyID)
			{
				return unlockControlStateList[i];
			}
		}
	}
	return NULL;
}

void addNavigatorControlStateForProperty(uint8_t propertyID, PNAVIGATOR_STATE pState)
{
	PDEVICE_PROPERTY prop = findProperty(propertyID, NULL);
	if (prop != NULL)
	{
		if (prop->type == PTYPE_NAVIGATOR_CONTROL)
		{
			if (navigatorControlStateCount == 0)
			{
				navigatorControlStateList = (PNAVIGATOR_STATE*)malloc(sizeof(PNAVIGATOR_STATE));
				if (navigatorControlStateList != NULL)
				{
					pState->associatedPropertyID = propertyID;
					navigatorControlStateList[0] = pState;
					navigatorControlStateCount++;
				}
			}
			else
			{
				if (navigatorControlStateCount < 255)
				{
					PNAVIGATOR_STATE* pHolder = navigatorControlStateList;

					uint8_t newPCount = navigatorControlStateCount + 1;
					size_t len = newPCount;
					len *= sizeof(PNAVIGATOR_STATE);

					navigatorControlStateList = (PNAVIGATOR_STATE*)malloc(len);
					if (navigatorControlStateList != NULL)
					{
						memset(navigatorControlStateList, 0, len);

						for (uint8_t i = 0; i < navigatorControlStateCount; i++)
						{
							navigatorControlStateList[i] = pHolder[i];
						}
						pState->associatedPropertyID = propertyID;
						navigatorControlStateList[navigatorControlStateCount] = pState;
						navigatorControlStateCount++;
					}
					free(pHolder);
				}
			}
		}
	}
}

bool removeNavigatorControlStateForProperty(uint8_t propertyID)
{
	if (navigatorControlStateList != NULL)
	{
		for (uint8_t i = 0; i < navigatorControlStateCount; i++)
		{
			if (navigatorControlStateList[i]->associatedPropertyID == propertyID)
			{
				if (navigatorControlStateCount == 1)
				{
					free(navigatorControlStateList);
					navigatorControlStateCount = 0;
				}
				else
				{
					PNAVIGATOR_STATE* pHolder = navigatorControlStateList;
					uint8_t newPCount = navigatorControlStateCount - 1;
					size_t len = newPCount;
					len *= sizeof(PNAVIGATOR_STATE);

					navigatorControlStateList = (PNAVIGATOR_STATE*)malloc(len);

					if (navigatorControlStateList != NULL)
					{
						memset(navigatorControlStateList, 0, len);
						uint8_t targetCounter = 0;

						for (uint8_t i = 0; i < navigatorControlStateCount; i++)
						{
							if (pHolder[i]->associatedPropertyID != propertyID)
							{
								navigatorControlStateList[targetCounter] = pHolder[i];
								targetCounter++;
							}
						}
						navigatorControlStateCount--;
					}
				}
				return true;
			}
		}
	}
	return false;
}

PNAVIGATOR_STATE getNavigatorControlState(uint8_t propertyID)
{
	if (navigatorControlStateList != NULL)
	{
		for (uint8_t i = 0; i < navigatorControlStateCount; i++)
		{
			if (navigatorControlStateList[i]->associatedPropertyID == propertyID)
			{
				return navigatorControlStateList[i];
			}
		}
	}
	return NULL;
}

void addBarGraphStateForProperty(uint8_t propertyID, PBAR_GRAPH_STATE pState)
{
	PDEVICE_PROPERTY prop = findProperty(propertyID, NULL);
	if (prop != NULL)
	{
		if (prop->type == PTYPE_BAR_GRAPH)
		{
			if (barGraphStateCount == 0)
			{
				barGraphStateList = (PBAR_GRAPH_STATE*)malloc(sizeof(PBAR_GRAPH_STATE));
				if (barGraphStateList != NULL)
				{
					pState->associatedPropertyID = propertyID;
					barGraphStateList[0] = pState;
					barGraphStateCount++;
				}
			}
			else
			{
				if (barGraphStateCount < 255)
				{
					PBAR_GRAPH_STATE* pHolder = barGraphStateList;

					uint8_t newPCount = barGraphStateCount + 1;
					size_t len = newPCount;
					len *= sizeof(PBAR_GRAPH_STATE);

					barGraphStateList = (PBAR_GRAPH_STATE*)malloc(len);
					if (barGraphStateList != NULL)
					{
						memset(barGraphStateList, 0, len);

						for (uint8_t i = 0; i < barGraphStateCount; i++)
						{
							barGraphStateList[i] = pHolder[i];
						}
						pState->associatedPropertyID = propertyID;
						barGraphStateList[barGraphStateCount] = pState;
						barGraphStateCount++;
					}
					free(pHolder);
				}
			}
		}
	}
}

bool removeBarGraphStateForProperty(uint8_t propertyID)
{
	if (barGraphStateList != NULL)
	{
		for (uint8_t i = 0; i < barGraphStateCount; i++)
		{
			if (barGraphStateList[i]->associatedPropertyID == propertyID)
			{
				if (barGraphStateList[i]->data != NULL)
				{
					free(barGraphStateList[i]->data);
				}

				if (barGraphStateCount == 1)
				{
					free(barGraphStateList);
					barGraphStateCount = 0;
				}
				else
				{
					PBAR_GRAPH_STATE* pHolder = barGraphStateList;
					uint8_t newPCount = barGraphStateCount - 1;
					size_t len = newPCount;
					len *= sizeof(PBAR_GRAPH_STATE);

					barGraphStateList = (PBAR_GRAPH_STATE*)malloc(len);

					if (barGraphStateList != NULL)
					{
						memset(barGraphStateList, 0, len);
						uint8_t targetCounter = 0;

						for (uint8_t i = 0; i < barGraphStateCount; i++)
						{
							if (pHolder[i]->associatedPropertyID != propertyID)
							{
								barGraphStateList[targetCounter] = pHolder[i];
								targetCounter++;
							}
						}
						barGraphStateCount--;
					}
				}
				return true;
			}
		}
	}
	return false;
}

PBAR_GRAPH_STATE getBarGraphState(uint8_t propertyID)
{
	if (barGraphStateList != NULL)
	{
		for (uint8_t i = 0; i < barGraphStateCount; i++)
		{
			if (barGraphStateList[i]->associatedPropertyID == propertyID)
			{
				return barGraphStateList[i];
			}
		}
	}
	return NULL;
}

void addLineGraphStateForProperty(uint8_t propertyID, PLINE_GRAPH_STATE pState)
{
	PDEVICE_PROPERTY prop = findProperty(propertyID, NULL);
	if (prop != NULL)
	{
		if (prop->type == PTYPE_LINE_GRAPH)
		{
			if (lineGraphStateCount == 0)
			{
				lineGraphStateList = (PLINE_GRAPH_STATE*)malloc(sizeof(PLINE_GRAPH_STATE));
				if (lineGraphStateList != NULL)
				{
					pState->associatedPropertyID = propertyID;
					lineGraphStateList[0] = pState;
					lineGraphStateCount++;
				}
			}
			else
			{
				if (lineGraphStateCount < 255)
				{
					PLINE_GRAPH_STATE* pHolder = lineGraphStateList;

					uint8_t newPCount = lineGraphStateCount + 1;
					size_t len = newPCount;
					len *= sizeof(PLINE_GRAPH_STATE);

					lineGraphStateList = (PLINE_GRAPH_STATE*)malloc(len);
					if (lineGraphStateList != NULL)
					{
						memset(lineGraphStateList, 0, len);

						for (uint8_t i = 0; i < lineGraphStateCount; i++)
						{
							lineGraphStateList[i] = pHolder[i];
						}
						pState->associatedPropertyID = propertyID;
						lineGraphStateList[lineGraphStateCount] = pState;
						lineGraphStateCount++;
					}
					free(pHolder);
				}
			}
		}
	}
}

bool removeLineGraphStateForProperty(uint8_t propertyID)
{
	if (lineGraphStateList != NULL)
	{
		for (uint8_t i = 0; i < lineGraphStateCount; i++)
		{
			if (lineGraphStateList[i]->associatedPropertyID == propertyID)
			{
				if (lineGraphStateList[i]->data != NULL)
				{
					free(lineGraphStateList[i]->data);
				}

				if (lineGraphStateCount == 1)
				{
					free(lineGraphStateList);
					lineGraphStateCount = 0;
				}
				else
				{
					PLINE_GRAPH_STATE* pHolder = lineGraphStateList;
					uint8_t newPCount = lineGraphStateCount - 1;
					size_t len = newPCount;
					len *= sizeof(PLINE_GRAPH_STATE);

					lineGraphStateList = (PLINE_GRAPH_STATE*)malloc(len);

					if (lineGraphStateList != NULL)
					{
						memset(lineGraphStateList, 0, len);
						uint8_t targetCounter = 0;

						for (uint8_t i = 0; i < lineGraphStateCount; i++)
						{
							if (pHolder[i]->associatedPropertyID != propertyID)
							{
								lineGraphStateList[targetCounter] = pHolder[i];
								targetCounter++;
							}
						}
						lineGraphStateCount--;
					}
				}
				return true;
			}
		}
	}
	return false;
}

PLINE_GRAPH_STATE getLineGraphState(uint8_t propertyID)
{
	if (lineGraphStateList != NULL)
	{
		for (uint8_t i = 0; i < lineGraphStateCount; i++)
		{
			if (lineGraphStateList[i]->associatedPropertyID == propertyID)
			{
				return lineGraphStateList[i];
			}
		}
	}
	return NULL;
}

void addStringInterrogatorStateForProperty(uint8_t propertyID, PSTRING_INTERROGATOR_STATE pState)
{
	PDEVICE_PROPERTY prop = findProperty(propertyID, NULL);
	if (prop != NULL)
	{
		if (prop->type == PTYPE_STRING_INTERROGATOR)
		{
			if (stringInterrogatorStateCount == 0)
			{
				stringInterrogatorStateList = (PSTRING_INTERROGATOR_STATE*)malloc(sizeof(PSTRING_INTERROGATOR_STATE));
				if (stringInterrogatorStateList != NULL)
				{
					pState->associatedPropertyID = propertyID;
					stringInterrogatorStateList[0] = pState;
					stringInterrogatorStateCount++;
				}
			}
			else
			{
				if (stringInterrogatorStateCount < 255)
				{
					PSTRING_INTERROGATOR_STATE* pHolder = stringInterrogatorStateList;

					uint8_t newPCount = stringInterrogatorStateCount + 1;
					size_t len = newPCount;
					len *= sizeof(PSTRING_INTERROGATOR_STATE);

					stringInterrogatorStateList = (PSTRING_INTERROGATOR_STATE*)malloc(len);
					if (stringInterrogatorStateList != NULL)
					{
						memset(stringInterrogatorStateList, 0, len);

						for (uint8_t i = 0; i < stringInterrogatorStateCount; i++)
						{
							stringInterrogatorStateList[i] = pHolder[i];
						}
						pState->associatedPropertyID = propertyID;
						stringInterrogatorStateList[stringInterrogatorStateCount] = pState;
						stringInterrogatorStateCount++;
					}
					free(pHolder);
				}
			}
		}
	}
}

bool removeStringInterrogatorStateForProperty(uint8_t propertyID)
{
	if (stringInterrogatorStateList != NULL)
	{
		for (uint8_t i = 0; i < stringInterrogatorStateCount; i++)
		{
			if (stringInterrogatorStateList[i]->associatedPropertyID == propertyID)
			{
				if (stringInterrogatorStateList[i]->data != NULL)
				{
					free(stringInterrogatorStateList[i]->data);
				}

				if (stringInterrogatorStateCount == 1)
				{
					free(stringInterrogatorStateList);
					stringInterrogatorStateCount = 0;
				}
				else
				{
					PSTRING_INTERROGATOR_STATE* pHolder = stringInterrogatorStateList;
					uint8_t newPCount = stringInterrogatorStateCount - 1;
					size_t len = newPCount;
					len *= sizeof(PSTRING_INTERROGATOR_STATE);

					stringInterrogatorStateList = (PSTRING_INTERROGATOR_STATE*)malloc(len);

					if (stringInterrogatorStateList != NULL)
					{
						memset(stringInterrogatorStateList, 0, len);
						uint8_t targetCounter = 0;

						for (uint8_t i = 0; i < stringInterrogatorStateCount; i++)
						{
							if (pHolder[i]->associatedPropertyID != propertyID)
							{
								stringInterrogatorStateList[targetCounter] = pHolder[i];
								targetCounter++;
							}
						}
						stringInterrogatorStateCount--;
					}
				}
				return true;
			}
		}
	}
	return false;
}

PSTRING_INTERROGATOR_STATE getStringInterrogatorState(uint8_t propertyID)
{
	if (stringInterrogatorStateList != NULL)
	{
		for (uint8_t i = 0; i < stringInterrogatorStateCount; i++)
		{
			if (stringInterrogatorStateList[i]->associatedPropertyID == propertyID)
			{
				return stringInterrogatorStateList[i];
			}
		}
	}
	return NULL;
}

void addTextListPresenterStateForProperty(uint8_t propertyID, PTEXT_LIST_PRESENTER_STATE pState)
{
	PDEVICE_PROPERTY prop = findProperty(propertyID, NULL);
	if (prop != NULL)
	{
		if (prop->type == PTYPE_TEXT_LIST_PRESENTER)
		{
			if (textListPresenterStateCount == 0)
			{
				textListPresenterStateList = (PTEXT_LIST_PRESENTER_STATE*)malloc(sizeof(PTEXT_LIST_PRESENTER_STATE));
				if (textListPresenterStateList != NULL)
				{
					pState->associatedPropertyID = propertyID;
					textListPresenterStateList[0] = pState;
					textListPresenterStateCount++;
				}
			}
			else
			{
				if (textListPresenterStateCount < 255)
				{
					PTEXT_LIST_PRESENTER_STATE* pHolder = textListPresenterStateList;

					uint8_t newPCount = textListPresenterStateCount + 1;
					size_t len = newPCount;
					len *= sizeof(PTEXT_LIST_PRESENTER_STATE);

					textListPresenterStateList = (PTEXT_LIST_PRESENTER_STATE*)malloc(len);
					if (textListPresenterStateList != NULL)
					{
						memset(textListPresenterStateList, 0, len);

						for (uint8_t i = 0; i < textListPresenterStateCount; i++)
						{
							textListPresenterStateList[i] = pHolder[i];
						}
						pState->associatedPropertyID = propertyID;
						textListPresenterStateList[textListPresenterStateCount] = pState;
						textListPresenterStateCount++;
					}
					free(pHolder);
				}
			}
		}
	}
}

bool removeTextListPresenterStateForProperty(uint8_t propertyID)
{
	if (textListPresenterStateList != NULL)
	{
		for (uint8_t i = 0; i < textListPresenterStateCount; i++)
		{
			if (textListPresenterStateList[i]->associatedPropertyID == propertyID)
			{
				if (textListPresenterStateList[i]->data != NULL)
				{
					free(textListPresenterStateList[i]->data);
				}

				if (textListPresenterStateCount == 1)
				{
					free(textListPresenterStateList);
					textListPresenterStateCount = 0;
				}
				else
				{
					PTEXT_LIST_PRESENTER_STATE* pHolder = textListPresenterStateList;
					uint8_t newPCount = textListPresenterStateCount - 1;
					size_t len = newPCount;
					len *= sizeof(PTEXT_LIST_PRESENTER_STATE);

					textListPresenterStateList = (PTEXT_LIST_PRESENTER_STATE*)malloc(len);

					if (textListPresenterStateList != NULL)
					{
						memset(textListPresenterStateList, 0, len);
						uint8_t targetCounter = 0;

						for (uint8_t i = 0; i < textListPresenterStateCount; i++)
						{
							if (pHolder[i]->associatedPropertyID != propertyID)
							{
								textListPresenterStateList[targetCounter] = pHolder[i];
								targetCounter++;
							}
						}
						textListPresenterStateCount--;
					}
				}
				return true;
			}
		}
	}
	return false;
}

PTEXT_LIST_PRESENTER_STATE getTextListPresenterState(uint8_t propertyID)
{
	if (textListPresenterStateList != NULL)
	{
		for (uint8_t i = 0; i < textListPresenterStateCount; i++)
		{
			if (textListPresenterStateList[i]->associatedPropertyID == propertyID)
			{
				return textListPresenterStateList[i];
			}
		}
	}
	return NULL;
}

void removeComplexStateForPropertyID(uint8_t propertyID, uint8_t type)
{
	switch (type)
	{
	case PTYPE_RGB_SELECTOR:
		removeRGBSelectorStateForProperty(propertyID);
		break;
	case PTYPE_EX_LEVEL_SELECTOR:
		removeExLevelSelectorStateForProperty(propertyID);
		break;
	case PTYPE_TIME_SELECTOR:
		removeTimeSelectorStateForProperty(propertyID);
		break;
	case PTYPE_TIME_FRAME_SELECTOR:
		removeTimeFrameSelectorStateForProperty(propertyID);
		break;
	case PTYPE_DATE_SELECTOR:
		removeDateSelectorStateForProperty(propertyID);
		break;
	case PTYPE_UNLOCK_CONTROL:
		removeUnlockControlStateForProperty(propertyID);
		break;
	case PTYPE_NAVIGATOR_CONTROL:
		removeNavigatorControlStateForProperty(propertyID);
		break;
	case PTYPE_BAR_GRAPH:
		removeBarGraphStateForProperty(propertyID);
		break;
	case PTYPE_LINE_GRAPH:
		removeLineGraphStateForProperty(propertyID);
		break;
	case PTYPE_STRING_INTERROGATOR:
		removeStringInterrogatorStateForProperty(propertyID);
		break;
	case PTYPE_TEXT_LIST_PRESENTER:
		removeTextListPresenterStateForProperty(propertyID);
		break;
	default:
		break;
	}
}

void onInitRequest()
{
	char* initStr = getInitString();
	if (initStr != NULL)
	{
		sendData(initStr);
		free(initStr);
	}
}

void onPropertyRequest(const char* data, uint8_t length)
{
	if (length >= 10)
	{
		uint8_t propertyIndex = x2CharHexValueTo8BitValue(data[2], data[3]);
			
		languageIdentifier[0] = data[8];
		languageIdentifier[1] = data[9];
		languageIdentifier[2] = '\0';

		char* propertyDefinition = getPropertyDefinitionString(propertyIndex, TST_RESPONSE, languageIdentifier);
		if (propertyDefinition != NULL)
		{
			sendData(propertyDefinition);
			free(propertyDefinition);
		}	
	}
}

void onGroupRequest(const char* data, uint8_t length)
{
	if (length >= 10)
	{
		uint8_t groupIndex = x2CharHexValueTo8BitValue(data[2], data[3]);
		languageIdentifier[0] = data[8];
		languageIdentifier[1] = data[9];
		languageIdentifier[2] = '\0';

		char* groupDefinition = getGroupDefinitionString(groupIndex, TST_RESPONSE, languageIdentifier);
		if (groupDefinition != NULL)
		{
			sendData(groupDefinition);
			free(groupDefinition);
		}	
	}
}

void onPropertyStateRequest(const char* data, uint8_t length)
{
	uint8_t propertyIndex =
		x2CharHexValueTo8BitValue(data[2], data[3]);
	
	if (propertyIndex < propertyCount)
	{
		uint8_t pType = propertyList[propertyIndex]->type;
		uint8_t propertyID = getPropertyIDAtIndex(propertyIndex);

		if (pType < PTYPE_RGB_SELECTOR)
		{
			uint8_t state = getSimplePropertyState(propertyID);

			char twoStr[3] = { 0 };
			x8BitValueToHexTwoCharBuffer(state, twoStr);

			char pResponse[12];
			pResponse[0] = '3';
			pResponse[1] = '2';
			pResponse[2] = data[2];
			pResponse[3] = data[3];
			pResponse[4] = '0';
			pResponse[5] = '3';
			pResponse[6] = '0';
			pResponse[7] = '0';
			pResponse[8] = twoStr[0];
			pResponse[9] = twoStr[1];
			pResponse[10] = '\r';
			pResponse[11] = '\0';

			sendData(pResponse);
		}
		else
		{
			switch (pType)
			{
				case PTYPE_RGB_SELECTOR:
				onRGBStateRequest(propertyID);
				break;
				case PTYPE_EX_LEVEL_SELECTOR:
				onExLevelStateRequest(propertyID);
				break;
				case PTYPE_TIME_SELECTOR:
				onTimeSelectorStateRequest(propertyID);
				break;
				case PTYPE_TIME_FRAME_SELECTOR:
				onTimeFrameSelStateRequest(propertyID);
				break;
				case PTYPE_DATE_SELECTOR:
				onDateSelectorStateRequest(propertyID);
				break;
				case PTYPE_UNLOCK_CONTROL:
				onUnlockControlStateRequest(propertyID);
				break;
				case PTYPE_NAVIGATOR_CONTROL:
				onNavigatorControlStateRequest(propertyID);
				break;
				case PTYPE_BAR_GRAPH:
				onBarGraphStateRequest(propertyID);
				break;
				case PTYPE_LINE_GRAPH:
				onLineGraphStateRequest(propertyID);
				break;
				case PTYPE_STRING_INTERROGATOR:
				onStringInterrogatorStateRequest(propertyID);
				break;
				case PTYPE_TEXT_LIST_PRESENTER:
				onTextListPresenterStateRequest(propertyID);
				break;
				default:
				break;
			}
		}
	}
}

void onRGBStateRequest(uint8_t propertyID)
{
	PRGB_SELECTOR_STATE rgbState = getRGBSelectorState(propertyID);
	if (rgbState != NULL)
	{
		char response[23] = { 0 };
		rgbStateToStateResponseString(rgbState, response);

		sendData(response);
	}
}

void onExLevelStateRequest(uint8_t propertyID)
{
	PEX_LEVEL_SELECTOR_STATE exLevelState = getExLevelSelectorState(propertyID);
	if (exLevelState != NULL)
	{
		char response[26] = { 0 };
		exLevelStateToStateResponseString(exLevelState, response);

		sendData(response);
	}
}

void onTimeSelectorStateRequest(uint8_t propertyID)
{
	PTIME_SELECTOR_STATE timeSelState = getTimeSelectorState(propertyID);
	if (timeSelState != NULL)
	{
		char response[15] = { 0 };
		timeSelectorStateToStateResponseString(timeSelState, response);

		sendData(response);
	}
}

void onTimeFrameSelStateRequest(uint8_t propertyID)
{
	PTIME_FRAME_STATE timeFrameState = getTimeFrameSelectorState(propertyID);
	if (timeFrameState != NULL)
	{
		char response[19] = { 0 };
		timeFrameSelectorStateToStateResponseString(timeFrameState, response);

		sendData(response);
	}
}

void onDateSelectorStateRequest(uint8_t propertyID)
{
	PDATE_SELECTOR_STATE dateSelState = getDateSelectorState(propertyID);
	if (dateSelState != NULL)
	{
		char response[19] = { 0 };
		dateSelectorStateToStateResponseString(dateSelState, response);

		sendData(response);
	}
}

void onUnlockControlStateRequest(uint8_t propertyID)
{
	PUNLOCK_CONTROL_STATE unlockState = getUnlockControlState(propertyID);
	if (unlockState != NULL)
	{
		char response[15] = { 0 };
		unlockControlStateToStateResponseString(unlockState, response);

		sendData(response);
	}
}

void onNavigatorControlStateRequest(uint8_t propertyID)
{
	PNAVIGATOR_STATE navState = getNavigatorControlState(propertyID);
	if (navState != NULL)
	{
		char response[17] = { 0 };
		navigatorControlStateToStateResponseString(navState, response);

		sendData(response);
	}
}

void onBarGraphStateRequest(uint8_t propertyID)
{
	PBAR_GRAPH_STATE barGraphState = getBarGraphState(propertyID);
	if (barGraphState != NULL)
	{
		char* response = NULL;
		barGraphStateToStateResponseString(barGraphState, &response);
		if (response != NULL)
		{
			sendData(response);
			free(response);
		}
	}
}

void onLineGraphStateRequest(uint8_t propertyID)
{
	PLINE_GRAPH_STATE lineGraphState = getLineGraphState(propertyID);
	if (lineGraphState != NULL)
	{
		char* response = NULL;
		lineGraphStateToStateResponseString(lineGraphState, &response);
		if (response != NULL)
		{
			sendData(response);
			free(response);
		}
	}
}

void onStringInterrogatorStateRequest(uint8_t propertyID)
{
	PSTRING_INTERROGATOR_STATE stringIGState = getStringInterrogatorState(propertyID);
	if (stringIGState != NULL)
	{
		char* response = NULL;
		stringInterrogatorStateToStateResponseString(stringIGState, &response);
		if (response != NULL)
		{
			sendData(response);
			free(response);
		}
	}
}

void onTextListPresenterStateRequest(uint8_t propertyID)
{
	PTEXT_LIST_PRESENTER_STATE textLPState = getTextListPresenterState(propertyID);
	if (textLPState != NULL)
	{
		char* response = NULL;
		textListPresenterStateToStateResponseString(textLPState, &response);
		if (response != NULL)
		{
			sendData(response);
			free(response);
		}
	}
}

void onExecutionCommand(const char* data, uint8_t length)
{
	if (data != NULL)
	{
		uint8_t propertyIndex =
			x2CharHexValueTo8BitValue(data[2], data[3]);
			
		if (propertyIndex < propertyCount)
		{
			uint8_t pType = propertyList[propertyIndex]->type;
			uint8_t propertyID = getPropertyIDAtIndex(propertyIndex);

			switch (pType)
			{
			case PTYPE_BUTTON:
				if (rmtCallback != NULL)
				{
					rmtCallback(propertyID, REVT_BUTTON_PRESSED);
				}
				break;
			case PTYPE_SWITCH:
				if (length >= 10)
				{
					propertyList[propertyIndex]->state = (data[9] == '0') ? 0 : 1;
					if (rmtCallback != NULL)
					{
						rmtCallback(propertyID, REVT_SWITCH_STATE_CHANGED);
					}
				}
				break;
			case PTYPE_LEVEL_SELECTOR:
				if (length >= 10)
				{
					propertyList[propertyIndex]->state = x2CharHexValueTo8BitValue(data[8], data[9]);
					if (rmtCallback != NULL)
					{
						rmtCallback(propertyID, REVT_LEVEL_SELECTOR_VALUE_CHANGED);
					}
				}
				break;
			case PTYPE_LEVEL_INDICATOR:
				// this type has no execution
				break;
			case PTYPE_TEXT_DISPLAY:
				// this type has no execution
				break;
			case PTYPE_OPTION_SELECTOR:
				if (length >= 10)
				{
					propertyList[propertyIndex]->state = x2CharHexValueTo8BitValue(data[8], data[9]);
					if (rmtCallback != NULL)
					{
						rmtCallback(propertyID, REVT_OPTION_SELECTOR_INDEX_CHANGED);
					}
				}
				break;
			case PTYPE_RGB_SELECTOR:
				if (length >= 20)
				{
					PRGB_SELECTOR_STATE rgbState = getRGBSelectorState(propertyID);
					if (rgbState != NULL)
					{
						executionStringToRGBStateStruct(data, rgbState);
						if (rmtCallback != NULL)
						{
							rmtCallback(propertyID, REVT_RGB_SELECTOR_STATE_CHANGED);
						}
					}
				}
				break;
			case PTYPE_EX_LEVEL_SELECTOR:
				if (length >= 13)
				{
					PEX_LEVEL_SELECTOR_STATE exLevelState = getExLevelSelectorState(propertyID);
					if (exLevelState != NULL)
					{
						executionStringToExLevelStruct(data, exLevelState);
						if (rmtCallback != NULL)
						{
							rmtCallback(propertyID, REVT_EXLEVEL_SELECTOR_STATE_CHANGED);
						}
					}
				}
				break;
			case PTYPE_TIME_SELECTOR:
				if (length >= 12)
				{
					PTIME_SELECTOR_STATE timeSelState = getTimeSelectorState(propertyID);
					if (timeSelState != NULL)
					{
						executionStringToTimeSelectorStruct(data, timeSelState);
						if (rmtCallback != NULL)
						{
							rmtCallback(propertyID, REVT_TIME_SELECTOR_STATE_CHANGED);
						}
					}
				}
				break;
			case PTYPE_TIME_FRAME_SELECTOR:
				if (length >= 16)
				{
					PTIME_FRAME_STATE timeFrameState = getTimeFrameSelectorState(propertyID);
					if (timeFrameState != NULL)
					{
						executionStringToTimeFrameSelectorStruct(data, timeFrameState);
						if (rmtCallback != NULL)
						{
							rmtCallback(propertyID, REVT_TIME_FRAME_SELECTOR_STATE_CHANGED);
						}
					}
				}
				break;
			case PTYPE_DATE_SELECTOR:
				if (length >= 16)
				{
					PDATE_SELECTOR_STATE dateSelState = getDateSelectorState(propertyID);
					if (dateSelState != NULL)
					{
						executionStringToDateSelectorStruct(data, dateSelState);
						if (rmtCallback != NULL)
						{
							rmtCallback(propertyID, REVT_DATE_SELECTOR_STATE_CHANGED);
						}
					}
				}
				break;
			case PTYPE_UNLOCK_CONTROL:
				if (length >= 12)
				{
					PUNLOCK_CONTROL_STATE unlockControlState = getUnlockControlState(propertyID);
					if (unlockControlState != NULL)
					{
						executionStringToUnlockControlStruct(data, unlockControlState);
						if (rmtCallback != NULL)
						{
							rmtCallback(propertyID, REVT_UNLOCK_CONTROL_STATE_CHANGED);
						}
					}
				}
				break;
			case PTYPE_NAVIGATOR_CONTROL:
				if (length >= 14)
				{
					PNAVIGATOR_STATE navState = getNavigatorControlState(propertyID);
					if (navState != NULL)
					{
						executionStringToNavigatorControlStruct(data, navState);
						if (rmtCallback != NULL)
						{
							rmtCallback(propertyID, REVT_NAVIGATOR_CONTROL_STATE_CHANGED);
						}
					}
				}
				break;
			case PTYPE_BAR_GRAPH:
				// this type has no execution
				break;
			case PTYPE_LINE_GRAPH:
				// this type has no execution
				break;
			case PTYPE_STRING_INTERROGATOR:
				if (length >= 13)
				{
					PSTRING_INTERROGATOR_STATE stringIGState = getStringInterrogatorState(propertyID);
					if (stringIGState != NULL)
					{
						executionStringToStringInterrogatorStruct(data, stringIGState);
						if (rmtCallback != NULL)
						{
							rmtCallback(propertyID, REVT_STRING_INTERROGATOR_DATA_RECEIVED);
						}
					}
				}
				break;
			case PTYPE_TEXT_LIST_PRESENTER:
				// this type has no execution
				break;
			default:
				break;
			}
		}
	}	
}

void onNotificationTransmission(const char* data, uint8_t length)
{
	if (length >= 9)
	{
		switch (data[8])
		{
		case '1':	// property loading complete notification
			if (length >= 10)
			{
				if (rmtCallback != NULL)
				{
					if (data[9] == '1')
					{
						rmtCallback(0, REVT_PROPERTY_LOADING_FROM_CACHE_COMPLETE);
					}
					else
					{
						rmtCallback(0, REVT_PROPERTY_LOADING_FROM_DEVICE_COMPLETE);
					}
				}
			}
			break;
		case '2':	// time request response
			if (length >= 15)
			{
				if (rmtCallback != NULL)
				{
					// hours
					responseData.a = x2CharHexValueTo8BitValue(data[9], data[10]);
					// minutes
					responseData.b = x2CharHexValueTo8BitValue(data[11], data[12]);
					// seconds
					responseData.c = x2CharHexValueTo8BitValue(data[13], data[14]);

					rmtCallback(0, REVT_TIME_REQUEST_RESPONSE);
				}
			}
			break;
		case '3':	// property page navigation
			if (rmtCallback != NULL)
			{
				responseData.a = x2CharHexValueTo8BitValue(data[2], data[3]);
				rmtCallback(responseData.a, REVT_COMPLEX_PROPERTY_PAGE_INVOKED);
			}
			break;
		case '4':	// user navigated back to device main
			if (rmtCallback != NULL)
			{
				rmtCallback(0, REVT_BACK_NAVIGATION);
			}
			break;
		case '5':	// language request response
			if (length >= 11)
			{
				languageIdentifier[0] = data[9];
				languageIdentifier[1] = data[10];

				if (rmtCallback != NULL)
				{
					rmtCallback(0, REVT_LANGUAGE_REQUEST_RESPONSE);
				}
			}
			break;
		case '6':	// factory reset executed
			if (rmtCallback != NULL)
			{
				rmtCallback(0, REVT_FACTORY_RESET_EXECUTED);
			}
			break;
		case '7':	// device connection restored notification
			if (length >= 11)
			{
				if (rmtCallback != NULL)
				{
					uint8_t pid = x2CharHexValueTo8BitValue(data[9], data[10]);
					rmtCallback(pid, REVT_DEVICE_CONNECTION_RESTORED);
				}
			}
			break;
		case '8':
			// reserved, do not use
			break;
		case '9':	// date request response
			if (length >= 17)
			{
				if (rmtCallback != NULL)
				{
					// day
					responseData.a = x2CharHexValueTo8BitValue(data[9], data[10]);
					// month
					responseData.b = x2CharHexValueTo8BitValue(data[11], data[12]);
					// year
					responseData.d = x4CharHexValueTo16BitValue(data[13], data[14], data[15], data[16]);
	
					rmtCallback(0, REVT_DATE_REQUEST_RESPONSE);
				}
			}
			break;
		case 'a':	// ui mode request response
			if (length >= 10)
			{
				if (rmtCallback != NULL)
				{
					responseData.a = (data[9] == '0') ? DAY_MODE : NIGHT_MODE;
					rmtCallback(0, REVT_UI_MODE_REQUEST_RESPONSE);
				}
			}
			break;
		case 'b':	// user navigated to device settings page
			if(rmtCallback != NULL)
			{
				rmtCallback(0, REVT_SETTING_PAGE_INVOKED);
			}
			break;
		default:
			break;
		}
	}
}


void onBindingTransmission(const char* data, uint8_t length)
{
	if (bndgCallback != NULL)
	{
		if ((data[8] == '1') || (data[8] == '2'))
		{
			// authentication or enable -> key is necessary, so record it
			char bKey[11] = { 0 };
			uint8_t lastIndex = 10;

			for (uint8_t i = 0; i < 10; i++) {
				if ((data[i + 9] == '\r') || (data[i + 9] == '\0')) {
					lastIndex = i;
					break;
				}
				bKey[i] = data[i + 9];
			}
			bKey[lastIndex] = '\0';

			if (data[8] == '2')
			{
				// authentication request
				uint8_t authResult =
					bndgCallback(BINDING_CB_EVENT_AUTHENTICATION, bKey);

				if (authResult == BINDING_CB_RESULT_AUTHENTICATION_SUCCESS)
				{
					// authentication success
					sendData("6200040020\r\0");
				}
				else
				{
					sendData("6200040021\r\0");
				}
				
			}
			else if (data[8] == '1')
			{
				// enable binding request
				uint8_t enableResult =
					bndgCallback(BINDING_CB_EVENT_ENABLED, bKey);

				if (enableResult == BINDING_CB_RESULT_ENABLE_SUCCESS)
				{
					// binding success
					setBindingRequired(true);
					sendData("6200040010\r\0");
				}
				else
				{
					sendData("6200040014\r\0");
				}
			}
		}
		else if (data[8] == '0')
		{
			// release binding request
			uint8_t releaseResult =
				bndgCallback(BINDING_CB_EVENT_RELEASED, NULL);

			if (releaseResult == BINDING_CB_RESULT_RELEASE_SUCCESS)
			{
				// release success
				setBindingRequired(false);
				sendData("6200030000\r\0");
			}
			else
			{
				sendData("6200030000\r\0");
			}		
		}
		else {
			// invalid -> send error string
			sendData("6200041194\r\0");
		}
	}
	else
	{
		// binding handler not implemented -> send binding not supported response
		sendData("6200040032\r\0");
	}
}





