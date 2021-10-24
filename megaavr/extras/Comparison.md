# Dx-series peripherals vs other modern AVRs
Based on notes from initial reading of datasheets for DA, DB-series parts. Much was unchanged, and essentially all of the changes were welcome.

## NVMCTRL
Dramatic changes. 0/1-series parts erase and write with page-granularity. Dx-series erases with page granularity but writes with word-granularity. The flash protection mechanism now only requires the instruction that results in the actual write or erase (an SPM or ST instruction with NVMCTRL.CTRLA set appropriately) to execute from bootloader section.

## CLKCTRL
Major differences from 0/1-series, and further changes for DB-series.
* For internal oscillator, you just set the frequency: Options are 1/2/3/4 MHz, then increments of 4. Datasheet stops at 24. Part still works if you keep incrementing it up to 28, 32. Then it repeats 20-32 again.
* There's also a PLL, rated for operation with input frequency between 16 and 24 MHz (very conservative, it turns out) and an output frequency 2 or 3 times that (or 4x - known from the earliest I/O headers, but not otherwise documented), but all it can do is clock the type D timer. It also
* You can enable "AutoTuning" from a watch crystal.
* The DB and DD-series also support using an external crystal and have clock failure detection.

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

## EVSYS
Largely unchanged aside from basics needed to adjust for the larger number of channels.

## VREF
Reference voltages are 1.024, 2.048, and 4.096V, as well as 2.5V, Vdd, and external clock. Works the same for AC, DAC, and ADC. The first three are clearly meant to make life easier for people using the ADC - With 12-bit resolution, and 4.096V, 1 LSB is 1mV - It doesn't get any easier than that!
This means Vdd can be used for DAC reference voltage.

## DAC
Only significant difference is that it's 10 bit... and the DATA register is left-adjusted, so if you just want 8 bits, you can just write the one register. DAC vref can also now be set to Vdd so you can get rail to rail output.

## ADC
RESSEL now chooses between 10 and 12 bits, differential ADC (but measured voltage still must not exceed VRef for correct results - no high side current sense using the smallest reference voltage, that will have to wait until 2-series/EA-series parts), and slightly higher maximum accumulation option, much higher maximum sample duration. Specs permit slightly faster ADC clock.

Note that the 2-series tinyAVR has a completely reworked ADC, which will also be featured on the EA-series.

## AC
General layout of megaAVR 0-series, with ACn.DACREF, instead of additional DACn peripherals with no output buffer and DAC0 being shared with AC0's DACREF, and the windowed mode configuration like the "golden" tinyAVR 1-series.

## TWI
No changes from megaAVR 0-series.

## SPI
No changes.

## USART
Largely unchanged, except that one of the RS-485 modes, which was poorly described in the 1-series datasheets, appears to be gone entirely.

## CCL
D-latch works.

## RTC
Same as megaAVR 0-series. Free of the bugs that afflicted the tinyAVR 0/1-series.
