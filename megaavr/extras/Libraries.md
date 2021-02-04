# DxCore built-in libraries
The Dx-series parts have a lot of new capability and - compared to the "classic" AVR devices that many of us have become intimately familiar with - very different. In that (and most other) way, they have much in common with the rest of the "big three" post 2016 AVR product line, Thprovides a large number of built-in libraries for interacting with the powerful on-chip peripherals, writing to the NVM, and interacting with commonly-used classes of devices; this is a list of them.

## DxCore-specific
### DxCore
[DxCore Readme](../libraries/DxCore/README.md) The DxCore provides a wrapper around a couple of chip configuration functions tha didn't seem appropriate in either the core proper, or in their own library. So far, it's just fo

### Flash or flashWrite
1.3.0 introduces the ability to write to the application section on Optiboot boards by calling in to a stub of an entry point in Optiboot. I wasn't sure how to present this functionality, so there are two approaches to how to organize it, in [Flash](../libraries/Flash/README.md), and in [writeFlash](../libraries/flashWrite/README.md). Please let me know which one you like better, and how I could improve the overall experience. Future updates are expected to bring the ability to do this without using a bootloader too!

## New Peripheral libraries
The AVR Dx-series is the latest and greatest from Microchip - in addition to enhancements ranging from pedestrian to world-changing to the peripherals we know and love, the new families of chips have brought with them a series of entirely new peripherals. Many of the most important new peripherals fit into one sort of mold: There are are multiple on the chip (often 2, 3, or some number of pairs), they have little - if any - internal state, the list of options and modes is short. The combination of a more streamlined hardware design a library paradigm pioneered by @MCUdude,

