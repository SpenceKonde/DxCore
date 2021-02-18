# Changelog
This page documents (nearly) all bugfixes and enhancements that produce visible changes in behavior throughout the history of DxCore. Note that this document is maintained by a human, who is - by nature - imperfect; sometimes the changelog may not be updated at the same time as the changes go in, and occasionally a change is missed entirely in the changelog, though this is rare.

## Changes not yet in release
Changes listed here are checked in to GitHub ("master" branch unless specifically noted; this is only done when a change involves a large amount of work and breaks the core in the interim, or where the change is considered very high risk, and needs testing by others prior to merging the changes with master). These changes are not yet in any "release" nor can they be installed through board manager, only downloading latest code from github will work. These changes will be included in the listed version, though planned version numbers may change without notice - critical fixes may be inserted before a planned release and the planned release bumped up a version, or versions may go from patch to minor version depending on the scale of changes.

### planned 1.3.2
* Formatting and "make the CI pass" changes
* Rename optiboot_x.c to optiboot_dx.c, remove references to tinyAVR 0/1/2-series in source code. It is very different, so we shouldn't give it the same name
* Fix bug with "burn bootloader" for DB-series parts with more than 28 pins.
* Improve library docs, remove flashWrite() library
* Add GPIOR compatibility defines.
* Fix the issue with sideffects in the min, max, constrain, and sq functions.
* Fix bug where the INVEN bit would be unset when doing digitalWrite() on pins that can do TCD PWM; now it is only unset if PWM was actually turned off (which in turn implies that if it was inverted, that was done by analogWrite(pin,255) - that's how we can generate a continuous HIGH output without disconnecting the timer (which would produce a glitch on the other pwm channel)- we set the compare value higher than TOP (which would produce continuous low) and invert the pin.
* In the process of above, found both a terribly inefficient bit of code in analogWrite and turnOffPWM, and replaced with a much faster implementation that saves some flash too! By switching to bit_mask for most calculations a variable shift which could run as many as 5 iterations at 4 clocks each was eliminated from analogWrite targeting a type A timer.
* Correct multiple bugs in takeOverTCAn() functions, which were missed during testing.

## Released Versions

### 1.3.1
* Correct Servo library so that it actually works; 1.3.0 version was apparently never compile-tested (nor was it even based on a clean copy of the library from megaTinyCore)
* We were waiting for ENRDY to be set before changing enable-protected registers of TCD0. That doesn't appear to be needed. We just can't re-enable it until ENRDY is set. It makes the glitch when turning PWM off or on... maybe 6 CLK_PER shorter? I think under default settings, the loop goes from one iteration to none....
* Include License file and clarify license.
* Specify the name of the bootloader binaries for AVR DB-series parts. (#65)
* Correct filename for bootloader binaries when using serial adapter + resistor programming method. (the two programming tools use different syntax for specifying parameters; for the most part this is handled in platform.txt - except that boards which do not use a bootloader do not pass a file to the upload tool, and the platform.txt substitution syntax doesn't provide a mechanism to do something like 'If bootloader.file is defined, -Uflash:"{bootloader.file}" otherwise ""' so the workaround is to have the with-bootloader boards.txt entries include the whole command argument if they specify a bootloader. The ones for pymcuprog were missing the .hex after the name of and path to the bootloader hex file.
* Initialize the ADC. This was accidentally forgotten, and without it, analogRead() would simply hang, waiting for the disabled ADC to return a reading. (#63)
* Correct frequency of the type A timers when used for millis or PWM, and the type B timers when used for PWM.
* Initialize TCD0 to count prescale = 32 whenever F_CPU > 1 MHz. At 1 MHz, prescale of 4 will be used instead
* Adapt analogWrite(pinOnTCD0) to check `CMPBCLR`. It's a crude test (to be fast) but if high byte is 1 (509), leftshift 1, if 3 (for 1019) <<= 2 (F_CPU >=32 MHz only: if 7, <<=3 (for 2039)) - one of very few cases where direct registry changes are not only permitted but adapted to intelligently.
* Adapt PWM and timer usage document inherited from megaTinyCore to (more) accurately describe the state of affairs on these parts.
* Implement flexible PWM on TCD channels.
* Removed option to disable MVIO via fuse - there does not appear to be any benefit to this option, and setting it when the hardware is wired for MVIO  looks likely to cause hardware damage. (#58)
* Linting - fix huge number of trivial issues like broken links, newlines where they shouldn't be, and that kind of crap.
* TCA0, TCA1 pin assignments now dynamic - if you change PORTMUX.TCAROUTEA, analogWrite() will understand it! (6-channel mappings only - TCA1 on PC4-6 and PE4-6 not supported)
* takeOverTCA0, takeOverTCA1, and takeOverTCD0 added.
* Add a bunch of TCAn portmux helpers to DxCore.h. Adapt variant files to remove type A timers from the digital_pin_to_timer table, and otherwise account for these changes.
* Add in the Opamp library - Thanks @MCUdude!

### 1.3.0
#### New Features
* **NEW toolchain package - support for all DA and DB parts on all platforms!** (board manager only)
* Pull in new version of avrdude to **support Dx-series Curiosity** boards via avrdude. (board manager only)
* **Add support for pymcuprog-backed pyupdi-style programming with serial adapter as programmer (#38)**
* **Add new Event system library** to go with the new version of Logic! (#52)
* digitalWriteFast(), digitalReadFast() are now available for single-cycle pin writes! This also comes along with the compile time error checking for most digital and analog API functions.
* Safe fuses automatically written on all UPDI uploacs: Set all fuses that we ever set in response to menu selections except for BODCFG on all uploads to non-Optiboot configurations, and all "upload using programmer" to optiboot configurations. Fuses that the core does not provide a method of specifying are not touched by the normal upload process; if you have gone and changed one, it is assumed that it was intentional.
* The Burn bootloader command will set BODCFG, as well as resetting all fuses to their defaults.
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
* Freshly built toolchain that I'm hosting in S3 to accommodate the 128k DB-series parts and the smaller-flash DA-series. THESE SHOULD NOW WORK!
* Minor doc improvements
