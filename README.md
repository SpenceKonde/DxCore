### [Wiring](Wiring.md)
### [Installation](Installation.md)
### [Making a cheap UPDI programmer](https://github.com/SpenceKonde/AVR-Guidance/blob/master/UPDI/jtag2updi.md)

## **YES - the Curiosity Nano is finally supported!**

# DxCore - Arduino support for the NEW AVR DA-series, DB-series and upcoming DD-series
This is an Arduino core to support the exciting new AVR DA, DB, and "coming soon" DD-series microcontrollers from Microchip. These are the latest and highest spec 8-bit AVR microcontrollers from Microchip. It's unclear whether these had been planned to be the "1-series" counterpart to the megaAVR 0-series, or whether such a thing was never planned. But whatever the story of it's origin, these take the AVR architecture to a whole new level.  With up to 128k flash, 16k SRAM, 55 I/O pins, 6 UART ports, 2 SPI and I2C ports, and all the exciting features of the tinyAVR 1-series and megaAVR 0-series parts like the event system, type A/B/D timers, and enhanced pin interrupts... But for almost every system they've added a significant improvement of some sort (while largely preserving backwards compatibility. You liked the type A timer, but felt constrained by having only one prescaler at a time? Well now you have two of them (on 48-pin parts and up)! You wished you could make a type B timer count events? You can do that now! (this addresses something I always thought was a glaring deficiency of the new peripherals and event system). We still don't have more prescale options (other than having two TCA's to choose from) for the TCB - but you can now combine two TCB's into one, and use it to do 32-bit input capture. Time a pulse or other event up to approximately 180 seconds long... to an accuracy of 24th's of a microsecond! And of course, like all post-2016 AVR devices, these use the latest incarnation of the AVR instruction set, AVRxt, with the slightly-improved instruction timing compared to "classic" AVRs

Oh and you wish you had a bit more accuracy on the ADC? Yup - this is a 12-bit ADC - and the DAC (oh yeah, it has one of those) is 10-bits instead of the 8 that they tinyAVR 1-series had!

As if that all isn't enough, there's a 28-pin version in a DIP package. The 28-pin version really doesn't show the full power of these parts, but it's far better than nothing for those who aren't comfortable with using surface mount parts and have been feeling left out of the party (of course, you can buy breakout boards in my Tindie store of all sizes, even the 64-pin ones! Or you can buy a Curiosity Nano from Microchip, if you prefer that style of development board.)

# Big Picture
These parts depart from the naming scheme used for AVR devices in the past; these are named AVR followed by the size of the flash, in KB, followed by DA, DB, or DD (depending on the "series" or "family", then the number of pins. Note that the pin count also determines how many of certain peripherals the parts have available - parts with more pins have more peripherals to do things with those pins. 64-pin parts are not available in 32k flash size. There are no other

At present, it appears that there will be at least three lines of AVR Dx-series parts: The "basic" DA, the DB with improved featureset, and the low pincount DD; all are supported by DxCore - the peripherals are virtually identical, as are the pinouts. It appears overwhelmingly likely that future releases of full-size parts will continue to use compatible pinouts for the foreseeable future.

## DA-series
The "basic" large-size line - however much I was in awe of these when they were first released, having seen the DB-series, it now appears that these are more akin to a 0-series than a 1-series - by almost any measure, the DB-series is the same or slightly better (and barely more expensive!). They do not support using an external crystal for the main clock, like the other Dx parts do, but the internal oscillator on these parts is still WAY better than the classic AVRs had - all the ones I've tested are weithin half a percent at room temp and typical operating voltages, even without autotune... To make sure autotune was working, I had to point a torch at it, because I couldn't get enough of a change in the internal oscillator frequency from changing the supply voltage. It is also the only currently announced Dx series without MVIO. While they may not shine as brightly next to the other Dx lines, these are still far above any AVR released before the year 2020.

## DB-series
The DB-series is almost an exact copy of the DA-series (they fixed some of the most egregious silicon bugs, though they have hardly been a paragon of rigor ), only with a few MORE exciting features tacked on: Support for a real high-frequency crystal as clock source (seen for the first time since the modern AVR architecture was released in 2016), "MVIO" (multivoltage I/O), where PORTC can run at a different voltage than the rest of the chip (essentially, a builtin bidirectional level shifter). The other "headline feature", is the  two (28/32-pin parts) or three (48/64-pin parts) on-chip opamps, with software-controlled MUX for the inputs and an on-chip feedback resistor ladder. These can be used as gain stage for the ADC, for example, or to buffer the DAC output (though these opamps can't supply much current, they can supply tens of mA, instead of ~ 1 like the unaided DAC), connected together like the CCL LUTs. etc (on parts with 3, you can even connected them together as an instrumentation amplifier). The included `<opamp.h>` library provides a minimalist wrapper around these, though you should still expect to refer to the datasheet when working with them..

## DD-series
The DD-series is a smaller-pincount line; parts are available with 14-32 pins. They've got the MVIO (3 or 4 MVIO pins depending on pincount). The product brief claims 10 output pins, 11 input pins,  on the 14-pin package. With VDD, VDDIO, and GND. That implies that there will be a way to configure the UPDI pin to act as an I/O pin, and Reset to act as an input only if configured appropirately in the fuses. We'll have to wait until more information is available, but it sounds like the reset pin on these parts will be the pin that needs the HV pulse. One thing worth noting is that the 28-pin and 32-pin DD-series parts are, but for expanded port multiplexing options for SPI0, USART0, and TCD0, and the addition of ADC input functionality on pins in PORTA and PORTC,  One imagines that, since these do have enhanced PORTMUX options for TCD0, and since Microchip has been aware since the summer of 2020 that the TCD0 PORTMUX options other than the default were broken on DA-series and DB-series, we should anticipate that this issue should be fixed in the DD-series.

## Timeline
128k parts were released in April 2020, followed by 32k in early summer (though the initial release of the 32k parts suffered from a fatal flaw and was recalled, working ones were not available until the end of 2020), while the 64k parts became available late in summer of 2020, around the same time as the first AVR128DB-series parts became available. The AVR DD-series product brief was released in spring of 2020, with hardware availability beginning with the 64k parts, expected H1 2021.As of the beginning of Q2 2021, 64k and 32k DB-series parts are unavailable. It also looks disturbingly likely that the DD-series will be made available in the 32 and 28 pin packages (which is essentially a poverty-model DB with no opamps... and I guess slightly better mux options for USART and SPI0.... ) before the exciting 20 and 14 pin packages.

## Lots of errata
The silicon errata list in the initial versions of these parts is... longer than you may be used to if you were accustomed to the classic AVRs. If you've been working with the tinyAVR 0/1-series, many of these errata may be old friends of yours by now. A good number (really, I should say, a "bad number") of those issues appear to have sailed through the new chip design process without a remedy - and recently as late 2020, new silicon bugs have been appearing in the tinyAVR 0/1 errata and DA/DB errata simultaneously, presumably issues that were discovered on the DA-series and then found to impact the tinyAVR 0/1-series as well. See [errata and extras](megaavr/extras/errata_and_extras.md) for more information.  There were also a number of features that were pulled from the documentation at the last minute (in some cases, they were left in the io header files for a while). Presumably these did not meet quality/reliability requirements. Where we are aware of these, we have mentioned them there as well - the most exciting one, of course, is that the internal oscillator has a 32 MHz setting (and it seems to work fine in my tests; 32 MHz is also a great speed because of it's exactly twice what one of the most common speeds is). The PLL can also go to 4x multiplier (and appears to work there, to my great amazement, even at 32 MHz system clock! Very impressive on a 5v microcontroller, especially to those like myself who have been living mostly in an architecture that had been stuck at the same maximum clock speed for over a decade...).

## Supported Parts (click link for pinout diagram and details)
Note that you must install via board manager or replace your tool chain with the azduino3 version pulled in by board manager in order to work with anything other than an AVR128DA. Note also that there is a defect in the AVR32DA parts: interrupts do not work correctly (the chip has 2-byte vectors in the hardware, instead of 4-byte ones... it's got more than 8k flash, so that's not going to work no matter what - but it *really* doesn't work with the compiler making 4-byte vector binaries!). The AVR32DA parts in circulation have been recalled - It would be mighty nice if they had updated the silicon errata sheet though!
* [AVR128DA28, AVR64DA28, AVR32DA28](megaavr/extras/DA28.md)
* [AVR128DA32, AVR64DA32, AVR32DA32](megaavr/extras/DA32.md)
* [AVR128DA48, AVR64DA48, AVR32DA48](megaavr/extras/DA48.md)
* [AVR128DA64, AVR64DA64](megaavr/extras/DA64.md)
* [AVR128DB28, AVR64DB28, AVR32DB28](megaavr/extras/DB28.md)
* [AVR128DB32, AVR64DB32, AVR32DB32](megaavr/extras/DB32.md)
* [AVR128DB48, AVR64DB48, AVR32DB48](megaavr/extras/DB48.md)
* [AVR128DB64 and AVR64DB64](megaavr/extras/DB64.md)
* AVR64DD14, AVR32DD14, AVR16DD14 (pending release - I suspect H1 2020)
* AVR64DD20, AVR32DD20, AVR16DD20 (pending release - I suspect H1 2020)
* AVR64DD28, AVR32DD28, AVR16DD28 (pending release - I suspect H1 2020)
* AVR64DD32, AVR32DD32, AVR16DD32 (pending release - I suspect H1 2020)
**PINOUT CHART NOTE** - the pinout diagrams do not show the TCA0, TCA1, or TCD0 remapping options currently, only the default ones.

My personal opinion is that the 48-pin parts are the "sweet spot" for the DA and DB-series parts - they have the real gems of the product line - the second Type A timer, the two extra CCL LUTs, and enough pins to take full advantage of these peripherals. Most people can't really find something to do with a whole 64 pins in one project - short of indulging in kitchen-sink-ism just to take up pins. But the 27 I/O pins on the 32-pin parts can go faster than one might think (I had one project a while back where I switched to a '328PB instead of a '328P for the Rev. B, because otherwise I was 1 pin short of being able to lose the I2C backpack on the '1602 LCD, and if I did that, I could integrate the whole thing onto one PCB, and have a rigid connection between the LCD and main PCB - though I think I could just squeeze that project into a DA32).

For the upcoming DD-series, the 28 and 32-pin parts are of questionable utility considering the existence of the more capable DA and DB parts with the same number of pins. With AVR Dx parts already priced like the higher-end classic tinyAVR devices, and barely twice the price of the top edge of the tinyAVR 0/1/2-series, there is a pretty narrow range of prices that the DD-series would make sense at. The 14-pin and 20-pin packages are far more interesting, packing Dx-level capabilities intosizes and pincounts that are normally the province of the less capable tinyAVR product line.

## Supported Clock Speeds
All speeds are supported across the whole 1.8V ~ 5.5V operating voltage range!
**Crystal** is not supported as a clock source on the DA-series, only DB and DD.
* 24MHz Internal, Ext. Clock or Crystal
* 20MHz Internal, Ext. Clock or Crystal
* 16MHz Internal, Ext. Clock or Crystal
* 12MHz Internal, Ext. Clock or Crystal
*  8MHz Internal, Ext. Clock or Crystal
*  4MHz Internal, Ext. Clock or Crystal
*  1MHz Internal or Ext. Clock
* 28MHz Internal, Ext. Clock or Crystal Overclocked, not guaranteed!
* 32MHz Internal, Ext. Clock or Crystal Overclocked, not guaranteed!
* 36MHz Ext. Clock or Crystal Overclocked
* 40MHz Ext. Clock or Crystal Overclocked - probably won't work!
* 48MHz Ext. Clock or Crystal WAY Overclocked - almost certainly won't work!

The severely overclocked options are there for one simple reason - I have yet to get boards made with pads for a proper external clock with which I could test just how far I could push the speed before they stopped working. That will be in my next batch, so soon I may remove the highest speeds.

There are multiple ways to generate some of the lower frequencies from internal oscillator (do you prescale from higher frequency, or set the oscillator to the desired one? Suspect the latter is more power efficient, but with the former you could still use the PLL while staying in spec - though in my tests the PLL worked well beyond the spec in both directions, at least at room temperature) - currently, we set the main oscillator to the desired frequency, however we may revisit this decision in the future.

The DA-series does not support use of an external high frequency crystal (though the DB/DD-series do) - the internal oscillator is tightly calibrated enough that the internal clock will work fine for UART communication, and an external watch crystal can be used to automatically tune the internal oscillator frequency, a feature called Auto-Tune. Though they specify +/- 3% internal oscillator speed, in practice, I have yet to find one that was off by more than 1 calibration "notch" at room temperature. These are just in a different universe than the classic AVRs where a couple percent was normal. Nonetheless, we provide a wrapper around enabling external 32K crystal and enabling/disabling Auto-Tune in [DxCore library](megaavr/libraries/DxCore).

```c
#include <DxCore.h>

void setup() {
  //optionally call configXOSC32K() to get specific crystal settings; otherwise it uses conservative defaults.
  enableAutoTune(); //that easy - this returns 0 on success, you can check that if you want to be particularly
  // careful about whether or not it worked it can not-work if the crystal doesn't actually start
  // oscillating, either due to design flaws  inappropriate loading caps, improper crystal selection,
  // or poor layout, or for other reasons such as damage, extreme temperature, and so on.

  // more stuff after this to set up your sketch
}

```

There are a *lot* of strange clock speeds possible through combinations of prescalers and the internal oscillator - ever wanted to run an MCU at 7 MHz? *Me neither*, but you totally can, even without a crystal... These exotic speeds are not currently supported by DxCore - I'd be lying if I said I missed the struggle to make millis accurate with weirdo clock speeds back on ATTinyCore (which in turn was done to support UART crystals, which the fractional baud rate generator has made obsolete).

### Clock troubleshooting
On a classic AVR, the result of selecting a clock source (external crystal or clock) which does not function is not subtle: You burn the bootloader to set the fuse to use that clock source, and the chip ceases to respond, including all attempts to program. Fortunately, the Dx-series parts handle this situation more gracefully. However, without assistance from the core, recognizing that the problem was in fact a missing clock could be challenging. In order to aid in debugging such issues, DxCore will never run the sketch if the selected clock is not present. It will try for a short time before giving up and showing a blink code on pin PA7 (Arduino pin number 7); this blink code will be shown until the chip is reset. Similarly, on the DB-series, which features Clock Failure Detection, a clock failure at runtime will trigger a different blink code.

#### Blink Codes
All blink codes issued by the core start with the LED pin switching states three times (ie, if it is off, it will turn on, off, then on again), followed by a number of flashes indicating the nature of the failure. This will then repeat - since it initially changes state three times, this means that the pattern will be inverted the second time through. The number of short flashes indicates the nature of the problem: Three flashes indicates that the selected clock source was not present at startup. Four flashes indicates that it was present at startup, but then disappeared while it was running. It is hoped that this will make the blink codes distinctive enough that they cannot be mistaken for the running sketch. The core provides no facility to disable this, move it to another pin, or otherwise alter the clock source startup behavior. If you require this, compile with the internal oscillator at the desired frequency selected, and switch to the crystal or external clock at the beginning of your `setup()` function. In this case, on the DB-series parts, the clock failure detection will not be enabled, and - if you wish - you may enable it and write your own ISR to handle the CFD interrupt.

**The AVR DA-series does not provide clock failure detection (CFD)** - on DA-series parts, if the external clock is removed, it will just stop unless you use the watchdog timer to reset it in the event of such a hang.

**Severe disruptions to the crystal pins can trigger a reset** when that crystal is used as the main clock source. This means that even if the chip was running prior to a disruption to the crystal, you may end up with the 3-blink code, not the 4-blink one. These blink codes are meant only as a debugging aid ("your crystal oscillator isn't" is typically enough to get you on the right path anyway) - they do not replace careful (or - in this case - basic) observation of a malfunctioning project.

# UPDI programming
The UPDI programming interface is a single-wire interface for programming (and debugging - **U**niversal **P**rogramming and **D**ebugging **I**nterface) used on the AVR Dx-series, tinyAVR 0/1/2-series, megaAVR 0-series, and which will likely be used for all AVR microcontrollers for the foreseeable future. In addition to purchasing a purpose-made UPDI programmer (such as the ones produced by Microchip), there are two very low-cost approaches to creating a UPDI programmer:


## From a USB-Serial adapter with included SerialUPDI (recommended)
Before megaTinyCore existed, there was a tool called [pyupdi](https://github.com/mraardvark/pyupdi) - a simple python program for uploading to UPDI-equipped microcontrollers using a serial adapter modified by the addition of a single resistor. But pyupdi was not readily usable from the Arduino IDE, and so this was not an option. As of 2.2.0, megaTinyCore brings in a portable Python implementation, which opens a great many doors; Originally we were planning to adapt pyupdi, but at the urging of the pyupdi author and several Microchip employees, we have instead based this functionality on [pymcuprog](https://pypi.org/project/pymcuprog/), a tool developed and maintained by Microchip which includes the same serial-port upload mechanism (though within that context the SerialUPDI capacity is rather hidden - that tool is more about uploading via the native-USB debuggers sold by Microchip. I had initially hoped to use it for that as well, but it appeared that wrangling the USB/driver angle was going to be a problem - so when the fix for avrdude to support the 24-bit firmware on the Microchip Curiosity boards came out, we lost interest in that. **If installing manually** you must [add the python package](megaavr/tools/ManualPython.md) appropriate to your operating system in order to use this upload method (a system python installation is not sufficient, nor is one necessary).

With the release of 2.3.6 and the dramatic improvements in performance of SerialUPDI, **this is now the recommended method to program these parts**
[Instructions for wiring a serial adapter for SerialUPDI](https://github.com/SpenceKonde/AVR-Guidance/blob/master/UPDI/jtag2updi.md)

Note that this does not give you serial monitor - you need to connect a serial adapter the normal way for that (I suggest using two, along with an external serial terminal application). This technique works with those $1 CH340 serial adapters from ebay, aliexpress, etc. Did you accidentally buy some that didn't have a DTR pin broken out, and so weren't very useful with the Pro Minis you hoped to use them with?

## With an Arduino using jtag2updi
One can be made from a classic AVR Uno/Nano/Pro Mini; inexpensive Nano clones are the usual choice, being cheap enough that one can be wired up and then left like that - see [Making a UPDI programmer](https://github.com/SpenceKonde/AVR-Guidance/blob/master/UPDI/jtag2updi.md); using these, you should select jtag2updi from the tools->programmer menu. Prior to the release of 1.3.6  this was the recommended method, despite the balky jtag2updi firmware and incompatibility with converting an Arduino Micro because of the fiddliness and poor performance of SerialUPDI. Those issues have been addressed with a more robusty wiring scheme (replacing the 4.7k resistor with a schottky diode greatly widens the operating window) and software improvements have increased the speed *by a factor of 20-30* for uploading to Dx parts.

## Optiboot-derived bootloader
There is now support for an Optiboot derived bootloader! See the Bootloader section below for more information. The bootloader, of course, requires a UPDI programmer to install.

# Features

## Memory-mapped flash? It's complicated.
Unlike the tinyAVR 0/1-series and megaAVR 1-series parts, which are able to map their entire flash to memory, the DA-series parts can only map 32KB at a time. The FLMAP bits in NVMCTRL.CTRLB control this mapping. Unfortunately, because this can be changed at runtime, the linker can't automatically put constants into flash on 64k and 128k parts. However, on 32k parts, it can, and does!

As of 1.2.0, you can declare a ~const~ variable `MAPPED_PROGMEM`; this will put it in the final section of flash (section 1 or 3 - they're 0-indexed); in this case, the data is not copied to RAM, and *you can use the variable directly to access it through the mapped flash!* (this only works if you don't change which section of the flash is mapped in NVMCTRL.CTRLB); you can store up to 32k of data this way. The `PROGMEM` attribute also works normally, ie, if you declare something PROGMEM, it will be stored in flash in the lower 64k (if possible), and can be accessed using `pgm_read_*` macros.

Note that the errata relating to the memory mapping on the AVR128DA parts is not a problem for the application, as the bootloader does not set BOOTRP, and the application cannot directly write to the application section of the flash. n parts with more than 32k of flash, the bootloader uses the SPM instruction to write the flash one word at a time, rather than ST to write it one byte at a time; this is uneffected by the errata.

The `F()` macro works the same way as it does on normal boards as of 1.2.0, even on the 32k parts, where it is unnecessary to save RAM - this was done in order to maintain library compatibility; several very popular libraries rely on F() returning a `__FlashStringHelper *` and make use of `pgm_read_byte()` to read it.

## Writing to Flash from App
It is possible to write to the flash from the application code using the included Flash.h library. Please give me feedback on this one, as I don't know how suitable the user interface is or what features ought to be added to it. This works whether or not the Optiboot bootloader is in use!
* [Flash.h](megaavr/libraries/Flash/README.md) presents the same functions as methods of a `Flash` object\


## Bootloader support
DxCore now an Optiboot-derived bootloader for all parts! This can be installed using a UPDI programmer by selecting the desired part, and using the Tools -> Burn Bootloader option. Note that after the bootloader has been installed in this way, to use it without the bootloader, you must choose the non-optiboot board definition, and then again Burn Bootloader to configure the fuses appropriately; when the bootloader is enabled the vectors are located at the start of the application section, 1024 bytes in (like megaAVR 0-series and tinyAVR 0/1-series, and unlike classic AVRs, the bootloader section is at the beginning of the flash). Options to set the desired USART that the bootloader will run on are available for all serial ports, with either the normal or alternate pin locations. USART0 with default pins is the default option here, and these are the ones that are connected to the 6-pin serial header on the DA-series breakout boards that I sell. An option is provided when burning bootloader to choose an 8 second delay after reset - use this if you will be manually pressing reset

Once the part is bootloaded, sketches can be uploaded by connecting a serial adapter to those pins (including the usual DTR-autoreset circuit, present on my breakout boards), and clicking upload. If autoreset is not practical for whatever reason, an 8-second timeout version of the bootloader is provided. When reset is pressed, the bootloader will be active for the next 8 seconds. This may also be useful in combination with the software reset for updating a device in an inaccessible location.

### Bootloader size
As of 1.2.0, the Optiboot bootloader now takes up only 512b of flash, just like on the Arduino Uno and similar! If you were previously using DxCore with an older version of the bootloader, you must use a UPDI programmer to "burn bootloader" with the new veraion of the bootloader first. 1.3.0 introduces further enhancements regarding entry conditions; under the hood, there is also full support for writing to flash from the application.

### Bootloader updates recommended!
Since Optiboot was first added to DxCore, every release has included a lot of improvements. If you are programming your AVR Dx-series part through a bootloader, we strongly suggest keeping it updated. 1.3.0 brought another big raft of improvements too.

## Ways to refer to pins
This core uses a simple scheme for assigning the Arduino pin numbers, the same one that [MegaCoreX](https://github.com/MCUDude/MegaCoreX) uses for the pin-compatible megaAVR 0-series parts - pins are numbered starting from PA0, proceeding counterclockwise, which seems to be how the Microchip designers imagined it too.

#### PIN_Pxn Port Pin Numbers (recommended)
**This is the recommended way to refer to pins** Defines are provided of form PIN_Pxn, where x is the letter of the port (A through G), and n is a number 0 ~ 7 - (Not to be confused with the PIN_An defines described below). These just resolve to the digital pin number of the pin in question - they don't go through a different code path. However, they have particular utility in writing code that works across the product line with peripherals that are linked to certain pins (by port), making it much easier to port code between devices with the modern peripherals. Several pieces of demo code in the documentation take advantage of this.

Direct port manipulation is possible on the parts (and is easier to write with if you use PIN_Pxn notation!) - in fact, in some ways direct port manipulation is more powerful than it was in the past. several powerful additional options are available for it - see [direct port manipulation](megaavr/extras/DirectPortManipulation.md).

#### Arduino Pin Numbers
When a single number is used to refer to a pin - in the documentation, or in your code - it is always the "Arduino pin number". These are the pin numbers shown on the pinout charts. All of the other ways of referring to pins are #defined to the corresponding Arduino pin number.

#### An and PIN_An constants
The core also provides An and PIN_An constants (where n is a number from 0 to 21). These refer to the ADC0 *channel* numbers. This naming system is similar to what was used on many classic AVR cores - on some of those, it is used to simplify the code behind analogRead() - but here, they are just #defined as the corresponding Arduino pin number. The An names are intentionally not shown on the pinout charts, as this is a deprecated way of referring to pins. However, these channels are shown on the pinout charts as the ADCn markings, and full details are available in the datasheet under the I/O Multiplexing Considerations chapter. There are additionally PIN_An defines for compatibility with the official cores - these likewise point to the digital pin number associated with the analog channel. Note that channel A0 is on the UPDI/Reset pin - however, even when configured as UPDI, it can be used as an input as long as the signals it can be exposed to do not look like the UPDI enable sequence.

### swap() and pins() for Serial, SPI, and I2C
Like most recent parts, the Dx-series parts have multiple pin-mapping options for many of their peripherals, and the major serial interfaces of the Dx-series parts are not an exception. We provide the usual .swap() and .pins() methods whereby each instance of a UART, SPI interface, or I2C interface has a swap()

### Serial (UART) Support
All of these parts have a several hardware serial ports (USART) - from 3 on the 28-pin parts to SIX on the 64-pin parts! They work exactly like the ones on official Arduino boards. See the pinout charts for the location of the serial pins. On my breakout boards, we provide autoreset support as well (again, just like official Arduino boards). Starting in 1.3.3, USART0 is projected as Serial0. Serial is #defined as Serial0 (so the user need not think about this) whenever using a bare chip, however as explicit support is added for specific development boards where a different port is tied to the USB serial adapter, this will be defined differently for those boards allowing a more transparent user experience, that is Serial.print() will always output to the serial monitor.

On all supported devices, where the appropriate pins are present, they can be pin-swapped - each PORT except PORTD (AVR DD parts don't get one one PORT) gets a USART, which defaults to pins 0 and 1 for RX, TX (2 and 3 for XCK and XDIR - though these are not supported through the Serial class), and 4, 5, 6 and 7 when pinswapped. This is configured using the Serial.swap() or Serial.pins() methods. Both of them achieve the same thing, but differ in how you specify the set of pins to use. This should be called **before** calling Serial.begin().

`Serial.swap(1) or Serial.swap(0)` will set the the mapping to the alternate (1) or default (0) pins. It will return true if this is a valid option, and false if it is not (you don't need to check this, but it may be useful during development). If an invalid option is specified, or if the specified option does not exist, it will be set to the default one. If a mapping option is not available for a port on a specific pincount part

Starting in 1.3.4, additional swap options will be supported:
* `Serial.swap(3)` for all cases except Serial0 on AVR DD-series parts will set the pins to "none" - my understanding is that this is for use with CCL and event system to take over the TX and RX. I hope to have an example of this at some point - it looks like RX can take an arbitrary pin (subject to event channel availability) and any pin that can be used as a CCL output can be used for TX (which of course includes the EVOUT pins since the CCL can drive an event channel) - this of course comes at the cost of a bit of flash plus an event channel and logic block.

* On AVR DD-series parts, where pins are scarce and competition for pins is intense, they added a ton of pin mapping options for Serial and SPI in particular. Serial0.swap() can take 0 (default), 1 (alt1), 2 (alt2), 3 (alt3), 4 (alt4), or 5 (none). Serial1.swap() "just" has 0 (default), 1 (alt1), 2 (alt2) and 3 (none)

`Serial.pins(TX pin, RX pin)` - this will set the mapping to whichever mapping has the specified pins as TX and RX. If this is not a valid mapping option, it will return false and set the mapping to the default. This uses more flash than Serial.swap(); that method is preferred.

When operating at 1MHz, the USARTs can actually run at 115200 baud! (note: prior to 1.3.3, Serial.flush() could hang the system if TCA0 or TCA1 was used as millis timing source - there was a 1 clock cycle window during which, if the millis ISR fired, if the byte finished transmitting before the ISR, flush() would hang - but only the type A timer millis ISR took longer to execute than a byte at 115.2kbaud took to transmit).

Do also note that at relatively high baud rates, Serial.print() may no longer use the buffer, as the relatively slow print() class begins to take a similar amount of time as sending data down the wire (Serial.print - and all things serial adjacent to them, seemingly, are surprisingly slow); there is no set point where this switches, and it the buffer may be used for short stretches while sending a long string..

### SPI support
[SPI documentation](megaavr/libraries/SPI/README.md) A compatible SPI.h library is included; it provides one SPI master interface which can use either of the underlying SPI modules - they are treated as if they are pin mapping options.

### I2C (TWI) support
[SPI documentation](megaavr/libraries/SPI/README.md) All of these parts have two hardware I2C (TWI) peripherals, except the 28-pin version, which has one. TWI0 works exactly like the one on official Arduino boards using the Wire.h library, except that it does not activate the internal pullups unless they are specifically requested (see Wire documentation linked above)

The TWI pins can be swapped to an alternate location; this is configured using the Wire.swap() or Wire.pins() methods. Both of them achieve the same thing, but differ in how you specify the set of pins to use. This should be called **before** Wire.begin().

`Wire.swap(1) or Wire.swap(0)` will set the the mapping to the alternate (1) or default (0) pins. It will return true if this is a valid option, and false if it is not (you don't need to check this, but it may be useful during development). If an invalid option is specified, it will be set to the default one.

`Wire.pins(SDA pin, SCL pin)` - this will set the mapping to whichever mapping has the specified pins as SDA and SCL. If this is not a valid mapping option, it will return false and set the mapping to the default. This uses more flash than Wire.swap(); that method is preferred.

As with megaTinyCore, courtesey of https://github.com/LordJakson, in slave mode, it is now possible to respond to the general call (0x00) address as well. This is controlled by the optional second argument to Wire.begin(). If the argument is supplied amd true, general call broadcasts will also trigger the interrupt. The version supplied with DACore also supports an optional third argument, which is passed unaltered to the TWI0.SADDRMASK register. If the low bit is 0, any bits set 1 will cause the I2C hardware to ignore that bit of the address (masked off bits will be treated as matching). If the low bit is 1, it will instead act as a second address that the device can respond to. While these parts support "dual mode" allowing master and slave operation on different pairs of pins, like the megaAVR 0-series which also has this support in hardware, it is not currently exposed in Arduino; similarly, while these parts support master and slave simultaneously on the same pins, that is also not supported by the Arduino Wire library at this time. Work is ongoing to add support.

Most of these parts have a second TWI interface. A Wire1 library that was compatible with any library that was compatible with Wire (and I mean that in both senses of "any") is possible, but I do not have the skill with C++ classes to do it. If you do and want to be a hero: [Issue #54](https://github.com/SpenceKonde/DxCore/issues/54)

### PWM support
The core provides hardware PWM (analogWrite) support. On all parts, 6 pins (by default, PD0-PD5 for 28 and 32-pin parts, PC0-PC6 for 48 and 64-pin parts) - see the part specific documentation pages for pin numbers) provide 8-bit PWM support from the Type A timer, TCA0. On 48-pin and 64-pin parts, an additional 6 PWM pins are available on PB0-PB5 (by default) from TCA1. Additionally, Type B timers not used for other purposes (TCB2 is used for millis unless another timer is selected, and other libraries mmay use a TCB as well) can each support 1 8-bit PWM pin. The pins available for this are shown on the pinout charts. Additionally, TCD0 provides two additional PWM channels; WOA can output on PA4 or PA6, WOB on PA5, PA7. Those channels can each drive either - or both - of those pins, but only at one duty cycle. Users may prefer to configure this manually - TCD0 is capable of, among other things, generating much higher frequency PWM, as it can be clocked from the PLL at 48MHz (or more, if you don't mind exceeding the specified operating specs - I've gotten it up to 128 MHz, allowing 8-bit pwm at 500 kHz, or a 64 MHz square wave to be generated!).
```c++
analogWrite(PIN_PA4,128);
analogWrite(PIN_PA7,192); //50% PA4, 75% PA7
analogWrite(PIN_PA6,64); //25% PA5, 25% PA6, 75% PA7
//use digitalWrite to turn off the PWM on a pin
// or call turnOffPwm(pin);
```
The TCD0 prescaler is less flexible than one might want it to be. We set TOP and double or quadruple (even octuple, at high clocks) the TOP to lower the frequency. This is detected dynamically (Though existing dutycycles won't be adjusted.) Set TCD0.CMPBCLR to 254, 509, 1019, or (if running at 32 MHz or higher )2039. New in 1.3.1
```
TCD0.CMPBCLR = 1019;
while (!(TCD0.STATUS & TCD_CMDRDY_bm)); //make sure it's ready to accept command
TCD0.CTRLE=TCD_SYNCEOC_bm; //note that analogWrite on any TCD0 pin will do it too.
```
TCA0 and TCA1 will now detect the PORTMUX.TCAROUTEA register. As long as it is set to an option that allows all 6 outputs, analogWrite () and digitalWrite() will work normally with it. See helper functions in DxCore.h library.
If you want to take full control of one of the three pwm timers (maybe you want single mode), just call  `takeOverTCA0();` For the TCA's, it will also force hard reset, so they are returned to you in pristine condition. After this, analogWrite, digitalWrite() and turnOffPWM() will ignore anything these timers might be able to do on pins that those functions are called on.

If you are reconfiguring `TCA0`, `TCA1`, or `TCD0` manually, call `takeOverTCA0()`, `takeOverTCA0()`, or `takeOverTCA0()` first so that the core does not attempt to use that timer for analogWrite(). If taking over `TCD0`, I wish you luck, and may the gods of silicon have mercy; it is one of the most complicated peripherals I have worked with.

**Note that TCA0, and TCA1 if present are configured by DxCore in Split Mode by default, which allows them to generate 8-bit PWM output on 6 pins each, instead of 16-it pwm on three** See the [Taking over TCA0](megaavr/extras/TakingOverTCA0.md) guide for more information on this.
**For general information on the available timers and how they are used PWM and other functions, consult the guide:**
#### [Timers and DxCore](megaavr/extras/PWMandTimers.md)

### EEPROM support
A compatible `EEPROM.h` library is included; this implementation is derived from and fully compatible with the standard `EEPROM.h` api, even though the implementation differs internally.

### No USERROW library yet
Unlike the tinyAVR parts I support on megaTinyCore, there is not yet a `USERSIG` library to write to the "User Signature Space" also called the `USERROW` (the object that the library makes available can't be named that because the I/O headers already define that to point to the `USERROW` in memory, so it would conflict with any definition a library provided; hence why megaTinyCore has USERSIG.h). This is due largely to the need to make design decisions about how to expose it to the user. On megaTinyCore the USERROW can be erased with byte level granularity - like EEPROM - and an identical API to EEPROM is provided for USERROW. On the Dx-series, the only way to erase any one byte within USERROW is to erase the whole thing. While one can do read-modify-erase-write, the standard API would result in dramatic write amplification, as every single byte would result in a full erase and rewrite - even if those bytes were parts of a multi-byte datatype. Checking whether a cell was currently empty or held the same value being written would help greatly, but I would feel a lot better if there were an interface that "admitted" to the user that this was how it had to be, and put a simple wrapper around the "read to memory, modify buffer in RAM, then erase and rewrite EEPROM"

### NeoPixel (WS2812) support
The usual NeoPixel (WS2812) libraries have problems on these parts. This core includes two libraries for controlling WS2812/SK6812/etc LEDs, both of which are tightly based on the Adafruit_NeoPixel library. See the [tinyNeoPixel documentation](megaavr/extras/tinyNeoPixel.md) and included examples for more information. Support is in the code for all clock speeds from 8 MHz to well past the limits of the hardware. I suspect that it could be just barely made to work at 5 MHz or, taking advantage of research on the protocol and how the actual constraints differ from those presented in the datasheets, 4 MHz - but I do not see much demand for such an undertaking. Why run at 5 MHz? If you're driving a string of '2812s you're not worried about power consumption; the other reason to run at low frequencty is to operate at low voltage, but not only are the Dx parts rated for the full 24 MHz from 5.5V all the way down to 1.8V, at any voltage below 4V or so the blue LEDs don't work at full brightness anyway. So I decided that there was no reason to waste time porting the 'WS2812 driver to lower speeds.

### Tone Support
Support for tone() is provided on all parts using TCB0. This is like the standard tone() function on the Arduino megaavr board package. it does not support use of the hardware output compare to generate tones like tone on some parts does. (Note that if TCB0 is used for millis/micros timing - which is not a recommended configuration -  tone() will instead use TCB1). Prior to the release of 1.3.3, there were a wide variety of bugs impacting the tone() function, particularly where the third argument, duration, was used; it could leave the pin HIGH after it finished, invalid pins and frequencies were handled with obvously unintended behavior, and integer math overflows could produce weird results with larger values of duration at high frequencies, and thee-argumwent tone() ddn't work at all above 32767Hz (even though maximum supported frequency is 65535 Hz) As all of the DxCore parts have at least 16k of flash so we enable `SUPPORT_LONG_TONES` unless it is explicitly disabled (this is not exposed in the UI), A "long" tone is one where `(frequency * duration) < 4.2 billion` but `(frequency * duration)/500 > 4.294 billion`(we rearrange that to `(frequency/5) * (duration/100)` when duration > 65535).

### millis/micros Timekeeping Options
DxCore provides the option to us any available timer on a part for the millis()/micros timekeeping, controlled by a Tools submenu - except TCD0 - or it can be disabled entirely to save flash (or more likely available timers for manual configuration) and allow use of all timer interrupts. By default, TCB2 will be used by all parts. TCA0, TCA1 and any of the TCB's present on the part may be used, though this has not been rigorously tested. TCD0 os presemnt

For more information, on the hardware timers of the supported parts, and how they are used by DxCore's built-in functionality, see the [Timers and DxCore](megaavr/extras/PWMandTimers.md)

### ADC Support
These parts all have a large number of analog inputs - DA and DB-series have up to 24 analog inputs, while the DD-series has analog input on every pin that is not used to drive the HF crystal (though the pins on PORTC are only supported when MVIO is turned off) - plus the one on the UPDI pin which is not totally usable because the UPDI functionality). They can be read with analogRead() like on a normal AVR. While the `An` constants (ex, `A3`) are supported, and will read from the ADC channel of that number, they are in fact defined as the digital pin shared with that channel. Using the `An` constants is deprecated - the recommended practice is to just use the digital pin number, or better yet, use `PIN_Pxn` notation when calling analogRead(). Particularly since the release of 1.3.0 and megaTinyCore 2.3.0, a number of enhanced ADC features have been added to expose more of the power of the sophisticated ADC in these parts to users

### Reference Voltages
Analog reference voltage can be selected as usual using analogReference(). Supported reference voltages are listed below:

 | AVR Dx-series (all)                     |
 |-----------------------------------------|
 | `VDD` (Vcc/supply voltage - default)    |
 | `INTERNAL1V024`                         |
 | `INTERNAL2V048`                         |
 | `INTERNAL4V096`                         |
 | `INTERNAL2V500`                         |
 | `INTERNAL4V1` (alias of INTERNAL4V096)  |
 | `EXTERNAL`                              |
 |                                         |


### Internal Sources
In addition to reading from pins, you can read from a number of internal sources - this is done just like reading a pin, except the constant listed in the table below is used instead of the pin number:
| DA-series                              | DB and DD-series                    |
|----------------------------------------|-------------------------------------|
| `ADC_GROUND` (Offset calibration?)     | `ADC_VDDDIV10`                      |
| `ADC_TEMPERATURE`                      | `ADC_VDDIO2DIV10`                   |
| `ADC_DAC0`                             | `ADC_GROUND` (offset calibration?)  |
| `ADC_DACREF0`                          | `ADC_TEMPERATURE `                  |
| `ADC_DACREF1`                          | `ADC_DAC0`                          |
| `ADC_DACREF2`                          | `ADC_DACREF0`                       |
|                                        | `ADC_DACREF1`                       |
|                                        | `ADC_DACREF2`                       |

DACREF0-2 are the the reference voltages for the analog comparators. You must use the DAC or DACREF sources to measure the supply voltage, as there is neither a way to measure Vdd directly nor the reference voltages before they go through the DACREF or DAC.


### Analog Resolution
The hardware supports increasing the resolution of analogRead() to the limit of the hardware's native resolution (10 or 12 bits); Additionally, we provide automatic oversampling and decimation up to the limit of what can be gathered in one batch using the accumulation feature; this is exposed through the analogReadEnh() function detailed below.

### ADC Sampling Speed
DxCore takes advantage of the improvements in the ADC on the newer AVR parts to improve the speed of analogRead() by more than a factor of three over classic AVRs! The ADC clock which was - on the classic AVRs - constrained to the range 50-200kHz - can be cranked up as high as 1.5MHz at full resolution! We use use 1.2 to 1.33 MHz depending on the system clock. To compensate for the faster ADC clock, we set the sample duration to 14 to extend the sampling period from the default of 2 ADC clock cycles to 16 - providing approximately the same sampling period as on classic AVRs (which is probably a conservative decision).

### Functions:
This core includes the following analog reading related functions

#### analogRead(pin)
The standard analogRead(). Single-ended, and resolution set by analogReadResolution(), default 10. Negative return values indicate an error that we were not able to detect at compile time and issue a compile error for.

#### analogReadResolution(resolution)
Sets resolution for the analogRead() function. Unlike stock version, this returns true/false. If it returns false, the value passed was invalid, and resolution was set to the default, 10 bits. The only valid values are those that are supported natively by the hardware, plus 10 bit even if not natively supported, for compatibility.
Hence, the only valid values are 8, 10 and - on the 2-series - 12.
This is different from the Zero/Due/etc implementations, which allow you to pass anything from 1-32, and rightshift the minimum resolution or leftshift the maximum resolution as needed to get the requested range of values. Within the limited resourced of an 8-bit AVR with potentially only 4k of flash, this is a silly waste of resources; furthermore, padding a reading with zeros so it matches the format of a more precise measuring method strikes me as generally poor practice. Note also that we offer oversampling & decimation with `analogReadEnh()` and `analogReadDiff()`, which can extend the resolution keep the extra bits meaningful. `analogReadResolution()` only controls the resolution of analogRead() itself. The other functions take desired resolution as an argument.

#### analogSampleDuration(duration)
Sets sampling duration (SAMPLEN on 0/1 or SAMPDUR on 2-series) For high impedance voltage sources, use a longer sample length. For faster reads, use a shorter sample length. Passing 0 will approximately double analogRead()
speed. This returns a `bool` - it is `true` if value is valid; on 0/1-series maximum is 31 and the duration of a sample is SAMPLEN + 2 (max. 33 ADC clock sampling duration); on 2-series any 8-bit value is valid, and duration is this many plus 0.5 (without PGA) or 1 (with PGA) ADC clocks. This value is used for all analog measurement functions.

The megaTinyCore default is 14 for 0/1-series and 15 for the 2-series - for 16 or 15.5 ADC clocks (which is half as long on the 2-series, since it supports higher ADC clock speeds. ADC clock is configured 1-1.25 MHz on 0/1, and approx. 2.5 MHz on 2-series.

Reducing it to the minimum will approximately cut the time analogRead() takes in half, but will have poor accuracy unless measuring very low impedance signals;

#### analogReadEnh(pin, res=ADC_NATIVE_RESOLUTION, gain=0)
Enhanced `analogRead()` - returns a `long` (`int32_t`), not an `int` (`int16_t`). Perform a single-ended read on the specified pin. `res` is resolution in bits, which may range from 8 to `ADC_MAX_OVERSAMPLED_RESOLUTION`. This maximum is 13 bits for 0/1-series parts, and 17 bits for 2-series parts. If this is less than the native ADC resolution, that resolution is used, and then it is right-shifted 1, 2, or 3 times; if it is more than the native resolution, the accumulation option which will take 4<sup>n</sup> samples (where `n` is `res` native resolution). Note that even with the faster ADC in the 2-series, and even without extending the sampling duration, a 1024 sample burst read is not instantaneous - with defaults it's around 12 ms for 1024-sample burst read, and at maximum sample duration it will take over 100ms. The accumulated result is then decimated (rightshifted n places) to yield a result with the requested resolution, which is returned. See Atmel app note AVR121 (https://ww1.microchip.com/downloads/en/appnotes/doc8003.pdf - the specific case of the 2-series is discussed in DS40002200 from Microchip, but it is a far less informative document).  Alternately, to get the raw accumulated ADC readings, pass one of the `ADC_ACC_n` constants for the second argument where `n` is a power of 2 up to 64 (0/1-series), 128 (Dx-series), or 1024 (2-series). Be aware, however, that the lowest bits of a raw accumulated reading should not be trusted (which is why the decimation step is needed, and why 4x the samples are required for every extra bit of resolution instead of 2x). On 2-series parts *the PGA can be used for single ended measurements*. Valid options for gain are 0 (PGA disabled, default), 1 (unity gain - may help in some circumstances - but that is the extent of my knowledge on that matter, and powers of 2 up to 16) - on 0/1-series parts, the gain argument should be omitted or 0.
```c++
  int32_t second_pin_voltage = analogReadEnh(PIN_PA2,13);
  // measure voltage on PA2, to 13 bits of resolution, which will require either 3 or 1 bits of oversampling + decimation
  // (depending on whether it is a 0/1 or 2-series part.)
```


#### analogReadDiff(positive, negative, res=ADC_NATIVE_RESOLUTION, gain=0) *2-series only*
Differential `analogRead()` - returns a `long` (`int32_t`), not an `int` (`int16_t`). Performs a differential read using the specified pins as the positive and negative inputs. Any analog input pin can be used for the positive side, but only pins on PORTA (tinyAVR 2-series) or PORTD/PORTE (AVR Dx-series) can be used as the negative input. The result returned is the voltage on the positive side, minus the voltage on the negative side, measured against the selected analog reference. The `res` parameter works the same way as for `analogReadEnh()`, as does the `gain` function. Gain becomes FAR more useful here than in single-ended mode as you can now take a very small difference and "magnify" it to make it easier to measure.

```c++
  int32_t diff_first_second = analogReadDiff(PIN_PA1, PIN_PA2, 12, 0);
  // Simple differential read to with PA1 as positive and PA2 as negative. Presumable these are close in voltage.
  // (I used two pots and manually adjusted them to be very close; also could have done like 10k-100ohm-10k resistor network)
```

**ERRATA ALERT** There is a mildly annoying silicon bug in early revisions of the AVR DA parts (as of a year post-release in 2021, these are still the only ones available) where whatever pin the ADC positive multiplexer is pointed at, digital reads are disabled. This core neatly works around it by setting the the ADC multiplexer to point at ADC_GROUND when it is not actively in use; however, be aware that you cannot, say, set an interrupt on a pin being subject to analogReads (not that this is particularly useful).

### DAC Support
The DA-series parts have a 10-bit DAC which can generate a real analog voltage (note that this provides low current and can only be used as a voltage reference, it cannot be used to power other devices). This generates voltages between 0 and the selected VREF (unlike the tinyAVR 1-series, this can be Vcc!). Set the DAC reference voltage via the DACReference() function - pass it any of the ADC reference options listed under the ADC section above (including VDD!). Call `analogWrite()` on the DAC pin (PD6) to set the voltage to be output by the DAC (this uses it in 8-bit mode). To turn off the DAC output, call `digitalWrite()` or `turnOffPWM()` on that pin.

To use it in 10-bit mode
```
//assumes dacvalue is an unsigned 16-bit integer containing a number between 0 and 1023.

// enable DAC
DAC0.CTRLA |= DAC_OUTEN_bm | DAC_ENABLE_bm;

// write value to DAC
DAC0.DATA=(dacvalue<<6);

// disable DAC
DAC0.CTRLA &= ~(DAC_OUTEN_bm | DAC_ENABLE_bm);
```

#### DAC Error
For rather silly reasons, I wound up taking a bunch of measurements with a millivolt meter hooked up to the DAC. At least on the chip I tested, at room temperature and Vcc = approx. 5v, both the 4.096 and 1.024V references were within 1%, and the voltages coming out of the DAC were a few mV low below around the half-way point. It looked to me like, feeding it 8-bit values, if your "dac writing" function secretly set the low 2-bits of DATA a value dependent on the 8 high-bits and the current reference voltage, you could get a significant improvement in accuracy of the output. Something like this would do well - at least on the chip I was looking at. I have too many other tasks to pursue further - but someone who likes analog stuff could have a lot of fun with this, for certain values of fun...
```c++
void correctedDACWrite(uint8_t value) {
  // assumes DAC already enabled and outputting, just changing output value
  // elsewhere, pretend that it's not doing any of this.
  byte datal=0;
  byte dacref = VREFA.DAC0REF &0x07;
  if (dacref == 2) { // 4.096
    if (value < 128) {
      datal=1;
    }
  } else if (dacref == 2)  {// 1.024
    if (value < 112) {
      datal = min(value,3);
    } else if (value < 128) {
      datal = 2;
    } else if (value < 144) {
      datal = 1;
    }
  } else if (dacref = 1) { // 2.048
    /* I didn't  play with this reference voltage... */
  } else if (dacref = 3) { // 2.500
    /* or this one */
  }
  DAC0.DATAL=(datal << 6); // needs to be left-adjusted,
  DAC0.DATAH=value;
}

```

### Servo Support
This core provides a version of the Servo library. This version of Servo always uses TCB0. If millis/micros is set to use TCB1 on those parts, servo will use TCB0 instead, making it incompatible with tone there as well). Servo output is better the higher the clock speed - when using servos, it is recommended to run at the highest frequency permitted by the operating voltage to minimize jitter.

**Warning** If you have installed a version of the Servo library to your <sketchbook>/libraries folder (including via library manager), the IDE will use that version of the library (which is not compatible with these parts) instead of the one supplied with megaTinyCore. As a workaround, a duplicate of the Servo library is included with a different name - to use it, `#include <Servo_DACore.h>` instead of `#include <Servo.h>`

### printf() support for "printable" class
Unlike the official board packages, but like many third party board packages, megaTinyCore includes the .printf() method for the printable class (used for Serial and many other libraries that have print() methods); this works like printf(), except that it outputs to the device in question; for example:
```cpp
Serial.printf("Milliseconds since start: %ld\n", millis());
```
Note that using this method will pull in just as much bloat as sprintf(), so it may be unsuitable on devices with small flash memory.

### Pin Interrupts
All pins can be used with attachInterrupt() and detachInterrupt(), on `RISING`, `FALLING`, `CHANGE`, or `LOW`. All pins can wake the chip from sleep on `CHANGE` or `LOW`. Pins marked as ASync Interrupt pins on the pinout chart (this is marked by an arrow where they meet the chip on those charts - pins 2 and 6 on all ports have this feature) can be used to wake from sleep on `RISING` and `FALLING` edge as well. The Async Interrupt pins can also react to inputs shorter than one clock cycle (how *much* faster was not specified. )

Advanced users are advised to instead set up interrupts manually, ignoring attachInterrupt and manipulating the relevant port registers appropriately and defining the ISR with the ISR() macro - this will produce smaller code (using less flash and ram) the ISRs will run faster as they don't have to check whether an interrupt is enabled for every pin on the port. Note that, currently, if attachInterrupt() is used ANYWHERE IN THE SKETCH, even within a library, a duplicate vector definition error will be thrown if you attempt to manually configure a pin interrupt. There are plans to address this in a future version (the solution is to put the interrupts themselves into different C source files for each, like how the UARTs are done, such that ) For full information and example, see: [Pin Interrupts](megaavr/extras/PinInterrupts.md)

### Improved digital I/O

#### openDrain()
It has always been possible to get the benefit of an Open Drain configuration - you set a pin's output value to 0 and toggle it between input and output. This core provides a slightly smoother (also faster) wrapper around this thanusing pinmode (since pinMode must also concern itself with configuring the pullup, whether it needs to be changed or not, every time - it's actually significantly slower setting things input vs output. The openDrain() function takes a pin and a value - `LOW`, `FLOATING` (or `HIGH`) or `CHANGE`. openDrain always makes sure the output buffer is not set to drive the pin high; often the other end of a pin you[re using in open drain mode may be connected to something running from a lower supply voltage, where setting it OUTPUT with the pin set high could damage the other device. openDrainFast() is also provided; as of 1.3.6, it also writes the pin LOW prior to setting it output or toggling it with change. `CHANGE` is slightly slower and takes an extra 2 words of flash because the VPORT register only has set bit index and clear bit index, so we have to use a full STS instruction to write to the `PORT.DIRTGL` register. Use pinMode() to set the pin `INPUT_PULLUP` before you start using `openDrain()`, or use `pinConfigure()` if you need the pullup enabled too'; this donesn't touch it. We do not prevent you from setting a pin with a pullup on it as output and driving it low with this (or in general). That is non-destructive (it's not like, say bridging two pins and driving one high and the other low) but it doesn't exactly help with power consumption.

```c++
openDrain(PIN_PA1, LOW); // sets pin output, LOW.
openDrain(PIN_PA1, FLOATING); // sets pin input and lets it float.
```

#### Fast Digital I/O
This core includes the Fast Digital I/O functions, digitalReadFast(), digitalWriteFast() and openDrainFast(). These are always inlined, but still take up less flash than the normal version of the function, and execute the in a single clock cycle. The catch is that the pin MUST be a known constant at compile time. For the fastest, least flash-hungry results, you should use a compile-time known constant for the pin value as well. Remember than there are three options here, not just two. If you only want two of the options, you will get smaller binaries that run faster by using the ternary operator to explicitly tell the compiler that the value is only ever going to be those two values, and then it can optimize away the third case.

```c++
digitalWriteFast(PIN_PD0,val); // This one is slower than the one below:
digitalWriteFast(PIN_PD0,val?HIGH:LOW)

```
| function            | Any value | HIGH/LOW |   fixed |
|---------------------|-----------|----------|---------|
| openDrainFast()     | 14 words  | 7 words  | 2 words if LOW<br/>1 if FLOATING |
| digitalWriteFast()  | 10 words  | 6 words  | 1 words |

Execution time is 1 or 2 clocks per word that is actually executed (not all of them are in the multiple possibility options. in the case of the "any option" digitalWrite, if it's LOW, if change, 6, and if HIGH, 7
Note that the HIGH/LOW numbers include the overhead of a val?HIGH:LOW statement. That is how the numbers were generated - you can use a variable of volatile uint8_t and that will prevent the compiler from making; 3 and 5 for the two word case. Which highlights one problem: the execution times now depend on the values, qand rather strongly. Another reason to, as much as possible, to get your assumptions about it's value - even if you never set it-  so you can see how large the the binary is while keeping the test case as simple as possible.

1 word is 2 bytes; when openDrainFast is not setting the pin FLOATING, it needs an extra word to set the output to low level as well (just in case); if CHANGE is an option, it also uses an extra 2 words because instead of a single instruction against VPORT.IN, it needs to load the pin bit mask into a register (1 word) and use STS to write it (2 words) - and it also does the clearing of the output value - hence how we end up with the penalty of 4 for the unrestricted case vs digitalWriteFast.


#### pinConfigure()
pinConfigure is a somewhat ugly function to expose all options that can be configured on a per-pin basis. It is called as shown here; you can bitwise-or any number of the constants shown in the table below. All of those pin options will be configured as specified. Pin functions that don't have a corresponding option OR'ed into the second argument will not be changed.  There are very few guard rails here: This function will happily enable pin interrupts that you don't have a handler for (but when they fire it'll crash the sketch), or waste power with pins driven low while connected to the pullup and so on.

```c++
pinConfigure(PIN_PA4,(PIN_DIR_INPUT | PIN_PULLUP_ON | PIN_OUT_LOW | PIN_INLVL_TTL));
// Set PIN_PA4 to input, with pullup enabled and output value of LOW (ready for openDrainFast() and using TTL logic levels. Do not change settings on invert or input sense.
// This might be used for some sort of bi-directional open-drain communication protocol with a device operating at lower voltage.

pinConfigure(PIN_PD4,(PIN_DIR_INPUT | PIN_OUT_LOW | PIN_PULLUP_OFF | PIN_INVERT_OFF | PIN_INLVL_SCHMITT | PIN_INPUT_ENABLE));
// Set PD4 inpit, with output register set low, pullup, invert, and alternate levels off, and digital input enabled. Ie, the reset condition!

```


| Functionality |   Enable  | Disable            | Toggle |
|---------------|-------|---------------------|--------------------|
| Direction, pinMode() | `PIN_DIR_OUTPUT`<br/>`PIN_DIR_OUT`<br/>`PIN_DIRSET` | `PIN_DIR_INPUT`<br/>`PIN_DIR_IN`<br/>`PIN_DIRCLR`       | `PIN_DIR_TOGGLE`<br/>`PIN_DIRTGL` |
| Pin output, HIGH or LOW | `PIN_OUT_HIGH`<br/>`PIN_OUTSET`         | `PIN_OUT_LOW`<br/>`PIN_OUTCLR`          | `PIN_OUT_TOGGLE`<br/>`PIN_OUTTGL`       |
| Internal Pullup  | `PIN_PULLUP_ON`<br/>`PIN_PULLUP`        | `PIN_PULLUP_OFF`<br/>`PIN_NOPULLUP`       | `PIN_PULLUP_TGL`       |
| Invert HIGH and LOW |`PIN_INVERT_ON`        | `PIN_INVERT_OFF`       | `PIN_INVERT_TGL`       |
| Use TTL levels (DB/DD only) | `PIN_INLVL_TTL`<br/>`PIN_INLVL_ON`        | `PIN_INLVL_SCHMITT`<br/>`PIN_INLVL_OFF`     | Not supported<br/>No plausible use case      |
| Digital input buffer | `PIN_INPUT_ENABLE`     | `PIN_INPUT_DISABLE`    | Not supported<br/>No plausible use case |
| Interrupt on change | `PIN_INT_CHANGE`       | `PIN_INPUT_ENABLE` or<br/>`PIN_INPUT_DISABLE`     | Not applicable |
| Interrupt on Rise  | `PIN_INT_RISE`         | `PIN_INPUT_ENABLE` or<br/>`PIN_INPUT_DISABLE`     | Not applicable |
| Interrupt on Fall  | `PIN_INT_FALL`         | `PIN_INPUT_ENABLE` or<br/>`PIN_INPUT_DISABLE`      | Not applicable |
| Interrupt on LOW  | `PIN_INT_LEVEL`        | `PIN_INPUT_ENABLE` or<br/>`PIN_INPUT_DISABLE`      | Not applicable |

#### INLVL - input logic levels
On MVIO parts, pins can be configured to use one of two sets of input voltages: either the normal schmnitt



### Assembler Listing generation
Like my other cores, Sketch -> Export compiled binary will generate an assembly listing in the sketch folder. Starting in 1.3.3, a memory map is also created.

### EESAVE configuration option
The EESAVE fuse can be controlled via the Tools -> Save EEPROM menu. If this is set to "EEPROM retained", when the board is erased during programming, the EEPROM will not be erased. If this is set to "EEPROM not retained", uploading a new sketch will clear out the EEPROM memory. Note that this only applies when programming via UPDI - programming through the bootloader never touches the EEPROM. You must do Burn Bootloader to apply this setting.

### BOD configuration options
These parts support multiple BOD trigger levels, with Disabled, Active, and Sampled operation options for when the chip is in Active and Sleep modes - Disabled uses no power, Active uses the most, and Sampled is in the middle. See the datasheet for details on power consumption and the meaning of these options. You must do Burn Bootloader to apply this setting, as this is not a "safe" setting: If it is set to a voltage higher than the voltage the board is running at, the chip cannot be reprogrammed until you apply a high enough voltage to exceed the BOD threshold.

### Link-time Optimization (LTO) support
This core *always* uses Link Time Optimization to reduce flash usage.

### Identifying menu options and part identity within sketch or library
It is often useful to identify what options are selected on the menus from within the sketch - mainly the millis timer selection.



##### Millis timer
The option used for the millis/micros timekeeping is given by a define of the form USE_MILLIS_TIMERxx. Possible options are:
* MILLIS_USE_TIMERA0
* MILLIS_USE_TIMERA1
* MILLIS_USE_TIMERB0
* MILLIS_USE_TIMERB1
* MILLIS_USE_TIMERB2
* MILLIS_USE_TIMERB3 (48 and 64-pin parts only)
* MILLIS_USE_TIMERB4 (64-pin parts only)
* MILLIS_USE_TIMERD0 (not supported on DA/DB series - there's no shortage of TCBs here). May be supported for DD due to their only having 2 type B timers on the 14/20 pin parts.
* DISABLE_MILLIS

##### Using to check that correct menu option is selected
If your sketch requires that the B0 is used as the millis timer, for example:

```
#ifndef MILLIS_USE_TIMERB2
#error "This sketch is written for use with TCB2 as the millis timing source"
#endif
```

### Identifying part family within sketch
When writing code that may be compiled for a variety of target chips, it is often useful to detect which chip it is running on. Defines of the form `__AVR_AVRxDAy__` are provided, where `x` is the size of the flash (in kb) and `y` is the number of pins, for example `__AVR_AVR128DA64__` for the part with 128K flash and 64 pins.

This core provides an additional define depending on the number of pins on the part and it's family (which identifies it's peripheral selection) and the DxCore version:
* `DA_28_PINS`
* `DA_32_PINS`
* `DA_48_PINS`
* `DA_64_PINS`
* `DB_28_PINS`
* `DB_32_PINS`
* `DB_48_PINS`
* `DB_64_PINS`
* `DD_14_PINS`
* `DD_20_PINS`
* `DD_28_PINS`
* `DD_32_PINS`
* `DX_14_PINS`
* `DX_20_PINS`
* `DX_28_PINS`
* `DX_32_PINS`
* `DX_48_PINS`
* `DX_64_PINS`
* `DX_28_PINS`
* `__AVR_DA__`
* `__AVR_DB__`
* `__AVR_DD__`
* `DXCORE` - DxCore version number, as string.
* `DXCORE_MAJOR` - DxCore major version
* `DXCORE_MINOR` - DxCore minor version
* `DXCORE_PATCH` - DxCore patch version
* `DXCORE_RELEASED` - 1 if a released version, 0 if unreleased (ie, installed from github between releases).
* `DXCORE_NUM` - DxCore version, as unsigned long.

```


```
Will produce output like:
```
1.3.4
1 3 4 1
01030401
```
The various numeric representations of version are of interest to those writing libraries or code meant to be run by others. They are meant to ensure that there is always an easy way to test against specific versions (for when a release contains a regression), as well as an "X or better" type test.


### __AVR_ARCH__
This can be set to 102, 103, or 104 depending on flash size:
* `__AVR_ARCH__ == 103` - All parts where all of the flash is mapped in the data space. This means Dx-series parts with 32k or less of flash, tinyAVR 0/1/2-series, and megaAVR 0-series.
* `__AVR_ARCH__ == 104` - Parts with 128Kb of flash, mapped flash is split into 4 sections (AVR128DA, AVR128DB).
* `__AVR_ARCH__ == 102` - Parts with 64Kb of flash, mapped flash is split into 2 sections (AVR64DA, AVR64DB).

### Core feature detection
There are a number of macros for determining what (if any) features the core supports (these are shared with megaTinyCore)
* `CORE_HAS_FASTIO` (1) - If defined as 1 or higher. indicates that digitalWriteFast() and digitalReadFast() is available. If undefined or defined as 0, these are not available. If other "fast" versions are implemented, they would be signified by this being defined as a higher number. If defined as -1, there are no digital____Fast() functions, but with constant pins, these functions are optimized aggressively for speed and flash usage (though not all the way down to 1 instruction).
* `CORE_HAS_OPENDRAIN ` (1) - If defined as 1, indicates that openDrain() and (assuming `CORE_HAS_FASTIO` is >= 1) openDrainFast() are available. If undefined or 0, these are not available.
* `CORE_HAS_PINCONFIG ` (1) - If defined as Indicates that pinConfigure() is available. If not defined or defined as 0, it is not available.
* `CORE_HAS_TIMER_TAKEOVER` (1) - if defined as 1, takeOverTCxn() functions are available to let user code take full control of TCA0, TCA1 and/or TCD0.
* `CORE_HAS_TIMER_RESUME` (1)- if defined as 1, the corresponding resumeTCxn() functions, which reinitialize them and return them to their normal core-integrated functions, are available.
* `ADC_NATIVE_RESOLUTION` (12)- This is the maximum resolution, in bits, of the ADC without using oversampling.
* `ADC_NATIVE_RESOLUTION_LOW` (10) - The ADC has a resolution setting that chooses between ADC_NATIVE_RESOLUTION, and a lower resolution.
* `ADC_DIFFERENTIAL` (1) - This is defined as 1 if the part has a basic differential ADC (no gain, and V<sub>analog_in</sub> constrainted to between Gnd and V<sub>Ref</sub>), and 2 if it has a full-featured one. It does not indicate whether said differential capability is exposed by the core.
* `SUPPORT_LONG_TONES` (1)  - On some modern AVR cores, an intermediate value in the tone duration calculation can overflow (which is timed by counting times the pin is flipped) leading to a maximum duration of 4.294 million millisecond. This is worst at high frequencies, and can manifest at durations as short as 65 seconds worst case. Working around this, however, costs some flash, and some cores may make the choice to not address it (megaTinyCore only supports long tones on parts with more than 8k of flash).  If `SUPPORT_LONG_TONES` is defined as 1, as long as (duration * frequency)/500 < 4.294 billion, the duration will not be truncated. If it is defined as 0, the bug was known to the core maintainer and they chose not to fully correct it (eg, to save flash) but took the obvious step to reduce the impact, it will be truncated if (duration * frequency) exceeds 4.294 billion. If `SUPPORT_LONG_TONES` is not defined at all, the bug may be present in its original form, in which case the duration will be truncated if (duration * frequency) exceeds 2.14 billion.
* `CORE_HAS_ANALOG_ENH` (1)  - If defined as 1, analogReadEnh() (enhanced analogRead) is available. Otherwise, it is not.
* `CORE_HAS_ANALOG_DIFF` (1)  - If defined as 1, analogReadDiff() (differential enhanced analogRead) is available. Otherwise, it is not.  It has same features as enhanced, except that it takes a differential measurement.
* `ADC_MAX_OVERSAMPLED_RESOLUTION` (15) - If either `CORE_HAS_ANALOG_ENH` or `CORE_HAS_ANALOG_DIFF` is 1, this will be defined as the maximum resolution obtainable automatically via oversampling and decimation using those functions.
* `ADC_MAXIMUM_GAIN` - Some parts have an amplifier, often used for differential readings. The Dx-series are not among them. If this is defined as a positive number, it is the maximum gain available. If this is defined as -1, there are one or more `OPAMP` peripherals available which could be directed towards the same purpose, though more deliberation would be needed. If it is defined as -128 (which may come out as 128 if converted to an unsigned integer), there is a gain stage on the differential ADC, but it is a classic AVR, so the available gain options depend on which pins are being measured, and there is a different procedure as detailed in the core documentation (ex, ATTinyCore 2.0.0 and later). If it is 0 or undefined, there is no built-in analog gain state for the ADC, or it is not exposed through the core.




# Autoreset circuit
These parts can be used with the classic auto-reset circuit described below. It will reset the chip when serial connection is opened like on typical Arduino boards.
* 1 Small signal diode (specifics aren't important, as long as it's approximately a standard diode)
* 1 0.1uF Ceramic Capacitor
* 1 10k Resistor

Connect the DTR pin of the serial adapter to one side of a 0.1uF ceramic capacitor.
Connect the other side of the capacitor to the Reset pin.
Connect the 10k resistor between Reset and Vcc.
Connect the diode between Reset and Vcc with the band towards Vcc.

The breakout boards I sell on Tindie have the auto-reset circuit included. The RST_EN pads on the back are connected by default, but you may cut the small trace between them to disable it. It can be re-enabled by bridging with solder, like the other jumpers. Unlike the tinyAVR 0/1-series parts, UPDI is not shared with RESET, and the autoreset circuit does not effect UPDI programming


# Guides
Largely adapted from megaTinyCore
### [Power Saving techniques and Sleep](megaavr/extras/PowerSave.md)
### [Direct Port Manipulation](megaavr/extras/DirectPortManipulation.md)
### [Pin Interrupts](megaavr/extras/PinInterrupts.md)


# List of Tools sub-menus
* Tools -> Chip - sets the specific part within a selected family to compile for and upload to.
* Tools -> Clock Speed - sets the clock speed. You do not need to burn bootloader after changing this setting!
* Tools -> Retain EEPROM - determines whether to save EEPROM when uploading a new sketch. This option is not available on Optiboot board definitions - programming through the bootloader does not execute a chip erase function and never erases the bootloader.  ~You must burn bootloader after changing this to apply the changes~ As of 1.3.0, this setting is applied on all UPDI uploads without a "burn bootloader" cycle to AVR DA and AVR DB-series devices.
* Tools -> B.O.D. Voltage - If Brown Out Detection is enabled, when Vcc falls below this voltage, the chip will be held in reset. You must burn bootloader after changing this to apply the changes. Take care that these threshold voltages are not exact - they may vary by as much as +/- 0.3v! (depending on threshold level - see electrical characteristics section of datasheet). Be sure that you do not select a BOD threshold voltage that could be triggered during programming, as this can prevent successful programming via UPDI (reported in #86).
* Tools -> Reset pin - This menu option can be set to Reset (default) or Input; the latter allows this pin to be used as a normal input. As of 1.3.0, this setting is applied on all UPDI uploads without a "burn bootloader" cycle to AVR DA and AVR DB-series devices.
* Tools -> B.O.D. Mode (active) - Determines whether to enable Brown Out Detection when the chip is not sleeping. You must burn bootloader after changing this to apply the changes.
* Tools -> B.O.D. Mode (sleep) - Determines whether to enable Brown Out Detection when the chip is sleeping. You must burn bootloader after changing this to apply the changes.
* Tools -> millis()/micros() - If set to enable (default), millis(), micros() and pulseInLong() will be available. If set to disable, these will not be available, Serial methods which take a timeout as an argument will not have an accurate timeout (though the actual time will be proportional to the timeout supplied); delay will still work, though it's done using delayMicroseconds(), so interrupts are disabled for 1ms at a time during the delay, and any interrupts that happen during the delay will add to the length of the delay. Depending on the part, options to force millis/micros onto any type A or B timer on the chip are also available from this menu.
* ~Tools -> MVIO~ - As of 1.3.1, the MVIO option is removed from the Tools sub-menus. It is the default configuration, and nobody was able to describe a situation in which leaving it enabled in a correctly designed circuit would have detectable consequences. I was unable to come up with any cases where, in an incorrectly designed circuit, disabling MVIO would result in different behavior and the difference wasn't "if MVIO was enabled, the chip would continue to operate, if it was disabled, the chip would suffer an immediate hardware failure". Disabling MVIO on the chip installed in a circuit where it was intended to be using MVIO would also have that imnpact. Having nothing to gain by disabling it, that option was removed.


# Buying AVR DA-series breakout boards
I sell breakout boards with regulator, UPDI header, and Serial header and other basic supporting parts in my Tindie shop, as well as the bare boards. Buying from my store helps support further development on the core, and is a great way to get started using these exciting new parts with Arduino. Note that we do not currently sell a 28-pin version - this did not seem like a compelling part with the availability of the 32-pin version; the main appeal of the 28-pin part is that it is available in a through-hole version. As we would not be able to make the 28-pin version significantly smaller, there did not seem to be a compelling reason to create a 28-pin version. We may revisit this decision in the future, including potentially a 28-pin bare board for the through-hole version, suitable for assembly by those not experienced with drag soldering.
(The boards are not for sale yet at the time of release, as they need to be "approved" by Tindie, and it's a weekend...)

### [Assembled Boards](https://www.tindie.com/products/21007/)
### [Bare Boards](https://www.tindie.com/products/21008/)

# Warnings and Caveats

## Direct Register Manipulation
If you are manually manipulating registers controlling a peripheral, you should not count on the behavior of API functions that relate to the same peripheral, nor should you assume that calling said API functions will not adversely impact the rest of your application. For example, if you "take over" TCA0, you should not expect that using analogWrite() - except on the two pins on the 20/24-pin parts controlled by TCD0 - will work for generating PWM; indeed you should expect that it will break whatever you are doing with TCA0. In a few cases (such as TCD0 prescalers) exceptions to this rule are noted.

## Warning: I2C **requires** external pullup resistors
Earlier versions of megaTinyCore, and possibly very early versions of DxCore enabled the internal pullup resistors on the I2C pins. This is no longer done; they are miles away from being strong enough to meet the I2C specifications - it was decided that it is preferably for it to fail consistently without external ones than to work under simple conditions with the internal ones, yet fail under more demanding ones (more devices, longer wires, etc).

## Differences in Behavior between DxCore and Official Cores
While we generally make an effort to emulate the official Arduino core, there are a few cases that were investigated, but we came to the conclusion that the compatibility would not be worth the price. The following is a (hopefully nearly complete) list of these cases.

### Serial does not manipulate interrupt priority
The official core for the (similar) megaAVR 0-series parts, which this was based on, fiddles with the interrupt priority (bet you didn't know that!) in ways that are of dubious safety towards other code. megaTinyCore does not do this (in the process saving several hundred bytes of flash). Writing to Serial when it's buffer is full, or calling `Serial.flush()` while  with interrupts disabled, or during another ISR (which you really shouldn't do anyway) will behave as it does on classic AVRs, and simply block until there is either space in the serial buffer, or the flush is completed.

### digitalRead() does not turn off PWM
On official cores, and most third party ones, the digitalRead() function turns off PWM when called on a pin. This behavior is not documented by the Arduino reference. This interferes with certain optimizations - and moreover is logically inconsistent - a "read" operation should not change the thing it's called on. That's why it's called "read" and not "write". There does not seem to be a logically coherent reason for this - and it makes simple demonstrations of what PWM is non-trivial (imagine setting a pin to output PWM, and then looking at the output by repeatedly reading the pin). See also the above section on PWM uaing TCD0.

### digitalWrite()/pinMode() and INPUT pins
Like the official "megaavr" core, calling digitalWrite() on a pin currently set INPUT will enable or disable the pullups as appropriate. Additionally, as of 2.2.0, megaTinyCore fixes two bugs in this "classic emulation". On a classic core, digitalWrite() on an INPUT would also write to the port output register - thus, if one subsequently called pinMode(pin,OUTPUT), the pin would immediately output that level. This behavior is not emulated in the official core, and there is a considerable amount of code in the wild which depends on it. digitalWrite() now replicates that behavior. digitalWrite() also supports "CHANGE" as an option; on the official core, this will turn the pullup on, regardless of which state the pin was previously in, instead of toggling the state of it. The state of the pullup is now set to match the value that the port output register was just set to.

Similarly, using pinMode() to set a pin to INPUT or INPUT_PULLUP will now also set the port output register.

### analogWrite() and TCD0 pins
Please see the above PWM feature description if using PWM on those pins and also doing sensitive/niche work on the impacted pins (PIN_PA6 and PIN_PA7).

### TCA0/1 configured in Split Mode to get 3 additional PWM pins
On official "megaavr" board package, TCA0 is configured for "Single mode" as a three-channel 16-bit timer (used to output 8-bit PWM). DxCore always configures Type A timers for "split mode" to get additional PWM outputs. See the datasheets for more information on the capabilities of these peripherals. See [Taking over TCA0](megaavr/extras/TakingOverTCA0.md) for information on reconfiguring it.

### TCA0/1 TOP is 254, not 255
0 is a count, so at 255, there are 256 steps, and 255 of those will generate PWM output - but since Arduino defines 0 as always off and 255 as always on, there are only 254 possible values that it will use. The result of this is that (I don't remember which) either analogWrite(pin,254) results in it being LOW 2/256's of the time, or analogWrite(pin,1) results in it being HIGH 2/256's of the time. On megaTinyCore, with 255 steps, 254 of which generate PWM, the hardware is configured to match the API, and this does not occur; as it happens, 255 also (mathematically) works out such that integer math gets exact results for millis timing with both 16 MHz derived and 20 MHz derived clock speeds (relevant when TCA0 is used for millis() timing). I have not attempted this math for 24, 28, or 32 MHz system clock. The same thing is done for TCD0 (though to 509, giving 510 steps - analogWrite() accounts for this - so that we can get the same output frequency as an 8-bit timer would at the same unprescaled clock speed (higher frequencies) while keeping the fastest synchronization prescaler for fastest synchronization between TCD0 and system clock domains).

### digital I/O functions use old function signatures.
They return and expect uint8_t (byte) values, not enums like the official megaavr board package does. Like classic AVR cores, constants like `LOW`, `HIGH`, etc are simply #defined to appropriate values. The use of enums instead broke many common Arduino programming idioms and existing code, increased flash usage, lowered performance, and made optimization more challenging. While the enum implementation made language design purists comfortable, and provided error checking for newbies - because you couldn't pass anything  that wasn't a PinState to a digital I/O function, and would get that error checking if - as a newbie - you accidentally got careless. A compatibility layer was added to the official core - but then that got rid of what was probably the most compelling benefit, the fact that it did generate an error for new users to train them away from common Arduino practices like passing 1 or 0 to digitalWrite, `if(digitalRead(pin))` and the like. This change also had the perverse effect of making PinMode(pin,OUTPUT), an obvious typo of pinMode(pin,OUTPUT) into valid syntax (comma operator turns pin,OUTPUT into OUTPUT, and it returns a new PinMode of value OUTPUT...), instead of a syntax error - took me hours to find a simple typo! Anyway - the enums are not present here, and they never will be; this is the case with [MegaCoreX](https://github.com/MCUdude/MegaCoreX) and [DxCore](https://github.com/SpenceKonde/DxCore) as well.

### analogReadResolution() is different
Official AVR boards do not have analogReadResolution. Official ARM-based boards do, but the implementation is very different- they allow specifying any number from 1 to 32, and will shift the reading as required (padding it with zeros if a resolution higher than the hardware is capable of is specified). I dislike conceptually the idea of the core presenting values as if it has more information than it does, and in any event, on these resource constrained 8-bit microcontrollers, the code to create rounded or padded numbers with 1-32 bits is an extravagance we cannot afford, and the overhead of that should not be imposed on the vast majority of users who just want to read at the maximum resolution the hardware suopports, or 10 bits for code compatibility with classic AVRs. Since analogReadResolution() accepts a wide range of values on the official boards that have it, it does not need to report success or failure.

### As of 1.3.3, SerialEvent is removed
SerialEvent was an ill-conceived mess. I added support for it not knowing that the mess had already been deprecated; when I heard that it was, I wasted no time in fully removing it. if ENABLE_SERIAL_EVENT is de

# Instruction Set (AVRe/AVRe+ vs AVRxt)
The classic AVR devices all use the venerable `AVRe` (ATtiny) or `AVRe+` (ATmega) instruction set (`AVRe+` differs from `AVRe` in that it has hardware multiplication and supports devices with more than 64k of flash). Modern AVR devices (with the exception of ones with minuscule flash and memory, such as the ATtiny10, which use the reduced core `AVRrc`), these parts use the latest iteration of the AVR instruction set: `AVRxt`. This adds no new instructions (unlike `AVRxm`, the version used by the XMega devices, which adds ), but a small number of instructions have improved execution time, and one has slower execution time. This distinction is unimportant for 99.9% of users - but if you happen to be working with hand-tuned assembly (or are using a library that does so, and are wondering why the timing is messed up), the changes are:
* PUSH is 1 cycle vs 2 on classic AVR (POP is still 2)
* CBI and SBI are 1 cycle vs 2 on classic AVR (Sweeet!)
* LDS is 3 cycles vs 2 on classic AVR :disappointed: LD and LDD are still two cycle instructions.
* RCALL and ICALL are 2 cycles vs 3 on classic AVR
* CALL is 3 cycles instead of 4 on classic AVR
* (Saving the best for last) ST and STD is 1 cycle vs 2 on classic AVR! (STS is still 2)
The improvement to PUSH can make interrupts respond significantly faster (since they have to push the contents of registers onto the stack at the beginning of the ISR to make room for any that the ISR might need.), though the corresponding POP's at the end aren't any faster, and if your interrupt needs so many registers that it has to push half of the register file onto the stack, it's still gonna be slow (ex: Servo). It is likely that the improvements to the push-ing mechanism are responsible for the improvements to the call instructions (which push the PC value onto the stack so ret knows where execution should resume from.) The change with ST impacted tinyNeoPixel. Prior to my realizing this, the library worked on SK6812 LEDs (which happened to be what I tested with) at 16/20 MHz, but not real WS2812's. However, once I discovered this, I was able to leverage it to use a single tinyNeoPixel library instead of a different one for each port like was needed with ATTinyCore (for 8 MHz, they need to use the single cycle OUT on classic AVRs to meet timing requirements, the two cycle ST was just too slow; hence the port had to be known at compile time, or there must be one copy of the routine for each port, an extravagance that the ATtiny parts cannot afford. But with single cycle ST, that issue vanished)

## License
DxCore itself is released under the [LGPL 2.1](LICENSE.md). It may be used, modified, and distributed, and it may be used as part of an application which, itself, is not open source (though any modifications to these libraries must be released under the LGPL as well). Unlike LGPLv3, if this is used in a commercial product, you are not required to provide means for user to update it.

The DxCore hardware package (and by extension this repository) contains DxCore as well as libraries, bootloaders, and tools. These are released under the same license, *unless specified otherwise*. For example, tinyNeoPixel and tinyNeoPixel_Static, being based on Adafruit's library, are released under GPLv3, as described in the LICENSE.md in those subfolders and within the body of the library files themselves.

The pyupdi-style serial uploader in megaavr/tools is built on pymcuprog from Microchip, which ~is not open source~ *has now been released under the open source MIT license!*

Any third party tools or libraries installed on behalf of DxCore when installed via board manager (including but not limited to, for example, avr-gcc and avrdude) are covered by different licenses as described in their respective license files.
