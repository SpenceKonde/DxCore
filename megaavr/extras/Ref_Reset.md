# Reference: Resetting and Watchdog Timer
Various reset sources are available, I think the list is thus far the same on all modern AVRs. I have used all 6 types of reset several times minimum in the last few months, for various reasons. Except brown-out detect, which I rarely use since I haven'nt been deploying things to the field. It's a must for reliable operation in that case though.

## Determining reset cause
The cause of reset is recorded whenver any reset takes place. These flags are only cleared manually. On non-optiboot boards - that's it - you're responsible for reading and clearing them. Resets other than power on reset will not clear flags already set. If you are using Optiboot, when it jumps to the application (after bootloader runs normally if that reset cause is an entry condition), it will save the reset cause to the `GPIOR0` or `GPR0` register (both the new name, `GPR.GPR0` and the old style `GPIOR0` will work), and clear all of the flags. The 6 lower bits are used to record UPDI reset, Software Reset, Watchdog Reset, External Reset, Brownout Reset and Power-on Reset, in that order. Multiple can be set at once when multiple reset conditions occur. A BOD reset often happens as the power supply voltage is ramping up.

```c++
  /* Non-optiboot */
  uint8_t resetflags = RSTCTRL.RSTFR; // Read the flags.
  RSTCTRL.RSTFR = resetflags;         // Clear them by writing a 1 to them.
  /* Optiboot */
  uint8_t resetflags = GPR.GPR0;      // No special action required.
```

## Reset pin as Input
The Reset pin can be configured as an input. It will be `PIN_PF6` if this option is selected. It can never be made into an output, unlike on tinyAVR and most classic AVR parts. Unlike classic AVRs, *setting the reset pin as an input poses no particular difficulty for reprogramming*. This can be selected from the Tools -> Reset Pin menu; you must "burn bootloader" to apply the selection to a given chip - it is not set on normal uploads because it's on the same fuse as some non-safe ones.

