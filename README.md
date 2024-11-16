# LaRoomy_Min_API
![](https://img.shields.io/github/license/LaRoomy/LaRoomy_Min_API?color=009999&style=for-the-badge)
## Description
This is a minimized Version of the LaRoomy Api written in C and is optimized for 8 bit microcontroller. It's not platform specific: It can be used in conjunction with all controllers which have an externally bluetooth hardware or bluetooth capabilities included.

To get an introduction, a description of the concept behind and further info to the LaRoomy API vistit the [website](https://api.laroomy.com/p/get-started.html#genInfoHd)

## Implementation
> [!NOTE]
> This is a short outline of the implementation terminology.
> 
>  To get detailed info [click here](https://api.laroomy.com/2023/07/minimized-api-version-for-8bit-systems.html) to visit the respective page on the LaRoomy Api Website.

**To implement the LaRoomy functionality in your project the following steps are necessary:**

### Definitions
- Define unique identifier for all properties which should be added to the api, this makes it easy to work with the properties later.
- Create global device property struct parameter as data-holder for the LaRoomy Api. Please note that this api is written for controller with very limited system resources, that means even though the properties are added to the api, the api does not execute dynamic memory allocation, the api only saves references to the preliminary defined parameters and put them on stack which is managed by the api.
- If device properties with complex state type are used, the respective complex property state struct must be also created as data-holder
- Define a callback function for all events that should be processed in your project.

### Setup
- Initialize all device-property structs
- Fill the device-property structs with the property data
- Add the properties to the api
- Add the complex property state data-holder to the api (if applicable)
- Register the callback(s) to receive remote events
- Register the gateway function for outgoing data

### Loop
- Check for incoming data periodically and forward it to the api if there was data to process

### Important

Since the minimized LaRoomy Api is a generalized implementation of the transmission handling to interact with the LaRoomy App, the api does not automatically receive or send data.
To implement the API in the existing logic, the data to sent and the data which is received must be bypassed to the api by registering a gateway function for each direction, respectively.

To enable the api to send data a function must be decared which handles the send operation for Bluetooth data and a reference to the function must be passed to the LaRoomy Api:
``` C
// define a function which handles data output
// (in this case a bluetooth module with transparent uart is used)
void sendBLEData(const char* data)
{
    if(data != NULL)
    {
        USART_WriteString(data);
    }
}

// set the usart output gateway for the laroomy api
// - the internal transmission control will use this function to send data
setOutGateway(sendBLEData);
```
If data is received through the bluetooth line, it must be passed to the 'onDataReceived' function of the LaRoomy Api to process it:
``` C
// when data is received from the gateway, this function must be called to process the data
// - this is critical for the api to work correctly
void onDataReceived(const char *data);
```

## Examples
Included in this repository are [examples](https://github.com/LaRoomy/LaRoomy_Min_API/tree/main/Examples) created with the Microchip Studio IDE. They use a combination of an Atmega328P and a Microchip RN4871 Bluetooth Module.

The underlying schematic and board data is created with the Eagle Layout Editor and included in this repository. [Show it](https://github.com/LaRoomy/LaRoomy_Min_API/tree/main/Test_Circuit)
