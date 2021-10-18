# Startup order and callback-like-functions
These are not callbacks. Callbacks are very ineffecient on and AVR especialyl under the circumstances these are called (a callback would be where you _supply_ a functions which we then call) In C this means a function pointer, and saving and restoring large numbers of registers sicne it has top assume that the function uses every call-used register, and shoots the optimizert in the foot.

These are all weakly defined functions that can be overridden at compiletime with a different function definition. They are called at certain times, usually in initialization. If you override them you can make something different happen then.

They are shown here with their default implementation, if it's something you would likely be rewriting asnd not a gigantic blob straight out of ifdef hell..

## Startup Sequence

when execution reaches the application after reset or bootloader, it hits the reset vector first, andthen jumps where ver that is pointing and begins execution with the .init sections
```c++
/*
.init2: This initializes the stack and clears r1 (the known zero) that avr-gcc needs. No C code before here works correctly!
.init3: This calls the DxCore function _initThreeStuff()
.init4: For devices with > 64 KB of ROM, .init4 defines the code which takes care of copying the contents of .data from the flash to SRAM.
.init6: Used for constructors in C++ programs.
.init9: Jumps into main().

_initThreeStuff` is our internal very early initialization routine; it cannot be overridden., for stuff that needs to be set very early. If using app SPM wthout optiboot, this is where we tell it that the vectors start at 0 not 512.

If not using Optiboot, it then calls init_reset_flags() which checks the reset flags, and if none are set, fires a software reset. For protection against hangs and bootloops from dirty resets, you MUST either clear these flags as soon as possible in your code, or override this.
_onPreMain() ends by calling onPreMain(). Use this if you need to set anything up REALLY early - for example, if using attachInterrupt in manual made and you need to make sure the port is enabled before constructors.

Then the rest of the init sections run, followed by the jump to main()

main() calls init() for core initialization initVariant() which is used by a few libraries, then setup(), then repeatedly calls loop():
*/

int main() {
  onBeforeInit(); //Emnpty callback called before init but after the .init stuff. First normal code executed
  init(); //Interrupts are turned on just prior to init() returning.
  initVariant();
  if (!onAfterInit()) sei();  //enable interrupts.
  setup();
  for (;;) {
    loop();
    #ifdef ENABLE_SERIAL_EVENT /* this is never true unless core is modified */
      if (serialEventRun) serialEventRun();
    #endif
  }
}
```

If you for some reason end up directly putting functions into init 5, 7, or 8 be warned! the function must be declared with both `((naked))` and `((used))` attributes! The latter is required or the optimizert will just eliminate it. The former is needed to prevent it from generating a return instruction at the end. This causes a crash when it tries to return and there's nothing on the call stack to return to. This is all handled for you in the internal function that calles onPreMain - onPreMain is normal.

## Recommended override candidates

### init_reset_flags
```c
  void init_reset_flags() __attribute__((weak)) {
    if (RSTCTRL.RSTFR == 0){
      _PROTECTED_WRITE(RSTCTRL.SWRR, 1);
    }
  }
```
[The reset reference](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Reset.md) has recommended overrides and explanation of why this is so important.


```c
void onPreMain() __attribute__((weak)) {
  /* USER CODE THAT NEEDS TO RUN  WAY EARLY GOES HERE */
}
```


## less-recommended override candidates
None of these is expected to be particularly useful, but rather to give people debugging in despreate circumstances a way to not have to modify the core to debug their application problem.

### onBeforeInit
```c++
void onBeforeInit() __attribute__((weak));{;}
```
onBeforeInit is called  as the first code in main, useful if you need to do something AFTER class constructors have run but BEFORE initalization. No default action s taken.

### initVariant
```c++
void initVariant() __attribute__((weak)){;}
```
This is the ONLY one of these functions that is standard (other than init, setup, and loop, of course).


initVariant is meant for variant files to call, but none of them ever do that on  any core i've seen, and library authors use these for code that need s to run at startup.
**DO NOT OVERRIDE THIS** in the sketch - it is reseved for library/core/board authors. It is art of the Arduino API and is present on all cores as far as I know.


### onAfterInit
```c++

