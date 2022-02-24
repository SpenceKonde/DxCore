# DxCore - Arduino support for the AVR DA-series, DB-series and upcoming DD-series
**Please be on watch for typos in the docs and report them. Codespell is not a very good spellchecker. (Obviously, PR's to fix are even better!)**
**1.4.0 is in with some HUGE changes - new Wire! Improved Serial! Tons more stuff!**

## HEY HEY WE GOT AVR64DD PRELIMINARY DATASHEETS
[Read them they are *yawn* well, pretty much what everyone expected](https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ProductDocuments/DataSheets/AVR64DD32-28-Prelim-DataSheet-DS40002315A.pdf)
**Anyone able to find anything novel in there?** (aside from the fact that the HV override to turn UPDI back into UPDI goes on reset, which was already pretty obvious).
Reset is disabled by default (but this poses no challenges to working with the parts), and makes sense on the low pincount ones. I guess at that point, they'd do it for the whole line, rather than give the "small" ones (for which we don't yet have a datasheet) disabled reset as the default and the "large" ones enabled reset.
I mean, this is pretty much how it was supposed to go down - no surprises in the datasheet, landing pad ready for them in the core, any differences are trivial and nothing to get worked up about, and we all sit here twiddling our thumbs waiting for the 14-pin and 20-pins ones which are what everyone wants. If there were big surprises there, I wouldn't be happy. Even if they were good surprises.

## What is DxCore
This is an Arduino core to support the exciting new AVR DA, DB, and "coming soon" DD-series microcontrollers from Microchip. These are the latest and highest spec 8-bit AVR microcontrollers from Microchip. It's unclear whether these had been planned to be the "1-series" counterpart to the megaAVR 0-series, or whether such a thing was never planned and these are simply the successor to the megaAVR series. But whatever the story of their origin, these take the AVR architecture to a whole new level.  With up to 128k flash, 16k SRAM, 55 I/O pins, 6 UART ports, 2 SPI and I2C ports, and all the exciting features of the tinyAVR 1-series and megaAVR 0-series parts like the event system, type A/B/D timers, and enhanced pin interrupts... Yet for each of these systems they've added at least one small but significant improvement of some sort (while largely preserving backwards compatibility - the tinyAVR 2-series also typically adds the new features that the Dx-series gt , giving the impression that these reflect a "new version" of . You like the type A timer, but felt constrained by having only one prescaler at a time? Well now you have two of them (on 48-pin parts and up)! You wished you could make a type B timer count events? You can do that now! (this addresses something I always thought was a glaring deficiency of the new peripherals and event system). We still don't have more prescale options (other than having two TCA's to choose from) for the TCB - but you can now combine two TCB's into one, and use it to do 32-bit input capture. Time a pulse or other event up to approximately 180 seconds long... to an accuracy of 24th's of a microsecond! And of course, like all post-2016 AVR devices, these use the latest incarnation of the AVR instruction set, AVRxt, with slightly-improved instruction timing compared to "classic" AVRs

