### [Wiring](Wiring.md)
### [Installation](Installation.md)
### [Making a cheap UPDI programmer](megaavr/extras/MakeUPDIProgrammer.md)

# DxCore - Arduino support for DA-series parts and future DB-series
This is an early version of an Arduino core to support the new AVR DA-series microcontrollers from Microchip. These are the latest and highest spec 8-bit AVR microcontrollers from Microchip. It's unclear whether these had been planned to be the "1-series" counterpart to the megaAVR 0-series, or whether such a thing was ever planned. But whatever the story of it's origin, these take the AVR instruction set to a whole new level.  With up to 128k flash, 16k SRAM, 55 I/O pins, 6 UART ports, 2 SPI and I2C ports, and all the exciting features of the tinyAVR 1-series and megaAVR 0-series parts like the event system, type A/B/D timers but for almost every major system, and enhanced pin interrupts... But for each system they've added A significant improvement of some sort. You liked the type A timer, but felt constrained by having only one prescaler at a time? Well now you have two of them (on 48-pin parts and up)! You wished you could make a type B timer count events? You can do that now! (this addresses something I always thought was a glaring deficiency of the new peripherals and event system). We still don't have more prescale options (other than having two TCA's to choose from) for the TCB - but you can now combine two TCB's into one, and use it to do 32-bit input capture. Time a pulse or other event up to approximately 180 seconds long... to an accuracy of 24th's of a microsecond! You thought events and the CCLs were really cool, but if only you could fire an interrupt from them? Yup, you can do that too now.

Oh and you wish you had a bit more accuracy on the ADC? Yup - this is a 12-bit ADC - and the DAC (oh yeah, it has one of those) is 10-bits instead of the 8 that they tinyAVR 1-series had!

As if that all isn't enough, there's a 28-pin version in a DIP package. The 28-pin version really doesn't show the full power of these parts, but it's far better than nothing for those who aren't comfortable with using surface mount parts and have been feeling left out of the party (of course, you can buy breakout boards in my Tindie store of all sizes, even the 64-pin ones!)

These parts depart from the naming scheme used for AVR devices in the past; these are named AVR followed by the size of the flash, in KB, followed by DA, then the number of pins. 128k parts were released first (unfortunately, with some rather brutal silicon errata - as these are their flagship parts, I am hopeful that we will see a fix sooner rather than later), followed by 32k; as of the end of July 2020, the 64k parts are not out yet. Note that the pin count also determines how many of certain peripherals the parts have available - parts with more pins have more peripherals to do things with those pins. 32k parts with 64 pins are not available.

This core was based on megaTinyCore; it is likely that the documentation still contains references to megaTinyCore or ATtiny/tinyAVR. Please report these (or better yet, fix and PR) if you find them.

## Supported Parts (click link for pinout diagram and details)
Support for smaller-flash versions is not available using the current arduino7 toolchain. We will be working to get an arduino8 toolchain built with the new ATpacks which will add support for these. But, if you hack your toolchain, they should "just work"!
* [AVR128DA28, AVR64DA28, AVR32DA28](megaavr/extras/DA28.md)
* [AVR128DA32, AVR64DA32, AVR32DA32](megaavr/extras/DA32.md)
* [AVR128DA48, AVR64DA48, AVR32DA48](megaavr/extras/DA48.md)
* [AVR128DA64, AVR64DA64](megaavr/extras/DA64.md)

My personal opinion is that the 48-pin parts are the "sweet spot" - they have the real gems of the product line - the second Type A timer, the two extra CCL LUTs, enough pins that these parts can start to stretch their legs (pins?). Most people can't really find something to do with a whole 64 pins in one project - short of indulging in kitchen-sink-ism just to take up pins. But the 27 I/O pins on the 32-pin parts can go faster than one might think (I had one project a while back where I switched to a '328PB instead of a '328P for the Rev. B, because otherwise I was 1 pin short of being able to lose the I2C backpack on the '1602 LCD, and if I did that, I could integrate the whole thing onto one PCB, and have a rigid connection between the LCD and main PCB - though I think I could just squeeze that project into a DA32).

