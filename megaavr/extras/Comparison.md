# DA vs tinyAVR 1-series and megaAVR 0-series peripherals
While reading datasheets, I looked for differences as those are places where the core would need to be adapted. These are some notes, presented without further editing.

## VREF
Same deal, except the reference voltages are better now. With 12-bit resolution, and 4.096V, 1 LSB is 1mV! It doesn't get any easier than that!
Also, you can now (finally!) use Vcc as the DAC reference voltage.

## TCA
EVACTB added.
Otherwise, appears identical, complete with the wonky CTRLExxx CMDEN bits in split mode, with values of NONE (0b00) and BOTH (0b11), but the other two marked reserved. Something tells me they screwed up and this didn't work, and they didn't bother to fix it in the DA series. Were I microchip, I'd have just disabled those bits and made them act like 1's - unless they thought maybe they'd fix it in some version of the chip?
The **REAL** change with TCA is that you get 2 of them on 48 and 64 pin parts! Which meaaans: TWO INDEPENDENT PRESCALERS FOR PWM!

## TCB
The only timer with Real, Meaningful improvements. These have gone from great utility timers to amazing ones, with a few simple additions:
Added CLKSEL TCA1 option - all TCBs can pick their clock source! Now you can take over a TCA and still use TCBs (and the other TCA) without messing up the prescaler for your TCBs!
(the big one) Added EVENT clock option - now TCB as well as TCA can be clocked from events!
They got an overflow interrupt too! Though in most modes it is redundant...
Oh, and did I mention the CASCADE bit? You can clock one timer off another timer's overflow, and set cascade on the one that counts the two MSBs to make sure they are in sync (ie, correct for propagation delays) and get a 32-bit input capture. Yes, that's right, you can time something lasting nearly 3 minutes TO A TWENTYFOURTH OF A MICROSECOND!!!

## TCD0
Only difference appears to be the addition of the PLL as a clock source! Probably also has different errata.

## DAC
Only significant difference is that it's 10 bit... and the DATA register is left-adjusted, so if you just want 8 bits, you can just write the one register. Nifty.
And that VREF lets you use Vcc.

## ADC
RESSEL now chooses between 10 and 12 bits
differential ADC is now supported. I know this is very exciting to everyone in Arduino-land, because I know you all do differential ADC measurements on classic AVRs all the time right? No? Uh, me neither... I don't think I've ever taken a differential ADC reading in my life.

Oh, and you can also measure the AC DACREFs, if you want to measure a number you control or something...? Probably mostly a clever way to either check accuracy or make math easy for Vcc measurement?

## AC
Looks pretty much the same to me....

## TWI
Same one as the megaAVR 0-series got, with the dual mode thing...

## SPI
Same as before. On early 128k chips, SPI0 must have SSD=1 to run in master mode if set to the default pin mapping. We always set that because we NEVER supported slave

## USART
Same as before. Still has the same errata with open drain mode... they might as well just add it to the datasheet at this point.

## CCL
Errta doesn't mention the D-latch? Does that mean we have a working D-latch finally?
Other than that, minor changes to the INSEL bits to make sense with all the extra peripherals the DA series has, but otherwise, it's pretty much the same as the CCL that the megaAVR 0-series got.

## EVSYS
Basically the event system from the megaAVR 0-series.
Errata: ~Early~ AVR128DA64 parts have no connection to event system for the PE, PB pins that aren't present on the 48-pin parts. [8 months later, still no silicon revision to fix this]

## NVMCTRL
TOTALLY DIFFERENT!
Writing via UPDI means manipulating NVMCTRL to do your bidding. This was quite the challenge. The writing is lovely though,

Errata: Early 128k parts incorrectly apply the bootloader protections to all 32k blocks of flash, so you can't write using the memory mapping... Read still works fine assuming the bit to make the bootloader section unreadable from app is not set.


## CLKCTRL
Lot of differences!
For one thing, you just set the frequency: Options are 1/2/3/4 MHz, then increments of 4. Datasheet stops at 24. Part still works if you keep incrementing it up to 28, 32. Then it repeats 20-32 again.

There's also a PLL. Only works at >=16MHz internal oscillator speed, max frequency 48, multiply by 2 or 3 (basically, 16 and 24 get 48, 20 gets 40) - too bad all it can do is clock the single Type D timer!

And... there's AUTOTUNE! Use a crystal... but a watch crystal! It's used to adjust the main oscillator frequency. It ain't an MHz crystal at the system clock frequency, but it's better than nothing...

## RTC
Same as megaAVR 0-series
