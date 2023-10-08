# Detailed Clock Reference
This section seeks to cover information about the clocking options on the tinyAVR parts at somewhat greater depth than a simple list of supported options.
Be aware that this is very different from the equivalent document for the Dx and Ex-series. The oscillators are very different.

**Crystal** is not supported as a clock source on the tinyAVR, DA-series, or EB-series, but is on the DB, DD, EA, and DU-series.

## Background - how the clock is generated
See also [the timer reference](Ref_Timers.md)

### On tinyAVR
The OSCCFG fuse selects whether the speed is derived from a 20 MHz or 16 MHz oscillator. At power-on, these are always set to a prescaler of /6 hence 2.66 or 3.33 MHz. The core, prior to calling setup, will reconfigure the prescaler to match the requested speed. The OSCCFG fuse is always set when a sketch is uploaded via a UPDI programmer.
They additionally have the equivalent of the classic AVR's OSCCAL calibration register, and like those parts, the compliance is many times what the datasheet claims, and you can get a very wide range of speeds from them. 0/1-series has 64 settings for each nominal clock frequency, and 2-series parts have 128. Determining the value to set it to in order to obtain a desired clock speed is called "Tuning", even though you're not doing it for accuracy (it's generally already at the best setting from factory cal if you want the nominal clock speed at room temperature)

### On Dx-series
The OSCCTRL.OSCHFCTRLA register's *middle* 4 bits control the frequency of the internal oscillator. This field divides neatly into two sections. 0x0, 0x1, 0x2 and 0x3 are 1, 2, 3, and 4 MHz. The next one is reserved (I'd wager it would give 4 MHz). That's followed by the next section, 8, and every subsequent multiple of 4 up to 24 are listed in the datasheet, and at least at room temperature, extending the pattern 2 more rungs is consistent, and leads to speeds of 28 and 32 MHz, generally work at room temperature. (beyond that, the last 4 are repeated).

Additionally, there is a tuning register, CLKCTRL.OSCHFTUNE. Unfortunately, it is much less useful than the tinyAVR one. The register is limited to -32 to +31, corresponding to - and + around 16%. +/-16% doesn't exactly open up new frontiers.
If an external 32.768 MHz crystal has been connected, you can, for improved temperature stability, autotune the internal oscillator from this. I had to direct a torch at the chip (from a distance) to heat it up enough to see the speed autotune....
Not the feature I'm most impressed with. A+ on concept, C on execution.

Tuning to achieve non-standard speeds is not supported on the AVR Dx-series parts under DxCore.

### On Ex-series
The worst of both worlds! The OSCCFG fuse selects whether the speed is derived from a 20 MHz or 16 MHz oscillator. At power-on, these are always set to a prescaler of /6 hence 2.66 or 3.33 MHz. The core, prior to calling setup, will reconfigure the prescaler to match the requested speed. The OSCCFG fuse is always set when a sketch is uploaded via a UPDI programmer. So in this way, it is very much like tinyAVR.

However, the tuning register, CLKCTRL.OSCHFTUNE, is not nearly as powerful as it is on tinyAVR! It works instead like DxCore's does (i.e., it's useless).  Unfortunately, it is much less useful than the tinyAVR one. The register is limited to -32 to +31, corresponding to - and + around 16%. +/-16% doesn't exactly open up new frontiers.
If an external 32.768 MHz crystal has been connected, you can, for improved temperature stability, autotune the internal oscillator from this. I had to direct a torch at the chip (from a distance) to heat it up enough to see the speed autotune....

Tuning to achieve non-standard speeds is not supported on the AVR Ex-series parts under DxCore.

