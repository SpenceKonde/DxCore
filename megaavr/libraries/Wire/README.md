# Wire (TWI/I2C)
All of these parts have two hardware I2C (TWI) peripherals, except the 28-pin version, which has one. TWI0 works exactly like the one on official Arduino boards using the Wire.h library. There is at present no TWI1 library, see: https://github.com/SpenceKonde/DxCore/issues/54

## Pin Mappings
Like most peripherals, the TWI interface can be swapped to an alternate set of pins; this is configured using the Wire.swap() or Wire.pins() methods. Both of them achieve the same thing, but differ in how you specify the set of pins to use. This should be called **before** Wire.begin().


| Pin mapping | 64-pin | 48-pin | 32-pin  | 28-pin  | 20-pin | 14-pin | SDA | SCL | Dual SDA | Dual SCL |
|-------------|--------|--------|---------|---------|--------|--------|-----|-----|----------|----------|
| DEFAULT (0) |  YES   |  YES   |   YES   |   YES   |  YES   |  YES   | PA2 | PA3 | PC2      | PC3      |
| ALT_1   (1) |  YES   |  YES   |   YES*  |   YES*  |  YES*  |  YES*  | PA2 | PA3 | PC6      | PC7      |
| ALT_2   (2) |  YES   |  YES   |   YES*  |   YES*  |  YES*  |  YES*  | PC2 | PC3 | PC6      | PC7      |
| ALT_3   (3) |  NO    |  NO    | DD only | DD only |  YES   |  YES   | PA0 | PA1 | PC2      | PC3      |
`* indicates that the dual mode pins are unavailable which will be relevant if we ever support dual mode. `

`Wire.swap(pin_set)` will set the the pin mapping to the specified set of pins. Only 1-series parts with more than 8 pins support this; on other parts,  `Wire.swap()` will generate a compile error if a value known at compile time and not 0 is passed to it. On 1-series parts that do have an alternate pin mapping, a compile-time-known value that is not a 0 or 1 will similarly generate a compile error. An invalid value that is *not* known at compile time (in either case) will instead result in swap() returning false and selecting the default pins.

`Wire.pins(SDA pin, SCL pin)` - this will set the mapping to whichever mapping has the specified pins `SDA` and `SCL`. If this is not a valid mapping option, it will return false and set the mapping to the default. This uses more flash than Wire.swap(); that method is preferred. As with `Wire.swap()`, this will generate a compile error if the pins are compile-time-known constants which are not a valid SDA/SCL pair.

## Pullups
The I2C standard absolutely does require external pullups. The fact that I2C ever works with just the internal pullups is somewhat surprising - but the protocol is designed to be resiliant to mildly adverse hardware conditions. However, as wires get longer and/or slave devices more numerous, the bus capacitance increases and the internal pullups will no longer be sufficient for proper functioning. By popular demand we have added a method to enable the pullups; we suggest using this only as a debugging aid: *if this fixes any problems, you should install external pullup resistors* (in the absence of other devices on the bus having their own pullups breakout boards often do), 4.7k is a good default value for standard speed I2C). After selecting the desired pin mapping (if not the default) call:
```c++
Wire.usePullups();
```

## Wire.setClock()
`Wire.setClock()` is not exact (nor is it on the official core). The clock speed always depends on network conditions - the baud generator adapts to electrical conditions, particularly the rise time of the SCL and SDA line. `Wire.setClock()` must be called after `Wire.begin()`, and will briefly disable the TWI interface to make the necessary changes. It will handle the `FMPEN` bit to enable "Fast Mode Plus" for speeds in excess of 400 kHz.

Prior to 5/2021, `Wire.setClock()` did not work correctly and TWI baud rates could be wrong in either direction on any part. Thanks to the work of [@rneurink](https://github.com/rneurink) who saw my lousy "I give up" implementation after I botched it at least three times (all of which made it into a release) the baud calculations should now be reasonably accurate. Thanks to [@MX682X](https://github.com/MX682X), it no longer leaves the bus in a non-functional state when called after Wire.begin().

## Extra Features in Slave mode
In slave mode, it is  possible to respond to the general call (0x00) address as well (Thanks [@LordJakson](https://github.com/LordJakson)!). This is controlled by the optional second argument to Wire.begin(). If the argument is supplied amd true, general call broadcasts will also trigger the interrupt. These parts also support optionally having a second address, or masking (ignoring) some bits in the address. The optional third argument, if supplied, is passed unaltered to the TWI0.SADDRMASK register. If the low bit is 0, any bits set 1 will be ignored (masked off bits will be treated as matching). If the low bit is 1, it will instead act as a second address that the device can respond to. See the datasheet for more details. An everyday example of a part that does that sort of address masking is the 24-series of I2C EEPROMs (any 8-pin I2C EEPROM with 24 in the part number - many vendors make indistinguishable EEPROMs): When the size of the address of any cell within the EEPROM exceeds a multiple of 8 by 3 or less, it will use that many bits of the I2C address to address the memory; those bits are otherwise set by input pins. Hence with maximum options, the slave mode begin() call is:
```c++
Wire.begin(uint8_t address, bool receive_broadcast, uint8_t second_address)
```

## Dual Mode not supported
Dual mode is not supported by the Wire library; I am aware of no Arduino library implementations of dual mode. This may be implemented at some point in the future, but I do not expect to be able to get to it anytime soon. This is a more difficult task than most development work, and it's not clear to me how to extend the state machine model in the library to handle the same module acting as both master and slave. The hardware also supports master and slave on the *same* pins, but this is ALSO not supported by this library, for the same reason. I would be overjoyed to receive a pull request to add this functionality.

## The second TWI
Most of these parts have a second TWI interface. A Wire1 library that is compatible with any library that was compatible with Wire (and I mean that in both senses of "any") is possible, but I do not have the skill with C++ classes to do it. If you do and want to be a hero: [Issue #54](https://github.com/SpenceKonde/DxCore/issues/54)


## Errata warning
All modern AVRs, since the release of the first tinyAVR 0/1-series, through the AVR DB-series, have always had a silicon bug relating to the TWI pins. When the TWI peripheral takes control of the SCL and SDA, it correctly controls their being an INPUT or OUTPUT - but it fails to also take over the output value... That means that if the PORTx.OUT bit is 1 for either of the pins, it would be trying to drive the line high instead of low, and the I2C bus would be non-functional. As of 2.2.6, we always clear those bits in begin(); this was not done on older versions. In any event, do not `digitalWrite()` either of the pins  `HIGH` or set their `pinMode()` to `INPUT_PULLUP` after calling `Wire.begin()`. Calling them before that is pointless too, since they will be superseded by begin().
