# Improved Digital I/O Functionality
This core includes a number of features to provide more control or performance when doing digital I/O. This page describes how to use them. All of the options that can be configured for a pin are exposed. The only thing that isn't exposed is the slew rate limiting feature, which can only be configured on a per port basis. Turning it on and off is so simple (see below) that it needs no wrapper.

## openDrain()
It has always been possible to get the benefit of an open drain configuration - you set a pin's output value to 0 and toggle it between input and output. This core provides a slightly smoother (also faster) wrapper around this than using pinmode (since pinMode must also concern itself with configuring the pullup, whether it needs to be changed or not, every time - it's actually significantly slower setting things input vs output. The openDrain() function takes a pin and a value - `LOW`, `FLOATING` (or `HIGH`) or `CHANGE`. `openDrain()` always makes sure the output buffer is not set to drive the pin high; often the other end of a pin you're using in open drain mode may be connected to something running from a lower supply voltage, where setting it OUTPUT with the pin set high could damage the other device. An openDrainFast() is also provided; as of 1.3.6, it also writes the pin LOW prior to setting it output or toggling it with change. `CHANGE` is slightly slower and takes an extra 2 words of flash because the VPORT registers only work their magic when the sbi and cbi (set and clear bit index) instructions will get you what you want. For there output value, setting the pin in VPORT.IN will do that, but there is no analogous way to change the pin direction, so we have to use a full STS instruction (2 words) to write to the `PORT.DIRTGL` register after spending another word on LDI to load the bit we want to toggle into a working register.

Use pinMode() to set the pin `INPUT_PULLUP` before you start using `openDrain()`, or use `pinConfigure()` if you need the pullup enabled too; this donesn't change it. Note that there is nothing to stop you from doing `pinMode(pin,INPUT_PULLUP); openDrain(pin,LOW);` That is non-destructive (no ratings are exceeded), and would be a good place to start for a bitbanged I2C or I2C-like interface, should you ever create one. Though - it doesn't exactly help with power consumption; if running on batteries, be sure to turn off one of the two, either by using openDrain to release it to to pullps, or digitalWrite'ing it `HIGH`, or set pinMode to output, or any other method of configuring pins that gets you a pin held in a defined state where it isn't working against it's own pullup.

```c++
openDrain(PIN_PA1, LOW); // sets pin output, LOW.
openDrain(PIN_PA1, FLOATING); // sets pin input and lets it float.
```

## Fast Digital I/O
This core includes the Fast Digital I/O functions, digitalReadFast(), digitalWriteFast() and openDrainFast(). These are always inlined, but still take up less flash than the normal version of the function, and execute the in a single clock cycle. The catch is that the pin MUST be a known constant at compile time. For the fastest, least flash-hungry results, you should use a compile-time known constant for the pin value as well. Remember than there are three options here, not just two. If you only want two of the options, you will get smaller binaries that run faster by using the ternary operator to explicitly tell the compiler that the value is only ever going to be those two values, and then it can optimize away the third case.

```c++
digitalWriteFast(PIN_PD0, val); // This one is slower than the one below:
digitalWriteFast(PIN_PD0, val ? HIGH : LOW);
digitalWriteFast(PIN_PD0, HIGH); // This is fastest of all, 1 word of flash, and 1 clock cycle to execute
VPORTD.OUT |= 1 << 0; // The previous line is syntactic sugar for the this. Beyond being more readable, it eliminates the temptation to try to write more than one bit
                      // and the risk accidentally doing so, which instead of an atomic SBI or CBI, is compiled to a read-modify-write cycle of 3 instructions which will
                      // cause unexpected results should an interrupt fire in the middle and write the same register.
```

### Flash use of fastPinMode functions

| function            | Any value | HIGH/LOW | Constant        |
|---------------------|-----------|----------|-----------------|
| openDrainFast()     | 14 words  | 7 words  | 2 words if LOW<br/>1 if FLOATING |
| digitalWriteFast()  | 10 words  | 6 words  | 1 words         |