## Upcoming DB-series
Support for the DB-series will be added pending release of a datasheet by Microchip (though will be gated by the compiler toolchain issue - I can work around that and provide a toolchain if need be, though I'd rather not have to take this step.
It is likely that basic support could be added almost immediately upon release - the DB-series appears to be nearly identical to the DA-series, except for some improved analog options (apparently - on-chip opamps, improved analog comparators), multi-voltage I/O (this is one port which has I/O on a different voltage than the rest of the chip - which the Microchip blurb said could be lower *or higher* than the chip; my betting is it will be PORTD, and power will be supplied on the pin marked AVDD). There are tantalizing hints in the io headers of an external high frequency crystal... but I'm not sure if it can actually be used to clock CPU itself, or just the PLL. The latter would be a real bummer to many people, who have loathed the demise of the external crystal (personally, I am happy with the accuracy I get from the internal clock - it's in a different world compared to the old days of the classic AVRs, especially with autotune).


## Supported Clock Speeds
All speeds are supported across the whole 1.8V ~ 5.5V operating voltage range!
* 24MHz Internal
* 20MHz Internal
* 16MHz Internal
* 12MHz Internal
*  8MHz Internal
*  4MHz Internal
*  1MHz Internal

There are multiple ways to generate some of the lower frequencies (do you prescale from higher frequency, or set the oscillator to the desired one? Suspect the latter is more pwoer efficient, but with the former you could still use the PLL while staying in spec - but are people going to want to?) - currently, we set the main oscillator to the desired frequency, however we may revisit this decision in the future; these are still early days.

These parts do not support using an external crystal like the classic AVR parts do, however the internal oscillator is tightly calibrated enough that the internal clock should work for UART communication, and an external watch crystal can be used to automatically tune the crystal frequency, a feature called Auto-Tune. An example to call it is shown below - it really is that easy...

```c

// settings should be one of
//  (CLKCTRL_CSUT_1K_gc), (CLKCTRL_CSUT_16K_gc), (CLKCTRL_CSUT_32K_gc) or (CLKCTRL_CSUT_64K_gc), corresponding to startup times (at 32.768khz, of 1/32nd of a second, 1/2 second, 1 second and 2 seconds.
//  optionally can be or'ed with CLKCTRL_LPMODE_bm (low power mode) or CLKCTRL_SEL_bm, if it's an external 32.768k CLOCK instead and/or CLKCTRL_RUNSTDBY if it should be kept running at all times, even when not requested or in standby sleep mode.
//  Useful if it's also used for keeping time in sleep.

uint8_t enableXOSC32K(uint8_t settings)
{
	_PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA,0x23);
}
// since CLKCTRL.MCLKSTATUS&CLKCTRL_XOSC32KS_bm won't be true until something requests that clock source, you have to actually enable autotune. This verification gives you some assurance that it's working. The internal oscillator is pretty damned good on these. In my tests, was within 1% at room temperature across the full operating voltage range, even without AutoTune. The clock accuracy is really limited only by the large tuning increment of the HF oscillator (spec'ed at 0.4%).

uint8_t enableAutoTune(uint16_t verifytime=0)
{
	_PROTECTED_WRITE(CLKCTRL.OSCHFCTRLA,CLKCTRL.OSCHFCTRLA|0x01);
	if (verifytime!=0) {
		uint32_t startedAt=millis();
		while ((millis()-startedAt < verifytime)&&(CLKCTRL.MCLKSTATUS&CLKCTRL_XOSC32KS_bm));
		if (CLKCTRL.MCLKSTATUS&CLKCTRL_XOSC32KS_bm){
			return 1;
		}
	}
	return 0;
}
```


