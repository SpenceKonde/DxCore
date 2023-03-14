# PWM and Timers on modern AVRs and their usage in this core
This document is divided into two sections. The first one simply describes the available timers, and what they are capable of (by "simply describes" I don't claim to have made a simple description, only that the purpose is simple. and that is to describe the timers). The second section describes how they are used by this core in particular. The first section is shared by DxCore and megaTinyCore. The second contains many sections specific to one core or another.

## Quick answer: Which PWM pins should I use?
TCA or TCD pins; these timers are much better for generation of PWM. Only use TCB pins if desperate. See the part-specific docs for your part and pincount to see where the timers are pointed by the core on startup. You can set which pins the TCAs (and the TCD on the DD-series) use *at runtime* by simply writing to `PORTMUX.TCAROUTEA`. See the part-specific docs (the ones with the pinout charts, linked to from top of main README and from the column headings in [About the Dx-Series](AboutDxSeries.md)). These contain a table of all pins that exist on these parts that the timers can give PWM on, and list they are set to at startup, whether existing parts are constrained by errata, and what setting the core initializes the pins to.


| TIMER       | On DA/DB | On DD | On EA | On EB | Pins:                                                  |Relevant Errata: |
|-------------|----------|-------|-------|-------|--------------------------------------------------------|-----------------|
| TCA0        | Yes      | Yes   | No    | No    | Pins 0-5 on your choice of ports.                      | Restart Command not intended to reset direction and on DD and future revisions will not |
| TCA1        | >32 pin  | No    | Yes   | No    | Pins 0-5 on PB or PG, else pins 4-6 of PC, PE, PA* PD* | AVR128DA64 cannot output TCA1 compare match (pwm) on PORTG or PORTE |
| TCD0        | Yes      | Yes   | No    | No    | PA4-7 on DA/DB. DD has special split-port mux option   | Only default portmux works on pre-DD parts (Where is our damned die rev already?) |
| TCB0        | Yes      | Yes   | Yes   | Yes   | PA2 or PF4. All TCBs are poor PWM timers               | AND you have to set both duty cycle and period at the same time |
| TCB1        | Yes      | Yes   | Yes   | Yes   | PA3 or PF5.                                            | And you get just 1 channel of 8-bit PWM. |
| TCB2        | Yes      | 28/32 | Yes   | No    | PC0 or PB4. Default millis timer `**`                  | They make an AMAZING millis timer though! |
| TCB3        | 48/64 pin| No    | Yes   | No    | PB5 or PC1.                                            | because you can set them to interrupt once/ms |
| TCB4        | 64 pin   | No    | No    | No    | PG3 or PC6.                                            | which makes both micros and especially millis FAST |

`*` MUX options for PA/PD on TCA1 are EA-series only. On DA-series, only PB and PC mux options work.
`**` A TCB cannot be used for PWM if being used for millis. On DD-series with 14 or 20 pins, TCB1 is the default millis timer as it likely will be on the EB-series (unless it's clear that we want to use TCF0). Otherwise, TCB2 is: TCB2 is always the default millis timer if it exists (again, unless it turns out that TCF is better for that). A TCA can be used for millis, though it is less accurate, and it can output PWM and act as the millis timer. TCD as millis is not supported because TCD is *really* not meant for that kind of thing. and is an outstanding PWM timer (see [TCD reference for specific "can I change this without breaking stuff" questions re: TCD0](Ref_TCD.md). We are more forgiving of users tweaking the settings of TCD and wanting to use analogWrite with it because TCD0 is very confusing to configure, and we believe that the number of people who want to be able to tweak it's PWM is markedly larger than the number of people who can figure out how to manually configure it after taking over TCD0.

## Background: The Timers on the AVR Dx-series and Ex-series parts
This applies to the tinyAVR 0/1/2-series, megaAVR 0-series, and AVR DA, DB and DD-series, and all other future modern AVRs until such a time as a part which departs radically from the modern AVR precedents is released. There are few differences between the implementations on the different families.

**Before reconfiguring timers, be sure to read the "Section 2: How this core uses timers" below to avoid stepping on the core's toes, or getting your toes stepped on by the core!**


### TCA0 - Type A 16-bit Timer with 3/6 PWM channels
This timer is the crown jewel of the modern AVR devices, as far as timers go (Well, until the EB comes out with it's TCE+WEX). It can be operated in two very different modes. The default mode on startup is "Normal" or `SINGLE` mode - it acts as a single 16-bit timer with 3 output compare channels. It can count in either direction, and can also be used as an event counter (ie, effectively "clocked" off the event), is capable of counting up or down, generating PWM in single and dual slope modes, and has 7-output prescaler. In terms of PWM generation, a TCA in SINGLE mode is on the same level as the classic avr 16-bit Timer1 (though most classic AVRs only had 2 outputs per timer - there the third output was a rare feature reserved for top-end parts like the 2560). The newly added features aren't ones that are particularly relevant for most Arduino users. TCA0 can generate events or interrupts on compare match for each channel (independently), as well as on an overflow.

The Type A timer can be also be set to split mode to get six 8-bit PWM channels (this is how it is configured by default by megaTinyCore and DxCore - since `analogWrite()` is only 8-bit, we might as well double the number of channels we get right? In split mode the high and low bytes of the timer count `TCA0.SINGLE.CNT` register become `TCA.SPLIT.LCNT` and `TCA.SPLIT.LCNT`; likewise the period and compare registers *in SINGLE mode, these are 16-bit registers; accessing them uses the temporary register. In SPLIT mode, they are 8-bit registers!* (Unlike the TCB in PWM mode, this works correctly on all parts). The frequency of the clock from which the signals generated by the two halves of the timer are derived is always the same - they share the prescaler - but the period, and hence the frequecy of the output, is not. For example, if we have a 1 MHz clock (16 MHz prescaled by 16), and set LPER to 249 (250 clocks) and HPER to 199 (200 clocks) the low-half period, covering WO 0-2 will generate PWM at 4kHz, and the high half will generate PWM at 5 kHz on WO3-5.

Event functionality is not supported in SPLIT mode.

There are a few examples of using TCA0 to generate PWM at specific frequencies and duty cycles in the document on [Taking over TCA0](./TakingOverTCA0.md)

The pins that the type A timer is directed to can be mapped to a variety of different pins.
* On ATtiny parts, each Waveform Output (WO) pin can be controlled separately.
  * On ATtiny parts with 14+ pins, WO0-2 can be mapped to either PB0-2 or PB3-5, and WO3-5 (the split mode only ones) can be mapped to PA3-5 or PC3-5 (if the part has those pins).
  * On ATtiny parts with 8 pins, they are mapped to (in order) PA3-or-PA7, PA1, PA2, and PA3. Only the first 4 are available, and WO0 must be remapped to PA7 in order to use WO3.
* On everything else, each TCA can be pointed at one group of pins from a list of options. TCA0 can (on all parts thus far released) be directed to pins 0-5 of any port, while TCA1's WO0-5 channels are only available on PB0-5 or (on 64-pin parts only - and not DA-series as of mid-2022 due to errata) PG0-5, and TCA1's WO0-2 can be output on pins PC4-6, or (on DB-series parts - DA's are again impacted by errata here) PE4-6. The Ex-series adds options for PD4-6 and PA4-6.

#### Events and CCL channels
On all parts, the TCA can be used to count events (counting either positive edges, or all edges), instead of system clocks (in this mode, the prescaler is ignored, and the events must be longer than 2 system clocks or generated synchronously to the system clock to guarantee the event input is seen) or to count only while the event channel is HIGH, or to reverse direction while the event input is high. On 2-series tinyAVR parts, AVR Dx and AVR Ex, there is a second event input dedicated to restarting the timer compare cycle on rising edge, any edge or have the timer only count or to reverse direction when an event is HIGH. **None of these event inputs are not available in SPLIT mode**. There is no restart-on-event input on the tinyAVR 0/1-series or megaAVR 0-series.

The TCA also generates 4 event outputs - compare match for the first three channels, and an overflow event (in split mode, the overflow event is replaced by two underflow events, for a total of five event generators). These are all pulse events 1 system clock long, synchronous to the system clock (among other things, this means they can't be used to remap the PWM outputs). However the first three WO output levels are ALSO available to the CCL LUTs, so they *can* be used for remapping WO0-2 to any LUT output pin.

Remember that the pin invert (INVEN) effects event inputs and outputs. So if you want to count negative edges, or count prescaled clock cycles while the pin is LOW, etc - just invert the input pin.

#### Interrupt note
When firing interrupts from a TCA, you must *ALWAYS* manually clear the intflags. It is not done for you by the hardware.

### TCBn - Type B 16-bit Timer
The type B timer is what I would describe as a "utility timer" - It is a very good utility timer. In this role, it can take one of no less than 7 modes, most of which require using the event system. They can also be used as a single channel 8-bit PWM timer. Unfortunately, they have limited selection of clock prescalers: only the system clock, the system clock divided by 2, or a clock that is already being generated for TCA0 or TCA1, making these very unappealing PWM timers. 2-series, Dx, and Ex-parts can also count rising event edges using the second event input, which is not available on the tinyAVR 0/1-series or megaAVR 0-series. In terms of input capture, the TCBs are able to do everything a classic AVR timer1 could do, plus multiple things that the classic AVR's timers couldn't even dream of doing. On the other hand, the classic Timer1 was an excellent PWM timer - while these are terrible ones.

#### Periodic interrupt
This does exactly what you would expect - counts up to CCMP, and resets to 0 and generates an interrupt. This is how it's used for millis timekeeping. Even with the limited prescaling selections (1, 2, or the TCA prescaler), being able to count up to 64k makes this a lot less limiting than it would be on an 8-bit timer.

#### Input Capture on Event
Timer counts continually from 0 to 65535. This is very much like the old ICP mode on classic AVRs. Upon the specified edge (selectable) of the the event connected to the timer, it will copy the current count to CCMP and fire an interrupt. It is the responsibility of the user code to know track when the last input capture occurred and subtract those values, and your interrupt must swap EDGE if you want to detect both.

#### Input Capture Frequency Measurement
As above, but at the specified edge, the timer count is automatically reset (after copying count to CCMP of course), so it starts timing the next event. As the name implies, this is of great use for measuring the frequency of an input. Does not work with prescaled clocks on some tinyAVRs (see silicon errata)

#### Input Capture Pulse Width Measurement
One edge restarts the timer. The other edge copies the value and fires the interrupt, thus making it perfect for timing the length of pulses.

#### Input Capture Frequecy And Pulse Width Measurement
This one is the weird one. Does not work with prescaled clocks on some tinyAVRs (see silicon errata listings). On one edge, it starts the timer. On the other edge, it copies the count to CCMP *and continues to count* until the next edge. Only then will the interrupt fire (if enabled). Whether via polling or interrupt, when you see that this has happened, you need copy the count value someplace safely, then the CCMP value. As soon as CCMP has been read, when it next sees the "start count" edge, it will reset the timer and start counting. Thus, while great for measuring PWM of varying frequency and duty cycle where the duty cycle does not change drastically from cycle to cycle, it is not useful if you need to record every cycle (for that, you're forced to use Input Capture on Event mode.

#### Single-shot
In addition to the input capture modes, they can be used for precise generation of a pulse with timing accuracty down to a single clock cycle. In this mode, the output pin will go high or low (selectable) in response to the specified edge of an event (though see below). The counter will count up to it's CCMP value, then reset both it's count and it's pin (these are the same output pins as are used for PWM). It is NOT NECESSARY to use the pin itself - you can use this instead to generate a event or interrupt after a delay has passed! Likewise, it could be triggered with an event channel connected to nothing, and manually kicked off with a software event. Without relying on TCA's prescaler, this can generate pulses of around 130ms in length.

**Datasheet Clarification Warning:** On the latest AVR DA/DB datasheet, and on all DD datasheet versions, it is stated that when EDGE is 1, it triggers on *any* edge, not just negative ones, - it's not clear if that applies to other parts or not and if so which ones; it may very well apply to all parts and simply not have been updated for the other parts, or it may only reaches back as far as the DA-series. This may sound like a downgrade; if you think that, though, you have forgotten about some of the features common to all modern AVRs, namely the `INVEN` bit in PINnCTRL: it is applied *before* the signal passes through the event generator. So you want to trigger on negative edges? Just invert the pin and trigger on positive edges! (Though, there are some cases where you can't do this. )

#### Timeout Check
In this mode, one type of edge resets the counter to 0 and starts it counting, while the other edge stops the counting. In this mode the interrupt fires when the count passes CCMP - that's the "time out" that it's checking for. While this may at first sound like an odd feature, it is in fact extremely useful in combination with the custom logic and event system. The magic is that because it is both reacting to and can output an event you can use it to do things *entirely* in the background. Among other things, it makes working with bespoke single wire protocols much easier.

#### 8-bit PWM mode
They can be pressed into service as a rather poor PWM timer. TCB in PWM mode is 8-bit only. When used for PWM, they can only generate 8-bit PWM, despite being a 16-bit timer, because the 16-bit `TCBn.CCMP` register is used for both the period and the compare value in the low and high bytes respectively. They always operate in single-slope mode, counting upwards, the selection of clock options is limited in the extreme: system clock, half the system clock, or the frequency depends on that of the TCA (or one of the TCA's on parts with more than one).  In other words, **the type B timers are not very good at generating PWM**.

**Errata Alert** - `TCBn.CCMP` is effected by silicon errata on all available silicon save the DD and EA-series: It still acts like a 16-bit register. That means that it uses the TCB.TEMP register for access, and that you must read and write both bytes together, starting with the low byte, then high byte: Writes to the low byte are redirected to the temp register, reading the low byte copies the high byte of the CCMP register to TEMP, and writing to the high byte is what copies the low byte from the TEMP register to the actual CCMP register low byte. However, if you only write the high byte, and write the low byte only once, writes to the high byte alone do work. Until you do something like reading the CNT register, at which point everything will fall over.

#### Extra features on 2-series and Dx/Ex-series
The 2-series and Dx parts add three upgrades, two useful, and the other less-so. The less useful one is a separate OVF event/interrupt source. I find this to be of dubious untility - likely the best use of the separate OVF bit is as a 17th bit in input capture mode, but this can generally be done without using it as an interrupt.

A much more interesting option is the clock-on-event option: The TCBs now have a second event user, which can be selected as the clock source! Combined with the CCL, this can, for example, be used to get a prescaled system clock into the TCB different from that of a TCA (see the Logic library documentation and examples for discussion of how the CCL filter and synchronizer can be used to generate s prescaled clock).

Finally, on these parts, you can combine two timers to make a single, monster, 32-bit input capture timer using the CASCADE feature. To do this, you configure the high byte timer such that it is clocked from the OVF event of the low timer, and the CASCADE bit enabled. When the event fires, both timers will get their CCMP values saved and/or be stopped, permitting incredible precision (or just really long things) to be measured. 2<sup>32</sup> system clocks, which is around 3 minutes at 24 MHz. Prescaling the the low timer by 2 would double that. Ever wanted to time approximately 6 minute long event to a resolution of tenths of a microsecond? Er, sorry, I meant tenths of some period of time that is within a percent or so of a microsecond unless using an external crystal. This could also be useful if you were, instead of time, counting events on the first timer, but 16 bits just wasn't enough.

#### Intflag summary
Sometimes the intflags automatically clear, but usually they don't. Be sure to clear them manually in the cases where it isn't done automatically!

The OVF flag only exists on 2-series, DA, DB, DD, and EA parts

| Mode              | CAPT Reset by | OVF reset by | CAPT set under normal use  | OVF set under normal use   |
|-------------------|---------------|--------------|----------------------------|----------------------------|
| Interrupts        | User code     | User code    | When CNT reaches CCMP      | Only when CCMP set below CNT and allowed to roll over |
| Timeout Check     | User code     | User code    | When CNT reaches CCMP      | Only when time between events that reset it exceeds 2^16 ticks |
| Input capt. Event | Read CCMP     | User code    | When event occurs and CNT copied to CCMP | Every time the counter rolls over, Rarely needed |
| Input capt. (all) | Read CCMP     | User code    | When capture occurs, depending on mode |When counter rolls over. Effectively 17th bit |
| Singleshot        | User code     | User code    | Never                      | Never                      |
| PWM               | User code     | User code    | When CNT reaches CCMPH     | At end of every PWM cycle, when CNT reaches CCMPL |


### TCD0 - Type D 12-bit Async Timer
The Type D timer, is a very strange timer indeed. It can run from a totally separate clock supplied on EXTCLK, or from the unprescaled internal oscillator - or, on the Dx-series, from the on-chip PLL at 2, 3, or even 4 times the speed of the external clock or internal oscillator! (the 4x multiplication option is unofficial though - it was in the very earliest headers, but was neverin the datasheet andwas removed from headers shortly thereafter. It works under favorable conditions though). It was apparently designed with a particular eye towards motor control and SMPS control applications. This makes it very nice for those sorts of use cases, but in a variety of ways ,these get in the way of using it for the sort of things that people who would be using the Arduino IDE are likely to want to do with it. First, none of the control registers can be changed while it is running; it must be briefly stopped, the register changed, and the timer restarted. In addition, the transition between stopping and starting the timer is not instant due to the synchronization process. This is fast (it looks to me to be about 2 x the synchronizer prescaler 1-8x Synchronizer-prescaler, in clock cycless. The same thing applies to reading the value of the counter - you have to request a capture by writing the SCAPTUREx bit of TCD0.CTRLE, and wait a sync-delay for it. can *also* be clocked from the unprescaled 20 MHz (or 16 MHz) internal oscillator, even if the main CPU is running more slowly. - though it also has it's own prescaler - actually, two of them - a "synchronizer" clock that can then be further prescaled for the timer itself. It supports normal PWM (what they call one-ramp mode) and dual slope mode without that much weirdness, beyond the fact that `CMPBSET` is TOP, rather than it being set by a dedicated register. But the other modes are quite clearly made for driving motors and switching power supplies. Similar to Timer1 on the ATtiny x5 and x61 series parts in the classic AVR product line,  this timer can also create programmable dead-time between cycles.

It also has a 'dither' option to allow PWM at a frequency in between frequencies possible by normal division of the clock - a 4-bit value is supplied to the TCD0.DITHER register, and this is added to a 4-bit accumulator at the end of each cycle; when this rolls over, another clock cycle is inserted in the next TCD0 cycle.

The asynchronous nature of this timer, however, comes at a great cost: It is much harder to use than the other timers. Most changes to settings require it to be disabled as noted above - and you need to wait for that operation to complete (check for the `ENABLERDY` bit in `TCD0.STATUS`). Similarly, to tell it to apply changes made to the `CMPxSET` and `CMPxCLR` registers, you must use the `TCD.CTRLE` (the "command" register) to instruct it to synchronize the registers. Similarly, to capture the current count, you need to issue a SCAPTUREx command (x is A or B - there are two capture channels) - and then wait for the corresponding bit to be set in the `TCD0.STATUS` register. In the case of turning PWM channels on and off, not only must the timer be stopped, but a timed write sequence is needed ie, `_PROTECTED_WRITE(TCD0.FAULTCTRL,value)` to write to the register that controls whether PWM is enabled; this is apparenmtly because, in the intended use-cases of motor and switching power supply control, changing this accidentally (due to a wild pointer or other software bug) could have catastrophic consequences. Writes to any register when it is not "legal" to write to it will be ignored. Thus, making use of the type D timer for even simple tasks requires careful study of the datasheet - which is a frustratingly terse chapter at key points, yet is STILL the longest chapter at 50 pages (counting only chapters that are mostly text (so the electrical/typical charachteristics section doesn't count).

### TCE - Lurking in the distance with WEX
The recently announced EB-series will feature not one but TWO new timers.
Nothing is currently known about the TCE other than it can output 8 PWM channels in some configurations, and six in others, and that it's a 16-bit timer, and that it has WEX. Whether these are independent channels or whether half of them must be the inverse of the other half is not known (that has precedent on classic AVRs. I would have hoped they have learned).

WEX is a feature from xMega. One hopes it has been streamlined, as it was incomprehensible there (like essentially everything about xmega). It allowed generation of enhanced resolution PWM, and probably some other things (like I said, it was incomprehensible). Only time (specifically the moment that datasheet is released) will reveal whether WEX is friendly, or if heinous supervillian WEX Luther has escaped imprisonment on xMegatraz and is out for revenge...

Even though the EB-series, from a great distance away, is looking like something less than a big prize, the timers it gets may be a sign of good things to come. Or bad things.

### TCF - Another new timer
No, I didn't make a mistake there, yes, they're both coming in the same release. We know even less about this one. For example we know that TCE is going to be built around a 16-bit timer. TCF lists 16-bits on one page and 24 on another. And it suggests that the timer's waveform outputs are for "frequency generation" (read: duty cycle 50%; now that would represent a significant escalation of the the abuse we're taking here, and which is starting to look like the theme of the family. We already have the main feature of the family on the 2-series with the new ADC, but then they had to rip out the good internal oscillator and switch back to the tinyAVR one and cut the non-tiny EB's down to sub-tiny peripheral limitations with only USART? You think we just wanted pin options? There is a HUGE difference netween 1 UART and 2 - it's a larger difference (between 1 and 2 than there is between and 5, and only a little smaller than 0 versus 1 USART). By EB release we should have gotten to look at the titles of the chapters for the next installment or two of AVRs and we'll know whether the TCE and TCF are going almost everywhere or hardly anywhere - so the size of our wager will be set, and we will just have to wait a bit longer to see whether the Ex-series is as Exey as the double D's - or whether AVR is going to be like Windows (where alternating releases were either great or awful (by the standards of the company, to be clear. I don't think any hardware company could beat that record unless it blew up unexpectedly - (the unexpectedly bit is key, otherwise it's a different industry, and a rather lucrative one. A device that would burst into flames under programmable conditions would be an unsavory, but not unprofitable line of business. If it only burst into flames at random, well, that's not going to do well.  Samsung tried it a few years back, wasn't much of a success. I think I'd still rather keep my phone in an asbestos lined suitcase than use Windows ME...

> Hm? Where did I get the nice suitcase? Thanks, it's it from Hazmart Short & Small Menswear, same place as the buy one three free deal on Note 8's. Whether you are looking for Russian nerve agents, fissile material to fuel a home-built nuclear reactor or just trying to keep up with the family down the street and their pet tiger, they're's something for everyone on your list. There's no shopping experience quite like Hazmart. Hazardous goods for hazardous people. See in store for details on how you can win this stylish mercury fountain featuring a genuine poisonivywood base. Nothing quite like a mercury fountain on a genuine poison ivy wood base...
> Quiz - Which one of the following actually existed: The home nuclear reactor? The asbestos-lined fireproof clothing? The poison ivy knicknacks? The mercury fountain? Or the radium-infused shampoo?

Answers are **somewhere in this long and otherwise rather dry document**

### RTC - 16-bit Real Time Clock and Programmable Interrupt Timer
Information on the RTC and PIT will be added in a future update.

## Timer Prescaler Availability

Prescaler    | TCAn  | TCBn  | TCD0  | TCD0 sync | TD0 counter |
------------ | ------|-------|-------|-----------|-------------|
CLK          |  YES  |  YES  |  YES  |  YES      |  YES        |
CLK2         |  YES  |  YES  |  YES* |  YES      |  NO         |
CLK/4        |  YES  |  TCA  |  YES  |  YES      |  YES        |
CLK/8        |  YES  |  TCA  |  YES* |  YES      |  NO         |
CLK/16       |  YES  |  TCA  |  YES* |  NO       |  NO         |
CLK/32       |  NO   |  NO   |  YES  |  NO       |  YES        |
CLK/64       |  YES  |  TCA  |  YES* |  NO       |  NO         |
CLK/128      |  NO   |  NO   |  YES* |  NO       |  NO         |
CLK/256      |  YES  |  TCA  |  YES* |  NO       |  NO         |
CLK/1024     |  YES  |  TCA  |  NO   |  NO       |  NO         |

* Requires using the synchronizer prescaler as well. My understanding is that this results in sync cycles taking longer- I believe it takes 2-3 synchronizer clocks, with is 2-3 clock cycles without prescaling (just fast enough so that if you don't check it, and immediately write an enable locked register, it won't work) and 16-24 with maximum prescale. This is also how long it takes after disabling the timer for the EN_READY status flag to return indicating that you can turn it back on again.
*
`TCA` indicates that for this prescaler, a TCA must also use it, and the TCB set to use that TCA's clock.

## Resolution, Frequency and Period
When working with timers, I constantly found myself calculating periods, resolution, frequency and so on for timers at the common prescaler settings. While that is great for adhoc calculations, I felt it was worth some time to make a nice looking chart that showed those figures at a glance. The numbers shown are the resolution (when using it for timing), the frequency (at maximum range), and the period (at maximum range - ie, the most time you can measure without accounting for overflows).
### [In Google Sheets](https://docs.google.com/spreadsheets/d/10Id8DYLRtlp01KA7vvslC3cHaR4S2a1TrH7u6pHXMNY/edit?usp=sharing)

## Section Two: How the core uses these timers

### Initialization
This section consists of around 6 points, Under many of these points are the consequences for users and any thing the user might need to do.

1. All TCAs are initialized for SPLIT mode to generate 6x 8-bit PWM channels, not 3 16-bit ones. If you need that, you must implement it yourself and `takeOverTCA0();` (or TCA1 - TCA1 is probably a better choice since TCA0 has lots of 6-pin options while TCA1 does not) in `setup()`. The PORTMUX register is set according to the part specific documentation page for the part you are using.
2. If you want to take over a type A timer, simply call takeOverTCA0() or takeOverTCA1() - this will stop the timer, *send the reset command to reset it's registers to their defaults* and set a flag that tells the core not to use the timer for analogWrite().
  a. That means it will no longer be in SPLIT mode. If SPLIT mode is desired, simply write TCA0.CTRLD to 1 before enabling the timer.
  b. You may not takeOverTCAn if that timer is used for millis (it will be treated as a badCall compile error to call the takeOver function). Reconfiguring it anyway will likely cause unexpected, undefined, and undesired behavior, potentially impacting anything relating to PWM and timekeeping. So don't do that.
  b. If any type B timers are being used for PWM, remember that they share the prescaler settings with a TCA, and changing the frequency of the TCA will also change that of the TCB(s) following it. See TCB notes at bottom of this list.
3. The TCD is initialized for PWM in single-ramp mode. See [the TCD reference](Ref_TCD.md) for specifics about what speeds and what value it counts up to - TOP is 254 *or* a power-of-two multiple of that depending on clock speed, and other power-of-two multipliers may be set by the user at runtime with neither takeOverTCD0() nor custom code to handle updating the duty cycle! The PORTMUX register (being broken on DA/DB parts) is never set to anything other than the default value there. It works on the DD-series, but there is only one case where a certain option is unambiguously better: on the DD14 parts only the ALT4 mux option has any pins at all, So on the DD14's we set the portmux to ALT4.
4. If you do `takeOverTCD0()`, that will stop the timer at the end of the current cycle, and that function will wait until that has happened before returning (it will also, at that time, clear the TCD intflag register). , but **it is still your responsibility to clear out any settings the core put there** because there is no reset command. It may be best to override init_TCD() with an empty function, takeOverTCD0 immediately in setup, and reconfigure it then.
  a. Once you have taken over TCD0 with that function, if you want to restore TCD0 to it's power on reset state, you need to set these registers to 0: TCD0.CTRLA (this controls the prescalers as well as enabling), TCD0.CTRLC (the core sets this to 0x40 so WOD outputs the second channel), TCD0.CMPASET`*`, TCD0.CMPBSET`*`, TCD0.CMPACLR, TCD0.CMPBCLR.
  b. `__PROTECTED_WRITE(TCD0_FAULTCTRL,0)` `*` (this register is not just enablelocked. It is protected as well)
  c. You also need to set set TCD0.STATUS`*` = 0xC0 to clear the PWMACT bits if you need them cleared (If you don't know what I'm talking about you don't care, and if you do, you know if you care).
  d. You will never *literally* write 0 to that list of registers, since if you're taking over the type D timer, presumably you plan to reconfigure it to do something else. Those are just the registers you need to be sure to hit duruing this process in order to ensure that there's nothing left of the core-set options. Just remember to save TCD0.CTRLA (or at least it's enable bit) for last, because almost everything is enable-locked on the TCD.
  e. Registers marked with `*` can be assumed to be in power on reset state if you haven't set them manually nor called analogWrite() with them.
  f. The above ONLY applies to DxCore or megaTinyCore when a non-type-D timer is used for millis. On megaTinyCore, if you've used the type D timer for millis, takeOverTCD0 is not available, and TCD0 cannot be configured without causing gross misbehavior. If you need to take over TCD0 on mTC, make sure a different timer is chosen for millis in the tools submenu.
5. Assuming a TCB is used for millis(), it is configured for periodic interrupt mode. User code should absolutely not mess with the millis timer, otherwise unexpected, undefined, and undesired behavior is likely to occur relating to timekeeping or whatever you are trying to make the timer do. Turn off millis, or use a different timer.
6. PORTMUX.TCBROUTEA is set during initialization. The value here is assumed to never change. No method is provided to change this at the current time. The two conditions where this may be a problem are:
  a. If you are *reconfiguring a TCB, but are still using it 8-bit PWM mode*, AND there is no other timer available on the same pin as the one the core used by default, digitalWrite() and analogWrite() must not be called on that pin (digitalWriteFast() can be used instead, as it does not do anything relating to PWM).
  b. If you are changing the TCB portmux hoping to get PWM on different pins, it won't work. This can only be done by creating a new variant file which changes the
6. All other TCBs are configured for 8-bit PWM (even though they suck as PWM timers), so that analogWrite() can be used with them.
  a. There is no takeOverTCBn() function. Just remember that *THE TIMER IS NOT Ihe default state
  a. analogWrite won't try to use a TCB whose CTRLB register is not set for PWM - in that case, either you have reconfigured the timer (so we shouldn't mess up your configuration), or it's the millis timer, and we *definitely* shouldn't mess with that.

### Justification for TOP = 254, not 255
Iop were 255, there would be exactly 256 timer ticks per clock, If TOP is 355 ticker per clock cycle. The roblem would be  - how would you set th duty cycl;
### PWM via analogWrite()

#### Priority
Many pins have more than one possible timer that they could use. When asked to write PWM to a pin, the order of priority used is as listed below. The DAC is obviously not a timer; it is however USED like a timer as far as PWM is concerned. analogWrite() treats the DAC pin as if the DAC were a timer, and the duty cycle as the output voltage. See [ADC and DAC reference for more info on that](Ref_Analog.md).
1. TCA0
2. TCA1
3. DAC or TCD0 (these never overlap; there is no code written to support hypothetical parts where there's a DAC that can use the same pin as a TCD or TCB)
4. Any TCB.

In the future EB-series, the timer priorities will have to be considered carefully.

#### Channels without pins
If you are using a part (like a DD-series) where the pin mapping being used doesn't provide a pin for a given timer output channel, that channels is not available... at least not directly. However it is still there, it can still be used as an internal input... which means it can be output through a CCL logic block. See the CCL chapter of the datasheet or the [Logic library docs for information on how you would implement these](../libraries/Logic/README.md).

However, you CAN use the CCL (either manually or with the Logic library) and sent it to use input 0, 1, or 2 from a TCA, TCB, or TCD. In the case of a TCA, the input number of the logic block is the waveform output - so only WO0-WO2 (the outputs that go on Px0, Px1, and Px2). In the case of a TCB, the input number is the number of the type B timer (only the first three timers can be used in this way). And in the case of the TCD, input 0 is WOA, input 1 is WOB, and input 2 is WOC. But WOC just mirrors WOA (unless you've changed TCD0.CTRLC) - so it's just a way of using the WOA channel as input 2 instead of input 0, which may be important (since CCL input 2 is special, particularly for even numbered LUTs). Whereas the event system gives you a relatively useless 1-clock long pulse when the PWM compare match occurs, the CCL has access to the levels. So you if were using a DD14, with the TCA mux set to the most-pinful and the default of the core, PORTC, there is still no PC0, and only WO1-3 of the TCA available. Between those three and TCD0 on PD4 and PD5, you've still only got 5 PWMs.

But you could set input0 of LUT2 to be TCA0, enable the alternate pin mapping for the CCL LUT2 output (CCL output is on pin 3 or 6 of the LUT's "home port"), mask the other two inputs and set the truth table to 0x02, and get TCA0 WO0 on PD6. 6 PWM channels! Hm? You need seven you say? Well, okay - you could use TCB0 as the input for a the CCL LUT0, 1, or 3, but instead of enabling pin output (since those pins don't exist, or in the case of LUT1 on PORTC are already being used), you would have to instead set one of the event channels to use that CCL LUT as it's generator. Event output can be piped to either pin 2 or 7 in a port. PORTD is your only good choice. Then you could set EVOUTD to be an event user for that channel, and switch it's portmux to the alternate pin, PIN_PD7. Now you've got 7 channels! You need *another*? You can actually get an 8th if you really work at it - but millis must be either running on TCA0 or disabled, you must have an HV UPDI programmer to reprogram it, otherwise the required fuse settings will brick it. You would have to disable the UPDI functionality (hence it cannot be reprogrammed without an HV programmer). That turns PF7 into a GPIO pin (reset can be turned into an input, but not an output`*`) This has very few functions available - in fact it has only 2 special functions, the SS pin of one of the wacky SPI mappings on the DD-series... and the alternate output pin of EVOUTF! You can then repeat the process above with TCB1 and EVOUTF. And at that point you have 8 independently controllable duty cycles coming out of 8 pins on your 14-pin package, of which 3 pins are power or ground, and another of which (PF6/reset) is input only. So 80% of the potential output pins and more over half of the physical pins could be used to output PWM without even having to use some sort of software PWM scheme - once the duty cycles were set and the peripherals enabled, the PWM would be generated without CPU intervention. The two remaining pins could then be used as, say, a serial port so you could tell it what duty cycles to use or something.

`*` On modern AVRs, only tinies have the option to set the pin nominally used as reset as an I/O pin (to the extent that one can call PA0 on a modern tinyAVR the nominal reset pin - by default it's the UPDI pin, but it can be made to be the reset pin instead. If that's done it needs HV programming to reprogram) - non-tiny modern AVRs can use it as an input, but not an output. On the parts where it can be used as one, it's a miserable output: even sourcing or sinking just 0.5mA and running at Vdd = 5V (which maximizes the pin driver strength) could be a volt away from the rail it's trying to drive towards (on those parts, a pin sourcing or sinking so much current that it was a volt away from the power rail would be carrying at least 15mA. The same was true on classic AVRs when reset was disabled - it could be turned into I/O, but it was a crap output). You may be wondering why this is. The answer - apparently - is that whichever pin the HV for HV programming has to touch is forced to use weaker pin drivers. This isn't entirely surprising - imagine using MOSFETS and a very weak input. If the output would never be higher than supply or lower than ground, you'd just have a complementary pair of FETs... but if it can also be an input, which can go above supply, the high side P-channel FET would turn into a diode and channel the higher voltage to the positive supply rail! (the same would happen on the low side and voltages below 0V) Either would obviously be a Bad Thing, so on the pin that is designed to be exposed to 12v, they had to take precautions, and that ends up weakening the pin drive greatly. This hurts particularly on tinyAVR parts, because that's also UPDI: it is much easier for the tinyAVR's UPDI pin to get overwhelmed by some other source of voltage (ex, a pullup that's stronger than it should be) than other parts. Even optimistically assuming that the current will increase linearly if we accept higher voltage drop (not a valid assumption, generally speaking: all else being equal, doubling the current will generally more than double the voltage drop; see the I<sub>D</sub> vs V<sub>ds</sub> plot in any MOSFET datasheet, where lines are shown for each V<sub>gs</sub>: the result is linear when current is well below it's rated maximum, while after a point increasing the voltage drop has little impact on the current.

#### TCAn
The core reconfigures the type A timers in split mode, so each can generate up to 6 PWM channels simultaneously. The `LPER` and `HPER` registers are set to 254, giving a period of 255 cycles (it starts from 0), thus allowing 255 levels of dimming (though 0, which would be a 0% duty cycle, is not used via analogWrite, since `analogWrite(pin,0)` calls `digitalWrite(pin,LOW)` to turn off PWM on that pin). This is used instead of a PER=255 because `analogWrite(255)` in the world of Arduino is 100% on, and sets that via `digitalWrite()`, so if it counted to 255, the arduino API would provide no way to set the 255/256th duty cycle). Additionally, modifications would be needed to make `millis()`/`micros()` timekeeping work without drift when TCA is selected for timekeeping. Preventing drift is easy with PER = 254, but hard at PER = 255 (.
The core supports generating PWM using up to 6 channels per timer, and will work with alternate PORTMUX settings as long as the the selected option isn;t one of the three-channel ones for TCA1 - those are not supported. TCA1 can be on PB0-5 or PG0-5 (and not even the latter on DA due to errata). TCA0 can go on pin 0-5 in any port (though they must all be on the same port. We default to configuring it for PD on 28/32 pin parts, PA on 14 and 20-pin  and PC on 48/64 pin ones).

`analogWrite()` checks the `PORTMUX.TCAROUTEA` register, and on DD-series parts (and DA/DB if we ever get a fix) the `PORTMUX.TCDROUTEA` register to know which pins might have PWM from those timers,.


**Because analogWrite() supports PWM on TCB-controlled pins, any timer not being used for millis is configured in PWM mode on initialization. You are responsible for resetting the timers first if you need them for something else!**


#### bool digitalPinHasPWMNow(uint8_t p)
This function returns 1 if the pin currently has PWM available accounting for PORTMUX and `takeOverTCxn`. The dynamic analog of `digitalPinHasPWM()` - obviously, not compile-time constant.

#### uint8_t digitalPinToTimerNow(uint8_t p)
This function returns the timer the pin will be controlled by accounting for PORTMUX and `takeOverTCxn`. The dynamic analog of `digitalPinToTimer()` - obviously, not compile-time constant.

### TCD0
TCD0, by default, is configured for generating PWM (unlike TCA's, that's about all it can do usefully). TCD0 is clocked from the CLK_PER when the system is using the internal clock without prescaling. On the prescaled clocks (5 and 10 MHz) it is run it off the unprescaled oscillator (just like on the 0/1-series parts that it inherits the frequencies from), keeping the PWM frequency near the center of the target range. When an external clock is used, we run it from the internal oscillator at 8 MHz, which is right on target.

It is always used in single-ramp mode, with `CMPBCLR` (hence TOP) set to either 254, 509, or 1019 (for 255 tick, 510 tick, or 1020 tick cycles), the sync prescaler set to 1 for fastest synchronization, and the count prescaler to 32 except at 1 MHz. `CMPACLR` is set to 0xFFF (the timer maximum, 4095). The `CMPxSET` registers are controlled by `analogWrite()` which subtracts the supplied dutycycle from 255, checks the current CMPBCLR high byte to see how many places to left-shift that result by before subtracting 1 and writing to the register. The `SYNCEOC` command is sent to synchronize the compare value registers at the end of the current PWM cycle if the channel is already outputting PWM. If it isn't, we have to briefly disable the timer, turn on the pin, and then re-enable it, producing a glitch on the other channel. To mitigate this issue we treat 0 and 255 duty cycles differently for the TCD pins - they instead set duty cycle to 0% without disconnecting the pin from the timer, for the 100% duty cycle case, we invert the pin (setting CMPxSET to 0 won't produce a constant output). This eliminates the glitches when the channels are enabled or disabled.

TCD0 has two output channels - however, each of them can go to either of two pins. PA5 and PA7 use WOB, and PA4 and PA6 use WOA.
```c++
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

The frequency of PWM output using the settings supplied by the core is shown in the table below. The "target" is 1 kHz, never less than 490 Hz or morethan 1.5 kHz. As can be seen below, there are several frequencies where this has proven an unachievable goal. The upper end of that range is the point at which - if PWMing the gate of a MOSFET - you have to start giving thought to the gate charge and switching losses, and may not be able to directly drive the gate of a modern power MOSFET and expect to get acceptable results (ie, MOSFET turns on and off completely in each cycle, there is minimal distortion of the duty cycle, and it spends most of it's "on" time with the low resistance quoted in the datasheet, instead of something much higher that would cause it to overheat and fail). Not to say that it **definitely** will work with a given MOSFET under those conditions (see [the PWM section of my MOSFET guide](https://github.com/SpenceKonde/ProductInfo/blob/master/MOSFETs/Guide.md#pwm) for calculations and a shared spreadsheet that helps calculate  ), but the intent was to try to keep the frequency low enough that that use case was viable (nobody wants to be forced into using a gate driver), without compromising the ability of the timers to be useful for timekeeping.
#### TCA0

|   CLK_PER | Prescale A |   fPWM  | Prescale D  | TOP D |  fPWM (D) | Notes                                            |
|-----------|------------|---------|-------------|-------|-----------|--------------------------------------------------|
| ** 48 MHz |        256 |  735 Hz |             |       |           |                                                  |
| ** 44 MHz |        256 |  674 Hz |             |       |           |                                                  |
| ** 40 MHz |        256 |  613 Hz |             |       |           |                                                  |
| ** 36 MHz |        256 |  551 Hz |             |       |           |                                                  |
|  External |            |         | OSCHF@8  32 |   254 |    980 Hz |                                                  |
|  * 32 MHz |        256 |  490 Hz |          32 |  1019 |    980 Hz |                                                  |
|  * 30 MHz |         64 | 1836 Hz | OSCHF@8  32 |   254 |    980 Hz | TCD using OSC@8 because running from xtal/ext    |
|  * 28 MHz |         64 | 1716 Hz |          32 |  1019 |    858 Hz |                                                  |
|  * 27 MHz |         64 | 1654 Hz | OSCHF@8  32 |   254 |    980 Hz | TCD using OSC@8 because running from xtal/ext    |
|    25 MHz |         64 | 1532 Hz |          32 |  1019 |    766 Hz |                                                  |
|    24 MHz |         64 | 1471 Hz |          32 |  1019 |    735 Hz |                                                  |
|    20 MHz |         64 | 1225 Hz |          32 |   509 |   1225 Hz |                                                  |
|    16 MHz |         64 |  980 Hz |          32 |   509 |    980 Hz |                                                  |
|    14 MHz |         64 |  858 Hz | OSCHF@28 32 |  1019 |    858 Hz | Unsupported, timing functionality not guaranteed |
|    12 MHz |         64 |  735 Hz |          32 |   509 |    735 Hz |                                                  |
|    10 MHz |         64 |  613 Hz | OSCHF@20 32 |   509 |   1225 Hz |                                                  |
|     8 MHz |         64 |  490 Hz |          32 |   254 |    980 Hz |                                                  |
|     7 MHz |         16 | 1716 Hz | OSCHF@28 32 |  1019 |    858 Hz | Unsupported, timing functionality not guaranteed |
|     6 MHz |         16 | 1471 Hz | OSCHF@12 32 |   509 |    735 Hz | Unsupported, timing functionality not guaranteed |
|     5 MHz |         16 | 1225 Hz | OSCHF@20 32 |   509 |   1225 Hz |                                                  |
|     4 MHz |         16 |  980 Hz |          32 |   254 |    490 Hz |                                                  |
|     3 MHz |          8 | 1471 Hz |           4 |   509 |   1471 Hz | Unsupported, timing functionality not guaranteed |
|     2 MHz |          8 |  980 Hz |           4 |   509 |    980 Hz | Unsupported, timing functionality not guaranteed |
|     1 MHz |          8 |  490 Hz |           4 |   254 |    980 Hz |                                                  |

`*` Overclocked (generally works, 28 and 32 can be achieved with internal oscillator)

`**` Way overclocked, may not work (requires external crystal or oscillator).

External clock or crystal will always cause TCD0 to use the internal oscillator by default. Speeds higher than 32 MHz can only use external clock sources, so they always act as described on the External line (unless reconfigured at runtime). Note that this was not imoplemented correctly prior to 1.4.0, and ran at half the intended speed.

`Prescale A` and `fPWM` apply to all pins not on TCD0. TOP is always set to 254 for TCA

`Prescale D`, `TOP D`, and `fPWM (D)` apply to the pins on TCD0.
In addition to the case of an external system clock source. Note that this did not work correctly prior to 1.4.0Where marked, we clock TCD0 from OSCHF instead of using CLK_PER, prescale by 32. For speeds other than 5 MHz and 10 MHz, we set the internal oscillator to 8 MHz.

These are the overall Timer D prescaler (in all cases, by default only the count prescaler is used), TOP, and resulting frequency of TCD0 PWM output.

Where TCD0 TOP is not 254, but is 509, 1019, or (above 32 MHz only) 2039, all duty cycles passed to `analogWrite()` for those pins will by left-shifted as necessary to get an appropriate duty cycle.

### Previous versions
Versions 1.3.1-1.3.6 had issues with PWM frequency at under some condition, and with micros readout in others. In 1.3.0 and earlier, there was no coherent list of timer settings based on part and clock speed.

## Millis/Micros Timekeeping
DxCore allows any of the type A or B timers to be selected as the clock source for timekeeping via the standard millis timekeeping functions. The RTC timers will be added after the sleep/low power library for this and tinyAVR 0/1-series is completed. There are no plans to support the type D timer - this is not like tinyAVR where we are desperately short of timers, with the comparatively difficult to use type D timer an irresistible victim to palm off the task of millis timekeeping on. Now, the calculations are more complicated since there are a great many possible speeds it could be running at, as opposed to just 16 or 20 on the tinyAVR 0/1-series. The timer used and system clock speed will effect the resolution of `millis()` and `micros()`, the time spent in the millis ISR, and the time it takes for `micros()`  to return a value. The `micros()` function will typically take several times it's resolution to return, and the times returned corresponds to the time `micros()` was called, regardless of how long it takes to return.

A table is presented for each type of timer comparing the percentage of CPU time spent in the ISR, the resolution of the timekeeping functions, and the execution time of micros. Typically `micros()`  can have one of three execution times, the shortest one being overwhelmingly more common, and the differences between them are small.


### TCAn for millis timekeeping
When TCA0 is used as the millis timekeeping source, it is set to run at the system clock prescaled by 8 when system clock is 1MHz, 16 when system clock is <=5 MHz, and 64 for faster clock speeds, with a period of 255 (as with PWM). This provides a `millis()`  resolution of 1-2ms, and is effectively not higher than 1ms between 16 and 30 MHz, while `micros()` resolution remains at 4 us or less. At 32 MHz or higher, to continue generating PWM output within the target range, we are forced to switch to a larger prescaler (by a factor of 4), so the resolution figures fall by a similar amount, and the ISR is called that much less often.




#### TCA timekeeping resolution
|   CLK_PER | millis() | micros() | % in ISR | micros() time |
|-----------|----------|----------|----------|---------------|
|    48 MHz |  1.36 ms |   5.3 us |   0.19 % |        2.5 us |
|    44 MHz |  1.48 ms |   5.8 us |   0.19 % |   aprx 3.5 us |
|    40 MHz |  1.63 ms |   6.4 us |   0.19 % |        3.5 us |
|    36 MHz |  1.81 ms |   7.1 us |   0.19 % |   aprx   4 us |
|    32 MHz |  2.04 ms |   8.0 us |   0.19 % |          4 us |
|    30 MHz |  0.54 ms |   2.1 us |   0.51 % |   aprx   4 us |
|    28 MHz |  0.58 ms |   2.3 us |   0.51 % |          4 us |
|    25 MHz |  0.65 ms |   2.6 us |   0.51 % |   aprx   4 us |
|    24 MHz |  0.68 ms |   2.7 us |   0.51 % |          5 us |
|    20 MHz |  0.82 ms |   3.2 us |   0.51 % |          7 us |
|    16 MHz |  1.02 ms |   4.0 us |   0.51 % |          9 us |
| !  14 MHz |  1.14 ms |   4.6 us |   0.51 % |   aprx  10 us |
|    12 MHz |  1.36 ms |   5.3 us |   0.51 % |         10 us |
|    10 MHz |  1.63 ms |   6.4 us |   0.51 % |         14 us |
|     8 MHz |  2.04 ms |   8.0 us |   0.51 % |         17 us |
| !   7 MHz |  0.58 ms |   2.3 us |   2.13 % |   aprx  18 us |
| !   6 MHz |  0.68 ms |   2.7 us |   2.13 % |   aprx  19 us |
|     5 MHz |  0.82 ms |   3.2 us |   2.13 % |         27 us |
|     4 MHz |  1.02 ms |   4.0 us |   2.13 % |         33 us |
| !   3 MHz |  0.68 ms |   2.7 us |   3.55 % |   aprx  45 us |
| !   2 MHz |  1.02 ms |   4.0 us |   3.55 % |   aprx  60 us |
|     1 MHz |  2.04 ms |   8.0 us |   3.55 % |        112 us |

`!` - Theoretical, these speeds are not supported and have not been tested

In contrast to the type B timer where prescaler is held constant while the period changes, here period (in ticks) is constant but the prescaler is not. Hence each prescaler option is associated with a fixed % of time spent in the ISR (and yes, for reasons I don't understand, the generated ISR code is slightly faster for /64 prescaling compared to /256, /16, and /8 (which are equal to each other).

The micros execution time does not depend strongly on F_CPU, running from 112-145 clock cycles.

Except when the resolution is way down near the minimum, the device spends more time in the ISR on these parts. Notice that at these points that - barely - favor TCAn, the interrupt they're being compared to is firing twice as frequently! TCD0's interrupt is slower than TCA's, but it is the timer least likely to be repupurposed.


#### TCBn for millis timekeeping
When a TCB is used for `millis()` timekeeping, it is set to run at the system clock prescaled by 2 (except at 1 or 2 MHz system clock) and tick over every millisecond. This makes the millis ISR very fast, and provides 1ms resolution at all but the slowest speeds for millis. The `micros()` function also has 1 us or almost-1 us resolution at all clock speeds (though there are small deterministic distortions due to the performance shortcuts used for the microsecond calculations (see appendix below). A TCB (TCB2 if there is one, otherwise TCB1) is used for millis by default on DxCore.

ISR execution time was decreased by 25% from 2.6.1 of megaTinyCore through reimplementation of the ISR is assembly, meaning less time spent incrementing millis and more time spent executing your code.

|Note | CLK_PER | millis() | micros() | % in ISR | micros() time | Terms used             |
|-----|---------|----------|----------|----------|---------------|------------------------|
|     |  48 MHz |     1 ms |  1.33 us |   0.14 % | *      2.5 us | 9 (0-7, 9)             |
| !   |  44 MHz |     1 ms |     1 us | ! 0.15 % | !      3-4 us | 5/7 ( ~1,~ 2, 5 ~6,~ 7, ~10~)|
|        40 MHz |     1 ms |     1 us |   0.17 % | *      3-4 us | 6 (2,4,6,8,10)         |
| !   |  36 MHz |     1 ms |     1 us |   0.18 % |        3-4 us | 3/5 (3,6, ~9,10~ )     |
|  †  |  32 MHz |     1 ms |     1 us |   0.20 % |          3 us | 1                      |
|     |  30 MHz |     1 ms |  1.07 us |   0.22 % |        < 6 us | 5 (3,4,7,8)            |
| !   |  28 MHz |     1 ms |  1.14 us |   0.23 % |       <= 6 us | 5/7 (2,3,5,6 ~8,9~ )   |
| !   |  27 MHz |     1 ms |  1.18 us |   0.24 % |       <= 6 us | 4 (2,4,9)              |
|     |  25 MHz |     1 ms |  1.28 us |   0.26 % |       <= 6 us | 3/5 ( ~1,~ 2, ~4,~ 5)  |
|   * |  24 MHz |     1 ms |  1.33 us |   0.27 % |          5 us | 9 (0-7, 9)             |
|   * |  20 MHz |     1 ms |     1 us |   0.33 % |          6 us | 6 (2,4,6,8,10)         |
|  †  |  16 MHz |     1 ms |     1 us |   0.40 % |          6 us | 1                      |
| !   |  14 MHz |     1 ms |  1.14 us |   0.47 % | approx. 12 us | 5/7 (2,3,5,6 ~8,9~ )   |
|   * |  12 MHz |     1 ms |  1.33 us |   0.54 % |         10 us | 9 (0-7, 9)             |
|     |  10 MHz |     1 ms |     1 us |   0.65 % |         10 us | 6 (2,4,6,8,10)         |
|  †  |   8 MHz |     1 ms |     1 us |   0.80 % |         11 us | 1                      |
| !   |   7 MHz |     1 ms |  1.14 us |   0.94 % | approx. 25 us | 5/7 (2,3,5,6 ~8,9~ )   |
| ! * |   6 MHz |     1 ms |  1.33 us |   1.08 % |       > 20 us | 9 (0-7, 9)             |
|   * |   5 MHz |     1 ms |     1 us |   1.30 % |         23 us | 6 (2,4,6,8,10)         |
|  †  |   4 MHz |     1 ms |     1 us |   1.18 % |         21 us | 1                      |
| ! * |   3 MHz |     1 ms |  1.33 us |   1.61 % |       > 40 us | 9 (0-7, 9)             |
| !†  |   2 MHz |     2 ms |     1 us |   1.18 % |         39 us | 1                      |
|  †  |   1 MHz |     2 ms |     1 us |   2.36 % |         78 us | 1                      |

`!` - Theoretical, these speeds are not supported and have not been tested. % time in ISR and micros return times, where given are theoretically calculated, not experimentally verified.
`†` - Naturally ideal - no ersatz division is needed for powers of 2
`*` - indicates that the shift & sum ersatz-division is done in hand-optimized assembly because I just couldn't stand how stupid and stubborn the compiler was, and by the time I was done analyzing it, implementing it was mostly done. These use every term that would improve the accuracy of the approximation, shown in the final column. The algorithm divides the timer ticks (which is between zero and F_CPU/2000) by a power if 2 (by right shifting) to get term 0; a number of terms (generated by further rightshifting) are then added and subtracted to get the time since the last millis was counted, and that total is added to 1000x the millisecond count. The ideal number of terms varies from 1 (for powers of 2 - only term 0) to 9 (for the terrible twelves (12, 24, 48, as well as 3 and 6). In optimized cases, that's how many are used. Otherwise, some may be dropped, or different combinations used (adding 1 term, then subtracting the next, would seem equivalent to just adding the next - but because of integer math, it's not, and in fact can lead to several us of backwards timetravel, so if you'd have to do the same operation twice in a row, you get more accurate numbers if you avoid that by adding another term). In optimized cases, as well as the powers of 2, the results are as good as possible in light of the specified resolution - where resolution is coarser than 1us, (1/resolution) of possible values for the three lowest digits (in normal base 10 number systems) is skipped and never shows up. These skipped values are distributed evenly between 1 and 999. For example, for 12/24/48, that means 250 values never show up. Of the remaining 750, 500 will show for only 1 "actual" microsecond value, and 250 for 2 consecutive "actual" microsecond values. See the table at the end of this document for the list of skipped least-significant-digit combinations. None of the optimized options will never go back in time, even by a single microsecond (as long as the chance of backward time travel is limited to less than the time it takes micros to execute, it can't break `delay()`, cause timeouts to instantly expire or cause other catastrophic consequences) nor are more than 2 values in a row ever skipped. Those criteria are not guaranteed to be met for other speeds, though we do guarantee that negative time travel will never occur that causes two consecutive calls to micros to return a smaller value for the second call which is what triggers misbehavior of timing code.

Resolution is always exactly 1ms for millis with TCB millis except for 1 and 2 MHz speeds, where the millis resolution is 2ms, though micros resolution is still 1us, and whereas TCAn `micros()` is limited by the resolution of the timer, here it's instead limited only by the fact that the calculations drop the least significant bits first; this results in a value that may be as low as 750, yet is being mapped to 0-999, for 1.33 us resolution in the worst cases. The timer count and the running tally of overflows could get us microseconds limited only by F_CPU/2
The percentage of time spent in the ISR varies in inverse proportion to the clock speed - the ISR simply increments a counter and clears its flags. 65 clocks from interrupt bit set to interrupted code resuming.

The time that `micros()` takes to return a value varies significantly with F_CPU; where not exact, value is estimated. Specifically, powers of 2 are highly favorable, and almost all the calculations drop out of the 1 and 2 MHz cases (the are similar mathematically - at 1 MHz we don't prescale and count to 1999, at 2 we do prescale and count to 1999, but then we need only add the current timer count to 1000x the number of milliseconds. micros takes between 78 and 160 clocks to run. Everything else has to use some version of a shift-and-sum ersatz division algorithm, since we want to approximate division using `>>`, `+` and `-`. Each factor of 2 increase in clock speed results in 5 extra clocks being added to micros in most cases (bitshifts, while faster than division, are still slow when you need multiples of them on larger types, especially in compiler-generated code that doesn't reuse intermediate values.)

The terms used, and - where different - the number that would be ideal, is listed above
### TCD0 for millis timekeeping
TCD0 is not supported for millis timekeeping on these parts. Originally it was imagined that the implementation from megaTinyCore could simply be used - but there the main clock was prescaled from 16 or 20 MHz, and TCD0 ran from unprescaled osc, giving 2 channels of normal speed PWM and a predictable timebase for millis even clocked at 1 MHz. That was useful. Here, the clock options are better, while the case for running at 1 MHz to save power weaker because the current of these is lower than for tinyAVRs by several times over.

### Manipulating millis timekeeping
There are a handful of functions exposed that manipulate the timekeeping:
```c
stop_millis(); // stops the millis timer. Call restart_millis to restart it.
restart_millis(); // restarts the millis timer after calling stop_millis()
set_millis(uint32_t);  // Set the current millisecond count to the argument provided. With a TCB timer, millis and micros will stay in sync. Because of the more complicated math involved with the TCA,
// which has to track fractional milliseconds, set_millis() should not be expected to maintain synchronization between millis and micros, and micros may see small distortions.
nudge_millis(uint16_t); // Set the current millisecond time this many milliseconds forward. The same caveat about micros not being changed with TCA for timing apply here. Unlike the above, this is specifically directed at the application where you have to do something that will block interrupts for long enough that you know you'll miss millis counts, so you canculate how many millis behind you'll end up, and pass that to nudge_millis();
```

## Tone
The `tone()` function included with DxCore uses one Type B timer. It defaults to using TCB0; do not use that for millis timekeeping if using `tone()`. Tone is not compatible with any sketch that needs to take over TCB0. If possible, use a different timer for your other needs. When used with Tone, it will use CLK_PER or CLK_PER/2 as it's clock source - the TCA clock will never be used, so it does not care if you change the TCA0 prescaler (unlike the official megaAVR core). It does not support use of the hardware output compare to generate tones like tone on some parts does. (Note that if TCB0 is used for millis/micros timing - which is not a recommended configuration, we usually use the higher numbered TCB -  `tone()` will instead use TCB1).

**Recent Fixes** - Prior to the release of 1.3.3, there were a wide variety of bugs impacting the `tone()` function, particularly where the third argument, duration, was used; it could leave the pin `HIGH` after it finished, invalid pins and frequencies were handled with obvously unintended behavior, and integer math overflows could produce weird results with larger values of duration at high frequencies, and three-argumwent `tone()` ddn't work at all above 32767Hz due to an integer overflow bug (even though maximum supported frequency is 65535 Hz).

**Long tones specifying duration** The three argument `tone()` counts the toggles of the pin for the duration. This means some very large numbers can cause problems. Cases where this is an issue are hereafter known as "Long tones". This core supports long tones through the three argument tone. A long tone is any tone where `4.294 billion < (frequency * duration) < 2.149 trillion` - by rearranging some division, as long as 16k or more flash is present on the part we are compiling for (it uses more memory). Anything longer is a "very long" tone and is never supported via the three argument `tone()`. Your earsplitting high-pitched whine with a duration of more than a few hours (which is far outside the regime that tone is intended for) must be implemented with a 2-argument tone() and millis. If it needs to be implemented at all. Even normal "long tones" require a high frequency tone requested for minutes, which is an extreme use case.

Tone works the same was as the normal `tone()` function on official Arduino boards. Unlike the official megaAVR board package's tone function, it can be used to generate arbitrarily low frequency tones (as low as 1 Hz). If the period between required toggling's of the pin is greater than the maximum timer period possible, it will calculate how many cycles it has to wait through between switching the pins in order to achieve the desired frequency.

It can only generate a tone on one pin at a time.

All tone generation is done via interrupts. The hardware output compare functionality is not used for generating tones because in PWM mode, the type B timers kind-of suck. Hardware-tone would require using a type A timer, and few are willing to sacrifice 6 pwm channels for tone - though the library would be simple.

## Servo Library
The Servo library included with this core uses one Type B timer. It defaults to using TCB1 if available, unless that timer is selected for Millis timekeeping. Otherwise, it will use TCB0. The Servo library is not compatible with any sketch that needs to take over these timers - if possible, use a different timer for your other needs. Servo and `tone()` can only be used together on when neither of those is used for millis timekeeping.

Regardless of which type B timer it uses, Servo configures that timer in Periodic Interrupt mode (`CNTMODE` = 0) mode with CLK_PER/2 or CLK_PER as the clock source, so there is no dependence on the TCA prescaler. The timer's interrupt vector is used, and it's period is constantly adjusted as needed to generate the requested pulse lengths. In 1.1.9 and later, CLK_PER is used if the system clock is below 10MHz to generate smoother output and improve performance at low clock speeds.

The above also applies to the Servo_DxCore library; it is an exact copy except for the name. If you have installed a version of Servo via Library Manager or by manually placing it in your sketchbook/libraries folder, the IDE will use that in preference to the one supplied with this core. Unfortunately, that version is not compatible with the Dx-series parts. Include Servo_DxCore.h instead in this case. No changes to your code are needed other than the name of the library you include.






## Additional functions for advanced timer control
  We provide a few functions that allow the user to tell the core functions that they wish to take full control over a timer, and that the core should not try to use it for PWM. These are not part of a library because of the required integration with the core to control `analogWrite()` and digitalWrite behavior.

### takeOverTCA0()
  After this is called, `analogWrite()` will no longer control PWM on any pins attached to timer TCA0 (though it will attempt to use other timers that the pin may be controllable with to, if any), nor will `digitalWrite()` turn it off. TCA0 will be disabled and returned to it's power on reset state. All TCBs that are used for PWM on parts with only TCA0 use that as their prescaled clock source buy default. These will not function until TCA1 is re-enabled or they are set to use a different clock source. Available only on parts with TCA1 where a different timer is used for millis timekeeping.

### takeOverTCA1()
  After this is called, `analogWrite()` will no longer control PWM on any pins attached to timer TCA1 (though it will attempt to use other timers that the pin may be controllable with to, if any), nor will `digitalWrite()` turn it off. TCA1 will be disabled and returned to it's power on reset state. All TCBs that are used for PWM on parts with TCA1 use that as their prescaled clock source buy default. These will not function until TCA1 is re-enabled or they are set to use a different clock source. Available only on parts with TCA1, and only when a different timer is used for millis timekeeping.

### takeOverTCD0()
  After this is called, `analogWrite()` will no longer control PWM on any pins attached to timer TCD0, nor will digitalWrite stop said PWM. This applies to anything that makes changes to the TCD peripheral. (Note: things that are part of the API but which don't involve changes to the configuration of TCD0() are not impacted. There is no way to reset type D timers like Type A ones other than at power on reset, short of casting &TCD0 to a volatile to export the configuration you want to restore, and...

### resumeTCA0()
  This can be called after takeOverTimerTCA0(). It resets TCA0 and sets it up the way the core normally does and re-enables TCA0 PWM via analogWrite.

### resumeTCA1()
  This can be called after takeOverTimerTCA1(). It resets TCA1 and sets it up the way the core normally does and re-enables TCA1 PWM via analogWrite.

## Appendix I: Names of timers
Whenever a function supplied by the core returns a representation of a timer, these constants will be used

| Timer Name   | Value | Peripheral |          Used for |
|--------------|-------|------------|-------------------|
| NOT_ON_TIMER |  0x00 |      None  | disabled millis   |
| TIMERA0 *    |  0x10 |      TCA0  | millis and/or PWM |
| TIMERA1 *    |  0x08 |      TCA1  | millis and/or PWM |
| TIMERB0      |  0x20 |      TCB0  | millis     or PWM |
| TIMERB1      |  0x21 |      TCB1  | millis     or PWM |
| TIMERB2      |  0x22 |      TCB2  | millis     or PWM |
| TIMERB3      |  0x23 |      TCB3  | millis     or PWM |
| TIMERB4      |  0x24 |      TCB4  | millis     or PWM |
| TIMERD0      |  0x40 |      TCD0  |               PWM |
| TIMERD0      |  0x40 |      TCD0  |           PWM WOA |
| TIMERD0      |  0x41 |      TCD0  |           PWM WOB |
| TIMERD0      |  0x42 |      TCD0  |           PWM WOC |
| TIMERD0      |  0x43 |      TCD0  | millis    PWM WOD |
| TIMERE0      |  TBD  |      TBD   | Likely will be to support ways to change TCD0 with the timer |
| TIMERE0      |  TBD  |      TBD   | USE_TCF0          |
| DACOUT ***   |  0x80 |      DAC0  | logWrite (outat)  |
| TIMERRTC     |  0x90 |      `RTC` | @ sleeping time   |
| TIMERRTC_XTAL|  0x91 | `RTC,32kXtal`| @ sleeping time   |
| TIMERRTC_EXT |  0x92 | `RTC,extclk`| @ sleeping time   |
| TIMERPIT     |  0x98 | `RTCetx  PIT` @ PD sleep time   |


0 (`NOT_ON_TIMER`) will be returned if the specified pin has no timer.

`*` Currently, the 3 low bits are don't-care bits. At some point in the future we may pass around the compare channel in the 3 lsb when using for PWM. No other timer will ever be numbered 0x10-0x17, nor 0x08-0x0F. 0x18-0x1F is reserved for hypothetical future parts with a third TCA. Hence to test for TCA type: `(timerType = MILLIS_TIMER & 0xF8; if (timerType==TIMERA0) { ... })`
`**` What was described above would look much like this. Currently we do not use the WOA/WOB/WOC/WOD specific values, but things will change when we get a working PORTMUX for it.
`***` A hypothetical part with multiple DACs with output buffers will have extend up into 0x8x.
`@` Planned for future use. All 0x9x values are reserved for future applications of the RTC and/or PIT and not other new kinds of timers to be determined at a future date.

All other unforeseen timer-like peripherals will be assigned to values above 0x9F, unless we have reason to think that the two will never coexist.
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

Those 250 "missing" microseconds manifest as repeats. It is difficult to intuitively understand why these values are the values they are; these were calculated using a spreadsheet to simulate the results for every timer count value:
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


## Appendix III: you made it through all the intervening 374 lines of textm, just to be told
> *It's a trick question!* Yes, every single one of those, or something close enough for our team of expert(s) to deem them equivalent. These have all actually existed in some form or another, at one time or another - right here on Earth, the works of mankind. A teenager really did make a nuclear reactor at home (people had tracked radioactive dust all over the neighborhood by the time someone realized what was going on). Asbestos cloth was widely used to make flexible fireproof objects, including clothing. Wooden knickknacks are in fact made through a traditional artform from trees that contain the same compounds as poison ivy (the same "active" ones, to be clear); they used it to make things like bowls and dishes and because it hardened into a durable plastic like material... though it doesn't pose the persistent toxicity of some modern plastics. Some emperor's tomb contained, or had contained at one point, a pond and possibly a fountain of mercury. And as most fans of danger already knew, around the turn of the century radioactive material (usually radium) was just one of the many poisons sold as medicine. They put it in all kinds of things. Brought to you by Hazmart - something for everyone on your "list"!
