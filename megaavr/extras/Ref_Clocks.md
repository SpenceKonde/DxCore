# Detailed Clock Reference
This section seeks to cover information about the clocking options on the Dx-series parts at somewhat greater depth than a simple list of supported opotion. The use of an external RTC clock will be covered in a future document, and there is FAR MORE to say about that than meets the eye. All I will say here is that the external clock appears to be limited onlt to frequences less than 1/4th of CLK_PER. There are some theoretic possibilities there that will make your head spin.

## Supported Clock Speeds
All speeds are supported across the whole 1.8V ~ 5.5V operating voltage range!

| Clock Speed | Within Spec |      Internal |  Ext. Crystal |    Ext. Clock | Notes
|-------------|-------------|---------------|---------------|---------------|---------------
|       1 MHz |         Yes |           Yes | Not by DxCore | Not by DxCore | 1 MHz is used for low power stuff, but xtal/ext osc uses more power.
|       2 MHz |         Yes | Not by DxCore | Not by DxCore | Not by DxCore | Unsupported. micros() would work, delayMicroseconds would not.
|       3 MHz |         Yes | Not by DxCore | Not by DxCore | Not by DxCore | Unsupported. micros() would work. delayMicroseconds would not. micros() comes for free with 6/12/24/48
|       4 MHz |         Yes | Not by DxCore | Not by DxCore | Not by DxCore | 4 MHz Internal is default. Optiboot always uses this.
|       6 MHz |         Yes |     Prescaled | Not by DxCore | Not by DxCore | Unsupported. Prescale 12 by 2. micros would work, delayMicroseconds would not.
|       7 MHz |         Yes |            No | Not by DxCore | Not by DxCore | Unsupported. Prescale 24 by 4. micros would work, delayMicroseconds would not.
|       8 MHz |         Yes |           Yes |           Yes |           Yes |
|      12 MHz |         Yes |           Yes |           Yes |           Yes |
|      14 MHz |       Kinda | Not by DxCore | Not by DxCore | Not by DxCore | Unsupported. Prescale 28 by 2. micros would work, delayMicroseconds would not.
|      16 MHz |         Yes |           Yes |           Yes |           Yes |
|      20 MHz |         Yes |           Yes |           Yes |           Yes |
|      24 MHz |         Yes |           Yes |           Yes |           Yes |
|      25 MHz |          No |            No |           Yes |           Yes | A lot of people have infinite 25 MHz crystals, apparently.
|      28 MHz |          No |           Yes |           Yes |           Yes | Generally works at room temp. Lousy speed
|      30 MHz |          No |           Yes |           Yes |           Yes | Max. megaTinyCore overclock for 0/1 series via tuned internal
|      32 MHz |          No |           Yes |           Yes |           Yes | Generally works at toom temp.
|      36 MHz |          No |            No |           Yes |           Yes | Never tested it, oddly enough. Will probably work on nearly all parts in favorable conditions.
|      40 MHz |          No |            No |    Most chips |    Most chips | Majority of parts, even non-E-spec work w/xtal @ room temp. Some can't. Switching to external CLOCK might help.
|      44 MHz |          No |            No |  May not work | Not by DxCore | Unsupported because nobody wants it. I think it would just work though, if there was any reason to use it.
|      48 MHz |          No |            No |  Doesn't work |    Some chips | Some E-spec (extended temp. range) can do it.

Running at 48 MHz is an ambitious overclock and is totally unnecessary. So far, I had success at room temperature with external clocks, but not external crystals, and only when using the E-spec (extended temperature range) parts (which makes sense). I am surprised how they will "just work" at 40 from a crystal though, even I-spec parts usually do However, not all parts are capable of this. Out of around a dozen parts, I've so far found 1 that doesn't work at 40. As usual with AVRs, it's the ALU that starts failing first. I have an I-spec that, at 48 external clock will run a program that does a 1 second delay between calling micros (so lots of math is being done, then prints it). Like with over-overclocked tinies, if it's not so high that they crash immediately, they start getting math wrong, which shows up as 0's being printed for micros/millis(). This is unstable, as eventually a return address will get broken, it will return to that, and everything will fail (this may now turn into a reset if you're properly handling reset flags)