## This is a VERY EARLY version of the core
This core is still in development. At this point, it is belived that the core is far enough along for people to begin playing with it *HOWEVER* it should not be considered production-ready. Please report problems or suspected bugs using the Issues feature above - or, if this is not possible, via email ( spencekonde@gmail.com ). Current status of core:
* Serial works
* Clock selection works
* PWM and DAC output works
* Digital I/O works
* Wire and SPI should work. The bug in
* F() macro and `__FlashStringHelper` are back :-( - I don't (yet) have a method worked out by which you can automatically put strings into flash (though I have an idea....)

## Programming is done via UPDI
Currently, the only programmer I know works with the core is jtag2updi. See [Making a cheap UPDI programmer](MakeUPDIProgrammer.md) for information on using an Arduino for this purpose - almost any Arduino board can be used, but the Arduino Nano 3.0 is preferred, as it is dirt cheap, and includes a USB-serial adapter. The versions that use an ATmega168 can also be used. An Arduino Pro Mini is also an excellent choice - but becomes physically awkward.

## Brutal errata in initial hardware
The silicon errata in the initial versions of these parts is pretty brutal - and it's not even complete! There are a few sweeteners though too... See [errata and extras](megaavr/extras/errata_and_extras.md).

## Quick Peripheral by Peripheral comparison
[Compared to tinyAVR 0/1-series and/or mega 0-series](megaavr/extras/Comparison.md)

# Features

### Memory-mapped flash? No :-(
Unlike the tinyAVR 0/1-series and megaAVR 1-series parts, which are able to map their entire flash to memory, the DA-series parts can only map 32KB at a time. The FLMAP bits in NVMCTRL.CTRLB control this mapping. Unfortunately, because this can be changed at runtime, the linker does not configure this. So, for the time being, you must use the F() macro and PROGMEM to put constants into program memory, and the pgm_read functions to read it.

In a future version, we plan to develop a solution to this that will allow up to 32kb of constants to be declared in a way that puts them in the final 32k section of flash, and the flash mapping will be pointed at that section... how easy this to be remains to be seen...

Note that the errata relating to the memory mapping is not a problem for us.

### Ways to refer to pins

This core uses a simple scheme for assigning the Arduino pin numbers, the same one that [MegaCoreX](https://github.com/MCUDude/MegaCoreX) uses for the pin-compatible megaAVR 0-series parts - pins are numbered starting from PA0, proceeding counterclockwise, which seems to be how the Microchip designers imagined it too.

#### PIN_Pxn Port Pin Numbers (recommended)
**This is the recommended way to refer to pins** Defines are also provided of form PIN_Pxn, where x is A, B, or C, and n is a number 0 ~ 7 - (Not to be confused with the PIN_An defines described below). These just resolve to the digital pin number of the pin in question - they don't go through a different code path or anything. However, they have particular utility in writing code that works across the product line with peripherals that are linked to certain pins (by Port), as most peripherals are. Several pieces of demo code in the documentation take advantage of this.  Direct port manipulation is possible on the megaavr parts - and in fact several powerful additional options are available for it - see [direct port manipulation](megaavr/extras/DirectPortManipulation.md).


#### Arduino Pin Numbers
When a single number is used to refer to a pin - in the documentation, or in your code - it is always the "Arduino pin number". These are the pin numbers shown on the pinout charts. All of the other ways of referring to pins are #defined to the corresponding Arduino pin number.

#### An and PIN_An constants
The core also provides An and PIN_An constants (where n is a number from 0 to 21). These refer to the ADC0 *channel* numbers. This naming system is similar to what was used on many classic AVR cores - on some of those, it is used to simplify the code behind analogRead() - but here, they are just #defined as the corresponding Arduino pin number. The An names are intentionally not shown on the pinout charts, as this is a deprecated way of referring to pins. However, these channels are shown on the pinout charts as the ADCn markings, and full details are available in the datasheet under the I/O Multiplexing Considerations chapter. There are additionally PIN_An defines for compatibility with the official cores - these likewise point to the digital pin number associated with the analog channel. Note that channel A0 is on the UPDI/Reset pin - however, even when configured as UPDI, it can be used as an input as long as the signals it can be exposed to do not look like the UPDI enable sequence.

### Serial (UART) Support
All of these parts have a several hardware serial ports (USART) - from 3 on the 28-pin parts to SIX on the 64-pin parts! They work exactly like the ones on official Arduino boards. See the pinout charts for the location of the serial pins. On my breakout boards, we provide autoreset support as well (again, just like official Arduino boards)

On all supported devices, where the appropriate pins are present, they can be pin-swapped - each PORT gets a USART, which defaults to pins 0 and 1 for RX, TX (2 and 3 for XCK and XDIR - though these are not supported through the Serial class), and 4, 5, 6 and 7 when pinswapped. This is configured using the Serial.swap() or Serial.pins() methods. Both of them achieve the same thing, but differ in how you specify the set of pins to use. This should be called **before** calling Serial.begin().

`Serial.swap(1) or Serial.swap(0)` will set the the mapping to the alternate (1) or default (0) pins. It will return true if this is a valid option, and false if it is not (you don't need to check this, but it may be useful during development). If an invalid option is specified, it will be set to the default one.

`Serial.pins(TX pin, RX pin)` - this will set the mapping to whichever mapping has the specified pins as TX and RX. If this is not a valid mapping option, it will return false and set the mapping to the default. This uses more flash than Serial.swap(); that method is preferred.

When operating at 1MHz, the UART can output 56700 baud, but not 115200 baud (115200 is within the capabilities of the hardware at 1 MHz - a future enhancement to the core will add support for U2X mode to support this baud rate.)

### SPI support
All of these parts have two hardware SPI ports. On parts with more pins, they can be pin-swapped to different sets of pins. It works exactly like the one on official Arduino boards using the SPI.h library. See the pinout charts for the location of these pins.

Pin selection for SPI is configured using the SPI.swap() or SPI.pins() methods. Both of them achieve the same thing, but differ in how you specify the set of pins to use. This should be called **before** calling SPI.begin().

`SPI.swap(1) or SPI.swap(0)` will set the the mapping to the alternate (1) or default (0) pins. It will return true if this is a valid option, and false if it is not (you don't need to check this, but it may be useful during development). If an invalid option is specified, it will be set to the default one.

`SPI.pins(MOSI pin, MISO pin, SCK pin, SS pin);` - this will set the mapping to whichever mapping has the specified pins. If this is not a valid mapping option, it will return false and set the mapping to the default. This uses more flash than SPI.swap(); that method is preferred.

This core disables the SS pin when running in SPI master mode. This means that the "SS" pin can be used for whatever purpose you want - unlike classic AVRs, where this could not be disabled.  It could be enabled and the SS pin configured appropriately (probably as INPUT_PULLUP) if master/slave functionality is required. **If you plan to enable SPI slave functionality in this way, on early revisions of the chip (including all released ones at the time of writing) you must switch the SPI pins to an alternate pin set**

### I2C (TWI) support
All of these parts have two hardware I2C (TWI) peripherals, except the 28-pin version, which has one. TWI0 works exactly like the one on official Arduino boards using the Wire.h library. Support for TWI1 is planned for a future version of this core as a Wire1 library with an otherwise identical interface. See the pinout charts for the location of the pins.

The TWI pins can be swapped to an alternate location; this is configured using the Wire.swap() or Wire.pins() methods. Both of them achieve the same thing, but differ in how you specify the set of pins to use. This should be called **before** Wire.begin().

`Wire.swap(1) or Wire.swap(0)` will set the the mapping to the alternate (1) or default (0) pins. It will return true if this is a valid option, and false if it is not (you don't need to check this, but it may be useful during development). If an invalid option is specified, it will be set to the default one.

`Wire.pins(SDA pin, SCL pin)` - this will set the mapping to whichever mapping has the specified pins as SDA and SCL. If this is not a valid mapping option, it will return false and set the mapping to the default. This uses more flash than Wire.swap(); that method is preferred.

As with megaTinyCore, courtesey of https://github.com/LordJakson, in slave mode, it is now possible to respond to the general call (0x00) address as well. This is controlled by the optional second argument to Wire.begin(). If the argument is supplied amd true, general call broadcasts will also trigger the interrupt. The version supplied with DACore also supports an optional third argument, which is passed unaltered to the TWI0.SADDRMASK register. If the low bit is 0, and bits set 1 sill cause the I2C hardware to ignore that bit of the address (masked off bits will be treated as matching). If the low bit is 1, it will instead act as a second address that the device can respond to. While these parts support "dual mode" allowing master and slave operation on different pairs of pins, like the megaAVR 0-series which also has this support in hardware, it is not currently exposed in Arduino; similarly, while these parts support master and slave simultaneously on the same pins, that is also not supported by the Arduino Wire library at this time. Work is ongoing to add support.

### PWM support
The core provides hardware PWM (analogWrite) support. On all parts, 6 pins (on PD0-PD5 for 28 and 32-pin parts, PC0-PC6 for 48 and 64-pin parts) - see the part specific documentation pages for pin numbers) provide 8-bit PWM support from the Type A timer, TCA0. On 48-pin and 64-pin parts, an addional 6 PWM pins are available on PB0-PB5 from TCA1. Additionally, Type B timers not used for other purposes (TCB2 is used for millis unless another timer is selected, and other libraries mmay use a TCB as well) can each support 1 8-bit PWM pin. The pins available for this are shown on the pinout charts. Aditionally, TCD0 provides two additional PWM pins on PA4 and PA5; Users may prefer to configure this manually - TCD0 is capable of, among other things, generating much higher frequency PWM, as it can be clocked from the PLL at 48MHz (or more, if you don't mind exceeding the specified operating specs - I've gotten it up to 96 MHz!).
**Note that TCA0, and TCA1 if present are configured by DACore in Split Mode by default, which allows them to generate 8-bit PWM output on 6 pins each, instead of three** See the
#### [Taking over TCA0](megaavr/extras/TakingOverTCA0.md)
**For general information on the available timers and how they are used PWM and other functions, consult the guide:**
#### [Timers and DxCore](megaavr/extras/PWMandTimers.md)


### NeoPixel (WS2812) support
The usual NeoPixel (WS2812) libraries have problems on these parts. This core includes two libraries for this, both of which are tightly based on the Adafruit_NeoPixel library. See the [tinyNeoPixel documentation](megaavr/extras/tinyNeoPixel.md) and included examples for more information. This is identical to the library provided with megaTinyCore.

### Tone Support
Support for tone() is provided on all parts using TCB0. This is like the standard tone() function; it does not support use of the hardware output compare to generate tones. Note that if TCB0 is used for millis/micros timing, tone() will generate an error; do not use TCB0 as the millis/micros timekeeping timer if your application requires tone().

### millis/micros Timekeeping Options
DACore provides the option to use any available timer on a part for the millis()/micros timekeeping, controlled by a Tools submenu - or it can be disabled entirely to save flash (or more likely available timers for manual configuration) and allow use of all timer interrupts. By default, TCB2 will be used by on parts. TCA0, TCA1 and any of the TCB's present on the part may be used, though this has not been rigorously tested; I suspect that TCD0 may not work in the initial release. Note that TCB0 conflicts with tone() and TCB1 conflicts with the version of Servo supplied with this core.

For more information, on the hardware timers of the supported parts, and how they are used by DxCore's built-in functionality, see the [Timers and DxCore](megaavr/extras/PWMandTimers.md)

### ADC Support
These parts have many ADC channels available - see the pinout charts for the specifics, they can be read with analogRead() like on a normal AVR. While the An constants (ex, A0) are supported, and refer to the corresponding ADC channel (not the corresponding pin number), using these is deprecated - the recommended practice is to pass the digital pin number to analogRead(). Analog reference voltage can be selected as usual using analogReference(). Supported reference voltages are:
* VDD (Vcc/supply voltage - default)
* INTERNAL1V024
* INTERNAL2V048
* INTERNAL4V096
* INTERNAL2V5
* EXTERNAL

The 1.024V, 2.048V, and 4.096V options are particularly convenient when measuring analog voltages, as the ADC readings can be expediently converted to millivolts.

Theee parts support **12-bit analog readings** - by default this core configures them to 10-bit mode for compatibility. To enable 12-bit readings, call `analogReadResolution(12)` - thereafter, analogRead() will return a number from 0 ~ 4095. It can be set back to 10-bit mode using `analogReadResolution(10)`

In addition to the pin numbers, you can read from the following sources:
* ADC_INTREF (The internal reference - you can set it manually via VREF.CTRLA, or by calling analogReference(), followed by analogReference(VDD) - The internal reference register is not changed when switching back to VDD as reference)
* ADC_DAC0 (The value being output by DAC0)
* ADC_TEMPERATUIRE (The internal temperature sensor)

We have taken  advantage of the improvements in the ADC on the these parts to improve the speed of analogRead() by more than a factor of three compared to the classic AVR devices - the ADC clock which was constrained to the range 50-200kHz - on these parts (as well as tinyAVR 0/1-series and megaAVR 0-series) it can be run at 1.5MHz! To compensate for the faster ADC clock, we set ADC0.SAMPCTRL to 14 to extend the sampling period from the default of 2 ADC clock cycles to 16 - providing the same sampling period as most other AVR cores, which should preserve the same accuracy when measuring high impedance signals. If you are measuring a lower impedance signal and need even faster analogRead() performance - or if you are measuring a high-impedance signal and need a longer sampling time, you can adjust that setting. On the tinyAVR and megaAVR 0/1-series, this had a maximum of 0x1F (31). On the DA-series, the maximum is 0xFF (255); this is almost certainly too long.

```
ADC0.SAMPCTRL=255; // maximum sampling length = 255+2 = 257 ADC clock cycles (0.26 milliseconds!)
ADC0.SAMPCTRL=0; //minimum sampling length = 0+2 = 2 ADC clock cycles
```

With the minimum sampling length, analogRead() speed would be approximately doubled from it's already-faster value.

**ERRATA ALERT** There is a nasty silicon bug, at least in early versions, where whatever pin the ADC multiplexer is pointed at, digital reads are disabled. This core provides two options for dealing with this - if you enable the workaround, the core will point the multiplexer at a non-pin reserved value after every analogRead(). If it is set to startup only, this will be done before setup() is called - but never after that; it is your responsability to deal with in that case.

### DAC Support
The DA-series parts have a 10-bit DAC which can generate a real analog voltage (note that this provides low current and can only be used as a voltage reference, it cannot be used to power other devices). This generates voltages between 0 and the selected VREF (unlike the tinyAVR 1-series, this can be Vcc!). Set the DAC reference voltage via the DACReference() function - pass it any of the ADC reference options listed under the ADC section above (including VDD!). This voltage must be lower than Vcc to get the correct voltages. Call analogWrite() on the DAC pin (PD6) to set the voltage to be output by the DAC (this uses it in 8-bit mode). To turn off the DAC output, call digitalWrite() on that pin.

To use it in 10-bit mode
```
//assumes dacvalue is an unsigned 16-bit integer containing a number between 0 and 1023.

// enable DAC
DAC0.CTRLA|=(1<<DAC_OUTEN_bm)|1<<(DAC_ENABLE);

// write value to DAC
DAC0.DATA=(dacvalue<<6);

// disable DAC
DAC0.CTRLA&=~((1<<DAC_OUTEN_bm)|1<<(DAC_ENABLE));

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
All pins can be used with attachInterrupt() and detachInterrupt(), on RISING, FALLING, CHANGE, or LOW. All pins can wake the chip from sleep on CHANGE or LOW. Pins marked as ASync Interrupt pins on the pinout chart (this is marked by an arrow where they meet the chip on those charts - pins 2 and 6 on all ports have this feature) can be used to wake from sleep on RISING and FALLING edge as well.

Advanced users can instead set up interrupts manually, ignoring attachInterrupt and manipulating the relevant port registers appropriately and defining the ISR with the ISR() macro - this will produce smaller code (using less flash and ram) and the ISRs will run faster as they don't have to check whether an interrupt is enabled for every pin on the port. For full information and example, see: [Pin Interrupts](megaavr/extras/PinInterrupts.md)

### Assembler Listing generation
Like my other cores, Sketch -> Export compiled binary will generate an assembly listing in the sketch folder.

### EESAVE configuration option
The EESAVE fuse can be controlled via the Tools -> Save EEPROM menu. If this is set to "EEPROM retained", when the board is erased during programming, the EEPROM will not be erased. If this is set to "EEPROM not retained", uploading a new sketch will clear out the EEPROM memory. You must do Burn Bootloader to apply this setting. Note that this only applies when programming via UPDI - in a future version when a bootloader is introduced, this setting will not effect it.

### BOD configuration options
These parts support multiple BOD trigger levels, with Disabled, Active, and Sampled operation options for when the chip is in Active and Sleep modes - Disabled uses no power, Active uses the most, and Sampled is in the middle. See the datasheet for details on power consumption and the meaning of these options. You must do Burn Bootloader to apply this setting.

### Link-time Optimization (LTO) support
This core *always* uses Link Time Optimization to reduce flash usage - all versions of the compiler which support the DA-series parts also support LTO, so there is no need to make it optional as was done with ATtinyCore.

### Identifying menu options within sketch
It is often useful to identify what options are selected on the menus from within the sketch; this is particularly useful for verifying that you have selected the options you wrote the sketch for when you open it; however, note that as of 2.0.1, almost all of these options have been removed except for the millis timer option.

##### Millis timer
The option used for the millis/micros timekeeping is given by a define of the form USE_MILLIS_TIMERxx. Possible options are:
* MILLIS_USE_TIMERA0
* MILLIS_USE_TIMERA1
* MILLIS_USE_TIMERB0
* MILLIS_USE_TIMERB1
* MILLIS_USE_TIMERB2
* MILLIS_USE_TIMERB3 (48 and 64-pin parts only)
* MILLIS_USE_TIMERB4 (64-pin parts only)
* MILLIS_USE_TIMERD0
* DISABLE_MILLIS

##### Using to check that correct menu option is selected
If your sketch requires that the B0 is used as the millis timer, for example:

```
#ifndef MILLIS_USE_TIMERB2
#error "This sketch is written for use with TCB0 as the millis timing source"
#endif
```

### Identifying part family within sketch
When writing code that may be compiled for a variety of target chips, it is often useful to detect which chip it is running on. Defines of the form `__AVR_AVRxDAy__` are provided, where `x` is the size of the flash (in kb) and `y` is the number of pins, for example `__AVR_AVR128DA64__` for the part with 128K flash and 64 pins.

This core provides an additional define depending on the number of pins on the part, as this directly corresponds to the available peripherals:
* __AVR_DA_28__
* __AVR_DA_32__
* __AVR_DA_48__
* __AVR_DA_64__

And all parts will have the following defined:
* __AVR_DA__
* DxCORE


# Bootloader (optiboot) Support
Planned for a future version.

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
* Tools -> Retain EEPROM - determines whether to save EEPROM when uploading a new sketch. You must burn bootloader after changing this to apply the changes. This option is not available on Optiboot board definitions - programming through the bootloader does not execute a chip erase function.
* Tools -> B.O.D. Voltage - If Brown Out Detection is enabled, when Vcc falls below this voltage, the chip will be held in reset. You must burn bootloader after changing this to apply the changes. Take care that these threshold voltages are not exact - they may vary by as much as +/- 0.3v! (depending on threshold level - see electrical characteristics section of datasheet). Be sure that you do not select a BOD threshold voltage that could be triggered during programming, as this can prevent successful programming via UPDI (reported in #86).
* Tools -> Reset pin - This menu option can be set to Reset (default) or Input; the latter allows this pin to be used as a normal input.
* Tools -> B.O.D. Mode (active) - Determines whether to enable Brown Out Detection when the chip is not sleeping. You must burn bootloader after changing this to apply the changes.
* Tools -> B.O.D. Mode (sleep) - Determines whether to enable Brown Out Detection when the chip is sleeping. You must burn bootloader after changing this to apply the changes.
* Tools -> millis()/micros() - If set to enable (default), millis(), micros() and pulseInLong() will be available. If set to disable, these will not be available, Serial methods which take a timeout as an argument will not have an accurate timeout (though the actual time will be proportional to the timeout supplied); delay will still work, though it's done using delayMicroseconds(), so interrupts are disabled for 1ms at a time during the delay, and any interrupts that happen during the delay will add to the length of the delay. Depending on the part, options to force millis/micros onto any timer on the chip are also available from this menu.
* Tools -> ADC Workaround - If set to Enable, the workaround for the ADC silicon errata will be used, and ADC mux will be pointed away from pins after analogRead(). If set to Startup Only, this is only done at startup; digitalRead() will not work on the last pin you used analogRead() with.


# Buying AVR DA-series breakout boards
I sell breakout boards with regulator, UPDI header, and Serial header and other basic supporting parts in my Tindie shop, as well as the bare boards. Buying from my store helps support further development on the core, and is a great way to get started using these exciting new parts with Arduino. Note that we do not currently sell a 28-pin version - this did not seem like a compelling part with the availability of the 32-pin version; the main appeal of the 28-pin part is that it is available in a through-hole version. As we would not be able to make the 28-pin version significantly smaller, there did not seem to be a compelling reason to create a 28-pin version. We may revisit this decision in the future, including potentially a 28-pin bare board for the through-hole version, suitable for assembly by those not experienced with drag soldering.
(The boards are not for sale yet at the time of release, as they need to be "approved" by Tindie, and it's a weekend...)

### [Assembled Boards](https://www.tindie.com/products/21007/)
### [Bare Boards](https://www.tindie.com/products/21008/)

