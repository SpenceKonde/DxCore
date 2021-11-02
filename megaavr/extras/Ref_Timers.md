# PWM and Timer usage
This document describes how the timers are configured by the core prior to the sketch starting and/or by the built-in peripherals, and how this may impact users who wish to take full control of these peripherals. This document - besides the background section, applies only to DxCore - though much of it is very similar to megaTinyCore. The corresponding document for megaTinyCore is more accurate for that core.

## Background: The Timers on Dx-series parts
This applies to the DA, DB, and in overwhelming liklihood, the DD-series as well. These timers are, with very few changes, the same "modern" timers introduced on the tinyAVR 0/1-series, and featured on the megaAVR 0-series (including the ATmega4809 on the Nano Every) and tinyAVR 2-series. The megaAVR 0-series parts are all supported by @MCUdude's MegaCoreX [Hans/MCUdude](https://github.com/MCUdude)'s excellent [MegaCoreX](https://github.com/MCUdude/MegaCoreX).

### TCA0 - Type A 16-bit Timer with 3/6 PWM channels
This timer is the crown jewel of the modern AVR devices, as far as timers go. It can be operated in two very different modes. The default mode on startup is "Normal" or `SINGLE` mode - it acts as a single 16-bit timer with 3 output compare channels. It can count in either direction, and can also be used as an event counter (ie, effectively "clocked" off the event), is capable of counting up or down, generating PWM in single and dual slope modes, and has 7-output prescaler. For most use cases, a TCA in SINGLE mode is on the same level as the classic avr 16-bit Timers, only with more outputs (especially for 8-bit PWM) - the newly added features aren't ones that are particularly relevant for Arduino users. In this mode, TCA0 can generate events or interrupts on compare match for each channel (independently), as well as on an overflow.

The Type A timer can be also be set to split mode to get six 8-bit PWM channels (this is how it is configured for `analogWrite()` PWM in megaTinyCore. In split mode the high and low bytes of the timer count `TCA0.SINGLE.CNT` register becomes `TCA.SPLIT.LCNT` and `TCA.SPLIT.LCNT`; likewise the period and compare registers *in SINGLE mode, these are 16-bit registers; accessing them uses the temporary register. In SPLIT mode, they are 8-bit registers!*. Unlike the TCB in PWM mode, this works correctly. The count frequency of the two "halves" of the timer is always the same. However, the HPER and LPER registers can be used to adjust the period (that is, the period of high and low half can be independently adjusted). So, for the frequency of the PWM - albeit at a cost in resolution. he periods can be adjusted independently: Assuming 20MHz, prescaler 64 (default configuration), one could be generating 1.225 kHz PWM with period of 255 (LPER=254 - the default) on three channels, and PWM with frequency of 2 kHz with period of 156 (HPER=155) on the other three channels.

In megaTinyCore 2.2.6 and earlier and DxCore 1.3.0 and earlier, all `digitalWrite()` calls to pins that are output to in split mode will result in PWM being turned off on whichever pin normally corresponds to that channel (that is, if it has been remapped to a different pin, it will get turned off there), and no mechanism was provided to disable that. megaTinyCore 2.2.7 and DxCore 1.3.1 provide takeOverTCA0() and takeOverTCA1() (DxCore only). Calling those functions will tell `analogWrite()`/`digitalWrite()` etc functions to "forget about" that timer. PWM will not be altered, turned on, or turned off by functions supplied by the core. The


There are a few examples of using TCA0 to generate PWM at specific frequencies and duty cycles in the document on [Taking over TCA0](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/TakingOverTCA0.md)

### TCBn - Type B 16-bit Timer
The type B timer is what I would describe as a "utility timer". It is also the only timer which got a significant upgrade with the Dx-series... it received a new event user, `TCBn_COUNT`, and a new `TCBn.CTRLA` register layout with an option to clock on events, rather than just. This is a pretty big deal for the type B timers. But that's  Although, unlike the earlier 0/1-series parts, (though these both call the same ISR) (they now have `CAPT` and OVF), the behavior is somewhat muddled to retain compatibility with code written for the older timers, and the benefit . The input clock source can be either the system clock, optionally prescaled by 2, or whatever the prescaled clock of TCA0 (or TCA1 if present) is.

They can be set to act as 8 bit PWM source. When used for PWM, they can only generate 8-bit PWM, despite being a 16-bit timer, because the 16-bit `TCBn.CCMP` register is used for both the period and the compare value in the low and high bytes respectively. They always operate in single-slope mode, counting upwards, and the frequency depends on that of the TCAn (since CLK_PER/2 is far too fast for 8-bit PWM). In other words, **the type B timers are not very good at generating PWM**. Note also that `TCBn.CCMP` is effected by silicon errata: It still acts like a 16-bit register, using the temp register for access, so you must read the low byte first, then high byte, and always write the high byte after the low one, lest it not be written or a bad value written over the low byte!

While this makes them poor output generators, they are excellent utility timers, which is what they are clearly designed for. They can be used to time the duration of events down to single system clock cycles in the input capture modes, and with the event being timed coming from the event system, any pin can be used as the source for the input capture, as well as the analog comparators, the CCL modules, and more. As input capture timers, they are far more powerful than the 16-bit timers of the classic AVR parts. They can also be used as high resolution timers independent of the builtin `millis()`/`micros()` timekeeping system if this is needed for specific applications (in some cases during developmentm, millis/micros and a TCB were compared in order to detect errors in the timekeeping). The Dx-series adds support for two exciting new options - first, they can be clocked from events (those single cycle events became a lot more useful) - and secondly, you can use that to cascade two timers together, in order to do 32-bit input capture. 32 bits gives you a maximum count of 4.2 billion; with CLK_PER as the clock source, events with durations of several minutes can be timed to an accuracy of single clock cycles.

### TCD0 - Type D 12-bit Async Timer
The Type D timer, is a very strange timer indeed. It can run from a totally separate clock supplied on EXTCLK, or from the unprescaled internal oscillator - or, on the Dx-series, from the on-chip PLL at 2 or 3 times the speed of the external clock or internal oscillator! It was apparently designed with a particular eye towards motor control and SMPS control applications. This makes it very nice for those sorts of use cases, but in a variety of ways,these get in the way of using it for the sort of things that people who would be using the Arduino IDE typical arduino-timer purposes. First, none of the control registers can be changed while it is running; it must be briefly stopped, the register changed, and the timer restarted. In addition, the transition between stopping and starting the timer is not instant due to the synchronization process. This is fast (it looks to me to be about 2 x the synchronizer prescaler 1-8x Synchronizer-prescaler, in clock cycless. The same thing applies to reading the value of the counter - you have to request a capture by writing the SCAPTUREx bit of TCD0.CTRLE, and wait a sync-delay for it. can *also* be clocked from the unprescaled 20 MHz (or 16 MHz) internal oscillator, even if the main CPU is running more slowly. - though it also has it's own prescaler - actually, two of them - a "synchronizer" clock that can then be further prescaled for the timer itself. It supports normal PWM (what they call one-ramp mode) and dual slope mode without that much weirdness, beyond the fact that `CMPBSET` is TOP, rather than it being set by a dedicated register. But the other modes are quite clearly made for driving motors and switching power supplies. Similar to Timer1 on the ATtiny x5 and x61 series parts in the classic AVR product line,  this timer can also create programmable dead-time between cycles.

It also has a 'dither' option to allow PWM at a frequency in between frequencies possible by normal division of the clock - a 4-bit value is supplied to the TCD0.DITHER register, and this is added to a 4-bit accumulator at the end of each cycle; when this rolls over, another clock cycle is inserted in the next TCD0 cycle.

The asynchronous nature of this timer, however, comes at a great cost: It is much harder to use than the other timers. Most changes to settings require it to be disabled - and you need to wait for that operation to complete (check for the `ENABLERDY` bit in `TCD0.STATUS`). Similarly, to tell it to apply changes made to the `CMPxSET` and `CMPxCLR` registers, you must use the `TCD.CTRLE` (the "command" register) to instruct it to synchronize the registers. Similarly, to capture the current count, you need to issue a SCAPTUREx command (x is A or B - there are two capture channels) - and then wait for the corresponding bit to be set in the `TCD0.STATUS` register. In the case of turning PWM channels on and off, not only must the timer be stopped, but a timed write sequence is needed ie, `_PROTECTED_WRITE(TCD0.FAULTCTRL,value)` to write to the register that controls whether PWM is enabled; this is apparenmtly because, in the intended use-cases of motor and switching power supply control, changing this accidentally (due to a wild pointer or other software bug) could have catastrophic consequences. Writes to any register when it is not "legal" to write to it will be ignored. Thus, making use of the type D timer for even simple tasks requires careful study of the datasheet - which is itself quite terse in places where it really shouldn't be - and can be frustrating and counterintuitive.


### RTC - 16-bit Real Time Clock and Programmable Interrupt Timer
Information on the RTC and PIT will be added in a future update.

## Timer Prescaler Availability

Prescaler    | TCAn  | TCBn  | TCD0  | TCD0 sync | TD0 counterb|
------------ | ------|-------|-------|-----------|-------------|
CLK          |  YES  |  YES  |  YES  |  YES      |  YES        |
CLK2         |  YES  |  YES  |  YES* |  YES      |  NO         |
CLK/4        |  YES  |  TCA  |  YES  |  YES      |  YES        |
CLK/8        |  YES  |  TCA  |  YES  |  YES      |  NO         |
CLK/16       |  YES  |  TCA  |  YES* |  NO       |  NO         |
CLK/32       |  NO   |  NO   |  YES  |  NO       |  YES        |
CLK/64       |  YES  |  TCA  |  YES* |  NO       |  NO         |
CLK/128      |  NO   |  NO   |  YES* |  NO       |  NO         |
CLK/256      |  YES  |  TCA  |  YES* |  NO       |  NO         |
CLK/1024     |  YES  |  TCA  |  NO   |  NO       |  NO         |

* Requires using the synchronizer prescaler as well. My understanding is that this results in sync cycles taking longer.
`TCA` indicates that for this prescaler, a TCA must also use it, and then that can be prescaled, and the TCB set to use that TCA's clock.

## Resolution, Frequency and Period
When working with timers, I constantly found myself calculating periods, resolution, frequency and so on for timers at the common prescaler settings. While that is great for adhoc calculations, I felt it was worth some time to make a nice looking chart that showed those figures at a glance. The numbers shown are the resolution (when using it for timing), the frequency (at maximum range), and the period (at maximum range - ie, the most time you can measure without accounting for overflows).
### [In Google Sheets](https://docs.google.com/spreadsheets/d/10Id8DYLRtlp01KA7vvslC3cHaR4S2a1TrH7u6pHXMNY/edit?usp=sharing)


## PWM via analogWrite()
### TCAn
The core reconfigures they type A timers in split mode, so each can generate up to 6 PWM channels simultaneously. The `LPER` and `HPER` registers are set to 254, giving a period of 255 cycles (it starts from 0), thus allowing 255 levels of dimming (though 0, which would be a 0% duty cycle, is not used via analogWrite, since `analogWrite(pin,0)` calls `digitalWrite(pin,LOW)` to turn off PWM on that pin). This is used instead of a PER=255 because `analogWrite(255)` in the world of Arduino is 100% on, and sets that via `digitalWrite()`, so if it counted to 255, the arduino API would provide no way to set the 255/256th duty cycle). Additionally, modifications would be needed to make `millis()`/`micros()` timekeeping work without drift at that period anyway.
The core supports generating PWM using up to 6 channels per timer, and will work with alternate PORTMUX settings as long as the the selected option isn;t one of the three-channel ones for TCA1 - those are not supported. TCA1 can be on PB0-5 or PG0-5 (and not even the latter on DA due to errata). TCA0 can go on pin 0-5 in any port (though they must all be on the same port. We default to configuring it for PD on 28/32 pin parts and PC on 48/64 pin ones).

`analogWrite()` checks the `PORTMUX.TCAROUTEA` register. In a future version the same will be done for `PORTMUX.TCDROUTEA`, but no silicon is available where that works.

When there are multiple timers available for a pin, TCD and TCA take priority. TCBs are used only as a last resort.

#### bool digitalPinHasPWMNow(uint8_t p)
This function returns 1 if the pin currently has PWM available accounting for PORTMUX and `takeOverTCxn`. The dynamic analog of `digitalPinHasPWM()` - obviously, not compile-time constant.

#### uint8_t digitalPinToTimerNow(uint8_t p)
This function returns the timer the pin will be controlled by accounting for PORTMUX and `takeOverTCxn`. The dynamic analog of `digitalPinToTimer()` - obviously, not compile-time constant.

### TCD0
TCD0, by default, is configured for generating PWM (unlike TCA's, that's about all it can do usefully). TCD0 is clocked from the CLK_PER when the system is using the internal clock without prescaling. On the prescaled clocks (5 and 10 MHz) it is run it off the unprescaled oscillator (just like on the 0/1-series parts that it inherits the frequencies from), keeping the PWM frequency near the center of the target range. When an external clock is used, we run it from the internal oscillator at 8 MHz, which is right on target.

It is always used in single-ramp mode, with `CMPBCLR` (hence TOP) set to either 254, 509, or 1019 (for 255 tick, 510 tick, or 1020 tick cycles), the sync prescaler set to 1 for fastest synchronization, and the count prescaler to 32 except at 1 MHz. `CMPACLR` is set to 0xFFF (the timer maximum, 4095). The `CMPxSET` registers are controlled by `analogWrite()` which subtracts the supplied dutycycle from 255, checks the current CMPBCLR high byte to see how many places to left-shift that result by before subtracting 1 and writing to the register. The `SYNCEOC` command is sent to synchronize the compare value registers at the end of the current PWM cycle if the channel is already outputting PWM. If it isn't, we have to briefly disable the timer, turn on the pin, and then re-enable it, producing a glitch on the other channel. To mitigate this issue we treat 0 and 255 duty cycles differently for the TCD pins - they instead set duty cycle to 0% without disconnecting the pin from the timer, for the 100% duty cycle case, we invert the pin (setting CMPxSET to 0 won't produce a constant output). This eliminates the glitches when the channels are enabled or disabled.

TCD0 has two output channels - however, each of them can go to either of two pins. PA5 and PA7 use WOB, and PA4 and PA6 use WOA. :
```
analogWrite(PIN_PA4,64);  // outputting 25% on PA4
analogWrite(PIN_PA5,128); // 25% on PA4, 50% on PA5
analogWrite(PIN_PA5,0);   // 25% on PA4, PA5 constant LOW, but *still connected to timer*
digitalWrite(PIN_PA5,LOW);// NOW PA5 totally disconnected from timer. A glitch will show up briefly on PA4.
analogWrite(PIN_PA6,192); // This is on same channel as PA4. We connect channel to PA6 too (not in place of - we do the same thing on ATTinyCore for the 167 pwm output from Timer1 on the latest versions).
                          // so now, both PA4 and PA6 will be outputting a 75% duty cycle. Turn the first pin off with digitalWrite() to explicitly turn off that pin.
```
You can get a lot of control over the frequency without having to take over full management of the timer (which is rather complicated, and difficult to reconfigure) as long as you follow the rules carefully: See [TCD0 reference](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_TCD.md) Step off that narrow path, however, and `analogWrite()` will not work correctly.
### TCBn
The type B timers, while not particularly good for PWM, can be used for PWM as well; they are set to use the TCA1 clock by default. A type B timer used for millis cannot be used to output PWM.

### PWM Frequencies
The frequency of PWM output using the settings supplied by the core is shown in the table below. The "target" is 1 kHz, never less than 490 Hz or morethan 1.5 kHz. As can be seen below, there are several frequencies where this has proven an unachievable goal. The upper end of that range is the point at which - if PWMing the gate of a MOSFET - you have to start giving thought to the gate charge and switching losses, and may not be able to directly drive the gate of a modern power MOSFET and expect to get acceptable results (ie, MOSFET turns on and off completely in each cycle, there is minimal distortion of the duty cycle, and it spends most of it's "on" time with the low resistance quoted in the datasheet, instead of something much higher that would cause it to overheat and fail). Not to say that it **definitely** will work with a given MOSFET under those conditions (see [the PWM section of my MOSFET guide](https://github.com/SpenceKonde/ProductInfo/blob/master/MOSFETs/Guide.md#pwm) ), but the intent was to try to keep the frequency low enough that that use case was viable (nobody wants to be forced into using a gate driver), without compromising the ability of the timers to be useful for timekeeping.

Note that no attention had been paid to these for DxCore prior to the 1.3.0 release, and serious bugs were not discovered until 1.3.7

|   CLK_PER | Prescale A |   fPWM  | Prescale D  | TOP D |  fPWM (D) |
|-----------|------------|---------|-------------|-------|-----------|
| ** 48 MHz |        256 |  735 Hz |             |       |           |
| ** 44 MHz |        256 |  674 Hz |             |       |           |
| ** 40 MHz |        256 |  613 Hz |             |       |           |
| ** 36 MHz |        256 |  551 Hz |             |       |           |
|  External |            |         | OSCHF@8  32 |   254 |    980 Hz |
|  * 32 MHz |        256 |  490 Hz |          32 |  1019 |    980 Hz |
|  * 30 MHz |         64 | 1836 Hz | OSCHF@8  32 |   254 |    980 Hz |
|  * 28 MHz |         64 | 1716 Hz |          32 |  1019 |    858 Hz |
|    25 MHz |         64 | 1532 Hz |          32 |  1019 |    766 Hz |
|    24 MHz |         64 | 1471 Hz |          32 |  1019 |    735 Hz |
|    20 MHz |         64 | 1225 Hz |          32 |   509 |   1225 Hz |
|    16 MHz |         64 |  980 Hz |          32 |   509 |    980 Hz |
|    12 MHz |         64 |  735 Hz |          32 |   509 |    735 Hz |
|    10 MHz |         64 |  613 Hz | OSCHF@20 32 |   509 |   1225 Hz |
|     8 MHz |         64 |  490 Hz |          32 |   254 |    980 Hz |
|     5 MHz |         16 | 1225 Hz | OSCHF@20 32 |   509 |   1225 Hz |
|     4 MHz |         16 |  980 Hz |          32 |   254 |    490 Hz |
|     1 MHz |          8 |  490 Hz |           4 |   254 |    980 Hz |

`*` Overclocked (generally works, 28 and 32 can be achieved with internal oscillator)

`**` Way overclocked, may not work (requires external crystal or oscillator).

External clock or crystal will always cause TCD0 to use the internal oscillator by default. Speeds higher than 32 MHz can only use external clock sources, so they always act as described on the External line (unless reconfigured at runtime)

`Prescale A` and `fPWM` apply to all pins not on TCD0. TOP is always set to 254 for TCA

`Prescale D`, `TOP D`, and `fPWM (D)` apply to the pins on TCD0.
Where marked, we clock TCD0 from OSCHF instead of using CLK_PER, prescale by 32. For speeds other than 5 MHz and 10 MHz, we set the internal oscillator to 8 MHz.

These are the overall Timer D prescaler (in all cases, by default only the count prescaler is used), TOP, and resulting frequency of TCD0 PWM output.

Where TCD0 TOP is not 254, but is 509, 1019, or (above 32 MHz only) 2039, all duty cycles passed to `analogWrite()` for those pins will by left-shifted as necessary to get an appropriate duty cycle.

#### Previous versions
Versions 1.3.1-1.3.6 had issues with PWM frequency at under some condition, and with micros readout in others. In 1.3.0 and earlier, there was no coherent list of timer settings based on part and clock speed.

## Millis/Micros Timekeeping
DxCore allows any of the type A or B timers to be selected as the clock source for timekeeping via the standard millis timekeeping functions. The RTC timers will be added after the sleep/low power library for this and tinyAVR 0/1-series is completed. There are no plans to support the type D timer - this is not like tinyAVR where we are desperately short of timers, with the comparatively difficult to use type D timer an irresistible victim to palm off the task of millis timekeeping on. Now, the calculations are more complicated since there are a great many possible speeds it could be running at, as opposed to just 16 or 20 on the tinyAVR 0/1-series. The timer used and system clock speed will effect the resolution of `millis()` and `micros()`, the time spent in the millis ISR, and the time it takes for `micros()`  to return a value. The `micros()` function will typically take several times it's resolution to return, and the times returned corresponds to the time `micros()` was called, regardless of how long it takes to return.

A table is presented for each type of timer comparing the percentage of CPU time spent in the ISR, the resolution of the timekeeping functions, and the execution time of micros. Typically `micros()`  can have one of three execution times, the shortest one being overwhelmingly more common, and the differences between them are small.


### TCAn for millis timekeeping
When TCA0 is used as the millis timekeeping source, it is set to run at the system clock prescaled by 8 when system clock is 1MHz, 16 when system clock is 4 MHz or 5 MHz, and 64 for faster clock speeds, with a period of 255 (as with PWM). This provides a `millis()`  resolution of 1-2ms, and is effecively not higher than 1ms between 16 and 30 MHz, while `micros()` resolution remains at 4 us or less. At 32 MHz or higher, to continue generating PWM output within the target range, we are forced to switch to a larger prescaler (by a factor of 4), so the resolution figures fall by a similar amoubnt, and the ISR is called that much less often.

#### TCA timekeeping resolution
|   CLK_PER | millis() | micros() | % in ISR | micros() time |
|-----------|----------|----------|----------|---------------|
|    48 MHz |  1.36 ms |   5.3 us |   0.19 % |        2.5 us |
|    44 MHz |  1.48 ms |   5.8 us |   0.19 % |   aprx 3.5 us |
|    40 MHz |  1.63 ms |   6.4 us |   0.19 % |        3.5 us |
|    36 MHz |  1.81 ms |   7.1 us |   0.19 % |   aprx   4 us |
|    32 MHz |  2.04 ms |   8.0 us |   0.19 % |          4 us |
|    30 MHz |  0.54 ms |   2.1 us |   0.72 % |   aprx   4 us |
|    28 MHz |  0.58 ms |   2.3 us |   0.72 % |          4 us |
|    25 MHz |  0.65 ms |   2.6 us |   0.72 % |   aprx   4 us |
|    24 MHz |  0.68 ms |   2.7 us |   0.72 % |          5 us |
|    20 MHz |  0.82 ms |   3.2 us |   0.72 % |          7 us |
|    16 MHz |  1.02 ms |   4.0 us |   0.72 % |          9 us |
|    12 MHz |  1.36 ms |   5.3 us |   0.72 % |         10 us |
|    10 MHz |  1.63 ms |   6.4 us |   0.72 % |         14 us |
|     8 MHz |  2.04 ms |   8.0 us |   0.72 % |         17 us |
|     5 MHz |  0.82 ms |   3.2 us |   2.99 % |         27 us |
|     4 MHz |  1.02 ms |   4.0 us |   2.99 % |         33 us |
|     1 MHz |  2.04 ms |   8.0 us |   5.98 % |        112 us |

In contrast to the type B timer where prescaler is held constant while the period changes, here period (in ticks) is constant but the prescaler is not. Hence each prescaler option is associated with a fixed % of time spent in the ISR (and yes, for reasons I don't understand, the generated ISR code is slightly faster for /64 prescaling compared to /256, /16, and /8 (which are equal to each other).

The micros execution time does not depend strongly on F_CPU, running from 112-145 clock cycles.

Except when the resolution is way down near the minimum, the device spends more time in the ISR on these parts. Notice that at these points that - barely - favor TCAn, the interrupt they're being compared to is firing twice as frequently!


### TCBn for millis timekeeping
When TCB2 (or other type B timer) is used for `millis()` timekeeping, it is set to run at the system clock prescaled by 2 (1 at 1 MHz system clock) and tick over every millisecond. This makes the millis ISR very fast, and provides 1ms resolution at all speeds for millis. The `micros()` function also has 1 us or almost-1 us resolution at all clock speeds (though there are small deterministic distortions due to the performance shortcuts used for the microsecond calculations. The type B timer is an ideal timer for millis - as these parts (except for future DD-series parts) have plenty of them, TCB2 is used by default on all parts except DD-series devices with only 2 type B timers, which use TCB1 instead. Except on those smaller DD-series parts, there is rarely competition for type B timers for other purposes, like `tone()`, servo, input capture or outputting pulses of a controlled length, which is a relatively common procedure; it is anticipated that as libraries for IR, 433MHz OOK'ed remote control, and similar add support for the modern AVR parts, that these timers will see even more use.


|   CLK_PER | millis() | micros() | % in ISR | micros() time |
|-----------|----------|----------|----------|---------------|
|    48 MHz |     1 ms |  1.33 us |   0.14 % | *      2.5 us |
|    44 MHz |     1 ms |     1 us |   0.15 % |               |
|    40 MHz |     1 ms |     1 us |   0.17 % | *        3 us |
|    36 MHz |     1 ms |     1 us |   0.18 % |          3 us |
|    32 MHz |     1 ms |     1 us |   0.20 % |          3 us |
|    30 MHz |     1 ms |  1.07 us |   0.22 % |               |
|    28 MHz |     1 ms |  1.14 us |   0.23 % |               |
|    25 MHz |     1 ms |  1.28 us |   0.26 % |          6 us |
|    24 MHz |     1 ms |  1.33 us |   0.27 % | *        5 us |
|    20 MHz |     1 ms |     1 us |   0.33 % | *        6 us |
|    16 MHz |     1 ms |     1 us |   0.40 % |          6 us |
|    12 MHz |     1 ms |  1.33 us |   0.54 % | *       10 us |
|    10 MHz |     1 ms |     1 us |   0.65 % | *       10 us |
|     8 MHz |     1 ms |     1 us |   0.80 % |         11 us |
|     5 MHz |     1 ms |     1 us |   1.30 % | *       23 us |
|     4 MHz |     1 ms |     1 us |   1.60 % |         21 us |
|     1 MHz |     1 ms |     1 us |   6.50 % |         78 us |

Resolution is always exactly 1ms for millis, and whereas TCAn `micros()` is limited by the resolution of the timer, here it's instead limited only by the fact that the calculations drop the least significant bits first; this results in a value that may be as low as 750, yet is being mapped to 0-999, for 1.33 us resolution in the worst cases. The timer count and the running tally of overflows could get us microseconds limited only by F_CPU/2
The percentage of time spent in the ISR varies in inverse proportion to the clock speed - the ISR simply increments a counter and clears its flags. 65 clocks from interrupt bit set to interrupted code resuming.

The time that `micros()` takes to return a value varies significatly with F_CPU. Specifically, powers of 2 are highly favorable, and almost all the calculations drop out of the 1 MHz case. micros takes between 78 and 160 clocks to run. Each factor of 2 increase in clock speed results in 5 extra clocks being added to micros in most cases (bitshifts, while faster than division, are still slow when you need multiples of them on larger types, especially in compiler-generated code)
`*` indicates that the shift & sum ersatz-division is done in hand-optimized assembly because I just couldn't stand how stupid the and stubborn the compiler was, and by the time I was done analyzing it, implementing it was trivial. This improves accuracy, particularly in the case significantly as well. In these optimized cases, as well as the powers of 2, the results are as good as possible in light of the specified resolution - where resolution is coarser than 1us, (1/resolution) of possible values for the three lowest digits (in normal base 10 number systems) is skipped and never shows up. These skipped values are distributed evenly between 1 and 999. For example, for 12/24/48, that means 250 values never show up. Of the remaining 750, 500 will show for only 1 "actual" microsecond value, and 250 for 2 consecutive "actual" microsecond values. See the table at the end of this document for the list of skipped least-significant-digit combinations. None of the optimized options will ever go back in time, even by a single microsecond (as long as the chance of backward time travel is limited to less than the time it takes micros to execute, it can't break `delay()`, cause timeouts to instantly expire or cause other catastrophic consequences) nor are more than 2 values in a row ever skipped where resolution > 1us.

### TCD0 for millis timekeeping
TCD0 is not supported for millis timekeeping on these parts. Originally it was imagined that the implementation from megaTinyCore could simply be used - but there the main clock was prescaled from 16 or 20 MHz, and TCD0 ran from unprescaled osc, giving 2 channels of normal speed PWM and a predictable timebase for millis even clocked at 1 MHz. Here, the value proposition isn't as strong: there are more timers available, and the type D timer is readily put to use generating high frequency PWM which is more accessible thanks to the PLL  and higher maximum system clock speeds, which not only determine maximum frequency, but also how complicated calculations that have be performed at a specified frequency can be.  calculations that were on the edge of being too slow on the 0/1-series to be. (ex, motor control applications which must be outside of the range of human hearing for quieter operation. Note that such applications most certainly require a MOSFET gate driver. 50 kHz is an order of magnitude above the highest plausible frequency of PWM that could be used to drive a gate directly).

## Tone
The `tone()` function included with DxCore uses one Type B timer. It defaults to using TCB0; do not use that for millis timekeeping if using `tone()`. Tone is not compatible with any sketch that needs to take over TCB0. If possible, use a different timer for your other needs. When used with Tone, it will use CLK_PER or CLK_PER/2 as it's clock source - the TCA clock will never be used, so it does not care if you change the TCA0 prescaler (unlike the official megaAVR core).

Tone works the same was as the normal `tone()` function on official Arduino boards. Unlike the official megaAVR board package's tone function, it can be used to generate arbitrarily low frequency tones (as low as 1 Hz). If the period between required toggling's of the pin is greater than the maximum timer period possible, it will calculate how many cycles it has to wait through between switching the pins in order to achieve the desired frequency.

It can only generate a tone on one pin at a time.

All tone generation is done via interrupts. The hardware output compare functionality is not used for generating tones because in PWM mode, the type B timers kindof suck.

## Servo Library
The Servo library included with this core uses one Type B timer. It defaults to using TCB1 if available, unless that timer is selected for Millis timekeeping. Otherwise, it will use TCB0. The Servo library is not compatible with any sketch that needs to take over these timers - if possible, use a different timer for your other needs. Servo and `tone()` can only be used together on when neither of those is used for millis timekeeping.

Regardless of which type B timer it uses, Servo configures that timer in Periodic Interrupt mode (`CNTMODE`=0) mode with CLK_PER/2 or CLK_PER as the clock source, so there is no dependence on the TCA prescaler. The timer's interrupt vector is used, and it's period is constantly adjusted as needed to generate the requested pulse lengths. In 1.1.9 and later, CLK_PER is used if the system clock is below 10MHz to generate smoother output and improve performance at low clock speeds.

The above also applies to the Servo_DxCore library; it is an exact copy except for the name. If you have installed a version of Servo via Library Manager or by manually placing it in your sketchbook/libraries folder, the IDE will use that in preference to the one supplied with this core. Unfortunately, that version is not compatible with the Dx-series parts. Include Servo_megaTinyCore.h instead in this case. No changes to your code are needed other than the name of the library you include.

## Additional functions for advanced timer control
  We provide a few functions that allow the user to tell the core functions that they wish to take full control over a timer, and that the core should not try to use it for PWM. These are not part of a library because of the required integration with the core to control `analogWrite()` and digitalWrite behavior.
### takeOverTCA0()
  After this is called, `analogWrite()` will no longer control PWM on any pins attached to timer TCA0 (though it will attempt to use other timers that the pin may be controllable with to, if any), nor will `digitalWrite()` turn it off. TCA0 will be disabled and returned to it's power on reset state. All TCBs that are used for PWM on parts with only TCA0 use that as their prescaled clock source buy default. These will not function until TCA1 is re-enabled or they are set to use a different clock source. Available only on parts with TCA1 where a different timer is used for millis timekeeping.
### takeOverTCA1()
  After this is called, `analogWrite()` will no longer control PWM on any pins attached to timer TCA1 (though it will attempt to use other timers that the pin may be controllable with to, if any), nor will `digitalWrite()` turn it off. TCA1 will be disabled and returned to it's power on reset state. All TCBs that are used for PWM on parts with TCA1 use that as their prescaled clock source buy default. These will not function until TCA1 is re-enabled or they are set to use a different clock source. Available only on parts with TCA1 where a different timer is used for millis timekeeping.
### takeOverTCD0()
  After this is called, `analogWrite()` will no longer control PWM on any pins attached to timer TCD0 (though it will attempt to use other timers, if any), nor will `digitalWrite()` turn it off. There is no way to reset type D timers like Type A ones. Instead, if you are doing this at the start of your sketch, override init_TCD0. If TCD is ever supported as millis timing source, this will not be available.
### resumeTCA0()
  This can be called after takeOverTimerTCA0(). It resets TCA0 and sets it up the way the core normally does and re-enables TCA0 PWM via analogWrite.
### resumeTCA1()
  This can be called after takeOverTimerTCA1(). It resets TCA1 and sets it up the way the core normally does and re-enables TCA1 PWM via analogWrite.

## Appendix I: Names of timers
Whenever a function supplied by the core returns a representation of a timer, these constants will be used

| Timer Name   | Value | Peripheral |          Used for |
|--------------|-------|------------|-------------------|
| NOT_ON_TIMER |  0x00 |            |                   |
| TIMERA0 *    |  0x10 |      TCA0  | millis and/or PWM |
| TIMERA1 *    |  0x08 |      TCA1  | millis and/or PWM |
| TIMERB0      |  0x20 |      TCB0  | millis     or PWM |
| TIMERB1      |  0x21 |      TCB1  | millis     or PWM |
| TIMERB2      |  0x22 |      TCB2  | millis     or PWM |
| TIMERB3      |  0x23 |      TCB3  | millis     or PWM |
| TIMERB4      |  0x24 |      TCB4  | millis     or PWM |
| TIMERD0 **   |  0x40 |      TCD0  |               PWM |
|              |  0x40 |      TCD0  |           PWM WOA |
|              |  0x41 |      TCD0  |           PWM WOB |
|              |  0x42 |      TCD0  |           PWM WOC |
|              |  0x43 |      TCD0  |           PWM WOD |
| TIMERRTC     |  0x90 |       RTC  |                   |
| DACOUT ***   |  0x80 |      DAC0  |       analogWrite |
`*` Currently, the 3 low bits are don't-care bits. At some point in the future we may pass arouind the compare channel in the 3 lsb when using for PWM. No other timer will ever be numbered 0x10-0x17, nor 0x08-0x0F. 0x18-0x1F is reserved for hypothetical future parts with a third TCA. Hence to test for TCA type: `(timerType = MILLIS_TIMER & 0xF8; if (timerType==TIMER0) { ... })`
`**` What was described above would look much like this. Currently we do not use the WOA/WOB/WOC/WOD specific values, but things will change when we get a working PORTMUX for it.
`***` A hypothetical part with multiple DACs with output buffers will have extend up into 0x8x. Similarly, oddball timing related things will go higher values.


## Appendix II: TCB Micros Artifacts
3, 6, 12, 24, and 48 MHz, with the new optimized micros code, running from a TCB, is known to skip these (and only these) 250 values when determining the least significant thousand micros. That is, if you repeatedly calculate `micros % 1000`, none of these will show up until it has been running long enough for micros to overflow.

```text
2, 7, 10, 13, 18, 23, 28, 31,  34,  39,  42,  45,  50,  53,  56,  61,  66,  71,  74,  77,  82,  87,  92,  95,  98, 103, 108, 113, 116, 119, 124, 127, 130,
135, 138, 141, 146, 151, 156, 159, 162, 167, 170, 173, 178, 181, 184, 189, 194, 199, 202, 205, 210, 213, 216, 221, 224, 227, 232, 237, 242, 245, 248, 253,
258, 263, 266, 269, 274, 279, 284, 287, 290, 295, 298, 301, 306, 309, 312, 317, 322, 327, 330, 333, 338, 341, 344, 349, 352, 355, 360, 365, 370, 373, 376,
381, 384, 387, 392, 395, 398, 403, 408, 413, 416, 419, 424, 429, 434, 437, 440, 445, 450, 455, 458, 461, 466, 469, 472, 477, 480, 483, 488, 493, 498, 501,
504, 509, 512, 515, 520, 523, 526, 531, 536, 541, 544, 547, 552, 555, 558, 563, 566, 569, 574, 579, 584, 587, 590, 595, 600, 605, 608, 611, 616, 621, 626,
629, 632, 637, 640, 643, 648, 651, 654, 659, 664, 669, 672, 675, 680, 685, 690, 693, 696, 701, 706, 711, 714, 717, 722, 725, 728, 733, 736, 739, 744, 749,
754, 757, 760, 765, 770, 775, 778, 781, 786, 791, 796, 799, 802, 807, 810, 813, 818, 821, 824, 829, 834, 839, 842, 845, 850, 853, 856, 861, 864, 867, 872,
877, 882, 885, 888, 893, 896, 899, 904, 907, 910, 915, 920, 925, 928, 931, 936, 941, 946, 949, 952, 957, 962, 967, 970, 973, 978, 981, 984, 989, 992, 995
```

The repeats are:
```text
0, 4, 8, 12, 16, 20, 24,  27,  32,  36,  40,  44,  48,  52,  57,  60,  64,  68,  72,  76,  80,  84,  88,  91,  96, 100, 104, 107, 111, 115, 120, 123, 128,
132, 136, 140, 144, 148, 152, 155, 160, 164, 168, 172, 176, 180, 185, 188, 192, 196, 200, 204, 208, 212, 217, 220, 225, 229, 233, 236, 240, 244, 249, 252,
256, 260, 264, 268, 272, 276, 280, 283, 288, 292, 296, 300, 304, 308, 313, 316, 320, 324, 328, 332, 336, 340, 345, 348, 353, 357, 361, 364, 368, 372, 377,
380, 385, 389, 393, 397, 401, 405, 409, 412, 417, 421, 425, 428, 432, 436, 441, 444, 448, 452, 456, 460, 464, 468, 473, 476, 481, 485, 489, 492, 496, 500,
505, 508, 513, 517, 521, 525, 529, 533, 537, 540, 545, 549, 553, 557, 561, 565, 570, 573, 577, 581, 585, 589, 593, 597, 601, 604, 609, 613, 617, 620, 624,
628, 633, 636, 641, 645, 649, 653, 657, 661, 665, 668, 673, 677, 681, 684, 688, 692, 697, 700, 704, 708, 712, 716, 720, 724, 729, 732, 737, 741, 745, 748,
752, 756, 761, 764, 768, 772, 776, 780, 784, 788, 792, 795, 800, 804, 808, 812, 816, 820, 825, 828, 832, 836, 840, 844, 848, 852, 857, 860, 865, 869, 873,
876, 880, 884, 889, 892, 897, 901, 905, 909, 913, 917, 921, 924, 929, 933, 937, 940, 944, 948, 953, 956, 960, 964, 968, 972, 976, 980, 985, 988, 993, 997,
```
Similar lists can be generated for any other clock speed where resolution is coarser than 1us and a TCB is used.

The number of values will, for ideal values, be `(1000) * (1 - (1 / resolution))` or something very close to that. Non-ideal series of terms or particularly adverse clock speeds may have more than that, as well as more cases of consecutive times that get the same micros value. If the terms were chosen poorly, it is possible for a time t give micros M where M(t-1) > M(t), violating the assumptions of the rest of the core and breaking EVERYTHING. 1.3.7 is believed to be free of them for all supported clock speeds. Likewise, it is possible for there to be larger skips or discontinuities, where M(t) - M(t-1) > 2, in contrast to skips listed above (where M(t) - M(t-1) = 2 ) or consecutive times return a repeated value (ie, M(t) = M(t-1))

Timer options which have resolution of 1us (internally, it is lower) may have repeats or skips if fewer than the optimal number of terms were used (as is the case with non optimized options) or where the clock speed is particularky hard to work with, like the 28 MHz one.
