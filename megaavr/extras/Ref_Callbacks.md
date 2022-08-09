# Startup order and callback-like-functions
These are not callbacks. Callbacks are not efficient in this context. However, weakly defined functions like these achieve the same effect, and do so efficiently. To do so you override them - typically you would override one that isn't an empty function with an empty function to disable that initialization step, or you would override ones that default to an empty function with code that needs to run at the appointed time.

A callback would be where you supply a function pointer which we then call (which would end up as an icall instruction). Doing such a thing is much more costly, because the compiler can't assume it is constant. Hence call-used registers (even if the function doesn't use them) get saved and restored, function call overhead cannot be avoided, and the compiler can't optimize them. We try to avoid these. They're only used when the function to be called can actually change at runtime, mainly when "attaching" interrupts.

**This document reflects the 1.3.7 release of DxCore**
Some of these were added in 1.3.7 (actually, quite a few of them were, which was the impetus for writing this)

## Startup Sequence
When execution reaches the application after reset or bootloader, it hits the reset vector first, and then jumps where ver that is pointing and begins execution with the .init sections
```text
.init2: This initializes the stack and clears r1 (the known zero) that avr-gcc needs. No C code before here works correctly!
.init3: This calls the DxCore function _initThreeStuff()
_initThreeStuff() is a creatively named internal early initialization routine; it cannot be overridden. but the functions it calls can be.
  init_reset_flags() - checks reset flags, and if none are set, fires software reset. You should clear those flags as soon as possible, ideally by overriding this as suggested.
  If SPM from app is enabled on non-optiboot configuration, sets the interrupt vector location to start from 0x0000 (in the fake bootloader section) instead of 0x0200 (the start of "application code")
  onPreMain() - can be used to provide code that needs to run before class constructors.
.init4: For devices with > 64 KB of ROM, .init4 defines the code which takes care of copying the contents of .data from the flash to SRAM.
.init6: Used for constructors in C++ programs.
.init9: Jumps to main()
main()
  onBeforeInit() - This is called first, before any initialization.
  init() - to initialize the peripherals so the core-provided functions work.
    init_clock() - This is called first, to initialize the system clock.
    init_ADC0() - This is called to initialize the ADC.
    init_timers() - This function calls the timer initialization functions
      init_TCA0()
      init_TCA1() - If present.
      init_TCBs()
      init_TCD0() - If present.
    init_millis() - This is called to kick off millis() timekeeping. This will configure TCD0 or any TCB if it is used for millis as well.
  initVariant() - A rare few libraries define this for stuff they need to run before setup.
  onAfterInit() - Default simply returns 0. Can be overridden. If 0 is returned, enable interrupts.
  setup() - Finally the normal setup() is called
  loop() - and the normal loop(), called continually.
```

If you for some reason end up directly putting functions into init 5, 7, or 8 (best steer clear of the ones that are taken) be warned:
The function must be declared with both `((naked))` and `((used))` attributes! The latter is required or the optimizer will just eliminate it. The former is needed to prevent it from generating a return instruction at the end. This causes a crash when it tries to return and there's nothing on the call stack to return to.

## Recommended override candidates
These are the functions which are there with the intent that users will have good reason to override them. Except as described for `init_reset_flags`, these should only be used when you can't just do whatever you need to do in setup() for some reason/

### init_reset_flags
```c
  void init_reset_flags() __attribute__((weak)) {
    if (RSTCTRL.RSTFR == 0){
      _PROTECTED_WRITE(RSTCTRL.SWRR, 1);
    }
  }
```
[The reset reference](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Reset.md) has recommended overrides and explanation of why this is so important.


### onPreMain
```c
void onPreMain() __attribute__((weak)) {
}
```
This is the recommended way to run code before the class constructors. Runs after init_reset_flags, so it won't run on a dirty reset if reset flags are being cleared as they should be.