Execution time is 1 or sometimes 2 clocks per word that is actually executed (not all of them are in the multiple possibility options. in the case of the "any option" digitalWrite, if it's LOW, if change, 6, and if `HIGH`, 7
Note that the HIGH/LOW numbers include the overhead of a `(val ? HIGH : LOW)` which is required in order to get that result. That is how the numbers were generated - you can use a variable of volatile uint8_t and that will prevent the compiler from making; 3 and 5 for the two word case. Which highlights one problem: the execution times now depend on the values, and rather strongly. It is worth noting that when both pin and value are constant, this is 2-3 times faster and uses less flash than even the call to digitalWrite, much less the time it takes for the function itself.

1 word is 2 bytes; when openDrainFast is not setting the pin FLOATING, it needs an extra word to set the output to low level as well (just in case). This was something I debated, but decided that since it could be connected to a lower voltage part and damage caused if a HIGH was output, ensuring that that wasn't what it would do seemed justified; if CHANGE is an option, it also uses an extra 2 words because instead of a single instruction against VPORT.IN, it needs to load the pin bit mask into a register (1 word) and use STS to write it (2 words) - and it also does the clearing of the output value - hence how we end up with the penalty of 4 for the unrestricted case vs digitalWriteFast.

The size and speed of digitalReadFast is harder to calculate since it also matters what you do with the result, but if you plan to use it as a testing an if/while/etc; there are single-clock instructions to skip the next instruction if a specific bit in a "low I/O" register like the ones involved is set or cleared. Thus, it is most efficient to do so directly:
```
if (digitalReadFast(PIN_PA3)) {
  Serial.println("PA3 is high")
}
```

**The fast digital I/O functions do not turn off PWM** as that is inevitably slower than writing to pins and they would no longer be "fast" digital I/O.

## turnOffPWM(uint8_t pin) is exposed
This used to be a function only used within wiring_digital. It is now exposed to user code - as the name suggests it turns off PWM (only if analogWrite() could have put it there) for the given pin. It's performance is similar to analogWrite (nothing to get excited over), but sometimes an explicit function to turn off PWM and not change anything else is preferable.

## pinConfigure()
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
| Pin output, `HIGH` or LOW | `PIN_OUT_HIGH`<br/>`PIN_OUTSET`         | `PIN_OUT_LOW`<br/>`PIN_OUTCLR`          | `PIN_OUT_TOGGLE`<br/>`PIN_OUTTGL`       |
| Internal Pullup  | `PIN_PULLUP_ON`<br/>`PIN_PULLUP`        | `PIN_PULLUP_OFF`<br/>`PIN_NOPULLUP`       | `PIN_PULLUP_TGL`       |
| Invert `HIGH` and LOW |`PIN_INVERT_ON`        | `PIN_INVERT_OFF`       | `PIN_INVERT_TGL`       |
| Use TTL levels (DB/DD only) | `PIN_INLVL_TTL`<br/>`PIN_INLVL_ON`        | `PIN_INLVL_SCHMITT`<br/>`PIN_INLVL_OFF`     | Not supported<br/>No plausible use case      |
| Digital input buffer | `PIN_INPUT_ENABLE`     | `PIN_INPUT_DISABLE`    | Not supported<br/>No plausible use case |
| Interrupt on change | `PIN_INT_CHANGE`       | `PIN_INPUT_ENABLE` or<br/>`PIN_INPUT_DISABLE`     | Not applicable |
| Interrupt on Rise  | `PIN_INT_RISE`         | `PIN_INPUT_ENABLE` or<br/>`PIN_INPUT_DISABLE`     | Not applicable |
| Interrupt on Fall  | `PIN_INT_FALL`         | `PIN_INPUT_ENABLE` or<br/>`PIN_INPUT_DISABLE`      | Not applicable |
| Interrupt on LOW  | `PIN_INT_LEVEL`        | `PIN_INPUT_ENABLE` or<br/>`PIN_INPUT_DISABLE`      | Not applicable |

From 1.3.7, for every constant with TGL or TOGGLE at the end, we provide the other spelling as well.

### INLVL - input logic levels
On MVIO parts (DB and DD-series only), pins can be configured to use one of two sets of input voltages: either the normal schmnitt triggert input, has thresholds as a fraction of Vdd, the TTL mode has fixed thresholds independent of operating voltage. On MVIO pins, using the schmitt trigger Vddio2 takes the place of VDDIO. Note that this is overridden when pins are used for the TWI - that defaults to I2C voltage levels, but supports an optional SMBus 3.0 mode.

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

## Slew Rate Limiting
All of the Dx-series parts have the option to limit the [slew rate](https://en.wikipedia.org/wiki/Slew_rate) for the OUTPUT pins on a on a per-port basis. This is typically done because fast-switching digital pins contain high-frequency components (in this sense, high frequency doesn't necessarily mean that it repeats, only that if it continued, it would be high frequency; this way of thinking is useful in elecrical engineering), which can contribute to EMI, as well as ringing (if you're ever looked on a scope and noticed that after a transition, the voltage briefly oscillates around the final voltage - that's ringing) which may confuse downstream devices. You may not know exactly *why* it's an issue, either, rather a datasheet may specify a maximum slew rate on it's inputs.

```c
// Enable slew rate limiting on PORTA
PORTA.PORTCTRL |= PORT_SRL_bm;
// Disable slew rate limiting on PORTA
PORTA.PORTCTRL &= ~PORT_SRL_bm;
```
However, because there is only one bit in this register, you don't actually need to use the |= and &= operators, which do a read-modify-write operation, taking 10 bytes and 6 clocks each. Instead, you can use simple assignment. This saves 4 bytes (3 clocks) when enabling, and 6 bytes (4 clocks) on disabling. (going from `LDS, ORI/ANDI, STS` to `LDI, STS` and just `STS`, since avr-gcc always keeps a "known zero" in r1). Of course, if a future part with more port options comes out, porting to that part would require a code change if you also wanted to use one of those new bits. I suggest a reminder that it's a shortcut if you take it

```c
// Enable slew rate limiting on PORTA
PORTA.PORTCTRL = PORT_SRL_bm; // simple assignment is okay because on DA, DB, and DD-series parts, no other bits of PORTCTRL are used, saves 2 words and 3 clocks
// Disable slew rate limiting on PORTA
PORTA.PORTCTRL = 0;           // simple assignment is okay because on DA, DB, and DD-series parts, no other bits of PORTCTRL are used, saves 3 words and 4 clocks
```

These parts (like many electronics) are better at driving pins low than high (all values are from the typ. column - neither min nor max are specified):

| Direction   | Normal | Slew Rate Limited |
|-------------|--------|-------------------|
| Rising      |  22 ns |             45 ns |
| Falling     |  16 ns |             30 ns |

Running at 24 MHz, 1 clock cycle is just over 40 ns, to give a point of comparison, since Arduino users don't have to think in nanoseconds very often.
