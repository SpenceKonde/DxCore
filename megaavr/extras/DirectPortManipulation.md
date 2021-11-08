# Direct Port Manipulation
When using Arduino functions, like digitalWrite and digitalRead, is too slow, there are two ways to get much faster access to I/O pins. The first (and classic) method is to directly access the registers that control the pins; the second method is the digitalWriteFast() and digitalReadFast() calls (see bottom). Each port contains up to 8 pins, numbered 0 through 7 (see the pinout charts). For example, on the ATtiny1616 pin 0 is located on PORTA, bit 4, commonly written as PA4. On the AVR Dx-series parts, the numbering is even simpler than the tinyAVR ones, since the numbering *always* starts from PA0, and no pins are ever out of order (though there are often pins missing in the middle - for example, the 32-pin parts start with PA0-PA7, and end with PF0-PF5 plus the reset pin on PF6 - but they have no `PORTB` or `PORTE`, and only half of a `PORTC`). You can always get the Arduino pin number from the port/pin through the pre-defined constant, for example `PIN_PA4` - not to be confused with `PIN_A4` (which is defined to something else) or `PA4` (which is ALSO defined as something else by the io headers on some parts)

## Why one might need direct port manipulation
The main reason is that the Arduino API functions, digitalWrite, digitalRead, and pinMode are mindbogglingly slow. On smaller flash devices, the bulk of the full implementations may also motivate use of direct port manipulation.

I recently tried to trace the code path taken by DxCore for digitalWrite from an assembly listing, and gave up after 200+ clocks. The reason is that the Arduino API functions need to do a lot more that toggle or read the pin. They are given arbitrary pin number, usually at runtime, which they have to look up in a table to find the port, look up in another table to find the bit mask. Then if it's digitalWrite they have to check whether the pin can have any PWM outputs directed to it, which involves checking if it's pin 0-5 on it's port, and if so, whether the port matches the one that TCA0 or TCA1 is pointed at, if not, it looks up in a third table to see if it has a TCD or TCB pointed at it. If it has any timer pointed at it, it needs to determine the appropriate register to write in order to turn that off. Finally, once it's done that, it can finally write the pin. It also has to maintain compatibility with old code that assumes you can digitalWrite(pin,HIGH) when a pin is set INPUT to turn on the pullups, so it also has to get the pin control register, save the status register and disable interrupts, read the pin control register, set or clear that bit, write it back, and restore the status register.

pinMode and digitalRead don't need to clear the PWM, and in the case of pinMode, the pin control and interrupt shuffle, but breaking this down, the steps are something like:
1. Look up internal representations of the pins.
2. Turn off PWM, if any.
3. Get the port struct.
4. Write the new pin state within it or read current pin state.
5. Save and restore SREG around the the write the pin control register to configure pullups, which may be done just for backwards compatibility.

Most of the time, when using these functions, you know that many of these steps are unnecessary - but the compiler doesn't. Just because you've never made a call to analogWrite() doesn't free it from having to check for PWM. It can also take only limited measures to optimize away the looking up of values for constant pins or values. And while you may know that you don't have any interrupts that write to the same port or that you don't need the old pullup behavior emulated, the code is written to support that. The end result is that the digital I/O functions, which were always slow, are slower still here, and an operation like writing PIN_PA2 to HIGH could take hundreds of clock cycles. On the other hand, a direct port write to achieve the same thing takes... could be done in a single clock cycle.

## Fast Digital I/O functions
Before getting into the details - you may not need direct port manipulation at all! Like several other third party hardware packages, DxCore and megaTinyCore support Fast Digital I/O so direct port manipulation need be used much less often. Any case where you would be reading or writing a single pin with direct port manipulation can be done with fast digital I/O with **no overhead** and is far more readable.

See the [**Digital I/O Reference**](Ref_Digital.md).

