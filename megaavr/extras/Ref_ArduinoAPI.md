# Details of API function implementation
In many cases the types of return values and input values, and the details of which argument values are accepted and how invalid values are handled is not clearly defined by the Arduino API reference. This is unfortunate at one level. On the other hand, these ambiguities are typically handled in perverse ways by the standard Arduino implementations; were they explicitly stated, we would have less of an excuse to implement them in a more coherent (and efficient) manner - but they're not, so we have a bit of wiggle room.

**This document is a work in progress**

* Digital pin numbers are always represented as an 8-bit value that is not higher than 127. A `uint8_t` is used internally, but since only values under 128 are used, `int8_t` is equivalent. *The Arduino API does not specify a type, and standard implementation uses an int, which is less efficient, and there has never been an AVR device with more than 100 physical pins, and there is little risk of such a device being released*
  * Values above 127 are used internally by DxCore for analog channels, since the numeric values of analog channels overlap with those of digital pins. They should not be used by user code, and specifying them is not valid for digital functions on DxCore or megaTinyCore (this is handled differently on ATTinyCore, which is beyond the scope of this document).
  * `PIN_Pxn` where x is a port letter, and n the number of a pin within that port, is defined for all pins that exist. The inverse (that it is not defined for pins that do not exist) should not be assumed, see the pin mapping methodology section below. *The Arduino API provides no facility for referencing pins by port and bit. This makes interpreting non-Arduino resources, such as the datasheets, more difficult. It is the opinion of this writer that assigning arbitrary numbers to pins was one of the cardinal sins of the Arduino API. We discourage use of numeric values to directly reference pins, as it introduces unnecessary code portability issues*
  * `Px` where x is a port letter is defined for all ports. The numeric values start at 0 for PA and count upwards. This is returned by `digitalPinToPort()` and accepted by a number of macros. *standard behavior but undocumented, and varies for classic AVRs, and obviously for different architectures. But numeric values shouldn't be used.*
  * `NOT_A_PIN` is a special value returned by the "pin information macros" (unofficial term). It has a value of 255 (`uint8_t`) or -1 (`int8_t`). *This is standard Arduino behavior which is not documented except in the source code - though the official implementations, of course, use 16-bit ints instead of 8-bit ones.*
    * When checking for invalid pins compare the values from those macros with `NOT_A_PIN`, not with 255 or -1. The nature of macros can result in surprising behavior due to the way that datatypes are treated (or rather, completely ignored) by the preprocessor.
    * `NOT_A_PORT`, `NOT_AN_INTERRUPT` and `NOT_A_MUX` exist for the same purpose. They have the same value - they exist just to make it more obvious to a reader of the code what is meant. If a new system is added which requires a placeholder for invalid values of a different sort, more of these may be added. *not all of these exist in the standard core; NOT_A_MUX is definitely not in standard cores*
    * The digitalPinToPort macro **is not guaranteed to return NOT_A_PORT for pins that do not exist** The other pin information macros, however, will return NOT_A_PIN if the pin never exists on a specified part. See the pin mapping strategy section below to see when this matters. *The macro is entirely undocumented. The official cores are "board targeted" not "chip targeted". There is thus no situation that could arise with them where there are pins that are nominally valid pins, but which cannot be used for I/O because they have dedicated functions like reset or taking an external clock. Additionally no official Arduino pinmappings, to my knowledge, contain "holes" in the numbering - which is what presents the issue here.*
* All three of the digitalI/O functions take `uint8_t` arguments, and digitalRead returns a uint8_t that is either 1 or 0. HIGH (1), LOW (0), CHANGE(4), INPUT (1), OUTPUT (0), and INPUT_PULLUP (2) are simply #defined as appropriate values. *the official classic AVR cores take `int` arguments and return `int` values. The official modern AVR core uses enums to define those values. That had a benefit in that it prevented people from using numbers. But obviously it broke TONS of code. So they put in a compatibility layer that let you pass numbers in. **thus destroying the only benefit of the enums entirely**. We have no plans to drink from that bowl of koolaid.*
  * **With the exception of HIGH and LOW, we do not guarantee that the numeric values will never change.** You should always use the names in your code, not numeric values. *The official API does not say anything about the values of these constants*
  * **The case of HIGH and LOW is special**; there is an absolutely massive amount of code online that assumes that LOW == 0 == false and HIGH == 1 == true, and that you can do things like 'if (digitalRead(buttonPin)) ...' to test if a a pin is high. This core will never change the values of those constants, however, we suggest that users write code as though we might (ie, **just use HIGH and LOW, not 0 and 1 goddamnit**).
  * Do not forget that `4 = CHANGE` - while using 0 and 1 in calls to digitalWrite is acceptable, if poor practice, passing any other number (except as a named constant) is not a good idea.  4 will be treated as `CHANGE` while any other number will be treated as `HIGH` - which is likely not what you expected. *the official core does not use CHANGE in this way*
