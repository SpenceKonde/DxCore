# Notes on peripherals and other things
This document lays out some general notes on the peripherals - some general, some advanced, and some particularly aimed at people who are used to classic AVRs...

## With a #if, you can test for `_bm`, `_bp`, and `_gm` constants **but NOT `_gc` ones**!
The `_gc` constants are implemented as enums, not #defines. The preprocessor does not know how to deal with them - or rather, it deals with them precisely how it deals with all other "Identifiers which are not macros" - they are treated as equal to zero. It is not an error condition. More specifically, what you can and cannot do:

```
#define TCA0_PINS    PORTMUX_TCA0_PORTC_gc     // this is OK - and TCA0_PINS gets replaced with PORTMUX_TCA0_PORTC_gc
                           // (which is a constant, 2 in this case), when it gets substituted into C/C++ code.

#ifdef TCA0_PINS           // SOMETHING is defined, yup this is okay too
  conditionallyCompiled();
#endif

#if defined(TCA0_PINS)     // Same as the #ifdef
  conditionallyCompiled();
#endif
#if TCA0_PINS == 2         // However, here the preprocessor does not know the value of TCA0_PINS, because
  conditionallyCompiled(); // PORTMUX_TCA0_PORTC_gc is an enum; so while TCA0_PINS is defined, it is defined as
#endif                     // an enum; which as stated above is not defined as far as the preprocessor is concerned.
                           // So it treats TCA0_PINS as 0, and instead of this resolving to #if 2 == 2 and including
                           // the conditionally-compiled code, it turns into 0 == 2, and that code is not included.
```
The creators of the preprocessor considered that "shortcut" to be a "feature" - and so even -Wall does not warn about it. Since it is all but certain to be a bug in Arduino-land, I have enabled -Wundef. when any warnings are enabled. The use of enums for the group code constants is incredibly inconvenient for this reason. And you VERY frequently want to test for them too!

## Interrupts

### Don't let your code try to call an ISR that doesn't exist

If you don't have an ISR defined, it will go to the "bad interrupt" handler, which jumps to the reset vector... This puts your sketch into a guaranteed broken state: The INTFLAG is still set, and the interrupt is still enabled - but interrupts won't be firing because CPUINT thinks you're already in an interrupt because the `reti` instruction (RETurn from Interrupt) was never executed... The result is generally completely broken sketch behavior. So don't let that happen!

### Flags MUST be cleared in (many) interrupts
Unlike classic AVRs, you often must clear the interrupt flag in the ISR by writing a 1 to it. Not for all interrupts, though. In some cases, there is a specific condition that will result in the flag being cleared automatically (often something that is directly related to it, like reading the CAPTURE register for TCB input capture, or reading the ADC result for an ADC result ready interrupt. In a few cases (such as DRE for serial or buffered SPI), you can't control thestate of the flag at all, and can only enable or disable the interrupt). This is all described on a case-by-case basis in the datasheet where the INTFLAG registers in questoin is described
 Failing to do so can produce surprising results, because the processor doesn't *halt* - it just runs agonizingly slowly (plus whatever the interrupt does keeps happening - but this may not be as obvious) - because at least one instruction will always happen between interrupts.

Here's a sketch that demonstrates this, and another interesting thing; the loop waiting for the ISR is exited as soon as it fires the first time... but it keeps running continually, letting the while loop after it run one instruction for each time the ISR runs. Both of those unsigned ints cause the ISR and while() loop to toggle the pins more slowly, so you can see how fast the two are running relative to each other with even really crude means.

```c++
volatile unsigned int test=0;
unsigned int test2=0;
void setup() {
  PORTA.DIR=0xC0;
  TCA0.SINGLE.CTRLA=0x0F; //TCA0 1024 prescaler, Running
  TCB0.CTRLA=0; TCB0.CTRLB=0; TCB0.INTCTRL=1; //Stop TCB0, Periodic Interrupt timing mode, interrupt on
  TCB0.CNT=0;
  TCB0.CCMP=F_CPU/1024; // Set TOP to number of clocks until overflow
  TCB0.CTRLA=0x05;
  while(!test);
  while(1){
    __asm__ __volatile__("nop");
    if (!test2++) PORTA.OUTTGL=0x40;
  }
}
void loop () {}
ISR(TCB0_INT_vect){
  if (!test++) PORTA.OUTTGL=0x80;
  //TCB0.INTFLAGS=1; //Should be here!
}

```

As it happens the "test2" PA7 transitions 10 times for each time PA6 does - and sure enough in the assembly listing (sketch -> export compiled binary with this core exports it to sketch folder!), there are 10 instructions. **And the ISR takes a whopping 50 instructions!** (Lots of POPs and the RETI at the end; loading a byte from RAM into registers is also 3 cycles, and storing it back is 2 more. So it is surprisingly slow). Just like you'd expect (`20,000,000/(2^16*51)`), it transitionas once per 167mS or so.

#### Corollary
Accessing variables in an ISR is extra expensive because it has to save and restore the register used hold it's variable, since it doesn't know what it might be interrupting so it has to assume all registers are used and preserve them. The prologue and epilogue of an ISR is surprisingly costly. In this example, all we're doing is incrementing one unsigned integer, testing if the result is 0 and if it is executing a single instruction, and we're at 50 clock cycles! Well, yeah... loading that 2 byte global is 2x LDS, and saving it is 2x STS (2x3 + 2x2), the math and test is 4?, then the normal required prologue and epilogue is 3 push, 1 in, 1 clr, 3 pop, 1 out - another 12, and we needed 3 registers? So there's another 3 push 3 pop, 9 more clocks there, reti is 4, it takes 5 to actually start executing the ISR at all (2 to store the program counter to stack and three to execute the jmp from the vector to the routine - so without even having to look at the assembly listing, I can count up 40 clocks. Your brain should translate this to an extra-bold underline under the frequent admonation to **keep ISRs as small and short as possible** and that that includes the number of variables they access!


### Watch out for the optimizer...
You may be wondering about the nop instruction there.
That's because there's another wacky thing demonstrated there: The compiler can get awfully clever when optimizing - as in, too clever. Without that, it will perform loop unrolling - replacing the loop with with all the values test2 will hold:

This:
```c++
while(1){
  if (!test2++) PORTA.OUTTGL=0x40;
}
```
Becomes:
```c++
if (!0) PORTA.OUTTGL=0x40;
if (!1) PORTA.OUTTGL=0x40;
if (!2) PORTA.OUTTGL=0x40;
...
if (!65534) PORTA.OUTTGL=0x40;
if (!65535) PORTA.OUTTGL=0x40;
```
And the constants where the test for the if() evaluates to false (ie, 65535 of the 65536 cases) can be eliminated - they do nothing right?
Leaving:
```c++
while(1){
  PORTA.OUTTGL=0x40;
}
```

Which is why one normally writes busy-wait and cycle-counting stuff using inline assembly!