uint8_t onAfterInit() __attribute__((weak) {
  return 0;
}

```
onAfterInit runs just before setup is called, but after all other initialization. returning a non-zero value will prevent interrupts from being disabled. Good for debugging if you suspect that an interrupt is firing and wrecking everything as soon as they're enabled (from a flag rthat gets set during init() functions() and results in a bootloop so you can't otherwise get any info out. )


## Initializers of peripherals
```c
void init_reset_flags() __attribute__((weak)); // ensures reset controller has successfully reset the part
void init_clock()  __attribute__((weak)); // this is called first, to initialize the system clock.
void init_ADC0()   __attribute__((weak)); // this is called to initialize ADC0
void init_timers() __attribute__((weak)); // this function is expected to configure all timers for PWM. _init_millis() is called after this.
void init_TCA0()   __attribute__((weak)); // called by init_timers() - If you must override the init_timers() (try to do one of these instead)
void init_TCA1()   __attribute__((weak)); // called by init_timers() - you need to call init() methods for all timers you want analogWrite to work with
void init_TCBs()   __attribute__((weak)); // called by init_timers()
void init_TCD0()   __attribute__((weak)); // called by init_timers()
void init_millis() __attribute__((weak)); // called by init() after everything else and just before enabling interrupts and calling setup() - sets up and enables millis timekeeping.
```
They are called in the order shown above. They are in wiring,c and some are too long to put in here in full, but the point will still be described.

Any of them can be overridden - this is most useful for eliminating calls to save flash (overridden with an empty function, they will be optimized away) or when you need to initialize something differently and would rather start from scratch.  It is fine to override with empty function and then initiailize later depending on your needs.

Important to note that the timer init functions all behave differetly based on whether the timer is used for millis.

### init_clock
Initializes the system clock so that it will run at the F_CPU passed to it. Don't override this unless you can ensure that the F_CPU that is passed via the compiler command line will be correct. Overriding it may be useful in debugging exotic problems where you want to make sure that a problem isn't being caused by the clock speed (and accept that the timekeeping will be wrong while debugging - this is the sort of case where your question is "Does code executing get to this point at all? Is the chip working AT ALL?" when onPreMain() is getting called but nothign else is)

### init_ADC0
Initializes the ADC clock prescaler to get a legal frequency, and sets up ore defaults. As these are all configurable it is rarelty neeced to touch this. It can be overridden to prevent it from initializing the ADC to save flash, if the ADC is not used, or is used in a configuration very different from normal


### init_timers
Simply calls initTCA9, initTCA1, initTCBs, and initTCD0 in that order. No clear reason one would want to overrride


### init_TCAn
```c
void __attribute__((weak)) init_TCA0() {
  // Enable split mode
  TCA0.SPLIT.CTRLD = TCA_SPLIT_SPLITM_bm;
  TCA0.SPLIT.LPER    = PWM_TIMER_PERIOD;
  TCA0.SPLIT.HPER    = PWM_TIMER_PERIOD;
  // Anif this is the millis timer, we do a compiletime sanity check on the core (which is actually
  // a good test, catches many bugs), which also tells us which prescaler TCA0 needs to have.
  // notice that each speed bucket has an internal conditional that can #erroir but doesn't ipact the binary,
  // while one that does uses the correct prescaler for the target PWM frequencty (500-1500 Hz).
  #if (F_CPU > 30000000) //use 256 divider when clocked over 30 MHz.
    #if defined(MILLIS_USE_TIMERA0) && (TIME_TRACKING_TIMER_DIVIDER != 256)
      #error "wiring.c and timers.h want to set millis timer TCA0 to different divider"
    #endif
    TCA0.SPLIT.CTRLA   = (TCA_SPLIT_CLKSEL_DIV256_gc) | (TCA_SPLIT_ENABLE_bm);
  #elif (F_CPU > 5000000) //use 64 divider unless it's 5 MHz or under
    #if defined(MILLIS_USE_TIMERA0) && (TIME_TRACKING_TIMER_DIVIDER != 64)
      #error "wiring.c and timers.h want to set millis timer TCA0 to different divider"
    #endif
    TCA0.SPLIT.CTRLA   =  (TCA_SPLIT_CLKSEL_DIV64_gc) | (TCA_SPLIT_ENABLE_bm);
  #elif (F_CPU > 1000000) // anything above 1 MHz
    #if defined(MILLIS_USE_TIMERA0) && (TIME_TRACKING_TIMER_DIVIDER != 16)
      #error "wiring.c and timers.h want to set millis timer TCA0 to different divider"
    #endif
    TCA0.SPLIT.CTRLA   =  (TCA_SPLIT_CLKSEL_DIV16_gc) | (TCA_SPLIT_ENABLE_bm);
  #else
    #if defined(MILLIS_USE_TIMERA0) && (TIME_TRACKING_TIMER_DIVIDER != 8)
      #error "wiring.c and timers.h want to set millis timer TCA0 to different divider"
    #endif
    TCA0.SPLIT.CTRLA   =   (TCA_SPLIT_CLKSEL_DIV8_gc) | (TCA_SPLIT_ENABLE_bm);
  #endif
}

```
After takeOverTCA0(), if you then call releaseTCA0() to give it back, it will be given a hard reset command then this function gets called again too, too. It no longer touches the portmux here (that's done in init_timers(). There is an identical one for TCA1. TIME_TRACKING_TIMER_DIVIDER is the prescaler for millis - for TCA as millis (uncommon) it is 16 below 5 MHz, and 256 above 30 and otherwise always 64.

### init_TCBs
Initializes all the type B timers for PWM, except the one used for millis, if any. Can override this with nothing to get rid of the TCB initnialization for PWM if that's not used, but it will stll check those ppins in analogWrite, which is where most of the overhead associated with them is.


### init_TCD0
Initializes the type D timer.

### init_millis
Initialized and kicks off millis timekeeping. Overriding this is not recommended. Maybe as debugging aid when you don't want to go through code and comment out references to millis/micros but need to test something that can't be done if there are millis interrupts firtig.

Disabling millis from the tools submunui is MUCH better aoppriach.

Note that the ISR will still be defined, but this is the only place it gets enabled.


## Clock Failure Handling

```c
void onClockTimeout() __attribute__((weak)); // called if we try to switch to external clock, but it doesn't work. Default action is a blink code with 4 blinks.
void __attribute__((weak)) onClockTimeout() {
  _blinkCode(3);
}

void onClockFailure() __attribute__((weak)); // called by the clock failure detection ISR. Default action is a blink code with 4 blinks.
void __attribute__((weak)) onClockFailure() {
  _blinkCode(4);
}
```

These are called when the external clock source doesn't provide any clock - either it never does (onClockTimeout() ) or it does initially but then dies and gets called by the CFD ISR.
`_blinkcode()` never returns and blinks forever. Most likely to be overridden in the former case to run on internal and log an error, and in the latter case just to prevent the blink code because something else is connected to that pin.

See [The clock source reference](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Clocks.md) for examples of how to use it.
