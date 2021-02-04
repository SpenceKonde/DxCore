# Direct Port Manipulation
When using Arduino functions, like digitalWrite and digitalRead, is too slow, there are two ways to get much faster access to I/O pins. The first (and classic) method is to directly access the registers that control the pins; the second method is the digitalWriteFast() and digitalReadFast() calls (see bottom). Each port contains up to 8 pins, numbered 0 through 7 (see the pinout charts). For example, on the ATtiny1616  is located on PORTA, bit 4, commonly written as PA4. On the AVR Dx-series parts, the numbering is even simpler than the tinyAVR ones, since the numbering *always* starts from PA0, and no pins are ever out of order (though there are often pins missing in the middle - for example, the 32-pin parts start with PA0-PA7, and end with PF0-PF5 plus the reset pin on PF6 - but they have no PORTB or PORTE at all). You can always get the Arduino pin number from the port/pin through defines of the form `PIN_PA4` - not to be confused with `PIN_A4` (which is defined to something else) or `PA4` (which is ALSO defined as something else)

 The PORT registers on the modern AVR parts work a little differently from classic AVR microcontrollers (like ATmega328p, ATtiny85, etc), though the concept is the same. Each port has a structure available named `PORTx`   (ex, `PORTA`, `PORTB`, `PORTC`, etc).


DIR is the register which determines if the pin is an input or an output (like DDRx registers in classic AVR). After reset, all pins are set to input (0 in the register). In order to use the pin as an output the bit in the register must be set to 1, which can be done as follows:
```
    PORTA.DIR = PIN4_bm
```
This will set bit 4 on PORTA - if using an ATtiny1616, for example, that would correspond to Arduino pin 0.
Multiple pins on the same port can be configured by writing each bit, or by setting multiple bitmasks (`_bm` is a macro for a bit mask):
```
    PORTA.DIR = 0b01001000;         // sets PA3 and PA6 as an output
    PORTA.DIR =  PIN3_bm | PIN6_bm;  // sets PA3 and PA6 as an output
    PORTA.DIR = (1 << 3) | (1 << 6)  // sets PA3 and PA6 as an output
```

Besides setting the pins to an input (0) or output (1) in the DIR register, you can also use the DIRSET and DIRCLR registers to set (set as output) or clear (set as input) a specific pin:
```
    PORTA.DIRSET = PIN4_bm; // use PA4 as an output
    PORTA.DIRCLR = PIN4_bm; // use PA4 as an input
```

You can even toggle between an input or output by writing to the DIRTGL register.

Turning the pin on and off is done with the OUT register (this works like the PORTx register of classic AVR). Writing a 1 to the corresponding pin will set an output pin HIGH while a 0 will set it LOW:
```
    PORTA.OUT |= PIN4_bm; // write PB4 high
    PORTA.OUT &=~PIN4_bm; // write PB4 low
```
Note that the two examples above for flipping a single bit are not atomic - it's a read-modify-write operation. If an interrupt fires between the read and the write, the change that the ISR made will be reverted - if you have ISRs that are flipping pins (Servo.h does this), lines like those shown above would have to be be performed with interrupts disabled (just like classic AVR). Fortunately, the megaAVR architecture has a better solution - the OUTSET and OUTCLR registers, just like the DIRSET and DIRCLR registers described above - this is atomic:
```
    PORTA.OUTSET = PIN4_bm; // turn PA4 output on
    PORTA.OUTCLR = PIN4_bm; // turn PA4 output off
```
Or when you just want to toggle the output you can use:
```
    PORTA.OUTTGL = PIN4_bm; // toggle PA4 output
```
You can read the state of a pin by using the IN register (this is like the PINx register of classic AVR):
```
    bool status = PORTA.IN & PIN5_bm;
```
Unlike the classic AVRs, setting a pin HIGH with the OUT register while it is set as an input will not turn on the internal pullup. If you want to use the internal pullup resistor, you can set this in the PINnCTRL register as follows:
```
    PORTA.PIN6CTRL |= PORT_PULLUPEN_bm; // use the internal pullup resistor on PA6
    PORTA.PIN6CTRL &= ~PORT_PULLUPEN_bm; // don't use the internal pullup resistor on PA6
```
Note that this does mean that each pin has its own PINnCTRL register - unlike the classic AVRs where there was one register to control pullup for each port, with one bit per pin. The rest of the PINnCTRL register is used to configure pin interrupts on the pin. If not using pin interrupts with this pin, you can do:
```
    PORTA.PIN6CTRL = PORT_PULLUPEN_bm; // use the internal pullup resistor on PA6
    PORTA.PIN6CTRL = 0; // don't use the internal pullup resistor on PA6
```
## Alternate names for registers
On these parts (as shown above), the registers are normally accessed as members of a struct (eg, PORTA is a struct, PORTA.OUT is a member of that struct). This can in some cases be problematic when porting code. The compiler libraries also provide flat names for all of these registers - replace the . with an _ - ie, PORTA.OUT is the same as PORTA_OUT.


