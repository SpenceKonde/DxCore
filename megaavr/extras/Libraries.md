# DxCore built-in libraries
The Dx-series parts have a lot of new capability and - compared to the "classic" AVR devices that many of us have become intimately familiar with - very different. In that (and most other) way, they have much in common with the rest of the "big three" post 2016 AVR product line cores. One of these ways is that they provide a large number of built-in libraries for interacting with the powerful on-chip peripherals, writing to the NVM, and interacting with commonly-used classes of devices; this is a list of the libraries. It is felt that this is preferable to separately installed libraries due to the importance of this functionality and the somewhat spotty support for these in the community at large.

## DxCore-specific
### DxCore
[DxCore Readme](../libraries/DxCore/README.md) The DxCore provides a wrapper around a couple of chip configuration functions that didn't seem appropriate in either the core proper, or in their own library. So far, it's just for

### Flash
1.3.0 introduced the ability to write to the application section on Optiboot boards by calling in to a stub of an entry point in Optiboot. I wasn't sure how to present this functionality, so there were two approaches to how to organize it, in [Flash](../libraries/Flash/README.md), and writeFlash; I have now decided to go with Flash, and in 1.3.1 is now handles non Optiboot configurations (you must enable flash write from app from the tools menu).

To avoid colliding with data and code placed by the compiler/linker, use the highest addresses first. If you have tons of PROGMEM_MAPPED and not much code, the section before the mapped progmem may have more space (again, fill from highest to lowest address).

## New Peripheral libraries
The AVR Dx-series is the latest and greatest from Microchip - in addition to enhancements ranging from pedestrian to world-changing to the peripherals we know and love, the new families of chips have brought with them a series of entirely new peripherals. Many of the most important new peripherals fit into one sort of mold: There are are multiple on the chip (often 2, 3, or some number of pairs), they have little - if any - internal state, the list of options and modes is short. The combination of a more streamlined hardware design and a library paradigm pioneered by @MCUdude make for a powerful set of libraries which do not limit how one can use the hardware; essentially all functionality is accessible, but the experience of working with the hardware is a lot better when you don't need the datasheet open in one window, and the IO headers open in another window to copy/paste the na,es of the defines from. Okay, okay, you may still need the datasheet open, but at least you won't need the headers too. Nobody misses the days of (1<<CAPITAL_LETTER)|(3<<OTHERLETTERS)