## PORT registers on modern AVR devices
The PORT registers on the modern AVR parts work a little differently from classic AVR microcontrollers (like ATmega328p, ATtiny85, etc), though the concept is the same. Each port has a structure available named `PORTx`   (ex, `PORTA`, `PORTB`, `PORTC`, etc). Within that struct, there are fields to set the pin to be an input or output, to set the output value, read it's current value, and control pullups and other pin parameters, as well as read and clear interrupt flags. The names followed by `_bm` are bitmasks for the various pins, while `_bp` indicates a bit position. These constants come from the io-headers that Microchip supplies, and exist for every register and bitfield.
```c++
PINn_bm == 1 << PINn_bp
PINn_bp == n
/* Hence:
PIN5_bm = 1 << 5, or 0b 0010 0000 - 32.
```

You can read the state of a pin by using the IN register (this is like the PINx register of classic AVR):
```c++
    bool status = PORTA.IN & PIN5_bm;
```
The `PORTA.IN & PIN5_bm` will give you either 0 or PIN5_bm (since the bitmask will only ever have 1 bit set). Assigning it to a bool converts any non-zero value to 1/true. You may also see the C ideom `!!(PORTA.IN & PIN5_bm)` (not not (PORTA.IN & PIN5_bm)) used to convert a 1 in an unknown bit to 1, and leave 0 unchanged. If you need to read more multiple pins, of course, you don't need to mask off every bit.

`PORTx.DIR` is the register which determines if the pin is an input or an output (like DDRx registers in classic AVR). After reset, all pins are set to input (0 in the register). In order to use the pin as an output the bit in the register must be set to 1, which can be done as follows:
```c++
    PORTA.DIR = PIN4_bm
```
This will set pin 4 on PORTA as output, and all other pins on PORTA to input. This is Arduino pin 0 on tinyAVR with 14+ pins, Arduino pin 4 on Dx-series.
Multiple pins on the same port can be configured by writing the port value (specified either straight up or the bitwise OR of several bitmasks - bitwise ORs of constants are optimized away at compile time).
```c++
    PORTA.DIR =    0b01001000;          // sets PA3 and PA6 as an output, and all other pins on the port to input
    PORTA.DIR =    PIN3_bm | PIN6_bm;   // sets PA3 and PA6 as an output, and all other pins on the port to input
    PORTA.DIR =   (1 << 3) | (1 << 6)   // sets PA3 and PA6 as an output, and all other pins on the port to input
    PORTA.DIR |=   PIN3_bm | PIN6_bm;   // sets PA3 and PA6 as an output in addition to whatever pins are already outputs.
    PORTA.DIR &=   PIN3_bm | PIN6_bm;   // does not change PA3 or PA6, but sets all other pins to inputs.
    PORTA.DIR &= ~(PIN3_bm | PIN6_bm);  // sets PA3 amd PA6 to inputs, and leaves all other pins unchanged.
```

Most often, you want to set or clear one bit while not changing the others in order to control a single pin. You can use the DIRSET and DIRCLR registers to set (set as output) or clear (set as input) a specific pin or pins, or toggle it with `DIRTGL`:
```c++
    PORTA.DIRSET = PIN4_bm; // use PA4 as an output
    PORTA.DIRCLR = PIN4_bm; // use PA4 as an input
```

Turning the pin on and off is done with the OUT register (this works like the PORTx register of classic AVR). Writing a 1 to the corresponding pin will set an output pin HIGH while a 0 will set it LOW:
```c++
    PORTA.OUT |= PIN4_bm; // write PA4 high
    PORTA.OUT &=~PIN4_bm; // write PA4 low
    PORTA.OUT =
```
In the same way as above, there is an `OUTSET`, `OUTCLR`, and `OUTTGL` register.
```c++
    PORTA.OUTSET = PIN4_bm; // turn PA4 output on
    PORTA.OUTCLR = PIN4_bm; // turn PA4 output off
    PORTA.OUTTGL = PIN4_bm; // toggle PA4 output
    PORTA.OUTTGL = PIN4_bm | PIN5_bm; // toggle PA4 and PA5
```
The `OUTSET`/`DIRSET`, `OUTCLR/DIRCLR`, and `OUTTGL/DIRTGL` registers have several key advantages:
1. They are atomic. You do not have to worry about an interrupt firing between when you read and when you write the register, as you would have to do when doing |= or &=
2. They are faster than |= or &=. Straight assignment takes 3 clock cycles (one to load the value, 2 to store it to the register - so if you happened to have the bitmask in a register for math of some sort, that saves 1 clock) while |= or &= take 6 no matter what.

