# Interrupts vectors and DxCore
Use of advanced functionality of these frequenctly requires use of interrupts.

## What is an interrupt
As the name implies, an interrupt is something that can cause the currently running code to stop in it's tracks. The location of the instruction it was about to execcute is pushed onto the stack, and it then jumps to a specific "interrupt vector" near the start of the flash. This in turn is a jump (or rjump for parts with 8k or less of flash) the Interruipt Service Routine (ISR). This runs, and then returns to the code that was interrupted through a RETI instruction. Almost every peripheral can generate at least one interrupt, and most can generate several. See the datasheet for more information on what conditions they can be generated.

## Creating an ISR
There are two ways that you may end up writing an ISR, but the same considerations described at the bottom of this page apply to both of them. The first most Arduino users will see is an `attachInterrupt()` function or method - these take a function name as an argument. Somewhere in the core or library is the ISR itself, which checks if you've attached one, and calls it if so. This is simpler - where it's an option - though naturally the performance suffers as there's another layer of calls and returns, and a larger minimum number of registers that the ISR will have to save and restore (see the notes below). The other way is directly - you declare a function, but instead of the name, you use the ISR() macro with the vector name as it's argument:

```
ISR(CCL_CCL_vect) {
  //try to keep this part fast.
}
```

## List of interrupt vector names