### onAfterInit
```c++

uint8_t onAfterInit() __attribute__((weak)) {
  return 0;
}

```
onAfterInit runs just before setup is called, but after all other initialization. returning a non-zero value will prevent interrupts from being enabled. Good for debugging if you suspect that an interrupt is firing and wrecking everything as soon as they're enabled (for example. from a flag that gets set during initialization) and this is malfunctioning badly enough that you can't get any debugging information out (since most Arduino users are not set up for using a hardware debugger).


### onBeforeInit
```c++
void onBeforeInit() __attribute__((weak));{;}
```
onBeforeInit is called as the first code in main, useful if you need to do something AFTER class constructors have run but BEFORE initialization.


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
These are called when the external clock source doesn't provide any clock - either it never does (onClockTimeout() ) or it does initially but then dies and gets called by the CFD ISR; that interrupt rarely fires successfully if a crystal malfunctions - if it gets as far as enabling it, it will usually reset instead if something goes wrong with the crystal.
`_blinkcode()` never returns and blinks forever. Most likely to be overridden in the former case to run on internal and log an error, and in the latter case just to prevent the blink code because something else is connected to that pin.

See [The clock source reference](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Clocks.md) for examples of how to use it.

## Things you probably shouldn't override
The functions listed below can be overridden. In most cases they should not be. They are listed for completeness; many of them were added for use during development of the core to make core development easier. They may be useful for debugging and on rare occasions in

### main
```c
int main() {
  onBeforeInit(); //Emnpty callback called before init but after the .init stuff. First normal code executed - no peripherals have been set up yet, but class constructors have been run. Useful if you need to fiddle with a class instance at this point in time,
  init(); //Interrupts are turned on just prior to init() returning.
  initVariant();
  if (!onAfterInit()) sei();  //enable interrupts.
  setup();
  for (;;) {
    loop();
    #ifdef ENABLE_SERIAL_EVENT /* this is never true unless core is modified
      This option is strongly deprecated and Serial Event's days i nthe core are numbered. Users of serialEvent should begin migration with a sense of urgency*/
      if (serialEventRun) serialEventRun();
    #endif
  }
}
```
This is the main program, and it calls all the initialization functions described herein except for initThreeStuff(). The only things that run first are the things in the .initN sections - this means init_reset_flags(), onPreMain(), and class constructors. It can be overridden but in this case nothing will be initialized, and the clock will be 4 MHz internal when it is called. If you have a different speed selected, everything that depends on F_CPU (including the avrlibc delay.h) will have all timing wrong. Even if you override main, you probably want to call init_clock() at the start or be sure to compile for 4 MHz. If you just don't want to use the Arduino setup/loop structure, but you do want everything else (millis, pwm, adc, and anything that a library needs to do in initVariant), simply put your code in setup and leave loop empty - don't override main.

### Initializers of peripherals
Overriding these is not recommended. They're most likely to become relevant if you have overridden init() or main(), yet still want the peripheral in question initialized (which of course raises the question of why you overrode init() or main() in the first place). Occasionally useful for debugging, and that's why they exist.
```c
void init()             __attribute__((weak)); // This calls all of the others.
void init_clock()       __attribute__((weak)); // this is called first, to initialize the system clock.
void init_ADC0()        __attribute__((weak)); // this is called to initialize ADC0. be called manually on parts with an ADC1 to initialize that just as we do ADC0.
void init_timers();                            // this function calls the timer initialization functions. Overriding is not permitted.
void init_TCA0()        __attribute__((weak)); // called by init_timers() - Don't override this if using TCA0 for millis.
void init_TCA1()        __attribute__((weak)); // called by init_timers() - Don't override this if using TCA1 for millis.
void init_TCBs()        __attribute__((weak)); // called by init_timers() - Does not break millis if overridden, even if using the same timer, because it is either skipped or overwritten by the millis confige in init_millis();
void init_TCD0()        __attribute__((weak)); // called by init_timers() - Does nothing if TCD0 is used as millis timer but that is not currently supported on DxCore.
void init_millis()      __attribute__((weak)); // called by init() after everything else and just before enabling interrupts and calling setup() - sets up and enables millis timekeeping.
```
They are called in the order shown above.