There are multiple ways to generate some of the lower frequencies from internal oscillator (do you prescale from higher frequency, or set the oscillator to the desired one? Suspect the latter is more power efficient, but with the former you could still use the PLL while staying in spec - (in my tests the PLL worked well beyond the spec in both directions, at least at room temperature, not that you'd want to do that in production) - currently, we set the main oscillator to the desired frequency, however we may revisit this decision in the future. There might be reasons to just run the TCD off the unprescaled clock in order to.... I'm not sure what....

The DA-series does not support use of an external high frequency crystal, only external clock or the internal oscillator can be usedl The internal oscillator is pretty accurate, so internal clock will work fine for UART communication()they're within a fraction of a percent at room temp) with very little voltage dependence (they have an internal regulator to generate the core voltage, which runs at a much lower voltage, and I suspect that's where the internal oscillator is located.

## Auto-tuning

All parts, including the DA, can - if needed - use an external watch crystal to automatically tune the internal oscillator frequency, a feature called Auto-Tune. Though they specify +/- 3% internal oscillator speed, in practice, I have yet to find one that was off by more than 1 calibration "notch" at room temperature - the accuracy is limited by the granularity of tuning more than anything else. These are just in a different universe than the classic AVRs where a couple percent was normal. I had to use a torch aimped at the chip to swing the temperature enough that autotune had to correct the frequency on the fly (resting the soldering iron on it didn't. The modern AVR internal oscillator is really good. Nonetheless, we provide a wrapper around enabling external 32K crystal and enabling/disabling Auto-Tune in [the DxCore library](megaavr/libraries/DxCore).

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

There are a *lot* of strange clock speeds possible through combinations of prescalers and the internal oscillator - ever wanted to run an MCU at 7 MHz? *Me neither*, but you totally can, even without a crystal... These exotic speeds are not currently supported by DxCore - I'd be lying if I said I missed the struggle to make millis and micros accurate with weirdo clock speeds back on ATTinyCore (which in turn was done to support UART crystals, because some people are very picky about baud rate accuracy. You may laugh at their posts about making sure the clock is perfect for serial - but if you're not careful, on classic AVRs, the granularit of USART baud rates was horrendous near the higher speeds! 8 MHz classic AVRs wanting to use 115200 baud had a choice between 4% too fast, and 3.5% too slow.  There is no longer a need to use weirdo crystals - the fractional baud rate generator has solved the baud rate generation issue decisively!

## Fuses do not control the clock speed
These parts always start up running at 4 MHz from the internal oscillator. The core confihures that on startupnit_clock() function (weakly defined so you can override it if need be), the clock is set to the desired option. If it's external, we tell it to switch the clock and then poll the status to wait for it to pick up, if we wait around 1ms and still have no clock, we presume the clock non-functional and call `onClockTimeout()` which triggers the blink code. This is weakly defined and can be overriden.

The Optibott bootloaded never changes the clock speed; it always runs at 4 MHz. The bottleneck is not processing speed, it's the serial baud rate and the flash write time (during which the CPU is halted).  This means that even if you set it to use a clock source that doesn't exist, or if an overclock the chip can't hadle is selected, the bootloader will still run normally at startup.

As a result of these factors, you never need to burn bootloader to configure the clock! If you are using an external clock source and timekeeping + serial come out wrong (eg, blink blinks at the wrong speed, Serial outputs wrong-clock gibberish) the clock or crystal used does not match the selected setting. While there are ways we could detect this, I cannot justify the added bulk, time and complexity. Almost all crystals have the frequency printed on them (though it may be hard to read), and the ones that don't - if they're pulls, don't use them, and if you bought them new, check the purchase records - even if you ordered multiple speeds that weren't marked, and then unpacked them into unmarked containters, you can figure out which one you actually used by uploading a sketch that prints something to serial and trying the speeds you bought until one works.

## Rules for PCB layout around crystals
There are only two places where someone with an AVR is likely to routinely encounter layout sensitive behavior. Crystals and clocks are one of them (the other is decoupling caps, which don't do their job unless they are right next to the pins they are associated with):
### Crystals
* The crystal and capacitors should be on the same side of the board as the chip.
* The crystal should be as close to the chip as possible, as should the caps.
* Don't route anything right underneath the crystal, and if you must do so, it should not have any fast changing signals or high frequency noise that could confuse the part.
  a. that assumes a two layer board. if your board has more layers and a ground plane underneath it, you don't need to be as careful.
* Nothing else should be connected to lines between crystal, caps and chips. Thy should be as short as possible with nothing else on them, including traces connected to nothing. On the breakout boards I sell, if not using a crystal, you need to connect a solder bridge jumper to tie the crystal pin to the holes along the edge of the board. If they were connected, it'd never oscillate.
* The total capacitance "seen" by each crystal pin must be as close to the load capacitance as possible. The capacitance comes from more than just the capacitors placed on the board: C<sub>load</sub> = C<sub>pin</sub> + C<sub>stray</sub> + C<sub>cap</sub>.
  * C<sub>cap</sub> is the value of the loading capacitors, individually.
  * C<sub>pin</sub> is the capacitance of the pin. It can be found in the electrical characteristics chapter of the datasheet, "DC Characteristics" -> "I/O pin  Characteristics". For example on the DB-series, it is given as 4 pf.
  * C<sub>stray</sub> is the parasitic capacitance between the trace and the rest of the board. You can neither calculate this it depends on almost every physical parameter of trace, even if you knew thopse parameters, it would be a fiendishly difficult calculation. It is also very difficult to measure. Frequently, it is instead estimated. A value of 2-3pF is generally assumed. Thankfully, capacitance of loading caps does not need to be dead on accurate - small inaccuracies can be tolerated, with a small impact on frequency ("frequency pulling" when done intentionally). Large inaccuracies, however, will keep the crystal from working at all.
  * C<sub>load</sub> is rarely outside the range of 8pF to 22pF, with 18pF being very common, 12pF is also fairly common, while 22 pf is not uncommon on through-hole crystals. 8pF is the lowest plausible load capacitance that should be considered when selecting a crystal; 18pF is my suggestion.
  * Watch out when buying low cost crystals from China. It's easy to see that they come in the right package size, the frequency you need, and they're cheap as dirt (there's usually a large "China discount" on crystals), then realize the load capacitance wasn't listed, nor was the part number, that you can't find the part number by looking up the the numbers printed on the crystal, and that when asked, the seller in broken english says that they don't know.
* Breadboards will usually supply too much stray capacitance. If they work at all, it will likely be with no loading caps at all. Breadboard is kryptonite for high frequency.
  * The same goes for prototyping board, though it is not quite as bad. On my upcoming wide DIP protoboard, an easy place to cut the trace (like the normally-connected solderbridge jumpers on other boards) is proivided that, for every pin, you can isolate it's hole and the nearest connected hole from the rest of them, and the copper around the rim of the hole is thinner too'; this helps. You will liekly want smaller caps than usual, too. On boards for specific chips which support a crystal (currently the DA/DB/DD DIY prototyping board), PA0, PA1 and PF0, PF1 that is present, and it is NOT connected by default, on the grounds that it's easier to bridge them then to cute a trace, evem one meant to be cut. For other protoboard, you use the highest load capaccitance crystal that is within spec and hope that with no loading caps or small ones. With 18pF caps, you can usually make it work with very small caps.
* The two traces going to the crystal should be similar in length. They don't need to be exact, but they should not be radically different
* SMD caps are preferable to through-hole ones for loading capacitors. If you do use through hole ones, put the leads in all the way and trim the excess off the rear of the board.
* Touching the crystal, caps, or traces may cause it to stop working. This is most common when the loading caps are barely close enough to work.


### External clocks
Thankfully, these are simpler: Just keep the high frequencty trace short, and make sure that you provide the decoupling capacitor the datasheet asks for. Most often I see 0.01uF specified. There's usually a pin, pulled up internally, that will disable it when brought low. Make sure you donn't connect that to ground. Be careful about the orientation, too, since they are rectangular, and can be rotated 180 degrees with all the pads lining up. This will swap Vcc and Ground and destroy the oscillator almost instantly.

So with the added simplicity, one might wonder why they are so uncommon. There are several reasons:
1. They are power hogs. 10-25 mA is normal, 50mA is not unheardof.
2. They are picky about voltage. There was a line of oscillators in 5032 package in production until mid 2020 which worked at 1.8-5v. They were discontinued, and there is no other such oscillator available.
  * The 5v ones specify minimum 4.5v maximum 5.5v.
  * 1.6-3.6v ones are available, so the lower end of the range is covered. None of the major vendors with parametric search on their catalog indicate that any which are in spec operating between 3.6v and 4.5V, so one cannot run directly from a LiPo battery with one while remaining within spec.
3. 5v units are not available in packages smaller than 7050 (7mm x 5mm) typically, 5032 is exotic for 5v oscillators, and anything smaller nonexistent.
4. They are strangely expensive, typically $1 or more from western suppliers. There is hardly any "China discount", either. :ike crystals, however, the oscillators advertised on aliexpress don't list essential specs like the voltage or part number, and sellers don't seem to know when asked.
  * One gets the impression that external oscilaltors are a specialty item for precision applications, while crystals are not. They may be designed to higher accuracy and they also don't depend on external components that could "pull" the frequency.


## Clock troubleshooting
On a classic AVR, the result of selecting a clock source (external crystal or clock) which does not function is not subtle: You burn the bootloader to set the fuse to use that clock source, and the chip ceases to respond, including all attempts to program. Fortunately, the Dx-series parts handle this situation far more gracefully. Largely this is simply because they don't set it with fuses - it is set by the appllication at runtime (when you compile DxCore, it builds with the selected frequency options) including switching clock sources before calling setup(). Sometimes the sketch will just cease to function entirely when it tries to switch. We attempt to show a blink code - but this doesn't seem to work as well as it should. In any event, there are no challenges recovering from it. Just change the clock source to an internal one, upload normally and you're good.

### Blink Codes
All blink codes issued by the core start with the LED pin switching states three times (ie, if it is off, it will turn on, off, then on again), followed by a number of flashes indicating the nature of the failure. This will then repeat - since it initially changes state three times, this means that the pattern will be inverted the second time through. The number of short flashes indicates the nature of the problem: Three flashes indicates that the selected clock source was not present at startup. Four flashes indicates that it was present at startup, but then disappeared while it was running. It is hoped that this will make the blink codes distinctive enough that they cannot be mistaken for the running sketch. You can override the `onClockFailure()` and `onClockTimeout()` functions to run different code. The clock failure detection seems flaky, and often it just resets instead. when onClockTimeout() is called, it will be just after startup running at 4 MHz; scale any baud rates by (baud rate compiled for)/(4 MJz) to calculate the baud rate to ask it for if you want to output debug messages. These may be most important if PA7 needs to be repurposed, and you can't have it taking over that pin no matter what. Also, be warned that they are not bulletproof - sometimes clock problems will just not work, issuing neither blink code.

For example, if the crystal (or clock) speed is one that can also be generated from the internal oscillator, you might want to resort to that and run the app - albeit with less accurate timing - in spite of the problem with the crystal - One approach is shown below.
```c++
byte clockfallback = 0; // global scope

void onClockTimeout() {
  // Uhoh, the external crystal or clock isn't oscillating.
  clockfallback = 1; // Application can check this to produce an appropriate warning
  _switchInternalToF_CPU(); // Set internal HF oscillator to correct source
  PROTECTED_WRITE(CLKCTRL_MCLKCTRLA, CLKCTRL_CLKSEL_OSCHF_gc); // set the main clock source back to internal, so it's not in the state where it's standingby to switch over to the (obviously dodgy) external one if that suddenly works.
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

`_switchInternalToF_CPU()` is a macro provided essentially for this and only this purpose (I can't think of any other use cases...).  While you could of course write to `CLKCTRL_OSCHFCTRLA` and that's normally what we would tell you to do,, Microchip has changed the name of the bitfield in a released ATpack which is not yet being used by this core. To futureproof against that, this macro, like the initialization routines, uses the bare numbers, and if I told you to use one spelling, it would break in the next toolchain version, and if I told you the spelling that would work in the next compiler version, it wouldn't work today. But working with a bunch of raw numbers and remembering what they do is

An analogous situation in the event of clock failure is probably not a good idea; the clock failure detection interrupt hardly ever triggers in my experience, even when I, say, cause the crystal to stop oscillating by connecting a long wire to it or touching it a non-rigid but internally conducting object (which changes capacitance) connected to a nearly 6 foot long, oddly shaped and utterly untuned squishy antenna (respectivly, I think the technical terms for those are "hand" and "body, but that's way outside my field). In these cases, the failure rarely results in the CFD interrupt being called. Instead, I would suggest you just reset if you want to fallback to internal - use the above code to detect the clock timeout at startup, and override onClockFailure to simply reset, where presumably the startup would find that it didn't have a clock and set intoernal clock correctly:
```c++
void onClockFailure() {
  _PROTECTED_WRITE(RSTCTRL.SWRR,1); //issue a software reset.
}
```

### Failure modes
A number of behaviors have been observed when the clock has been configured wrong, is not functioning, or that particcular specimen can't do the overclocking you are asking of it.

* If there is a crystal/oscillator which does not oscillate at all, you will get the blink code On-Off-On, then 3 brief "antiblinks" where the led turns off momentarily, then Off-On-Off and 3 brief flashes, repeating. (this pattern of alternating inverted output is meant to be unlikely to mimic sketch behavior).
* If there is a crystal, and is mostly non-functional, but is close enough to working that the clock controller thinks it is OK while it's not actually working, it will typically hang or bootloop. This is commonly seen when inappropriate loading capacitors are used.
* If the clock source fails whike the system is running, a DA-series will hang (until somerthing resets it - the WDT, the reset button, or powecycling are the only ways to get it out of this state. Sometimes DB-series with the CFD enabled do that too.
* A clock failure while running very often resets the chip (likely a [dirty reset](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Reset.md#the-danger-of-dirty-resets))
* A clock source that resets the chip or hangs immediately upon switching to it (easier to detect if using Optiboot - though you could blink an LED before the switch to see if it's hanging or bootlooping by overriding onPreMain():
```
#include <util/delay.h> -  there is no timekeeping in onPreMain

void onPreMain() {
  openDrainFast(LED_BUILTIN,LOW); //As discused in digital I/O reference, this is currently also the cheap trick to change direction with fastDigitalIO.
  _delay_ms(500*4/24); // F_CPU is #defined globally, but init is the first part of main. This is running before that so you need to use the builtin avr delay functions.
  digitalWrite(LED_BUILTIN, CHANGE); // now it's high
  _delay_ms(500*4/24); // F_CPU is #defined globally, but init is the first part of main. This is running before that so you need to use the builtin avr delay functions.
  digitalWrite(LED_BUILTIN, CHANGE); // now it's low
  _delay_ms(500*4/24); // F_CPU is #defined globally, but init is the first part of main. This is running before that so you need to use the builtin avr delay functions.
  digitalWrite(LED_BUILTIN, CHANGE); // now it's high again. Allow startup to proceed, leaving he LED on to detect resetting into a bad state as opposed to hanging in a bad state.
}


```
)

## Solving clock problems
First, if the sketch doesn't seem to run at all, if you have an oscilloscope, or even a "logic probe" that will tell you if there are frequent transitions on a pin, connect it to PA7, and override onPreMain() with:
```c++
void onPreMain() {
  _PROTECTED_WRITE(CLKCTRL_MCLKCTRLA,0x80);
}

```
That will cause PA7 (can't be chagned) to output the system clock, and the startup routines do not change the high bit of that register, hence, the clock will continue to be output. If you see the app hung, but the clock output is coming out, that implies that it's not caused by a problem with the clock itself. This does not rule out an excessive oveclock (the AVR128DB32 that doesn't run at 40 does this).

Try an internal clock of not higher than 24 MHz. If this also doesn't work, it is 100% a bug in your sketch, fix it.

If you are unable to upload new code, the problem has nothing to do with the clock, and you may have danaged the chip in some way.


### Crystals
Frequency crystals are fickle beasts. They all come spec'ed with a load capacitance, (well, unless you buy from halfasssed sellers on aliexpress, in which case the manufactuerer most certainly did specify it, but the vendors often have no idea what it is). The crystal and two loading caps should be located on the same side of the PCB as the chip, away from sources of noise or interferance (read: surrounded by ground pour and nothng likeky to cause interferance on the other side ofthe board ). Caps far from the correct values will result in no oscillation at all, as will a failed crystal (it happens).
My protocol when I encounter this is:
1. Unplug board, multimeter in continuity mode
  a. Verify no short between ground and either crystal pin (if you can't see where the short is, it's either under the crystal or under one of the loading caps.
  b. Verify the two pins are not shorted to eachother.
  c. Verify that neither of the pins are shorted to any nearby pins or traces.
2. If it never worked:
  a. squint at the loading capacitors, and compare them to the ones you were supposed to use. Do they look like the same capacitors? For SMT capacitors, compare both the color of the ceramic and the shape of the terminations (which varies by manufacturer - some have a perfectly straight boundary between metalized termination and ceramic, on others, it is bowed slightly. For through-hole ones, double check the value; use an online capacitor value calculator if you aren't confident in interpreting the 3-digit codes for capacitance that are often seen here. You may have used the wrong capacitors, or you may have been blindly pulling from one bin.... which was contaminated with a few of a different value (I usually don't try to save spilled SMD capacitors. they cost a penny or two most of the time - not worthe the debugging time if you get it wrong)
  b. Double check your calculations of the appropriate load capacitance. If you're not confident that they're approximately correct, remove the two capacitors and try again (with no caps - amazingly this often *just barely* works, though it will usually crash if you put your hand near it. Your original loading capacitor value was too large, likely because you underestimated stray capacitance. Use a significantly lower value capacitor.
  c. Your crystal may just be bad.
3. if it previously worked
  a. If SMD caps, put a touch of "no clean" flux ("rework", "gel" ,"tacky" and similar words often usedto describe the type you want. The liquid kind is usually not as effectvive)  on the caps, and briefly touch an soldering iron tip to each end of each cap. (alternate capacitors to give the end you reflowed a chance to cool. if you can, do the same for the pads of the crystal. Try it again.
  b. Failing that, replace the crystal.

### External clocks
Unlike crystals, you can actuall put a 'scope probe onto the output of a the oscillator and see the signal. Do that. Do you see the signal?
1. If not - examine the datasheet and try to figure out why it might not be oscillating.
  a. Verify that it is getting power and ground comnnected to the proper pins. (on my breakout board which support either a crtystal or external clock, you need to complete a solder bridge to  connect it to Vcc, so you can also use it with a crystal)/
  b. Most have a pin that can be pulled low to turn off the output to save power. If allowed to float, they generally have internal pullups and can be left alone, but putting a 10k pullup to Vcc is an easy test to verify. Is yours shorted to ground by any chance?
  c. You didn't install it rotated 180 degrees did you? the rectangular oscillators usually have Vcc and ground opposite eachother, so everythign will be lined up and can be soldred down with the oscillator connected backwards. This will irrevocably destroy it within seconds. (put your finger on it. If it's hot, the oscillator is dead, and likely this is why).
2. If you do see a signal, and it no higher than 24 MHz, it really should be working, and I'm not sure where I would look next.