If there is a list of the names defined for the interrupt vectors is present somewhere in the datasheet, I was never able to find it. These are the possible names for interrupt vectors on the parts supported by megaTinyCore. Not all parts will have all interrupts listed below (interrupts associated with hardware not present on a chip won't exist there). An ISR is created with the ISR() macro.

**WARNING** If you misspell the name of a vector, you will get a compiler warning BUT NOT AN ERROR! Hence, you can upload the bad code... in this case the chip will freeze the instant the ISR you thought you assigned is called, as it jumps to BAD_ISR, which in turn jumps to the reset vector... but since the interrupt flag never gets cleared, as soon as interrupts are enabled, it will do it all over again, so the chip will be hung. Encountering this (and the annoying lack of a nice list anywhere outside of the io.h) was the impetus for creating this list.


| Vector Name         | DA | DB | DD | Cleared By       | Notes                                               |
|---------------------|----|----|----|------------------|-----------------------------------------------------|
| `AC0_AC_vect`       | XX | XX | XX | Manually         |                                                     |
| `AC1_AC_vect`       | XX | XX |    | Manually         |                                                     |
| `AC2_AC_vect`       | XX | XX |    | Manually         |                                                     |
| `ADC0_RESRDY_vect`  | XX | XX | XX | Read result reg  |                                                     |
| `ADC0_WCMP_vect`    | XX | XX | XX | Manually         |                                                     |
| `BOD_VLM_vect`      | XX | XX | XX | Manually(?)      |                                                     |
| `CCL_CCL_vect`      | XX | XX | XX | Manually         | Check flags to see which triggered, like with PORT  |
| `CLKCTRL_CFD_vect`  |    | XX | XX | Manually         | Called when ext. clock fails, used by core for blink|
| `MVIO_MVIO_vect`    |    | XX | XX | Manually         | Called when MVIO enables or disables (due to vDDIO2)|
| `NMI_vect`          | XX | XX | XX | Reset            |                                                     |
| `NVMCTRL_EE_vect`   | XX | XX | XX | Write(?)         | Unclear if can clear, or is like DRE on USARTs      |
| `PORTA_PORT_vect`   | XX | XX | XX | Manually         |                                                     |
| `PORTB_PORT_vect`   |  X |  X |    | Manually         |                                                     |
| `PORTC_PORT_vect`   | XX | XX | XX | Manually         |                                                     |
| `PORTD_PORT_vect`   | XX | XX | XX | Manually         |                                                     |
| `PORTE_PORT_vect`   |  X |  X |    | Manually         |                                                     |
| `PORTF_PORT_vect`   | XX | XX | XX | Manually         |                                                     |
| `PORTG_PORT_vect`   |  X |  X |    | Manually         |                                                     |
| `PTC_PTC_vect`      | XX |    |    | Handled by QTouch| All aspects of PTC only handled by QTouch library   |
| `RTC_CNT_vect`      | XX | XX | XX | Manually         | Two possible flags, CNT and OVF                     |
| `RTC_PIT_vect`      | XX | XX | XX | Manually         | Time to first PIT int is random from 0 to period    |
| `SPI0_INT_vect`     | XX | XX | XX | Depends on mode  | 2 or 4 flags, some autoclear, some dont             |
| `SPI1_INT_vect`     | XX | XX | XX | Depends on mode  | 2 or 4 flags, some autoclear, some dont             |
| `TCA0_CMP0_vect`    | XX | XX | XX | Manually         | Alias: `TCA0_LCMP0_vect`                            |
| `TCA0_CMP1_vect`    | XX | XX | XX | Manually         | Alias: `TCA0_LCMP1_vect`                            |
| `TCA0_CMP2_vect`    | XX | XX | XX | Manually         | Alias: `TCA0_LCMP2_vect`                            |
| `TCA0_HUNF_vect`    | XX | XX | XX | Manually         | Split Mode                                          |
| `TCA0_OVF_vect`     | XX | XX | XX | Manually         | Alias: `TCA0_LUNF_vect`                             |
| `TCA1_CMP0_vect`    |  X |  X |    | Manually         | Alias: `TCA1_LCMP0_vect`                            |
| `TCA1_CMP1_vect`    |  X |  X |    | Manually         | Aloas: `TCA1_LCMP1_vect`                            |
| `TCA1_CMP2_vect`    |  X |  X |    | Manually         | Alias: `TCA1_LCMP2_vect`                            |
| `TCA1_HUNF_vect`    |  X |  X |    | Manually         | Split Mode                                          |
| `TCA1_OVF_vect`     |  X |  X |    | Manually         | Alias: `TCA1_LUNF_vect`                             |
| `TCB0_INT_vect`     | XX | XX | XX | Depends on mode  | Two flags: CMP on read ccmp in capt mode, OVF Manual|
| `TCB1_INT_vect`     | XX | XX | XX | Depends on mode  | Two flags: CMP on read ccmp in capt mode, OVF Manual|
| `TCB2_INT_vect`     | XX | XX |  X | Depends on mode  | Two flags: CMP on read ccmp in capt mode, OVF Manual|
| `TCB3_INT_vect`     |  X |  X |    | Depends on mode  | Two flags: CMP on read ccmp in capt mode, OVF Manual|
| `TCB4_INT_vect`     |  X |  X |    | Depends on mode  | Two flags: CMP on read ccmp in capt mode, OVF Manual|
| `TCD0_OVF_vect`     | XX | XX | XX | Manually         |                                                     |
| `TCD0_TRIG_vect`    | XX | XX | XX | Manually         |                                                     |
| `TWI0_TWIM_vect`    | XX | XX | XX | Usually Auto     | See datasheet for list of what clears it            |
| `TWI0_TWIS_vect`    | XX | XX | XX | Usually Auto     | See datasheet for list of what clears it            |
| `TWI1_TWIM_vect`    | XX | XX |    | Usually Auto     | See datasheet for list of what clears it            |
| `TWI1_TWIS_vect`    | XX | XX |    | Usually Auto     | See datasheet for list of what clears it            |
| `USART0_DRE_vect`   | XX | XX | XX | Write/Disable    | ISR must write data or disable interrupt            |
| `USART0_RXC_vect`   | XX | XX | XX | RXCIF, on read   | Error flags, if enabled only clear manually         |
| `USART0_TXC_vect`   | XX | XX | XX | Manually         | Often polled and not cleared until next write       |
| `USART1_DRE_vect`   | XX | XX | XX | Write/Disable    | ISR must write data or disable interrupt            |
| `USART1_RXC_vect`   | XX | XX | XX | RXCIF, on read   | Error flags, if enabled only clear manually         |
| `USART1_TXC_vect`   | XX | XX | XX | Manually         | Often polled and not cleared until next write       |
| `USART2_DRE_vect`   | XX | XX |    | Write/Disable    | ISR must write data or disable interrupt            |
| `USART2_RXC_vect`   | XX | XX |    | RXCIF, on read   | Error flags, if enabled only clear manually         |
| `USART2_TXC_vect`   | XX | XX |    | Manually         | Often polled and not cleared until next write       |
| `USART3_DRE_vect`   |  X |  X |    | Write/Disable    | ISR must write data or disable interrupt            |
| `USART3_RXC_vect`   |  X |  X |    | RXCIF, on read   | Error flags, if enabled only clear manually         |
| `USART3_TXC_vect`   |  X |  X |    | Manually         | Often polled and not cleared until next write       |
| `USART4_DRE_vect`   |  X |  X |    | Write/Disable    | ISR must write data or disable interrupt            |
| `USART4_RXC_vect`   |  X |  X |    | RXCIF, on read   | Error flags, if enabled only clear manually         |
| `USART4_TXC_vect`   |  X |  X |    | Manually         | Often polled and not cleared until next write       |
| `USART5_DRE_vect`   |  X |  X |    | Write/Disable    | ISR must write data or disable interrupt            |
| `USART5_RXC_vect`   |  X |  X |    | RXCIF, on read   | Error flags, if enabled only clear manually         |
| `USART5_TXC_vect`   |  X |  X |    | Manually         | Often polled and not cleared until next write       |
| `ZCD0_ZCD_vect`     | XX | XX |    | Manually         |                                                     |
| `ZCD1_ZCD_vect`     |  X |  X |    | Manually         |                                                     |
| `ZCD2_ZCD_vect`     |  X |  X |    | Manually         |                                                     |
| `ZCD3_ZCD_vect`     |    |    | XX | Manually         | Yeah for whatever reason they have ZCD3 only on DDs |

`XX` indicates available on at least three of the four pincounts that series is available in (ex: PORTF is on 20, 28 and 32-pin DD)

` X` indicates available on only one or two of the four sizes that series is available in (ex: TCA1 is only on 48 and 64 pin DA or DB)

## Additional notes

### Clearing flags - why so complicated?
Almost all flags *can* be manually cleared - the ones that can be cleared automatically generally do that to be helpful:
* when the purpose of the flag is to tell you that something is ready to be read, reading it clears the flag. ADC, serial interfaces, and TCB input capture do that.
* The TWI interrupts work the same way - you need to read, write, or ack/nack something to respond to the bus event; doing so clears the flag too.
* Sometimes interrupts like that can have error flags that can trigger them enabled too; those typically have to be manually cleared - by enabling them, you declare an intent to do something about them, so you're responsible for telling the hardware you did it.
* USART, and buffered SPI have DRE interrupt that can only be cleared by writing more data - otherwise you need to disable the interurpt from within the ISR. The TXC (transfer/transmit complete) flags are freqently polled rather than used to fire interrupts. It's not entirley clear from the datasheet if the EEPROM ready interrupt is like that, or can be cleared manually.
* The NMI is a very special interrupt; it can be configured to be a normal interrupt *or* a Non-Maskable Interrupt. In NMI mode, the part will sit there running the interrupt instead of almost-working with damaged firmware - which could potentially create a dangerous situation if it was part of a life-saftety critical device, like the controller for your antilock breaks. No matter what the damaged firmware tries to do, it cannot disable or bypass the NMI. Onlu loading working firmware and resetting it will clear the NMI. This is of particular relevance in life-safety-critical applications which these parts (but NOT this software package nor Arduino in genberal) are certified for. Not something likely to be used in Arduino-land.

### Vectors linked to many flags
There are a few vectors with a lot of flags that can trigger them. For example, each of the PORT interrupts has 8 flags that can trigger it. One hazard with these is that if you have a large number enabled - especially if your ISR is longer than it ought to be - that interrupts could fire whikle the ISR is running. You need to make sure you aren't missing those:

```c++
ISR(PORTA_PORT_vect) {
  if (VPORTA.INTFLAGS & (1 << 0)) {
    doSomething();
  }
  if (VPORTA.INTFLAGS & (1 << 1)) {
    doSomethingElse();
  }
  VPORTA.INTFLAGS=VPORTA.INTFLAGS //WRONG - if an interrupt happened after it's conditional, it would be missed.
}
```

```c++
ISR(PORTA_PORT_vect) {
  byte flags=PORTA.INTFLAGS; //Note: slower than VPORT!
  if (flags & (1 << 0)) {
    doSomething();
  }
  if (flags & (1 << 1)) {
    doSomethingElse();
  }
  PORTA.INTFLAGS=flags; // Better... if you care whether one of those conditions happens again, though, you could still miss it.
}
```
```c++
ISR(PORTA_PORT_vect) {
  byte flags=VPORTA.INTFLAGS;
  PORTA.INTFLAGS=flags; // Very common approach
  if (flags & (1 << 0)) {
    doSomething();
  }
  if (flags & (1 << 1)) {
    doSomethingElse();
  }
}
```
Another approach
```c++
ISR(PORTA_PORT_vect) {
  // Only handles one interrupt source per call of the ISR
  if (VPORTA.INTFLAGS & (1 << 0)) {
    VPORTA.INTFLAGS |= (1 << 0);
    doSomething();
  }   // Could be made into an else-if in order to let other interrupts fire if your ISR
      // is slow, and likely to be called with multiple flags set;
      // that case goes particularly well with round-robin interrupt scheduling option.
  if (VPORTA.INTFLAGS & (1 << 1)) {
    VPORTA.INTFLAGS |= (1 << 1);
    doSomethingElse();
  }
}
```


Note: `if (VPORTx.INTFLAGS & (1 << n))` is a maximally efficient way to test for a bit in a `VPORTx.anything` register or one of the 4 GPIORn/GPIOn/GPR.GPRn. Those registers (like many assorted important registers on classic AVRs - and unlike any other registers besides VPORTs and GPR/GPIOR/GPIO registers (over the past 6 years, the've been known by every one of those officially; based on current trends, by the time we get the EA series, we should have a yet another name for them, and by the tinyAVR 4-series, maybe we will be writing to `GOGO1`))

### Reminders
* ISRs should run FAST. Minimize the time that the code spends in the ISR. Never use polling loops, and avoid writing to serial. Most interrupts should just set a flag that is checked elsewhere, and do what must be done that instant (eg, reading from certain hardware registers). If you absolutely must write something to serial, maybe for debugging - can you make it a single character? `Serial.write('*');` is significantly less bad than `Serial.print("Int1 fired");`
* Read the datasheet, particularly relating to the relevant INTFLAGS register - make sure you understand when it is and is not cleared automatically, and when you need to clear it. And be sure you do so.
* Any global variable that an interrupt changes, if used outside the ISR, must be declared volatile - otherwise the compiler may optimize away access to it, resulting in code elsewhere not knowing that it was changed by the ISR.
* Any global variable read by the ISR and written to by code outside of the ISR which are larger than a byte must be written to with interrupts disabled - if the interrupt triggers in the middle of a write, the ISR would see a corrupted value.


### A bit more on timing
So as described above, execution reaches the ISR within 6 system clock cycles (5 on 8k or less parts); Then the interrupt has to take special measures to save the state of what was interrupted, and then restore it at the end - this is automatically done by AVR-GCC at the beginning and end of an ISR (these are called the prologue and epilogue); the compiler must assume that every working register, plus the SREG, contains something that must be saved. These get saved by `push`ing them onto the stack. All of the needed registers are freed up this way before your code starts to actually execute (the more variables your ISR needs at a time, the more it needs to `push` onto the stack, prolonmging the delay before your code begins executing. At least on the modern AVRs, `push` is only 1 clock cycle (in the past, it was two). Of course, saving the state is only half of the job - after running the ISR that you wrote, the epilogue has to `pop` all those registers off the stack - this takes two clocks a piece. So while the datasheet loves to talk about 6 clocks to enter the interrupt routine - even the bare minimum is 11: 6 to get there, then push r1, and r0, load SREG into one of those, push that onto the stack, and then zero out r1 (gcc needs a known zero register, even though it often doesn't always use it when it could). So 11 clocks to enter, and 7 to restore the minimal variables and 4 for the reti - 11 on either end.... plus the overhead of any registers your code uses. In DxCore or megaTinyCore, when a TCB is used for millis, all the ISR does is load the millisecond count, add 1 to it, and save it (that's all it does!) requires 17 clocks in the prologue, 12 more to load the current millis tally, 3 to increment it, 8 more to save it, 3 to clear the interrupt flag, then the 4 registers we pushed need to be restored taking 8 clocks plus the 11 standard ones. That ISR wind up taking 17 + 26 + 19 = 62 clock cycles! This amplification of execution time is a big part of why everyone always tell folks to make the ISRs fast. What often isn't mentioned is the importance of mimnimizing the number of simultaneously used variables.

If you're desperate for speed - or space - and if all you are doing is setting a flag, you can use one of the general purpose registers:GPR.GPR0/1/2/3 - the only place the core uses those is when using a bootloader, the reset cause is stashed in GPR.GPR0 (you can clear in setup: `GPR.GPR0 = 0` What is magic about the GPRs is that they are in the low I/O space. So something like `GPR.GPR1 |= (1 << n)` where n is known at compile time, is a single clock operation which consumes no registers - it gets turned into a `sbi` - set bit index. The same goes for `GPR.GPR1 &= ~(1 << n)`  - these are also atomic (an interrupt couldn't interrupt them like it could a read-modify-write). There are analogous instructions that make `if(GPR.GPR1 & (1 << n))` and `if (!(GPR.GPR1 & (1 << n))` lightning fast tests. But this only works for 1 bit at a time! (`GPR.GPR1 |= 3` is a 3 clock read-modify-write operation) (though you can read or write to them in one clock, instead of 2 or 3 like most memory locations.)
