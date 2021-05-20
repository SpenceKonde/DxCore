# Wire (TWI/I2C)
All of these parts have two hardware I2C (TWI) peripherals, except the 28-pin version, which has one. TWI0 works exactly like the one on official Arduino boards using the Wire.h library. There is at present no TWI1 library, see: https://github.com/SpenceKonde/DxCore/issues/54

## Pin Mappings
Like most peripherals, the TWI interface can be swapped to an alternate set of pins; this is configured using the Wire.swap() or Wire.pins() methods. Both of them achieve the same thing, but differ in how you specify the set of pins to use. This should be called **before** Wire.begin().

`Wire.swap(1) or Wire.swap(0)` will set the the mapping to the alternate (1) or default (0) pins. On parts which do not support an alternate pinout for this peripheral, `Wire.swap()` will generate a compile error if a value known at compile time and not 0 is passed to it; on parts which do, a compile-time-known value that is neither 0 nor 1 will similarly generate an error. An invalid value that is *not* known at compile time will instead result in swap() returning false and selecting the default pins.

`Wire.swap(1)` corresponds to the `ALT_2` pin mapping from the datasheet. `Wire.swap(0)` corresponds to the to the `DEFAULT` pin mapping. The `ALT_1` pin mapping is not supported, as it is the same as default except when dual mode is enabled.

`Wire.pins(SDA pin, SCL pin)` - this will set the mapping to whichever mapping has the specified pins `SDA` and `SCL`. If this is not a valid mapping option, it will return false and set the mapping to the default. This uses more flash than Wire.swap(); that method is preferred. As with `Wire.swap()`, this will generate a compile error if the pins are compile-time-known constants which are not a valid SDA/SCL pair.

## Pullups
The I2C standard absolutely does require external pullups. The fact that I2C ever works with just the internal pullups is somewhat surprising - but the protocol is designed to be resiliant to mildly adverse hardware conditions. However, as wires get longer and/or slave devices more numerous, the bus capacitance increases and the internal pullups will no longer be sufficient for proper functioning. By popular demand we have added a method to enable the pullups; we suggest using this only as a debugging aid: *if this fixes any problems, you should install external pullup resistors* (in the absence of other devices on the bus having their own pullups breakout boards often do), 4.7k is a good default value for standard speed I2C). After selecting the desired pin mapping (if not the default) call:
```
Wire.usePullups();
```

## Wire.setClock()
`Wire.setClock()` is not exact (nor is it on the official core; this implementation should do a better job of matching ther requested clock speed than ). The clock speed always depends on network conditions - the baud generator adapts to electrical conditions on the pins and - if they are rising more slowly than they should, it will run more slowly. To use speeds higher than 400 kHz, the main clock must be higher than 8 MHz. This fu

Prior to 5/2021, Wire.setClock() did not work correctly and TWI baud rates could be wrong in either direction on any part. In 1.3.6, this is impacted by a newly introduced bug which may prevent the TWI bus from functioning correctly when it is called after `Wire.begin()`

## Extra Features
As courtesey of https://github.com/LordJakson, in slave mode , it is now possible to respond to the general call (0x00) address as well. This is controlled by the optional second argument to Wire.begin(). If the argument is supplied amd true, general call broadcasts will also trigger the interrupt. These parts also support optionally having a second address, or allow you to mask (ignore) some bits in the address. The optional third argument, if supplied, is passed unaltered to the TWI0.SADDRMASK register. If the low bit is 0, any bits set 1 will be ignored (masked off bits will be treated as matching). If the low bit is 1, it will instead act as a second address that the device can respond to. Hence with maximum options, the slave mode begin() call is:

```
Wire.begin(uint8_t address, bool receive_broadcast, uint8_t second_address)
```

## Errata warning
All modern AVRs, since the release of the first tinyAVR 0/1-series, through the AVR DB-series, have always had a silicon bug relating to the TWI pins. When the TWI peripheral takes control of the SCL and SDA, it correctly controls their being an INPUT or OUTPUT - but it fails to also take over the output value... That means that if the PORTx.OUT bit is 1 for either of the pins, it would be trying to drive the line high instead of low, and the I2C bus would be non-functional. As of 2.2.6, we always clear those bits in begin(); this was not done on older versions. In any event, do not `digitalWrite()` either of the pins  `HIGH` or set their `pinMode()` to `INPUT_PULLUP` after calling `Wire.begin()`.

## Dual Mode not supported
Dual mode is not supported by the Wire library; I am aware of no Arduino library implementations of dual mode. This may be implemented at some point in the future, but I do not expect to be able to get to it anytime soon. This is a more difficult task than most development work, and it's not clear to me how to extend the state machine model in the library to handle the same module acting as both master and slave. The hardware also supports master and slave on the *same* pins, but this is ALSO not supported by this library, for the same reason. I would be overjoyed to receive a pull request to add this functionality. 

## The second TWI
Most of these parts have a second TWI interface. A Wire1 library that is compatible with any library that was compatible with Wire (and I mean that in both senses of "any") is possible, but I do not have the skill with C++ classes to do it. If you do and want to be a hero: [Issue #54](https://github.com/SpenceKonde/DxCore/issues/54)
