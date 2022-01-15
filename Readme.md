# SocksWarmerVoltageControl

## Overview
Simple voltage control for electric socks warmer which features:
* linear voltage control with desired steps within designated range
* user interface by lever switch (power on/off & voltage control)

## Supported Boards
* Seeeduino XIAO

## Supported Devices
* CAT5171 digital potentiometer (wiper)
* MP1584 DC/DC converter

## Ciruit Diagram
T.B.D.

## Pin Assignment
| XIAO Pin # | XIAO Pin Name | Net Name | Description |
----|----|----|----
|  1 | D1 | PIN_EN | MP1584 EN |
|  2 | D2 | PIN_UP | Lever Switch Up |
|  3 | D4 | PIN_DN | Lever Switch Down |
|  4 | SDA | I2C_SDA | CAT5171 SDA (pulled-up with 10Kohm from 3.3V) |
|  5 | SCL | I2C_SCL | CAT5171 SCL (pulled-up with 10Kohm from 3.3V) |
|  6 | D6 | PIN_CT | Lever Switch Center |
| 11 | 3V3 | 3V3 | 3.3V out |
| 12 | GND | GND | Ground |
| 13 | 5V | 5V | 5.0V in from USB Battery |

## How to build in Arduino
### Seeeduino XIAO
* append the URL of Seeduino Boards Manager JSON file in: File > Preferences > Additional Boards Manager URLs. 
```
https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json
```
* select Tools > Board > Boards Manager as Seeeduino XIAO
* click upload to build and upload the program to the board

## Customize
| Parameter Name | Description |
----|----
| vRangeMin | lowest voltage of the range |
| vRangeMax | highest voltage of the range |
| numLinearPos | the steps in the range (e.g. set 11 to have steps 0 ~ 10) |

## Control Guide
* plug-in USB to power-on and output lowest voltage within designated range
* long push of Center button to power on/off
* push up button to step up the voltage
* push down button to step down the voltage
* long push down button to output lowest voltage within designated range
