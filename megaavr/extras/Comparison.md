# Dx-series peripherals vs other modern AVRs
Based on notes from the Dx-series and Ex-series datasheets.

## Errata
Very few issues were fixed on the DA-series, and numerous new ones were added. The DD series has improved dramatically. Unfortunately, they have not been giving us the promised die revisions.

## NVMCTRL
* Dramatic changes. 0/1-series parts erase and write with page-granularity. Dx-series erases with page granularity but writes with word-granularity. The flash protection mechanism now only requires the instruction that results in the actual write or erase (an SPM or ST instruction with NVMCTRL.CTRLA set appropriately) to execute from bootloader section.
* On the Ex-series, this has reverted to more tiny-like behavior, but with the addition of RWW/NRWW sections.

## CLKCTRL
Major differences from 0/1-series, and further changes for DB-series.
* For internal oscillator, you just set the frequency: Options are 1/2/3/4 MHz, then increments of 4. Datasheet stops at 24. Part still works if you keep incrementing it up to 28, 32. Then it repeats 20-32 again.
* There's also a PLL, rated for operation with input frequency between 16 and 24 MHz (very conservative, it turns out) and an output frequency 2 or 3 times that (or 4x - known from the earliest I/O headers, but not otherwise documented), but all it can do is clock the type D timer.
* Both the system clock and PLL can be overclocked to an almost unbelievable degree, particularly on extended temperature range parts when used at room temperature. 2x the rated system clock with an external oscillator on a DB with extended temp range is possible. The PLL seems able to do at least than 2.67 times what it is rated for, though 3.33 is apparently asking too much.
* You can enable "AutoTuning" from a watch crystal.
* The DB and DD-series also support using an external crystal and have clock failure detection which does not work all that well.
* No significant changes between DB and DD.
* All announced Ex-series parts appear to return to a more tiny-avr like clock subsystem, only with the addition of autotune.

## TCA
Second event channel, EVACTB added. Otherwise, appears identical, complete with the wonky CTRLExxx CMDEN bits in split mode, with values of NONE (0b00) and BOTH (0b11), but the other two marked reserved.

The tinyAVR 2-series also got the new version.

## TCB
The only timer with real and meaningful improvements. These have gone from great utility timers to amazing ones, with a few additions:
* Added EVENT clock option - now TCB as well as TCA can be clocked from events.
* They got an overflow interrupt too! Though in most modes it is redundant and/or only fires in very unusual conditions.
* Cascade functionality - when one timer is clocked from the others' overflow event generator, there is a cascade bit that can be set, which will permit 32-bit input capture. and set cascade on the one that counts the two MSBs to make sure they are in sync (ie, correct for propagation delays) and get a 32-bit input capture.  You can time something lasting nearly 3 minutes (at 24 MHz system clock) down to single processor clock cycles.
* TCA1 clock option on parts that have it (obviously)

tinyAVR 2-series also got the new version (without the TCA1 clock option of course,)

## TCD0
Only difference appears to be the addition of the PLL as a clock source. It looks like in general everyone ignored the TCD until the Dx-series came out - there was hardly any errata listed for it until the year following the Dx-series release, but it has now grown significantly, with the same bugs appearing both here and for Dx-series.

## TCE and TCF
Two new timers appear on the AVR EB-series parts. Very little is known about them other than that TCE has something called a Waveform Extension (WEX), is 16-bit, and lists pin mappings with 8 waveform outputs. TCF is 24-bit and has two compare channels. The EB-series has one of each.

## EVSYS
* Largely unchanged from megaAVR 0-series or tinyAVR 2-series.
* Major changes from tinyAVR 0/1-series: Now the user channels are named, not numbered, and all event channels work for both synchronous and asynchronous inputs and outputs.

* Ex-series brings a major change. All generator channels get two options for each port on the device, as well as two options for the RTC. These are channels 0 and 1 and are configured by the new EVGENCTRL register found on the ports and the RTC. This means *all generator channels are created equal*.

## VREF
* Reference voltages are 1.024, 2.048, and 4.096V, as well as 2.5V, Vdd, and external clock. Works the same for AC, DAC, and ADC. The first three are clearly meant to make life easier for people using the ADC - With 12-bit resolution, and 4.096V, 1 LSB is 1mV - It doesn't get any easier than that!
This means Vdd can be used for DAC reference voltage.

## DAC
Only significant difference is that it's 10 bit... and the DATA register is left-adjusted, so if you just want 8 bits, you can just write the one register. DAC vref can also now be set to Vdd so you can get rail to rail output.

## ADC
* RESSEL now chooses between 10 and 12 bits, differential ADC (but measured voltage still must not exceed VRef for correct results - no high side current sense using the smallest reference voltage, that will have to wait until 2-series/EA-series parts), and slightly higher maximum accumulation option, much higher maximum sample duration. Specs permit slightly faster ADC clock.

* The 2-series tinyAVR has a completely reworked ADC, which will also be featured on the EA-series with small changes. The ADC clock can be significantly higher, accumulation of up to 1024 samples is supported. They now feature a programmable gain amplifier (1-16x), and the differential mode does not require that all inputs be less than VREF (this is required on the Dx-series and kills many of the applications of the differential ADC).

## AC
* General layout of megaAVR 0-series, with ACn.DACREF, instead of additional DACn peripherals with no output buffer and DAC0 being shared with AC0's DACREF, and the windowed mode configuration like the "golden" tinyAVR 1-series.

## TWI
* No changes from megaAVR 0-series aside from the pin mapping options, and the addition of the SMBus logic level option.

## SPI
* No changes aside from the pin mapping options.

## USART
* Largely unchanged, except that one of the RS-485 modes, which was poorly described in the 1-series datasheets, appears to be gone entirely.

## CCL
* D-latch works.
* The input list has been adjusted somewhat.
* Interrupts, not supported on tinyAVR 0/1-series, are supported on Dx-series and Ex-series.

## PORT
* Dx and Ex-series parts feature a facility for mass-configuration of PINnCTRL registers not found on tinyAVR.
* Ex-series add an EVGENCTRL register to each port. Each nybble, independently sets which pin within the port is used for each port's two event generation channels.
* Starting from the DD-series, it seems that now all pins will be fully async instead 2 fully async and 6 partially async pins per port.

## RTC
* Same as megaAVR 0-series. Free of the bugs that afflicted the tinyAVR 0/1-series.
* Ex-series add an EVGENCTRL. Each nybble, independently sets which division factor to use for the RTC's two event generation channels.