Any of them can be overridden, but overriding them with anything other than an empty function is not recommended and is rarely a good idea except for debugging. If the peripheral isn't being used and you're desperate for flash (maybe on a small flash DD or EA series parts) these will save a tiny amount of flash. Don't override with anything other than an empry function. If you're initializing them differently, do it in setup().

The timer initialization functions do different things if the timer is used for millis.

#### init_clock
Initializes the system clock so that it will run at the F_CPU passed to it. Don't override this unless you can ensure that the F_CPU that is passed via the compiler command line will be correct or as s desperate measure in debugging. Overriding it with an empty function may be useful when debugging exotic problems where you want to make sure that a problem isn't being caused by the clock configuration code. (and accept that the timekeeping will be wrong while debugging)

*if overriding main() or init() you probably want to call this directly at initialization, otherwise no timing or baud rate calculation will work correctly - this is what sets the chip to run at the clock speed you're compiling for*

Changing this is the wrong way to debug a problem that you think might be caused by a malfunctioning external clock. In that case, just build for internal clock.

#### init_ADC0
Initializes the ADC clock prescaler to get a legal frequency, sets up defaults and enables the ADC. It can be overridden with an empty function to prevent it from initializing the ADC to save flash if the ADC is not used. if main is overridden and tou want the right clock speed, you MUST init_clock MUST be called first in that case.
#### init_timers
Calls initTCA9() and initTCA1() if TCA1 is present, and sets PORTMUX.TCAROUTEA() to match what variant specifies, then calls initTCBs(), and initTCD0(). No clear reason one would want to override

#### init_TCA0 and init_TCA1
Initialize the type A timers for PWM. The one for a timer used as millis must not be overridden. It is not recommended to override these at all except with an empty function in order to leave the peripheral in reset state (but takeOverTCAn() will also put it back in it's reset state. If you don't want to use analogWrite() through the timer, instead call takeOverTCAn() - which you need to do even if these are overridden if you don't want analogWrite() and digitalWrite() to manipulate the timer. This is solely a space saving method, and will most likely have little place except on things like the AVR8EA-series.
#### init_TCBs
Initializes the type B timers (the one used for millis is skipped if it's the highest numbered timer). It is not recommended to override this except with an empty function in order to leave the type B timers that are not used for millis in reset state.

#### init_TCD0
Initializes the type D timer. It is not recommended to override this except with an empty function in order to leave the peripheral in reset state. This is particularly useful with the type D timer, which has no "hard reset" command, and it's got the enable-locked fields and he ENRDY bit - If you're going to take it over anyway, this makes sense to override in order to make your life easier when reconfiguring it. As with the others, it is recommended to override with just an empty function in that case. It also saves some flash.

#### init_millis
Initializes and kicks off millis timekeeping. If millis is handled by a type B or D timer, it also performs all initialization of that timer. Overriding this (with an empty function) is for debugging ONLY - it is a way of leaving in place millis, micros (they will always return 0) and delay (it will hang forever) if you need to isolate the impact of the millis interrupt running.

If you just want to turn off millis, set the millis timer to "disabled". That both gets rid of the ISR and provides a working delay().

Note that the ISR will still be defined, but not enabled, if this is overridde, that is, more flash wikk be used for the same functionality.

### initVariant
```c++
void initVariant() __attribute__((weak)){;}
```
This is the ONLY one of these functions that is standard (other than init, setup, and loop, of course).

initVariant is meant for variant files to call, but none of them ever do that on any core i've seen, and library authors use this sometimes for code that needs to run at startup. **NO SKETCH SHOULD EVER OVERRIDE THIS** - This hook is reserved for library and variant authors. It is part of the Arduino API and is present on all cores as far as I know.