The atomicity is a big deal when writing library code, since you don't know whether someone will have an interrupt modifying the same registers or whether interrupts will be enabled, so you have to take countermeasures:

```c++
void doit() {
  PORTA.OUTSET = PIN4_bm;
} // Takes 3 clocks and 3 words of flash
// Is equivalent to
void doitworse() {
  uint8_t oldsreg = SREG; // 1 clock. save state. You don't know if interrupts are enabled when someone calls your function.
  cli();                  // 1 clock. disable interrupts if they aren't already.
  PORTA.OUT |= PIN4_bm;   // 6 clocks - read, modify, write
  SREG = oldsreg;         // 1 clock. restore state, so interrupts are turned back on only if they were on originally.
} // takes 9 clocks and 8 words of flash.
```

**Warning: Do not use |= or &= with the SET/CLR/TGL registers** When *read* they give the value of the corresponding register, but when written they will do their special function. The result is often surprising but never useful.

```c++
  PORTA.OUTSET |= PIN4_bm;  // same effect, only slower.
  PORTA.OUTCLR |= PIN4_bm;  // turn off PA4 and every pin currently set 1. Same effect as PORTA.OUT = 0, only slower.
  PORTA.OUTSET &= ~PIN4_bm; // Turn on every pin on PORTA except PA4. Same effect as PORTA.OUT = ~PIN4_bm, only slower.
  PORTA.OUTCLR &= ~PIN4_bm; // Turn off every pin on PORTA except PA4. Same effect as PORTA.OUT = ~PIN4_bm, only slower.
  PORTA.OUTTGL |= PIN4_bm;  // toggle every pin that is on, turning it off. Toggle PA4. Same effect as PORTA.OUT = (PORTA.OUT & PIN4_bm) ? 0 : PIN4_bm;
```

Unlike the classic AVRs, setting a pin HIGH with the OUT register while it is set as an input will not turn on the internal pullup. If you want to use the internal pullup resistor, you must work with the PINnCTRL register, like this:
```c++
    PORTA.PIN6CTRL |= PORT_PULLUPEN_bm;   // use the internal pullup resistor on PA6
    PORTA.PIN6CTRL &= ~PORT_PULLUPEN_bm;  // don't use the internal pullup resistor on PA6
```

Note that this does mean that each pin has its own PINnCTRL register - unlike the classic AVRs where there was one register to control pullup for each port, with one bit per pin. The rest of the PINnCTRL register is used to configure pin interrupts on the pin (see the [interrupt reference](Ref_Interrupts.md)), and optionally invert the pin, or (on MVIO parts) set the input logic levels. (See the [digital I/O reference](Ref_Digital.md) for the details on those parameters). If you're not using any of those, you can use straight assignment, which is faster and uses less flash.

```c++
    PORTA.PIN6CTRL = PORT_PULLUPEN_bm;  // use the internal pullup resistor on PA6. 3 clocks.
    PORTA.PIN6CTRL = 0;           // don't use the internal pullup resistor on PA6. 2 clocks - the compiler uses r1, the known zero and doesn't need to load the new value.
```

Finally, as discussed in the aforementioned interrupt reference, there is an INTFLAGS register, which must be written to within a pin interrupt handler in order to clear the flag so the interrupt doesn't fire for eternity.

## Alternate names for registers
On these parts (as shown above), the registers are normally accessed as members of a struct (eg, PORTA is a struct, PORTA.OUT is a member of that struct). This can in some cases be problematic. All registers that are members of a struct also have their own flat names in the i/o headers - replace the . with an _ - ie, PORTA.OUT is the same as PORTA_OUT.

## VPORT registers
If you were paying attention, you might have noticed that I said you could write to a pin in a single clock cycle - but writing it via PORTx.OUT will normally take at least 2, and usually 3. So how do we get those single cycle atomic writes and direction changes?

