# Improved Digital I/O Functionality
This core includes a number of features to provide more control or performance when doing digital I/O. This page describes how to use them. All of the options that can be configured for a pin are exposed. The only things that aren't exposed are the slew rate limiting feature, and the multi-pin configuration facilities. The slew rate limiting is can only be configured on a per port basis; trning it on and off is so simple (see below) that it needs no wrapper. The multi-pin configuration system does not have an obvious "right way" to expose it and should be handled directly by the sketch - it is very flexible and no wrapper around it would be able to preserve it's virtues while being much of a wrapper.

## Ballpark overhead figures
The digital I/O functions are astonishingly inefficient. This isn't my fault - it's the Arduino API's fault
Single call to the function, volatile variables used as argumentsto the functions to prevent compiler from making assumptions about their values.

The comparison to the fast I/O functions is grossly unfair, because the fast I/O functions have constant arguments - but this gives an idea of the scale.
Remember also that executing 1 word of code (2 bytes) takes 1 clock cycle, though not all of the code paths are traversed every time.

| Function           | DB64 | t3224 | t3216 |
|--------------------|------|-------|-------|
| pinMode()          | +306 |  +128 |  +146 |
| digitalWrite()     | +656 |  +198 |  +340 |
| both of above      | +784 |  +296 |  +438 |
| openDrain()        | +584 |  +128 |  +262 |
| turnOffPWM()       | +572 |   +72 |  +228 |
| pinConfigure() 1   | +216 |  +104 |  +116 |
| pinConfigure() 2   | +362 |  +202 |  +220 |
| pinConfigure() 3   | +414 |  +232 |  +250 |
| digitalWriteFast() |   +2 |    +2 |    +2 |
| pinModeFast()      |  +12 |   +12 |   +12 |

pinConfigure called with second argument constrained to 0x00-0x0F (setting only direction and output value) (1), with it constrained to that plus configuring pullups (2), and without restrictions (3).
About 200 bytes of that space is used for lookup tables, not instructions.

This is why the fast digital I/O functions exist.

This is why the fast digital I/O functions exist, and why there are people who habitually do `VPORTA.OUT |= 0x80;` instead of `digitalWrite(PIN_PA7,HIGH);`

It's also why I am not comfortable automatically switching digital I/O to "fast" type when constant arguments are given. The normal functions are just SO SLOW that you're sure to break code that hadn't realized they were depending on the time it took for digital write, even if just for setup or hold time for the thing it's talking to.
## openDrain()
It has always been possible to get the benefit of an open drain configuration - you set a pin's output value to 0 and toggle it between input and output. This core provides a slightly smoother (also faster) wrapper around this than using pinmode (since pinMode must also concern itself with configuring the pullup, whether it needs to be changed or not, every time - it's actually significantly slower setting things input vs output. The openDrain() function takes a pin and a value - `LOW`, `FLOATING` (or `HIGH`) or `CHANGE`. `openDrain()` always makes sure the output buffer is not set to drive the pin high; often the other end of a pin you're using in open drain mode may be connected to something running from a lower supply voltage, where setting it OUTPUT with the pin set high could damage the other device.

Use pinMode() to set the pin `INPUT_PULLUP` before you start using `openDrain()`, or use `pinConfigure()` if you need the pullup enabled too; this doesn't change it. Note that there is nothing to stop you from doing `pinMode(pin,INPUT_PULLUP); openDrain(pin,LOW);` That is non-destructive (no ratings are exceeded), and would be a good place to start for a bitbanged open drain protocol, should you ever create one - but it doesn't exactly help with power consumption if you leave it like that! If running on batteries, be sure to turn off one of the two, either by using openDrain to release it to to pullups, or digitalWrite'ing it `HIGH`, or setting pinMode to output, or any other method of configuring pins that gets you a pin held in a defined state where it isn't working against it's own pullup.

```c++
openDrain(PIN_PA1, LOW); // sets pin OUTPUT, LOW.
openDrain(PIN_PA1, FLOATING); // sets pin INPUT. It will either float, or (if pullup was previously enabled) be pulled up to Vdd.
```

