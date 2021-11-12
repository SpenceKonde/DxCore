# Changelog
This page documents (nearly) all bugfixes and enhancements that produce visible changes in behavior throughout the history of DxCore. Note that this document is maintained by a human, who is - by nature - imperfect; sometimes the changelog may not be updated at the same time as the changes go in, and occasionally a change is missed entirely in the changelog, though this is rare.

## Changes not yet in release
Changes listed here are checked in to GitHub ("master" branch unless specifically noted; this is only done when a change involves a large amount of work and breaks the core in the interim, or where the change is considered very high risk, and needs testing by others prior to merging the changes with master). These changes are not yet in any "release" nor can they be installed through board manager, only downloading latest code from github will work. These changes will be included in the listed version, though planned version numbers may change without notice - critical fixes may be inserted before a planned release and the planned release bumped up a version, or versions may go from patch to minor version depending on the scale of changes. This release really snowballed.... It was supposed to be a quick, small release! Since not many people use the board packages without the installer because of the need to manually update the toolchain, the only way I'm going to get eyes onto this thing is to release it.
### Planned 1.4.0
__ DD-series support, assuming they ever come out (All that's left undone here is the variant files, which were delayed by the serial questions, but now that those have been answered, that can proceed.

### 1.4.0 or earlier, depending on events
__ Completely new Wire.h library with exciting features - Thanks to @MX682X!
* Master + Slave on the same TWI - either using the same pins or in dual mode
* Support for TWI1 assuming your device has it.
* Reduced flash use - not as much of a pressing issue compared to megaTinyCore, I know, but even using both master and slave will consume less flash than the old library. And future parts, when they come out will use them.
__ Looks like we we're getting those serial changes - also thanks to @MX682X!
__ Expanded Event library from collaboration with @MCUDude
__ USERSIG library for Dx-series parts. Has a few extra complications, but that's Microchip's fault!

## Released Versions

## 1.3.10
* Fix Wire.swap() correctly. (#184)
* Correct burn bootloader when BOD is enabled. (#185)
* Correct a an error in SerialUPDI definitions for AVR64DD28 devices (which are not yet available)
* Implement CI automated testing! Finally!
* Implement expanded markdown format checking.
* Remove teaser added in 1.3.7 as the document did not meet editorial standards and was not worth retyping. My hope remains for automatically making the .map files human (or machine - either would be an improvement) readable, and cleaning up the bogus offsets relative to the nearest constant when avr-objdump can't find the logical one to use in the .lst files. This is a low priority, alongside the instruction usage count tool I want to make which will probably be done at the same time.
* Harmonize EEPROM.h with megaTinyCore, update documentation.
* Major cleanup of Comparator library to ensure support for future parts and harmonize codebase across all supported parts. Oh, and it compiles now too.


## 1.3.9
* Doc improvements continue. Added reference for functions provided by the core, expanded documentation of constants, and fixed some poorly written documentation and comments.
* Correct critical bug with burn bootloader on non-optiboot DB-series parts - the MVIO menu hadn't been restored there.
* Correct critical bug that prevented all compiling for DA-series parts.
* Docs - added function (or macro - most are macros) reference that should contain all functions not present in the stock cores, and many that are but aren't properly documented.
* Corrected bug relating to appspm and exported binary naming.
* Correct critical bug that made SPI.h incompatible with the new attachInterrupt().
* Correct critical bug compiling for 64-pin parts due to a stray character in the variant file.
* Correct critical bug in 28-pin variant file that broke writes to pins 0 and 1 within every port!
* Markdown linting.
* Minor things
  * Added a few variants of `_NOP()` for longer delays in minimum number of words. 2 clocks in 1, 8 clocks in 3, 14 clocks in 4 (simplest loop is 3 * n + 1 in 3 words, or pad with nop/rjmp .+0 for any number of clocks up to 770 in 3-4 words; in an ISR that loop may add 1 clock to the start of the ISR and 2 clocks to the end, as well as 2 words to the size of the binary (though it won't if there's an, everything else below 770 in 6 words)
  * analogClockSpeed used 300 kHz as the minimum instead of 125 kHz.
  * `PIN_TCA0_WO0_DEFAULT` and `PIN_TCA0_WO1_DEFAULT` were defined when those pins were not available because they were used for the main clock source.
  * Handle a compatibility issue with the TCA event action.
  * Mention newly disclosed DAC errata for DA-series
  * Fix INLVL feature detection.
  * Default external crystal settings should be more forgiving - instead of using the crystal drive strengths for >24, >16, >8 and <= 8, we use them for >=24, >=16, >=8 and <8, and we specify a longer startup time for the external crystal in hopes of having it handle crystal problems a little better (#170)
  * Reduced binary size of tinyNeopixel by 2 or 4 bytes for some clock speeds.
  * Fixed dumb mistake that impacted only my Azduino board defs. My attempt to ensure that someone couldn't burn out a pin which I made a questionable decision about in the hardware of the Azduino Nano DB Rev. A by setting that hardwired pin as an OUTPUT, instead ensured that no pin could ever be set as OUTPUT.

### ~1.3.7~ 1.3.8
* General:
  * Reimplemented attachInterrupts completely, so it now has a separate file for eaach port.  It doesn't do what I'd hoped to be able to do, get it to automatically only include ports you attach to; so I default to it being on all ports for compatibility, but have menu option to either do that manually - or revert to the old version if I broke something. Note: I didn't bother fixing PORTG on the old version implementation. That never worked, and nobody noticed! The new version is smaller.
  * Move to C++17 pulling in the changes from megaTinyCore (to new.cpp and new.h)
  * We weren't passing -mrelax to the linker. Without it, we got larger binaries because the linker missed opportunities to optimize jmp and call instructions (2-word instructions taking 3 cycles) to rjmp and rcall (1 word instructions taking 2 cycles). This magnitude of the improvement in binary size is surprising considering you're saving only 2 bytes and only on jumps and calls that didn't have this already, and it's clear that it does more than just replacing those instructions with the shorter ones (looking at assembly listings, the binary has been reorganized significantly). Anyway, it's not a risky optimization, and it should be turned on. I thought it was on already!
  * Add missing device definitions to SerialUPDI, including some that microchip has failed to release for currently available silicon.
  * Avrdude verbosity was grossly excessive for jtag2updi. This is corrected, it is now like megaTinyCore.
  * Add getAnalogReference(), getDACReference(), getAnalogSampleDuration() and getAnalogReadResolution() macros (all are trivial, all should have existed for a long time).
* Startup related (also, see new reference on reset)
  * Ensure that anything that determines what is built is always part of the name of expoered binaries. Writing to SPM from app wasn't, and this caused difficulty tracking down huge bug.
  * Ensure that that functions in `.init` sections do not get a `ret` autogenerated at the end. They must have attributes used _and_ naked. if not ((used)) they are optmimized away. If not ((naked)) they will get an autogenerated `ret` and the end which will generally result in a dirty reset loop. Also, if have any sort of entrypoint like we do for unrestricted flash writes from app, you need to have them start with an rjmp to jump over the body of the function in case execution gets there during startup. Also the init sections don't work for getting things into the first page of flash. To do that, you need to put it in `.trampolines`, otherwise progmem variables will get to go first.  and the ) that do have a return, thery need to start weith an rjmp to carry execution at startup around them.
  * Add (commented out) entries to boards.txt for DD series (only one thing is not known: size of bootsize/codesize segments) and EA without optiboot. Re-add MVIO menu: I finally checked on hardware, MVIO being disabled just seems to turn off the oversight of that behavior. With it disabled it does not short VDDIO2 to power rails as some, including myself, had feared it might. Add menu option to enable printf (and everything that uses it) to print floats.
  * Fix critical bug relating to flash writing (don't think it was ever in a release).
* Pin-mapping related
  * Wire.swap(2) was broken on parts that didn't have a pinswap 3. No parts, available nor announced, have a pinswap 3. Oops.
  * Fix bug in variants files regarding PA0, PA1 on all parts - the PORT needs to be defined accurately, but when they're not available, mask and position are NOT_A_PIN so accessing them when we know we can't will give compile error. Improve formatting.
  * Add `PIN_NOW` macros, eg, `SCL1_NOW`.
  * Fix bug where missing pin related macro on 28 pin parts related to TWI1 would cause compile errors (may not be in any release)
  * Fix bug whrre under some conditions, errors would be encoutnered for 32-pin parts due to pinmappings missing for USART2 (not in any release)
  * Various pins_arduino formatting improvements echoing what I did to boards.txt
  * Correct issue with name of portToPinZero() a macro used by the DxCore library to assist in managing TCA multiplexing.
  * Split DxCore.h into DxCore.h and DxCore.cpp, because otherwise you couldn't include it from more than one file without duplicate definitions.
  * Fixed bug that caused analogWrite() to reject all pins as invalid (a simple typo - I'd meant to do an `if () {...}` structure spread across 3 lines in the normal fashion - but instead wound up with an `if () {} ...` structure, so whether or not you passed and invalid pin, it will reject it and produce a compile error.
  * Fixed bug in digitalPinHasPWM macros on non-64-pin parts. How had this gone unnoticed?!! Oh - because the core doesn't use it internally (we use the PORTMUX-aware function call, digitalPinHasPWMNow(), which can't always be a drop-in replacement for the macro, since the macro is constant for a given pin while digitalPinHasPWMNow() obviously cannot be) and few libraries use it.
* PWM and timekeeping
  * Correct bug with TCD PWM which could fail to enable PWM because of being unable to set TCD0.FAULTCTRL immediately after disabling the timer - the disabling hadn't yet taken effect. Only happened when the TCD clock was slower than system clock, IIRC some other things manifested in that way. Once we set FAULTCTRL, there is no further sync delay needed before reenabling.
  * Fix broken timekeeping on overclocked devices with TCA as millis source.
  * Add onClockFailure() and onClockTimeout() and defined weakly so users can override.
  * Fix integer overflow bug at high clock speeds with some timekeeping options.
  * Improve micros() tolerance to disabled interrupts so it will give accurate results for longer after interrupts are disabled. We had been assuming that it could take us almost an entire overflow period to get between the point when we read the ticks and the point when we read the flags on the next line, on the grounds that micros is not expected to work while interrupts are disabled. But we can make it work for most of an overflow period - we don't *have* to break it a couple of timer ticks after the overflow.
  * delayMicroseconds(not_compile_time_known) was inaccurate for several supported speeds at the high and low ends of the range, notably 5, 10, 40 and 44 (which is for some reason supported in the timekeeping functions, if not in the board definitions). 20 MHz used an excessive intermediate value which was both inaccurate and overflowed with significantly lower values passed than any other speed. 20 MHz now uses a 10 clock loop instead of a 4 clock one, so you can go up to a 32767us delay with it instead of 13107us. 5 MHz and 10 MHz now use a 5 clock loop, and do a better job on very short delays.. 44 now uses an 11-clock loop, and both 40 and 44 now burn enough clocks to get 1 us delays right or at least more right.
  * Correct frequency of PWM output from TCD0 at 20 MHz, which was 8 times faster than intended.
  * When the clock source is NOT the internal oscillator, TCD0 will be configured to use the internal oscillator as it's clock source, and it will be set to 8 MHz and TOP to 254 for consistent 980 Hz PWM out of the box.
  * Corrected micros at 1 MHz. The ISR takes 65 clocks, we really don't need to jump through hoops to call it half as often, and micros assumed we didn't do that, but timers.h thought we did.
  * Fix obscure bug with TCB when used as PWM after you have manipulated the timer in ways rarely encountered on this core.
  * Correct micros on more obscure frequencies, mostly overclocked ones for which no division-without-division was available.
  * Support 25 and 30 MHz operation from an external crystal or clock source. This makes the list of frequencies match what when support on megaTinyCore via tuning of the internal oscillator, and answers user requests (this would not normally be added because of user requests alone - more suitable crystals are inexpensive and readily available, and the internal oscillator is very well calibrated so external crystals are rarely necessary anyway. Seems we have more than one user who has an abundance of 25 MHz crystals and is too cheap to buy new ones.)
  * Got fed up with the crap job the compiler did of on type B timer micros math. The two most importnant mathy (non-power-of-two MHz) clocks - 20 (and 40/10/5) and 24 (also 48/12) MHz are two of the hardest to do with bitshift-division. The compiler's incredible stupidity was just too much.
  * Fix typo in resumeTCA1()
  * init_millis() is supposed to be weakly defined so you can override it to kneecap millis from within your code, without relying on menu option. It wasn't
* Absolutely massive documentation overhaul
  * Reorganized documentation dramatically. Less is in readme, more broken out into smaller files on individual topics. Remove several hopelessly out of date or inapplicable documentation files.  Fix major documentation links so that they always point back to the repo, so that the readme can be used to get links from, even when even dug from a board-manager install, which doesn't have most of the docs included. New documentation pages are named Ref_Subject.md (where Subject is replaced by the subject of that file)
  * Documented reset flags and how important they are to check and **clear**. Optiboot does this and stashes them in GPIOR0 in case you care (and resets if there are none, indicating that the board just jumped to 0 or execution wrapped around from the end of the flash - likely due to a bug (a lot of memory corruption ends with this happening: if you somehow end up scribbling on the stack, eventually a function return address gets clobbered and now contains a bogus value. Once a return address is damaged, your fate is sealed: eventually it returns you to someplace that isn't where the function was called from. If it's a high value (empty flash), execution then skids along that until it wraps around and restarts from reset vector without reinitializing anything. If it's a small number (from say, an integer representing a small number), it will land in the reset vectors, likely jumping to BAD_ISR, which then jumps to 0), though some naive and poorly written code dating to classic AVRs lacking the software reset functionality commits this terrible sin intentionally (instead of doing a WDT reset, likely because it was meant for pre-optiboot days where the WDT reset would bootloop). Such crimes are inexcusable a decade or so since there ceased to be a valid use case for them. Now, non-optiboot configurations check them in .init3 amd resets if there are none. But it doesn't clear them out of fear of breaking an application that needs to know them; so it's check is useless if you don't clear those flags.. Either be sure to clear them at start of setup, or (for production code) override init_reset_flags() and clear them there.
  * Improve documentation on ADC, advanced digital I/O, the principles behind what we consider invalid and generate errors on, and ways of resetting (including use of the WDT to reset when hung), moving those to their own file and linking from the readme. This treatment needs to be given to more of the document. It has suffered from too much info being crammed into one document, resulting in something with the form of a wall that many people don't read.
  * Clean up and improve comments, particularly in wiring.c
  * ~Add teaser for possible future improvements to "export compiled binary" assembly listings and memory map (for assemgly listings, I just want to get rid of the irrelevant and stupid offsets relative to randomly chosen constants that it often uses instead of references to more sensible landmarks (eg, offsets of peripheral registers referenced to any things like `__EEPROM_REGION_LENGTH__ + 0x45` and garbage like that - it assumes that all of these are addresses (and meaningful ones) which they almost never are. For the map, it needs the columns fixed, or at least the formatting tweaked so you can import it into a spreadsheet. Right now it requires a manual regex step just to get it clean enough that excel can import it, and is hopelessly unreadable as text without even more manual regex-application to it. We pull in a python installation for SerialUPDI, why not get some more use out of it processing the crap listing and memorymap output avrgcc gives us to work with?)~

### 1.3.6
* Improve #defines relating to on-chip peripherals which operate on specific pins.
* Introduce dramatically enhanced performance for SerialUPDI (it has a name now!) programming method. This is now the recommended programming method. The read and write speed compared to jtag2updi is faster by 2:1 when the baud rates are equal, and SerialUPDI can operate at a baud rate three times higher with most serial adapters. SerialUPDI does not suffer from the bugs that afflicted jtag2updi either, and can be made with serial adapters that are cheaper than dirt: 7 for $5 including shipping - using only an external diode! There is essentially no reason to use jtag2updi on a Dx-series part other than having already built a dedicated jtag2updi programmer.
* Ensure that Wire library correctly configures pin registers. Even when they were configured improperly.
* Correct TWI baud rates (change imported from from megaTinyCore #422)
* Adjust TWI buffer sizes. Everything with at least 4096b of ram (ie, all parts supported by this core except the future AVR16DD) has 130 byte buffers (why 130? That *just* lets you squeeze in a full page read or write from an AT24-class EEPROM - and these parts have so much ram it's not even funny. We don't have to be cheap with it like we do on small parts. Everything else gets 32 byte buffers except for the smallest tinyAVR parts (the same version of Wire and associated files is used here and on megaTinyCore)
* Correct bug that prevented Servo and Servo_DxCore from compiling. Tidy the code surrounding the point where the issue arose (including eliminating the need to disable style checking there, and stuff the ugly stuff into ServoTimers.h with similarly ugly stuff.
* Changed name of constant to FLOATING instead of FLOAT, as that is used for some floating point related things. Actually documented openDrain() so people might end up actually using it. You can also just use HIGH - it just seems dirty to call it that, when we're NOT driving it high.
* Implement pinConfigure() to set weird options in PINnCTRL easily, in arbitrary combinations.
* Implement all the new ADC functionality, including analogClockSpeed, as well as analogReadEnh and analogReadDiff!
* Most keywords from datasheet imported to keywords.txt. Some values were dumped to reduce the length, and it's still crazy long.
* Massive boards.txt cleanup (internal - but far more navigable!)
* Correct bug with CRYSTAL vs XTAL (thanks Microchip, for changing the names your headers define after product release, and with no compatibility layer). Slight cleanup in init_clock(). Add compatibility measures to Arduino.h
* Correct bug with new.h/new.cpp missing a key method (#434)
* The Opamp library's `Opamp0` (and the other two) objects now have public member variables: input_p_pin, output_pin, and input_n_pin. so you can do analogRead(Opamp0.output_pin) - mostly intended to permit more modular code, and code where references are used such that you don't knnw which opamp you even have3 without investigating.

### 1.3.5
* Board Manager re-release only. Windows toolchain package was mangled too.

### 1.3.4
* Adopt Comparator library fixes from [https://github.com/MCUdude/MegaCoreX/issues/115](https://github.com/MCUdude/MegaCoreX/issues/115).
* Board manager re-release to pick up fixed toolchain version for Linux/Mac
* Programmers.txt names tweaked - the changes in 1.3.3 were directly copied from megaTinyCore and referenced official boards that don't exist for the Dx family.
* A number of minor internal changes and corrections.
* Documentation enhancements.

### 1.3.3
* New version of the Arduino <-> pymcuprog bridge introduced - verbose output vanishing bug fixed, and it can no longer fail silently when it receives invalid command line arguments (this was leading to "burn bootloader" with missing bootloader file reporting success but burning no bootloader.)
* New toolchain package with support for AVR64DD and tinyAVR 2-series parts with 32k flash.
* Add a lot of infrastructure that will be used for future ADC enhancements. Nothing major that is user-facing - yet - just the plumbing.
* Add non-user-facing infrastructure to support the AVR DD-series' large number of UART mux options.
* Add PIN_HWSERIAL macros for the XCK and XDIR pins. Wasn't hard to do, hadn't realized I never did this in DxCore.
* Split off the stuff in Arduino.h that involves defines representing specific parts, ennumerating hardware capabilities that aren't part of the standard API (ie, the analog references don't move, but the things to create all the group defines like `__AVR_DA__` and `Dx_48_PINS` and so on do get moved.
* Correct large number of tone() bugs. Tone could leave the pin high. A high pitch and long duration could result in much shorter tones than intended. Frequency of 0 would leave the timer running and generating interrupts in the background for the requested duration (now it stops output if it's on the current pin, otherwise does nothing). while doing nothing. No attempt was made to handle invalid pins: it would get null pointer to port struct.... and proceed to use it without testing if it was valid, or if the bitmask was valid. Now we test for valid bitmask before requesting the port struct. Frequencies over 32768 would overflow an intermediate when called with duration and the duration would be very short. Long durations can also overflow the intermediate. The intermediate is found as `2 * frequency * duration` and both multiplications can overflow (first one is as unsigned int, second as signed long.) Since the result is divided by 1000, remove 2* and change to /500 doubled the limit and removed the problem above 32768 Hz. It could still run into a problem because that /500 meant that it would roll over at 1/500th of the maximum value for 32-bit integer; this was solved by dividing frequency by 5 and duration by 100 when we are at risk (defined as duration > 65535) DxCore defaults to supporting long tones, as all Dx-series parts have at least 16k of flash.
* ~Shamelessly copied from~ _Inspired by_ ArminJo's version of the digistump cores - no more shell script on mac/linux for assembler listings; bash is invoked with the command required and the -c flag. Are there any problems with this? Getting rid of the damned shell scripts would be great. Also eliminated double-entry-record-keeping regarding the name structure of those files (er, well.... the information is still in 2 places, but at least now it doesn't have be in triplicate on 3 sets of 3 rows. That's how stupid bugs sneak in. Also, we now create a memory map as well as the assembly listing!
* Fix serial hang bug at high speeds and low system clock - megaTinyCore #352. Initially thought to not be relevant to DxCore, it was just hidden because we default to using a TCB for timekeeping. This issue never reproduced with a TCB as millis source because that ISR runs so fast that even if it happens at the critical, pessimal instant, it runs fast enough that it won't break anything.
* USART0 is now Serial0, rather than Serial, but Serial is #defined'ed as Serial0, so this should be transparent. It does, however, allow us to reroute "Serial" to the HW serial port connected to the serial adapter, for example, in the future, rather than having to modify every Serial.print(); This will likely be of partoicular DD-series parts in general, where the cramped pinout may lead to the effective roles of USART0 and USART1 being swapped under common conditions for the 14 and 20pin parts on the DD-series. Even the DB-series isn't free from the tyranny of pinout distribution, with a less flexible PORTMUX that leaves USART0 stuck on PORTA. Errata leaves TCD0 stuck on PORTA if , which is the hottest piece of real estate on the part - the primary pins may be needed for an external crystal, the bug with the TCD0 PORTMUX leaves only it's PORTA pin set usable, and those ALSO overlap with the only pinout option for SPI0 on 28/32 pin parts, (and the only option for SPI1 overlaps with Serial1!). So if you need SPI, and a crystal .
* fix bug with RISING edge interrupt in comparator library being configured on FALLING edge instead. Improve documentation and add in::pd2, in::pd6 to in_p, in::pd0, in::pd7 to in_n - these are aliases to the corresponding inputs, which are available on all three comparators. Tweaks to documentation and examples.
* Fix bug where analogWrite() of 0 or 255 to PD6 (the DAC output) would call digitalWrite() to disable the DAC output entirely. Instead, we leave it connected and assign the value the user passed to the DAC's data register (#83).
* The bungled Wire1 library was observed to not even compile anymore. That mess is a disgrace and never met any reasonable quality standard for inclusion in DxCore. The non-functional library has been removed from the distribution. Producing a proper Wire1 library requires someone with far deeper knowledge of C and C++ than I; looking at the changes involved in this change on MiniCore for a classic AVR - I have not the foggiest idea how to approach this.
* Correct inconsistencies in OpAmp docs vs code. (#89)
* Correct bug where the BOD could not be set to anything other than Disabled (would generate error when Burn Bootloader selected) because the default BODCFG definition (defining it in terms of bodlevbits and bodmodebits - each controlled by the appropriate submenu) was missing from boards.txt - it was only defined when it was overridden by the mode: disabled/disabled option, and on one of the two lines in each board referring to the Enabled/Sampled (32 Hz) option, it was named ensampslo instead of ensampslow.
* Corrected sampling frequencies listed for sampled BOD (1 kHz/125 Hz is only for ATTiny 0/1/2, these use 128 Hz and 32 Hz
* Remove optional support for SerialEvent; it is deprecated in the official core (which I hadn't noticed when I added support). SerialEvent was an awful misfeature that should never have existed; with the official core having seen the light and deprecated it, I feel no obligation to keep it in. These are new shiny parts, and it is well established that SerialEvent was an abomination. Let us not allow it on this nice new silicon.
* Add a bunch of stuff to keywords.txt
* Add BOOTUSART and BOOTSWAP defines based on bootloader usart menu, since that's probably where the monitor will be connected. Use to determine SERIAL_PORT_MONITOR.
* delayMicroseconds now uses the implementation in wiring.c only when the delay dduration is not known at compile time. When it's compile time known, we use the avrlibc `_delay_us()` function.
* delay() no longer breaks when called asked to wait for more than 4.29 million ms. One could argue that if someone was running into that problem, they were doing something wrong anyway, but there is no reason delay() needed to do it the way it did - certainly not for the paltry 24b of flash it saved on parts with 32-128k.
* micros() using TCA0 or TCA1 as timing source did not experience the arrow of time - in rare circumstances, time could go backwards, and micros could return a lower number than a preceding call. While 800 us is not long enough to generate the classic paradoxes, it was certainly enough to break almost any practical application of micros(). With considerable effort, flow of time was constrained to the forward direction; additionally, the fix for TCD0 was ported from megaTinyCore - while TCD0 is not supported on DA and DB-series parts, it IS planned to be an option on the DD-series where the more interesting packages - 14 and 20 pin - are also sadly very tightly constrained on the all-importsant type B timers. This way, if two type B timers are required along with a type A timer that must be "taken over", TCD0 can be used for millis instead. On the DA/DB parts, with at least 3 type B timers, and in many cases more, I see less value to this (and more value in leaving TCD0 free to do exotic things with) on the DA/DB-series part
* non-Arduino-IDE tools may not pass all the defines that we expect - basically, no defines that we platform.txt and boards.txt can normally guarantee can be relied upon to be there!. If Arduino.txt does not see a DXCORE define, that must have happened. In this case, detect and define a placeholder - checking for that define is how other libraries recognize that this core is in use. Also check a few other important defines for existence, rather than assuming that because my boards.txt and platform.txt will always provide them, that they will be present and one of the valid options - the intent is simply to make sure we don't sleep-walk into a wacky wromg state (imagine an X which is always defined as A, B or C via the boards.txt/platform.txt configuration, one could do `/* well it's gotta be C then right? */`. But in a situation where we don't fully control the defines passed to the compiler through the command invocation with total certainty is supposed to be supported, we need to complain if there make sure we don't blindly assume that can't-happen-per-our-boards-and-platform-definitions actually can't happen, thus above situation needs to be `#if X==A` ..... `#elif X==B`  ..... `#elif X==C` ..... `#else      #error "X undefined or invalid, X must be #defined as A, B or C" #endif`. Doing this is less work than dealing with the support requests that would result from not doing so.
* Add some defines to indicate features provided by the core.... This list will be expanded over time:
  * `CORE_HAS_FASTIO` - if defined as 1. indicates that digitalWriteFast() and digitalReadFast() is available.
  * `CORE_HAS_OPENDRAIN` - if defined as 1, indicates that openDrain() and openDrainFast() are available.
  * `CORE_HAS_PINCONFIG` - Indicates that pinConfig() is available. Currently, this is not defined here as this is not yet implemented.
  * `CORE_HAS_TIMER_TAKEOVER` - if defined as 1, takeOverTCxn() functions are available to let user code take full control of TCA0, TCA1 and/or TCD0.
  * `CORE_HAS_TIMER_RESUME` - if defined as 1, the corresponding resume TCxn() functions, which reinitialize them and return them to their normal core-integrated functions, are available.
  * `ADC_NATIVE_RESOLUTION` - This is the maximum resolution of the ADC without using oversampling. 12 for Dx-series.
  * `ADC_NATIVE_RESOLUTION_LOW` - This is the low ADC resolution setting. 10 for all Dx-series parts.
  * `DIFFERENTIAL_ADC` - This indicates that the part has a differential ADC. It does not indicate whether said differential capability is exposed by the core.
  * `SUPPORT_LONG_TONES` - This is 1 if the above mentioned long tones are supported.
  * `CORE_HAS_ANALOG_ENH` - This is 1 if the enhanced version of analogRead is available, with automatic oversampling and decimation to extend resolution.
  * `CORE_HAS_ANALOG_DIFF` - This is 1 if the differential analogRead is available. It has same features as enhanced, except that it takes a differential measurement.
  * `MAX_OVERSAMPLED_RESOLUTION` - This is the maximum resolution obtainable via oversampling and decimation using those functions (when those are implemented, this will be 15)
  * `ADC_MAXIMUM_GAIN` - Some parts have an amplifier, often used for differential readings. If any Dx-series parts had one, this would be defined as the maximum gain. Unfortunately, none of these do.

### 1.3.2
* Correct critical bug that prevented bootloaders from being installed when using the pyupdi-style serial port and resistor. Would report success, but only fuses were set. Critical bug is not fixed.
* Formatting and "make the CI pass" changes
* Rename optiboot_x.c to optiboot_dx.c, remove references to tinyAVR 0/1/2-series in source code. It is very different, so we shouldn't give it the same name
* Fix bug with "burn bootloader" for DB-series parts with more than 28 pins.
* Improve library docs, remove flashWrite() library
* Add GPIOR compatibility defines.
* Fix the issue with sideffects in the min, max, constrain, and sq functions.
* Fix bug where the INVEN bit would be unset when doing digitalWrite() on pins that can do TCD PWM; now it is only unset if PWM was actually turned off (which in turn implies that if it was inverted, that was done by analogWrite(pin,255) - that's how we can generate a continuous HIGH output without disconnecting the timer (which would produce a glitch on the other pwm channel)- we set the compare value higher than TOP (which would produce continuous low) and invert the pin.
* In the process of above, found both a terribly inefficient bit of code in analogWrite and turnOffPWM, and replaced with a much faster implementation that saves some flash too! By switching to bit_mask for most calculations a variable shift which could run as many as 7 iterations at 4 clocks each was eliminated from analogWrite. A loop that could
* Correct multiple bugs in takeOverTCAn() functions, which were missed during testing (How, I'm not sure, some of them were far from subtle)
* Add openDrain(), openDrainFast() - called with two arguments, a pin, and either LOW, CHANGE, or FLOAT. (FLOAT is simply #defined as equal to HIGH). These are the direction-only homolog to digitalWrite() which only operates on output val (ie, PORTx.DIR vs PORTx.OUT). The full size (but not the Fast version) sets the pin's output valure to 0 as well - thus, if you use the non-fast one the first time you call it, you can subsequently use the fast version and not worry about it slipping up and driving the pin HIGH. These do not change the pullup configuration.
* Correct constrain, round and similar macros. These implementations get rid of the bugs that can be caused if sideffects (eg, x++) are used in the arguments.

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
* Adapt Flash.h to support writing to the application section *without* the bootloader in use. (Restrictions apply, offer void where prohibited by lockbits)

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