* The other constants for API functions are also #defined *they are enums on the official modern AVR cores, but macros on the official classic AVR cores*.

```c
#define LOW              0    /* used for digitalWrite(), digitalRead(), openDrain() and attachInterrupt() */
#define HIGH             1    /* used for digitalWrite(), digitalRead(). There is no option for HIGH level interrupt provided by the hardware */
#define FALLING          2    /* used for attachInterrupt() */
#define RISING           3    /* used for attachInterrupt() */
#define CHANGE           4    /* used for attachInterrupt() */
#define FLOATING         HIGH /* Not in official API - used for openDrain, while making clear what you're doing */
#define INPUT            0    /* used for pinMode() */
#define OUTPUT           1    /* used for pinMode() */
#define INPUT_PULLUP     2    /* used for pinMode() */
#define LSBFIRST         0    /* used for shiftIn/shiftOut */
#define MSBFIRST         1    /* used for shiftIn/shiftOut */
```
* Notice that the actual values of the input sense configuration bitfield dont match the constants used above; this is accounted for in WInterrupts.c

## DxCore/megaTinyCore extensions
More detail and information on functions provided that aren't part of the official API can be found on the other reference pages. This just covers things that don't really fit anywhere else.

## Pin Mapping Strategy
It is important to understand the strategy used to map pins provided by the hardware to pin identifiers in the code (either numbers, or PIN_Pxn macros, the latter being recommended), because there are several instances where it becomes relevant that do not exist on the official cores. Pins are numbered starting from PA0, and proceeding counterclockwise. On all DxCore parts, pins are never physically out of order, so if you were to walk along the pins, the port numbers would always progress forwards through the alphabet, and the numbers of successive pins within their port will never decrease except when transitioning to a new port. Let's all thank Microchip for that - it is a welcome change. That said, not all parts possess pins in all ports. As you walk around a 32-pin part, for example, you will pass PA7 immediately followed by PC0 - there's no PORTB (or PORTE or PORTG) on 32-pin parts. Ports can have up to 8 pins each, but they may have fewer. The way the pin numbering works here not entirely consistent, and this is not considered a bug. It was a well reasoned and deeply considered decision.
* If no pins within a port exist, no numbers are skipped for it.
* If the port has fewer than 8 pins, no numbers are skipped after the last pin within the port (so PC3 will be digital pin 11, and will be followed by PD0 as digital pin 12 on parts with 28 or 32 pins).
  * Exception: The first 8 numbers are always reserved for pins on PORTA. These pins are always present, except on 14-pin parts. On 14-pin parts, however, the 6 absent pins on PORTA still occupy numbers. pins 2-7 will return NOT_A_PIN when pin information macros are called for them.
* Occasionally, pins at the beginning or middle of a port may not exist **these numbers are skipped** - this is most notably PD0 on the DB-series parts with fewer than 48 pins, plus PC0 on parts with fewer than 28 pins.
  * *PIN_Px0 will always be defined if any pin on that port which is not nominally reset or the UPDI pin exists*. These are hereafter referred to as "phantom pins" and are treated differently that ordinary "skipped pins".
  * Other skipped pins will not have their PIN_Pxn macros defined
* All parts have one or more pin which may or may not exist from the perspective of the software - that is to say, a pin which might under some circumstances be a usable I/O pin may not be due to the current configuration (example: Crystal pins, Reset, UPDI). The PIN_Pxn macros for these will exist. We attempt to recognize these cases, and have the pin information macros return NOT_A_PIN for them. This filter is incomplete though, as we must be able to perform that test at compile time. Thus since the clock source is defined by the tools submenu, we can NOT_A_PIN PA0 (pin 0), and PA1 (pin 1) if the sketch was compiled for external clock or crystal (DxCore does not support switching to or from a clock source except during initialization, for a number of good reasons), but we can't do that for PIN_PA7 if it is set to output the system clock. In that case, though the pin is obviously not available for digital I/O, the pin information macros will return the same values for it.
  * This is a code speed-and-size tradeoff, since we aren't able to know whether that pin is available without reading a `volatile` special function register. The same is true of PF0 and PF1 when they are used for a 32 kHz crystal, and many other pins when a peripheral overrides the pin function.
  * This is also true of PF6 when used as reset and PF7 when used as UPDI (on parts that support using it as I/O instead - on parts that don't, we obviously know that there is never a PIN_PF7, and in fact don't define PIN_PF7 either). Compiled binaries can be exported and uploaded to parts with fuses configured differently, so this would again involve a read from an SFR.
  * Thus: PIN_PA0 and PIN_PA1 are always defined, but will return NOT_A_PIN when the pin information macros are called for them if we are using them for the clock. Otherwise, if the pin is present and potentially usable as an I/O pin, it's PIN_Pxn macro will be defined, and the pin information macros will give values as normal. The digital pin functions will thus try to read or write them - however, they will not be able to read values from or write to them.