## Fast Digital I/O
This core includes the Fast Digital I/O functions, digitalReadFast(), digitalWriteFast() and openDrainFast(). These are take up less flash than the normal version, and can execute in as little as 1 clock cycle. The catch is that the pin MUST be a known constant at compile time. For the fastest, least flash-hungry results, you should use a compile-time known constant for the pin value as well. Remember than there are three options here, not just two. If you only want two of the options, you will get smaller binaries that run faster by using the ternary operator to explicitly tell the compiler that the value is only ever going to be those two values, and then it can optimize away the third case.

An openDrainFast() is also provided. openDrainFast() also writes the pin LOW prior to setting it output or toggling it with change, in case it was set `INPUT` and `HIGH`. `CHANGE` is slightly slower and takes an extra 2 words of flash because the `VPORT` registers only work their magic when the sbi and cbi (set and clear bit index) instructions will get you what you want. For the output value, writing a 1 to the bit in `VPORT.IN` will toggle it, and that's what we do, but there is no analogous way to change the pin direction, so we have to use a full STS instruction (2 words) to write to the `PORT.DIRTGL` register after spending another word on LDI to load the bit we want to toggle into a working register.

It also includes pinModeFast(). pinModeFast() requires a constant mode argument. INPUT and INPUT_PULLUP as constant arguments are still as large and take as long to execute as digitalWriteFast with either-high-or-low-state - they turn the pullups on and off (use openDrainFast(pin,FLOATING) to execute faster and leave the pullups as is).

```c++
digitalWriteFast(PIN_PD0, val); // This one is slower than the one below:
digitalWriteFast(PIN_PD0, val ? HIGH : LOW);
digitalWriteFast(PIN_PD0, HIGH); // This is fastest of all, 1 word of flash, and 1 clock cycle to execute
VPORTD.OUT |= 1 << 0; // The previous line is syntactic sugar for this. Beyond being more readable
                      // it eliminates the temptation to try to write more than one bit which instead
                      // of an atomic SBI or CBI, is compiled to a read-modify-write cycle of three
                      // instructions which will cause unexpected results should an interrupt fire in the
                      // middle and write the same register.
```

### Flash use of fast digital output functions

| function            | Any value | HIGH/LOW | Constant        |
|---------------------|-----------|----------|-----------------|
| openDrainFast()     | 14 words  | 7 words  | 2 words if LOW<br/>1 if FLOATING |
| digitalWriteFast()  | 10 words  | 6 words  | 1 words         |
| pinModeFast()       |      N/A  |     N/A  | 1 word if OUTPUT<br/>6 otherwise |

Execution time is 1 or sometimes 2 clocks per word that is actually executed (not all of them are in the multiple possibility options. in the case of the "any option" digitalWrite, it's 5-7 (there are some that )
Note that the HIGH/LOW numbers include the overhead of a `(val ? HIGH : LOW)` which is required in order to get that result. That is how the numbers were generated - you can use a variable of volatile uint8_t and that will prevent the compiler from assuming anything about it's value. It is worth noting that when both pin and value are constant, this is 2-3 times faster and uses less flash than *even the call to the normal digital IO function*, much less the time it takes for the function itself (which is many times that. The worst offender is the normal digitalWrite(), because it also has to check for PWM functionality and then turn it off if enabled (and the compiler isn't allowed to skip this if you never use PWM).

1 word is 2 bytes; when openDrainFast is not setting the pin FLOATING, it needs an extra word to set the output to low level as well (just in case). This was something I debated, but decided that since it could be connected to a lower voltage part and damage caused if a HIGH was output, ensuring that that wasn't what it would do seemed justified; if CHANGE is an option, it also uses an extra 2 words because instead of a single instruction against VPORT.IN, it needs to load the pin bit mask into a register (1 word) and use STS to write it (2 words) - and it also does the clearing of the output value - hence how we end up with the penalty of 4 for the unrestricted case vs digitalWriteFast.

