# Detailed Clock Reference
This section seeks to cover information about the clocking options on the modern AVR devices.

The modern AVRs can broadly be divided into two groups based on their


**Crystal** is not supported as a clock source on the tinyAVR, DA-series, or EB-series, but is on the DB, DD, EA, and DU-series.

## Background - how the clock is generated
See also [the timer reference](Ref_Timers.md)

Heading        | TinyAVR0/1 | TinyAVR1+/2 | Mega0 | DA | DB/DD/DU | EA | EB  |
---------------|------------|-------------|-------|----|----------|----|-----|
OSCHF, nbr opt.| 2*         | 2*          | 2*    |    |          | 2* | 2*  |
OSCHF, Dx-type | -          | -           | -     | Y  | Yes      | -  | -   |
External Xtal  | -          | -           | -     | -  | Yes      | Y  | -   |
External Clock | Yes        | Yes         | Yes   | Y  | Yes      | Y  | Y   |
PLL Max freq   | -          | -           | -     | 48 | 48       | -  | 80  |
PLL Mult opt.  | -          | -           | -     | 2/3| 2/3 ***  | -  |8/16 |
PLL for TCD    | -          | -           | -     | Y  | Yes      | -  | -   |
PLL clock Event| -          | -           | -     | -  | -        | -  | Y   |
PLL for HiRES  | -          | -           | -     | -  | -        | -  | Y   |
PLL for TCF    | -          | -           | -     | -  | -        | -  | Y   |
PLL for CPU    | -          | -           | -     | -  | -        | -  | Y** |
Ext 32k xtal   | No (0)/Yes | Yes         | -     | Y  | Yes      | Y  | Y   |


`*` Marked parts have selectabkle 16 or 20 MHz, controlled by a *fuse*. These parts are also impacted by speed grades (eg, a graph of minimum voltage versus F_CPU is included in the datasheet). For example, many have three speed grades, typically 4 or 5 MHz @ 1.8V, 8 or 10 MHz at 2.7, and

`**` - PLL must be run no lower than at 8x 2.5 = 20 MHz, thus the chip can just barely be in spec feeding the PLL an external 2.5 MHz clock, Normally when you run chip off PLL, you run it at 4s F_CPU and use the PRESCB - to get it back down to 20 MHz while preserving the 80 MHz clock to ise for HiRes.