### Event
[Event Readme](../libraries/Event/README.md)The Event System on the modern AVR parts (which has largely been in it's current form since the megaAVR 0-series - the slightly earlier tinyAVR 0/1-series have some strange quirks, and the whole design is less coherent (I guess that was the public beta).  Many things on the chip can generate events - a timer might do so when it overflowed, a pin can generate an event based on whether it is high or low, and so on. And many things on the chip can "use" events, taking some action in response to them - the ADC could stat a conversion, the type D timer can turn off it's PWM output, a type B timer could time how long the event was active for (in fact, that is the only way to do "input capture" with these parts - except now you can use any pin as your input, pre-process it with the logic library, and on. It is far more powerful  and it's a helpful model to understabd wgat ut

### Logic
[Logic Readme](../libraries/Logic/README.md)The CCL (Configurable Custom Logic) strikes many people, at first glance, as a "multifunction logic IC built into the chip" and that's how many descriptions present it. While it can be used that way, if most of the inputs to your logic blocks are pin inputs, you're missing the point the CCLs. Up to two of the three inputs can be piped straight from the event system. Even without the sequential logic, the feedback channel can make one of them act as a "latch". In addition to their nominal purposes, the synchronizer and filter can be used as a "delay" when feedback is being used.

### Comparator
[Comparator Readme](../libraries/Comparator/README.md) Like the classic AVRs, the modern ones have on-chip analog comparators (generally 1 or 3); you can use these to compare analog voltages and generate interrupts - or (of course) events in response to analog voltages crossing eachother. The old trick of firing up a comparator with the negative end set to some mid-range reference voltage to generate an interrupt from the (digital) pin without fighting with some other library for the pin interrupt is, of course, still valid here too.

### ZCD (Zero-crossing detector)
[ZeroCross Readme](../libraries/ZCD/README.md) The AVR Dx-series parts have up to three Zero-Crossing Detectors in hardware. These allow certain pins to be connected to anb AC voltage (with - typically - the digital ground tied to the AC neutral, and a potentially much higher voltage, to support applications like AC dimmers.

### Opamp
(Not Yet Implemented) The AVR DB-series parts introduce a new and exotic peripheral to the AVR product line: A trio (fewer on rthe lower-pincount ones) of on-chip opamps, with software controlled multiplexetrs on their inputs and outpuds.They can be used to buffer the DAC output, as a programmable gain amplifier for the ADC, and so on. My specialty is digital electronics, so I'm not qualified to give a more in-depth assessment, but my imprtession is that while it's no great shakes as far as opamps go, the biggest value of it is that it is tightly integrated with the microcontroller and is already present. It is also worth noting that they can give a great deal of control to the event system

### USERSIG
(Not Yet Implemented) The AVR Dx-series parts have a User Signature Space ("USERROW") of 32 bytes, like all of the other modern AVRs - unfortunately while the previous ones wrote to it like EEPROM, these write to it like flash (that is, no erase granularity - only option for erasing is to erase the whole thing) so the EEPROM-like interface that we were able to provide on tinyAVR is not going to happen here. W

## Standard Arduino Libraries
These are the very-basic libraries that all Arduino-compatible board packages (with very few exceptions) implement and provide things everyone takes for granted and uses all the time (and which are used by many more libraries under the hood).


### EEPROM
[EEPROM readme](../libraries/EEPROM/README.md) This is the standard wrapper around interacting with the 512b on-chip EEPROM available on all DA-series and DB-series parts. The future DD-series parts will have 256b EEPROM. It replicates the standard API exactly. There are no special concerns - except that other libraries which build upon the EEPROM library may make assumptions about how EEPROM is implemented which are not compatible with these parts.

### SoftwareSerial
SoftwareSerial was - against my better judgement - brought over. It is unmodified from the version in the official megaavr core. Avoid using software serial wherever possible - these parts have between 3 and 6 hardware serial ports; a hardware serial port will always be less likely to cause problematic interactions. Just like on other devices, SoftwareSerial takes over ALL pin interrupts, because it calls `attachInterrupt()` (more of the blame rests with the abominable implementation of `attachInterrupt()`, really for no reason other than that attaching one interrupt to one port takes over all port interrupts on all ports.). Note that I hope to introduce a better software serial implementation in the somewhat near future.

### SPI
[SPI.h readme](../libraries/SPI/README.md) The included version of SPI.h includes all the standard Arduino API functions, the `swap()` and `pins()` methods, and as of 1.3.0 supports using either SPI0 or SPI1 to increase the range of available pins without compromising compatibility. See the readme for details and the sordid story of why it's an issue.

### Wire
[SPI.h readme](../libraries/Wire/README.md) The included version of the Wire library provides TWI master or slave functionality; it provides all the functionality of the official Arduino Wire library - **EXCEPT** that it does not automatically enable the on-chip pull-up resistors like most parts and cores do; as of 1.3.0 we provide a Wire.usePullups() method to enable them. This version of Wire also adds the ability to respond to broadcast/general call messages and/or recognize a second address or mask some bits in the address, see the linked Wire readme for more information.

### Wire1
This library was intended to provide a compatible interface to the second TWI peripheral, with the same behavior as the first. Due to poor understanding of how classes were to be handled, this operation was bungled. The creation of this library was rushed and mishandled. It is not compatible because the type of Wire1 (TwoWire1) does not match what libraries would require (TwoWire). See Issue #54.


## Common Hardware
Two classes of hardware stand out for their popularity and (prior to these libraries) the poor quality of available libraries for post-2016 AVR microcontrollers

### Servo
The standard Arduino Servo library has a number of serious - and wholly unnecessaery - limitations. This was originally reimplemented for megaTinyCore, and then fixed in 2020 after it had been laid low by "bit rot". It removes dependence on the TCA0 timer prescaler, so changing the PWM frequency will not break PWM, and improves potential accuracy and lowers ISR execution time by moving away from digitalWrite(). If the Library Manager version of this library has been installed and is taking precedence over this one, change `#include <Servo.h>` to `#include <Servo_DxCore.h>`. The API is unchanged from what is describes in the [official Servo reference](https://www.arduino.cc/reference/en/libraries/servo/)

### tinyNeoPixel
[tinyNeoPixel documentation](tinyNeoPixel.md) This core includes the two versions of my tinyNeoPixel library - while it was originally developed by adapting adafruitNeoPixel to deal better with the flash and memory constraints of the tinyAVR classic line for ATTinyCore, the fact that the Dx-series parts have far more flash and memory does not require that one waste it with reckless abandon; the libraries are regularly updated with the latest enhancements from the Adafruit upstread. More importantly, the show() methods, written in inline assembly, have been adjusted to be compatible with the timing differences in the AVRxt variant of the AVR instruction set - and extended to theoretical clock speeds that we have no business hoping for from the AVR Dx-series.

The tinyNeoPixel_Static library expects the constructor to be passesd a `(uiunt8_t *)` which can hold the frame buffer. While the flash savings from losing malloc() and company probably aren't a big deal, with the likely use of longer strings LEDs, the fact that the dynamic allocation causes the memory used for the frame buffer to be hidden in the compiler memory usage is still a problem. Whether you've got 400 LEDs and some accessory buffers with 2k of ram, of 2000 LEDs and an animation scratch space just as large on a AVR128Dx, you still want to know how much memory you have left... With the static version, you will see that, because you declare the pixel buffer and hand it off, instead of letting the library mallod() it.

It looks possible to run two strings at once through a modification of the assembly code at least at 24+ MHz (ie, each "bit" would cover 2 port bits instead of 1) (in my use case, I intend to feed data into the middle of a string extending in both directions, displaying a unified animation.)