The size and speed of digitalReadFast is harder to calculate since it also matters what you do with the result. It is far faster than digitalRead() no matter what. The largest impact is seen if you plan to use it as a test in an if/while/etc statement; there are single-clock instructions to skip the next instruction if a specific bit in a "low I/O" register like the ones involved is set or cleared. Thus, it is most efficient to do so directly:
```c
if (digitalReadFast(PIN_PA3)) {
  Serial.println("PA3 is high")
}
```
Getting a 1 or 0 into a variable is slower and uses more flash; I can think of at least one way to do it in 3 which makes no assumption about whether it's stored in an upper or lower register, but I'm not confident in how clever the compiler is, and it's tricky to set something up where the compiler doesn't have options such that you can generalize the result.

**The fast digital I/O functions do not turn off PWM** as that is inevitably slower (far slower) than writing to pins and they would no longer be "fast" digital I/O.

## turnOffPWM(uint8_t pin) is exposed
This used to be a function only used within wiring_digital. It is now exposed to user code - as the name suggests it turns off PWM (only if analogWrite() could have put it there) for the given pin. It's performance is similar to analogWrite (nothing to get excited over), but sometimes an explicit function to turn off PWM and not change anything else is preferable.

## Finding current PWM timer, if any
On many cores, there is a `digitalPinToTimer(pin)` macro, mostly for internal use, which returns the timer associated with the pin. On DxCore there is the added complication that we account for the TCA portmux register, and digitalPinToTimer() only returns non-TCA timers. For the PORTMUX-aware test, call `digialPinToTimerNow(pin)` instead.

## pinConfigure()
pinConfigure is a somewhat ugly function to expose all options that can be configured on a per-pin basis. It is called as shown here; you can bitwise-or any number of the constants shown in the table below. All of those pin options will be configured as specified. Pin functions that don't have a corresponding option OR'ed into the second argument will not be changed. There are very few guard rails here: This function will happily enable pin interrupts that you don't have a handler for (but when they fire it'll crash the sketch), or waste power with pins driven low while connected to the pullup and so on.

```c++
pinConfigure(PIN_PA4,(PIN_DIR_INPUT | PIN_PULLUP_ON | PIN_OUT_LOW | PIN_INLVL_TTL));
// Set PIN_PA4 to input, with pullup enabled and output value of LOW (ready for openDrainFast() and using TTL logic levels, without changing whether the pin is inverted or triggers an interrupt.
// This might be used for some sort of bi-directional open-drain communication protocol with a device operating at lower voltage.

pinConfigure(PIN_PD4,(PIN_DIR_INPUT | PIN_OUT_LOW | PIN_PULLUP_OFF | PIN_INVERT_OFF | PIN_INLVL_SCHMITT | PIN_ISC_ENABLE));
// Set PD4 inpit, with output register set low, pullup, invert, and alternate levels off, and digital input enabled. Ie, the reset condition!

```


| Functionality |   Enable  | Disable            | Toggle |
|---------------|-------|---------------------|--------------------|
| Direction, pinMode() | `PIN_DIR_OUTPUT`<br/>`PIN_DIR_OUT`<br/>`PIN_DIRSET` | `PIN_DIR_INPUT`<br/>`PIN_DIR_IN`<br/>`PIN_DIRCLR`       | `PIN_DIR_TOGGLE`<br/>`PIN_DIRTGL` |
| Pin output, `HIGH` or LOW | `PIN_OUT_HIGH`<br/>`PIN_OUTSET`         | `PIN_OUT_LOW`<br/>`PIN_OUTCLR`          | `PIN_OUT_TOGGLE`<br/>`PIN_OUTTGL`       |
| Internal Pullup  | `PIN_PULLUP_ON`<br/>`PIN_PULLUP`        | `PIN_PULLUP_OFF`<br/>`PIN_NOPULLUP`       | `PIN_PULLUP_TGL`       |
| Invert `HIGH` and LOW |`PIN_INVERT_ON`        | `PIN_INVERT_OFF`       | `PIN_INVERT_TGL`       |
| Use TTL levels (DB/DD only) | `PIN_INLVL_TTL`<br/>`PIN_INLVL_ON`        | `PIN_INLVL_SCHMITT`<br/>`PIN_INLVL_OFF`     | Not supported<br/>No plausible use case      |
| Digital input buffer | `PIN_INPUT_ENABLE`or<br/> `PIN_ISC_ENABLE`    | `PIN_ISC_DISABLE` or<br/>`PIN_INPUT_DISABLE`    | Not supported<br/>No plausible use case |
| Interrupt on change | `PIN_ISC_ENABLE` or<br/> `PIN_INPUT_ENABLE`       | `PIN_ISC_ENABLE` or<br/>oth     | Not applicable |
| Interrupt on Rise  | `PIN_ISC_RISE` or<br/> `PIN_INT_RISE`         | `PIN_ISC_ENABLE` or<br/>`PIN_ISC_DISABLE`     | Not applicable |
| Interrupt on Fall  | `PIN_ISC_FALL` or<br/> `PIN_INT_FALL` | `PIN_ISC_ENABLE` or<br/>`PIN_ISC_DISABLE`      | Not applicable |
| Interrupt on LOW  | `PIN_ISC_LEVEL`  or<br/> `PIN_INT_LEVEL` | `PIN_ISC_ENABLE` or<br/>`PIN_ISC_DISABLE`      | Not applicable |