`***` - The DU PLL is internal and controlled by the hardware for the USB (it's presence is not explicitly noted that I could see, but it could be deduced by it being a USB 2.0 compliant device. that would need a 48 MHz clock, So while TCD left, I suspect the PLL just went internal. The other three Dx's have a rather boring PLL that can only drive the TCD. If you set the register bitfield to the reserved 11 value, you get a 4x multiplier. The PLL works for me at a much wider range of input frequencies than they say too (applies to DA as well (128 MHz seemed okay, but 160 MH it started glitching hard.)

## Supported Clock Speeds and Speed Grades
Like classic AVRs, tinyAVRs and Ex-series parts have a "speed grades" depending on the voltage and operating conditions that they are rated for operation within. The spec is

* 5 MHz @ 1.8V up to 85C
* 10 MHz @ 2.7V (3.3V nominal) up to 85C - 8 MHz @ 2.7V and 105 or 125C for extended temperature range parts.
* 20 @ 4.5V (5.0V nominal) up to 85C - Or 16 @ 4.5V  and 105 or 125C for extended temperature range parts.

Users of megaTinyCore can refer to their speed grade reference in the documentation. However, as speed grades are relatively new for DxCore, being reintroduced only on the Ex-series, we have not written the Ex-series document yet.

AVR-Dx-series parts eliminate the matter of speed grades by eliminating the dependence of the the CPU core and indeed most peripherals on this

In all cases where attempts at overclocking have been made, the maximum speed specs have been foundto be extremely conservative, as you would expect for devices aimed at life-safety-critical devices (and apparently Microchip sees a market for even more "safe" chips) speed grades are extremely conservative for room temperature operation. The speed grades are indicated using the old Atmel FUN system. U is 85C, N is 105C, and F is 125C. Additionally, on 1-series parts, the temperature range portion of the part number is marked on the chip. **2-series part temperature grades can be determined only from purchase records or by contacting Microchip suoport with the lot number**



The AVR Dx-series come in I (105C) and E (125C) spec parts. **This is not marked on the chips**. Be sure that if you have both speed grades, you mark the chips somehow.

Some of the listed speeds, while supported by the hardware are not supported by the core - typically weird, slow clocks. Crystals in particular can be made in any speed - though classic AVR rewarded the use of bizarre clocks by requiring them in order to generate UART baud clocks for normal speeds, modern AVRs do not need them, and so we don't support any oscillator frequency which is not an integer multiple of 1 MHz
For unsupported speeds, the micros and delay-us columns indicate what internal plumbing has been implemented. micros is implemented for almost all speeds, delayMicroseconds with non-compile-time-known delays for most, even some unsupported ones. delayMicroseconds() is supported and accurate at any speed when the argument is a compile-time-known constant, as we use the avr-libc implementation of `_delay_us()`.

### megaTinyCore (ATtiny 0-Series, 1-series, and 2-series.
| Clock Speed | Within Spec |      Internal |    Ext. Clock | micros | delay-us | Notes
|-------------|-------------|---------------|---------------|--------|----------|-------
|       1 MHz |         Yes |           Yes |            ** |    Yes |      Yes | 1
| *     2 MHz |         Yes |      Possible |            ** |    Yes |      Yes |
| *     3 MHz |Yes and/or no|      Possible |            ** |    Yes |       No | 2
|       4 MHz |         Yes |           Yes |            ** |    Yes |      Yes | 3, 10
|       5 MHz |         Yes |           Yes |            ** |    Yes |      Yes | 10
| *     6 MHz |Yes and/or no|      Possible |            ** |    Yes |       No | 2
| *     7 MHz |  Yes and no |      Possible |            ** |    Yes |       No | 2
|       8 MHz |         Yes |           Yes |           Yes |    Yes |      Yes | 10
|      10 MHz |         Yes |           Yes |           Yes |    Yes |      Yes | 10
|      12 MHz |Yes and/or no|         Tuned |           Yes |    Yes |      Yes | 2
| *    14 MHz |  Yes and no |      Possible |            ** |    Yes |       No | 2
|      16 MHz |         Yes |           Yes |           Yes |    Yes |      Yes |
|      20 MHz |         Yes |           Yes |           Yes |    Yes |      Yes |
|      24 MHz |          No |         Tuned |           Yes |    Yes |      Yes |
|      25 MHz |          No |         Tuned |           Yes |    Yes |       No | 5, 6
|      28 MHz |          No |      Possible |           Yes |    Yes |      Yes | 5
|      30 MHz | No way! *** |         Tuned |           Yes |    Yes |      Yes | 5
|      32 MHz | No way! *** | 2series tuning|           Yes |    Yes |      Yes |

`*` - Option not exposed via tools menu due to lack of interest or particular reason to use these speeds. We don't have to worry about UART bullshit anymore. Let's make life easy on ourselves and not clock our system at 7 MHz, hmm? The plumbing is in `(2^n) x 7 for integer n >= 0` went in so 28 MHz, which can be done with internal on Dx without tuning, and enough code is shared that ensuring that plumbing was present was straightforward.

`**` - While the hardware supports it, and the listed plumbing is present, these options make little sense, and are not exposed.

`***` - It is common to encounter parts which do not work at this speed at room temperature. Extended temperature range parts (F-spec) tend to work better than industrial (U or N-spec. Note that the 2-series has U-spec parts, which are 85C. 0/1-series industrial parts are N-spec and 105C, though the extended temp range version of both ssries is 125C F-spec. *That's one thing I miss about Atmel. Their temperature grading scheme was just more FUN* "It was also objectively better: Microchip doctrine does not have a universal abbreviation for each common temperatiure grade... Not only that, they don't mark the temperature grade on the package, so sleazy assembly houses could substitute cheaper I-specs for the E-specs you paid for, and unless you emailed microchip support with lot numbers, you'll never catch it." *It makes scamming easier? Maybe this new system isn't so bad...*

### DxCore (AVR Dx and EA series)
Note that no decision has been made regarding which clock speeds to expose for the EB at this time, due to it's novel clocking options.

| Clock Speed | Within Spec |       Int. Dx | Int. Ex |  Ext. Crystal |    Ext. Clock | micros | delay-us | Notes
|-------------|-------------|---------------|---------|---------------|---------------|--------|----------|-------
|       1 MHz |         Yes |           Yes |    Yes  |            ** |            ** |    Yes |      Yes | 1
| *     2 MHz |         Yes |           Yes |    Yes  |            ** |            ** |    Yes |      Yes | 9
| *     3 MHz |         Yes |      Possible |    No   |            ** |            ** |    Yes |       No | 9
|       4 MHz |         Yes |           Yes |    Yes  |            ** |            ** |    Yes |      Yes | 10
| *     5 MHz |         Yes |     Prescaled |    No   |            ** |            ** |    Yes |      Yes | 4
| *     6 MHz |         Yes | **  Prescaled |    No   |            ** |            ** |    Yes |       No | 4, 9
| *     7 MHz |         Yes |            No |    No   |            ** |            ** |    Yes |       No | 4, 9
|       8 MHz |         Yes | **  Prescaled |    Yes  | DD, DB, EA    |           Yes |    Yes |      Yes | 10
|      10 MHz |         Yes |     Prescaled |    Yes  | DD, DB, EA    |           Yes |    Yes |      Yes | 10
|      12 MHz |         Yes |           Yes |    No   | DD, DB, DU, EA|           Yes |    Yes |      Yes | 11
| *    14 MHz |         Yes |            No |    No   |            ** |            ** |    Yes |       No |
|      16 MHz |         Yes |           Yes |    Yes  | DD, DB, DU, EA|           Yes |    Yes |      Yes |
|      20 MHz |         Yes |           Yes |    Yes  | DD, DB, DU, EA|           Yes |    Yes |      Yes |
|      24 MHz |     Dx Only |           Yes |    No   | DD, DB, DU, EA|           Yes |    Yes |      Yes |
|      25 MHz |      No *** |** Maybe, hard |    No   | DD, DB, EA,   |           Yes |    Yes |       No | 3, 4
|      28 MHz |      No *** |           Yes |    No   | DD, DB, EA,   |           Yes |    Yes |      Yes | 3
|      30 MHz |      No *** |** Maybe, hard |    No   | DD, DB, EA,   |           Yes |    Yes |      Yes | 3
|      32 MHz |      No *** |           Yes |    No   | DD, DB, EA,   |           Yes |    Yes |      Yes | 3, 5
|      36 MHz | No way **** |            No |    No   |    DD, DB     |       Dx-only |    Yes |      Yes | 8
|      40 MHz | No way **** |            No |    No   |    DD, DB     |       Dx-only |    Yes |      Yes | 8
| *    44 MHz | No way **** |            No |    No   |    DD, DB     |       Dx-only |    Yes |      Yes | 2, 8
|      48 MHz | No way **** |            No |    No   |    DD, DB     |       Dx-only |    Yes |      Yes | 8

`*` These speeds are not provided by the tools menu, but a custom board definition using those speeds should find that they work. This is not tested and fixing it if it doesn't work will be a low priority, as these speeds are largely useless. The power efficiency discussion in note 1 explains why I consider the low speeds useless. 14 MHz is not included because it's a frequency that only a masochist would love, being as it makes math very unpleasant and offers no compelling appeal. 44 MHz is dropped as crystals of that speed are rare, it's a huuuge overclock, AND it's a masochistic frequency. I say things like that thinking from user perspective, not core maintainer perspective. From the perspective of the person writing the timekeeping code (me), the hardest was 6/12/24/48, because those you have to rightshift until you're dividing by 3 - which is maximally hard for ersatz division. It's still faster than real division, but it's the slowest of all the clock speeds to calculate micros for. Binary and 3's don't mix well.

`**` - While the hardware supports it, and the listed plumbing is present, these options make little sense, and are not exposed.

`***` - In Dx-series parts, these modest overclocks succeeded at room temperature on essetially 100% of parts.

`****` - These aggressive overclocks are will not work with all parts. E-spec parts should be used if attempting these sort of clock speeds


1. 1 MHz is often used for low power applications, and so is supported with the internal oscillator only. It is not supported by the tools submenu options - this is intended, even though it is not technically difficult to support. The only justifiable reason for running at such a low speed is if you are desperate for lower power consumption, and the chip can't be put in sleep for most of the time for some reason(if you can, you're better off running it at a higher speed and sleeping more. See Idd vs Vdd charachteristics graphs in datasheet). This is a bizarre corner case to begin with. Using an external crystal will significantly increase power consuption, making it unsuitable for that one plausible use case for 1 MHz operation. While external clock uses less power from the perspective of the chip, you need an external clock, and I have been consistently stunned by how high the current for external clocks is. So the total system power in that case also makes it unsuitable. As these options are not suitable for any application I've been able to think of, creating and maintaining these cores for years.
2. On tinyAVR, achieving this speed from internal oscillator possible with tuning and prescaling. If 16 MHz base oscillator is used, 14 MHz is possible by tuning 16 down to 14, though why you would want to run at such a godawful speed is another matter. But when running from 20 MHz internal, it can't be tuned down far enough, but it can be tuned to 28 MHz and prescaled by 2. Hence in that case it is not within spec. 12 MHz from internal is sometimes achievable on 16 MHz by tuning downwards, but never on 2-series, and rarely on 0/1-seriues. Tuning up to 24 and prescaling works for 12 (and 6) in cases that can't tune down to 12, but of course that exceeds spec.
3. This level of overclocking has rarely or never been observed to misbehave at room temperature and Vdd = 5v.
4. Internal oscillator at this speed is achieved by prescaling
5. 30 MHz is fastest speed we support for tinyAVR 1-series. Parts that work at higher speeds are uncommon (2-series parts, in contrast, often do work at 32)
6.
8. This is an aggressive overclock, and cannot be expected to work on all parts. Best results can be had by using E-spec (extended temperature range) parts, and higher speeds will always be achieved with an external clock.
9. This is still a synthesizable frequency for internal with prescaling on Dx or prescaling and/or tuning on mTC though the cores do not expose this
10. Exact Mechanism of generation as internal clock (be it with tuning and prescaling on mTC or prescaling or unusual clock speed options on Dx) varies.
11. 12 MHz is generated from direct tuning of the 16 MHz oscillator on the 1-series, or prescaling the 20 MHz oscillator tuned up to 24 MHz on the 2-series.

There are multiple ways to generate some of the lower frequencies from internal oscillator (do you prescale from higher frequency, or set the oscillator to the desired one? Suspect the latter is more power efficient, but with the former you could still use the PLL while staying in spec - (in my tests the PLL worked well beyond the spec in both directions, at least at room temperature, not that you'd want to do that in production) - currently, we set the main oscillator to the desired frequency, however we may revisit this decision in the future

The DA-series, like the tinyAVRs, does not support use of an external high frequency crystal, only external clock or the internal oscillator can be used.

On all of these parts, the internal oscillator is pretty accurate, so internal clock will work fine for UART communication (they're within a fraction of a percent at room temp) with very little voltage dependence (they have an internal regulator to generate the core voltage, and it is in that low voltage domain that the oscillator is located.

## Auto-tuning
All Dx and Ex parts can - if needed - use an external watch crystal to automatically tune the internal oscillator frequency, a feature called Auto-Tune. Though they specify +/- 3% internal oscillator speed, in practice, I have yet to find one that was off by more than 1 calibration "notch" at room temperature - the accuracy is limited by the granularity of tuning more than anything else. These are just in a different universe than the classic AVRs where a couple percent was normal. I had to use a torch aimed at the chip (at a distance, obviously) to swing the temperature enough that autotune had to correct the frequency on the fly (resting the soldering iron on top of the chip didn't). The modern AVR internal oscillator is really good. Nonetheless, we provide a wrapper around enabling external 32K crystal and enabling/disabling Auto-Tune in [the DxCore library](../libraries/DxCore/README.md). DU devices will get a function to turn on and of the autotune from USB SOF in that library as more support is sdded for tose parts.

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

Running at 48 MHz is an ambitious overclock and is totally unnecessary - so obviously this is a very exciting chance to do something for that fundamental instintive reason: "Because I can"! So can you. In fact, there's almost nothing to it - just put on a faster crystal, and run a sketch that keeps the part contiually active and printing material that can easily be checked for errors, particularly those that turn 1's into 0's. So far, I had success at room temperature with both external clocks and external crystals, but only when using the E-spec (extended temperature range) parts (which makes sense). Even most I-spec parts will run at 40 MHz - though not quite all: I've found that something under 10% of parts doesn't work at 40. As usual with AVRs, it's the ALU that starts failing first. I have an I-spec that, at 48 external clock will run a program that does a 1 second delay between calling micros (so lots of math is being done), then prints it. Like with over-overclocked tinies, if it's not so high that they crash immediately, they start getting math wrong, which shows up as 0's being printed for micros/millis(). This is unstable, as eventually a return address will get broken, it will return to that, and everything will fail (this may now turn into a reset if you're properly handling reset flags)

No special methods are necessary to undo overclocking - just upload code that uses the internal clock at an in-spec speed. (uploads, whether through the bootloader or via UPDI, are never carried out at clock speeds in excess of the specifications). Unlike, say, a desktop CPU, which if overclocked aggressively without sufficient cooling could easily destroy itself, the AVR core dissipates very little power amd so the heat being generated by the CPU itself is negligible even at double its rated clock speed,

## Strange possibilities for divided clocks
There are a *lot* of strange clock speeds possible through combinations of prescalers and the internal oscillator - ever wanted to run an MCU at 7 MHz? *Me neither*, but you totally can, even without a crystal... These exotic speeds are not currently supported by the tools menu options, but all the plumbing is there so that a custom board could be added. The switching logic to get that clock speed from the internal oscillator is implemented for the integer MHz speeds that can be achieved in that way. Fractional speeds are not implemented at all and total timekeeping failure should be expected. I'd be lying if I said I missed the struggle to make millis and micros accurate with weirdo clock speeds back on ATTinyCore (which in turn was done to support UART crystals, because some people are very picky about baud rate accuracy. You may laugh at their posts about making sure the clock is perfect for serial - but on classic AVR, without fractional baud rate generation, accuracy ) See also the [serial reference](Ref_Serial.md)
Like classic AVRs, tinyAVRs have a "speed grades" depending on the voltage and operating conditions that they are rated for operation within. The spec is 5 MHz @ 1.8V , 10 MHz @ 2.7V (3.3V nominal) and 20 @ 4.5V (5.0V nominal) (2.7 or 4.5

## Fuses do not control the clock speed on Dx
On Ex-series and tinyAVR parts, the internal oscillator is derived from either 16 or 20 MHz, and starts up prescaled by 6. Dx-series parts start up at 4 MHz with no prescaling. Regardless, the core configures that on startup in `init_clock()` (weakly defined so you can override it if need be). If it's external, we tell it to switch the clock and then poll the status to wait for it to pick up, if we wait around 1ms and still have no clock, we presume the clock non-functional and call `onClockTimeout()` which triggers the blink code. This is weakly defined function so it can also be overridden.

The Optiboot bootloader never changes the clock speed; it always runs at the startup speed - the bottleneck is not processing speed, it's the serial baud rate maximum imposed by .  This means that even if you set it to use a clock source that doesn't exist, or if an overclock the chip can't handle is selected, the bootloader will still run normally at startup, and an external reset will still reset the chip so the bootloader runs.

As a result of these factors, you never need to burn bootloader to configure the clock! If you are using an external clock source and timekeeping + serial come out wrong (eg, blink blinks at the wrong speed, Serial outputs wrong-clock gibberish), that means that the clock or crystal is not running at the speed you told the core it is. While there are ways we could detect this, I cannot justify the added bulk, time and complexity. Almost all crystals have the frequency printed on them (though it may be hard to read), and the ones that don't - if they're pulls, don't use them, and if you bought them new, you know the specs - even if you ordered multiple speeds that weren't marked, and then unpacked them into unmarked containers (tsk tsk), you can figure out which one you actually used by uploading a sketch that prints something to serial and trying the speeds you bought until one works.

## Why isn't my bloody crystal working?
*It's bloody? There's your problem! Wash that off amd dry thoroughly in flowing warm air* "I mean, we know that question isn't meant literally" *Reslly? I have that problem all the time - even oif it doesn't bridge things, it can throw off the load capacitance*
"If you're bleeding because of an injury while soldering, there might be a bigger problem there."

### Crystals
* The crystal and capacitors should be on the same side of the board as the chip.
* The crystal should be as close to the chip as possible, as should the caps.
* Don't route anything right underneath the crystal, and if you must do so, it should not have any fast changing signals or high frequency noise that could confuse the part.
  a. that assumes a two layer board. if your board has more layers and a ground plane underneath it, you don't need to be as careful.
* Nothing else should be connected to lines between crystal, caps and chips. They should be as short as possible with nothing else on them, including traces connected to nothing. This is why, on the breakout boards I sell, if not using a crystal, you need to connect a solder bridge jumper to tie the crystal pin to the holes along the edge of the board. If they were connected, it'd never oscillate.
* The total capacitance "seen" by each crystal pin must be as close to the correct load capacitance as possible. The capacitance comes from more than just the capacitors placed on the board: C<sub>load</sub> = C<sub>pin</sub> + C<sub>stray</sub> + C<sub>cap</sub>.
  * C<sub>cap</sub> is the value of the loading capacitors, individually. Notice from the above equation that this must be lower than the crystal's C<sub>load</sub>.
  * C<sub>pin</sub> is the capacitance of the pin. It can be found in the electrical characteristics chapter of the datasheet, "DC Characteristics" -> "I/O pin  Characteristics". For example on the DB-series, it is given as 4 pf.
  * C<sub>stray</sub> is the parasitic capacitance between the trace and the rest of the board. You can neither calculate this it depends on almost every physical parameter of trace, even if you knew thopse parameters, it would be a fiendishly difficult calculation. It is also very difficult to measure. Frequently, it is instead estimated. A value of 2-3pF is generally assumed. Thankfully, capacitance of loading caps does not need to be dead on accurate - small inaccuracies can be tolerated, with a small impact on frequency ("frequency pulling" when done intentionally). Large inaccuracies, however, will keep the crystal from working at all.
  * C<sub>load</sub> is spec'ed by the crystal and is rarely outside the range of 8pF to 22pF, with 18pF being very common, 12pF is also fairly common, while 22 pf is not uncommon on through-hole crystals. 8pF is the lowest plausible load capacitance that should be considered when selecting a crystal; 18pF is my suggestion.
  * Watch out when buying low cost crystals from China. It's easy to see that they come in the right package size, the frequency you need, and they're cheap as dirt (there's usually a large "China discount" on crystals, particularly the small SMD ones, which are markedly more expensive from Western suppliers), then realize the load capacitance wasn't listed, nor was the part number, that you can't find the part number by looking up the the numbers printed on the crystal, and that when asked, the seller in broken english says that they don't know (this in turn implies that those crystals aren't even coming from reels in someone's basement - because those would have the part number and likely the load capacitance marked. *I'm imagining someone with loose smd crystals in sandwich bags, selling them in an alleyway...*)
* Breadboards will usually have too much stray capacitance. If they work at all, it will likely be with no loading caps at all. Breadboard is kryptonite for high frequency.
  * The same goes for prototyping board, though it is not quite as bad. On my upcoming wide DIP protoboard, an easy place to cut the trace (like the normally-connected solderbridge jumpers on other boards) is proivided that, for every pin, you can isolate its hole and the nearest connected hole from the rest of them, and the copper around the rim of the hole is thinner too; this helps. You will likely want smaller caps than usual, too. On boards for specific chips which support a crystal (currently the DA/DB/DD DIY prototyping board), PA0, PA1 and PF0, PF1 that is present, and it is NOT connected by default, on the grounds that it's easier to bridge them then to cute a trace, even one meant to be cut. For other protoboard, you use the highest load capaccitance crystal that is within spec and hope that with no loading caps or small ones. With 18pF caps, you can usually make it work with very small caps.
  * This applies to both the watch crystal and high frequency crystals. Both external crystal circuits are markedly weaker than you're used to coming from classic AVRs, and hence are markedly less forgiving of poor design practices regarding crystal layout.
* The two traces going to the crystal should be similar in length. They don't need to be exact, but they should not be radically different
* SMD caps are preferable to through-hole ones for loading capacitors. If you do use through hole ones, put the leads in all the way and trim the excess off the rear of the board.
* Touching the crystal, caps, or traces may cause it to stop working. This is most common when the loading caps are barely close enough to work.
* It is easier to get these parts to work with too little loading capacitance than with too much.
* Remember **There is much less reason to use a crystal on a modern AVR:** Our internal osc is full speed, and within a percent or so of nominal at room temperature, little (tinyAVR) to no (AVR Dx) dependence on voltage,


### External clocks
Thankfully, these are simpler: Just keep the high frequencty trace short, and make sure that you provide the decoupling capacitor the datasheet asks for. Most often I see 0.01uF specified for external clocks, rather than the more common 0.1uF . There's usually a pin, pulled up internally, that will disable it when brought low. Make sure you don't connect that to ground - usually it will be pulled up if allowed to float. Be careful about the orientation, too - these are rectangular, and can be rotated 180 degrees with all the pads lining up. Doing that will swap Vcc and Ground. Like most ICs, that reversed polarity configuration will destroy them almost instantly, and present a near-short to the power supply.

So with the added simplicity, one might wonder why they are so uncommon. There are several reasons:
1. They are power hogs. 10-25 mA is normal, 50mA is not unheardof.
2. They are picky about voltage. There was a line of oscillators in 5032 package in production until mid 2020 which worked at 1.8-5v. They were discontinued, and there is no other such oscillator available as far as I can tell.
  a. The 5v ones specify minimum 4.5v maximum 5.5v. One thus, cannot be run direct from a lipo cell (3.7). The 3.3v ones are spec'ec 3.6v max, znd nothing is rated for use between 3.6 and 4.5V. LiPo voltages are 3.7-4.2, so this
  b. 1.6-3.6v ones are available, so the lower end of the range is covered. None of the major vendors with parametric search on their catalog indicate that any which are in spec operating between 3.6v and 4.5V, so one cannot run directly from a LiPo battery with one while remaining within spec.
3. 5v units are sometimes hard to find in smaller package sizes, 5032 is exotic for 5v oscillators, and anything smaller nonexistent.
4. They are strangely expensive, typically $1 or more from western suppliers. There is hardly any "China discount", either. Like crystals, however, the oscillators advertised on Aliexpress don't list essential specs like the voltage or part number, and sellers don't seem to know when asked.
  a. One gets the impression that external oscillators are a specialty item for precision applications, while crystals are not. They are often designed to higher accuracy and they don't depend on external components that could "pull" the frequency, and so on. In a typical application, if you don't need a precision clock source, there are other ways to get it, and they're all cheaper and easier.
  b. To add injury to insult, or the other way around (*Surely you can figure out which way around you are* "All I know is there's an excess of both insult and injury here"), the most common package has an almost invisible polarity marker, and if installed backwards, will instantly destroy itelf.

## Clock troubleshooting
On a classic AVR, the result of selecting a clock source (external crystal or clock) which does not function is not subtle: You burn the bootloader to set the fuse to use that clock source, and the chip ceases to respond, including all attempts to program, other than HV programming unless provided a suitable clock. That sucked. IIRC I used to use  a version of ArduinoAsISP that I'd run a seventh wire from, carrying a 4 or 8 MHz square wave (generated with TIMER1) because it was so common to end up with crystal-less systems set to use a crystal, or crystal-using systems set for external clock. *Never a fun time, and crystals are one of the more unpleasant types of parts to work with.*  Fortunately, this is a thing of the past with modern AVRs. in the new clocking sysytem, it starts up at a universally tolerable speed, and it's left to application code to set it. These cores do that to set it to try to run at the selected speed prior to calling setup.  a clock switch.  speed and can be easily switched at runtime. Recovery is typically as simple as uploading a new sketch with more appropriate clock settings.

On *megaTinyCore*, troubleshooting is generally straightforward, and on the DA, it's even simpler.

* Are you using an external clock? Verify w/scope that input is making it to PA3.
  * If no clock reaching PA3, there's a hardware issue - incorrect connections, poor connections, damaged or defective components. That should guide your investigation. Regardless, it has nothing to do with the AVR.
  * If there is - you're sure the signal is there even when you're not pressing with the scope probe? Try uploading some dead simple test sketch;
  * You are not prevented from simply uploading a new sketch compiled to use a different clock source. Uploading something totally different (ex, blink) with clock set to an internal source without tuning.
    * I do not believe it is possible for anything other that wiring/connection/soldering issue provided that a known working serial adapter (FT232RL and fakes), afaik all the WCH USB-serial bridges (the 343 is a nicer single port one,and the 342 is a nicer *DUAL*  serial chip. They start to get annoying to route after that,)



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

It is important to notethat

```c++
void onClockFailure() {
  _PROTECTED_WRITE(RSTCTRL.SWRR,1); //issue a software reset.
}
```

### Failure modes
A number of behaviors have been observed when the clock has been configured wrong, is not functioning, or that particcular specimen can't do the overclocking you are asking of it.

1. If there is a crystal/oscillator which does not oscillate at all, you will get the blink code On-Off-On, then 3 brief "antiblinks" where the led turns off momentarily, then Off-On-Off and 3 brief flashes, repeating. (this pattern of alternating inverted output is meant to be unlikely to mimic sketch behavior).
2. If there is a crystal, but it is defective, damaged, or being used outside it's operating range, it may "limp", or intermittently oscillate with inconsistent pulse lengthsfrom cycleto cycle, leading the the spec for change in length of consecutive clock cycles being violated
3. If the clock source fails while the system is running, a DA-series will hang (until something resets it - the WDT, the reset button, or powecycling are the only ways to get it out of this state.
4. A clock failure while running very often resets the chip (likely a [dirty reset](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Reset.md#the-danger-of-dirty-resets), caused by incorrect execution of instructions when the variation between clock periods is far in excess of the manufacturer spec and it fumbles math with the program counter - the most common overclock manifestation as far as I can tell is 1's in math and memory access and like, everything coming out as 0', often and entire byte full)
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

Without a "FET probe" for my scope (don't even think of using a normal scope), I don't think I have much hope of understanding this bizarre behavior. I'm sure it would make total sense if I could see the waveforms, but it changes the capacitance of the

## Solving clock problems
First, if the sketch doesn't seem to run at all, switch to internal clock at a speed which is within spec and make sure you're not just trying to overclock it too hard, or trying to use tuning but somehow have bunk tuning values written or something dumb like that. If that doesn't do anything either it points to your sketch, and/or bad connections to external devices causing a hang during initialization (this is what the majority of hangs are with the dirty reset protection that we have anyway. (Without it, it was fairly easy to get it to a state where it needed a reset pin reset or power cycle to revive through bad code. Now it will generally just bootloop at worst, and it's straightforward to probe them by writing to a pin with an LED on it and moving that around to find where it's getting stuck, if you keep it on github, compare to older versions, etc - you introduced a bug around the same time you decided to try the crystal, or have a loose wire (don't underestimate this if you wait on external devices when starting up. Especially if you wired it up using "dupont line" - the Chinese dupont line, normally made from ribbon cable wire (generally several AWG smaller than the claimed thickness, if spec'ed), uses an inferior clone not of the DuPont connector, but of a western knockoff; that is where most of the innovation (cost savings, coupled ) . (Without crimping your own, I've gotten best results from buying chinese "terminal line" with the terminals but no housings, and a bunch of housings - they are usually made with less-bad terminals,  and avoid the ribbon cable dupont like like the plague). If it does work when you go back to internal oscillator, disconnect power and inspect it carefully for dubious solder joints, especially around the crystal or clock source.

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
There are two common failure modes that can be seen here. Either: it marks itself as due to switch clock to external. It then polls the clock status to see when the oscillator switch has occurred successfully. That should be seen within microseconds and after some milliseconds times out (and should do the blink code)
Unlike crystals, you can actually put a 'scope probe onto the output of a the oscillator and see the signal. Do that. Do you see the signal?
1. If not - examine the datasheet and try to figure out why it might not be oscillating.
  a. You didn't install it rotated 180 degrees did you? the rectangular oscillators usually have Vcc and ground opposite each other, so everything will be lined up and can be soldered down with the oscillator connected backwards. *This will irrevocably destroy it the moment power is applied*. You can easily determine if this has happened - the external clock will be hot, and you may notice signs of the board consuming more current than expected (like power LEDs on a serial adapter supplying power being dimmer when connected to the board).
  b. Verify that it is getting power and ground connected to the proper pins. (on my breakout boards which support an external clock, you may need to change the connections of some solder bridges - if the board can take either a crystal or a clock in one of those rectangular packages, there must be a solder bridge jumper somewhere to configure whether the two corners that are neither the data output nor the enable pin are both Gnd (as is the case for a crystal) or if one needs to be grounded and the other at the supply voltage.
  c. Most have a pin that can be pulled low to turn off the output to save power. If allowed to float, they generally have internal pullups and can be left alone, but putting a 10k pullup to Vcc is an easy test to verify. Is yours shorted to ground by any chance?
  d. These arent't crystals, but they can't drive an arbitrarily long wire or work into a large load.
2. If you do see a signal, and it is no higher than the chip's rated speed (i.e., there's no chance it's actually too fast for the chip), make sure you see the same signal on the pin of the chip (cold solder joints happen - I've seen parts with a pin that wasn't actually soldered in place, but which looked like it was, and when pressure was applied to the pin with the scope probe - it started working; this is much more frustrating when you're trying to debug it with the power off with a DMM, and you check every connection, yup, continuity.... but when you actually apply power, it's not making contact, because the pressure you were exerting with the probe was what held the pins in contact. This is of course  basically impossible to diagnose with power off (since you are never sure if the poor contact is just between your probe and the pin, or between the pin and pad); it becomes much easier to diagnose while running, since prodding the pin will suddenly bring the device back to life.
