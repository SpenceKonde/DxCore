# Errata
Okay, you know what, I give up

Here's a chart in google sheets (please contact me if you can confirm any ?'s as actually being fixed,because I don't take Microchip's word that a series doesn't have the erratum (if the errata says some revs areeffected, and later ones aren't, you can be confident that that is the case. But what am I to think when it's brothers with other flash size have the bug, and I've seen issues show up in other errata sheets years after the problem was frst added to an errata sheet.))

I've also given up commenting on all but the unofficial and/or confusing ones, and removed old comments, which were in need of updating.

## The list is on [Google Sheets] (https://docs.google.com/spreadsheets/d/1F7n2NNO2quG_Z7WWqgiCBlM0cRfgJtFfuKUZYxUV6DE/edit?usp=sharing)

## Determining silicon rev
Read SYSCFG.REVID; SYSCFG.REVID&0xF0 is the major rev (the letter), SYSCFG.REVID&0x0F is the minor rev.
See [ModernRevSerial](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/DxCore/examples/ModernRevSer/ModernRevSer.ino) for a sample (reads serial number, silicon rev)

## Particularly relevant or noteworthy official errata

### (DA, older DBs) TCD0 on non-default port doesn't work. There are now DBs with this corrected
When PORTMUX.TCDROUTEA is set to anything other than default (PORTA), all four outputs are controlled by the TCD0.FAULTCTRL CMPAEN bit (in other words, only PORTA works for TCD0, unless you want the same output on 4 pins).

### 128DA64 took a beating
Most of these are still not fixed in 128DA hardware...
* TCA1 couldn't output to port E or G (this was a portmux optionthat was onlt available on 64-pin parts, for both of those)
* The 6 pins which were part of a port that was present on the 48-pin parts, but was not itself present on 48 pin parts (PB6-7, PE4-7) were not connected to the event system
* All the 128DA's had (and still have) a bug with writing to the flash using the memory mapping; don't use the memory-mapping to write the flash.

### (DA, older DBs, tinyAVR 2-series) All LUTs enable locked to entire CCL module
The LUTs (all of them) cannot be modified if the CCL (the whole thing) is enabled, instead of just the LUT in question. This is a rather nasty one if you want to use different LUTs for different purposes. This impacts all AVRs with a CCL released as of Q1 2022, but the DD-series fixes it.

### (DA, very old 128DBs) CCL on 32 and 28-pin parts, LINK input to LUT3 does not work
It's not just the owners of 64-pin parts who got special presents from the errata fairy - the LINK input to LUT3 is not connected to the output of LUT0 on the 28 and 32-pin parts (one imagines it's connected to the output of LUT4, except that there is no LUT4-5 on the 28 and 32-pin parts). Most of us probably don't need a LINK input on every LUT, if we're using CCL at all, so this is likely not a problem... annoying, but only that, and quickly fixed on the DB-series.


## Complicated Dx-series errata
These are official erratums/clarifications that are complicated enough to warrant comment.

### TCA restart resets counter direction (Errata *and* datasheet change) DA, DB, tinyAVR 2
It was not initially clear how that differed from the intended behavior...

From Errata:

***Restart Will Reset Counter Direction in NORMAL and FRQ Mode**
When the TCA is configured to a NORMAL or FRQ mode (WGMODE in TCAn.CTRLB is ‘0x0’ or ‘0x1’), a RESTART
command or Restart event will reset the count direction to default. The default is counting upwards.*

This of course leads to some head scratching for those of us who have read the relevant section of the datasheet, which said, amd I quote (though emphasis mine):

*The software can force a restart of the current waveform period by issuing a RESTART command. In this case the counter, **direction**, and all compare outputs are set to 0*

Thus, at the time of the erratum's publication, the datasheet stated that the parts did exactly what the datasheet said they would.

The datasheet was then "clarified" to drop the mention of direction. As corrected silicon had not (in some cases has not) been made available

DB-series parts with the new correct behavior are now available.