For every constant with TGL or TOGGLE at the end, we provide the other spelling as well. For every binary option, in addition to the above, there is a `PIN_(option)_SET` and `PIN_(option)_CLR` for turning it on and off. The goal was to make it hard to not get the constants right.


### INLVL - input logic levels
On MVIO parts (DB and DD-series only), pins can be configured to use one of two sets of input voltages: either the normal schmitt triggert input, which has thresholds as a fraction of Vdd, or the TTL mode which has fixed thresholds independent of operating voltage. On MVIO pins, using the schmitt trigger Vddio2 takes the place of VDDIO. Note that this is overridden when pins are used for the TWI - that defaults to I2C voltage levels, but supports an optional SMBus 3.0 mode.

| Voltage             |  Schmitt  |     I2C   |  TTL   | SMBus 3.0 |
|---------------------|-----------|-----------|--------|-----------|
| Low input (Max)     | 0.2 x Vdd | 0.3 x Vdd | 0.80 V |    0.80 V |
| High Input (Min)    | 0.8 x Vdd | 0.7 x Vdd | 1.60 V |    1.35 V |

These are the maximum voltage guaranteed to qualify as LOW and the minimum guaranteed to qualify as `HIGH`. Although the I2C levels are described as being different, it is worth noting that 1) if the schmitt trigger input on a recognized a 0.3 x Vdd voltage as LOW, it would surely do the same for a 0.2 x Vdd one, likewise at the high end. And typical characteristics graphs, when available, for modern AVRs with the same thresholds and I2C levels, show typical I/O pin thresholds as easily meeting the specs given for the I2C levels. The two options for I2C levels are not new for the DB either - they are widespread. What appears to be new is a less tightly specified but conceptually similar input circuit on all pins, selectable by a bit in PINnCTRL, insead of just I2C pins. It's important to keep in mind how this divides up possible input voltages (note: this is just based on room temp typical characteristics)
* With Schmitt trigger inputs, highest and lowest 1/5th of the voltage-space each go to a definite value, while the voltages between allow the hysteresis that the schmitt triggers are known for. The graphs in the DA-series datasheet (DB typical charachteristics are not yet available) show that in practice, the hysteresis occurs over a much narrower voltage range than the same graph for the 4809 (which depicted three regions of similar size).
  * Typical falling threshold ranged from 0.8 up to 2.05, and rising thresholds from 1.05 V to 2.75 V. Thus, the hysteresis region ranged in size from 0.2 to 0.7 volts, but reached a nadir just around 2-2.25 V where it could be as small as 0.15 V, since the lower threshold increases faster than the upper one at the bottom of the range.
  * When Vdd = 5.0 V - 3.3 V is not guaranteed to be `HIGH`, though the typical characteristic graphs suggest that it will be, as has always been the case on AVRs.