## VPORT registers
The normal port registers are at addresses starting at 0x400. This means they are outside the range of the `sbi`, `cbi`, `in`, and `out` instructions. This can be an issue when porting code, or when writing assembler for these parts. Fortunately, to address this, each port has 4 VPORT registers - VPORTx.OUT, VPORTx.IN, VPORTx.DIR, and VPORTx.INTFLAGS - these are aliases of the corresponding PORTx registers, and can be used with `sbi`, `cbi`, `in` and `out` (the compiler will do this automatically). Writing 1 to a bit in VPORTx.IN will toggle the pin state if the pin is an output. Using the VPORTx registers in a way that can compile to `cbi` or `sbi`, like using the `PORTx.OUTSET` and similar, is atomic (you don't have to disable interrupts if an interrupt might also change the same register like you would with a read-modify-write cycle). Similarly, if you test against a specific bit within one of them (ex: `if(VPORTA.IN & (1 << 6))` it will compile down to `sbrc` or `sbis` (Skip if Bit in I/o register Set/Cleared), another bit-level access instruction for the low I/O registers.


```
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

// And finally, never ever do this with the ___SET/___CLR registers:
PORTA.DIRCLR    =   1 << 4;     // What you meant to do
VPORTA.DIR     &= ~(1 << 4);    // does the same thing
PORTA.DIRCLR   |=   1 << 4;     // NOT the same!
// Were you expecting this to clear bit 4 only? It will clear ALL bits currently set!
// it will read PORTA.DIRCLR, which always reads as PORTA.DIR, bitwise or with 1 << 4, and then write the result back.
// |= and &= should never be used with SET/CLR/TGL registers, only normal registers (with the knowledge that it's not interrupt safe to do so), and the VPORT registers, where it is interrupt safe if and only if a single compile time known bit is being set or cleared.

```


### Don't generalize VPORT rules to other registers
The restrictions like setting/clearing one bit at a time being faster and interrupt safe, or assignment being slower than flipping one bit is only true of the VPORT registers and the 4 GPIOR/GPR registers (which have no function other than being an unused register that works with those instructions).

## Equivalents to classic AVR registers
Classic AVR |  modern AVR |  VPORT
------------ | ------------- | -------------
PORTx | PORTx.OUT | VPORTx.OUT
PINx  | PORTx.IN | VPORTx.IN
DDRx  | PORTx.DIR | VPORTx.DIR

**NOTE** Unlike classic AVRs, setting the bit in PORTx.OUT while pin is set as an INPUT will *NOT* enable the pullups. Only the PORTx.PINnCTRL registers can do that. There is no VPORT register that allows changing pullup status.

# Fast Digital I/O functions
Like several other third party hardware packages, DxCore and megaTinyCore support Fast Digital I/O; this takes the form of a "Fast" version of `digitalRead()` and `digitalWrite()` - these require that the pin number be known at compile time (and will error if it's not). digitalWriteFast() is in fact just shorthand for a cbi/sbi on a VPORT register - though it looks nicer, lets you use Arduino pin numbers, and enforces correct syntax (whereas typing `VPORTA.OUT |= 1 << 3` you could put and =, or forget that you can only set one bit at a time - both of which compile without errors, even though they're not what you want). Similarly, digitalReadFast(), will usually compile down to sbic/sbis (assuming the compiler could do that if you'd put a test for `VPORT.IN & (1 << bit)`  there instead).

```
digitalWrite(MyPin,HIGH);       // Slowest (where MyPin is not constant)
digitalWrite(PIN_PA4,HIGH);     // Slow
PORTA.OUTSET    = 1 << 4;       // Fast (3 clocks)
digitalWriteFast(PIN_PA4,HIGH)  // Fastest (1 clock)
VPORTA.OUT      = 1 << 4;       // Fastest (1 clock)
```