### TCB single-shot EDGE bit (Datasheet clarification)
Originally the datasheets described EDGE = 0 as triggering on positive edge, and EDGE = 1 as triggering on negative edges. EDGE = 1 in fact is triggered on both edges (this is better, as you can invert the pin to trigger on the opposite edge if you need to.



## Other issues
### ADC issues

#### AVR DA: ADC disables digital input
*If an input pin is selected to be analog input, the digital input function for those pins is automatically disabled*

In my opinion, poorly described. What they mean is: `While ADC0 is enabled, if ADC0.MUXPOS is set to a value which selects a pin as the positive input to the ADC, the PORTx.IN register is not updated for that pin` This is hardly fatal - and easily worked around - but it is also nearly impossible to ignore if the core does not work around it. The most disconcerting part is that since the core will enable the ADC for you in init(), if it doesn't also work around this, you will immediately find that AIN0 is non-functional as a digital pin! This caused considerable consternation during core development. Despite the frightening-sounding "Workaround: None" in the errata sheet, there is a simple workaround for most use cases - just point the mux somewhere else between reads on impacted parts.

This core implements this simple workaround - on startup, and every every call to analogRead(), the mux is set to 0x40 (GND): `ADC0.MUXPOS=0x40` It is necessary to have started a conversion at least once, though you don't need to complete it, for this to work - the core starts and aborts an ADC conversion.

Just because this is worked around does not mean you can totally ignore it though; there are still a few cases where it becomes relevant.
* If you are taking over the ADC - for example to use free running mode, the window comparator, etc, you must remember than digitalRead() will not work on the pin you are doing analog readings on.
* Pin interrupts will not be triggered on the pin being used for analog readings (even with the workaround, they won't be triggered until AFTER the ADC cycle is completed and we move the ADC mux)
* I have yet to see a case where it was relevant to the Right Way to solve a problem, however. AFAICT, if you're wishing you could digitalRead() the pin you're taking ADC readings from, and you're


#### AVR EA, EB: Missing Codes When Using Internal Reference and CPU is Running
*The ADC has missing codes when selecting internal reference (REFSEL in ADCn.CTRLC) and performing conversions when the CPU runs.*
**Work Around**
*Perform ADC conversions using external reference or keep the device in Idle or Standby sleep mode.* [They mean, while the conversion is running, obviously]

> UGH! IT'S THE RETURN OF ADC SLEEP! I thought we had run that demon through!


I guess what you do is set the sleep mode to IDLE, enable the interrupt and have an empty interrupt that just cleared the intflags and return? I'll probably add a way to do that, since it looks like it's back....

### TWI issues
These seem to have been slow to be detected. One of them is an official erratum.

#### Flush Non-Functional (official, everywhere with a flush command, which is everywhere with dual mode)
The TWI flush command, intended to clear the state machine when it got into a bad state, apparently could instead produce them.

Instead, disable and reenable the TWI peripheral.

#### TWI0 MUX 2 option on DD
The ALT2 mux option may be broken on DD-series parts. Inconsistent behavior has been seen in testing. Sometimes it will work, other times it will drive the SDA line low (to generate the first start condition) and then do nothing more. Attempts to elucidate the nature of this issue have failed.

**Partial workaround**
Use another TWI mapping option. Enable the SMBUS 3.0 levels using the Wire.configSpecialOptions()

#### Dual mode slave only works if dual mode master/slave is enabled (unofficial, apparently universal where dual mode is)
See [Github Issue regarding this, including Microchip quotes](https://github.com/SpenceKonde/DxCore/issues/567)

All things considered, it's arguable if there's ever an appropriate time for it. For wanting to do it, I mean. There's never an appropriate time to *do* it, not with it not working.


### They seemed to be having some difficulties with flash
Uhm.... Yeah

#### Flash endurance 1k, not 10k cycles (all Dx-series parts)
Originally, the flash endurance of AVR Dx-series parts was advertised as 10k rewrite cycles.

About 1 year after release, a "datasheet clarification" was issued for AVR128DA, clarifying that the rewrite endurance was actually 1000 rewrites; the AVR32DA, AVR64DA, and all AVR DB-series datasheets, on their next update, got changed to say 1000 rewrites.  On the one hand, 1000 rewrites implies very intensive development work, or some self programming scheme going berserk, it's not a particularly realistic concern. It app

#### Initial EA silicon had major self-programming issues
* Below 4.3V (BODLEVEL3), programming endurance was drastically degraded.
* Below 2.7V (BODLEVEL2), programming failed entirely.
* Reading from the flash in the section you're supposed to be able to read during a write could trash the write
  * This makes a bootloader rather challenging.
* At one time it was claimed that the problem with the Page Erase Write functionality.
* All serious programming issues fixed with B2/A2 die rev (B2 for 64k, since that's what they developed with and had to go through an entire major rev on before release. No wonder it was so late)).

#### DU-series tried to get RWW working on the Dx-style NVMCTRL
No dice!
```text
Read-While-Write Feature Non-Functional
```


Seems they've gotten most of the problems worked out, finally, other than the Dx-write endurance. 1000 rewrites is still enough though, and supposedly, the Dx-series parts meet the original (10k) spec at room temperature, just not over the whole temperature range (explaining why the problem had not been noticed for over a year)

### Issues that we expected to see in errata, but have not yet seen

#### The Vector Table was Wrong on very early AVR32DA, some of which escaped into the wild; these are not usable.
Microchip recalled them, but some disties had already shipped some out.

Since they never owned up to it in an erratum and didn't increment the die rev when they fixed it, I don't know what date codes are effected. The way you discover if the chip is effected is by trying to use any interrupt - none will work correctly on the effected chips.

It's tested by the simplest sketch around: Blink. Connct an LED and decoupling capacitors, and upload blink. Impacted parts will not blink at a steady rate (possibly not at all) and will behave erratically. Tools -> Millis timer -> Disabled, and repeat upload. On effected parts, blink uploaded with disabled millis should produce a blink normally. The issue is that the chip thinks it's supposed to have 2-byte vectors, while, having more than 8k of flash, it needs 4-byte ones. The compiler generates those, but instead of jumping to (vector number * 4), it jumps to (vector number * 2), so half of the vectors land in the middle of an instruction word, and the other half land on a vector for another interrupt, which is probably an ISR you haven't defined, meaning BAD_ISR which results in a reset.

#### TCB Async events are slower than Sync events
See [the discussion topic, which quotes Microchip support](https://github.com/SpenceKonde/megaTinyCore/discussions/735) as saying that it likely effects all parts with a TCB. (this was some time ago, and it's not been seen on errata lists, though)

### Issues not expected to be seen as errata or datasheet clarifications

#### (DA/DB) Fully vs Partially async pin behavior inconsistent with datasheet w/rt "dead time"
The description of fully vs partially async pins in the datasheet is muddled and, as best as I can tell, backwards. It also doesn't matter on newer parts, only DA/DB, as there is no longer anything other than fully async pins.
The actual behavior I observed is:
* The dead time is *the time after clearing an interrupt flag before it will become set again*.
* At only 2-3 clock cycles (a typical sync delay), it's not relevant to ~most~ any cases when actually using interrupts, as the time it takes to enter or exit the interrupt exceeds this delay several times over.
* The dead time is seen on fully async pins, not partially async pins. This is the opposite of what the documentation says.

Nobody is sad to see the fully/partially async pin distinction go.

### SPM does not ignore low bit of address
Despite the datasheet saying otherwise *(and saying so very explicitly)*, the lowest bit of the address is **not** ignored when self-programming the flash using the SPM instruction. Actual behavior seen is:
* Except at the boundary between pages, the low bit is treated as one might naively expect - you can do non-word aligned writes just fine!
* If address provided is the last byte in a page, however, the write malfunctions:
  * The high byte will be written at the first address in the next page (as one would naively expect)
  * The low byte is written to the second address in the next page.
  * The supplied address is not written at all.
* This bug is not hard to avoid. But it looks like you don't have to if you go playing around - until you do. So, no, don't SPM to odd addresses.
* If you want to write bytes 5 and 6 to the values 0x55 and 0x66 respectively, you instead can do this:
  * SPM 0xFF, 0x55 to address 4 and 5, then SPM 0x66, 0xFF to address 6, 7.
  * If flash is not erased prior to a write, the value in flash following the write is the bitwise and of the previous value and the value written. Hence writing 0xFF to the irrelevant bytes. This (writing all 1's to an unerased byte to leave it unchanged) works on just about any flash memory that has a separate page erase command, as it's based on the physical constraints on solid state memory.

My only thought here is "If SPM instruction requires word aligned access, why not just require writes to be word addressed, and then you could forget about rampz except for the sole case of ELPM, and everone's life would be simpler". I would have no objection to losing ST/STS writes to the flash if it meant that I could take RAMPZ out of the equation while writing to the flash. Hell, imagine if ELPM assumed RAMPZ = 1 and LPM to assume RAMPZ = 0, and all the headaches caused by RAMPZ would be gone!

### UPDI programming issue with 16-bit STS after 24-bit STptr (Datasheet should be clarrified)
On parts which use 24-bit addressing, 16-bit addressing should not be used for STS operations. The results are profoundly baffling - the long and short of it is: See the table in the UPDI chapter, titled `Figure xx-10: STS Instruction Operation`? Cross out the top two lines. If STptr has been used with a 24-bit pointer (which you normally do to write the flash), if a subsequent STS doesn't use the 24-bit address, it will use the high byte from ptr! This behavior is apparently intended and expected, but that isn't communicated to the reader of the datasheet.

## Errata and Datasheets
Get the most up to date information from Microchip's website. They keep moving files around, so I'm just going to link their product pages.
[AVR DA-series](https://www.microchip.com/en-us/products/microcontrollers-and-microprocessors/8-bit-mcus/avr-mcus/avr-da)
[AVR DB-series](https://www.microchip.com/en-us/products/microcontrollers-and-microprocessors/8-bit-mcus/avr-mcus/avr-db)
[AVR DD-series](https://www.microchip.com/en-us/products/microcontrollers-and-microprocessors/8-bit-mcus/avr-mcus/avr-dd)
[AVR EA-series](https://www.microchip.com/en-us/products/microcontrollers-and-microprocessors/8-bit-mcus/avr-mcus/avr-ea)
[AVR DU-series](https://www.microchip.com/en-us/products/microcontrollers-and-microprocessors/8-bit-mcus/avr-mcus/avr-du)
[AVR EB-series](https://www.microchip.com/en-us/products/microcontrollers-and-microprocessors/8-bit-mcus/avr-mcus/avr-eb)


## Interesting things removed from early io headers

### Dx-series

Between the initial releases of the io headers (eg, `ioavr128da64.h`), and more recent ones, of course, they corrected an assortment of errors, typos, missing information - the usual... And also, it would appear, the accidental inclusion of references to features not described my the datasheet? Nothing **SUPER** interesting, but...

#### EVSYS_USEROSCTEST

```c++
#define EVSYS_USEROSCTEST  _SFR_MEM8(0x024B)
```
An extra event user... OSCTEST? I wonder what it does! I guess there's only one way to find out, and that's to monitor the system clock carefully with that set, and then trigger the event! The real question is whether it is to test some special oscillator feature... or just test if the oscillator is "safe" at it's current speed by stressing it (and you'd want that to be an event so you can have it executing code maybe? I dunno!)  Someone go try it out and let me know if they can make it do anything....

I bet the reason it's an event user is so they can trigger and untrigger it without CPU involvement

#### CLKCTRL_MULFAC_4x_gc
This next one is the bitfield that controls what the PLL will multiply the input clock by...
```c++
/* Multiplication factor select */
typedef enum CLKCTRL_MULFAC_enum
{
    CLKCTRL_MULFAC_DISABLE_gc = (0x00<<0),  /* PLL is disabled */
    CLKCTRL_MULFAC_2x_gc = (0x01<<0),  /* 2 x multiplication factor */
    CLKCTRL_MULFAC_3x_gc = (0x02<<0),  /* 3 x multiplication factor */
    CLKCTRL_MULFAC_4x_gc = (0x03<<0),  /* 4 x multiplication factor */
} CLKCTRL_MULFAC_t;
```
That last line was removed in later versions, to match the datasheet... but it does indeed work!

#### AC_POWER_PROFILE3_gc
Analog Comparators had a third option for power profile planned... The higher the option, the lower the power consumption, but the higher the latency. If I had to place a bet on it, my money would be that like the 4x multiplier, the third power profile was removed only from the headers, and that it's still in the silicon - but it may not work so well.
```c++
    AC_POWER_PROFILE3_gc = (0x03<<3),  /* Power profile 3 */
```
This is once more listed on the AVR DD and EA-series headers, so it looks like they got it sorted out during the intervening time. One imagines that the option is there on DA/DB parts, but just doesn't function correctly.

### USART_RS485_gm
Finally, the USART had the two RS485 modes listed, like tinyAVR 0/1-series, instead of just the one. The second mode raised all sorts of questions about how the chip ought to behave - and was one of those terse-and-useless sections that answers little. If I correctly understand the intent - it was meant to listen to XDIR and only output data when XDIR was in the correct state. So what does it do when the pin transitioned to "no sending" state while it was in the middle of a byte? Is there any answer to that that will result in correct behavior in all applicable use-cases? I think they thought about that one, realized the answer was "probably not" after they re-read what they'd said about it in the datasheets of the parts that had been released a few times, and figured that they'd be better off not supporting that. It is unclear whether the feature is still present in the silicon. On the DA-series I suspect it is, and likely DB as well, as they don't appear to have touched the USART.

## Ex-series
Much less interesting... only one thing that wasn't supposed to be there was apparently left in, a section of NVMCTRL that configures the EEPROM writes. Oddly, the initial silicon's NVMCTRL was badly broken - but the EEPROM worked fine, it was the flash that was busted.