* With TTL input levels, behavior relative to the operating voltage varies greatly.
  * With Vdd = 1.8 V (the minimum), below about half the operating voltage, it's going to read low, while it may need to be very close to the power rail to read `HIGH`.
  * With Vdd = 5.0 V (maximum typically used intentionally) anything over about 1/3rd if the supply voltage is guaranteed to be `HIGH`, and you need to get down to 1/6th of the supply voltage for a guaranteed `LOW`.
  * The actual lower threshold is likely to be higher, and the upper threshold lower. For example, on DA-series SMBus pins, those voltages are 1.09 V and 1.24 V over most of the operating voltage range. This leave you with very little hysteresis, if that happens to be problematic.
  * If interacting with an output that for whatever reason struggles to pull the pin as close to ground as an ideal output should, or if you're counting on the transition being somewhere near the middle of the range, definitley don't use this option.

Particularly in the context of these MVIO-equipped parts, however, the TTL input level option has an obvious major advatage: It makes communication with parts that run at lower voltages easier, and reduces competition for the limited number of MVIO pins. If we allow the low part to control it push-pull (aka totam-pole and other names), and treat it as an `INPUT`, we will always read what they intend. The fact that you can switch a pin between input and output, while pulling it up externally was noted above, and the voltage it was pulled up to could be the lower operating vboltage. That can be done on any AVR. But with INLVL, this scheme is viable for a bidirectional line: the The line between devices could either be pulled up to the lowest operating voltage externally, with the DB or DD series part manipulating the pin as an open drain output (see above). One need not stop with just two devices - multiple DB/DD-series parts with `INVLV = TTL` set could communicate with multiple devices operating at the low voltage, as long as none of them ever drove it `HIGH` (though, if it was low, they would not know which device was doing that. The designer would have to make sure this approach made sense in that regard, and that everything was smart enough to not hold onto the line or anything.

## PINCONFIG and associated registers
The hardware has a series of registers - one shared across all ports, `PORTx.PINCONFIG` with bitfields matching the ones in the the PINnCTRL registers, and three which always read zero, `PORTx.PINCTRLUPD`,  `PORTx.PINCTRLSET` and  `PORTx.PINCTRLCLR`. These allow mass updating of the PINnCTRL registers: Every pin corresponding to a 1 on the port will have the contents of their PINnCTRL register either set to (`PINCTRLUPD`), bitwise-OR'ed with (`PINCTRLSET`) or bitwise-AND'ed with the inverse (`PORTx.PINCTRLCLR`) of the  `PINCONFIG` register.

So:
```c
PORTA.PINCONFIG = PORT_PULLUPEN_bm;
PORTA.PINCTRLSET = 0b11110000; // Turns on the pullup on PA4~7
PORTB.PINCTRLUPD = 0b10000001; // Sets PINnCTRL to only have pullup enabled on PB0 and PB7. If one of those pins was inverted or using TTL levels or had an interrupt on it, it doesn't anymore.
PORTC.PINCTRLCLR = 0b11111111; // Turns off the pullup on every pin on PORTC
PORTA.PINCONFIG = PORT_PULLUPEN_bm | PORT_INVEN_bm; //invert and turn on pullup
PORTG.PINCTRLUPD = 0b00001111; // inverts and turns on pullups on PG0~3
PORTF.PINCTRLUPD = 0b01010101; // inverts and turns on pullups on PF 0, 2, 4, and 6.

/* executes faster and looks less hideous than: */
PORTA.PIN4CTRL |=  PORT_PULLUPEN_bm; //Read-modify-write takes 6 clocks and 5 words of flash! - lds, ori, sts
PORTA.PIN5CTRL |=  PORT_PULLUPEN_bm;
PORTA.PIN6CTRL |=  PORT_PULLUPEN_bm;
PORTA.PIN7CTRL |=  PORT_PULLUPEN_bm;
PORTB.PIN0CTRL  =  PORT_PULLUPEN_bm; // Simple write 2 clocks 2 words to write, plus a single 1 for the whole group to load the value to a working register.
PORTB.PIN7CTRL  =  PORT_PULLUPEN_bm;
PORTC.PIN0CTRL &= ~PORT_PULLUPEN_bm; // 6 clocks again!
PORTC.PIN1CTRL &= ~PORT_PULLUPEN_bm;
PORTC.PIN2CTRL &= ~PORT_PULLUPEN_bm;
PORTC.PIN3CTRL &= ~PORT_PULLUPEN_bm;
PORTC.PIN4CTRL &= ~PORT_PULLUPEN_bm;
PORTC.PIN5CTRL &= ~PORT_PULLUPEN_bm;
PORTC.PIN6CTRL &= ~PORT_PULLUPEN_bm;
PORTC.PIN7CTRL &= ~PORT_PULLUPEN_bm;
PORTF.PIN4CTRL  =  PORT_PULLUPEN_bm | PORT_INVEN_bm;
PORTF.PIN5CTRL  =  PORT_PULLUPEN_bm | PORT_INVEN_bm;
PORTF.PIN6CTRL  =  PORT_PULLUPEN_bm | PORT_INVEN_bm;
PORTF.PIN7CTRL  =  PORT_PULLUPEN_bm | PORT_INVEN_bm;
PORTG.PIN0CTRL  =  PORT_PULLUPEN_bm | PORT_INVEN_bm;
PORTG.PIN1CTRL  =  PORT_PULLUPEN_bm | PORT_INVEN_bm;
PORTG.PIN2CTRL  =  PORT_PULLUPEN_bm | PORT_INVEN_bm;
PORTG.PIN3CTRL  =  PORT_PULLUPEN_bm | PORT_INVEN_bm;

/* The above contrived examples are 7 x 3 - 21 clocks and 21 words for the multipin-configuration method.
 * versus 4*6 + 2*2+1 + 8*6 + 2*8+1 = 94 clocks and 82 words of flash for the alternative.
 * The contrast is even more stark for somee realistic scenarios on high pincount devices.
 */

```

All bits make sense with PINCTRLUPD - some more than others - as long as you don't forget that it will replace whatever is there currently.
The lower three bits, controlling whether the pin is acting as an interrupt, will have surprising and likely unwelcome effects with  `PORTx.PINCTRLSET` and  `PORTx.PINCTRLCLR`. For PINCTRLSET they should never be 1 (any of them). For PINCTRLCLR, all should be 0 or all should be 1 ( 0b00000111 would turn off pin interrupts on the pins it's applied to without otherwise changing PINnCTRL register contents). Otherwise, the concept of flipping individual bits makes little sense for that last bitfield - the bits individually do not have a separate function, there is just the function of those three bits as a group. When changing the input sense configuration on large numbers of pins, use PINCTRLUPD instead.

This system is most useful at startup for devices running on battery to meet the datasheet requirement that the pins not be allowed to float if their digital input buffers are enabled.

This core *currently* does not make use of this feature.


## Slew Rate Limiting
All of the Dx-series parts have the option to limit the [slew rate](https://en.wikipedia.org/wiki/Slew_rate) for the OUTPUT pins on a on a per-port basis. This is typically done because fast-switching digital pins contain high-frequency components (in this sense, high frequency doesn't necessarily mean that it repeats, only that if it continued, it would be high frequency; this way of thinking is useful in electrical engineering), which can contribute to EMI, as well as ringing (if you're ever looked on a scope and noticed that after a transition, the voltage briefly oscillates around the final voltage - that's ringing) which may confuse downstream devices (not usually, at least in arduino land, though). Often, you will not know exactly *why* it's an issue, either, rather a datasheet may specify a maximum slew rate on it's inputs.
```c
// These work but are slow and inefficient.
// Enable slew rate limiting on PORTA
PORTA.PORTCTRL |= PORT_SRL_bm;
// Disable slew rate limiting on PORTA
PORTA.PORTCTRL &= ~PORT_SRL_bm;
```
However, because there is only one bit in this register, you don't actually need to use the |= and &= operators, which do a read-modify-write operation, taking 10 bytes and 6 clocks each. Instead, you can use simple assignment. This saves 4 bytes (3 clocks) when enabling, and 6 bytes (4 clocks) on disabling. (going from `LDS, ORI/ANDI, STS` to `LDI, STS` and just `STS`, since avr-gcc always keeps a "known zero" in r1). Of course, if a future part with more port options comes out, porting to that part would require a code change if you also wanted to use one of those new bits. I suggest a reminder that it's a shortcut if you take it, like the comment below.

```c
// Enable slew rate limiting on PORTA more efficiently
PORTA.PORTCTRL = PORT_SRL_bm; // simple assignment is okay because on DA, DB, and DD-series parts, no other bits of PORTCTRL are used, saves 2 words and 3 clocks
// Disable slew rate limiting on PORTA
PORTA.PORTCTRL = 0;           // simple assignment is okay because on DA, DB, and DD-series parts, no other bits of PORTCTRL are used, saves 3 words and 4 clocks
```

These parts (like many electronics) are better at driving pins low than high (all values are from the typ. column - neither min nor max are specified; one gets the impression that this is not tightly controlled):

| Direction   | Normal | Slew Rate Limited |
|-------------|--------|-------------------|
| Rising      |  22 ns |             45 ns |
| Falling     |  16 ns |             30 ns |

With a 24 MHz system clock, that means "normal" would be just ovwr half a clock cycle while rising, and just under half a clock cycle falling; with slew rate limiting, it would be just over a clock cycle rising, and 3/4ths of a clock cycle on the falling side.


## Note on number of pins and future parts
The most any announced AVR has had is 86 digital pins, the ATmega2560; In the modern AVR era, the digital pin maximum is 55, and the maximum possible without rearchitecting how pins are accessed is 56 (ie, if UPDI could be turned into I/O on DA/DB we are already there). There is no way for an AVR that allows the same bit-level access to we have on all pins currently to more than 56 pins. Each port takes up 4 of the 32 addresses in the low I/O space for the VPORT registers, on which the single cycle bit-level access relies. Only time will tell how this is handled,
* **No VPORTH or higher** - This is clean, simple, and limiting in some ways, but probably the best route. If you needed to use VPORT access, you'll just need to use the lower 7 ports for it. Surely you don't need 57+ pins all with VPORT atomic single cycle access! This is also, in my judgement, most likely based on the fact that on the ATmega2560, the only precedent we have for running out of low I/O registers for pins, they didn't even put ports H through L into the high I/O space.
* **VPORTs in High I/O** - There are many ways of dealing with it using a new set of differently behaving registers in the high I/O space. None of them retain the arbitrary compile-time-known-bit-write-is-one-clock of current VPORTs. At 8 registers per port, you could make a series of N operations on a bit or number of bits take N+1 clocks, and keep them atomic. This would mean a VPORT.DIR/DIRCLR/DIRSET, and VPORT.OUT/OUTSET/OUTCLR and VPORT.IN(writing to IN toggles, and there was never a single cycle DIR toggle). and VPORT.INTFLAGS. That's viable, though with the other registers in that, you could get at most only 3 more ports - which is only 24 pins: You couldn't quite do it for a 100 pin chip). Other solutions would lower capability and comprehensibility in exchange for squeezing in more pins. No matter how you do it, it doesn't really solve the problem.
* **VPORTs that can be remapped at runtime** - The stuff of library author's nightmares. You'd have some subset of the VPORTs (or maybe all of them) could be remapped. If you put the register for that in the high I/O space it could be written quickly. But this will cause unaware old code to break by writing to the wrong pins if it didn't know to check for it, and any code that used them (ie, code that is supposed to be fast) might have to check the mapping first. This is most like what was done on the XMegas, but also serves as a shining example of the over-complexity that I think doomed those parts to poor uptake and slow death. The best of these bad approaches would probably be to have VPORTG changed into say VPORTX. Could configure it with a register in the high I/O space so it was 2 cycle overhead to write (1 for LDI port number, 1 to OUT to register). That way all code for other ports wouldn't worry, and port G is present on the fewest parts so the least code would be impacted if ported, and would be straightforward to document
