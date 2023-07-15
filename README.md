# LaRoomy_Min_API

## Description
This is a minimized Version of the LaRoomy Api written in C and is optimized for 8 bit microcontroller. It's not platform specific: It can be used in conjunction with all controllers which have an externally bluetooth hardware or bluetooth capabilities included.

## Implementation
Since the minimized LaRoomy Api is a generalized implementation of the transmission handling to interact with the LaRoomy App, the api does not automatically receive or send data.
To implement the API in the existing logic, the data to sent and the data which is received must be bypassed to the api by registering a gateway function for each direction, respectively.

To enable the api to send data a function must be decared which handles the send operation for Bluetooth data and a reference to the function must be passed to the LaRoomy Api:
``` C
// define a function which handles data output (in this case a bluetooth module with transparent uart is used)
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
If data is received through the bluetooth line, it must be bypassed to the 'onDataReceived' function of the LaRoomy Api to process it:
``` C
// when data is received from the gateway, this function must be called to process the data
// - this is critical for the api to work correctly
void onDataReceived(const char *data);
```

## Examples
Included in this repository are [examples](https://github.com/LaRoomy/LaRoomy_Min_API/tree/main/Examples) created with the Microchip Studio IDE. They use a combination of an Atmega328P and a Microchip RN4871 Bluetooth Module.