| Pins         | Condition      | PIN_Pxn macro | digitalPinToPort | Other info macros | Number       |
|--------------|----------------|---------------|------------------|-------------------|--------------|
| PA0          | extclk or xtal | defined       | PA               | NOT_A_PIN         | skipped      |
| PA1          | xtal           | defined       | PA               | NOT_A_PIN         | skipped      |
| PA2-7        | 14-pin         | undefined     | NOT_A_PORT       | NOT_A_PIN         | skipped      |
| PA7          | clkout         | defined       | PA               | Normal            | not skipped  |
| PB0/6-7      | < 48/64 pins   | undefined     | N/A - no way to refer to pin | N/A   | not skipped  |
| PC0          | 14/20 pins     | defined       | PC               | NOT_A_PIN         | skipped      |
| PC4-7        | < 48 pins      | undefined     | N/A - no way to refer to pin | N/A   | not skipped  |
| PD0          | MVIO, <48 pins | defined       | PD               | NOT_A_PIN         | skipped      |
| PD1-3        | 14/20 pins     | undefined     | NOT_A_PORT       | NOT_A_PIN         | skipped      |
| PE0/4-7      | < 48/64 pins   | undefined     | N/A - no way to refer to pin | N/A   | not skipped  |
| PF0, PF1     | 32kHz xtal     | defined       | PF               | Normal            | skipped      |
| PF0/2-5      | < 28/32 pins   | undefined     | N/A - no way to refer to pin | N/A   | not skipped  |
| PF6          | Reset as input | defined       | PF               | Normal            | skipped      |
| PF7          | DA or DB       | undefined     | N/A - no way to refer to pin | N/A   | not skipped  |
| PF7          | DD and later   | defined       | PF               | Normal            | skipped      |
| PG0-7        | < 64 pins      | undefined     | N/A - no way to refer to pin | N/A   | N/A          |

"skipped" means that the number is NOT used for another pin. "not skipped" means that the number this pin might otherwise get will instead go to the next pin that is present.
* This all gets important in a few cases:
  * When we can tell at compile time that you're writing to or reading from a pin that the part doesn't have, we stop compilation and inform you. *the official core marches onward, outputting a binary that writes bits that have no effect, giving no indication that anything is amiss*
  * Some peripherals, notably TCA0, can be mapped to every port. It is thus useful to be able to figure out a port's pin 0 (loop over the pins from PIN_PA0 to NUM_DIGITAL_PINS, until the first one that returns the right port from digitalPinToPort() - that will always be pin 0 of that port *on DxCore*. The same may be applicable for other peripherals where each instance can be said to have a "home port" - like how LUT0's home port is PORTA. digitalPinToBitMask or digitalPinToBitPosition(PIN_Px0 + n) will return NOT_A_PIN if PIN_Pxn does not exist.

| Peripheral  | Home Port | Exclusive |
|-------------|-----------|-----------|
| USART0      | PA        | DA/DB     |
| USART1      | PC        | DA/DB     |
| USART2      | PF        | Yes       |
| USART3      | PB        | Yes       |
| USART4      | PE        | Yes       |
| USART5      | PG        | Yes       |
| CCL LUT0    | PA        | Yes       |
| CCL LUT1    | PC        | Yes       |
| CCL LUT2    | PD        | Yes       |
| CCL LUT3    | PF        | Yes       |
| CCL LUT4    | PB        | Yes       |
| CCL LUT5    | PG        | Yes       |
| TWI0        | PA, PC    | No        |
| TWI1        | PB/PC/PF  | No        |
| All ACs     | PD        | No        |
| OPAMP0      | PD        | Yes       |
| OPAMP1      | PD        | Yes       |
| OPAMP2      | PE        | Yes       |

Here exclusive means all of it's pin options are on that port - CCL LUT3 is exclusively available on PORTF, while TWI0 is not exclusive, because it can use pins on either PORTA or PORTC. The AC's "live" in PORTD, but depending on the specific part, they have varying levels of opportunity to reach beyond that port, though PORTD comprises the majority of their mapping options.


## Datatypes
* `byte`, `word`, and `boolean` are Arduino names for `uint8_t`, `uint16_t`, and `bool`. We provide these typedefs, but recommend against their use. You may have noticed in nearly all code and examples, I almost never use `int`, always `int16_t` - I In fact, I try to avoid naming any type by the names that do not explicitly specify it's size, to guarantee that there will be no surprises.
