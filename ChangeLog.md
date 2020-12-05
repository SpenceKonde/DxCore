# Changelog
This page documents (nearly) all bugfixes and enhancements that produce visible changes in behavior throughout the history of DxCore. Note that this document is maintained by a human, who is - by nature - imperfect; sometimes the changelog may not be updated at the same time as the changes go in, and occasionally a change is missed entirely in the changelog, though this is rare.

## Changes not yet in release
Changes listed here are checked in to GitHub ("master" branch unless specifically noted; this is only done when a change involves a large amount of work and breaks the core in the interim, or where the change is considered very high risk, and needs testing by others prior to merging the changes with master). These changes are not yet in any "release" nor can they be installed through board manger, only downloading latest code from github will work. These changes will be included in the listed version, though planned version numbers may change without notice - critical fixes may be inserted before a planned release and the planned release bumped up a version, or versions may go from patch to minor version depending on the scale of changes.

### Planned for 1.3.0
* Fix slow sampled BOD mode
* Fix DISABLE_MILLIS and other timer options
* Set SPI_INTERFACES_COUNT to 1, not 2 (#32). SPI_1 library will be removed - it's a mess, it's incompatible with everything for multiple reasons, and SPI_INTERFACES_COUNT breaks perfectly sane implementations, instead of making it easier for libraries to figure out how many usable SPI ports are available, which is the point of that #define. Core problem is that all other Arduino boards called the SPIclass for second serial port SPI1 - but that's the name for the struct defined in the io headers! As far as I can tell, there's basically nothing compatible with my SPI1 library anyway! That was not my finest work... for 1.3.0, will adapt SPI.swap() to accept SPI1 pins/port.
* Set all fuses that we ever set in response to menu selections except for BODCFG on all uploads to non-Optiboot configurations, and all "upload using programmer" to optiboot configurations. Fuses that the core does not provide a method of specifying are not touched by the normal upload process; if you have gone and changed one, it is assumed that it was intentional.
* The Burn bootloader command will set BODCFG, as well as resetting all fuses to their defauts.
* Correct several bugs with Optiboot entry condition detections (megaTinyCore issue #259)
* Implement numerous optimizations and corrections in Optiboot.
* Correct Servo being generally hosed (megaTinyCore #195, #241 - not yet implemented)
* Correct period of TCD0 PWM (from megaTinyCore - not yet implemented)
* Correct issue with ADC errata workaround (#43)
* Correct issue with EEPROM library (#34)
* Update Readme to reflect current state of core

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