## Triggering a reset from software
These parts support a native software reset operation; on classic AVRs, the only way was to enable the watchdog timer, and then wait for it to time out. With the modern AVR devices, there are two ways to trigger a reset from software: A watchdog timer reset (as before), and the native software reset. Unlike classic AVRs, after a WDT reset, the watchdog timer is not forced on with the minimum timeout (on classic devices, this is why doing a WDT restart with very old bootloaders instead hung the board - the bootloader wasn't smart enough to turn it off before it was reset by it.). These two methods of resetting the chip allow you to signal to the application or bootloader what sort of condition triggered the reset.

Additionally, while the bootloader, if used (see below) will run after a software reset, it will NOT run after a watchdog reset (well - it will run, but only long enough to read the reset flag register and see that it was restarted by the WDT: That means that either the bootloader just ran, finished, and reset the device (If we didn't jump to the app in this case, we'd just sit in the bootloader doing WDT resets forever), that the application suffered from a WDT timeout due to a bug or adverse conditions (that's not the bootloader's business to get involved in) or that the application intentionally triggered a WDT reset. None of those are "entry conditions" for Optiboot - so Optiboot jumps to the app immediately).This allows the application a convenient mechanism to restart itself without having to potentially wait through the bootloader attempting to communicate with whatever is connected to the serial port.

Note: While the windowed mode would at first seem to suggest that you could reset much faster by setting it and then executing `WDR` until it resets from missing the window, you don't gain nearly as much as you'd think. First, the initial WDR needs to be synchronized - 2-3 WDT clocks, ie, 2-3 ms. Additional WDRs executed while this is happening are ignored. Only when the second WDR makes it to the watchdog timer domain will it reset the system. So the overall time to restart is 6-9ms. Instead 10-11 ms (sync delay + minimum timeout).

```c++
void resetViaWDT() {
  _PROTECTED_WRITE(WDT.CTRLA,WDT_PERIOD_8CLK_gc); //enable the WDT, minimum timeout
  while (1); // spin until reset
}

```c++
void resetViaWDTFaster() {
  _PROTECTED_WRITE(WDT.CTRLA,WDT_WINDOW_8CLK_gc | WDT_PERIOD_8CLK_gc); //enable the WDT, minimum timeout, minimum window.
  while (1) __asm__ __volatile__ ("wdr"::); // execute WDR's until reset. The loop should in total take 3 clocks (the compiler will implement it as wdr, rjmp .-4), but because of the sync to the relatively slow
}

void resetViaSWR() {
  _PROTECTED_WRITE(RSTCTRL.SWRR,1);
}
```

## Using watchdog to reset when hung
If you only worked with the watchdog timer as an Arduino user - you might not even know why it's called that, or what the original concept was, and just know it as that trick to do a software reset on classic AVRs, and as a way to generate periodic interrupts. The "purpose" of a watchdog timer is to detect when the part has become hung - either because it's wound up in an infinite loop due to a bug, or because it wound up in a bad state due to a glitch on the power supply or other adverse hardware event, has been left without a clock by an external clock source failing, went to sleep waiting for some event which doesn't end up happening (or without correctly enabling whatever is supposed to wake it up) - and issue a reset. It is often anthropomorphized as a dog, who needs to be "fed" or "pet" periodically, or else he will "bite" (commonly seen in comments - the latter generally only when intentionally triggering it, as in `while (1); //wait for the watchdog to bite`).

You would first initialize the WDT like:
```c++
_PROTECTED_WRITE(WDT.CTRLA,settings); //enable the WDT
```

To configure the WDT to reset the device after a period of time, replace `settings` above with the desired WDT timeout period from this table. If is getting stuck somewhere that causes it to repeatedly reset the WDT you can configure the window mode to reset if an attempt is made to reset the watchdog timer within the specified period. To do this, bitwise OR the two, eg: ` _PROTECTED_WRITE(WDT.CTRLA, WDT_PERIOD_8KCLK_gc | WDT_WINDOW_16CLK_gc );` would set the WDT to reset the device if two attempts to reset the watchdog were ever made within 16 milliseconds (before the "window" opens), or if no reset was performed in the 8 seconds after that (when the window closes).


| Timeout | WDT period name      | WDT Window name      |
|---------|----------------------|----------------------|
|  0.008s | WDT_PERIOD_8CLK_gc   | WDT_WINDOW_8CLK_gc   |
|  0.016s | WDT_PERIOD_16CLK_gc  | WDT_WINDOW_16CLK_gc  |
|  0.032s | WDT_PERIOD_32CLK_gc  | WDT_WINDOW_32CLK_gc  |
|  0.064s | WDT_PERIOD_64CLK_gc  | WDT_WINDOW_64CLK_gc  |
|  0.128s | WDT_PERIOD_128CLK_gc | WDT_WINDOW_128CLK_gc |
|  0.256s | WDT_PERIOD_256CLK_gc | WDT_WINDOW_256CLK_gc |
|  0.512s | WDT_PERIOD_512CLK_gc | WDT_WINDOW_512CLK_gc |
|  1.024s | WDT_PERIOD_1KCLK_gc  | WDT_WINDOW_1KCLK_gc  |
|  2.048s | WDT_PERIOD_2KCLK_gc  | WDT_WINDOW_2KCLK_gc  |
|  4.096s | WDT_PERIOD_4KCLK_gc  | WDT_WINDOW_4KCLK_gc  |
|  8.192s | WDT_PERIOD_8KCLK_gc  | WDT_WINDOW_8KCLK_gc  |

### Resetting the WDT
A typical use of this is to have the main loop (generally loop() in an Arduino sketch) reset the watchdog at the start or end of each loop, so when a function it calls ends up hung, we can use:

```c
// As a function
void wdt_reset() {
  __asm__ __volatile__ ("wdr"::);
}
```

Or

```c++
// as a macro (which is all that wdt.h does)
#define wdt_reset() __asm__ __volatile__ ("wdr"::)
```

### Disabling WDT
In some cases you may only want the WDT enabled when certain routines prone to hanging due to external conditions, and then turn it off again.
```c++
_PROTECTED_WRITE(WDT.CTRLA,0); //Yeah, that's it.
```

At the other extreme you may want it to be impossible for code, even the cleverest bugs, to turn it off. You can lock the WDT in it's current configuration by writing the WDT_LOCK bit in WDT.STATUS to 1 - only a system reset will unset the bit.

```c++
__PROTECTED_WRITE(WDT.STATUS,WDT_LOCK_bm); // call after setting WDT to desired configuration.
```
For even more protection (and more nuissance in keeping the WDT from biting at all times). you can set the WDTCFG fuse via UPDI programming. At startup, that value is copied to WDT.CTRLA, and the lock bit in WDT.STATUS is set. UPDI programming is the only thing that can undo the WDT if congfiguree through the fuses. This feature is not exposed by the core - you must manually run avrdude or SerialUPDI and use it to write that fuse. UPDI uploads do not set it (the current value is left in place), and "burn bootloader" will reset that fuse to 0 (the point of burn bootloader is initializing the chip to a fully known state; it only involves a bootloader if you've chosen an optiboot board configuration.

### Summary and mini-example
So overall, if you wanted your sketch to reset if you ever spent longer than 8 seconds between loop() iterations and also detect when a WDT reset just occurred and take special actions in setup you might do this
```c++
void wdt_enable() {
  _PROTECTED_WRITE(WDT.CTRLA,WDT_PERIOD_8KCLK_gc); // no window, 8 seconds
}

void wdt_reset() {
  __asm__ __volatile__ ("wdr"::);
}

void wdt_disable() {
  _PROTECTED_WRITE(WDT.CTRLA,0);
}

/* If you at some point plan to put the chip to sleep you need to turn off the WDT or it will reset you out of sleep... */
void goToSleep() {
  wdt_disable();
  sleep_cpu();
  wdt_enable();  // turn the WDT back on promptly when we awaken.
}

void loop() {
  wdt_reset(); // reset watchdog.
  <snip - rest of loop goes here>
}

/* for NON-OPTIBOOT */
void setup() {
  wdt_enable(); //we're super paranoid, so we turn on WDT immediately!
  uint8_t resetflags = RSTCTRL.RSTFR; // Read the flags
  RSTCTRL.RSTFR = resetflags; // clear them for next time (only power on reset will clear then otherwise)
  if (resetflags & RSTCTRL_WDRF) { //means it was a WDT reset
    NotifyUserOfHangAndWDTReset();
    if (resetflags != RSTCTRL_WDRF) {
      Serial.println("Weird, we clear the flags upon reset, how did we end up with more than just one reset cause flag?"); // far from unheardof.
    }
  }
  <snip - rest of your setup goes here>
}
/* For OPTIBOOT */
void setup() {
  wdt_enable(); //we're super paranoid, so we turn on WDT immediately.
  uint8_t resetflags = GPR.GPR0; // Optiboot stashes the reset flags here before clearing them to honor entry conditions
  // GPR.GPR0 = 0; // no need to clear because this is reset at startup to 0 either way - unless you need it later..
  if (resetflags == RSTCTRL_WDRF) { //means it was a WDT reset. In optiboot configurations, this should always be accompanied by another flag unless it was a wdt reset from the application.
    NotifyUserOfHangAndWDTReset();
  }
  <snip - rest of your setup goes here>
}

```


## The wrong way to reset from software
I have seen people throw around `asm volatile("jmp 0");` as a solution to a need to reset from software. **Don't do that** - all compiled C code makes assumptions about the state from which it is run. Jumping to 0 from a running application violates several of them unless you take care to avoid those pitfalls (if I were to add a comment after that line, it would read something like `// Reset chip uncleanly to produce unpredictable results`. Both Optiboot and the init() method of megaTinyCore make assumptions about the state of peripheral registers (namely, that they're in their reset configuration when it starts). Resetting with a jumop to 0 was always risky business and should never be done on any part, ever (certainly not without taking a bunch of precautions). Now that we finally have a right way to do software reset, there is absolutely no excuse for using such a tactic!

## Wrong unindended not-quite-a-reset
There a couple of other situations that can result in sketch restarting. Sufficient noise (like if you had a large 5-foot-something broad band antenna, like a human body, touching the crystal case or pin it can trigger a reset and will almost certainly stop the clock; while in theory that should trigger the clock failure detection, it often seems to just reset instead. Overclocking far too hard will do it (though you'll notice that it has lost its ability to get the right answers to math at lower overclocks; if you keep cranking that up, then it will eventually get so far out of whack that it resets). Enabling a interrupt without an ISR and letting it trigger will reset it particularly uncleally, such that it will not reset any registers, just go back to the beginning, with the flag still set so it will happen again as soon as you reenable it. Don't enable interrupts without the ISR defined. Severely broken code or "wild pointers" can lead to a similar situation. Whenever this happens, reset flags will not be set (there was no reset) and the sketch will likely be misbehaving badly. If you are, say, making a tool to test how hard a chip can be overclocked, and expect dirty resets (as in tuning sketch on megaTinyCore) I do software resets afterall resets, reading, clearing reset flags each time. until that comes back as only flag (In my case I make an exception and accept a UPDI reset, because that means I just uploaded it, so I know it's a clean reset.
