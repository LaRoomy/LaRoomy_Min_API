# Common Bluetooth Control Board
The aim of this construction was to create an own development board for the Microchip RN4871. Why doing this? Because all other available boards with bluetooth capabilities are consuming too much current. I wanted something that is tailor made for the LaRoomy Api and has a minimum power consumption.
If the RN4871 is configured in the right way and the power-down sleep mode of the atmega328 is used, the power consumption of the board is lower than 100µA and remains a fully functional and connectable bluetooth device.

This schematic/board is used in all examples in this repository.

## Board
![](img/atmega328_rn4871_ble_board.png)
