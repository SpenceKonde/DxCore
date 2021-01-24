# Changelog
This page documents (nearly) all bugfixes and enhancements that produce visible changes in behavior throughout the history of DxCore. Note that this document is maintained by a human, who is - by nature - imperfect; sometimes the changelog may not be updated at the same time as the changes go in, and occasionally a change is missed entirely in the changelog, though this is rare.

## Changes not yet in release
Changes listed here are checked in to GitHub ("master" branch unless specifically noted; this is only done when a change involves a large amount of work and breaks the core in the interim, or where the change is considered very high risk, and needs testing by others prior to merging the changes with master). These changes are not yet in any "release" nor can they be installed through board manger, only downloading latest code from github will work. These changes will be included in the listed version, though planned version numbers may change without notice - critical fixes may be inserted before a planned release and the planned release bumped up a version, or versions may go from patch to minor version depending on the scale of changes.


### 1.3.0
#### New Features
* **NEW toolchain package - support for all DA and DB parts on all platforms!** (board manager only)
* Pull in new version of avrdude to **support Dx-series Curiosity** boards via avrdude. (board manager only)
* **Add support for pymcuprog-backed pyupdi-style programming with serial adapter as programmer (#38)**
* **Add new Event system library** to go with the new version of Logic! (#52)
* digitalWriteFast(), digitalReadFast() are now available for single-cycle pin writes! This also comes along with the compiletime error checking for most digital and analog API functions.
* Safe fuses automatically written on all UPDI uploacs: Set all fuses that we ever set in response to menu selections except for BODCFG on all uploads to non-Optiboot configurations, and all "upload using programmer" to optiboot configurations. Fuses that the core does not provide a method of specifying are not touched by the normal upload process; if you have gone and changed one, it is assumed that it was intentional.
* The Burn bootloader command will set BODCFG, as well as resetting all fuses to their defauts.
* Optiboot now supports writing to flash from the application section. This is done through a novel and highly efficient (of bootloader flash, of which we are terribly short!) method; only 2 instruction words are used within the bootloader section.
* Add the `writeflash.h` or `Flash.h` library to take advantage of it to program the flash. Please give feedback!
* Update Logic library to latest version, improving init(), keyword highlighting, and documentation. Docs now specific to DxCore.
* Split init() up into a few weakly defined sub-functions to support future enhancements (#40)
* **POTENTIALLY BREAKING CHANGE** Timer D PWM pins for all parts moved to pins 6, 7 instead of 4, 5. This is because I realized that USART0 will often be pinswapped in order to use PA0 (and PA1 on DB/DD-series) as a clock source. Future updates will make TCD PWM pins flexible, that is whichever two of the four pins you call analogWrite() on will output PWM.
* Part-specific pages for AVR DB-series parts. (#16, #44)
* Several TCD0 PWM enhancements from megaTinyCore. (#39)
* Adapt SPI library to be able to use either SPI0 or SPI1 (#32, #35)

#### Minor enhancements
* Fix decade-old bug in an SPI example (lol)
* Stop checking SS pin passed to SPI.pins()
* Correct PIN_SPI1_SS in pins_arduino.h for 64-pin parts - not that it matters.
* Move sections on peripheral libraries into their own files
* Add ZeroCross::have_seperate_mux(); test
* Add in the SERIAL_PORT_whatever macros, against my better judgement.
* The 1-second pre-built optiboot hex files do not consider POR to be an entry condition (consistent with their intended use case of there being a hardware reset )
* Slightly better emulation of classic behavior in digital I/O, particularly regarding ways to ensure the OUT bit for a bit is in the right state before you change it's

#### Bugfixes
* Fix slow sampled BOD mode
* Fix DISABLE_MILLIS and other timer options.
* Correct several bugs with Optiboot entry condition detections (megaTinyCore issue #259)
* Implement numerous optimizations and corrections in Optiboot.
* Correct Servo being generally hosed. Add in future plumbing for potential support of use of arbitrary timer for Servo
* Correct issue with ADC errata workaround (#43)
* Correct issue with EEPROM library (#34)
* Correct period of TCD0 PWM. (it was off by 1/510th of the target speed,
* Pull in major Serial rework from megaTinyCore, which is hoped to fix some stability issues (#33)
* Remove SPI1 library, set SPI_INTERFACES_COUNT to 1, not 2 (#32, #35)
* Corrected name of serial pin defines
* Fixed the mess I made out of Wire's baud rate calculation. Resync'ed it with megaTinyCore, hardened it against the TWI PORT.OUT errata (realized the minor digital I/O improvements could have made it more likely to cause issues.)
* Corrected compile issues with the Comparator library, and improved handling of the voltage reference, and called out in the README the fact that they share a reference.


### 1.2.0/1.2.0a
* Add support for AVR64DB and AVR32DB parts! Too bad the compiler issue is still a problem.... I was not able to make a working toolchain for these parts, not quite sure what I got wrong this time around...
* Add tinyNeoPixel support for new methods added to the Adafruit library and the higher speeds possible on these devices.
* Declare a const variable as MAPPED_PROGMEM to put it in flash - you get up to 32 kb like this (assuming you have enough space left after the rest of your code), and then access it like any other variable! (this only works if you don't change the FLMAP bits in NVMCTRL.CTRLB from their default setting).
* Massive overhaul to organization of boards.
* Add support for reading ADC_VDDDIV10, ADC_VDDIO2DIV10 - these are available only on the DB-series, even for VDD, unfortunately - they make reading VDD super easy!
* Add DxCore.h library for controlling AutoTune and checking status of MVIO.
* Remove ADC workaround option; it is now always used on DA-series (where it is omnipresent) and not on DB-series (where it is not present at all).
* Fix EEPROM library
* Optiboot now fits in 512b! You will need to reburn bootloader to use boards previously loaded with the un-shrinkified bootloader.
* Correct pins_arduino files to account for the fact that PD0 does not exist on 28 and 32-pin DB-series parts.

### 1.1.0a
* As 1.1.0, but contains new toolchain. Does not work correctly on Linux platforms.

### 1.1.0
* Corrected 128DB board definitions.
* Added support for Optiboot Bootloader
* Include prebuilt Optiboot binaries for each size of flash, with 1s and 8s start delay.
* Corrected avrdude.conf for 128KB parts to support programming via bootloader
* Corrected DB board definitions so that 'burn bootloader' does not disable MVIO

### 1.0.1
* Tried to add support for curiosity nano (unsuccessfully)
* Freshly built toolchain that I'm hosting in S3 to accomodate the 128k DB-series parts and the smaller-flash DA-series. THESE SHOULD NOW WORK!
* Minor doc improvements
