# PlatformIO

[PlatformIO](https://platformio.org) is an open source ecosystem for embedded development.
It has a built-in library manager and is Arduino compatible. It supports most operating systems; Windows, MacOS, Linux 32 and 64-bit, ARM and X86. And best of all, DxCore is supported!

* [What is PlatformIO?](http://docs.platformio.org/en/latest/what-is-platformio.html)
* [PlatformIO IDE](https://platformio.org/platformio-ide)
* Getting started with [PlatformIO IDE](https://docs.platformio.org/en/latest/integration/ide/visualstudio.html) or [PlatformIO command line interface](https://docs.platformio.org/en/latest/core/index.html)
* [Advanced functionality](https://docs.platformio.org/en/latest/platforms/atmelmegaavr.html)
* [Project Examples](https://docs.platformio.org/en/latest/platforms/atmelmegaavr.html#examples)


## DxCore + PlatformIO
DxCore and PlatformIO goes great together. You can do serial uploads and upload using a dedicated UPDI programmer, but you can also let PlatformIO calculate the fuses and load the correct bootloader file, just like Arduino IDE does!

PlatformIO uses the information provided in platformio.ini to calculate what fuse bits and what bootloader file to load.
Simply provide enough information and run the following commands:

```ini
; Only set fuses
pio run -t fuses -e fuses_bootloader
; Set fuses and burn bootloader
pio run -t bootloader -e fuses_bootloader
; (where "fuses_bootloader" can be replace with a different environment to match your build configuration)
```

You can find a platformio.ini template you can use when creating a project for a DxCore compatible device below.
The most common functionality is available in this template. As you can see, the templated is divided into multiple environments.

* The default build environment are defined under *[platformio]*.
* All parameters that are common for all environments are defined under *[env]*.
* Use the *[env:Upload_UPDI]* or *[env:Upload_UART]* to upload to your target.
* Use *[env:fuses_bootloader]* to set the fuses or burn the bootloader.

More information on what each line means can be found further down on this page.


## platformio.ini template

```ini
; PlatformIO template configuration file for DxCore
; https://github.com/SpenceKonde/DxCore/
;
;   Build options: build flags, source filter
;   Hardware options: oscillator type, BOD, UART number, EEPROM retain
;   Upload options: custom upload port, speed, and extra flags
;   Library options: dependencies, extra library storages
;   Advanced options: extra scripting
;
; Please visit documentation for the other options
; https://github.com/SpenceKonde/DxCore/blob/master/PlatformIO.md
; https://docs.platformio.org/page/projectconf.html
; https://docs.platformio.org/en/latest/platforms/atmelmegaavr.html

[platformio]
; Default build target
default_envs = Upload_UPDI

; Parameters used for all environments
[env]
platform = atmelmegaavr
framework = arduino

; Chip in use
board = AVR128DA48

; Unflag build flags
build_unflags =
; Extra build flags
build_flags =

; Monitor port is auto detected. Override here
;monitor_port =
; Serial monitor baud rate
monitor_speed = 115200


; Run the following command to upload with this environment
; pio run -e Upload_UPDI -t upload
[env:Upload_UPDI]
; Upload protocol for UPDI upload
upload_protocol = pkobn_updi
upload_flags =


; Run the following command to upload with this environment
; pio run -e Upload_UART -t upload
[env:Upload_UART]
; Upload protocol for serial uploads (using Optiboot)
upload_protocol = arduino
upload_flags =
upload_port = /dev/cu.usbserial*


; run the following command to set fuses
; pio run -e fuses_bootloader -t fuses
; run the following command to set fuses + burn bootloader
; pio run -e fuses_bootloader -t bootloader
[env:fuses_bootloader]
; Upload protocol for used to set fuses/bootloader
upload_protocol = ${env:Upload_UPDI.upload_protocol}
upload_flags =
; Hardware settings
board_hardware.bod = 2.7v
board_hardware.eesave = yes
board_hardware.uart = no_bootloader
board_hardware.rstpin = reset

```


### `board`
PlatformIO requires the `board` parameter to be present.
The table below shows what board name should be used for each target
| AVR DA       | AVR DB       | AVR DD      |
|--------------|--------------|-------------|
| `AVR32DA28`  | `AVR32DB28`  |             |
| `AVR32DA32`  | `AVR32DB32`  |             |
| `AVR32DA48`  | `AVR32DB48`  |             |
|              |              | `AVR64DD14` |
|              |              | `AVR64DD20` |
| `AVR64DA28`  | `AVR64DB28`  | `AVR64DD28` |
| `AVR64DA32`  | `AVR64DB32`  | `AVR64DD32` |
| `AVR64DA48`  | `AVR64DB48`  |             |
| `AVR64DA64`  | `AVR64DB64`  |             |
| `AVR128DA28` | `AVR128DB28` |             |
| `AVR128DA32` | `AVR128DB32` |             |
| `AVR128DA48` | `AVR128DB48` |             |
| `AVR128DA64` | `AVR128DB64` |             |
| `curiosity_nano_da` | `curiosity_nano_db` | |



### `board_build.f_cpu`
Specifies the clock frequency in [Hz].
Used to determine what oscillator option to choose. A capital *L* has to be added to the end of the frequency number.
Below is a table with supported clocks for DxCore. Defaults to 24 MHz internal if not specified.

| Clock speed | Oscillator | board_build.f_cpu         |
|-------------|------------|---------------------------|
| 32 MHz      | Internal   | `32000000L` (overclocked) |
| 28 MHz      | Internal   | `28000000L` (overclocked) |
| 24 MHz      | Internal   | `24000000L` (default)     |
| 20 MHz      | Internal   | `20000000L`               |
| 16 MHz      | Internal   | `16000000L`               |
| 10 MHz      | Internal   | `10000000L`               |
| 8 MHz       | Internal   | `8000000L`                |
| 5 MHz       | Internal   | `5000000L`                |
| 4 MHz       | Internal   | `4000000L`                |
| 2 MHz       | Internal   | `2000000L`                |
| 1 MHz       | Internal   | `1000000L`                |
| 16 MHz      | External   | `16000000L`               |
| 12 MHz      | External   | `12000000L`               |
| 8 MHz       | External   | `8000000L`                |
| 4 MHz       | External   | `4000000L`                |
| 1 MHz       | External   | `1000000L`                |


### `board_hardware.oscillator`
Specifies to use the internal or an external oscillator.

| Oscillator option    |
|----------------------|
| `internal` (default) |
| `external`           |


### `board_hardware.uart`
Specifies the hardware UART port used for serial upload. Use `no_bootloader` if you’re using a dedicated UPDI programmer, i.e not using a bootloader for serial upload.

| Upload serial port option | Description                                    |
|---------------------------|------------------------------------------------|
| `no_bootloader` (default) |                                                |
| `ser0`                    | Use UART0 default pins                         |
| `ser0_alt`                | Use UART0 alternative pins                     |
| `ser1`                    | Use UART1 default pins                         |
| `ser1_alt`                | Use UART1 alternative pins                     |
| `ser2`                    | Use UART2 default pins                         |
| `ser2_alt`                | Use UART2 alternative pins                     |
| `ser3`                    | Use UART3 default pins (48-pin parts only)     |
| `ser3_alt`                | Use UART3 alternative pins (48-pin parts only) |

### `board_hardware.bod`
Specifies the hardware brown-out detection. Use `disabled` to disable.

| BOD              |
|------------------|
| `4.3v`           |
| `2.6v` (default) |
| `1.8v`           |
| `disabled`       |


### `board_hardware.eesave`
Specifies if the EEPROM memory should be retained when uploading using a programmer. Use `no` to disable.

| EEPROM retain   |
|-----------------|
| `yes` (default) |
| `no`            |


### `board_hardware.rstpin`
Specifies what functionality the reset pin should have. Note that the option `reset` *will* be selected regardless if you're using a bootloader.

| Reset pin functionality   |
|---------------------------|
| `reset` (default)         |
| `gpio`                    |

### `board_hardware.millistimer`
Specifies the millis/micros timing source.

| Timer                                 |
|---------------------------------------|
| `A0`                                  |
| `A1`                                  |
| `B0`                                  |
| `B1` (default if B2 is not available) |
| `B2` (default if available)           |
| `B3`                                  |
| `B4`                                  |
| `D0`                                  |
| `RTC_XTAL`                            |
| `RTC_XOSC`                            |

### `board_build.variant`
Holds the current pinout in use.

| Series | Pinouts 64 pin parts | Pinouts 48 pin parts | Pinouts 32 pin parts | Pinouts 28 pin parts | Pinouts 20 pin parts | Pinouts 14 pin parts |
|--------|----------------------|----------------------|----------------------|----------------------|----------------------|----------------------|
| DA     | [`64pin-standard` (default)](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DA64.md)| [`48pin-standard` (default)](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DA48.md) | [`32pin-standard` (default)](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DA32.md) | [`28pin-standard` (default)](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DA28.md) | N/A | N/A |
| DB     | [`64pin-standard` (default)](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DB64.md)| [`48pin-standard` (default)](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DB48.md) | [`32pin-standard` (default)](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DB32.md) | [`28pin-standard` (default)](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DB28.md) | N/A | N/A |
| DD     | N/A | N/A | [`32pin-ddseries` (default)](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DD32.md) | [`28pin-ddseries` (default)](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DD28.md) | [`20pin-ddseries` (default)](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DD20.md) | [`14pin-ddseries` (default)](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DD14.md) |


### `build_flags`
This parameter is used to set compiler flags.

**Example:**
`build_flags =-Wall -Wextra`


### `build_unflags`
This parameter is used to unflag flags automatically set by the PlatformIO build environment.

**Example:**
`build_unflags =-flto -fpermissive`


### `upload_port`
Holds the serial port used for uploading. Only needed if you're uploading using a JTAG2UPDI programmer or with a USB to serial adapter using the Optiboot bootloader. PlatformIO automatically detects the serial port. However, if you want to override this you can uncomment `upload_port`. Use `/dev/[port]` on Unix compatible systems, and use `COMx` on Windows.


### `upload_protocol`
Programmer used for uploading.

| Supported UPDI programmers in Avrdude | Notes                                                                                                            |
|---------------------------------------|------------------------------------------------------------------------------------------------------------------|
| `jtag2updi`                           | Requires upload port                                                                                             |
| `arduino`                             | Used when uploading using the Optiboot bootloader. Requires upload port                                          |
| `xplainedmini_updi`                   | Xplained Mini (mEDBG) and [microUPDI](https://www.tindie.com/products/MCUdude/microupdi-programmer/) programmers |
| `pkobn_updi`                          | On-board Curiosity nano programmer (nEDBG)                                                                       |
| `pickit4_updi`                        | PICkit4 programmer in UPDI mode                                                                                  |
| `snap_updi`                           | MPLAB SNAP programmer in UPDI mode                                                                               |
| `atmelice_updi`                       | Atmel ICE programmer in UPDI mode                                                                                |
| `xplainedpro_updi`                    | Xplained Pro in UPDI mode                                                                                        |
| `powerdebugger_updi`                  | Power Debugger in UPDI mode                                                                                      |


### `upload_flags`
Used to pass extra flags to Avrdude when uploading using a programmer.
Typical parameters are `-PUSB` and `-v`.
**Note that every flag has to be on its own line, and they have to be indented with two spaces:**
```ini
upload_flags =
  -PUSB
  -v
```


### `monitor_port`
PlatformIO detects serial ports automatically. However, if you want to override this you can uncomment `monitor_port`. Use `/dev/[port]` on Unix compatible systems, and use `COMx` on Windows.


### `monitor_speed`
Sets the serial monitor baud rate. Defaults to 9600 if not defined.


## pymcuprog
[pymcuprog](https://github.com/microchip-pic-avr-tools/pymcuprog) is a Python utility for programming various Microchip MCU devices using Microchip CMSIS-DAP based debuggers. It can be installed directly in the PlatformIO virtual environment using the following command:

```console
pip install pymcuprog
```

Once pymcuprog is installed it can be used as the uploader via a custom upload_command option:

```ini
[env:pymcuprog_upload]
upload_protocol = custom
upload_command = pymcuprog erase && pymcuprog write -f $SOURCE
```