## Big Picture
These parts depart from the naming scheme used for AVR devices in the past; these are named AVR followed by the size of the flash, in KB, followed by DA, DB, or DD (depending on the "series" or "family", then the number of pins. Note that the pin count also determines how many of certain peripherals the parts have available - parts with more pins have more peripherals to do things with those pins. 64-pin parts are not available in 32k flash size. The 128k flash size is the highest that can be supported with a 16-bit program counter (above that, a number of instructions become slower, and everything gets more complicated), and with the current scheme for interacting with the pins, the 55 I/O pins (56 less the UPDI pin which takes the place of PF7) are the limit of what a modern AVR can accommodate while allowing single cycle access to all pins - so these take them to the top end of what is possible without extensions to the architecture.

At present, it appears that there will be at least four lines of AVR Dx-series parts: The "basic" DA, the DB with improved featureset, the as-yet unreleased low pincount DD-series and potentially a new "DU" series with DD-like low pincounts, which instead of `MVIO`, sacrifice all of `PORTC` in the name of **native USB**. A product brief was available, but only, as the name suggests, briefly, which implies that the retracted product brief may not reflect what will actually come to exist (or maybe management told them to get back to work and get the DD out before they they're allowed to start hyping the product that would come after it) - There's reason to suspect it was marketing/timing based, rather than due to factual inaccuracies in the product brief, since the EA-series product brief lists 2 USARTs for all pincounts, while the pinout chart clearly shows a USART2 alongside USARTs 0 and 1 on the 48-pin version... and has been like that for over a year now. *shrug*. As long as there is a native USB modern AVR in the not-too-distant future, I'll be happy with any plausible configuration. The prospect of a new, more capable AVR with native USB is exciting indeed, since the 32u4 was getting a little long in the tooth. This core supports the DA and DB-series parts, and will support the DD-series once those parts become available and the EA-series unless the scale of changes there is greater than expected and a separate core is required.. If/when The DU-series materializes, they would also fall under the perview of this core.

### DA-series
The "basic" full-size line - however much I was in awe of these when they were first released, having seen the DB-series, it now appears that these are more akin to a 0-series than a 1-series - in every arena, the DB has the same or better. They do not support using an external crystal for the main clock, like the other Dx parts do, but the internal oscillator on these parts is still WAY better than the classic AVRs had - all the ones I've tested are weithin half a percent at room temp and typical operating voltages, even without autotune... To make sure autotune was working, I had to point a torch at it, because I couldn't get enough of a change in the internal oscillator frequency from changing the supply voltage. It is also the only currently announced Dx series without `MVIO`. While they may not shine as brightly next to the other Dx lines. The fact that these look less than stellar beside the DB doesn't change the fact that they absolutely bury any AVR from before 2020. There is only one thing that they have and the DB doesn't - a peripheral touch controller (which we can't use in Arduino land because they won't share the source code and force you to use their IDE.)

### DB-series
The DB-series is almost an exact copy of the DA-series (they fixed some of the most egregious silicon bugs, but only some), only with a few even more exciting features tacked on: Support for a real high-frequency crystal as clock source (finally - first time since the modern AVR architecture was released in 2016), "MVIO" (multivoltage I/O), where PORTC can run at a different voltage than the rest of the chip (essentially, a builtin bidirectional level shifter). The other "headline feature", is the  two (28/32-pin parts) or three (48/64-pin parts) on-chip opamps, with software-controlled MUX for the inputs and an on-chip feedback resistor ladder. These can be used as gain stage for the ADC, for example, or to buffer the DAC output (though these opamps can't supply **that** much current, they can supply tens of mA, instead of ~ 1 like the unaided DAC), and connected to each other for multistage amplifiers (on parts with 3, you can even connect them together as an instrumentation amplifier). The included `<Opamp.h>` library provides a minimalist wrapper around them exposing all of the functionality.

### DD-series
The DD-series is a smaller-pincount line; parts will be available with 14-32 pins. They've got the `MVIO` (3 or 4 MVIO pins depending on pincount), and the UPDI pin can be configured as an I/O pin (in this case, further UPDI programming requires an HV pulse to be applied to the *reset* pin, while the reset pin, like the DA and DB parts, can be configured only as reset or input. (This sidesteps the awkward problem that tinyAVR has of "How do I put an HV pulse on the reset pin when it's an output being driven low?") One thing worth noting is that the 28-pin and 32-pin DD-series parts are, but for expanded port multiplexing options for `SPI0`, `USART0`, and `TCD0`, and the addition of ADC input functionality on pins in `PORTA` and `PORTC`, the same as or worse that the DB-series in every way. Unfortunately all indications are that we'll get the 28/32 pin parts before the 14/20 pin parts that we're all waiting for..

If you were hoping to use a tinyAVR breakout board for the 14 or 20-pin versions, I hate to be the bearer of bad news, but the tinyAVR boards won't work. They swapped power and ground on the SOIC packages vis-a-vis tinyAVR, and the layout of the VQFN20 is different (and it's 33% larger in size - 4mm x 4mm) than the 20-pin tinyAVRs. (The 28 and 32 pin parts are pin compatible with AVR-DB - but as I said, I don't think anyone is standing in line for those). Actually, no, I don't hate being the bearer of that news. To me, it's great news - I sell breakout boards, remember? I've already got boards on standby for the 14-pin parts, in fact....

### EA-series
The EA-series is known only from it's product brief. According to the product brief it will be available only in 28, 32, and 48 pin packages, with up to 64k of flash, no DAC or Type D timer, but in exchange, there will be dual type A timers in all pincounts, and it will feature the 12-bit true differential ADC with programmable gain amplifier that the tinyAVR 2-series has.
The most puzzling thing about these, though is that some of their specs like the oscillator seem to represent a step backwards. On the third hand, they will represent A NEW version of the NVM controller, being the first modern AVR to advertise NRWW and RWW sections of flash. As more information becomes available the determination of whether support for these partswill be through DxCore or a new core will be made, but unless there are substantial changes to other systems, we will likely keep it in DxCore.

### DU-series (details unconfirmed)
The DU-series was described by a product brief which was almost immediately pulled down. It described something with DD-like pincounts, MVIO replaced with USB, and no TCD. One imagines that under the hood, TCD is repurposed for the 48 MHz USB reference clock, and the MVIO is levelshifting the USB data lines to 3.3v. It feature crystalless USB, the same flash options as the DD (except no 64k 14/20 pin). It also suggested a builtin USB programming interface of some sort, which is of course very exciting news - though all USB stuff will depend on availability of reference code to write the USB modules. We look forward to the release of more information, and, evnetually, the product. I predict the DU14 and DU20 will be extremely popular.

## Timeline
128k DA-series parts were released in April 2020, followed by 32k in early summer (though the initial release of the 32k parts suffered from a fatal flaw and was recalled, working ones were not available until the end of 2020), while the 64k parts became available late in summer of 2020 without such brutal heat), around the same time as the first AVR128DB-series parts became available, while the 32 snd 64k versiond arrived in Q2 2021. The AVR DD-series product brief was released in spring of 2020, while headers for the 64k-flash version have been released, essentially confiming what was in the product brief with no serious surprises. The DD was expected to launch in the first half of 2021, but so far, there's been no further news. It's a similar story with the EA-series. Likely the current capacity shortfall is a big part of that. I like to imagine that they're also taking more care to get all the kinks worked out. Errata lists like the DA has are not cool.

## Lots of errata
The silicon errata list in the initial versions of these parts - the DA-series in particular - is... longer than you may be used to if you were accustomed to the classic AVRs. The initial DB silicon looked bad too, but right after the release, a new silicon rev was added to the errata listing which fixed most of the bad ones **AVR128DB** (all the parts I've gotten had the fixes, even the ones I got delivered before they'd released the datasheet....). Sadly, the AVR128DA never got a corresponding pack of fixes. In both cases, the smaller flash versions, released later, incorporated fixes that the 128k parts haven't gotten. If you've been working with the tinyAVR 0/1-series, many of these errata may be old friends of yours by now. A large number of those issues appear to have sailed through the new chip design process without a remedy - and in late 2020, new silicon bugs appeared in the tinyAVR 0/1 errata and DA/DB errata simultaneously - apparently issues that were discovered on the Dx-series and then found to impact the tinyAVR 0/1-series as well.

See [**our errata summary**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Errata.md) for more information.

## Supported Parts (click link for pinout diagram and details)
Note that you must install via board manager or replace your tool chain with the azduino4 version pulled in by board manager in order to work with anything other than an AVR128DA. Note also that there is a defect in some of the earliest-shipped AVR32DA parts: interrupts do not work correctly (the chip has 2-byte vectors in the hardware, instead of 4-byte ones... it's got more than 8k flash, so that's not going to work no matter what - but it *really* doesn't work with the compiler making 4-byte vector binaries!). The AVR32DA parts in circulation have been recalled from distributors and replaced with working ones, but if you bought bad ones, you'd have to shake down support to get fixed ones - It would be mighty nice if they had updated the silicon errata sheet though!
* [AVR128DA28, AVR64DA28, AVR32DA28](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DA28.md)
* [AVR128DA32, AVR64DA32, AVR32DA32](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DA32.md)
* [AVR128DA48, AVR64DA48, AVR32DA48](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DA48.md)
* [AVR128DA64, AVR64DA64](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DA64.md)
* [AVR128DB28, AVR64DB28, AVR32DB28](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DB28.md)
* [AVR128DB32, AVR64DB32, AVR32DB32](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DB32.md)
* [AVR128DB48, AVR64DB48, AVR32DB48](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DB48.md)
* [AVR128DB64 and AVR64DB64](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DB64.md)
* AVR64DD14, AVR32DD14, AVR16DD14 (pending release)
* AVR64DD20, AVR32DD20, AVR16DD20 (pending release)
* AVR64DD28, AVR32DD28, AVR16DD28 (pending release)
* AVR64DD32, AVR32DD32, AVR16DD32 (pending release)
* AVR64EA28, AVR32EA28, AVR16EA28, AVR8EA28 (pending release)
* AVR64EA32, AVR32EA32, AVR16EA32, AVR8EA32 (pending release)
* AVR64EA48, AVR32EA48, AVR16EA48, AVR8EA48 (pending release)

[AVR Dx-series comparison as a giant table](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Dx_Series_Overview.md)

My personal opinion is that the 48-pin parts are the "sweet spot" for the DA and DB-series parts - they have the real gems of the product line - the second Type A timer, the two extra CCL LUTs, and enough pins to take full advantage of these peripherals. Most people can't really find something to do with a whole 64 pins in one project - short of indulging in kitchen-sinkism just to take up pins. But the 27 I/O pins on the 32-pin parts can get used up faster than one might think (I had one project a while back where I switched to a '328PB instead of a '328P for the Rev. B, because otherwise I was 1 pin short of being able to lose the I2C backpack on the '1602 LCD, and if I did that, I could integrate the whole thing onto one PCB, and have a rigid connection between the LCD and main PCB - and then I thought I would be fine with a 32-pin Dx as that had a few more pins... But I wound up switching to the 48 and am using about half of the added pins.

For the upcoming DD-series, the 28 and 32-pin parts are of questionable utility considering the existence of the more capable DA and DB parts with the same number of pins. With AVR Dx parts already priced like the higher-end classic tinyAVR devices, and barely twice the price of the top edge of the tinyAVR 0/1/2-series, there is a pretty narrow range of prices that the DD-series would make sense at as a "poverty model" DB. The 14-pin and 20-pin packages are far more interesting, packing Dx-level capabilities into sizes and pincounts that are normally the province of the less capable tinyAVR product line. Naturally, it looks like the 28 and 32 pin ones (which likely use the same die, as I expect the 14 and 20 pin versions to do - notice that the peripheral counts change in a few cases between the big and small DDs) will be released first, potentially well before the smaller ones.

## Supported Clock Speeds
### For the DA, DB, and DD-series parts
The maximum rated spec is 24 MHz **across the entire voltage and temperature range.**
The internal oscillator can be used a 1 MHz, or any increment of 4 beyond that up to and including 32 MHz (note that this is 1/3rd more than max rating). These parts overclock very well. For compatibility with tinAVR, we also offer 5/10 MHz by dividing
All parts can use an external clock, and DB and DD-series parts can also use a crystal.
**Supported from internal:** 1 MHz, 4 MHz, 5 MHz, 8 MHz, 10 MHz, 12 MHz, 16 MHz, 20 MHz, 24 MHz, 28 MHz, 32 MHz
**Supported from external or crystal:** 8 MHz, 10 MHz, 12 MHz, 16 MHz, 20 MHz, 24 MHz, 28 MHz, 32 MHz, 36 MHz, 40 MHz, 48 MHz

If a watch crystal is installed, there is an option to "Auto-tune" the internal oscillator based on that, though the improvement is small except at extreme temperatures due to the granularity of the tuning. Note that this does not allow generation of clock speeds not natively supported; I suspect it is tuning based on the frequency before any PLL used to multiply or divide the clock speed.

The DU - assuming it ever exists in the form presented in the briefly available product brief, will likely be similar to the other Dx parts. It is highly likely - though not certain, they've been doing more with multiple clock domains on these recent parts - that only a limited number of speeds will be compatible with USB. Because all indications are that it has made great sacrifices in exchange for the USB, and hence would not be likely to see use in non-USB applications, chances are that we will only offer support for USB-compatible speeds, because if you aren't using USB, the other parts in the Dx-series would be more appropriate and effective.

See the [Clock Reference](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Clocks.md) for more information

### For the EA-series
The maximum rated clock speed is 20 MHz. It is not yet known if the clock is tinyAVR-like or Dx-like. That it has autotune suggests DxLike, but the low maximum speed is a puzzler.  It would be a shame to get the 20 MHz maximum AND the the limited manual tuning of the Dx-series.

## UPDI programming
The UPDI programming interface is a single-wire interface for programming (and debugging - **U**niversal **P**rogramming and **D**ebugging **I**nterface) used on the AVR Dx-series, tinyAVR 0/1/2-series, megaAVR 0-series, and which will likely be used for all AVR microcontrollers for the foreseeable future. In addition to purchasing a purpose-made UPDI programmer (such as the ones produced by Microchip), there are two very low-cost approaches to creating a UPDI programmer:

### From a USB-Serial adapter with included SerialUPDI (recommended)
Before megaTinyCore existed, there was a tool called [pyupdi](https://github.com/mraardvark/pyupdi) - a simple python program for uploading to UPDI-equipped microcontrollers using a serial adapter modified by the addition of a single resistor. But pyupdi was not readily usable from the Arduino IDE, and so this was not an option. As of 2.2.0, megaTinyCore brings in a portable Python implementation, which opens a great many doors; Originally we were planning to adapt pyupdi, but at the urging of the pyupdi author and several Microchip employees, we have instead based this functionality on [pymcuprog](https://pypi.org/project/pymcuprog/), a tool developed and maintained by Microchip which includes the same serial-port upload mechanism (though within that context the SerialUPDI capacity is rather hidden - that tool is more about uploading via the native-USB debuggers sold by Microchip. I had initially hoped to use it for that as well, but it appeared that wrangling the USB/driver angle was going to be a problem - so when the fix for avrdude to support the 24-bit firmware on the Microchip Curiosity boards came out, we lost interest in that. **If installing manually** you must [add the python package](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/tools/ManualPython.md) appropriate to your operating system in order to use this upload method (a system python installation is not sufficient, nor is one necessary). No additional actions are required if you install via board manager, which lets us pull that in automatically.

With the release of 2.3.6 and dramatic improvements in performance of SerialUPDI, **this is now the recommended method to program these parts**
[Instructions for wiring a serial adapter for SerialUPDI](https://github.com/SpenceKonde/AVR-Guidance/blob/master/UPDI/jtag2updi.md)

Note that this does not give you serial monitor - you need to connect a serial adapter the normal way for that (I suggest using two, along with an external serial terminal application). This technique works with those $1 CH340 serial adapters from ebay, aliexpress, etc. Did you accidentally buy some that didn't have a DTR pin broken out, and so weren't very useful with the Pro Minis you hoped to use them with? Now is your chance to make them useful.

Until 1.3.6, SerialUPDI was slow, and sometimes fiddly to set up. Those issues have been addressed and software improvements have increased the speed *by a factor of 20-30* for uploading to Dx parts, and we are able to get as close to the maximum rate that the NVM controller can write as one could hope for, with speeds in the area of 24kb/s at 345.6k baud, allowing the whole 128k of flash to be written and validated in 11 seconds at maximum speed, 16 seconds at a less demanding 230400 baud - compared with 31 seconds via optiboot, 42 seconds via jtag2updi, or a minute to a Curiosity Nano via the onboard debugger. Prior to 1.3.6, this upload method was hopelessly slow, and the same upload would have taken 2-20 minutes depending on the serial adapter.

### With an Arduino using jtag2updi
One can be made from a classic AVR Uno/Nano/Pro Mini; inexpensive Nano clones are the usual choice, being cheap enough that one can be wired up and then left like that - see using a Nano running jtag2updi, you should select jtag2updi from the tools->programmer menu. Prior to the release of 1.3.6, this was the recommended method, despite the balky jtag2updi firmware and incompatibility with converting an Arduino Micro - there wasn't a good alternative. Now there is, and jtag2updi is slower, less reliable, and costs more than SerialUPDI.

### Using a Curiosity Nano
Choose the nEDBG programmer option, and things should just work. There are a lot of things I'm not a huge fan of on those boards, and the grindingly slow upload through avrdude (40 seconds for a full 128k flash) is just one of them, but programming them should work without particular issue. Some people have had issues on linux machines, others had no problems.

### Troubleshooting uploads - quick tips
From experience, there are a few quick things to check before you conclude that there's a more serious problem. These only apply to jtag2updi and SerialUODI
* Check the serial port selected.
* If you get an error about bootloader.BOOTEND, you were using megaTinyCore, with SerialUPDI, and then switched to a DxCore part and didn't select a programmer. The IDE is still set to use the other core's programmer... which is done *using that core's platform.txt*. When you open the tools menu you'll see a that no programmer name is listed next to "programmer". Select one, and it should work.
* One of the most common problems is still poor connections. It seems like it would be very hard to have wiring problems with just 1 pin and power and ground (usually one knows if those aren't right) - but it still happens. I've worn out the crap terminals on several sets of dupont line that wi

### Optiboot-derived bootloader (optional)
There is an included Optiboot derived bootloader included with the core. You may use it instead of UPDI programming it it suits your application or preferences better. The bootloader, of course, requires a UPDI programmer to install.
[**Optiboot Reference**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Optiboot.md)

## Ways to refer to pins
This core uses a simple scheme for assigning the Arduino pin numbers, the same one that [MegaCoreX](https://github.com/MCUDude/MegaCoreX) uses for the pin-compatible megaAVR 0-series parts - pins are numbered starting from PA0, proceeding counterclockwise, which seems to be how the Microchip designers imagined it too.

### PIN_Pxn Port Pin Numbers (recommended)
**This is the recommended way to refer to pins** Defines are provided of form PIN_Pxn, where x is the letter of the port (A through G), and n is a number 0 ~ 7 - (Not to be confused with the PIN_An defines described below). These just resolve to the digital pin number of the pin in question - they don't go through a different code path. However, they have particular utility in writing code that works across the product line with peripherals that are linked to certain pins (by port), making it much easier to port code between devices with the modern peripherals. Several pieces of demo code in the documentation take advantage of this.

Direct port manipulation is possible on the parts (and is easier to write with if you use PIN_Pxn notation!) - in fact, in some ways direct port manipulation is more powerful than it was in the past. several powerful additional options are available for it - see [**direct port manipulation**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/DirectPortManipulation.md).

### Arduino Pin Numbers (if you must)
When a single number is used to refer to a pin - in the documentation, or in your code - it is always the "Arduino pin number". These are the pin numbers shown on the pinout charts. All of the other ways of referring to pins are #defined to the corresponding Arduino pin number.

### An and PIN_An constants (for compatibility)
The core also provides An and PIN_An constants (where n is a number from 0 to the number of analog inputs). These refer to the ADC0 *channel* numbers. This naming system is similar to what was used on many classic AVR cores - on some of those, it is used to simplify the code behind `analogRead()` - but here, they are just #defined as the corresponding Arduino pin number. The An names are intentionally not shown on the pinout charts, as this is a deprecated way of referring to pins. However, these channels are shown on the pinout charts as the ADCn markings, and full details are available in the datasheet under the I/O Multiplexing Considerations chapter. There are additionally PIN_An defines for compatibility with the official cores - these likewise point to the digital pin number associated with the analog channel.

### There is no A0 or PIN_PD0 aka 12 on DB-series or DD-series parts with less than 48 pins
DB-series parts with 32 or 28 pins don't have a an analog channel 0. It's located on pin PD0, which was displaced by the `VDDIO`2 pin. Based on the errata - the PD0 pad exists on the chip... but doesn't have any bond wire attached to it. Per manufacturer recommendations we disable the digital input buffer to save power.

## Exposed Hardware Features

### ADC Support
These parts all have a large number of analog inputs - DA and DB-series have up to 22 analog inputs, while the DD-series has analog input on every pin that is not used to drive the HF crystal (though the pins on PORTC are only supported when MVIO is turned off). They can be read with `analogRead()` like on a normal AVR, and we default to 10-bit resolution; you can change to the full 12-bit with `analogReadResolution()`, and use the enhanced analogRead functions to take automatically oversampled, decimated readings for higher resolution and to take differential measurements. There are 4 internal voltage references in 1.024, 2.048, 4.096 and 2.5V, plus support for external reference voltage (and Vdd of course). ADC readings are taken 3 times faster than an classic AVR, and that speed can be doubled again if what you are measuring is low impedance, or extend the sampling time by a factor greatly for reading very high impedance sources. This is detailed in the analog reference.

### DAC Support
The Dx-series parts have a 10-bit DAC which can generate a real analog voltage (note that this provides low current and can only be used as a voltage reference or control voltage, it cannot be used to power other devices). This generates voltages between 0 and the selected `VREF` (unlike the tinyAVR 1-series, this can be Vcc!). Set the DAC reference voltage via the DACR`eference()` function - pass it any of the ADC reference options listed under the ADC section above (including VDD!). Call `analogWrite()` on the DAC pin (PD6) to set the voltage to be output by the DAC (this uses it in 8-bit mode). To turn off the DAC output, call `digitalWrite()` or `turnOffPWM()` on that pin.

See the [**ADC and DAC Reference**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Analog.md)

Using the `An` constants is deprecated - the recommended practice is to just use the digital pin number, or better yet, use `PIN_Pxn` notation when calling `analogRead()`. Particularly since the release of 1.3.0 and megaTinyCore 2.3.0, a number of enhanced ADC features have been added to expose more of the power of the sophisticated ADC in these parts to users.

### Watchdog timer, software reset
There are more options than on classic AVR for resetting, including if the code gets hung up somehow. The watchdog timer can only reset (use the RTC and PIT for timed interrupts)
See the [**Reset and Watchdog (WDT) Reference**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Reset.md)

### Serial (UART) Support
All UARTs are made available as Serial, Serial1, and so on. All parts have at least 2, with the 64-pin parts having no less than 6 of them. Serial nominally refers to USART0, but variants are permitted to define it as a different UART (eg, it is supported for a variant creator to `#define Serial Serial2` - which they would do if that was the port connected to the serial adapter). DxCore does not contain any variant definitions that do that at this time.

The alternate pins may be selected using `swap()` or `pins()`. From 1.4.0 onwards, there is support for additional advanced features like one-wire mode and RS-485 mode (which automatically controls an external line-drivers TX-enable pin).
The USART support is described fully in the [Serial Documentation](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Serial.md)

### SPI support
A compatible SPI.h library is included; it provides one SPI master interface which can use either of the underlying SPI modules - they are treated as if they are pin mapping options (only one interface is available at a time - the library code available in the wild has a name collision with the I/O headers if one wanted to support using both at once, and all the workarounds that I can think of involve the libraries being changed as well). That's fine though, as treating them as pin mappings gives you most of the benefit as master, and slave support is not and never has been a thing in Arduino (it's pretty easy to do manually, at least for simple stuff).

See the [**SPI documentation**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/SPI/README.md)

### I2C (TWI) support
All of these parts have two hardware I2C (TWI) peripherals, except the 28-pin version, which has one. The included copy of the Wire library works almost exactly like the one on official Arduino boards, except that it does not activate the internal pullups unless they are specifically requested as described in the documentation linked below. The TWI pins can be swapped to an alternate location; this is configured using the Wire.swap() or Wire.pins() methods. Both of them achieve the same thing, but differ in how you specify the set of pins to use. This should be called **before** Wire.begin(), as should any method that enable certain modes.

See **[Wire.h documentation](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/Wire/README.md)** for full description and details.

From version 1.4.0 onwards, Wire.h supports operating as a master and slave simultaneously, either one set of pins (as in a multi-master configuration) or on different pins (using dual mode). Wire can now also run on both TWI peripherals in this manner - or, if only one instance is needed, but you need it on the Wire1 pins, you can swap the module that Wire uses. A facility has been added whereby the slave can check how many bytes were read by the master since the last time that method was called (useful to implement the "register" type interface with autoincrement like normal devices do).

### Alternate pin mapping (PORTMUX) support
Like most recent parts, the Dx-series parts have multiple pin-mapping options for many of their peripherals. For the serial data interfaces, we provide the same `.swap()` and `.pins()` methods like megaTinyCore and MegaCoreX (which first introduced this feature) whereby each instance of a UART, SPI interface, or I2C interface can be moved appropriately, excepting SPI1 as noted above and described in detail in that library documentation.

Many libraries that are dedicated to specific peripherals also provide a method for choosing alternate pins. This is described in the library documentation.

For PWM, it is handled differently depending on the timer:
* TCA0/TCA1 - analogWrite() is PORTMUX aware. On a part that starts up with PORTD on TCA0 (so analogWrite(PIN_PD1,duty_cycle); will make PWM come out of PD1, if you set PORTMUX.TCAROUTEA to point to PORTC, analogWrite(PIN_PD1,duty_cycle) will no longer cause PWM to be output, while analogWrite(PIN_PC1,duty_cycle) will). Only 64-pin parts have an alternate mapping for TCA1 (the 3 output only mapping options would slow down analogWrite too much to support; this is only viable because of favorable numeric alignment).
* TCD - TCD has 4-pin pinsets. The timer can output 2 PWM signals at a time, and the core assigns them to alternating pins. You are not prevented from enabling PWM on the two pins that both output the same signal (you probably don't want to do that though). There is supposed to be support for moving around those 4 pins, and when silicon that works becomes available, we will implement the same solution as above (code has been written, but the silicon bug impacts all available parts). See below for more information on PWM.
* TCB - TCBs are not for PWM. There is no support for changing their mapping. It is recommended to stick to the proper pwm timers, and use these for everything else.


### PWM on LOTS of pins
The core provides hardware PWM (analogWrite) support. On all parts, 6 pins (by default, PD0-PD5 for 28 and 32-pin parts, PC0-PC6 for 48 and 64-pin parts) - see the part specific documentation pages for pin numbers) provide 8-bit PWM support from the Type A timer, `TCA0`. On 48-pin and 64-pin parts Dx-series parts, an additional 6 PWM pins are available on PB0-PB5 (by default) from `TCA1`. `TCA0` and `TCA1` can be remapped - TCA0 can output to any port, (on pins 0-5) and TCB can output on PORTB and PORTG (on 64-pin parts only; note that DA64's have an errata here, and only PORTB works) Analog read understands this, and it will check the `PORTMUX.TCAROUTEA` register. As long as that is set to an option that allows 6 outputs in split mode (the two weird options for TCA1 aren't supported),  `analogWrite()` will make the pin output PWM, and digitalWrite() will turn it off . Note that changing PORTMUX while outputting PWM will return the current PWM pins to the whatever the PORT register says (and they'll remain outputs). Any active channels will immediately begin outputting that PWM on the new pinas long as it is set as an output. That might be undesirable; See the helper functions in DxCore.h library for some examples of this. This is only used for parts where there are available pins to choose from (eg, 48 pin parts have only a single 6-pin output option for TCA1, so this code path isn't used there).

Additionally, Type B timers not used for other purposes (TCB2 is used for millis unless another timer is selected, and other libraries mmay use a `TCB` as well) can each support 1 8-bit PWM pin. The pins available for this are shown on the pinout charts. There is no TCB PWM pin swap supported. It could be implemented, but as the timers make very poor PWM timers, and there are so many other sources of PWM, it doesn't seem worth the development time.

Some additional information about the output frequency and configuration of the TCAs is found in the [**Timers and PWM**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Timers.md) reference.

`TCD0` provides two PWM channels: `WOA` can output on PA4 or PA6, `WOB` on PA5, PA7. Those channels can each drive either - or both - of their pins, but only at one duty cycle. Users may prefer to configure this manually - `TCD0` is capable of, among other things, generating much higher frequency PWM, as it can be clocked from the PLL at 48MHz (or more, if you don't mind exceeding the specified operating ratings - I've gotten it up to 128 MHz (it wouldn't do 160 though - the PWM was glitchy), allowing 8-bit pwm at 500 kHz, or a 64 MHz squarewave). It is supposed to be remappable to other sets of pins. That doesn't work in current silicon, but future silicon revs (likely the next one - this is a particularly egregious issue, and it's something that's being expanded with additional options on the DD) will fix this - in all cases, the core uses channel A for the 2 even numbered pins, and channel B for the two odd numbered pins. For the full details, see the [**TCD and DxCore reference**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_TCD.md). If you try to get PWM out of three pins, or out of both odd or even ones, they will both output PWM, but only at the most recent duty cycle (though there is a complicated hacky way to squeeze a third channel out of it, see aforementioned document and the example it references.
```c++
analogWrite(PIN_PA4, 128); // 50% PA4. - like usual
analogWrite(PIN_PA7, 192); // 50% PA4, 75% PA7 - like usual
analogWrite(PIN_PA6, 64); // 25% PA5, 25% PA6, 75% PA7 - PA4 and PA6 are both channel A
//use digitalWrite or turnOffPwm to turn off the PWM on a TCD pin - because turning a channel on or off can cause a glitch on the other channel,
// analogWrite(pin,0 or 255) for TCD leaves it connected to the timer while outputting a duty cycle of 100% or 0%. thus:
analogWrite(PIN_PA4, 0);  // PA4 set low, but still on timer!
analogWrite(PIN_PA6, 128); // PA4 still connected, so both pins will output 50% duty cycle!
```

If you want to take full control of one of the three pwm timers (maybe you want single mode for 16-bit pwm), just call `takeOverTCA0();` For the TCA's, it will also force hard reset, so they are passed to you in pristine condition. After this, analogWrite, `digitalWrite()` and `turnOffPWM()` will pretend the timer you took over doesn't exist. If taking over `TCD0` - may the gods of silicon have mercy on you. It is one of the most fiendishly complicated contraptions on an AVR (not counting the XMega line, of course. On XMega, every peripheral is a byzantine nightmare like this). It's behavior is... often counterintuitive... but the featureset is incredible. I allowed into the core limited support for users making certain adjustments while still using `analogWrite()` because, well - because if I were to take my default position of "if you want to manually configure it, take it over"), *nobody would do it, and that seems like a waste* (it took days to get analogWrite working with it, and wasn't until a year later that I figured out why I hadn't been able to make single-ramp mode work (and latent bugs have been found as recently as 2021).  But the basics are thus:

**Note that TCA0, and TCA1 if present are configured by DxCore in Split Mode by default, which allows them to generate 8-bit PWM output on 6 pins each, instead of 16-bit pwm on three**; since the Arduino analogWrite() function we get takes values 0-255, this seems appropriate. See the [**Taking over TCA0**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/TakingOverTCA0.md) guide for more information on reconfiguring this if you need 16-bit PWM.
**For general information on the available timers and how they are used PWM and other functions, consult the guide:**
#### [**Timers and DxCore**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Timers.md)

### EEPROM - Yes
A compatible `EEPROM.h` library is included; this implementation is derived from and fully compatible with the standard `EEPROM.h` api, even though the implementation differs internally.

### USERROW - Yes - New in 1.4.0
The "USERROW", more formally known as the User Signature Space, is a small section of EEPROM-like memory which always survives a chip erase cycle. It is only erased if the application does so, or when the chip undergoes a chip erase while locked (that way sensitive information could be stored there on a locked chip). It also has the unique option of being writable via updi *on a locked chip* (using a special procedure; I've never had any reason to do it). We present a very similar interface to the EEPROM library, however, erase is all or nothing, and in order to prevent unnecessary erase cycles,it's a bit more complicated, and an additional function call is sometimes needed to commit the changes to flash.

See the [USERSIG library documentation](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/Wire/README.md)

### Pin Interrupts
All pins can be used with `attachInterrupt()` and `detachInterrupt()`, on `RISING`, `FALLING`, `CHANGE`, or `LOW`. All pins can wake the chip from a sleep mode where the clock is turned off (power down and standby unless you have set something that needs it to run in standby) sleep on `CHANGE` or `LOW`. Pins marked as Async Interrupt pins on the pinout chart (this is marked by an arrow where they meet the chip on those charts - pins 2 and 6 on all ports have this feature) can be used to wake from sleep on `RISING` and `FALLING` edge as well. The async pins can also react to inputs shorter than one clock cycle (how *much* shorter was not specified) - this is both a blessing and a curse. If you need to detect super-short pulses, it's a blessing; but if you're not, those spikes are called "noise", and the async pins are more likely to be triggered by it.

#### `attachInterrupt()` rework
There are three options, controlled by the Tools -> attachInterrupt Mode submenu: the new, enabled on all pins always (like the old one), manual (ports must be enabled before attaching to them), and old version (if the new implementation turns out to break something). Manual mode is required for the main benefit. In manual mode, you must call `attachPortAEnable()` (replace A with the letter of the port) before attaching the interrupt. The main point of this is that (in addition to saving an amount of flash that doesn't much matter on the Dx-series) `attachInterrupt()` on one pin (called by a library, say) will not glom onto every single port's pin interrupt vectors so you can't manually define any. The interrupts are still just as slow (it's inherrent to calling a function by pointer from an ISR - and low-numbered pins are faster to start executing than high numbered ones. The method to enable may change - I had hoped that I could detect which pins were used, but I couldn't get the function chose which ports to enable to not count as "referencing" those ports, and hence pull in the ISR. I am not happy with it, but "can't use any pin interrupts except through `attachInterrupt()` if using a library that uses `attachInterrupt()`" is significantly worse.

See the [**Interrupt reference**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Interrupts.md) for more information.

### Additional supported peripherals
These parts have a great many powerful peripherals far beyond what the classic AVRs did, and we provide a simple wrapper library around them when we think doing so is useful.

#### On-chip Opamps
The DB-series parts have 2 (28 or 32 pin) or 3 (48/64 pin) on-chip opamps, with programmable resistor ladder, configurable for a variety of applications. They can be used as a voltage follower (you can follow the DAC and then use the output to drive VDDIO2, though the current is still only tens of mA, that's often enough - driving heavy loads at the lower voltage is an unusual use case which requires a separate power supply; many sensors exist now with maximum voltage below 3.3V and draw very little current. This is a good use case for it.)

We provide a basic wrapper in the form of the [**Opamp Library**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/Opamp) by MCUDude.

#### Analog Comparators
The analog comparators are exposed through the [**Comparator library**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/Comparator) by MCUDude. Availability varies by pincount:
* 2 on 28 and 32 pin DA/DB
* 3 on 48/64 pin DA/DB
* 1 on all DD-series
* 2 on all EA-series

#### Configurable Custom Logic
The CCL is exposed through the [**Logic library**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/Logic) by MCUDude. Number of logic blocks depends on series and pincount:
* 6 on 48/64 pin DA/DB
* 4 on everything else

#### Event System
The event system is exposed through the [**Event library**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/Event) by MCUDude. Number of channels depends on series and pincount:
* 8 channels on 28/32-pin DA/DB-series
* 10 on larger DA/DB-series
* 6 on everything else.

#### Zero-Crossing Detector
The ZCD(s) are exposed through the [**ZCD library**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/ZCD) by MCUDude. Availability depends on pincount:
* 2 on 28 and 32 pin DA/DB
* 3 on 48/64 pin DA/DB
* 1 on all DD
* The EA-series does not have a ZCD

#### Timers
These parts for the most part are swimming in timers - The exception being the 14 and 20 pin DD-series, which are sadly stuck with a tinyAVR-like number of timers to go with its tinyAVR-like pincount. More information can be found it [Timers and PWM](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Timers.md) and [TCD0 reference](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_TCD.md)
* TCA - 16 bit timer, 3x16-bit or 6x8-bit PWM channels and lots of features.
  * 2 on 48/64 pin DA/DB and all EA-series; extra PORTMUX options of 64-pin only.
  * 1 elsewhere
* TCB - 16-bit utility timer for input capture or 8-bit PWM. No independent prescaler
  * 5 on 64-pin Dx-series parts
  * 4 on 48-pin Dx-series parts and all EA-series
  * 3 on all 28/32-pin Dx-series
  * 2 on smaller parts
* TCD - 12-bit asynchronous timer for high-speed and/or async PWM and internal PLL
  * 1 on all DA, DB, and DD parts
  * None listed on the initial DU product brief (presumably it's still on the die, but being used to generate the 48 MHz reference clock needed for standards compliant USB 2.0)
  * None on the EA-series

## Other major features

### Memory-mapped flash? It's complicated
Unlike the tinyAVR 0/1/2-series and megaAVR 0-series parts, which are able to map their entire flash to memory, most Dx-series parts have too much flash for a 16-bit address space. They can only map 32KB at a time. The FLMAP bits in NVMCTRL.CTRLB control this mapping. Unfortunately, because this can be changed at runtime, the linker can't automatically put constants into flash on 64k and 128k parts. However, on 32k parts, it can, and does. The latest ATpacks have released support for that, but it unclear how to make that usable by Arduino.  The `F()` macro works the same way as it does on normal boards as of 1.2.0, even on the 32k parts, where it is unnecessary to save RAM - this was done in order to maintain library compatibility; several very popular libraries rely on `F()` returning a `__FlashStringHelper *` and make use of `pgm_read_byte()` to read it.

See [**PROGMEM and mapped flash reference**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_PROGMEM.md) for information on how to store constant variables in the mapped sections of flash.


### Writing to Flash from App
It is possible to write to the flash from the application code using the included Flash.h library. See the documentation for more information. Note that the API is completely different in every way from the Flash.h used on MegaCoreX and megaTinyCore (which use the same flash library). They were developed independently and reflect both the differences between the two NVM controllers and the differing programming ideologies of the author of the libraries. I make no claim that mine is better, though I note that at the time, I didn't believe it would be possible to get that behavior. I was barely able to find the 4 bytes of flash in the bootloader section that this needs!
See the [**Flash Library Documentation**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/Flash/README.md)

### Servo Support
This core provides a version of the Servo library. This version of Servo always uses TCB0. If millis/micros is set to use TCB1 on those parts, servo will use TCB0 instead, making it incompatible with tone there as well). Servo output is better the higher the clock speed - when using servos, it is recommended to run at the highest frequency permitted by the operating voltage to minimize jitter.

**If you have also installed Servo** to your `<sketchbook>/libraries` folder (including via library manager), the IDE will use that version of the library (which is not compatible with these parts) instead of the one supplied with DxCore (which is). As a workaround, a duplicate of the Servo library is included with a different name - to use it, `#include <Servo_DxCore.h>` instead of `#include <Servo.h>` - all other code can remain unchanged.

### `printf()` support for "printable" class
Unlike the official board packages, but like many third party board packages, megaTinyCore includes the `printf()` method for the printable class (used for Serial and many other libraries that have `print()` methods); this works like `printf()`, except that it outputs to the device in question; for example:
```cpp
Serial.printf("Milliseconds since start: %ld\n", millis());
```
Note that using this method will pull in just as much bloat as `sprintf()` and is subject to the same limitations as printf - by default, floating point values aren't printed.
You can choose to have a full `printf()` implementation from a tools submenu if you want to print floating point numbers

**Gotcha warning** There are a considerable number of ways to screw up with printf. Some of the recent issues that have come up:
* Formatting specifiers have modifiers that they must be paired with depending on the datatype being printed, for all except one type. See the table of ones that I expect will work below (it was cribbed from [cplusplus.com/reference/cstdio/printf/](https://www.cplusplus.com/reference/cstdio/printf/), and then I chopped off all the rows that aren't applicable, which is most of them). Apparently many people are not fully aware (or at all aware) of how important this is - even when they think they know how to use printf(), and may have done so on previously (on a desktop OS, with 32-bit ints and no reason to use smaller datatypes for simple stuff).
* There are (as of 1.4.0) warnings enabled for format specifiers that don't match the the arguments, but you should not rely on them. Doublecheck what you pass to printf - printf bugs are a common cause of software bugs in the real world. Be aware that *while you can use F() on the format string, there are no warnings for invalid format strings in that case*; a conservative programmer would first make the app work without F() around the format string, and only switch to F() once the format string was known working.

From cplusplus.com:
>The length sub-specifier modifies the length of the data type. This is a chart showing the types used to interpret the corresponding arguments with and without length specifier ~(if a different type is used, the proper type promotion or conversion is performed, if allowed)~:
Strikethrough mine 'cause that don't work here (and it's not my fault nor under my control - it's supplied with avrlibc, and I suspect that it's because the overhead of implementing it on an 8-bit AVR is too large) - When incorrect length specifiers are given (including none when one should be used) surprising things happen. It looks to me like all the arguments get smushed together into a group of bytes. Then it reads the format string, and when it gets to a format specifier for an N byte datatype, it grabs N bytes from the argument array, formats them and prints them to whatever you're printing to, proceeding until the end of the format string. Thus, failing to match the format specifiers' length modifiers with the arguments will result in printing wrong data, for that substitution **and all subsequent ones** in that call to printf.

The table below is the relevant lines from that table - many standard types are not a thing in Arduino.

| length | d i | u o x X | f F e E g G a A |  c  |    s   |  p   |    n     |
|--------|-----|---------|-----------------|-----|--------|------|----------|
| (none) |int16|  uint16 | float           | int |  char* |void* | int*     |
| hh     |int8 |  uint8  |                 |     |        |      | char*    |
| l      |int32|  uint32 |                 |     |        |      | int32_t* |

Notice that there is no line for 64 bit types in the table above; these are not supported (support for 64-bit types is pretty spotty, which is not surprising. Variables of that size are hard to work with on an 8-bit microcontroller). This applies to all versions of printf - the capability is not supplied by avrlibc.


#### Selectable printf() implementation
A tools submenu lets you choose from full `printf()` with all features, the default one that drops float support to save 1k of flash, and the minimal one drops almost everything and for another 450 bytes (will be a big deal on the 16k and 8k parts. Less so on 128k ones.) - note that selecting any non-default option here *will cause it to be included in the binary even if it's never called* - and if it's never called, it normally wouldn't be included. So an empty sketch will take more space with minimal printf selected than with the default, while a sketch that uses printf will take less space with minimal printf vs default.


### Assembler Listing generation
Like my other cores, Sketch -> Export compiled binary will generate an assembly listing in the sketch folder. A memory map is also created. The formatting of the memory map leaves something to be desired, and I've written a crude script to try to improve it, see the Export reference for more information.
see [**Exported Files documentation**](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Export.md)

### EESAVE configuration option
The EESAVE fuse can be controlled via the Tools -> Save EEPROM menu. If this is set to "EEPROM retained", when the board is erased during programming, the EEPROM will not be erased. If this is set to "EEPROM not retained", uploading a new sketch will clear out the EEPROM memory. Note that this only applies when programming via UPDI - programming through the bootloader never touches the EEPROM. Burning the bootloader is not required to apply this change on DA and DB parts, as that fuse is "safe". It IS required on DD-series parts, because its on the same fuse that controls whether the UPDI pins is acting as UPDI or I/O

### BOD configuration options
These parts support multiple BOD trigger levels, with Disabled, Active, and Sampled operation options for when the chip is in Active and Sleep modes - Disabled uses no power, Active uses the most, and Sampled is in the middle. See the datasheet for details on power consumption and the meaning of these options. You must do Burn Bootloader to apply this setting, as this is not a "safe" setting: If it is set to a voltage higher than the voltage the board is running at, the chip cannot be reprogrammed until you apply a high enough voltage to exceed the BOD threshold. The BOD thresholds are quite low on these devices,

### Link-time Optimization (LTO) support
This core *always* uses Link Time Optimization to reduce flash usage.


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
### Reference Material
#### [Function Reference](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Functions.md)
Covering top-level functions and macros that are non-standard, or are standard but poorly documented.
#### [Analog Input (ADC) and output (DAC)](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Analog.md)
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
#### [IO-headers from Microchip](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/ioheaders/README.md)
These are a copy of the headers packages with the compiler toolchain to support these parts, for user convenience; they are meant for online viewing or manual installations, since a board manager installation will bury them just as deeply as the copies the toolchain uses.
#### [Assembly Listing and Memory Maps](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Export.md)
Export compiled binary generates both assembly listings and memory maps, in addition to the hex file. The options selected are encoded in the name of the file to help prevent confusion, and make it easy to compare two configurations when you are surprised by the differences between them.
#### [Reset control and the WDT](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Reset.md)
The sources of reset, and how to handle reset cause flags to ensure clean resets and proper functioning in adcverse events. **Must read for production systems**
#### [Considerations for robust applications](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Robust.md)
Covers a variety of design considerations for making something that will opperate reliably in the field, some specific to DxCore, others general. Lately I have been seeing a lot of projects get too far along without considering these. **Must read for production systems**
#### [Power Saving techniques and Sleep - inherited from megaTinyCore](megaavr/extras/PowerSave.md)
There are plans for a better wrapper around this sort of functionality, which keep getting deferred as more pressing issues come up. This was inherited from megaTinyCore and is essentially unmodified and may not reflect all the features of the Dx-series.
### Library documentation
See the [library index](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Libraries.md) or readme files for each library (the former is mostly composed of links to the latter)

## List of Tools sub-menus
* Tools -> Chip - sets the specific part within a selected family to compile for and upload to.
* Tools -> Clock Speed - sets the clock speed. You do not need to burn bootloader after changing this setting!
* Tools -> Retain EEPROM - determines whether to save EEPROM when uploading a new sketch. This option is not available on Optiboot board definitions - programming through the bootloader does not execute a chip erase function and never erases the bootloader.  ~You must burn bootloader after changing this to apply the changes~ As of 1.3.0, this setting is applied on all UPDI uploads without a "burn bootloader" cycle to AVR DA and AVR DB-series devices.
* Tools -> B.O.D. Voltage - If Brown Out Detection is enabled, when Vcc falls below this voltage, the chip will be held in reset. You must burn bootloader after changing this to apply the changes. Take care that these threshold voltages are not exact - they may vary by as much as +/- 0.3v! (depending on threshold level - see electrical characteristics section of datasheet). Be sure that you do not select a BOD threshold voltage that could be triggered during programming, as this can prevent successful programming via UPDI (reported in #86).
* Tools -> Reset/UPDI - This menu option can be set to Reset (default) or Input; the latter allows this pin to be used as a normal input. DD-series have extra options to configure the UPDI pin as well, and on these parts, This setting is applied to DA and DB series on all UPDI uploads without a "burn bootloader" cycle. It is not set on DD-series parts - the UPDI disable option makes this fuse "unsafe" to reconfigure.
* Tools -> B.O.D. Mode (active/sleeping) - Determines whether to enable Brown Out Detection when the chip is not sleeping, and while it is. Only combinations where active is at least as aggressive as sleep mode are shown, as those are the only sensible operating modees.=. You must burn bootloader after changing this to apply the changes.
* Tools -> `millis()`/`micros()` - If set to enable (default), `millis()`, `micros()` and `pulseInLong()` will be available. If set to disable, these will not be available, Serial methods which take a timeout as an argument will not have an accurate timeout (though the actual time will be proportional to the timeout supplied); delay will still work, though it's done using `delayMicroseconds()`, so interrupts are disabled for 1ms at a time during the delay, and any interrupts that happen during the delay will add to the length of the delay. Depending on the part, options to force millis/micros onto any type A or B timer on the chip are also available from this menu.
* Tools -> MVIO - MVIO option is back in 1.3.7. It is not a risk of hardware damage if it is turned off inappropriately, though the pins may not behave correctly. It saves 0.5 uA power consumption to disable it. Disabling it when you shouldn't doesn't keep the pins from being readable and writable, nor does it short the VDDIO pin to VDD.... As far as I could tell, it just no longer watches the voltage to ensure sane behavior if insufficient voltage is applied on VDDIO2. This is in effect an extra layer of monitoring like the BOD is, so the added current should not come as a surprise.
* Tools -> Write flash from App - Either disabled (Flash.h library does not work), "Everywhere" (allow writes everywhere in the flash after first page), or allow writes only above a certain address. On Optiboot definirtions, it's always enabled for writes anywhere.
* Tools -> `printf()` imoplementation - The default option can be swapped for a lighter weight version that omits most functionality to save a tiny amount of flash, or for a full implementation (which allows printing floats with it) at the cost of about 1k extra. Note that if non-default options are selected, the implementation is always linked in, and will take space even if not called. Normal Arduino boards are set to default. They also don't have `Serial.printf()`
* Tools -> attachInterrupt Mode - Choose from 3 options - the new, enabled on all pins always (like the old one), Manual, or the old implementation in case of regressions in the new implementation. When in Manual mode, You must call `attachPortAEnable()` (replace A with the letter of the port) before attaching the interrupt. This allows attachInterrupt to be used without precluding any use of a manually defined interrupt (which is always much faster to respond. Basically any time you "attach" an interrupt, the performance is much worse. )
* Tools -> Wire Mode  - In the past, you have only had the option of using Wire as a master, or a slave. Now the same interface can be used for both at the same time, either on the same pins, or in dual mode. To use simultaneous master or slave, or to enable a second Wire interface, the approipriate option must be selected from tools -> Wire Mode.


## Support Continued Development
I sell breakout boards with regulator, UPDI header, and Serial header and other basic supporting parts in my Tindie shop, as well as the bare boards. Buying from my store helps support further development on the core, and is a great way to get started using these exciting new parts with Arduino. Note that we do not currently sell a 28-pin version - this did not seem like a compelling part with the availability of the 32-pin version; the main appeal of the 28-pin part is that it is available in a through-hole version. As we would not be able to make the 28-pin version significantly smaller, there did not seem to be a compelling reason to create a 28-pin version. We may revisit this decision in the future, including potentially a 28-pin bare board for the through-hole version, suitable for assembly by those not experienced with drag soldering.
### [Assembled Boards](https://www.tindie.com/products/21007/)
### Assembled AVR128DB64 Boards - Coming Soon
### [Bare Boards](https://www.tindie.com/products/21008/)
### "Azduino Nano DB" inspired by the shape of a Nano, but with a more coherent pinout - Coming Soon
### DIY 2" x 2" DA-series or DB/DD-series prortotyping boards - Coming Soon


## Warnings and Caveats

### Direct Register Manipulation
If you are manually manipulating registers controlling a peripheral, you should not count on the behavior of API functions that relate to the same peripheral, nor should you assume that calling said API functions will be harmless either. For example, if you reconfigure TCA0, you should not expect that using `analogWrite()` will work correctly on TCA0-based pins (for example, if it's no longer in split mode, analogWrite, might overwrite one half of the compare value. If you have reconfigured the ADC for free running mode, `analogRead()` will return errors, and so on. There are exceptions to this of course - Timer/counter prescalers just change the frequency of the PWM, and there are several other exceptions noted in the above reference documents.

#### Timer PWM and exceptions
In the special case of TCA0, TCA1, and TCD0, a special function called `takeOverTCAn()` (or `takeOverTCD0()`) is provided - calling this will tell the core that you are assuming full responsibility for everything related to that timer. analogWrite on pins it is pointed at will not turn on PWM, nor will digitalWrite turn it off. This function will not be available for any timer used to control millis timekeeping (and manually reconfiguring such a timer should be expected to break timekeeping. Note that if you are using PWM on a pin provided by a type B timer (not recommended, they're lousy at it) they depend on the prescaler settings of a type A timer See the [Timers and PWM](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Timers.md) reference for more information.

TCD0 has additional specific exceptions to the normal "manual configuration = our API functions won't work right" policy, in order to keep it from being forever unused beyond what analogWrite() defaults to (it can be slowed way down, or sped way the hell up, and some of the advanced functions can be used as long as you don't touch certain parts.)

### I2C **requires** external pullup resistors
Earlier versions of megaTinyCore, and possibly very early versions of DxCore enabled the internal pullup resistors on the I2C pins. This is no longer done automatically - they are not strong enough to meet the I2C specifications, and it is preferable for it to fail consistently without external ones than to work under simple conditions with the internal ones, yet fail under more demanding ones (more devices, longer wires, etc). However, as a testing aid, we supply Wire.`usePullups()` to turn on the weak internal pullups. If `usePullups()` ever fixes anything, you should install external pullups straight away. Our position is that whenever external pullups are not present, I2C is not expected to work. Remember that many modules include their own on-board pullups. For more information, including on the appropriate values for pullups, see the [Wire library documentation](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/Wire/README.md)

## Differences in Behavior between DxCore and Official Cores
While we generally make an effort to emulate the official Arduino core, there are a few cases where the decision was made to have different behavior to avoid compromising the overall functionality; the official core is disappointing on many levels. The following is a (hopefully nearly complete) list of these cases.

### `digitalRead()` does not turn off PWM
On official cores, and most third party ones, the `digitalRead()` function turns off PWM when called on a pin. This behavior is not documented by the Arduino reference. This interferes with certain optimizations and moreover is logically inconsistent - a "read" operation should not change the thing it's called on. That's why it's called "read" and not "write". There does not seem to be a logical reason to do this, either. while it's rarely useful per se, it does let you demonstrate what PWM is just by reading the pin a bunch of times in succession. Though, I suppose that would also highlight just how slow digitalRead is...

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
Official AVR boards do not have analogReadResolution. Official ARM-based boards do, but the implementation on those boards, awash in memory and flash, is very different - they allow specifying any number from 1 to 32, and will shift the reading as required (padding it with zeros if a resolution higher than the hardware is capable of is specified). I dislike conceptually the idea of the core presenting values as if it has more recision than it does, and in any event, on these resource constrained 8-bit microcontrollers, the code to create rounded or padded numbers with 1-32 bits is an extravagance we cannot afford, and the overhead of that should not be imposed on the vast majority of users who just want to read at the maximum resolution the hardware suopports, or 10 bits for code compatibility with classic AVRs. Since `analogReadResolution()` accepts a wide range of values on the official boards that have it, it does not need to report success or failure.

### As of 1.3.3, SerialEvent is removed
SerialEvent was an ill-conceived mess. I knew that when I added support for it, but I didn't know that the mess had already been deprecated; when I heard that it was, I wasted no time in fully removing it.


### Oh, and -Wall (compile warnings) are enabled no matter what you choose in the preferences
The IDE defaults to "none", and the majority of users go through life unaware that they have this critical source of debugging information turned off. Warnings should be opt-out, not opt-in. Almost every time I see a warning, it's a latent bug that impacts behavior in an unwanted way. It's mighty handy to have the compiler tell you where your bugs are. The core and the libraries included are free of things that generate warnings - if you ever get a warning from a core file or included library which is not a `#warning`, that is a bug and should be reported as a github issue. Warnings are a good thing to keep enabled, running with warnings disabled is just making life harder for yourself; since most people using Arduino are unaware of that setting, the core is improved by ensuring that they are always shown.

## Instruction Set Enhancements (AVRe/AVRe+ vs AVRxt)
The classic AVR devices all use the venerable `AVRe` (ATtiny) or `AVRe+` (ATmega) instruction set (`AVRe+` differs from `AVRe` in that it has hardware multiplication and supports devices with more than 64k of flash). Plain `AVR` is very old, and very few devices still in production use it - it didn't support parts with over 8k of flash (no jump/call) nor a bunch of other normal functions. The few production parts that use it (like the tiny15) also had a bunch of other functions dropped. In practice, AVR has meant `AVRe` or `AVRe+` for most of it's life. Modern AVR devices (with the exception of ones with minuscule flash and memory, such as the ATtiny10, which use reduced core `AVRrc` which drops many of the more complicated instructions), use the latest iteration of the AVR instruction set: `AVRxt`. This adds no new instructions (unlike `AVRxm`, the version used by the XMega devices, which added 4 combined read-write instructions for accessing SRAM. (which I think doesn't include peripheral registers, which takes out all the use cases for it that I can think of), but a small number of instructions have improved execution time, (and one has slower execution time). This distinction is unimportant for 99.9% of users - but if you happen to be working with hand-tuned assembly (or are using a library that does so, and are wondering why the timing is messed up), this could be why:
* PUSH is 1 cycle vs 2 on classic AVR (POP is still 2) (this is the biggest change in terms of overall speed - It is not unusual for 20% or more of an arduino sketch to be push and pop instructions! Now, one could argue that this is an incictment of the author and framework - I wouldn't disagree, but this is an Arduino core, so... )
* CBI and SBI are 1 cycle vs 2 on classic AVR (Sweeet! very noticeable when we use it, even though it's a relatively rare instruction.)
* LDS is 3 cycles vs 2 on classic AVR :disappointed: LD and LDD are still two cycle instructions.
* RCALL and ICALL are 2 cycles vs 3 on classic AVR
* CALL is 3 cycles instead of 4 on classic AVR
* Best one for last: ST and STD is 1 cycle vs 2 on classic AVR! (STS is still 2). This includes std as well, which makes this nearly as large as the improvement to push.
* When ld, ldd, or lds is used to load data from flash using the flash mapping... it takes an extra clock for the NVM access (as far as I can tell; the location in the datasheet where theinstruction set manual "says more information can be found" has no such information or mention of any delay, but the instruction set manual says "minimum of 1 cycle must be added".)
* Not quite the instruction set... but on classic AVRs, the working registers were accessible at loctions 0x00 to 0x1F in the main address space. They no longer are. That means the offset of 0x20 applied to peripheral registers is gone, and the SFR_TO_IO_ADDR macro is gone too (SFR stands for Special Function Register I think)

This really comes down to 2 changes
* Faster stores - including ST, STD, and PUSH (PUSH and POP are essentially ST SP+ and LD -SP, treating the stack pointer like one of the register pairs And since CALL and RCALL involve pushing something onto the stack, you'd pick up enhancement there for free too. This almost certainly accounts for the slowdown in LDS. If I had to guess, I would say that LD's take 1 clock to send the address to the memory controller, andget the data in the next clock. Since in LDS, it doesn't have the address until the second clock, there you go.
* CBI/SBI are single clock - unclear if the mechanism is the same as above/

## License
DxCore itself is released under the [LGPL 2.1](LICENSE.md). It may be used, modified, and distributed, and it may be used as part of an application which, itself, is not open source (though any modifications to these libraries must be released under the LGPL as well). Unlike LGPLv3, if this is used in a commercial product, you are not required to provide means for user to update it.

The DxCore hardware package (and by extension this repository) contains DxCore as well as libraries, bootloaders, and tools. These are released under the same license, *unless specified otherwise*. For example, tinyNeoPixel and tinyNeoPixel_Static, being based on Adafruit's library, are released under GPLv3, as described in the LICENSE.md in those subfolders and within the body of the library files themselves.

The pyupdi-style serial uploader in megaavr/tools is a substantially renovated version of pymcuprog from Microchip, which ~is not open source~ *has now been released under the open source MIT license!*.

Any third party tools or libraries installed on behalf of DxCore when installed via board manager (including but not limited to, for example, avr-gcc and avrdude) are covered by different licenses as described in their respective license files.