### Event
[Event Readme](../libraries/Event/README.md)The Event System on the modern AVR parts (which has largely been in its current form since the megaAVR 0-series - the slightly earlier tinyAVR 0/1-series have some strange quirks, and the whole design is less coherent (the phrase "public beta" comes to mind).  Many things on the chip can "generate" events - a timer might do so when it overflowed, a pin can generate an event based on whether it is high or low, and so on. And many things on the chip can "use" events, taking some action in response to them - the ADC could start a conversion, the type D timer can turn off it's PWM output, a type B timer could time how long the event was active for (in fact, that is the only way to do "input capture" with these parts - except now you can use any pin as your input, pre-process it with the logic library, and on). There are some planned enhancements on that way that we hope will make this more practical to work with.

### Logic
[Logic Readme](../libraries/Logic/README.md)The CCL (Configurable Custom Logic) strikes many people, at first glance, as a "multifunction logic IC built into the chip" and that's how many descriptions present it. While it can be used that way, if most of the inputs to your logic blocks are pin inputs, you're missing the point the CCLs. Up to two of the three inputs can be piped straight from the event system. Even without the sequential logic, the feedback channel can make one of them act as a "latch". In addition to their nominal purposes, the synchronizer and filter can be used as a "delay" when feedback is being used. They get a bunch of unique inputs including USART TX (hence you can use them to move the TX of a USART to an LUT output pin - combine with the IRCOM event user and a pin event generator to move both of them around limited only by available event channels! In master mode (only) MOSI and SCK are available as inputs to a Logic block - to a similar effect, except that you can't reroute the input.

### Comparator
[Comparator Readme](../libraries/Comparator/README.md) Like the classic AVRs, the modern ones have on-chip analog comparators (generally 1 or 3); you can use these to compare analog voltages and generate interrupts - or (of course) events in response to analog voltages crossing each other. The old trick of firing up a comparator with the negative end set to some mid-range reference voltage to generate an interrupt from the (digital) pin without fighting with some other library for the pin interrupt is, of course, still valid here too (and if anything calls attach interrupt, ever, .

### ZCD (Zero-crossing detector)
[ZeroCross Readme](../libraries/ZCD/README.md) The AVR Dx-series parts have up to three Zero-Crossing Detectors in hardware. These allow certain pins to be connected to an AC voltage (with - typically - the digital ground tied to the AC neutral, and a potentially much higher voltage (albeit protected with a resistor), to support applications like AC dimmers. Application notes from Atmel back in the day described an analogous setup that used just the GPIO and a large resistor. This solution is more accurate and more graceful. It does, however, require a level of care and attention to safety not typically needed in arduino projects if it is to be used to switch mains voltage (which is probably the most likely use of it. )

### Opamp
[Opamp Readme](../libraries/Opamp/README.md)The AVR DB-series parts introduce a new and exotic peripheral to the AVR product line: A trio (pair on the lower-pincount ones) of on-chip opamps, with software controlled multiplexers on their inputs and outputs. They can be used to buffer the DAC output, as a programmable gain amplifier for the ADC, and so on. My specialty is digital electronics, so I'm not qualified to give a more in-depth assessment, but my imprtession is that while it's no great shakes as far as opamps go, the biggest value of it is that it is tightly integrated with the microcontroller and is already present. It is also worth noting that they can give a great deal of control to the event system - the event system can really do everything except configure the multiplexer.... It's sort of like the analog counterpart to the CCL (Logic).

### USERSIG
(Not Yet Implemented) The AVR Dx-series parts have a User Signature Space ("USERROW") of 32 bytes, like all of the other modern AVRs - unfortunately while the previous ones wrote to it like EEPROM, these write to it like flash (that is, no erase granularity - only option for erasing is to erase the whole thing) so the EEPROM-like interface that we were able to provide on megaTinyCore would not go great. Assuming every time we wrote a byte we tried to preserve the rest of the contents, storing a long that overwrote another long would involve 4 erase/write cycles instead of one. This gets ugly very fast, which is why I haven't implemented it....

My thinking is that like, you'd need to have buffer in RAM, and a way to record at least 2 states: No unsaved data in buffer vs unsaved data in buffer, and maybe a third one indicating that it wasn't yet known. write() would return 0 or 1, Once there was unsaved data in the buffer, write would always return 0 and always write to the buffer. Otherwise, if newval & oldval == newval (ie, if we can write it without erasing), d and return 1. Otherwise, write to buffer, switch state and return 0. On that first write, it would read contents of USERSIG into buffer and set the state before doing that. And to make it stop returning 0, you might have three options:
USERSIG.load() - clear buffer and reload from USERROW
USERSIG.erase() - erase the USERROW AND the buffer
USERSIG.commit() - erase the USERROW and write the buffer to USERROW. Reset state of USERSIG libreary to in sync()

What would be really nice is a way to pass it any datatype.... but that's deep magic.


## Standard Arduino Libraries
These are the very-basic libraries that all Arduino-compatible board packages (with very few exceptions) implement and provide things everyone takes for granted and uses all the time (and which are used by many more libraries under the hood).


### EEPROM
[EEPROM readme](../libraries/EEPROM/README.md) This is the standard wrapper around interacting with the 512b on-chip EEPROM available on all DA-series and DB-series parts. The future DD-series parts will have 256b EEPROM. It replicates the standard API exactly. There are no special concerns - except that other libraries which build upon the EEPROM library may make assumptions about how EEPROM is implemented which are not compatible with these parts.

### SoftwareSerial
SoftwareSerial was - against my better judgement - brought over. It is unmodified from the version in the official megaavr core. Avoid using software serial wherever possible - these parts have between 3 and 6 hardware serial ports; a hardware serial port will always be less likely to cause problematic interactions. Just like on other devices, SoftwareSerial takes over ALL pin interrupts, because it calls `attachInterrupt()` (See [the interrupt reference](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Interrupts.md) - there is now a way around this in 1.3.7)

### SPI
[SPI.h readme](../libraries/SPI/README.md) The included version of SPI.h includes all the standard Arduino API functions, the `swap()` and `pins()` methods, and as of 1.3.0 supports using either SPI0 or SPI1 to increase the range of available pins without compromising compatibility. See the readme for details and the sordid story of why that was an issue. If you're not using the SPI1 pinsets, you could manually implement SPI slave on SPI1 while using SPI.h as master with the SPI0 pin sets.

### Wire
[Wire.h readme](../libraries/Wire/README.md) The included version of the Wire library provides TWI master or slave functionality; it provides all the functionality of the official Arduino Wire library - **EXCEPT** that it does not automatically enable the on-chip pull-up resistors like most parts and cores do; as of 1.3.0 we provide a Wire.usePullups() method to enable them. This version of Wire also adds the ability to respond to broadcast/general call messages and/or recognize a second address or mask some bits in the address, see the linked Wire readme for more information.

The version following the 1.3.7 release has contained a near total reimplementation of Wire.h with lower flash usage, dual mode support and support for the second TWI.

## Common Hardware
Two classes of hardware stand out for their popularity and (prior to these libraries) the poor quality of available libraries for post-2016 AVR microcontrollers

### Servo
The standard Arduino Servo library has a number of serious - and wholly unnecessaery - limitations. This was originally reimplemented for megaTinyCore, and then fixed in 2020 after it had been laid low by "bit rot". It removes dependence on the TCA0 timer prescaler, so changing the PWM frequency will not break PWM, and improves potential accuracy and lowers ISR execution time by moving away from digitalWrite(). If the Library Manager version of this library has been installed and is taking precedence over this one, change `#include <Servo.h>` to `#include <Servo_DxCore.h>`. The API is unchanged from what is describes in the [official Servo reference](https://www.arduino.cc/reference/en/libraries/servo/)

### tinyNeoPixel
[tinyNeoPixel documentation](tinyNeoPixel.md) This core includes the two versions of my tinyNeoPixel library - while it was originally developed by adapting adafruitNeoPixel to deal better with the flash and memory constraints of the tinyAVR classic line for ATTinyCore, the fact that the Dx-series parts have far more flash and memory does not require that one waste it with reckless abandon; the libraries are regularly updated with the latest enhancements from the Adafruit upstream. More importantly, the show() methods, written in inline assembly, have been adjusted to be compatible with the timing differences in the AVRxt variant of the AVR instruction set - and extended to theoretical clock speeds that we have no business hoping for from the AVR Dx-series.

The tinyNeoPixel_Static library expects the constructor to be passesd a `(uiunt8_t *)` which can hold the frame buffer. While the flash savings from losing malloc() and company probably aren't a big deal, with the likely use of longer strings LEDs, the fact that the dynamic allocation causes the memory used for the frame buffer to be hidden in the compiler memory usage is still a problem. Whether you've got 400 LEDs and some accessory buffers with 2k of ram, of 2000 LEDs and an animation scratch space just as large on a AVR128Dx, you still want to know how much memory you have left... With the static version, you will see that, because you declare the pixel buffer and hand it off, instead of letting the library malloc() it.

It looks possible to run two strings at once through a modification of the assembly code at least at 24+ MHz (ie, each "bit" would cover 2 port bits instead of 1) (in my use case, I intend to feed data into the middle of a string extending in both directions, displaying a unified animation.)