The normal port registers are at addresses starting at 0x400. This means they are far outside the range of the `sbi`, `cbi`, `in`, and `out` instructions. This can be an issue when porting code, for inline assembly, or when maximizing performance. Writes to addresses above 0x0040 either need a pointer register pair directed to them for `st` or use the 2-word `sts` instruction. Fortunately, to address this, each port has 4 `VPORT` registers - `VPORTx.OUT`, `VPORTx.IN`, `VPORTx.DIR`, and `VPORTx.INTFLAGS` - these are aliases of the corresponding PORTx registers that are located in the most desirable real estate in the address space, the "low I/O space", and can be used with `sbi`, `cbi`, `in` and `out`. These are single-clock, single word instructions that set or clear a bit, or load and store the value. The compiler will automatically use these when it can when it sees them written with the |= or &= operators. The requirements are that both the port and bit must be known at compile time and constant, and only a single bit must be set at a time. They will otherwise compile to a read-modify-write cycle that is not atomic, albeit a slightly faster one. Use the DIRSET, DIRCLR, OUTSET, OUTCLR, or OUTTGL registers in PORTx to manipulate multiple pins at once.

Writing 1 to a bit in VPORTx.IN will toggle the pin state if the pin is an output (unlike PORTx.IN, where writing does nothing). Similarly, if you test against a specific bit within one of them (ex: `if(VPORTA.IN & (1 << 6))` it will compile down to `sbrc` or `sbis` (Skip if Bit in I/o register Set/Cleared), another bit-level access instruction for the low I/O registers, which takes 1 clock cycle plus 1 for each word that it skips.

Unlike the classic AVRs, sbi/cbi takes 1 clock cycle, not 2.

```c++
PORTA.DIR      |= 1<<4; // Not interrupt safe. 6 clock cycles, 10 bytes (lds, ori, sts)
PORTA.DIRSET    = 1<<4; // Interrupt safe. 3 clock cycles, 6 bytes (ldi, sts)
VPORTA.DIR     |= 1<<4; // Interrupt safe. 1 clock cycle, 2 bytes (sbi)
// Also:
// Assignment is slower than single bit-set/clear!
VPORTA.DIR      = 1<<4; // Replaces current value, 2 clock cycles, 4 bytes (ldi, out)

// And cbi/sbi is one bit at a time only:
VPORTA.DIR     |= (1 << 4) | (1 << 5); // Not interrupt safe. 3 clock cycles, 6 bytes (in, ori, out)

// while the ___SET/___CLR ones are not:
PORTA.DIRSET    = (1 << 4) | (1 << 5); // Interrupt safe. 3 clock cycles, 6 bytes (ldi, sts)

// It must be known at compile time which pin you're doing it to, and whether you are setting or clearing it, in order to become a `cbi`/`sbi` instruction:
// where pin_bm is (1 << pin_bit_in_port) and not constant.
VPORTA.DIR     |= pin_bm; // Not interrupt safe! 3 clock cycles, 6 bytes (in, or, out) - not counting setting up the variable.
PORTA.DIRSET    = pin_bm; // Interrupt safe. 2 clock cycles, 4 bytes (sts) - not counting setting up the variable
```

### Don't generalize VPORT rules to other registers
The restrictions like setting/clearing one bit at a time being faster and interrupt safe, or assignment being slower than flipping one bit is *only true of the VPORT registers and the 4 GPIOR/GPR registers*. 4 VPORTs per port, times up to 7 ports is 28 VPORT registers, plus 4 GPIOR registers is 32, which is also the size of the low I/O space. The GPIOR/GPR registers have no function other than being in the low I/O space, so this is the only place they apply.

## Equivalents to classic AVR registers
Classic AVR |  modern AVR |  VPORT
------------|-------------|-------------
PORTx       | PORTx.OUT   | VPORTx.OUT
PINx        | PORTx.IN    | VPORTx.IN
DDRx        | PORTx.DIR   | VPORTx.DIR

**NOTE** Unlike classic AVRs, setting the bit in PORTx.OUT while pin is set as an INPUT will *NOT* enable the pullups. Only the PORTx.PINnCTRL registers can do that. There is no `VPORT` register that allows changing pullup status.
