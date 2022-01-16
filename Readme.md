# SocksWarmerVoltageControl

## Overview
Simple voltage control for electric socks warmer which features:
* linear voltage control with desired steps within designated range
* user interface by lever switch (power on/off & voltage control)

## Supported Boards
* Seeeduino XIAO
* Seeed XIAO RP2040

## Supported Devices
* CAT5171 digital potentiometer (wiper)
* MP1584 DC/DC converter

## Ciruit Diagram
T.B.D.

## Pin Assignment
common for Seeeduino XIAO and Seeed XIAO RP2040

| Pin No. | Pin Name | Net Name | Description |
----|----|----|----
|  1 | D1 | PIN_EN | MP1584 EN |
|  2 | D2 | PIN_UP | Lever Switch Up |
|  3 | D4 | PIN_DN | Lever Switch Down |
|  4 | SDA | SDA | CAT5171 I2C SDA (pulled-up with 10Kohm from 3.3V) |
|  5 | SCL | SCL | CAT5171 I2C SCL (pulled-up with 10Kohm from 3.3V) |
|  6 | D6 | PIN_CT | Lever Switch Center |
| 11 | 3V3 | 3V3 | 3.3V out |
| 12 | GND | GND | Ground |
| 13 | 5V | 5V | 5.0V in from USB Battery |

## How to build in Arduino
* append the URL of Seeduino Boards Manager JSON file in: File > Preferences > Additional Boards Manager URLs. 
```
https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json
```
### Seeeduino XIAO
* select Tools > Board > Boards Manager as Seeeduino XIAO

### Seeed XIAO RP2040
Arduino RP2040 library needs to be re-generated since it doesn't include hardware_sleep object at default.
To update this, you need to prepare Developer Command Prompt for VS 2019 and Visual Studio Code on Windows enviroment.
See ["Getting started with Raspberry Pi Pico"](https://datasheets.raspberrypi.org/pico/getting-started-with-pico.pdf)
.
* modify %USERPROFILE%\AppData\Local\Arduino15\packages\Seeeduino\hardware\rp2040\1.9.3\pico-sdk\src\rp2_common\pico_stdio\stdio.c

(add static not to coflict posix.c)
```
//int _read(int handle, char *buffer, int length) {
static int _read(int handle, char *buffer, int length) {
...

//int _write(int handle, char *buffer, int length) {
static int _write(int handle, char *buffer, int length) {
...
```

* modify %USERPROFILE%\AppData\Local\Arduino15\packages\Seeeduino\hardware\rp2040\1.9.3\tools\libpico
  CMakeLists.txt
```
cmake_minimum_required(VERSION 3.12)

set(PICO_SDK_PATH ${CMAKE_CURRENT_LIST_DIR}/../../pico-sdk) ##### Add this #####
include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)
...
target_link_libraries(pico 
...
	pico_audio
	pico_audio_i2s
	hardware_sleep ##### Add this #####
)

# comment below
#add_custom_command(TARGET pico POST_BUILD
#	COMMAND ar d libpico.a stdio.c.obj stdio_usb.c.obj stdio_usb_descriptors.c.obj
#)
```

* Launch Visual Studio 2019 -> Developer Command Prompt for VS 2019
```
> cd %USERPROFILE%\AppData\Local\Arduino15\packages\Seeeduino\hardware\rp2040\1.9.3\tools\libpico
> mkdir build
> cd build
> cmake -G "NMake Makefiles" ..
> nmake
```
Then replace build\libpico.a with %USERPROFILE%\AppData\Local\Arduino15\packages\Seeeduino\hardware\rp2040\1.9.3\lib\libpico.a

* Modify %USERPROFILE%\AppData\Local\Arduino15\packages\Seeeduino\hardware\rp2040\1.9.3\lib\platform_inc.txt
```
...
-iwithprefixbefore/pico-extras/src/common/pico_util_buffer/include
-iwithprefixbefore/pico-extras/src/rp2_common/pico_audio_i2s/include
-iwithprefixbefore/pico-extras/src/rp2_common/pico_sleep/include        ##### Add this #####
-iwithprefixbefore/pico-extras/src/rp2_common/hardware_rosc/include     ##### Add this #####
...
-iwithprefixbefore/pico-sdk/src/rp2_common/hardware_resets/include
-iwithprefixbefore/pico-sdk/src/rp2_common/hardware_spi/include
-iwithprefixbefore/pico-sdk/src/rp2_common/hardware_rtc/include         ##### Add this #####
-iwithprefixbefore/pico-sdk/src/rp2_common/hardware_sync/include
...
```

* Then at Arduino, select Tools > Board > Boards Manager as Seeed XIAO RP2040

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

## NeoPixel Indicator (Seeed XIAO RP2040 only)
* 1times dimming when change voltage level or power on
* 2times blink to power off
* Color indicates voltage level: Blue (lowest) --> Geen (midium) --> Red (highet)
