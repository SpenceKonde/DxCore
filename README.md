# DxCore - Arduino support for the AVR DA, DB-series and DD-series

## Prize: $50 merchandise of your choice from my tindie store
To anyone who can find an alternative to astyle we can use in place of astyle in the current formatting check, but which handles the preprocessor directive. [Read more](https://github.com/SpenceKonde/AVR_Research/blob/main/GetFiftyBucks_for_stylechecker.md)

## There is a bug in recent versions of the IDE and the CLI
It prevents installation of custom cores by mangling the file name of the json file when it saves it, and then failing to find it because they're using the correct filename (but the file is named incorrectly) Really, there have been a TON of bugs in the 2.x versions, I'm not sure if any "good" versions of the 2.x codebase exist - they have been introducing bugs as fast as they fix them.

For Arduino-ci, in your workflow under with:, add `cli-version: 0.33.0` to make CI that uses third party hardware packages work correctly; see our compile-examples workflow.

## ~1.5.9~ 1.5.10 is here
This is a pretty big one, 1.5.10 fixes huge problems in 1.5.9 that went in after the CI stopped working due to an apparent regression in Arduino-cli. CI now works again.
* EA series now compiles. Still working on the upload.
  * Event.h may not work correctly with pin events.
* The problems with the bootloaders and with Flash.h are corrected (entry conditions work now). All users on optiboot should burn bootloader with the 1.5.9 version when it is released.
* A large number of significant fixes have gone in, forcing this release when I had not planned on a release, as there are many unachieved goals.
* EA-series CI. For a brief time, now CI is down until I can get https working on drazzy.com. I have a cert, but apache steadfastly refuses to use it.
* A few particularly excruciatingly long markdown files got a TOC.
* Compatibility warning: Versions of the IDE around 2.2.1, which use the latest versions of Arduino-cli, do not seem compatible with third party hardware packages, as these versions drop pieces of the filename from the json file, and then fail to find it.

## WE HAVE A PROBLEM: WE HAVE NO PINMAP IMAGES
I thought I had a tool contributed, but I don't think I ever got the final version, nor do I have any of the images.

We are now TWO GENERATIONS OF CHIPS behind the eight ball on pinout diagrams. I have not the talent for making them.

Our existing images *should really be updated*

## WE HAVE ANOTHER PROBLEM: Our link checker is blocked by Microchip
This means we have no way to check if a link is valid. Have reached out to my guy at Microchip to seeif anything can be done.

We may be forced to remove all links to their website which would suck.

### Optiboot for EA does not look encouraging
Before the errata, I was pretty sure that it would be part definitions and a few lines of logic change in optiboot_x. oh ho ho no. No such luck, cause we can't do a bloody erase+write, the optiboot protocol doesn't know where a given page is going to be written (hence where to erase) to until the first bytes of the blob of data that was sent to it is hurtling towards it, and see the quoted section of the datasheet below. I am praying for an early die rev.

> The resulting data will be a binary AND operation between the new and the previous content of the page buffer. The page buffer will automatically be erased (all bits set) **after**:
> * A device Reset
> * Any page write **or erase** operation
> * A Clear Page Buffer command
> * A device wake-up from any sleep mode
> Note: **Any operation on the page buffer will halt the CPU** until the previous NVMCTRL operation (command) is completed.


Emphasis mine, in other words, since erase write is broken per errata, we have to do an erase, then a write, and we can't start the former until the data for the latter is coming at us.

That means instead of putting incoming data straight into the page buffer, we have to put it into a software buffer while we wait for the erase to finish. Once the erase finishes, copying the data from the software buffer to the page buffer should take a negligible amount of time (640 clock cycles I calculate, or just 32-40 us - an eyeblink compared to the rest of what we're doing here, and then we would kick off the write and acknowledge getting the page, and start buffering the next one)

Hence we are forced to put incoming data into a software buffer (though if the page is in the RWW section, we can kick it off as soon as we know which page we're writing to save time, then at the end of the page, we have to copy the data from the software buffer to page buffer, execute a write, wait for it to complete and then acknowledge the write. Ideally we want to immediately acknowledge the write when we start it, so we can be receiving more data for the next page and putting it into the software buffer, and then, when the write is done, kick off the erase without missing any characters. That method would maximize the speed of the write at the slower of the two: protocol overhead plus time to erase and then write a page, and time required to transmit a page to the target device at the selected baud rate. With AVRdude 7.1 as standard planned for 1.6.0 at the latest, we can break through the 115200 baud barrier. 230400? 460800? Well, the sum of page erase and page write time is 10 ms. A page is 128 bytes, plus 1-2 ms USB latency. 128 bytes = 1024 bits. Because of the start and stop bit, best case, it takes 10/8ths that long to upload it, since 1280 bits is required to transmit that data (not counting overhead). So, `10ms >= ((1300 to 1400)/baud) + 2ms`. Of course until we get silicon that works correctly, this is largely adademic.

Simplify:

`8ms >= (1300 to 1400)/baud`

Convert to consistent units, and multiply both sides by baud rate which has units of bits/seconds.

`0.008s >= (1300 b to 1400 b)/baud (b/s)`

`0.008s * baud (b/s) >= 1300 b to 1400 b`

Separate the two extremes, divide both sides by the length of time to get the baud rates that will reach the speed limit. I think I did that right. Dimensional analysis looks okay at least.

`baud (b/s) >= 1300b / 0.008s`

`baud = 162500 to 175000`

Of course we'd need a different case to handle the times when we were in the NRWW section, where we must basically be a Dx and stuff the page into a buffer, then erase, then copy to page buffer then write, and finally after all that, we could acknowledge the write. That'll make the bootloader larger (though we'll be starting from Optiboot_x instead of Optiboot_dx - optiboot_dx has a single digit number of instruction words of flash left in some configurations before having to step up bootloader size. And different bootloader sizes are not binary compatible, nor can binaries compiled for one size be readily converted to those for a different size, so changing the size of the bootloader is a BFD on modern AVR.)

Are those calculations right? The maximum baud rate that will improve the rate at which data is uploaded via optiboot is in the 160,000-180,000 baud area? We're currently stuck at 115200 due to old avrdude version, but I was hoping we could go at least twice as fast. A 64k chip with 128b pages has 512 pages, and if we're programming them at 100/second, that's 5.12s minimum for a full upload which I guess isn't terrible but gee, 16kb/sec absolute speed limit? That beats old optiboot speeds for sure, but that speed limit would apply to UPDI in the general case too... And we could do 24k/s, which would entail transmitting overhead + 82k, 82000/5.12 okay - 160,000. So the two calculations agree. We'd get something out of 172,800 baud (1.5x 115200) but beyond that we'd see little if anything from 230400, and after that, nothing except for a small improvement in NRWW write speed from bumping baud rate. Did I do the math right up there?

## Table of Contents
* [DxCore - Arduino support for the AVR DA, DB-series and DD-series](README.md#dxcore---arduino-support-for-the-avr-da-db-series-and-dd-series)
  * [1.5.9 approaches](README.md#159-approaches)
  * [WE HAVE A PROBLEM: WE HAVE NO PINMAP IMAGES](README.md#we-have-a-problem-we-have-no-pinmap-images)
  * [WE HAVE ANOTHER PROBLEM: Our link checker is blocked by Microchip](README.md#we-have-another-problem-our-link-checker-is-blocked-by-microchip)
    * [Optiboot for EA does not look encouraging](README.md#optiboot-for-ea-does-not-look-encouraging)
  * [Table of Contents](README.md#table-of-contents)
  * [IMPORTANT WARNINGS](README.md#important-warnings)
    * [ATTN: Linux Users](README.md#attn-linux-users)
    * [IDE 2.0.x unsupported](README.md#ide-20x-unsupported)
  * [What is DxCore](README.md#what-is-dxcore)
  * [Supported Parts (click link for pinout diagram and details)](README.md#supported-parts-click-link-for-pinout-diagram-and-details)
    * [Magic Packages](README.md#magic-packages)
      * [VQFN's aren't so bad](README.md#vqfns-arent-so-bad)
    * [Quick Summary](README.md#quick-summary)
  * [Supported Clock Speeds](README.md#supported-clock-speeds)
    * [For the DA, DB, DD and DU-series parts](README.md#for-the-da-db-dd-and-du-series-parts)
    * [For the EA and EB-series](README.md#for-the-ea-and-eb-series)
  * [UPDI Programming](README.md#updi-programming)
    * [We set fuses when writing a sketch, except where specifically noted](README.md#we-set-fuses-when-writing-a-sketch-except-where-specifically-noted)
    * [UPDI programming hardware](README.md#updi-programming-hardware)
    * [From a USB-Serial Adapter With SerialUPDI (pyupdi-style - Recommended)](README.md#from-a-usb-serial-adapter-with-serialupdi-pyupdi-style---recommended)
    * [What about Debugging?](README.md#what-about-debugging)
    * [HV debrick project delayed](README.md#hv-debrick-project-delayed)
    * [What's With All The Different SerialUPDI Options?](README.md#whats-with-all-the-different-serialupdi-options)
    * [Why is My FTDI Adapter Insanely Slow?](README.md#why-is-my-ftdi-adapter-insanely-slow)
    * [With a Classic Arduino (jtag2updi)](README.md#with-a-classic-arduino-jtag2updi)
  * [So we can do HV UPDI on the DD's?](README.md#so-we-can-do-hv-updi-on-the-dds)
  * [Compatibility Note for 32-bit Linux](README.md#compatibility-note-for-32-bit-linux)
  * [Ways to refer to pins](README.md#ways-to-refer-to-pins)
    * [PIN_Pxn Port Pin Numbers (recommended)](README.md#pin_pxn-port-pin-numbers-recommended)
    * [Arduino Pin Numbers (if you must)](README.md#arduino-pin-numbers-if-you-must)
    * [An and PIN_An constants (for compatibility)](README.md#an-and-pin_an-constants-for-compatibility)
    * ["Missing" Pins](README.md#missing-pins)
  * [Link-time Optimization (LTO) support](README.md#link-time-optimization-lto-support)
  * [Exposed Hardware Features](README.md#exposed-hardware-features)
    * [MVIO (DB, DD only)](README.md#mvio-db-dd-only)
    * [ADC Support](README.md#adc-support)
    * [DAC Support](README.md#dac-support)
    * [Watchdog timer, software reset](README.md#watchdog-timer-software-reset)
    * [Improved Digital I/O](README.md#improved-digital-io)
    * [Serial (UART) Support](README.md#serial-uart-support)
    * [SPI support](README.md#spi-support)
    * [I2C (TWI) support, including all alternate pins, master & slave from the same TWI](README.md#i2c-twi-support-including-all-alternate-pins-master--slave-from-the-same-twi)
    * [Alternate pin mapping (PORTMUX) support](README.md#alternate-pin-mapping-portmux-support)
    * [PWM on LOTS of pins](README.md#pwm-on-lots-of-pins)
    * [EEPROM - Yes](README.md#eeprom---yes)
    * [USERROW - Yes](README.md#userrow---yes)
    * [Pin Interrupts](README.md#pin-interrupts)
      * [`attachInterrupt()` rework](README.md#attachinterrupt-rework)
    * [Additional supported peripherals](README.md#additional-supported-peripherals)
      * [On-chip Op-amps](README.md#on-chip-op-amps)
      * [Analog Comparators](README.md#analog-comparators)
      * [Configurable Custom Logic](README.md#configurable-custom-logic)
      * [Event System](README.md#event-system)
      * [Zero-Crossing Detector](README.md#zero-crossing-detector)
      * [Timers](README.md#timers)
        * [Timer types](README.md#timer-types)
  * [Other major features](README.md#other-major-features)
    * [Memory-mapped flash? It's complicated](README.md#memory-mapped-flash-its-complicated)
    * [Writing to Flash from App](README.md#writing-to-flash-from-app)
    * [Servo Support](README.md#servo-support)
    * [`printf()` support for "printable" class](README.md#"printf-support-for-printable-class)
      * [**WARNING** `printf()` and variants thereof Have Many Pitfalls](README.md#warning-printf-and-variants-thereof-have-many-pitfalls)
      * [Selectable `printf()` Implementation](README.md#selectable-printf-implementation)
    * [Interrupts From Pins and in General](README.md#interrupts-from-pins-and-in-general)
    * [Assembler Listing generation](README.md#assembler-listing-generation)
    * [EESAVE configuration option](README.md#eesave-configuration-option)
    * [BOD configuration options](README.md#bod-configuration-options)
    * [NeoPixels (aka WS2812/SK6812 and many, many others)](README.md#neopixels-aka-ws2812sk6812-and-many-many-others)
    * [Tone Support](README.md#tone-support)
    * [millis/micros Timekeeping Options](README.md#millismicros-timekeeping-options)
    * [Improved digital I/O](README.md#improved-digital-io)
  * [Additional DxCore documentation](README.md#additional-dxcore-documentation)
    * [Library documentation](README.md#library-documentation)
  * [Bootloader (Optiboot) Support](README.md#bootloader-optiboot-support)
  * [Guides](README.md#guides)
    * [Reference Material (these are what I would call the documentation)](README.md#reference-material)
      * [Function Reference](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Functions.md)
      * [Analog Input (ADC) and Output (DAC)](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Analog.md)
      * [Digital I/O and enhanced options](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Digital.md)
      * [Interrupts](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Interrupts.md)
      * [Timers and PWM](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Timers.md)
      * [TCD0](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_TCD.md)
      * [Serial](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Serial.md)
      * [Mapped flash and PROGMEM in DxCore](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_PROGMEM.md)
      * [Optiboot Bootloader](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Optiboot.md)
      * [SerialUPDI](https://github.com/SpenceKonde/AVR-Guidance/blob/master/UPDI/jtag2updi.md)
      * [Clock Information](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Clocks.md)
      * [Callbacks/weakly defined functions](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Callbacks.md)
      * [Constants for part/feature/pin/version identification](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Defines.md)
      * [Assembly Listing and Memory Maps](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Export.md)
      * [Reset control and the WDT](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Reset.md)
      * [Considerations for robust applications](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Robust.md)
      * [Power Saving techniques and Sleep - inherited from megaTinyCore](megaavr/extras/PowerSave.md)
      * [Link time Optimization](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_LTO.md)
  * [List of Tools sub-menus](README.md#list-of-tools-sub-menus)
    * [Optiboot only menu options](README.md#optiboot-only-menu-options)
    * [Options not available with Optiboot](README.md#options-not-available-with-optiboot)
  * [Support Continued Development](README.md#support-continued-development)
  * [Warnings and Caveats](README.md#warnings-and-caveats)
    * [Direct Register Manipulation](README.md#direct-register-manipulation)
      * [Timer PWM and exceptions](README.md#timer-pwm-and-exceptions)
  * [Differences in Behavior between DxCore and Official Cores](README.md#differences-in-behavior-between-dxcore-and-official-cores)
    * [I2C **requires** external pullup resistors](README.md#i2c-requires-external-pullup-resistors)
    * [Serial Does Not Manipulate Interrupt Priority](README.md#serial-does-not-manipulate-interrupt-priority)
    * [SerialEvent Support is Dropped](README.md#serialevent-support-is-dropped)
    * [`digitalRead()` does not turn off PWM](README.md#digitalread-does-not-turn-off-pwm)
    * [`digitalWrite()`/`pinMode()` and INPUT pins](README.md#digitalwritepinmode-and-input-pins)
    * [`analogWrite()` and TCD0 pins](README.md#analogwrite-and-tcd0-pins)
    * [Serial does not manipulate interrupt priority](README.md#serial-does-not-manipulate-interrupt-priority)
    * [Serial setting constants have different numeric values](README.md#serial-setting-constants-have-different-numeric-values)
    * [TCA(s) are configured in Split Mode to get 3 additional PWM pins](README.md#tcas-are-configured-in-split-mode-to-get-3-additional-pwm-pins)
    * [TCA0/1 and all TCB's used for PWM have TOP at 254, not 255](README.md#tca01-and-all-tcbs-used-for-pwm-have-top-at-254-not-255)
    * [digital I/O functions use old function signatures](README.md#digital-io-functions-use-old-function-signatures)
    * [`analogReadResolution()` is different](README.md#analogreadresolution-is-different)
    * [As of 1.3.3, SerialEvent is removed](README.md#as-of-133-serialevent-is-removed)
    * [Oh, and -Wall (compile warnings) are enabled no matter what you choose in the preferences](README.md#oh-and--wall-compile-warnings-are-enabled-no-matter-what-you-choose-in-the-preferences)
  * [Instruction Set Enhancements (AVRe/AVRe+ vs AVRxt)](README.md#instruction-set-enhancements-avreavre+-vs-avrxt)
  * [License](README.md#license)

## IMPORTANT WARNINGS

### ATTN: Linux Users
**Only versions of the Arduino IDE downloaded from [arduino.cc](https://arduino.cc) should be used, NEVER from a Linux package manager. The package managers often have the Arduino IDE - but have *modified it*. This is despite their knowing nothing about Arduino or embedded development in general, much less what they would need to know to modify it successfully** Those version are notorious for subtle but serious issues caused by these unwise modifications. This core should not be expected to work on such versions, and no modifications will be made for the sake of fixing versions of the IDE that come from package managers for this reason.

### IDE 2.0.x unsupported
If you use it, use the release not an old RC: all versions prior to 2.0.0-RC9.2 known to have critical regressions. These bugs in the IDE prevent board settings from being correctly recognized. [This thread tracks known issues with 2.0 and workarounds](https://github.com/SpenceKonde/megaTinyCore/discussions/760). If you use unsupported software please reproduce all issues in 1.8.13 before reporting.

## What is DxCore
This is an Arduino core to support the exciting new AVR DA, DB, and DD-series microcontrollers from Microchip. These are the latest and highest spec 8-bit AVR microcontrollers from Microchip. It's unclear whether these had been planned to be the "1-series" counterpart to the megaAVR 0-series, or whether such a thing was never planned and these are simply the successor to the megaAVR series. But whatever the story of their origin, these take the AVR architecture to a whole new level.  With up to 128k flash, 16k SRAM, 55 I/O pins, 6 UART ports, 2 SPI and I2C ports, and all the exciting features of the tinyAVR 1-series and megaAVR 0-series parts like the event system, type A/B/D timers, and enhanced pin interrupts... Yet for each of these systems they've added at least one small but significant improvement of some sort (while largely preserving backwards compatibility - the tinyAVR 2-series also typically adds the new features that the Dx-series get, giving the impression that these reflect a "new version"). You like the type A timer, but felt constrained by having only one prescaler at a time? Well now you have two of them (on 48-pin parts and up)! You wished you could make a type B timer count events? You can do that now! (this addresses something I always thought was a glaring deficiency of the new peripherals and event system). We still don't have more prescale options (other than having two TCA's to choose from) for the TCB - but you can now combine two TCBs into one, and use it to do 32-bit input capture. Time a pulse or other event up to approximately 180 seconds long... to an accuracy of 24ths of a microsecond! And of course, like all post-2016 AVR devices, these use the latest incarnation of the AVR instruction set, AVRxt, with slightly-improved instruction timing compared to "classic" AVRs.

For a basic overview of the parts and a comparison table, see [General AVR Dx-series and Ex-series information](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/AboutDxSeries.md)

## Supported Parts (click link for pinout diagram and details)
Note that you must install via board manager or replace your tool chain with the azduino4 version pulled in by board manager in order to work with anything other than an AVR128DA. Note also that there is a defect in some of the earliest-shipped AVR32DA parts not acknowledge properly by Microchip; those parts do not correctly use interrupts and are not functional. They are not supported. They cannot be readily distinguished other than by noticing that interrupts do not work, and complaining to Microchip support with the lot number. Likely they'll give you new ones if you've got bad AVR32DA's

All of the pinout diagrams have gotten really ugly from my MS-paint hacking, and some of them don't exist at all. Please help.

* [AVR128DA28, AVR64DA28, AVR32DA28](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DA28.md) (Need help with better pinout charts!)
* [AVR128DA32, AVR64DA32, AVR32DA32](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DA32.md) (Need help with better pinout charts!)
* [AVR128DA48, AVR64DA48, AVR32DA48](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DA48.md) (Need help with better pinout charts!)
* [AVR128DA64, AVR64DA64](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DA64.md)  (Need help with better pinout charts!)
* [AVR128DB28, AVR64DB28, AVR32DB28](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DB28.md) (Need help with better pinout charts!)
* [AVR128DB32, AVR64DB32, AVR32DB32](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DB32.md) (Need help with better pinout charts!)
* [AVR128DB48, AVR64DB48, AVR32DB48](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DB48.md) (Need help with better pinout charts!)
* [AVR128DB64 and AVR64DB64](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DB64.md) (Need help with better pinout charts!)
* [AVR64DD14, AVR32DD14, AVR16DD14](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DD14.md) (Need help with better pinout charts!)
* [AVR64DD20, AVR32DD20, AVR16DD20](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DD20.md) (Need help with better pinout charts!)
* [AVR64DD28, AVR32DD28, AVR16DD28](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DD28.md) (Need help with better pinout charts!)
* [AVR64DD32, AVR32DD32, AVR16DD32](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DD32.md) (Need help with better pinout charts!)
* [AVR64EA28, AVR32EA28, AVR16EA28, *AVR8EA28*](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/EA28.md) (Need help with any pinout charts!)
* [AVR64EA32, AVR32EA32, AVR16EA32, *AVR8EA32*](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/EA32.md) (Need help with any pinout charts!)
* [AVR64EA48, AVR32EA48, AVR16EA48](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/EA48.md) (Need help with any pinout charts!)
* [*AVR32DU14, AVR16DU14* (pending release - `*`)](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DU14.md) (Need help with any pinout charts!)
* [*AVR32DU20, AVR16DU20* (pending release - `*`)](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DU20.md) (Need help with any pinout charts!)
* [*AVR64DU28, AVR32DU28, AVR16DU28* (pending release)](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DU28.md) (Need help with any pinout charts!)
* [*AVR64DU32, AVR32DU32, AVR16DU32* (pending release)](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DU32.md) (Need help with any pinout charts!)
* [*AVR32EB14, AVR16EB14, AVR8EB14* (pending release)](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/EB14.md) (Need help with any pinout charts!)
* [*AVR32EB20, AVR16EB20, AVR8EB20* (pending release)](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/EB20.md) (Need help with any pinout charts!)
* [*AVR32EB28, AVR16EB28, AVR8EB28* (pending release)](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/EB28.md) (Need help with any pinout charts!)
* [*AVR32EB32, AVR16EB32, AVR8EB32* (pending release)](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/EB32.md) (Need help with any pinout charts!)

Part names *in italics* denote parts for which support is not yet available, as they are future products and no silicon is available.

Everything needed to make DU pinout charts is currently known. I've been told it is coming, possibly by the end of the year. But he didn't actually specify which year, and no matter when it comes out, it will come out before the end of the year in the year that it's released...

`*` On the DD they (likely) weren't able to fit the 64k die into a QFN20. appears true here too. Unclear whether 64k DU14 was scrubbed because it made it's 20-pin counterpart look fat, or or it's 20-pin counterpart wasn't going to exist and they didn't want a hole in the product line. Or maybe someone realized that after USB and power, UPDI and reset, there were only 7 available pins left. It may not entirely make sense to offer something with 64k of flash and only 7 available pins.

My personal opinion is that the 48-pin parts are the "sweet spot" for the DA and DB-series parts - they have the real gems of the product line - the second Type A timer, the two extra CCL LUTs, and enough pins to take full advantage of these peripherals. Most people can't really find something to do with a whole 64 pins in one project - short of indulging in kitchen-sinkism just to take up pins. But the 27 I/O pins on the 32-pin parts can get used up faster than one might think (I had one project a while back where I switched to a '328PB instead of a '328P for the Rev. B, because otherwise I was 1 pin short of being able to lose the I2C backpack on the '1602 LCD, and if I did that, I could integrate the whole thing onto one PCB, and have a rigid connection between the LCD and main PCB - and then I thought I would be fine with a 32-pin Dx as that had a few more pins... But I wound up switching to the 48 and am using about half of the added pins.

For the DD-series, the 28 and 32-pin parts offer obvious economic benefits, but no new capability versus the DB (other than the very cheap VQFN28 package). The smaller versions however offer capability well beyond the tinyAVR product line in the same tiny package sizes.

### Magic Packages
These are packages, that when offered, open up new frontiers

| Pincount | Package | Magic
|----------|---------|---------------------
|       14 |   TSSOP | Can fit on a .425" wide board that plugs into DIP socket
|       20 |3x3 VQFN | As above. Generally insanely tiny, though often large flash sizes don't make it in.
|       28 |    VQFN | Very well priced when available.
|       48 |     Any | 48 has often been a break point at which parts get additional peripherals and features, making the 48-pin parts often particularly attractive.


#### VQFN's aren't so bad
Everyone always hates on QFN packaging, but it has a few advantages. The obvious one is size, the VQFN allows the 48 pin chip to easily fit into an Arduino Nano-width board. But surprisingly, Both are markedly easier to solder at home if you have a reflow oven (I'm thinking converted toaster oven or cheapo chinese one) or hotplate compared to the fine pitch TQFPs which almost always suffer solder bridges. As much as 80% of my TQFP packages soldered via reflow require at least one bridge hand cleared, of those, maybe 10-25% require rework on a hotplate because the chip has gone cockeyed. In the case of VQFN the latter fraction stays about the same - but instead of 20% yield without rework, I see more like 80%. QFN gets easier the larger the package gets, because of the magic of surface tension and that exposed pad on the bottom. As long as your soldering method melts that (either reflow, hot air, or hotplate is thus best), you're better off with the VQFN, because the EP supplies a strong self-centering, self-aligning force compared to the pins on TQFP. I

### Quick Summary
| Feature   | t0 | t1 | t2 | DA | DB | DD | EA | DU | EB |
|-----------|----|----|----|----|----|----|----|----|----|
| Max Flash | 16 | 32 | 32 |128 | 128| 64 | 64 | 64 | 32 |
| Min Flash | 2  | 2  | 4  | 32 | 32 | 16 | 8  | 16 | 8  |
| Max RAM   | 1  | 2  | 3  | 16 | 16 | 8  | 6  | 8  | 3  |
| Pins max  | 24 | 24 | 24 | 64 | 64 | 32 | 48 | 32 | 32 |
| Pins min  | 8  | 8  | 14 | 28 | 28 | 14 | 28 | 14 | 14 |
| EEPROM    | 256| 256| 256| 512| 512| 256| 512| 512| 512|
| USERROW   | 32 | 32 | 32 | 32 | 32 | 32 | 64 | 512| 64 |
| BOOTROW   | -  | -  | -  | -  | -  | -  | -  | 64 | 64 |
| TCA's     | 1  | 1  | 1  | 2  | 2  | 1  | 2  | 1  | 0  |
| TCB's     | 1  | 2  | 2  | 5  | 5  | 3  | 4  | 2  | 2  |
| TCD's     | -  | 1  | -  | 1  | 1  | 1  | -  | -  | -  |
| TCE's     | -  | -  | -  | -  | -  | -  | -  | -  | 1  |
| WEX       | -  | -  | -  | -  | -  | -  | -  | -  | 1  |
| TCF's     | -  | -  | -  | -  | -  | -  | -  | -  | 1  |
| CCL       | 4  | 4  | 4  | 6  | 6  | 4  | 4  | 4  | 4  |
| MVIO      | -  | -  | -  | -  | X  | X  | -  | -  | -  |
| EVSYS     | 3  | 6  | 6  | 10 | 10 | 6  | 6  | 6  | 6  |
| ADC bits  | 10 | 10 | 12 | 12 | 12 | 12 | 12 | 10?| 12 |
| Has PGA   | -  | -  | X  | -  | -  | -  | X  | -  | X  |
| Released  | X  | X  | X  | X  | X  | X  |some| -  | -  |
| USB       | -  | -  | -  | -  | -  | -  | -  | X  | -  |
| Core      | mTC| mTC| mTC| DxC| DxC| DxC| DxC| DxC| DxC|

t1/t2: Migration path for classic tinyAVR, excellent for small applications in general (frequently better than DD/EX-series parts if you need all the peripheral pins of all the peripherals - For example, a CCL-heavy application might skate by with a 424 (I have one that does), but, well, a DD14 don't have enough pins I can get CCL out on, )
t0: If it cost 25% less than the t1's I might give them a second look. They cost around 1-2% less. So I don't know who the hell buys this garbage.
DA or DB: Migration path for high end megaAVR applications
DD: Migration path for low cost megaAVR - and an alternative more powerful chip than the tinyAVRs in low pincounts. Writing on the wall about tinyAVR now.
EA: Migration path for people who used megaAVRs and actually used the differential ADC in ways that took advantage of it's differentiality.
DU: Migration path for 32u4, 16u2, etc. And for the refugees from VUSB-equipped tinyAVRs that really can't act as USB devices. As long as the USB implementation isn't totally botched, these will also open up new doors for AVR. DD14-based dual serial adapters anyone? With one of the ports in one-wire mode for UPDI programming?
EB: Migration path for tiny861 in low pincounts (these will be the first modern AVRs that can dance around an 861 singing "anything you can do I can do better" loud enough to drown out the 861 belting out "I got a badass ADC and drive a BLDC"), and some of the assorted weird old PWM oriented AVRs in larger pincounts that Atmel had kinda left with a paucity of compelling newer options (maybe ATmega64M1?). With 8-32k flash, this is a strike at the heart of AVR territory. Writing on the wall for tinyAVR branding has now been photographed, printed, and nailed to the lid of its sealed coffin.

## Supported Clock Speeds
### For the DA, DB, DD and DU-series parts
The maximum rated spec is 24 MHz **across the entire voltage and temperature range.**
And that temperature range is pretty nuts - 105C for I-spec and 125C for E-spec. With waterproofing, you could run at the bottom of a pot of boiling water! Maybe you're making a still for makin moonshine, but you want the control system entirely internal to make it easier to camouflage. There are lots of reasons not to do this (legal, chemical, flavor, sanity), but the temperature range of the AVR isn't one of them. Even if it's sitting in boiling water, that's no hotter than 100C.
The internal oscillator can be used at 1 MHz, or any increment of 4 beyond that up to and including 32 MHz (note that this is 1/3rd more than max rating). For compatibility with tinyAVR, we also offer 5/10 MHz (generated by dividing 20 MHz).

All parts can use an external clock, and DB and DD-series parts can also use a crystal.

At room temperature, you can overclock the bejeezus out of these things. Crazy overclocks are possible. Many E-spec parts run from crystal or clock at 48 MHz room temp, I spec parts usually do 40. In limited (because it wasn't very interesting) testing, I confirmed that 128 MHz from the 4x multiplication setting of the PLL (undocumented) also worked at room temperature. However on the same specimen 160 MHz proved too much to ask. It was trying, but dropping cycles left and right, especially when things were being sync'ed to that clock domain. I think it was oscillating at the desired speed, but it looked as if the rest of the timer couldn't keep up. I don't recall if that was E or I spec.

**Supported from internal:** 1 MHz, 4 MHz, 5 MHz, 8 MHz, 10 MHz, 12 MHz, 16 MHz, 20 MHz, 24 MHz, 28 MHz, 32 MHz

**Supported from external or crystal (crystal not supported by DA):** 8 MHz, 10 MHz, 12 MHz, 16 MHz, 20 MHz, 24 MHz, 28 MHz, 32 MHz, 36 MHz`*`, 40 MHz`**`, 48 MHz`**`

If a watch crystal is installed, there is an option to "Auto-tune" the internal oscillator based on that, though the improvement is small except at extreme temperatures due to the granularity of the tuning. Note that this does not allow generation of clock speeds not natively supported. The tuning is based on the intermediate 1 MHz frequency from which all others are derived.

The DU will likely be similar to the other Dx parts. It is highly likely - though not certain, they've been doing more with multiple clock domains on these recent parts - that only a limited number of speeds will be compatible with USB. Because all indications are that it has made great sacrifices in exchange for the USB, and hence would not be likely to see use in non-USB applications, chances are that we will only offer support for USB-compatible speeds, because if you aren't using USB, the other parts in the Dx-series would be more appropriate and effective.

`*` Speeds over 32 MHz prevent the OPAMP settling time from being set correctly.
`**` Speeds of 40 MHz may or may not be achievable on I-spec parts at room temperature. Try to get E-spec parts if you plan to overclock, especially that much.
`***` Gee, the nerve of them! I don't have a prayer of getting it to run from a crystal twice the rated speed while generating PWM derived from a 196 MHz clock now. Maybe if I chilled it to below ambient temperature with a peltier junction. Condensation would be a problem, I'd need to use a dry cold chamber insulated very well At least I just scored some monster heatsinks., . Maybe I could just dunk the whole board in acetone and chill that with dry ice... (the cold CO2 coming off the dry ice will keep the acetone from catching fire. You could probably throw a lit match into it and have it go out, especially if it wasn't very windy (you'd be doing this outside for hopefully obvious reasons). I wonder how high I could get the main CPU like that (though it is far below the specified minimum operating temperature. And the min temp of many crystals, which as I recall often to just straight up crap out at low temperatures, while CPUs rarely do, -40C is just as low as anyone bothers to characterize). Could also submerge in boiling tetraflouroethane (computer duster), except that this is extremely environmentally irresponsible

See the [Clock Reference](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Clocks.md) for more information

### For the EA and EB-series
The maximum rated clock speed is 20 MHz. Assuming that the actual silicon behaves as the headers imply - and this is borne out by the EA-series, though note that the EB-series is getting a significant update to CLKCTRL, so there may be additional options available there. Or maybe not. I mean, you could probably use it for overclocking, but the maximum speed is rated 20, and the hardware requires a 5-bit field to be set to 1 less than the number of clock cycles, rounded up, equal to one microsecond, so overclocking is limited to 32 MHz if the silicon can keep up (it likely can - 2-series tinies could, and if anything these should be more able to function at high F_CPU, that means 1 MHz, 2 MHz, 4 MHz, 5 MHz, 8 MHz, 10 MHz, 12 MHz, 16 MHz, 20 MHz internal will be the options, like a tinyAVR.

The EA and EB series CLKCTRL is tinyAVR-like not Dx-like, with 16MHz and 20 MHz derived clocks. Hopefully after they'll at least let us have an oscillator with the compliance of the 2-series tinyAVRs, not the pathetic calibration facilities we get on DxCore (headers already show we're not getting the Dx-style speed selector. But we may be able to tune like a tinyAVR 2-series). Update: NOPE! No fun for us! Dx-series tuning! That's rough. Reading that section of the datasheet felt like getting kicked in the groin, falling to the ground, and being kicked several more times. The EB is a wildcard - initial headers betray the existence of a greatly expanded PLL compared to the one on the Dx-series, and it can be used as the main clock source, which is likely to give some consolation to those who hope to continue life on the edge - though Ex-series has a problematic register for us: it wants us to give it a 5 bit number equal to the number of clock cycles per microsecond, rounded up, less 1, in order to time operations that take a given number of microseconds. For overclocks beyond 32 MHz (which, recall, is essentially universally achievable on a Dx-series part at room temperature), this obviously becomes problematic. Which in turn means that even if the silicon can take it (which I question - E-spec parts don't seem to be out yet, but once they are I'll snag one and see how high I can make it run, but I expect tinyAVR-like speed limits - that is, even with external oscillators, you can't hit any nice round convenient speeds above 32 MHz - the next one, at 40 MHz, represents a 100% overclock. That was possinble on Dx. Only time and experimentation will for Ex (the problem with the delay calculation is sidestepped by running in LOWLAT mode and manually discarding the first reading immediately after changing references or inputs).

The EA-series, but not the EB, will support an external crystal. No, I don't really understand why they hate MHz crystals so much either, but they sure seem to leave the crystal off of anything they can get away with now. And as the internal oscillator is no longer completely awful, there are a lot more chips where they can get away with it on, especially with almost all parts having the 32k crystal option for longer term clock accuracy. It was a pretty tough sell for Atmel to not accept external crystals when the alternative was those shitty +/- 10% internal oscillators, instead of +/-4% or +/-2% as the modern AVRs have. It becomes a much easier sell when you can offer 2% spec-sheet accuracy: And much much higher at room temperature the calibration is much better than that. In fact, they pretty much hit the room temp bullseye with the calibration now, so the difference in speed between the cal steps, divided in half, is the approximate maximum difference in speed between factory cal at room temp and an ideal oscillator. It's under half a percent.

One notable thing here is that they have **moved** a special function! Prior releases has XOSC32K on PF0 and PF1 as long as the parts had that, and only if they didn't did it fall back to PA0 and PA1 (on 14/20 pin DD series). On EB-series, which never support an HF xtal (nominally on PA0 and PA1), the 32 kHz crystal always goes between those pins and never between PF0 and PF1. I'd bet on this being an error in the product brief - I predict that the EB28+EB32 will move the LF crystal back where it belongs.

Were I a wagering man I might bet ten big ones on LF XTal on PF0, PF1 for EB's that have it. But if I were a wagering man, I'd be a lot poorer. I make predictions about future AVRs all the time. Most of them turn out to be wrong, but until they start placing wagers, nobody's really keeping track. So that's what the edit button is for (proven wrong? "Who even suggested that dumb idea? Huh I did? No wai! Where? Let me see...." *edit -> select sentence suggesting that false theory -> backspace -> commit* "... I'm looking at that right now, now where do I suggest such a preposterous idea?!" "right at... wait, I thought it was...." "You hallucinated an incorrect opinion and attributed it to me? I'm closing this issue. I may sometimes go by Dr. Azzy, but I think you need to see a proper doctor if these sort of hallucinations continue". "You know, it's funny how they only seem to occur when I call you out for being wrong" "But I'm never wrong, all of your allegations have proven specious!"). That sort of editing over time leaves only my most most prescient visions. Even if my predictions were worse than chance, by post-selecting only the correct ones and editing out the other predictions (which is only proper - is's irresponsible to propagate discredited ideas!), as a side-effect I appear significantly more like Nostradamus than a nostra-dumb-ass.

## UPDI Programming
The UPDI programming interface is a single-wire interface for programming (and debugging - **U**niversal **P**rogramming and **D**ebugging **I**nterface - naturally Microchip keeps the UPDI debug protocol under wraps to try to force you to use their tooling). It is used on all modern (post-2016/Microchip buyout/post-revolutionary) AVR microcontrollers, (though the AVRrc (reduced core) chips use different methods - they're also not suited to Arduino because they don't have nearly enough flash to fit the normal API. While one can always purchase a purpose-made UPDI programmer from Microchip, this is not recommended when you will be using the Arduino IDE rather than Microchip's (god-awful complicated) IDE. There are widespread reports of problems on Linux for the official Microchip programmers. There are two very low-cost alternative approaches to creating a UPDI programmer, both of which the Arduino community has more experience with than those official programmers. Hell, the one time I tried to use an official UPDI programmer, I couldn't even get Microchip's own tooling to see it.

### We set fuses when writing a sketch, except where specifically noted
Whenever a UPDI programmer is used to upload code, all fuses that can be set "safely" (as in, without risk of bricking the board, or bricking the board if one does not have access to an HV programmer), and which have any built-in configuration options, will be set. Thus, except where noted, behavior will always match the selected tools menu.
This is shown in the [linked chart on Google Sheets](https://docs.google.com/spreadsheets/d/1sH3yKzWRdVs0sI-pwLtoKDs16Gv14jkkbqlMrs2ziUI/edit?usp=sharing)

### UPDI programming hardware
While ISP was not a complex protocol it still required a microcontroller to implement it. This is *no longer true* - rather than being based on SPI it is based on UART serial in one-wire mode with autobaud. There are several inexpensive ways to make your own UPDI programmer from even just a serial adapter and a **small signal schottky** diode (no, you cannot use a normal silicon diode, and no you can't use that diode as big as a minivan that you bought to OR two power supplies a while back)


### From a USB-Serial Adapter With SerialUPDI (pyupdi-style - Recommended)
Before megaTinyCore existed, there was a tool called [pyupdi](https://github.com/mraardvark/pyupdi) - a simple Python program for uploading to UPDI-equipped microcontrollers using a serial adapter modified by the addition of a single resistor. But pyupdi was not readily usable from the Arduino IDE, and so this was not an option. As of 2.2.0, megaTinyCore brings in a portable Python implementation, which opens a great many doors; Originally we were planning to adapt pyupdi, but at the urging of its author and several Microchip employees, we have instead based this functionality on [pymcuprog](https://pypi.org/project/pymcuprog/), a "more robust" tool developed and "maintained by Microchip" which includes the same serial-port upload feature, only without the performance optimizations. **If installing manually** you must [add the Python package](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/tools/ManualPython.md) appropriate to your operating system in order to use this upload method (a system Python installation is not sufficient, nor is one necessary).

Read the [**SerialUPDI documentation**](https://github.com/SpenceKonde/AVR-Guidance/blob/master/UPDI/jtag2updi.md) for information on the wiring.

As of 2.3.2, with the dramatic improvements in performance, and the proven reliability of the wiring scheme using a diode instead of a resistor, and in light of the flakiness of the jtag2updi firmware, this is now the recommended programming method. As of this version, programming speed has been increased by as much as a factor of 20, and now far exceeds what was possible with jtag2updi (programming via jtag2updi is roughly comparable in speed to programming via SerialUPDI on the "SLOW" speed option, 57600 baud; the normal 230400 baud version programs about three times faster than the SLOW version or jtag2updi, while the "TURBO" option (runs at 460800 baud and increases upload speed by approximately 50% over the normal one. The TURBO speed version should only be used with devices running at 4.5v or more, as we have to run the UPDI clock faster to keep up (it is also not expected to be compatible with all serial adapters - this is an intentional trade-off for improved performance), but it allows for upload and verification of a 32kB sketch in 4 seconds.

At this point in time, while the compilation issues are believed to all be fixed, we cannot upload to EA without Microchip programmers, still. jtag2updi doesn't work either. Woe is us.

### What about Debugging?
See [my AVR research page for the state of my knowledge of the matter](https://github.com/SpenceKonde/AVR_Research/blob/main/Debugging_via_UPDI.md). We know the key, we just need someone to snoop on the UPDI line and correlate commands given to their official tooling with data picked up by their serial-spy.


### Coming sometime - something standalone
A direct-or-standalone programmer, for UPDI - and likely also classic AVRs.
In direct mode, a new upload tool will be used. Because the chip on the device will implement the UPDI protocol, USB latency will be drastically reduced as data can be sent in huge chunks at high baud rates and buffered in ram (we'll be using AVR Dx-series parts, and we should be able to feed data to the chip while receiving it from the computer, making this the fastest way to program a modern AVR. Because ISP is a somewhat more involved protocol, for those parts, HyperUPDI will still buffer large chunks of data, but will then write them before asking the computer for the next chunk of data (if there is one - for most tinyAVR parts, the ram is sufficient to buffer the entire flash contents). HyperUPDI will also come equipped with an 8 MB flash chip to hold flash (and optionally EEPROM and USERROW) images, and a 64k EEPROM to hold the table of contents (since it will be written and erased much more often). In full standalone mode, it will have a screen and basic UI to dump target flash to it's own flash.

No plans to use an SD card. Why? I have found them to be of miserably poor reliability (I've had one permanently fail after a reset during write, several others fail to work when I attempted to reuse them when they had previously worked, and one of them, in my phone, died suddenly, abbruptly, and completely while I was listening to music from it, and thereafter it was never recognized when plugging it into a reader), and they are particularly resource intensive because of the whole filesystem thing, yet the concept of a filesystem isn't really a perfect. SPI flash in contrast is quite reliable, and organized into 512b pages and 4k blocks. My plan is to store the index on an EEPROM (which can be rewritten more times) - like an AT24-series, with the Index entries containing an identifying string. the processor ID, and options like "lock the chip" "Set the fuses to these values" - later firmwares could even have an incrementing serial number writeen to flash, USERROW or EEPROM feature added fairly easily.

### Coming Much Sooner
Three new serial adapter products, two highly relevant to UPDI.

The first is a deluxe serial adapter, meant for someone who might want to monitor GPIOs with the modem control inputs, use the RTS modem control output for something special, or who might have either a 6-pin FTDI adapter connected, or want the adapter in UPDI programming mode, with a 3-pin header for that. It has an optional mezzanine board, (some TH assembly required) that adds status leds for modem control line, and a 6-pin JST-XH (for "FTDI adapter" like mode, the usual pin order. I started with pin 1 = Gnd), a 3-pin JST-XH connector for UPDI (pin 1 = Vdd, 2 = Gnd, 3 = UPDI), and a 6p molex picoblade connector (commonly sold as microJST 1.25, cause it looks like something JST would make on casual inspection - though anyone who is experienced with JST's design patterns could immediately point out at least three ways they know it's not a copy of anything made by JST) - I've found that an inexpensive and conveneint connector to use when I needed a smaller 6-pin serial connector, since both the connectors and pre-assembled 6p cables are really cheap on aliexpress. XH and Picoblade are both far more reliable than "dupont" connectors (though not real DuPont connectors, but they are prohibitively expensive. I use cheap knockoff terminals with gold flash (this is mostly to screen out the almost identical looking bad knockoffs - they aren't made with gold flash - the premium of gold vs non-gold terminals of the best knockoff design (the only one commonly seen in gold flash) is small, and since the cheap dupont terminals are copies of a flawed and ineffective knockoff they'll always suck. when I have to adapt it to dupont, I use pre-wired, housingless JST-XH terminal line (chosen with lengths a bit on the long side) and put them in the housing (so they're color coded), and then crimp on cheap chinese dupont terminals, with the expectation of having to periodically re-terminal the dupont end of the connector periodically; same with the UPDI programming connector, though as always, the lifespan of a dupont connector has a positive relationship to the number of pins, so UPDI connectors wear out faster). XH and picoblade do not have the same problem that dupont does because the retention force is supplied in large part by the housing, while on dupont it falls entirely to the pin contact. The original DuPont connectors dealt with this using a leaf spring which could reversibly deform and provided the mating force. Harwin, and then the chinese clones, instead made the terminal from a single piece of stamped metal, folded up into just the right shape. The stamped metal is generally brass, and brass doesn't make a terribly good spring - it bends easily and is by no means up to this task on a frequently used connector). But I digress. The deluxe adapter will have a switch to select between serial adapter and UPDI programmer, and the addon board gives you the other connection options and modem control line status lights.

Second is a dual serial adapter (this is the one unrelated to UPDI, beyond that one or both could be made into UPDI programming ports), all modem control lines broken out.

Finally, the same dual serial chip - only with one port permanently wired for UPDI programming, modem control lines for the serial port (but not the dedicated FTDI port - routing constraints and difficult design decisions. )

All three of them have a three-position voltage switch - VUSB (nom, +5V), 3.3v via an onboard regulator, or disconnect both of those from VIO, and expect it to be supplied by the target, and run at those logic levels - these adapters all have the separate VIO pin that works from 1.8-5.5v (chip held in hardware reset if VIO not supplied or too low. These are switched via fets, not directly by that tiny SMD switch so it can be used to supply up to 500mA without worrying about the voltage select switch being damaged.

### HV debrick project delayed
Based on information about the reset input cell on DD and later devices.

### What's With All The Different SerialUPDI Options?
Depending on adapter model, and operating system, it has been found that different timing settings are required; however, settings needed to keep even 230400 baud from failing on Linux/Mac with most adapters impose a much larger time penalty on Windows, where the OS's serial handling is slow enough that nothing needs that delay...

The "write delay" mentioned here is to allow for the page erase-write command to finish executing; this takes a non-zero time. Depending on the adapter, USB latency and the implicit 2 or 3 byte buffer (it's like a USART, and probably implemented as one internally. The third byte that arrives has nowhere to go, because the hardware buffer is only 2 bytes deep) may be enough to allow it to work without an explicit delay. Or, it may fail partway through and report an "Error with st". The faster the adapter's latency timeout, and the faster the OS's serial handling is, the greater the chance of this being a problem. This is controlled by the `-wd` command line parameter if executing prog.py manually. As of 2.5.6 this write delay is closer to the actual time requested (in ms), previously it had a granularity of several ms, when 1 is all you needed, and as a result, the penalty it imposed was *brutal*, particularly on Windows.

Selection guide:
* 460800+ baud requires the target to be running at 4.5V+ to remain in spec (in practice, it probably doesn't need to be quite that high - but it must be a voltage high enough to be stable at 16 MHz. We set the interface clock to the maximum for all speeds above 230400 baud - while a few adapters sometimes work at 460800 without this step (which in and of itself is strange - 460800 baud is 460800 baud right?), most do not and SerialUPDI doesn't have a way of determining what the adapter is.
* CH340-based adapters have high-enough latency on most platforms, and almost always work at any speed without resorting to write delay. All options work without using the write delay.
* Almost all adapters work on Windows at 230.4k without using the write delay. A rare few do not, including some native USB microcontrollers programmed to act as serial adapters (ex: SAMD11C).
* Almost nothing except the CH340-based adapters will work at 460.8k or more without the write delay, regardless of platform.
* On Windows, many adapters (even ones that really should support it) will be unsuccessful switching to 921600 baud. I do not know why. The symptom is a pause at the start of a few seconds as it tries, followed by uploading at 115200 baud. The only one I have had success with so far is the CH340, oddly enough.
* 460800 baud on Windows with the write delay is often slower than 230400 baud without it. The same is not true on Linux/Mac, and the smaller the page size, the larger the performance hit from write delay.
* 57600 baud should be used if other options are not working, or when programming at Vcc = < 2.7V.
* 460800 baud works without the write delay on some adapters with a 10k resistor placed across the Schottky diode between TX and RX, when it doesn't work without that unless the write delay is enabled. No, I do not understand how this could be either!
* As you can see from the above, this information is largely empirical; it is not yet known how to predict the behavior
* We are currently working on EA-series support.

### Why is My FTDI Adapter Insanely Slow?
FTDI adapters (FT232, FT2232, and FT4232 etc), including the fake ones that are available on eBay/AliExpress for around $2, on Windows default to an excruciatingly long latency period of 16ms. On many protocols this latency goes unnoticed, but when the majority of the communication is composed of messages transmit in less time than the latency timer takes to expire, and then wait on receiving a (similarly short) response suffer mightily from the USB latency. Even with the lengths we go to in order to limit the number of latency delay periods we must wait through, this will prolong a 2.2 second upload to over 15 second. You must change this in order to get tolerable upload speeds:
1. Open control panel, device manager.
2. Expand Ports (COM and LPT)
3. Right click the port and choose properties
4. Click the Port Settings tab
5. Click "Advanced..." to open the advanced settings window.
6. Under the "BM Options" section, find the "Latency Timer" menu, which will likely be set to 16. Change this to 1.
7. Click OK to exit the advanced options window, and again to exit properties. You will see device manager refresh the list of hardware.
8. Your adapter should be 6-10 times faster now (even so, at the maximum speeds, your programming speed still becomes latency-dominated )

### With a Classic Arduino (jtag2updi)
One can be made from a classic AVR Uno/Nano/Pro Mini; inexpensive Nano clones are the usual choice, being cheap enough that one can be wired up and then left like that. We no longer provide detailed documentation for this processes; jtag2updi is deprecated. If you are still using it, you should select jtag2updi from the tools->programmer menu. This was previously our recommended option. Due to persistent jtag2updi bugs, and its reliance on the largely unmaintained 'avrdude' tool (which among other things inserts a spurious error message into all UPDI uploads made with it), this is no longer recommended. Note that this will not support the EA-series.

## Compatibility Note for 32-bit Linux
Apparently Arduino isn't packaging 32-bit versions of the latest avrdude.
I defined a new tool definition which is a copy of arduino18 (the latest) except that it pulls in version 17 instead on 32-bit Linux, since that's the best that's available for that platform. The arduino17 version does not correctly support uploading with some of the Microchip programming tools.

## Ways to refer to pins
This core uses a simple scheme for assigning the Arduino pin numbers, the same one that [MegaCoreX](https://github.com/MCUDude/MegaCoreX) uses for the pin-compatible megaAVR 0-series parts - pins are numbered starting from PA0, proceeding counterclockwise, which seems to be how the Microchip designers imagined it too.

### PIN_Pxn Port Pin Numbers (recommended)
**This is the recommended way to refer to pins** Defines are provided of form PIN_Pxn, where x is the letter of the port (A through G), and n is a number 0 ~ 7 - (Not to be confused with the PIN_An defines described below) For example, `PIN_PC2`. These just resolve to the digital pin number of the pin in question - they don't go through a different code path. However, they have particular utility in writing code that works across the product line with peripherals that are linked to certain pins (by port), making it much easier to port code between devices with the modern peripherals. Several pieces of demo code in the documentation take advantage of this.

Direct port manipulation is possible on the parts (and is easier to write with if you use PIN_Pxn notation!) - in fact, in some ways direct port manipulation is more powerful than it was in the past. several powerful additional options are available for it - see [**direct port manipulation**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DirectPortManipulation.md).

### Arduino Pin Numbers (if you must)
When a single number is used to refer to a pin - in the documentation, or in your code - it is always the "Arduino pin number". The PIN_Pxn constants All of the other ways of referring to pins are #defined to the corresponding Arduino pin number. Note that Arduino pin numbers, while they will always increase in the counterclockwise direction, may have gaps - particularly on low-pin-count devices (If I could go back and redo it, I'd have put a gap in PORTF where the middle pins would be on the 28-pin parts.

### An and PIN_An constants (for compatibility)
The core also provides An and PIN_An constants (where n is a number from 0 to the number of analog inputs). These refer to the ADC0 *channel* numbers. This naming system is similar to what was used on many classic AVR cores - on some of those, it is used to simplify the code behind `analogRead()` - but here, they are just #defined as the corresponding Arduino pin number. The An names are intentionally not shown on the pinout charts, as this is a deprecated way of referring to pins. However, these channels are shown on the pinout charts as the ADCn markings, and full details are available in the datasheet under the I/O Multiplexing Considerations chapter. There are additionally PIN_An defines for compatibility with the official cores - these likewise point to the digital pin number associated with the analog channel.

### Missing pins
Sometimes for one reason or another pins get "skipped". For example. the 32 and 28 pin AVR DD-series don't have a PD0, while the smaller ones don't have a PC0 (in both cases, that is the pin that was taken over for VDDIO2). There are times when it makes things easier to skip the numbers those pins would have too. We often do this - PD0, even though it doesn't exist, still gets a number given to it (having the way to find the number of Px0 is useful (remember, the pins the pins are always numbered in order, except for discontinuities for missing ports). My rule for putting in "phantom" pins and "ghost" numbers is - with the exception of PORTF, (which appears on all parts, but never produces situations where this would be useful):
* If the port is entirely absent, it's numbers are not skipped.
* If only the first half of a port is present, the numbers for the second half are not skipped.
* Whenever one or more pins in a port is present, the pins before it, and after it to the midpoint or end of the port, have their numbers skipped.
* PORTA always gets their numbers skipped when not present, which is only the case on DD14.
* Px0 of any port will always be a phantom pin. At this point, only PC0 and PD0 are sometimes phantom pins.
* Other pins are never phantom pins.
* A phantom pin is defined, digitalPinToPort gives it's port, but all other "pin info" calls on it give NOT_A_PIN.
* The structure of the pin numbering is often taken advantage of to expedite calculations. It is not recommended to change the pin mappings.
For PORTF:
* IF only PF6 and PF7 present, do not skip the earlier numbers in PORTF. PF6 is numbered right after PD7 on some parts in PORTF
* Otherwise, either all of PORTF is present, or PF0, PF1, PF6 and PF7 are and we skip 4 numbers between them, so on 28+ pin parts there's a 4 arduino pin number gap

**Seriously, use the PIN_Pxn defines - it makes like much easier**

#### Examples of potentially surprising gaps in pin
* If you are using Arduino pin numbers on a DD14, you will notice that, yes, PA0 and PA1 are pins 0 and 1, then PC1, the next pin is pin 9, then 10, and 11. Then 12 through 15 don't exist and the numbering picks back up with PD4-7 at 16-19, with Reset and UPDI finishing it off with 20 and 21.
* There is no PC0 on MVIO parts with 14 or 20 pins.
* There is no PD0 on MVIO parts with 28 or 32 pins.
  * This means no analog channel 0.
* There is no PC0, PC1, or PC2 on DU-series.
* There is no PD0-PD3 on 14 or 20 pin parts.
  * This means no analog channels 0-3.


#### Reasoning behind the numbers
14-pin parts have digital pin numbers 0, 1, 9, 10, 11, 16, 17, 18, 19, 20, 21
But if you look them in PIN_Pxn notation, the reasoning is clearer: PA0-1, skip over the rest of portA, There's no PC0, then there is PC1-3, and PD4-7, plus PF6 and PF7, which are less than useful, being reset and input only, and UPDI, respectively.
20-pin parts have PA0-7, PC1-3, PD4-7, PF6-7 17 pins plus 3 power pins on DD, meaning pin numbers 0-7, 9, 10, 11, 16, 17, 18, 19, 20, 21 - All those six pins got added to PORTA, filling in that big hole

Because the EB will not have MVIO, it will not need VDDIO2, hence it will have PC0 even on 14-pin parts, while the DU-series is expected to not have PC0-PC2, those pins having been fed to the USB peripheral, along with the PLL/TCD...

### Link-time Optimization (LTO) support
This core *always* uses Link Time Optimization to reduce flash usage - all versions of the compiler which support the modern tinyAVR or Dx-series parts also support LTO, so there is no need to make it optional, as was done with ATTinyCore. This was a HUGE improvement in code size when introduced, typically on the order of 5-20%!

### So we can do HV UPDI on the DD's?
Well, in theory yes. A sharp eyed user pointed out that the datasheet specifies a lower absolute maximum on the Reset pin (where the HV pulse is to be directed) than the HV pulse was supposed to require (12V). I figured that was probably just a documentation error, but to humor him I asked my guy at Microchip. He indicated that actually it's a new I/O cell, voltage doesn't matter, energy does (and how the hell am I supposed to measure that and what is the target?), and left me unsure how one would go about programming one with HV UPDI. I don't recommend it until we can beat some more information out of Microchip.

## Exposed Hardware Features
To the greatest extent possible, all hardware features of these devices are exposed

### MVIO (DB, DD only)
There isn't really anything to do differently in the core to support MVIO - though the [DxCore library](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/DxCore/README.md) provides a slightly easier interface for checking the MVIO state, and measuring the voltage on VDDIO2. In short, what MVIO does is act as a level shifter built into the chip for PORTC:
* PORTC is powered by VDDIO2. The input voltage levels on PORTC will be based on VDDIO2, not VDD, and the output HIGH voltage will be VDDIO2 not VDD.
  * This applies to **all** functionality of the port.
* VDDIO2 is the power pin in the middle of that port that was a normal Vdd pin on the DA for 64/48-pin parts. On smaller parts it takes the place of PD0. It is always the physical pin immediately following PC3.  VDDIO2 can rise and fall independently of VDD. If only VDDIO2 is powered, the chip is not running, but is not harmed. If VDDIO2 is not powered but VDD is, the chip will run, but PORTC pins will all be tristated. In order for the MVIO pins to work, VDDIO2 must be between 1.65 and 5.5V (I don't know if there's a test on the high end of that, I suspect there is not).
* The upper protection diodes on PORTC go to VDDIO2, not VDD, clamping PORTC to not exceed VDDIO2 by more than around 0.3-0.5V (provided that whatever is powering it is able to sink the injected current. If you were, say, using a 3.3v linear regulator, most of them can only source current, and you would instead end up pulling the voltage on VDDIO2 upwards). This is not a good thing to do. See note at end.
* MVIO can be disabled from the fuses. This is controlled by the MVIO tools submenu, and set on all UPDI uploads, but with optiboot configurations, you must 'burn bootloader' to apply it as the bootloader cannot write it's own fuses. The VDDIO2 pin must be connected to VDD if this is done.
  * There is no internal connection between VDD and VDDIO2 even when MVIO is disabled.
  * Hence PORTC still runs at the voltage on the VDDIO2 pin (which should be the same as VDD unless wired incorrectly) if MVIO is disabled. What is disabled is the internal monitoring of the state of VDDIO2. The status bit always reads 1 (MVIO OK). If VDDIO2 is not powered, the pins are not tristated, nor are inputs set to 0 - reading the pins returns random values.
    * It may be possible to damage the part in this improper operating regime.
* It appears that the MVIO functionality was repurposed on the DU to get the 3.3v USB signal levels regardless of Vdd.

**Note regarding the internal clamp diodes** You generally want to avoid current flowing through the clamp diodes. There is no reason that it's any more or less bad on the MVIO pins - that similarly pulls Vdd upwards. Both of these are "survivable" as long as the maximum "clamp current" (some sources call it "current injection") limit from the datasheet (20mA absolute maximum) is not exceeded. This is 20mA on these parts, so they are much more forgiving than classic AVRs where it was.... 1mA, or even modern tinyAVRs (15mA, as long as Vdd is less than 4.9V, but 1 mA if its 4.9V+). However, it is not something that should be done intentionally unless the current is limited to a substantially lower value (a few mA or less). It's fairly common practice to put a sufficiently high value resistor between an I/O pin, and something that could go outside of the power rails to allow you to measure if the pin is powered or not or 0V (For example, to see if the external supply is connected - or if we're running on the batteries, and adjust our power usage behavior accordingly). This functions like a resistor divider, except that instead of a resistor, the bottom leg is the internal clamp diode. Even on the classic AVRs, Atmel provided an app note describing making a zero crossing detector for mains voltage with just a resistor in the mega-ohm range - so it's not something that you need avoid like the plague - but you should do it only with awareness that you are doing it and measures in place to limit the current.

### ADC Support
These parts all have a large number of analog inputs - DA and DB-series have up to 22 analog inputs, while the DD-series has analog input on every pin that is not used to drive the HF crystal (though the pins on PORTC are only supported when MVIO is turned off). They can be read with `analogRead()` like on a normal AVR, and we default to 10-bit resolution; you can change to the full 12-bit with `analogReadResolution()`, and use the enhanced analogRead functions to take automatically oversampled, decimated readings for higher resolution and to take differential measurements. There are 4 internal voltage references in 1.024, 2.048, 4.096 and 2.5V, plus support for external reference voltage (and Vdd of course). ADC readings are taken 3 times faster than an classic AVR, and that speed can be doubled again if what you are measuring is low impedance, or extend the sampling time by a factor greatly for reading very high impedance sources. This is detailed in the analog reference.

### DAC Support
The Dx-series parts have a 10-bit DAC which can generate a real analog voltage (note that this provides low current and can only be used as a voltage reference or control voltage, it cannot be used to power other devices). This generates voltages between 0 and the selected `VREF` (unlike the tinyAVR 1-series, this can be Vcc!). Set the DAC reference voltage via the DAC `reference()` function - pass it any of the ADC reference options listed under the ADC section above (including VDD!). Call `analogWrite()` on the DAC pin (PD6) to set the voltage to be output by the DAC (this uses it in 8-bit mode). To turn off the DAC output, call `digitalWrite()` or `turnOffPWM()` on that pin.

There may be additional options to configure the DAC on the EA-series.

See the [**ADC and DAC Reference**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Analog.md) for the full details.

Using the `An` constants for analog pins is deprecated - the recommended practice is to just use the digital pin number, or better yet, use `PIN_Pxn` notation when calling `analogRead()`.

### Watchdog timer, software reset
There are more options than on classic AVR for resetting, including if the code gets hung up somehow. The watchdog timer can only reset (use the RTC and PIT for low power timed interrupts). There are wrapper functions around the software and WDT reset methods in DxCore.h

See the [**Reset and Watchdog (WDT) Reference**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Reset.md) and [The core-auxiliary library, DxCore](megaavr/libraries/DxCore/README.md)

### Improved Digital I/O
This core adds a number of new features include fast digital I/O (1-14 clocks depending on what's known at compile time, and 2-28 bytes of flash (pin number must be known at compile time for the `________Fast()` functions, and for configuring all per-pin settings the hardware has with `pinConfigure()`.

See the [**Improved Digital I/O Reference**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Digital.md).

### Serial (UART) Support
All of the 0/1-Series parts have a single hardware serial port (UART or USART); the 2-Series parts have two. It works exactly like the one on official Arduino boards except that there is no auto-reset, unless you've wired it up by fusing the UPDI pin as reset (requiring either HV-UPDI or the Optiboot bootloader to upload code), or set up an "ersatz reset pin" as described elsewhere in this document. See the pinout charts for the locations of the serial pins.

Prior to putting the part into a sleep mode, or otherwise disabling it's ability to transmit, be sure that it has finished sending the data in the buffer by calling `Serial.flush()`, otherwise the serial port will emit corrupted characters and/or fail to complete transmission of a message.

See the [**Serial Reference**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Serial.md) for a full list of options. As of 1.4.10, almost every type of functionality that the serial hardware can do is supported, including RS485 mode, half-duplex (via LBME and ODME), and even synchronous and Master SPI mode, and 1.5.0 adds autobaud, even though it's not very useful.

### SPI support
A compatible SPI.h library is included; it provides one SPI master interface. These parts have either 1 or 2 SPI interfaces, however the library we supply does not provide support for two simultaneous SPI ports operating. Although it can use either of the hardware SPI ports, it does not allow both of them to be used at once. This is a reasonable limitation because the main reason one might want that would be to operate as an SPI slave device, but there is no Arduino API for that. SPI slave has never been supported on Arduino's SPI library on any device. More importantly, the precedent that has been set for multiple-SPI parts is for the second port to be SPI1, and so on, and this has to be the name for an instance of SPIClass. Unfortunately, Microchip took that name first, and SPI1 is instead an instance of their data structure type, `SPI_t`. But because all the existing code that supports more than 1 SPI module is hardwired to use that name - and we're bound to the rules of the underlying SPI modules - the SPI interfaces are treated as if they are pin mapping options (only one interface is available at a time - the library code available in the wild has a name collision with the I/O headers if one wanted to support using both at once, and all the workarounds that I can think of involve the libraries being changed as well). That's fine though, as treating them as pin mappings gives you most of the benefit as master, and slave support is not and never has been a thing in Arduino (it's pretty easy to do manually, at least for simple stuff).

The desired port and pinset are selected with `SPI.swap()` (preferred) or `SPI.pins()`. Both of them achieve the same thing, but differ in how you specify the set of pins to use. Either must be called **before** calling `SPI.begin()`, and neither should be called when the SPI port is enabled (turn it off with `SPI.end()`, and back on with `SPI.begin` after changing the pin set if needed).

### I2C (TWI) support, including all alternate pins, master & slave from the same TWI
All DA and DB parts with >28 pins have 2 I2C peripherals peripherals. S The included copy of the Wire library works almost exactly like the one on official Arduino boards, except that it does not activate the internal pullups unless they are specifically requested as described in the documentation linked below. The TWI pins can be swapped to an alternate location; this is configured using the Wire.swap() or Wire.pins() methods. Both of them achieve the same thing, but differ in how you specify the set of pins to use. This should be called **before** Wire.begin(), as should any method that enable certain modes. All DD-series parts have only a single TWI, `TWI0`. See the Wire library documentation for all of the details on the enhanced Wire library.

See **[Wire.h documentation](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/Wire/README.md)** for full description and details. The hardware I2C is one of the more complicated peripherals

### Alternate pin mapping (PORTMUX) support
Like most recent parts, the Dx-series parts have multiple pin-mapping options for many of their peripherals. For the serial data interfaces, we provide the same `.swap()` and `.pins()` methods like megaTinyCore and MegaCoreX (which first introduced this feature) whereby each instance of a UART, SPI interface, or I2C interface can be moved appropriately, excepting SPI1 as noted above and described in detail in that library documentation.

Many libraries that are dedicated to specific peripherals also provide a method for choosing alternate pins. This is described in the library documentation.

For PWM, it is handled differently depending on the timer:
* TCA0/TCA1 - analogWrite() is PORTMUX aware. On a part that starts up with PORTD on TCA0 (so analogWrite(PIN_PD1,duty_cycle); will make PWM come out of PD1, if you set PORTMUX.TCAROUTEA to point to PORTC, analogWrite(PIN_PD1,duty_cycle) will no longer cause PWM to be output, while analogWrite(PIN_PC1,duty_cycle) will). Only 64-pin parts have an alternate mapping for TCA1 (the 3 output only mapping options would slow down analogWrite too much to support; this is only viable because of favorable numeric alignment).
* TCD - TCD has 4-pin pinsets. The timer can output 2 PWM signals at a time, and the core assigns them to alternating pins. You are not prevented from enabling PWM on the two pins that both output the same signal (you probably don't want to do that though). There is supposed to be support for moving around those 4 pins to a different port, but no DA or DB silicon is available not impacted by the TCD portmux errata). The DD-series is expected to be free from that issue, and so we have implemented this functionality as well. we will implement the same solution as above (code has been written, but the silicon bug impacts all available parts). See below for more information on PWM.
* TCB - TCBs are not for PWM (as in, they're lousy at it). There is no support for changing their mapping. It is recommended to stick to the proper PWM timers, and use these for everything else.


### PWM on LOTS of pins
The core provides hardware PWM (analogWrite) support. On all parts, 6 pins (by default, see part-specific doc pages for details) provide 8-bit PWM support from the Type A timer, `TCA0`. On 48-pin and 64-pin parts Dx-series parts, an additional 6 PWM pins are available on PB0-PB5 (by default) from `TCA1`. `TCA0` and `TCA1` can be remapped - TCA0 can output to any port, (on pins 0-5) and TCB can output on PORTB and PORTG (on 64-pin parts only; note that DA64's have an errata here, and only PORTB works) Analog read understands this, and it will check the `PORTMUX.TCAROUTEA` register. As long as that is set to an option that allows 6 outputs in split mode (the two weird options for TCA1 aren't supported),  `analogWrite()` will make the pin output PWM, and digitalWrite() will turn it off . Note that changing PORTMUX while outputting PWM will return the current PWM pins to the whatever the PORT register says (and they'll remain outputs). Any active channels will immediately begin outputting that PWM on the new pins as long as it is set as an output. That might be undesirable; See the helper functions in DxCore.h library for some examples of this. This is only used for parts where there are available pins to choose from (e.g. 48 pin parts have only a single 6-pin output option for TCA1, so this code path isn't used there).

Additionally, Type B timers not used for other purposes (TCB2 is used for millis unless another timer is selected, and other libraries may use a `TCB` as well) can each support 1 8-bit PWM pin. The pins available for this are shown on the pinout charts. There is no TCB PWM pin swap supported. It could be implemented, but as the timers make very poor PWM timers, and there are so many other sources of PWM, it doesn't seem worth the development time.

Some additional information about the output frequency and configuration of the TCAs is found in the [**Timers and PWM**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Timers.md) reference.

`TCD0` provides two PWM channels: On DA and DB parts currently available, a serious errata prevents none-default options from behaving in a useful way, thus limiting it to the default pinset, PA4-PA7. `WOA` can output on PA4 or PA6, `WOB` on PA5, PA7. Those channels can each drive either - or both - of their pins, but only at one duty cycle. Users may prefer to configure this manually - `TCD0` is capable of, among other things, generating much higher frequency PWM, as it can be clocked from the PLL at 48MHz (or more, if you don't mind exceeding the specified operating ratings - I've gotten it up to 128 MHz (it wouldn't do 160 though - the PWM was glitchy), allowing 8-bit PWM at 500 kHz, or a 64 MHz square wave). It is believed that this is corrected for the AVR-DD-series will fix this - in all cases, the core uses channel A for the 2 even numbered pins, and channel B for the two odd numbered pins. For the full details, see the [**TCD and DxCore reference**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_TCD.md). If you try to get PWM out of three pins, or out of both odd or even ones, they will all output PWM (assuming they are in the same PORTMUX option), but only at the most recent duty cycle for each channel - you could set WOA to 50% duty cycle and WOB to 75% duty cycle, if you then tried to output 25% on WOC, though it would output the 25% duty cycle, it would also set WOA to 25% duty cycle (digitalWrite the pin to turn this off).
```c++
analogWrite(PIN_PA4, 128); // 50% PA4. - like usual
analogWrite(PIN_PA7, 192); // 50% PA4, 75% PA7 - like usual
analogWrite(PIN_PA6, 64); // 25% PA5, 25% PA6, 75% PA7 - PA4 and PA6 are both channel A
//use digitalWrite or turnOffPwm to turn off the PWM on a TCD pin - because turning a channel on or off can cause a glitch on the other channel,
analogWrite(pin,0 or 255); // leaves it connected to the timer while outputting a duty cycle of 100% or 0%. thus:
analogWrite(PIN_PA4, 0);  // PA4 set low, but still on timer! This would also impact the `PA6` output we just set in addition to `PA4`.
analogWrite(PIN_PA6, 128); // PA4 still connected, so both pins will output 50% duty cycle - without the usual short glitch on the PWM you've been outputting since the second line of the example.

```

The issue with alternate mapping options should be fixed on the DD-series.

(Note that there is a complicated and very hacky way to squeeze a third channel out of using the delayed event, that's covered in Ref_TCD, linked above).

If you want to take full control of one of the three PWM timers (maybe you want single mode for 16-bit PWM), just call `takeOverTCA0();` For the TCA's, it will also force hard reset, so they are passed to you in pristine condition. After this, analogWrite, `digitalWrite()` and `turnOffPWM()` will pretend the timer you took over doesn't exist. If taking over `TCD0` - may the gods of silicon have mercy on you. It is one of the most fiendishly complicated contraptions on an AVR (not counting the XMega line, of course. On XMega, every peripheral is a byzantine nightmare like this). It's behavior is... often counterintuitive... but the feature set is incredible. I allowed into the core limited support for users making certain adjustments while still using `analogWrite()` because, well - because if I were to take my default position of "if you want to manually configure it, take it over"), *nobody would do it, and that seems like a waste* (it took days to get analogWrite working with it, and wasn't until a year later that I figured out why I hadn't been able to make single-ramp mode work (and latent bugs have been found as recently as 2021).  But the basics are thus:

**Note that TCA0, and TCA1 if present are configured by DxCore in Split Mode by default, which allows them to generate 8-bit PWM output on 6 pins each, instead of 16-bit PWM on three**; since the Arduino analogWrite() function we get takes values 0-255, this seems appropriate. See the [**Taking over TCA0**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/TakingOverTCA0.md) guide for more information on reconfiguring this if you need 16-bit PWM.
**For general information on the available timers and how they are used PWM and other functions, consult the guide:**
#### [**Timers and DxCore**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Timers.md)

### EEPROM - Yes
A compatible `EEPROM.h` library is included; this implementation is derived from and fully compatible with the standard `EEPROM.h` API, even though the implementation differs internally.

### USERROW - Yes
The "USERROW", more formally known as the User Signature Space, is a small section of EEPROM-like memory which always survives a chip erase cycle. It is only erased if the application does so, or when the chip undergoes a chip erase while locked (that way sensitive information could be stored there on a locked chip). It also has the unique option of being writable via UPDI *on a locked chip* (using a special procedure; I've never had any reason to do it). We present a very similar interface to the EEPROM library, however, erase is all or nothing, and in order to prevent unnecessary erase cycles,it's a bit more complicated, and an additional function call is sometimes needed to commit the changes to flash.

See the [USERSIG library documentation](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/Wire/README.md)

### Pin Interrupts
All pins can be used with `attachInterrupt()` and `detachInterrupt()`, on `RISING`, `FALLING`, `CHANGE`, or `LOW`. All pins can wake the chip from a sleep mode where the clock is turned off (power down and standby unless you have set something that needs it to run in standby) sleep on `CHANGE` or `LOW`. Pins marked as Async Interrupt pins on the pinout chart (this is marked by an arrow where they meet the chip on those charts - pins 2 and 6 on all ports have this feature) can be used to wake from sleep on `RISING` and `FALLING` edge as well. The async pins can also react to inputs shorter than one clock cycle (how *much* shorter was not specified) - this is both a blessing and a curse. If you need to detect super-short pulses, it's a blessing; but if you're not, those spikes are called "noise", and the async pins are more likely to be triggered by it.

#### `attachInterrupt()` rework
There are three options, controlled by the Tools -> attachInterrupt Mode submenu: the new, enabled on all pins always (like the old one), manual (ports must be enabled before attaching to them), and old version (if the new implementation turns out to break something). Manual mode is required for the main benefit. In manual mode, you must call `attachPortAEnable()` (replace A with the letter of the port) before attaching the interrupt. The main point of this is that (in addition to saving an amount of flash that doesn't much matter on the Dx-series) `attachInterrupt()` on one pin (called by a library, say) will not glom onto every single port's pin interrupt vectors so you can't manually define any. The interrupts are still just as slow (it's inherent to calling a function by pointer from an ISR - and low-numbered pins are faster to start executing than high numbered ones. The method to enable may change - I had hoped that I could detect which pins were used, but I couldn't get the function chose which ports to enable to not count as "referencing" those ports, and hence pull in the ISR. I am not happy with it, but "can't use any pin interrupts except through `attachInterrupt()` if using a library that uses `attachInterrupt()`" is significantly worse.

See the [**Interrupt reference**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Interrupts.md) for more information.

### Additional supported peripherals
These parts have a great many powerful peripherals far beyond what the classic AVRs did, and we provide a simple wrapper library around them when we think doing so is useful.

#### On-chip Op-amps
The DB-series parts have 2 (28 or 32 pin) or 3 (48/64 pin) on-chip op-amps, with programmable resistor ladder, configurable for a variety of applications. They can be used as a voltage follower (you can follow the DAC and then use the output to drive VDDIO2, though the current is still only tens of mA, that's often enough - driving heavy loads at the lower voltage is an unusual use case which requires a separate power supply; many sensors exist now with maximum voltage below 3.3V and draw very little current. This is a good use case for it.)

We provide a basic wrapper in the form of the [**Opamp Library**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/Opamp) by MCUDude.

#### Analog Comparators
The analog comparators are exposed through the [**Comparator library**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/Comparator) by MCUDude. Availability varies by pin count:
* 2 on 28 and 32 pin DA/DB
* 3 on 48/64 pin DA/DB
* 1 on all DD-series
* 2 on all EA-series

#### Configurable Custom Logic
The CCL is exposed through the [**Logic library**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/Logic) by MCUDude. Number of logic blocks depends on series and pin count:
* 6 on 48/64 pin DA/DB
* 4 on everything else

#### Event System
The event system is exposed through the [**Event library**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/Event) by MCUDude. Number of channels depends on series and pin count:
* 8 channels on 28/32-pin DA/DB-series
* 10 on larger DA/DB-series
* 6 on everything else

#### Zero-Crossing Detector
The ZCD(s) are exposed through the [**ZCD library**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/ZCD) by MCUDude. Availability depends on pin count:
* 2 on 28 and 32 pin DA/DB
* 3 on 48/64 pin DA/DB
* 1 on all DD
* The EA-series does not have a ZCD

#### Timers
These parts for the most part are swimming in timers - The exception being the 14 and 20 pin DD-series, which are sadly stuck with a tinyAVR-like number of timers to go with its tinyAVR-like pin count. More information can be found at [Timers and PWM](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Timers.md) and [TCD0 reference](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_TCD.md)
##### Timer types
* TCA - 16 bit timer, 3x16-bit or 6x8-bit PWM channels and lots of features.
  * 2 on 48/64 pin DA/DB and all EA-series; extra PORTMUX options of 64-pin only.
  * 1 elsewhere
  * Capabilities:
    * 6 PWM channels in split mode or 3 in non-split mode.
    * TCA0 on non-tiny PORTMUX option is simply the port number!
    * Prescale by 1, 2, 4, 8, 16, 64, 256 or 1024.
    * Can count events on all parts (events must last more than 2 system clocks to be seen, though).
* TCB - 16-bit utility timer for input capture or 8-bit PWM. No independent prescaler
  * 5 on 64-pin Dx-series parts
  * 4 on 48-pin Dx-series parts and all EA-series
  * 3 on all 28/32-pin Dx-series
  * 2 on smaller parts
  * Capabilities:
    * 1 PWM channel each. It is not recommended to use them for PWM.
    * Most have 2 pin options (counting the default pin), some don't have a remapping option and some don't have a pin at all (particularly on DD-series)
    * Prescaler can only be 1, 2 or a value used by a TCA.
    * These are lousy as PWM timers. They are excellent utility timers however.
    * 2 can be combined for 32-bit input capture with the CASCADE option. (Dx and later only)
    * Can be used to count on event inputs (not on pre-DA-series parts though)
* TCD - 12-bit asynchronous timer for high-speed and/or async PWM and internal PLL
  * 1 on all DA, DB, and DD parts
  * None on DU (presumably it's still on the die, but being used to generate that 48 MHz reference clock needed for standards compliant USB 2.0)
  * None on EA/EB-series. EB has 2 new kinds of timers about which very little is known.
  * Capabilities:
    * Can be used to generate 2 independent 12-bit PWM outputs on up to 2 pins each - usually pins 4-7 of the port selected with PORTMUX (we use the PA4-5 PD4-5 PORTMUX option on 14 and 20 pin DD-series during initialization, and do not change it from the default on parts with more, since it can be con
      * The PORTMUX is busted in DA/DB parts per errata.
    * See the Timer and TCD references for more information on how this timer is used for analogWrite().
    * Can react to events asynchronously (i.e., events shorter than 1 system clock cycle), but this only works correctly when the count prescaler is disabled due to errata on DA/DB).
    * Can use an external clock source, optionally multiplied by the PLL. Runs at speeds far higher than CPU core can (and there's an undocumented but functional 4x PLL multiplier).
    * Complex and fully automatic reactions to events to permit an "emergency stop" that would work without CPU intervention.
    * Challenging to configure, even to do simple stuff.


## Other major features

### Memory-mapped flash? It's complicated
Unlike the tinyAVR 0/1/2-series and megaAVR 0-series parts, which are able to map their entire flash to memory, most Dx-series parts have too much flash for a 16-bit address space. They can only map 32KB at a time. The FLMAP bits in NVMCTRL.CTRLB control this mapping. Unfortunately, because this can be changed at runtime, the linker can't automatically put constants into flash on 64k and 128k parts. However, on 32k parts, it can, and does. The latest ATpacks have released support for that, but it unclear how to make that usable by Arduino.  The `F()` macro works the same way as it does on normal boards as of 1.2.0, even on the 32k parts, where it is unnecessary to save RAM - this was done in order to maintain library compatibility; several very popular libraries rely on `F()` returning a `__FlashStringHelper *` and make use of `pgm_read_byte()` to read it.

See [**PROGMEM and mapped flash reference**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_PROGMEM.md) for information on how to store constant variables in the mapped sections of flash.

### Writing to Flash from App
It is possible to write to the flash from the application code using the included Flash.h library. See the documentation for more information. Note that the API is completely different in every way from the Flash.h used on MegaCoreX and megaTinyCore (which use the same flash library). They were developed independently and reflect both the differences between the two NVM controllers and the differing programming ideologies of the author of the libraries. I make no claim that mine is better, though I note that at the time, I didn't believe it would be possible to get that behavior. I was barely able to find the 4 bytes of flash in the bootloader section that this needs! Dx-series only, and does not require optiboot.
See the [**Flash Library Documentation**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/Flash/README.md)

### Servo Support
This core provides a version of the Servo library. This version of Servo always uses TCB0. If millis/micros is set to use TCB1 on those parts, servo will use TCB0 instead, making it incompatible with tone there as well). Servo output is better the higher the clock speed - when using servos, it is recommended to run at the highest frequency permitted by the operating voltage to minimize jitter.

**If you have also installed Servo** to your `<sketchbook>/libraries` folder (including via library manager), the IDE will use that version of the library (which is not compatible with these parts) instead of the one supplied with DxCore (which is). As a workaround, a duplicate of the Servo library is included with a different name - to use it, `#include <Servo_DxCore.h>` instead of `#include <Servo.h>` - all other code can remain unchanged.

### `printf()` support for "printable" class
Unlike the official board packages, but like many third party board packages, megaTinyCore includes the `printf()` method for the printable class (used for UART serial ports and most everything else with `print()` methods); this works like `sprintf()`, except that it outputs to the device in question; for example:
```cpp
Serial.printf("Milliseconds since start: %ld\n", millis());
```
Note that using this method will pull in just as much bloat as `sprintf()` and is subject to the same limitations as printf - by default, floating point values aren't printed. You can use this with all serial ports
You can choose to have a full `printf()` implementation from a Tools submenu if you want to print floating point numbers, at a cost of some additional flash.

#### **WARNING** `printf()` and variants thereof Have Many Pitfalls
There are a considerable number of ways to screw up with `printf()`. Some of the recent issues that have come up:
* Formatting specifiers have modifiers that they must be paired with depending on the datatype being printed, for all except one type. See the table of ones that I expect will work below (it was cribbed from cplusplus.com/reference/cstdio/printf/ which has since ceased to be a working link, and then I chopped off all the rows that aren't applicable, which is most of them). Apparently many people are not fully aware (or at all aware) of how important this is - even when they think they know how to use printf(), and may have done so on previously (on a desktop OS, with 32-bit ints and no reason to use smaller datatypes for simple stuff).
* There are (as of 1.4.0) warnings enabled for format specifiers that don't match the the arguments, but you should not rely on them. Double check what you pass to `printf()` - `printf()` bugs are a common cause of software bugs in the real world. Be aware that *while you can use F() on the format string, there are no warnings for invalid format strings in that case*; a conservative programmer would first make the app work without F() around the format string, and only switch to F() once the format string was known working.

From cplusplus.com:
>The length sub-specifier modifies the length of the data type. This is a chart showing the types used to interpret the corresponding arguments with and without length specifier ~(if a different type is used, the proper type promotion or conversion is performed, if allowed)~:
Strikethrough mine 'cause that don't work here (and it's not my fault nor under my control - it's supplied with avrlibc, and I suspect that it's because the overhead of implementing it on an 8-bit AVR is too large). When incorrect length specifiers are given (including none when one should be used) surprising things happen. It looks to me like all the arguments get smushed together into a group of bytes. Then it reads the format string, and when it gets to a format specifier for an N byte datatype, it grabs N bytes from the argument array, formats them and prints them to whatever you're printing to, proceeding until the end of the format string. Thus, failing to match the format specifiers' length modifiers with the arguments will result in printing wrong data, for that substitution **and all subsequent ones** in that call to `printf()`.

The table below comprises the relevant lines from that table - many standard types are not a thing in Arduino.

| length | d i | u o x X | f F e E g G a A |  c  |    s   |  p   |    n     |
|--------|-----|---------|-----------------|-----|--------|------|----------|
| (none) |int16|  uint16 | float           | int |  char* |void* | int*     |
| hh     |int8 |  uint8  |                 |     |        |      | char*    |
| l      |int32|  uint32 |                 |     |        |      | int32_t* |

Notice that there is no line for 64 bit types in the table above; these are not supported (support for 64-bit types is pretty spotty, which is not surprising. Variables of that size are hard to work with on an 8-bit microcontroller with just 32 working registers). This applies to all versions of `printf()` - the capability is not supplied by avr-libc.

There are reports of memory corruption with printf, I suspect it is misunderstanding of above that is actually at hand here.

#### Selectable `printf()` Implementation
A Tools submenu lets you choose from three levels of `printf()`: full `printf()` with all features, the default one that drops float support to save 1k of flash, and the minimal one drops almost everything and for another 450 bytes flash saving (will be a big deal on the 16k and 8k parts. Less so on 128k ones). Note that selecting any non-default option here *will cause it to be included in the binary even if it's never called* - and if it's never called, it normally wouldn't be included. So an empty sketch will take more space with minimal `printf()` selected than with the default, while a sketch that uses `printf()` will take less space with minimal `printf()` vs default.

### Interrupts From Pins and in General
All pins can be used with `attachInterrupt()` and `detachInterrupt()`, on `RISING`, `FALLING`, `CHANGE`, or `LOW`. All pins can wake the chip from sleep on `CHANGE` or `LOW`. Pins marked as Async Interrupt pins on the megaTinyCore pinout charts (pins 2 and 6 within each port) can be used to wake from sleep on `RISING` and `FALLING` edges as well. Those pins are termed "fully asynchronous pins" in the datasheet.

Advanced users can instead set up interrupts manually, ignoring `attachInterrupt()`, manipulating the relevant port registers appropriately and defining the ISR with the `ISR()` macro - this will produce smaller code (using less flash and RAM) and the ISRs will run faster as they don't have to check whether an interrupt is enabled for every pin on the port.

For full information and example, see [the Interrupt Reference](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Interrupts.md).

### Assembler Listing generation
Like my other cores, Sketch -> Export compiled binary will generate an assembly listing in the sketch folder. A memory map is also created. The formatting of the memory map leaves something to be desired, and I've written a crude script to try to improve it, see the Export reference for more information.
see [**Exported Files documentation**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Export.md)

### EESAVE configuration option
The EESAVE fuse can be controlled via the Tools -> Save EEPROM menu. If this is set to "EEPROM retained", when the board is erased during programming, the EEPROM will not be erased. If this is set to "EEPROM not retained", uploading a new sketch will clear out the EEPROM memory. Note that this only applies when programming via UPDI - programming through the bootloader never touches the EEPROM. Burning the bootloader is not required to apply this change on DA and DB parts, as that fuse is "safe". It IS required on DD-series parts, because its on the same fuse that controls whether the UPDI pins is acting as UPDI or I/O

See the [Export Reference](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Export.md).


### BOD configuration options
These parts support multiple BOD trigger levels, with Disabled, Active, and Sampled operation options for when the chip is in Active and Sleep modes - Disabled uses no power, Active uses the most, and Sampled is in the middle. See the datasheet for details on power consumption and the meaning of these options. You must do Burn Bootloader to apply this setting, as this is not a "safe" setting: If it is set to a voltage higher than the voltage the board is running at, the chip cannot be reprogrammed until you apply a high enough voltage to exceed the BOD threshold. The BOD thresholds are quite low on these devices,

### NeoPixels (aka WS2812/SK6812 and many, many others)
Many of the usual NeoPixel (WS2812) libraries have problems on these parts. This core includes two libraries for controlling WS2812/SK6812/etc LEDs, both of which are tightly based on the Adafruit_NeoPixel library. See the [**tinyNeoPixel documentation**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/tinyNeoPixel.md) and included examples for more information. Support is in the code for all clock speeds from 8 MHz up to 48 MHz. I suspect that it could be just barely made to work at 4 MHz by relaxing constraints - but I do not see much demand for such an undertaking. It just doesn't make sense to be driving WS2812's from low-clocked Dx-series. If you're driving a string of '2812s you're not worried about power consumption - a half dozen 2812's uses more power than the chip at full speed, and that's when the LEDs are off! the other reason to run at low frequencty is to operate at low voltage, but not only are the Dx parts rated for the full 24 MHz from 5.5V all the way down to 1.8V, at any voltage below 4V or so the blue LEDs don't work at full brightness anyway. So I decided that there was no reason to waste time porting the 'WS2812 driver to lower speeds.

### Tone Support
Support for `tone()` is provided on all parts using a type B timer. See the Timers and DxCore reference linked below for a few additional notes.

### millis/micros Timekeeping Options
DxCore provides the option to us any available timer on a part for the `millis()`/micros timekeeping, controlled by a Tools submenu - (except, currently, TCD0 - implementations are available, but there are more options available here than on the tinyAVR 1-series, making it's use more complicated) - or it can be disabled entirely to save flash, eliminate the presence of frequent millis interrupts, and allow full use of the timers. By default, TCB2 will be used, except for DD-series parts without the third timer. Those will default instead to TCB1. TCA0, TCA1 (if present) and any of the TCB's present on the part may be used. TCD support will be added after DD-series release, but it will never be the default. TCD0 is far more powerful here than on the tinyAVR 1-series since we get a PLL to drive it, and (pending fixes) flexible pin mapping options.

For more information, on the hardware timers of the supported parts, and how they are used by DxCore's built-in functionality, see the [**Timers and DxCore reference**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Timers.md)

### Improved digital I/O
This core adds a number of new features include fast digital I/O (1-14 clocks depending on what's known at compile time, and 2-28 bytes of flash, and for configuring all per-pin settings the hardware has with `pinConfigure()`

See the [**Improved Digital I/O Reference**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Digital.md)

## Additional DxCore documentation

### Official [application notes from Microchip](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Microchip.md)
A partial listing of applicable app notes. The ones that looked most useful or interesting.

#### [IO-headers](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/ioheaders/README.md)
These are a copy of the latest i/o headers (not necessarily the ones we use!), for user convenience; they are meant for online viewing or manual installations, since a board manager installation will bury them just as deeply as the copies the toolchain uses.

### Library documentation
See the [library index](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Libraries.md) or readme files for each library (the former is mostly composed of links to the latter)

## Bootloader (Optiboot) Support
A new version of Optiboot (Optiboot_dx) now runs on the tinyAVR DA and DB-series and DD is expected shortly It's under 512 bytes, and (will) on all parts supported by this core, allowing for a convenient workflow with the same serial connections used for both uploading code and debugging (like a normal Arduino Pro Mini). Note the exception about not having autoreset unless you disable UPDI (except for the 20 and 24-pin 2-Series parts which can put reset on PB4 instead), which is a bit of a bummer.

To use the serial bootloader, select a board definition with (optiboot) after it. Note - the optiboot suffix might be visually cut off due to the width of the menu; the second / lower set of board definitions in the board menu are the optiboot ones). The 2-Series Optiboot definitions and the 0/1-Series Optiboot definitions are separate entries in the board menu.

See the [Optiboot reference](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Optiboot.md)for more information.

## Guides
These guides cover subsystems of the core in much greater detail (some of it extraneous or excessive).
### Reference Material
#### [Function Reference](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Functions.md)
Covering top-level functions and macros that are non-standard, or are standard but poorly documented, and which aren't covered anywhere else.
#### [Analog Input (ADC) and Output (DAC)](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Analog.md)
The API reference for the analog-related functionality that is included in this core beyond the standard Arduino API.
#### [Digital I/O and enhanced options](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Digital.md)
The API reference for the digital I/O-related functionality that is included in this core beyond the standard Arduino API, as well as a few digital I/O related features that exist in the hardware which we provide no wrapper around.
#### [Interrupts](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Interrupts.md)
Includes a list of all interrupt vectors that can be used, how the flags are cleared (not a substitute for the datasheet - just a very quick reminder), which parts each vector exists on, and and what parts of the core, if any, make use of a vector. It also has general guidance and warnings relating to interrupts their handling, including estimates of real-world interrupt response times.
#### [Timers and PWM](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Timers.md)
We configure the timers in specific ways upon startup, which determines the frequency of PWM output, and some parameters of `millis()` timekeeping.
#### [TCD0](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_TCD.md)
The type D timer is a powerful timer, but has quirks which one must be aware of if using it. This describes what you can do without having to take full control of the timer.
#### [Serial](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Serial.md)
The USARTs (Serial) have some additional features not seen on the official cores.
#### [Mapped flash and PROGMEM in DxCore](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_PROGMEM.md)
There are two ways to access constants stored in flash on DxCore. Which ones can read data stored where can be confusing; this document should make this clear.
#### [Optiboot Bootloader](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Optiboot.md)
An Optiboot-derived bootloader is provided and may be optionally used. How that impacts operations is described here. This covers relevant considerations for deciding whether to use it as well.
#### [SerialUPDI](https://github.com/SpenceKonde/AVR-Guidance/blob/master/UPDI/jtag2updi.md)
Serial UPDI is our recommended tool for UPDI programming.
#### [Clock Information](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Clocks.md)
Supported clock sources and considerations for the use thereof.
#### [Callbacks/weakly defined functions](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Callbacks.md)
These are provided by the core and can be overridden with code to run in the event of certain conditions, or at certain times in the startup process.
#### [Constants for part/feature/pin/version identification](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Defines.md)
The core feature defines are used by megaTinyCore and other cores I maintain as well. This also documents what constant values are defined by the core for version identification, testing for features, and dealing with compatibility problems.
#### [Assembly Listing and Memory Maps](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Export.md)
Export compiled binary generates both assembly listings and memory maps, in addition to the hex file. The options selected are encoded in the name of the file to help prevent confusion, and make it easy to compare two configurations when you are surprised by the differences between them.
#### [Reset control and the WDT](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Reset.md)
The sources of reset, and how to handle reset cause flags to ensure clean resets and proper functioning in adcverse events. **Must read for production systems**
#### [Considerations for robust applications](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Robust.md)
Covers a variety of design considerations for making something that will operate reliably in the field, some specific to DxCore, others general. Lately I have been seeing a lot of projects get too far along without considering these. **Must read for production systems**
#### [Power Saving techniques and Sleep - inherited from megaTinyCore](megaavr/extras/PowerSave.md)
There are plans for a better wrapper around this sort of functionality, which keep getting deferred as more pressing issues come up. This was inherited from megaTinyCore and is essentially unmodified and may not reflect all the features of the Dx-series.
#### [Link time Optimization](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_LTO.md)
LTO is a miraculous (although obvious in retrospect) method of optimizing during the final step of compilation, and is used to produce smaller faster binaries. This document describes how, and also how it can be turned off when you are getting an inscrutable error relating to the definition of a function which is claimed to be in a file that doesn't define that function, at a location that isn't a line number (`.text+0`), and you just can't figure out where the hell it's coming from. Note that turning off LDO breaks most core functionality so once things compile, you need to turn it back on.

## List of Tools sub-menus
* Tools -> Chip - sets the specific part within a selected family to compile for and upload to.
* Tools -> Clock Speed - sets the clock speed. You do not need to burn bootloader after changing this setting!
* Tools -> Retain EEPROM - determines whether to save EEPROM when uploading a new sketch. This option is not available on Optiboot board definitions - programming through the bootloader does not execute a chip erase function and never erases the bootloader.  ~You must burn bootloader after changing this to apply the changes~ As of 1.3.0, this setting is applied on all UPDI uploads without a "burn bootloader" cycle to AVR DA and AVR DB-series devices.
* Tools -> B.O.D. Voltage - If Brown Out Detection is enabled, when Vcc falls below this voltage, the chip will be held in reset. You must burn bootloader after changing this to apply the changes. Take care that these threshold voltages are not exact - they may vary by as much as +/- 0.3v! (depending on threshold level - see electrical characteristics section of datasheet). Be sure that you do not select a BOD threshold voltage that could be triggered during programming, as this can prevent successful programming via UPDI (reported in #86).
* Tools -> Reset/UPDI - This menu option can be set to Reset (default) or Input; the latter allows this pin to be used as a normal input. DD-series have extra options to configure the UPDI pin as well, and on these parts, This setting is applied to DA and DB series on all UPDI uploads without a "burn bootloader" cycle. It is not set on DD-series parts - the UPDI disable option makes this fuse "unsafe" to reconfigure.
* Tools -> B.O.D. Mode (active/sleeping) - Determines whether to enable Brown Out Detection when the chip is not sleeping, and while it is. Only combinations where active is at least as aggressive as sleep mode are shown, as those are the only sensible operating modees.=. You must burn bootloader after changing this to apply the changes.
* Tools -> `millis()`/`micros()` - If set to enable (default), `millis()`, `micros()` and `pulseInLong()` will be available. If set to disable, these will not be available, Serial methods which take a timeout as an argument will not have an accurate timeout (though the actual time will be proportional to the timeout supplied); delay will still work, though it's done using `delayMicroseconds()`, so interrupts are disabled for 1ms at a time during the delay, and any interrupts that happen during the delay will add to the length of the delay. Depending on the part, options to force millis/micros onto any type A or B timer on the chip are also available from this menu.
* Tools -> MVIO - MVIO option is back in 1.3.7. It is not a risk of hardware damage if it is turned off inappropriately, though the pins may not behave correctly. It saves 0.5 uA power consumption to disable it. Disabling it when you shouldn't doesn't keep the pins from being readable and writable, nor does it short the VDDIO pin to VDD.... As far as I could tell, it just no longer watches the voltage to ensure sane behavior if insufficient voltage is applied on VDDIO2. This is in effect an extra layer of monitoring like the BOD is, so the added current should not come as a surprise.
* Tools -> `printf()` implementation - The default option can be swapped for a lighter weight version that omits most functionality to save a tiny amount of flash, or for a full implementation (which allows printing floats with it) at the cost of about 1k extra. Note that if non-default options are selected, the implementation is always linked in, and will take space even if not called. Normal Arduino boards are set to default. They also don't have `Serial.printf()`
* Tools -> WDT Timeout: The WDT can be forcibly enabled by fuses. This menu now lets you do that. You should absolutely not rely on this for making your code work (if your code needs WDT resets, understand why it does first!)
* Tools -> WDT Window: The WDT can also have a separate delay before the "window" open. If you try to issue a WDR, but the window is closed (if you "hit the window, like a bird") the watchdog timer will also
* Tools -> attachInterrupt Mode - Choose from 3 options - the new, enabled on all pins always (like the old one), Manual, or the old implementation in case of regressions in the new implementation. When in Manual mode, You must call `attachPortAEnable()` (replace A with the letter of the port) before attaching the interrupt. This allows attachInterrupt to be used without precluding any use of a manually defined interrupt (which is always much faster to respond. Basically any time you "attach" an interrupt, the performance is much worse. )
* Tools -> Wire Mode  - In the past, you have only had the option of using Wire as a master, or a slave. Now the same interface can be used for both at the same time, either on the same pins, or in dual mode. To use simultaneous master or slave, or to enable a second Wire interface, the appropriate option must be selected from tools -> Wire Mode in addition to calling the correct form of `Wire.begin()`. This is fully documented in the [Wire.h documentation](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/Wire/README.md)
* Tools -> Optimization Level - We now have a tools submenu that allows you to select either -Os or -O3 with or without GCSE. Note that we do not warrantee that options other than the default will work. The equivalent menu on megaTinyCore has been found to have issues compiling sketches that use the serial port, and I don't know how to fix the issue; the compiler insists on butchering hand optimized assembly in ways that make no sense, and it's very difficult to debug because of LTO.

### Optiboot only menu options
* Tools -> Optiboot pins
As the name implies, this menu lets you select when UART and pin mapping is used for Optiboot. In the case of the AVR DD-series parts (only) this ALSO determines what pin the Optiboot triple blink is located on. See the  [DD14 part specific docs](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DD14.md) for the full story on that.

### Options not available with Optiboot
* Tools -> Write flash from App - Either disabled (Flash.h library does not work), "Everywhere" (allow writes everywhere in the flash after first page), or allow writes only above a certain address. On Optiboot configurations, the writing is mediated by Optiboot, and it's **always enabled for writes anywhere** (except to the bootloader itself. Self programming of the bootloader section can never be performed on modern AVRs, no matter what tricks are used. You cannot make a bootloader that upgrades itself like digispark and the like can.

## Support Continued Development
I sell breakout boards with regulator, UPDI header, and Serial header and other basic supporting parts in my Tindie shop, as well as the bare boards. Buying from my store helps support further development on the core, and is a great way to get started using these exciting new parts with Arduino. Note that we do not currently sell a 28-pin version - this did not seem like a compelling part with the availability of the 32-pin version; the main appeal of the 28-pin part is that it is available in a through-hole version. As we would not be able to make the 28-pin version significantly smaller, there did not seem to be a compelling reason to create a 28-pin version. We may revisit this decision in the future, including potentially a 28-pin bare board for the through-hole version, suitable for assembly by those not experienced with drag soldering.
### [Assembled Boards](https://www.tindie.com/products/21007/)
### Assembled AVR128DB64 Boards - Coming Soon
### [Bare Boards](https://www.tindie.com/products/21008/)
### "Azduino Nano DB" inspired by the shape of a Nano, but with a more coherent pinout - Coming Soon
### DIY 2" x 2" DA-series or DB/DD-series prototyping boards - Coming Soon


## Warnings and Caveats
There are however a few cautions warranted regarding DxCore - either areas where the core is different from official cores, or where the behavior is the same, but not as well known.

### Direct Register Manipulation
If you are manually manipulating registers controlling a peripheral, except as specifically noted in relevant reference pages, the stated behavior of API functions can no longer be assured. It may work like you hope, it may not, and it is not a bug if it does not, and you should not assume that calling said API functions will not adversely impact the rest of your application. For example, if you "take over" TCA0, you should not expect that using `analogWrite()` - except on the two pins on the 20/24-pin parts controlled by TCD0 - will work for generating PWM. If you reconfigure TCA0 except as noted in Ref_Timers, without calling `takeOverTCA0`, both `analogWrite()` and `digitalWrite()` on a PWM pin may disrupt your changed configuration.
#### Timer PWM and exceptions
In the special case of TCA0, TCA1, and TCD0, a special function called `takeOverTCAn()` (or `takeOverTCD0()`) is provided - calling this will tell the core that you are assuming full responsibility for everything related to that timer. analogWrite on pins it is pointed at will not turn on PWM, nor will digitalWrite turn it off. This function will not be available for any timer used to control millis timekeeping (and manually reconfiguring such a timer should be expected to break timekeeping. Note that if you are using PWM on a pin provided by a type B timer (not recommended, they're lousy at it) they depend on the prescaler settings of a type A timer See the [Timers and PWM](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Timers.md) reference for more information.

TCD0 has additional specific exceptions to the normal "manual configuration = our API functions won't work right" policy, in order to keep it from being forever unused beyond what analogWrite() defaults to (it can be slowed way down, or sped way the hell up, and some of the advanced functions can be used as long as you don't touch certain parts.)
## Differences in Behavior between DxCore and Official Cores
While we generally make an effort to emulate the official Arduino core, there are a few cases where the decision was made to have different behavior to avoid compromising the overall functionality; the official core is disappointing on many levels. The following is a (hopefully nearly complete) list of these cases.

### I2C **requires** external pullup resistors
Earlier versions of megaTinyCore, and possibly very early versions of DxCore enabled the internal pullup resistors on the I2C pins. This is no longer done automatically - they are not strong enough to meet the I2C specifications, and it is preferable for it to fail consistently without external ones than to work under simple conditions with the internal ones, yet fail under more demanding ones (more devices, longer wires, etc). However, as a testing aid, we supply Wire.`usePullups()` to turn on the weak internal pullups. If `usePullups()` ever fixes anything, you should install external pullups straight away. Our position is that whenever external pullups are not present, I2C is not expected to work. Remember that many modules include their own on-board pullups. For more information, including on the appropriate values for pullups, see the [Wire library documentation](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/Wire/README.md)

### Serial Does Not Manipulate Interrupt Priority
The official core for the (similar) megaAVR 0-Series parts, which megaTinyCore was based on, fiddles with the interrupt priority (bet you didn't know that!) in methods that are of dubious wisdom. megaTinyCore does not do this, saving several hundred bytes of flash in the process, and fixing at least one serious bug which could result in the microcontroller hanging if Serial was used in ways that everyone tells you not to use it, but which frequently work anyway. Writing to Serial when its buffer is full, or calling `Serial.flush()` with interrupts disabled, or during another ISR (which you *really shouldn't do*) will behave as it does on classic AVRs and simply block, manually calling the transmit handlers, until there is space in the buffer for all of the data waiting to be written or the buffer is empty (for `flush()`). On th stock megaAVR core, this could hang forever.

### SerialEvent Support is Dropped
This is deprecated on the official core and is, and always has been, a dreadful misfeature. Dropped as of 1.3.0.

### `digitalRead()` does not turn off PWM
On official cores, and most third party ones, the `digitalRead()` function turns off PWM when called on a pin. This behavior is not documented by the Arduino reference. This interferes with certain optimizations, makes `digitalRead()` take at least twice as long (likely much longer) as it needs to and generally makes little sense. Why should a "read" operation change the thing it's called on? We have a function that alters the pin it's called on: `digitalWrite()`. There does not seem to be a logically coherent reason for this and, insofar as Arduino is supposed to be an educational platform it makes simple demonstrations of what PWM is non-trivial (imagine setting a pin to output PWM, and then looking at the output by repeatedly reading the pin).

### `digitalWrite()`/`pinMode()` and INPUT pins
Like the official "megaavr" core, calling `digitalWrite()` on a pin currently set INPUT will enable or disable the pullups as appropriate. Recent version of DxCore fix two gaps in this "classic emulation". On a classic core, `digitalWrite()` on an INPUT would also write to the port output register - thus, if one subsequently called `pinMode(pin, OUTPUT)`, the pin would immediately output that level. This behavior is not emulated in the official core, and there is a considerable amount of code in the wild which depends on it. `digitalWrite()` now replicates that behavior. `digitalWrite()` also supports `CHANGE` as an option; on the official core, this will turn the pullup on, regardless of which state the pin was previously in, instead of toggling the state of it. The state of the pullup is now set to match the value that the port output register was just set to.

Similarly, using `pinMode()` to set a pin to `INPUT` or `INPUT_PULLUP` will now also set the port output register. **Do not call pinmode on a pair of I2C pins that is being used for that purpose, ever** Wire.`begin()` and optionally `usePullups()` will do that for you, while `pinMode()` will break them.

### `analogWrite()` and TCD0 pins
See the [TCD0 reference](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_TCD.md) - 0 and 255 produce constant output while the pin remains connected to the timer. digitalWrite will fully disconnect it.

### Serial does not manipulate interrupt priority
The official core for the (similar) megaAVR 0-series parts, which this was based on, fiddles with the interrupt priority (bet you didn't know that!) in ways that are of dubious safety towards other code. megaTinyCore does not do this (in the process saving several hundred bytes of flash). Writing to Serial when it's buffer is full, or calling `Serial.flush()` while  with interrupts disabled, or during another ISR (which you really shouldn't do anyway) will behave as it does on classic AVRs, and simply block until there is either space in the serial buffer, or the flush is completed. This will probably never be a problem, and I think removing that fixed several bugs.

### Serial setting constants have different numeric values
The official core defined 4 different settings that were rolled up into constants like `SERIAL_8N1`, and all of them went into CTRLC. You'd think they'd be in the order they were in, in PORTC, and the constants would be 8-bits in size right? They're not, and a lookup gets done within Serial.begin(). Utter insanity! We now have 16-bit settings values - *but that's because we specify 7 additional options!* This will not impact you unless you were using numbers instead of the names for those constants (who does that?!)

### TCA(s) are configured in Split Mode to get 3 additional PWM pins
On official "megaavr" board package, TCA0 is configured for "Single mode" as a three-channel 16-bit timer (used to output 8-bit PWM anyway). DxCore always configures Type A timers for "split mode", which turns it into 6 independent 8-bit channels. See the datasheets for more information on the capabilities of these peripherals. See [**Taking over TCA0**](megaavr/extras/TakingOverTCA0.md) for information on reconfiguring it.

### TCA0/1 and all TCB's used for PWM have TOP at 254, not 255
0 is a count, so at 255, there are 256 steps, and 255 of those will generate PWM output - but since Arduino defines 0 as always off and 255 as always on, there are only 254 possible values that it will use. The result of this is that (I don't remember which) either `analogWrite(pin,254)` results in it being LOW 2/256's of the time, or `analogWrite(pin,1)` results in it being `HIGH` 2/256's of the time. On DxCore, with 255 steps, 254 of which generate PWM, the hardware is configured to match the API. If you make a graph of measured duty cycle vs the value passed to analogWrite, it is a straight line with no kink or discontinuity and an intercept at 0. In the event that TCA0 is used for millis, as it happens, 255 also (mathematically) works out in a uniquely favorable way, resulting no long term rounding loss without added steps to compensate, which not not the case for most numbers.

### digital I/O functions use old function signatures
They return and expect uint8_t (byte) values, not enums like the official megaavr board package does, and pin numbers are uint8_t (byte) Like classic AVR cores, constants like `LOW`, `HIGH`, etc are simply #defined to appropriate values. There are several fundamental problems to switching to enums, and the idea looks good only from a distance, and the hack they used to fix the compatibility problems also eliminated any benefit from it. [MegaCoreX](https://github.com/MCUdude/MegaCoreX) and [megaTinyCore](https://github.com/SpenceKonde/megaTinyCore) have done this as well.

### `analogReadResolution()` is different
Official AVR boards do not have analogReadResolution. Official ARM-based boards do, but the implementation on those boards, awash in memory and flash, is very different - they allow specifying any number from 1 to 32, and will shift the reading as required (padding it with zeros if a resolution higher than the hardware is capable of is specified). I dislike conceptually the idea of the core presenting values as if it has more precision than it does, and in any event, on these resource constrained 8-bit microcontrollers, the code to create rounded or padded numbers with 1-32 bits is an extravagance we cannot afford, and the overhead of that should not be imposed on the vast majority of users who just want to read at the maximum resolution the hardware supports, or 10 bits for code compatibility with classic AVRs. Since `analogReadResolution()` accepts a wide range of values on the official boards that have it, it does not need to report success or failure.

### As of 1.3.3, SerialEvent is removed
SerialEvent was an ill-conceived mess. I knew that when I added support for it, but I didn't know that the mess had already been deprecated; when I heard that it was, I wasted no time in fully removing it.


### Oh, and -Wall (compile warnings) are enabled no matter what you choose in the preferences
The IDE defaults to "none", and the majority of users go through life unaware that they have this critical source of debugging information turned off. Warnings should be opt-out, not opt-in. Almost every time I see a warning, it's a latent bug that impacts behavior in an unwanted way. It's mighty handy to have the compiler tell you where your bugs are. The core and the libraries included are free of things that generate warnings - if you ever get a warning from a core file or included library which is not a `#warning`, that is a bug and should be reported as a github issue. Warnings are a good thing to keep enabled, running with warnings disabled is just making life harder for yourself; since most people using Arduino are unaware of that setting, the core is improved by ensuring that they are always shown.

## Instruction Set Enhancements (AVRe/AVRe+ vs AVRxt)
The classic AVR devices all use the venerable `AVRe` (ATtiny) or `AVRe+` (ATmega) instruction set (`AVRe+` differs from `AVRe` in that it has hardware multiplication and supports devices with more than 64k of flash). Plain `AVR` is very old, and very few devices still in production use it - it didn't support parts with over 8k of flash (no jump/call) nor a bunch of other normal functions. The few production parts that use it (like the tiny15) also had a bunch of other functions dropped. In practice, AVR has meant `AVRe` or `AVRe+` for most of it's life. Modern AVR devices (with the exception of ones with minuscule flash and memory, such as the ATtiny10, which use reduced core `AVRrc` which drops many of the more complicated instructions), use the latest iteration of the AVR instruction set: `AVRxt`. This adds no new instructions (unlike `AVRxm`, the version used by the XMega devices, which added 4 combined read-write instructions for accessing SRAM. (which I think doesn't include peripheral registers, which takes out all the use cases for it that I can think of), but a small number of instructions have improved execution time, (and one has slower execution time). This distinction is unimportant for 99.9% of users - but if you happen to be working with hand-tuned assembly (or are using a library that does so, and are wondering why the timing is messed up), this could be why:
* PUSH is 1 cycle vs 2 on classic AVR (POP is still 2) (this is the biggest change in terms of overall speed - It is not unusual for 20% or more of an arduino sketch to be push and pop instructions! Now, one could argue that this is an incitement of the author and framework - I wouldn't disagree, but this is an Arduino core, so... )
* CBI and SBI are 1 cycle vs 2 on classic AVR (Sweeet! very noticeable when we use it, even though it's a relatively rare instruction.)
* LDS is 3 cycles vs 2 on classic AVR :disappointed: LD and LDD are still two cycle instructions.
* RCALL and ICALL are 2 cycles vs 3 on classic AVR
* CALL is 3 cycles instead of 4 on classic AVR
* Best one for last: ST and STD is 1 cycle vs 2 on classic AVR! (STS is still 2). This includes std as well, which makes this nearly as large as the improvement to push.
* When ld, ldd, or lds is used to load data from flash using the flash mapping... it takes an extra clock for the NVM access (as far as I can tell; the location in the datasheet where the instruction set manual "says more information can be found" has no such information or mention of any delay, but the instruction set manual says "minimum of 1 cycle must be added".)
* Not quite the instruction set... but on classic AVRs, the working registers were accessible at locations 0x00 to 0x1F in the main address space. They no longer are. That means the offset of 0x20 applied to peripheral registers is gone, and the SFR_TO_IO_ADDR macro is gone too (SFR stands for Special Function Register I think)

This really comes down to 2 changes
* Faster stores - including ST, STD, and PUSH (PUSH and POP are essentially ST SP+ and LD -SP, treating the stack pointer like one of the register pairs And since CALL and RCALL involve pushing something onto the stack, you'd pick up enhancement there for free too. This almost certainly accounts for the slowdown in LDS. If I had to guess, I would say that LD's take 1 clock to send the address to the memory controller, and get the data in the next clock. Since in LDS, it doesn't have the address until the second clock, there you go.
* CBI/SBI are single clock - unclear if the mechanism is the same as above.

## License
DxCore itself is released under the [LGPL 2.1](LICENSE.md). It may be used, modified, and distributed freely, and it may be used as part of an application which, itself, is not open source (though any modifications to these libraries must be released under the LGPL as well). Unlike LGPLv3, if this is used in a commercial product, you are not required to provide means for user to update it.

The DxCore hardware package (and by extension this repository) contains DxCore as well as libraries, bootloaders, and tools. These are released under the same license, *unless specified otherwise*. For example, tinyNeoPixel and tinyNeoPixel_Static, being based on Adafruit's library, are released under GPLv3, as described in the LICENSE.md in those subfolders and within the body of the library files themselves.

The pyupdi-style serial uploader in megaavr/tools is a substantially renovated version of pymcuprog from Microchip, which ~is not open source~ *has now been released under the open source MIT license!*.

Any third party tools or libraries installed on behalf of DxCore when installed via board manager (including but not limited to, for example, avr-gcc and avrdude) are covered by different licenses as described in their respective license files.
