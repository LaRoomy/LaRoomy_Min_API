# LaRoomy_Min_API

## Description
This is a minimized Version of the LaRoomy Api written in C and is optimized for 8 bit microcontroller. It's not platform specific: It can be used in conjunction with all controllers which have an externally bluetooth hardware or bluetooth capabilities included.

## Implementation
To implement the API in the existing logic, the data which must be sent and the data which is received must be bypassed to the api through registering a gateway function for each direction, respectively.

## Examples
Included in this repository are [examples](https://github.com/LaRoomy/LaRoomy_Min_API/tree/main/Examples) created with the Microchip Studio IDE. They use a combination of an Atmega328P and a Microchip RN4871 Bluetooth Module.