## Supported Clock Speeds
Like classic AVRs, tinyAVRs have a "speed grades" depending on the voltage and operating conditions that they are rated for operation within. The spec is 5 MHz @ 1.8V , 10 MHz @ 2.7V (3.3V nominal) and 20 @ 4.5V (5.0V nominal) (2.7 or 4.5 for 8MHz and 16 MHz at >105C . See the Speed Grade reference for more information on this. Note that the speed grades are extremely conservative for room temperature operation, and unreal overclocking is easily achievable at room temperaturee, particularly with high temp. rated parts.

The AVR Dx-series come in I (105C) and E (125C) spec parts. Since the DA-series was released, Microchip has STOPPED MARKING THE SPEED GRADE ON THE CHIPS. The marking seems to be absent from 24-pin tinyAVRs, all 2-series tinyAVRs and all Dx and Ex-series parts. Be sure that if you have both speed grades, you mark the chips somehow (0 and 1-series parts do still have the temperature rating marked (N = 105, F = 125) on most but not all packages.

Some of the listed speeds, while supported by the hardware are not supported by the core - typically weird, slow clocks. Crystals in particular can be made in any speed - though classic AVR rewarded the use of bizarre clocks by requiring them in order to generate UART baud clocks for normal speeds, modern AVRs do not need them, and so we don't support any oscillator frequency which is not an integer multiple of 1 MHz
For unsupported speeds, the micros and delay-us columns indicate what internal plumbing has been implemented. micros is implemented for almost all speeds, delayMicroseconds with non-compile-time-known delays for most, even some unsupported ones. delayMicroseconds() is supported and accurate at any speed when the argument is a compile-time-known constant, as we use the avr-libc implementation of `_delay_us()`.

| Clock Speed | Within Spec |      Internal |  Ext. Crystal |    Ext. Clock | micros | delay-us | Notes
|-------------|-------------|---------------|---------------|---------------|--------|----------|-------
|       1 MHz |         Yes |           Yes |            ** |            ** |    Yes |      Yes | 1
| *     2 MHz |         Yes |            No |            ** |            ** |    Yes |      Yes | 2, 9
| *     3 MHz |         Yes |            No |            ** |            ** |    Yes |       No | 2, 9
|       4 MHz |         Yes |           Yes |            ** |            ** |    Yes |      Yes | 3, 10
| *     5 MHz |         Yes |     Prescaled |            ** |            ** |    Yes |      Yes | 10
| *     6 MHz |         Yes |            No |            ** |            ** |    Yes |       No | 2, 4, 9
| *     7 MHz |         Yes |            No |            ** |            ** |    Yes |       No | 2, 4, 9
|       8 MHz |         Yes |           Yes | DD, DB, DU, EA|           Yes |    Yes |      Yes | 10
|      10 MHz |         Yes |     Prescaled | DD, DB, DU, EA|           Yes |    Yes |      Yes | 10
|      12 MHz |         Yes |  mTC tune/Yes | DD, DB, DU, EA|           Yes |    Yes |      Yes | 11
| *    14 MHz | Potentially | mTC tune only |            ** |            ** |    Yes |       No | 2, 4
|      16 MHz |         Yes |           Yes | DD, DB, DU, EA|           Yes |    Yes |      Yes |
|      20 MHz |         Yes |           Yes | DD, DB, DU, EA|           Yes |    Yes |      Yes |
|      24 MHz |     Dx Only |  mTC tune/Yes | DD, DB, DU, EA|           Yes |    Yes |      Yes |
|      25 MHz |          No | mTC tune only | DD, DB, DU, EA|           Yes |    Yes |       No | 5, 6
|      28 MHz |          No |           Yes | DD, DB, DU, EA|           Yes |    Yes |      Yes | 5
|      30 MHz |          No | mTC tune only | DD, DB, DU, EA|           Yes |    Yes |      Yes | 5, 7
|      32 MHz |          No |  mTC tune/Yes | DD, DB, DU, EA|           Yes |    Yes |      Yes | 5
|      36 MHz |          No |            No |    DD, DB, DU |       Dx-only |    Yes |      Yes | 8
|      40 MHz |          No |            No |    DD, DB, DU |       Dx-only |    Yes |      Yes | 8
| *    44 MHz |          No |            No |    DD, DB, DU |       Dx-only |    Yes |      Yes | 2, 8
|      48 MHz |          No |            No |    DD, DB, DU |       Dx-only |    Yes |      Yes | 8

Notes:
* Only the DB, DU, DD, and EA-seris thus far support crystals. EB will not.
* tinyAVRs crap out much above 32 MHz at best, so for tinyAVRs and the tinyAVR-derived Ex-series, that is the top speed supported by the core. The oscillators on 2-series would go to 36 or so at the maximum - but the chip stops executing correctly before then.

`*` This speed is not exposed and is unsupported, but the mechanics to make it work are believed to be there

`**` This option is theoretically possible but is not exposed due to lack of user interest
1. 1 MHz is often used for low power applications, and so is supported with the internal oscillator only. It is not supported with other clock sources. Crystal uses more power, and so doesn't make sense, and external clocks seem to be power hogs. Also refer to the Idd vs Vdd characteristics graphs: If you are CPU-bound, since a first order approximation of power vs frequency is a * F_CPU + b, where b is larger than sleeping power consumption
2. Unsupported because of low demand, not technical obstacle.
3. 4 MHz from internal is always the clock source at startup. Optiboot always runs at this speed.
4. This is not natively supported by the internal oscillator but could be generated by prescaling the internal oscillator at 12 or 28 MHz. Other possible frequencies that can be created through prescaling are not shown, only integer number of MHz is included in table.
5. This is an overclock that is likely to work on all parts as long as it is run at room temperature.
6. delayMicroseconds uses 24 MHz implementation.
7. 30 MHz is notable because it is the highest overclocked option available for tinyAVR 0/1-series running from tuned internal oscillator.
8. This is an aggressive overclock, and cannot be expected to work on all parts. Best results can be had by using E-spec (extended temperature range) parts, and higher speeds will always be achieved with an external clock.
9. This is still a synthesizable frequency for internal with prescaling on Dx or prescaling and/or tuning on mTC though the cores do not expose this
10. Exact Mechanism of generation as internal clock (be it with tuning and prescaling on mTC or prescaling or unusual clock speed options on Dx) varies.
11. 12 MHz is generated from direct tuning of the 16 MHz oscillator on the 1-series, or prescaling the 20 MHz oscillator tuned up to 24 MHz on the 2-series.

There are multiple ways to generate some of the lower frequencies from internal oscillator (do you prescale from higher frequency, or set the oscillator to the desired one? Suspect the latter is more power efficient, but with the former you could still use the PLL while staying in spec - (in my tests the PLL worked well beyond the spec in both directions, at least at room temperature, not that you'd want to do that in production) - currently, we set the main oscillator to the desired frequency, however we may revisit this decision in the future

The DA-series, like the tinyAVRs, does not support use of an external high frequency crystal, only external clock or the internal oscillator can be used.

On all of these parts, the internal oscillator is pretty accurate, so internal clock will work fine for UART communication (they're within a fraction of a percent at room temp) with very little voltage dependence (they have an internal regulator to generate the core voltage, and it is in that low voltage domain that the oscillator is located.

## Auto-tuning
All Dx and Ex parts can - if needed - use an external watch crystal to automatically tune the internal oscillator frequency, a feature called Auto-Tune. Though they specify +/- 3% internal oscillator speed, in practice, I have yet to find one that was off by more than 1 calibration "notch" at room temperature - the accuracy is limited by the granularity of tuning more than anything else. These are just in a different universe than the classic AVRs where a couple percent was normal. I had to use a torch aimed at the chip (obviously (I hope) from a distance) to swing the temperature enough that autotune had to correct the frequency on the fly (resting the soldering iron on top of the chip didn't). The modern AVR internal oscillator is really good. Nonetheless, we provide a wrapper around enabling external 32K crystal and enabling/disabling Auto-Tune in [the DxCore library](../libraries/DxCore/README.md).

```c
#include <DxCore.h>

void setup() {
  //optionally call configXOSC32K() to get specific crystal settings; otherwise it uses conservative defaults.
  enableAutoTune(); //that easy - this returns 0 on success, you can check that if you want to be particularly
  // careful about whether or not it worked it can not-work if the crystal doesn't actually start
  // oscillating, either due to design flaws  inappropriate loading caps, improper crystal selection,
  // or poor layout, or for other reasons such as damage, extreme temperature, and so on.

  // more stuff after this to set up your sketch
}

```

## Overclocking
The capacity of these parts to run at speeds in excess of their rated maximum boggles the mind. To my knowledge, the maximum is around 48 MHz with an external clock. It didn't work on an I-spec part, but seems to work with E-spec parts (most of them anyway). E-spec always overclocks better than I-spec. There is no known voltage dependence related to overclocking - the DB-series power consumption errata implies that if there is, it only manifests low voltages, namely below 2.1 volts, when that power consumption errata manifests (likely a result of the regulator getting into a bad state = these parts have an internal regulator to generate a core voltage in the neighborhood of 1.8v)

Running at 48 MHz is an ambitious overclock and is totally unnecessary - so obviously this is a very exciting chance to do something for that fundamental instintive reason: "Because I can"! So can you. In fact, there's almost nothing to it - just put on a faster crystal, and run a sketch that keeps the part contiually active and printing material that can easily be checked for errors, particularly those that turn 1's into 0's. So far, I had success at room temperature with both external clocks and external crystals, but only when using the E-spec (extended temperature range) parts (which makes sense). Even most I-spec parts will run at 40 MHz - though not quite all: I've found that something under 10% of parts doesn't work at 40. As usual with AVRs, it's the ALU that starts failing first. I have an I-spec that, at 48 external clock will run a program that does a 1 second delay between calling micros (so lots of math is being done, then prints it). Like with over-overclocked tinies, if it's not so high that they crash immediately, they start getting math wrong, which shows up as 0's being printed for micros/millis(). This is unstable, as eventually a return address will get broken, it will return to that, and everything will fail (this may now turn into a reset if you're properly handling reset flags)

These can be returned to normal operation by uploading code that doesn't try to use an excessively fast clock - since the heat generated by the CPU core is negligible even at double its rated clock speed, the damage that a desktop computer CPU might incur when overclocked too hard is not an issue.


## megaTinyCore

## Strange possibilities for divided clocks
There are a *lot* of strange clock speeds possible through combinations of prescalers and the internal oscillator - ever wanted to run an MCU at 7 MHz? *Me neither*, but you totally can, even without a crystal... These exotic speeds are not currently supported by DxCore. The switching logic to get that clock speed from the internal oscillator is implemented for the integer MHz speeds that can be achieved in that way. Fractional speeds are not implemented at all and total timekeeping failure should be expected. I'd be lying if I said I missed the struggle to make millis and micros accurate with weirdo clock speeds back on ATTinyCore (which in turn was done to support UART crystals, because some people are very picky about baud rate accuracy. You may laugh at their posts about making sure the clock is perfect for serial - but if you're not careful, on classic AVRs, the granularity of USART baud rates was horrendous near the higher speeds!) See also the [serial reference](Ref_Serial.md)
Like classic AVRs, tinyAVRs have a "speed grades" depending on the voltage and operating conditions that they are rated for operation within. The spec is 5 MHz @ 1.8V , 10 MHz @ 2.7V (3.3V nominal) and 20 @ 4.5V (5.0V nominal) (2.7 or 4.5

Note though, that the speeds that we have switching logic for, if specified, would have working millis, but micros **and delay** would not, because delay relies on micros if it thinks it has it. A test could be added to the #if for delay to exclude speeds to get it working at that speed

## Fuses do not control the clock speed on Dx
On Ex-series and tinyAVR parts, the internal oscillator is derived from either 16 or 20 MHz, and starts up prescaled by 6. Dx-series parts start up at 4 MHz with no prescaling. Regardless, the core configures that on startup in `init_clock()` (weakly defined so you can override it if need be). If it's external, we tell it to switch the clock and then poll the status to wait for it to pick up, if we wait around 1ms and still have no clock, we presume the clock non-functional and call `onClockTimeout()` which triggers the blink code. This is weakly defined function so it can also be overridden.

The Optiboot bootloader never changes the clock speed; it always runs at the startup speed - the bottleneck is not processing speed, it's the serial baud rate maximum imposed by Seriu.  This means that even if you set it to use a clock source that doesn't exist, or if an overclock the chip can't handle is selected, the bootloader will still run normally at startup, and an external reset will still reset the chip so the bootloader runs.

As a result of these factors, you never need to burn bootloader to configure the clock! If you are using an external clock source and timekeeping + serial come out wrong (eg, blink blinks at the wrong speed, Serial outputs wrong-clock gibberish), that means that the clock or crystal is not running at the speed you told the core it is. While there are ways we could detect this, I cannot justify the added bulk, time and complexity. Almost all crystals have the frequency printed on them (though it may be hard to read), and the ones that don't - if they're pulls, don't use them, and if you bought them new, you know the specs - even if you ordered multiple speeds that weren't marked, and then unpacked them into unmarked containers (tsk tsk), you can figure out which one you actually used by uploading a sketch that prints something to serial and trying the speeds you bought until one works.

## Rules for PCB layout around crystals
There are only two places where someone with an AVR is likely to routinely encounter layout sensitive behavior. Crystals and clocks are one of them (the other is decoupling caps, which don't do their job unless they are right next to the pins they are associated with):

### Crystals
* The crystal and capacitors should be on the same side of the board as the chip.
* The crystal should be as close to the chip as possible, as should the caps.
* Don't route anything right underneath the crystal, and if you must do so, it should not have any fast changing signals or high frequency noise that could confuse the part.
  a. that assumes a two layer board. if your board has more layers and a ground plane underneath it, you don't need to be as careful.
* Nothing else should be connected to lines between crystal, caps and chips. They should be as short as possible with nothing else on them, including traces connected to nothing. This is why, on the breakout boards I sell, if not using a crystal, you need to connect a solder bridge jumper to tie the crystal pin to the holes along the edge of the board. If they were connected, it'd never oscillate.
* The total capacitance "seen" by each crystal pin must be as close to the load capacitance as possible. The capacitance comes from more than just the capacitors placed on the board: C<sub>load</sub> = C<sub>pin</sub> + C<sub>stray</sub> + C<sub>cap</sub>.
  * C<sub>cap</sub> is the value of the loading capacitors, individually. Notice from the above equation that this must be lower than the crystal's C<sub>load</sub>.
  * C<sub>pin</sub> is the capacitance of the pin. It can be found in the electrical characteristics chapter of the datasheet, "DC Characteristics" -> "I/O pin  Characteristics". For example on the DB-series, it is given as 4 pf.
  * C<sub>stray</sub> is the parasitic capacitance between the trace and the rest of the board. You can neither calculate this it depends on almost every physical parameter of trace, even if you knew thopse parameters, it would be a fiendishly difficult calculation. It is also very difficult to measure. Frequently, it is instead estimated. A value of 2-3pF is generally assumed. Thankfully, capacitance of loading caps does not need to be dead on accurate - small inaccuracies can be tolerated, with a small impact on frequency ("frequency pulling" when done intentionally). Large inaccuracies, however, will keep the crystal from working at all.
  * C<sub>load</sub> is spec'ed by the crystal and is rarely outside the range of 8pF to 22pF, with 18pF being very common, 12pF is also fairly common, while 22 pf is not uncommon on through-hole crystals. 8pF is the lowest plausible load capacitance that should be considered when selecting a crystal; 18pF is my suggestion.
  * Watch out when buying low cost crystals from China. It's easy to see that they come in the right package size, the frequency you need, and they're cheap as dirt (there's usually a large "China discount" on crystals, particularly the small SMD ones, which are markedly more expensive from Western suppliers), then realize the load capacitance wasn't listed, nor was the part number, that you can't find the part number by looking up the the numbers printed on the crystal, and that when asked, the seller in broken english says that they don't know (this in turn implies that those crystals aren't even coming from reels in someone's basement - because those would have the part number and likely the load capacitance marked)
* Breadboards will usually supply too much stray capacitance. If they work at all, it will likely be with no loading caps at all. Breadboard is kryptonite for high frequency.
  * The same goes for prototyping board, though it is not quite as bad. On my upcoming wide DIP protoboard, an easy place to cut the trace (like the normally-connected solderbridge jumpers on other boards) is proivided that, for every pin, you can isolate its hole and the nearest connected hole from the rest of them, and the copper around the rim of the hole is thinner too; this helps. You will likely want smaller caps than usual, too. On boards for specific chips which support a crystal (currently the DA/DB/DD DIY prototyping board), PA0, PA1 and PF0, PF1 that is present, and it is NOT connected by default, on the grounds that it's easier to bridge them then to cute a trace, even one meant to be cut. For other protoboard, you use the highest load capaccitance crystal that is within spec and hope that with no loading caps or small ones. With 18pF caps, you can usually make it work with very small caps.
* The two traces going to the crystal should be similar in length. They don't need to be exact, but they should not be radically different
* SMD caps are preferable to through-hole ones for loading capacitors. If you do use through hole ones, put the leads in all the way and trim the excess off the rear of the board.
* Touching the crystal, caps, or traces may cause it to stop working. This is most common when the loading caps are barely close enough to work.
* It is easier to get these parts to work with too little loading capacitance than with too much.


### External clocks
Thankfully, these are simpler: Just keep the high frequencty trace short, and make sure that you provide the decoupling capacitor the datasheet asks for. Most often I see 0.01uF specified for external clocks, rather than the more common 0.1uF . There's usually a pin, pulled up internally, that will disable it when brought low. Make sure you don't connect that to ground - usually it will be pulled up if allowed to float. Be careful about the orientation, too - these are rectangular, and can be rotated 180 degrees with all the pads lining up. Doing that will swap Vcc and Ground. Like most ICs, that reversed polarity configuration will destroy them almost instantly, and present a near-short to the power supply.

So with the added simplicity, one might wonder why they are so uncommon. There are several reasons:
1. They are power hogs. 10-25 mA is normal, 50mA is not unheardof.
2. They are picky about voltage. There was a line of oscillators in 5032 package in production until mid 2020 which worked at 1.8-5v. They were discontinued, and there is no other such oscillator available as far as I can tell.
  a. The 5v ones specify minimum 4.5v maximum 5.5v.
  b. 1.6-3.6v ones are available, so the lower end of the range is covered. None of the major vendors with parametric search on their catalog indicate that any which are in spec operating between 3.6v and 4.5V, so one cannot run directly from a LiPo battery with one while remaining within spec.
3. 5v units are sometimes hard to find in smaller package sizes, 5032 is exotic for 5v oscillators, and anything smaller nonexistent.
4. They are strangely expensive, typically $1 or more from western suppliers. There is hardly any "China discount", either. Like crystals, however, the oscillators advertised on Aliexpress don't list essential specs like the voltage or part number, and sellers don't seem to know when asked.
  a. One gets the impression that external oscillators are a specialty item for precision applications, while crystals are not. They are often designed to higher accuracy and they don't depend on external components that could "pull" the frequency, and so on. In a typical application, if you don't need a precision clock source, there are other ways to get it.

## Clock troubleshooting
On a classic AVR, the result of selecting a clock source (external crystal or clock) which does not function is not subtle: You burn the bootloader to set the fuse to use that clock source, and the chip ceases to respond, including all attempts to program, other than HJV programming. Fortunately, this is a thing of the past with modern AVRs, it comes for free with the new clock handling - it starts up at a universally acceptable speed and can be easily switched at runtime. Recovery is typically as simple as uploading a new sketch with more appropriate clock settings.

On megaTinyCore, troubleshooting is generally straightforward.


### Blink Codes (DxCore only)
All blink codes issued by the core start with the LED pin switching states three times (ie, if it is off, it will turn on, off, then on again), followed by a number of flashes indicating the nature of the failure. This will then repeat - since it initially changes state three times, this means that the pattern will be inverted the second time through. The number of short flashes indicates the nature of the problem: Three flashes indicates that the selected clock source was not present at startup. Four flashes indicates that it was present at startup, but then disappeared while it was running. It is hoped that this will make the blink codes distinctive enough that they cannot be mistaken for the running sketch. You can override the `onClockFailure()` and `onClockTimeout()` functions to run different code. The clock failure detection seems flaky, and often it just resets instead - I think the reason for this is that CFD only detects when the clock is not present. But if it is transitioning at an intermittent frequency, it may be enough to fool CFD, while not providing a clock of sufficient quality for operation. When onClockTimeout() is called, it will be just after startup running at 4 MHz; scale any baud rates by (baud rate compiled for)/(4 MHz) to calculate the baud rate to ask it for if you want to output debug messages. Overriding those functions may be most important if PA7 needs to be repurposed, and you can't have it taking over that pin no matter what.

For example, if the crystal (or clock) speed is one that can also be generated from the internal oscillator, you might want to resort to that and run the app - albeit with less accurate timing - in spite of the problem with the crystal - One approach is shown below.
```c++
byte clockfallback = 0; // global scope

void onClockTimeout() {
  // Uh oh, the external crystal or clock isn't oscillating.
  clockfallback = 1; // Application can check this to produce an appropriate warning
  _switchInternalToF_CPU(); // Set internal HF oscillator to correct source
  PROTECTED_WRITE(CLKCTRL_MCLKCTRLA, CLKCTRL_CLKSEL_OSCHF_gc); // set the main clock source back to internal, so it's not in the state where it's standing by to switch over to the (obviously dodgy) external one if that suddenly works.
}

void setup() {
  Serial.begin(115200);
  if (clockfallback) {
    Serial.print("ERROR: external clock or crystal not present or not functional. Falling back to the internal oscillator at ");
    Serial.print(F_CPU/1000000);
    Serial.println(" MHz");
    Serial.println("Timekeeping of all sorts will be less accurate.")
  } else {
    Serial.println("Startup OK");
  }

}

```

`_switchInternalToF_CPU()` is a macro provided essentially for this and only this purpose (I can't think of any other use cases...).

An analogous situation in the event of clock failure is probably not a good idea, because as noted above *the clock failure detection interrupt hardly ever triggers* in my experience, even when I, say, cause the crystal to stop oscillating - for example clipping a long wire to one side, touching one side or another of the crystal, and sometimes even touching the case of the crystal (that's one of the reasons the case of a crystal is sometimes grounded) - a human body, remember, is a 6-foot long, non-optimally-shaped, untuned, low quality broadband antenna. In these cases, the failure rarely results in the CFD interrupt being called; it just resets and/or hangs. Instead, I would suggest you just reset if you want to fallback to internal - use the above code to detect the clock timeout at startup, and override onClockFailure to simply reset, where presumably the startup would find that it didn't have a clock and set internal clock correctly:

```c++
void onClockFailure() {
  _PROTECTED_WRITE(RSTCTRL.SWRR,1); //issue a software reset.
}
```

### Failure modes
A number of behaviors have been observed when the clock has been configured wrong, is not functioning, or that particcular specimen can't do the overclocking you are asking of it.

1. If there is a crystal/oscillator which does not oscillate at all, you will get the blink code On-Off-On, then 3 brief "antiblinks" where the led turns off momentarily, then Off-On-Off and 3 brief flashes, repeating. (this pattern of alternating inverted output is meant to be unlikely to mimic sketch behavior).
2. If there is a crystal, and is mostly non-functional, but is close enough to working that the clock controller thinks it is OK while it's not actually working, it will typically hang or bootloop. This is commonly seen when inappropriate loading capacitors are used, as it can fool the CFD.
3. If the clock source fails while the system is running, a DA-series will hang (until something resets it - the WDT, the reset button, or powecycling are the only ways to get it out of this state.
4. A clock failure while running very often resets the chip (likely a [dirty reset](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Reset.md#the-danger-of-dirty-resets), caused by incorrect execution of instructions when the variation between clock periods is far in excess of the manufacturer spec and it fumbles math with the program counter.)
5. A clock source that resets the chip or hangs immediately upon switching to it is much easier to detect if using Optiboot - however, you could blink an LED before the switch to see if it's hanging or bootlooping by overriding onPreMain():
```c++
#include <util/delay.h> -  there is no timekeeping in onPreMain

void onPreMain() {
  openDrainFast(LED_BUILTIN,LOW); //As discussed in digital I/O reference, this is currently also the cheap trick to change direction with fastDigitalIO.
  _delay_ms(500*4/24); // F_CPU is #defined globally, but init is the first part of main. This is running before that so you need to use the builtin avr delay functions.
  digitalWrite(LED_BUILTIN, CHANGE); // now it's high
  _delay_ms(500*4/24); // F_CPU is #defined globally, but init is the first part of main. This is running before that so you need to use the builtin avr delay functions.
  digitalWrite(LED_BUILTIN, CHANGE); // now it's low
  _delay_ms(500*4/24); // F_CPU is #defined globally, but init is the first part of main. This is running before that so you need to use the builtin avr delay functions.
  digitalWrite(LED_BUILTIN, CHANGE); // now it's high again. Allow startup to proceed, leaving he LED on to detect resetting into a bad state as opposed to hanging in a bad state.
}
```

Problems with inappropriate loading caps, cold solder joints (to not only the two clock pins, but (if present) the ground pins on the crystal as well) can lead to problems 2 or 3 above occur when you touch the crystal housing. That is a sign that the crystal is barely working under normal conditions, and either the force is disconnecting a pad that isn't properly soldered, or the caps are wrong or the layout to terrible, such that you introduce enough electrical noise to kill the whole thing, just by touching what should be a grounded crystal housing (ideally - not the case for HC/49s nor for garbage cans (aka cylindrical - I personally like them, but it amuses me to compare them to a garbage can. HC/49 package is the one I can't stand - it takes up so much board space, and it has to have short traces and nothing running under it and the package has a whole bunch of other pins on it that I want to connect to stuff. I much prefer a tiny garbage can cap. You can usually put them on top of the chip and hot-glue them in place to keep them from breaking during handling. Check the loading caps, check the solder joints. If you're sure the solder joints are good, and you're sure the loading caps are appropriate, change the loading caps anyway, maybe for a slightly higher or lower value. The crystal drive circuit is weak on these parts, probably due to power considerations, and fiendishly sensitive to everything.

Without a FET probe, I don't think I have much hope of understanding this.

## Solving clock problems
First, if the sketch doesn't seem to run at all, switch to internal clock not more than 24 MHz and make sure you're not just trying to overclock it too hard. If that doesn't do anything either it points to your sketch, and/or bad connections to external devices causing a hang during initialization (this is what the majority of hangs are with the dirty reset protection that we have anyway. (Without it, it was fairly easy to get it to a state where it needed a reset pin reset or power cycle to revive through bad code. Now it will generally just bootloop at worst, and it's straightforward to probe them by writing to a pin with an LED on it and moving that around to find where it's getting stuck, if you keep it on github, compare to older versions, etc - you introduced a bug around the same time you decided to try the crystal, or have a loose wire (don't underestimate this if you wait on external devices when starting up. Especially if you wired it up using "dupont line" - the Chinese dupont line, normally made from ribbon cable wire (generally several AWG smaller than the claimed thickness, if spec'ed), uses an inferior clone not of the DuPont connector, but of a western knockoff; that is where most of the innovation (cost savings, coupled ) . (Without crimping your own, I've gotten best results from buying chinese "terminal line" with the terminals but no housings, and a bunch of housings - they are usually made with less-bad terminals,  and avoid the ribbon cable dupont like like the plague). If it does work when you go back to internal oscillator, disconnect power and inspect it carefully for dubious solder joints, especially around the crystal or clock source.

If you have an oscilloscope, or even a "logic probe" that will tell you if there are frequent transitions on a pin, connect it to PA7, and override onPreMain() with:
```c++
void onPreMain() {
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLA,0x80);
}

```
That will cause PA7 (can't be changed) to output the system clock, and the startup routines do not change the high bit of that register, hence, the clock will continue to be output. If you see the app hung, but the clock output is coming out, that implies that it's not caused by a problem with the clock itself. This does not rule out an excessive oveclock (the AVR128DB32 that doesn't run at 40 does this) - the part could have a stable clock that's just too fast for the ALU, causing the ALU to fail to execute instructions often enough that nothing works.

If you are unable to upload new code, even if the last thing you did was to change the clock, the problem is unlikely anything to do with the clock. A reset should always run the bootloader before switching to whatever other clock source doesn't work - and UPDI never relies on an external oscillator so should always work, unless you have turned the UPDI pin into a RESET pin. There are known to be a few ways that

### Crystals
Frequency crystals are fickle beasts. They all come spec'ed with a load capacitance, (well, unless you buy from halfasssed sellers on aliexpress, in which case the manufacturer most certainly did specify it, even if the vendor doesn't know what it is. It's very common to see crystals described as having the "standard" load capacitance. The crystal and two loading caps should be located on the same side of the PCB as the chip, away from sources of noise or interference (read: surrounded by ground pour and nothng likeky to cause interference on the other side of the board). Caps far from the correct values will result in no oscillation at all, as will a failed crystal (it happens).
My protocol when I encounter this is:
1. Unplug board, multimeter in continuity mode
  a. Verify no short between ground and either crystal pin (if you can't see where the short is, it's either under the crystal or under one of the loading caps.
  b. Verify the two pins are not shorted to each other.
  c. Verify that neither of the pins are shorted to any nearby pins or traces.
2. If it never worked:
  a. squint at the loading capacitors, and compare them to the ones you were supposed to use. Do they look like the same capacitors? For SMT capacitors, compare both the color of the ceramic and the shape of the terminations (which varies by manufacturer - some have a perfectly straight boundary between metalized termination and ceramic, on others, it is bowed slightly. For through-hole ones, double check the value; use an online capacitor value calculator if you aren't confident in interpreting the 3-digit codes for capacitance that are often seen here. You may have used the wrong capacitors, or you may have been pulling the capacitors from one right bin - it only takes one 0.1uF cap falling into the decoupling cap bin - which was contaminated with a few of a different value (I usually don't try to save spilled SMD capacitors specifically for this reason).
  b. Double check your calculations of the appropriate load capacitance. If you're not confident that they're approximately correct, remove the two capacitors and try again (with no caps - amazingly this often *just barely* works, though it will usually crash if you put your hand near it. If that works, your original loading capacitor value was too large, likely because you underestimated stray capacitance. Use a capacitor larger than 0pF but smaller than the one you had.
  c. Your crystal may just be bad.
3. if it previously worked
  a. If SMD caps/crystal, put some flux on the terminations of the cap and crystal, and briefly touch a soldering iron tip to each connection. (alternate capacitors to give the end you reflowed a chance to cool. Do the same for the crystal.
  b. Failing that, replace the crystal.
4. Make sure when soldering that you do not accidentally short Vcc, enable, or the output to the case of the crystal - it can be difficult to clear bridges occurring between parts of the crystal package - try to be neat while soldering those small flat rectangular crystals.

### External clocks
There are two common failure modes that can be seen here. Either: it marks itself as due to switch clock to external. It then polls the clock status to see when the oscillator switch has occurred successfully. That should be seen within microseconds and after some milliseconds times out (and should do the blink code). Alternately, it could transition to a poor
Unlike crystals, you can actually put a 'scope probe onto the output of a the oscillator and see the signal. Do that. Do you see the signal?
1. If not - examine the datasheet and try to figure out why it might not be oscillating.
  a. You didn't install it rotated 180 degrees did you? the rectangular oscillators usually have Vcc and ground opposite each other, so everything will be lined up and can be soldered down with the oscillator connected backwards. *This will irrevocably destroy it the moment power is applied*. You can easily determine if this has happened - the external clock will be hot, and you may notice signs of the board consuming more current than expected (like power LEDs on a serial adapter supplying power being dimmer when connected to the board).
  b. Verify that it is getting power and ground connected to the proper pins. (on my breakout boards which support an external clock, you may need to change the connections of some solder bridges - if the board can take either a crystal or a clock in one of those rectangular packages, there must be a solder bridge jumper somewhere to configure whether the two corners that are neither the data output nor the enable pin are both Gnd (as is the case for a crystal) or if one needs to be grounded and the other at the supply voltage.
  c. Most have a pin that can be pulled low to turn off the output to save power. If allowed to float, they generally have internal pullups and can be left alone, but putting a 10k pullup to Vcc is an easy test to verify. Is yours shorted to ground by any chance?
  d. These arent't crystals, but they can't drive an arbitrarily long wire or work into a large load.
2. If you do see a signal, and it is no higher than the chip's rated speed (i.e., there's no chance it's actually too fast for the chip), make sure you see the same signal on the pin of the chip (cold solder joints happen - I've seen parts with a pin that wasn't actually soldered in place, but which looked like it was, and when pressure was applied to the pin with the scope probe - it started working; this is much more frustrating when you're trying to debug it with the power off with a DMM, and you check every connection, yup, continuity.... but when you actually apply power, it's not making contact, because the pressure you were exerting with the probe was what held the pins in contact. This is of course  basically impossible to diagnose with power off (since you are never sure if the poor contact is just between your probe and the pin, or between the pin and pad); it becomes much easier to diagnose while running, since prodding the pin will suddenly bring the device back to life.
