# Errata
The errata for the DA128 parts was a rather depressing document. A large number of issues were present, many of them rather serious. Reasonably complete errata sheets were not available until 8 months after release - and the errata sheet does a poor job of explaining some of the most important issues that it does mention. Where a revision is named, that is the last revision which exhibits this errata.
As hardware which is not impacted by these issues becomes available, we will provide methods to determine the "silcon revision" and hence whether a part is effected.

Note that as new errata have been discovered, not all of them have made it into here.

Issue                          | Severity | Source    | 128DA | 64DA | 32DA    |  128DB  | 64DB  | 32DB  | 64DD  | 32DD  | 16DD  | Notes                                |
--------------------------------------|---|-----------|-------|------|---------|---------|-------|-------|-------|-------|-------|--------------------------------------|
Increased Current if Vdd falls < 2.1V | 3 | Microchip | NO?   | NO?  | NO?     | YES     | YES   | YES   |       |       |       |                                      |
Violating maximum Vdd slew can damage | 3 |           | ???   | ???  | ???     | YES     | ???   | ???   |       |       |       | Probably impacts all Dx-series parts.|
BOD registers not reset if UPDI enable| 1 | Microchip | YES   | YES  | YES     | A4      | NO    | NO    |       |       |       | POR needed for them to be reset      |
Vector table is wrong                 | 5 | AVRFreaks | NO    | NO   |RECALLED | NO      | NO    | NO    |       |       |       | Impacted chips recalled, incident hushed up |
CRC check on reset non-functional     | 1 | Microchip | A6    | A3   | A3      | A4      | NO    | NO    |       |       |       |                                      |
DAC drifts long term if no buffer     | ? | Microchip | YES   | YES? | YES?    | YES     | YES?  | YES?  |       |       |       | Not quantified; Unclear how bad      |
ADC disables digital input            | 2 | Microchip | YES   | YES  | YES     | NO      | NO    | NO    |       |       |       | Likely fixed for DD                  |
Memory mapped flash issues            | 1 | Microchip | YES   | NO   | N/A     | NO      | NO    | N/A   |       | N/A   | N/A   | Largely a non-issue, just write by word |
SPM does not ignore low bit of address| 1 | DxCore    | YES   | YES  | Yes     | YES     | YES   | Yes   |       |       |       | Unacknowledged by Microchip.         |
Multipage erase can erase protected   | 1 | Microchop | YES   | YES  | YES     | YES     | YES   | YES   |       |       |       | Only applies to later sections of memory. Non-issue for Arduino |
TCA restart resets counter direction  | 2 | Microchip | YES   | YES  | YES     | YES     | YES   | N/A   |       |       |       | The datasheet says this is intended! |
TCB CCMPH/CCMPL act as 16-bit in PWM  | 2 | Microchip | YES   | YES  | YES     | YES     | YES   | YES   |       |       |       | Has impacted all modern AVRs         |
No TCA1 Remap on PORTE/G              | 4 | Microchip | YES   | NO   | N/A     | NO      | NO    | N/A   | N/A   | N/A   | N/A   | This is a real pain now that we support alt mappings! |
TWI Pins must be LOW                  | 1 | Microchip | YES   | YES  | YES     | YES     | NO??  | NO??  |       |       |       | Has impacted all modern AVRs         |
SPI SSD only works on alt pins        | 2 | Microchip | YES   | NO?  | NO?     | NO?     | NO?   | NO?   |       |       |       |                                      |
USART Open Drain TX must be INPUT     | 1 | Microchip | YES   | YES  | YES     | YES     | YES   | YES   |       |       |       | Has impacted all modern AVRs         |
USART start of frame detect in active | 3 | Microchip | YES   | YES  | YES     | YES     | YES   | YES   |       |       |       | Easily worked around, **if you know**|
TWI SDA Hold Times                    | 1 | Microchip | YES   | NO   | NO      | NO      | NO    | NO    |       |       |       |                                      |
TWI Flush non-functional              | 2 | Microchip | YES   | YES  | YES     | YES     | YES   | YES   |       |       |       | This feature not exposed in wire because of this |
ZCD Output remapping broken           | 1 | Microchip | YES   | YES  | YES     | A4      | NO    | NO    | N/A   | N/A   | N/A   | Can be worked around with events     |
No Event on PB6,7 PE4,5,6,7           | 3 | Microchip | YES   | NO   | N/A     | NO      | NO    | N/A   | N/A   | N/A   | N/A   | Use different pins.                  |
All CCL LUTs enable-locked to CCL     | 3 | Microchip | YES   | YES  | YES     | YES     | YES   | YES   |       |       |       | Has impacted all modern AVRs         |
CCL3 on 32/28-pin no LINK input       | 2 | Microchip | YES   | YES  | YES     | A4      | NO    | NO    |       |       |       | Fixed quickly, probably easy fix     |
Initial fuses don't match datasheet   | 1 | Microchip | A6    | A3   | A3      | A4      | NO    | NO    |       |       |       | No practical implications            |
TCD count prescale breaks async input | 1 | Microchip | YES   | YES  | YES     | YES     | YES   | YES   |       |       |       | Has impacted all modern AVRs         |
RUNSTBY broken for 32k external       | 2 | Microchip | NO?   | NO?  | NO?     | A4      | NO    | NO    |       |       |       | Apparently DB only and fixed fast    |
Ext. Clock status bit broken for ext. | 2 | Microchip | NO?   | NO?  | NO?     | A4      | NO    | NO    |       |       |       | Likely related to above              |
PD0 input buffer floating             | 1 | Microchip | NO    | NO   | NO      | YES     | YES   | YES   |       |       |       | Automatically worked around by core  |
TCD0 halt+wait for SW with CMPA = 0   | 1 | Microchip | YES   | YES  | YES     | YES     | YES   | YES   |       |       |       | Broken in dual slope or CMPA not used|
TCD0 portmux options broken           | 4 | Microchip | YES   | YES  | YES?    | YES     | YES   | YES   |       |       |       | Intensely frustrating!               |
ADC increased offset in single-ended  | 5 | Microchip | N/A   | N/A  | N/A     | A4      | NO    | NO    |       |       |       | Very nasty bug, fixed really fast    |
OPAMP power consumption 3x higher     | 2 | Microchip | N/A   | N/A  | N/A     | A4      | NO    | NO    | N/A   | N/A   | N/A   | Opamp being this far off spec...     |
OPAMP IRSEL bit read-only             | 2 | Microchip | N/A   | N/A  | N/A     | A4      | NO    | NO    | N/A   | N/A   | N/A   | ... was not a good look. Fast fix.   |
PLLS bit always 0 unless requested    | 1 | Microchip | YES   | YES  | YES     | YES     | YES   | YES   |       |       |       | As long as you know, non-issue       |
PLL doesn't work from ext. xtal       | 1 | Microchip | N/A   | N/A  | N/A     | YES     | YES   | YES   |       |       |       |                                      |
Flash endurance 1k, not 10k cycles    | 3 | Microchip | YES   | YES  | YES     | YES     | YES   | YES   |       |       |       | And it looks like a "won't fix"?     |


NO? means not mentioned in errata, but has not been confirmed as not being present in that size of chip.
NO?? means the same, but I am particularly doubtful.
YES?
N/A for the 32DA: There's no AVR32DA64, hence the 64-pin-only issues don't apply. The flash mapping is likewise not an issue there as they only have one section of flash.

## Severity scale
From Arduino perspective. Totally subjective and my own judgement.

1. Nobody in arduino land likely to care: Either it is worked around transparently by core+libraries, and/or the feature impacted is exotic.
2. Worked around in core and/or only rarely impacts users
3. Cannot be full worked around by core or user code, but also unlikely to cause severe problems for users.
4. No viable workaround, and could compromise the viability of the part for applicable use cases
5. Critical. The device is not usable for common tasks. These have all been fixed very rapidly.

The issues I consider 4's are the two remapping issues, one of which is impacting all DA/DB (TCD), while the other only effects AVR128DA64.

Alas - over a year after the release of the DA-series, we have seen only the AVR128DB get a new silicon rev that fixed a significant number of issues.

## Determining silicon rev
Read SYSCFG.REVID; SYSCFG.REVID&0xF0 is the major rev (the letter), SYSCFG.REVID&0x0F is the minor rev.
See [ModernRevSerial](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/DxCore/examples/ModernRevSer/ModernRevSer.ino) for a sample (reads serial number, silicon rev)

## Descriptions
Microchip's description in italics, if included. I've generally refrained from including their text here.

### Increased Current if Vdd falls < 2.1V
*The device may experience increased current consumption of approximately 1.5 mA if VDD drops below 2.1V and is
held in the range 1.9-2.1V. This will only occur if VDD is originally at a higher level and then drops down to the
mentioned voltage range.
**Work Around**
Ensure VDD is always kept above 2.1V by setting the BOR trigger level to 2.2V to keep the device from executing if
VDD drops towards the affected voltage range. If operation in voltage range 1.9-2.1V is required, make sure VDD does
not rise above 2.1V and then drops down again. Note that the voltage levels given are not absolute values but typical
values.*

Note that the only matters if you care about 1.5mA power consumption. Of course, if you're operating in that voltage range, you probably do.

This suggests some things about the internal voltage reglator....

### Extremely rapidly rising Vdd can cause damage
This was apparently discovered by customers who don't know the meaning of "board level decoupling", and built devices with just the originally spec'ed 0.1uF caps, and supplied power by shorting the supply rails to a 5v supply with lots of capacitance through very short wires/traces (think thumb-drive form factor, pluggest straight into a USB port on a laptop, which will typically have the USB ports directly on the motherboard, with a bunch of high value, low ESR capacitors across the 5v rail. The voltage could ramp up so quickly that the internal regulator could overshoot the target voltage and damage the chip. Not using board-level decoupling caps is inconsistent with electronics best practices. I think the only boards I've ever made without at least 4.7uF on the board were the Ultramini's that would plug into a DIP socket, and even those I put 1uF on...

I speculate that this is what led to the next version of the datasheet specifying 1.0 uF caps instead of 0.1uF caps for decoupling - these would be sufficient to slow down the slew rate down. But that wasn't really satisfactory, and they revised the datasheet again to specify a 1uF cap on the board plus 0.1uF caps on each supply pin.


### BOD registers not reset if UPDI enable
*If the UPDI is enabled, the VLMCTRL, INTCTRL, and INTFLAGS registers in BOD will not be reset by other reset sources than POR*

"If the UPDI is enabled" means that the UPDI peripheral is actually running. Only particularly relevant because applies when the programmer resets the chip. So those registers are not reset until the programming session has ended and the chip is powercycled. Could be absolutely maddening to figure out in the event that you found yourself in this corner case, but very few people will trip over it...


### Vector Table is Wrong on early AVR32DA
Remember how the ATmega808 and 809 had 4-byte interrupt vectors in hardware, but for a while the toolchain tried to generate 2-byte vector tables for it and interrupts didn't work? Well this was worse: It was the other way around! Not only does the toolchain generate binaries that don't work, it cannot be fixed with a toolchain update, because the hardware only did a 2-byte vector which couldn't reach the whole flash. **Impacted AVR32DA parts are not usable**. They were recalled, and were only available for a short time and if anyone has impacted parts, they can get replacements through support - not that I think it would be worth the it would take  according to a Microchip employee posting on the AVRFreaks forum. It appears that they have decided to pretend this never happened - the fact that they shipped product with a bug that renders them completely unusable is still not mentioned on the silicon errata (and was fixed without incrementing the silicon revision - does that mean that 2-byte vs 4-byte vectors is set at factory calibration? Or that they were just trying to hush this one up). It really raises some questions about their test procedures (They produced a new chip, packaged them, and shipped them, advertising them as fit for use in life safety critical applications without having tested even trying to run the equivalent of blink on them? - AND it was a screwup that they did the opposite of on the smallest flash version of the prior product line, so they should have been specifically checking for this. It does seem to have woken up their QA people - ever since then, there have been more delays, and shorter errata lists for new parts). Effected chips were pulled and did not become available again for several months. If you bought DA32 chips in the fall of '20 and they don't work right, this is why

### ADC disables digital input
*If an input pin is selected to be analog input, the digital input function for those pins is automatically disabled*

In my opinion, poorly described. What they mean is: `While ADC0 is enabled, if ADC0.MUXPOS is set to a value which selects a pin as the positive input to the ADC, the PORTx.IN register is not updated for that pin` This is hardly fatal - and easily worked around - but it is also nearly impossible to ignore if the core does not work around it. The most disconcerting part is that since the core will enable the ADC for you in init(), if it doesn't also work around this, you will immediately find that AIN0 is non-functional as a digital pin! This caused considerable consternation during core development. Despite the frightening-sounding "Workaround: None" in the errata sheet, there is a simple workaround for most use cases - just point the mux somewhere else between reads on impacted parts.

This core implements this simple workaround - on startup, and every every call to analogRead(), the mux is set to 0x40 (GND): `ADC0.MUXPOS=0x40` It is necessary to have started a conversion at least once, though you don't need to complete it, for this to work - the core starts and aborts an ADC conversion.

Just because this is worked around does not mean you can totally ignore it though; there are still a few cases where it becomes relevant.
* If you are taking over the ADC - for example to use free running mode, the window comparator, etc, you must remember than digitalRead() will not work on the pin you are doing analog readings on.
* Pin interrupts will not be triggered on the pin being used for analog readings (even with the workaround, they won't be triggered until AFTER the ADC cycle is completed and we move the ADC mux), though this is not a recommended configuration.

### SPIn.SSD required for master mode in default SPI pin mapping
If the SSD (slave select disable) bit is not set, if an SPI interface is used with the default pin mapping, master mode will not be operable. Not much of a concern for Arduino users, since the only SPI library around for the modern AVR parts only supports master mode and always sets this bit.

### USART Open Drain mode
Yeah, you can't have TX pin as OUTPUT if using open drain mode, because it can drive the pin high. Setting it INPUT works. *Just like on every other part with the new peripherals since their introduction*. They should really just amend the datasheets and remove it from the errata, and pretend they meant it to work like this if they're not going to fix it.

### USART start of frame detect in active mode
***Start-of-Frame Detection Can Unintentionally Be Triggered in Active Mode**
The Start-of-Frame Detection feature enables the USART to wake up from Standby sleep mode upon data reception.
The Start-of-Frame Detector can unintentionally be triggered when the Start-of-Frame Detection Enable (SFDEN)
bit in the USART Control B (USARTn.CTRLB) register is set, and the device is in Active mode. If the Receive
Data (RXDATA) registers are read while receiving new data, the Receive Complete Interrupt Flag (RXCIF) in the
USARTn.STATUS register is cleared. This triggers the Start-of-Frame Detector and falsely detects the following
falling edge as a start bit. When the Start-of-Frame Detector detects a start condition, the frame reception is
restarted, resulting in corrupt received data. Note that the USART Receive Start Interrupt Flag (RXSIF) always is ‘0’
when in Active mode. No interrupt will be triggered.*

**Work Around**
*Disable Start-of-Frame Detection by writing ‘0’ to the Start-of-Frame Detection Enable (SFDEN) bit in the USART
Control B (USARTn.CTRLB) register when the device is in Active mode. Re-enable it by writing the bit to ‘1’ before
transitioning to Standby sleep mode. This work around depends on a protocol preventing a new incoming frame
when re-enabling Start-of-Frame Detection. Re-enabling Start-of-Frame Detection, while a new frame is already
incoming, will result in corrupted received data*

Not a big deal IF YOU KNOW ABOUT IT. Would be an absolute nightmare to debug if you hadn't read the errata.

One very disturbing thing about this erratum is that it has appeared and then disappeared from multiple errata sheets for other part families.

### TCA1 remap options 2 and 3 not functional
Remapping TCA1 to PORTE and PORTG does not work. Note that this is only relevant for 64-pin parts, as the effected pins are not present on 48-pin parts. Kind of a bummer though that we can't get more use out of PORTG! This issue is only present on the DA however.

### Event System not connected to PB6, PB7, PE4, PE5, PE6, PE7
Another slap-in-the-face for 64-pin DA users: 6 of the pins only present on the 64-pin versions aren't enabled for the event system. Of course, you can always use them for other things (just not PWM output, per above)... Looks like they did all their testing on the DA48 and barely touched the DA48.

### TCD0 on non-default port doesn't work
When PORTMUX.TCDROUTEA is set to anything other than default (PORTA), all four outputs are controlled by the TCD0.FAULTCTRL CMPAEN bit (in other words, only PORTA works for TCD0, unless you want the same output on 4 pins...)

### Halting TCD and Wait for SW Restart Does Not Work if Compare Value A is 0 or Dual Slope Mode is Used
*Halting TCD and wait for software restart (INPUTMODE in TCDn.INPUTCTRLA is ‘0x7’) does not work if compare
value A is 0 (CMPASET in TCDn.CMPASET is ‘0x0’) or Dual Slope mode is used (WGMODE in TCDn.CTRLB is
‘0x3’).*
**Work Around**
*Configure the compare value A (CMPASET in TCDn.CMPASET) to be different from 0 and do not use Dual Slope
mode (WGMODE in TCDn.CTRLB is not ‘0x3’)*

Unlikely that any Arduino people will get that deep into TCD to care, and dual-slope mode is the least useful mode.


### ZCD Output remapping non-functional
The ZCD0 bit in PORTMUX.ZCDROUTEA effects all three ZCD's, not just ZCD0, the other two bits do nothing. If you were counting on having more than one output from a ZCD, is a modest inconvenience (you can make it up with event system). Likely few if any Arduino folks are interested in this anyway.

### All LUTs enable locked to entire CCL module
The LUTs (all of them) cannot be modified if the CCL (the whole thing) is enabled, instead of just the LUT in question. This is a rather nasty one if you want to use different LUTs for different purposes. This impacts all AVRs with a CCL as of Q1 2022. If it's ever fixed, it will enable a lot of cool stuff.

### CCL on 32 and 28-pin parts, LINK input to LUT3 does not work
It's not just the owners of 64-pin parts who got special presents from the errata fairy - the LINK input to LUT3 is not connected to the output of LUT0 on the 28 and 32-pin parts (one imagines it's connected to the output of LUT4, except that there is no LUT4-5 on the 28 and 32-pin parts). Most of us probably don't need a LINK input on every LUT, if we're using CCL at all, so this is likely not a problem... annoying, but only that, and quickly fixed on the DB-series.

### TWI pin output override not working correctly
The TWI is able to override the output data direction - but not the output value. Hence, the pins used to TWI must not be set HIGH otherwise the peripheral cannot drive them low (note that the inverse is not a problem - TWI is an open drain bus, and the devices on it only ever drive the lines low, or let them be pulled up).

DxCore has implemented a workaround in the Wire library which makes sure the relevant pins are always set LOW when the TWI peripheral is initialized.

### Flash mapping does not work correctly
When accessing the flash via mapping to data memory (according to the FLMAP bits in NVMCTRL.CTRL), the inter-section flash protection (which prevents the application from scribbling over itself or the bootloader) fails to account for the FLMAP bits when deciding if access to an address is to be blocked. This means that the BOOT section is "mirrored" across all sections (presumably likewise with the other sections, which probably means that appdata section is unwritable from appcode section too, unless the appdata section includes flash in the first 32k). If reading of the bootloader section is disabled, that is similarly effected (otherwise, reading works fine). Thankfully this only impacts access using LD/ST instructions - you can still write and read from everywhere you're supposed to be able to using SPM and LPM (though unless BOOTRP is set, reading is not impacted)) - which is of course considerably less convenient; this is accounted for by our Flash library and writing by word is twice as fast anyway. Application code reading from the flash would not be impacted unless the bootloader locked down reads of the bootloader section. And I don't think most of us use memory mapping anyway... Only impacts the AVR128DA, but already worked around, so a non-issue. The biggest impact is it means nobody will use byte writes, only word writes, and it doesn't matter if they fix it, because nobody's going to go back and rewrite that code.

### Flash multipage erase can erase protected sections
*When using Flash Multi-Page Erase mode, only the first page in the selected address range is verified to be within
a section that is not write-protected. If the address range includes any write-protected Application Data pages, it will
erase them*

Non-issue for almost everyone. What this means is that if the last 31 pages are app data, and APPDATAWP is set but APPCODEWP isn't, and while executing from the BOOTCODE section (otherwise APPCODE won't ever be writable - so you would need to have a "fake bootloader" that you can call from the application (ex, Flash.h with SPM from app = anywhere), or you're using a custom bootloader to upload which both sets APPDATAWP and uses multi-page erase, AND isn't smart enough to not try to erase your application data like this), if you attempt to perform a 32-page erase on one of the last 32 pages, it will erase those pages anyway, because it only checked the first page.

### SPM does not ignore low bit of address.
Despite the datasheet saying otherwise (and saying so very explicitly), the lowest bit of the address is **not** ignored when self-programming the flash using the SPM instruction. Except at the boundary between pages, the low bit is treated as one might naively expect - you can do non-word aligned writes just fine! If address provided is the last byte in a page, however, the high byte will be written at the first address in the next page... and the low byte to the second address in the next page! If the last bit was ignored, we would expect the the two bytes to be written to the last two bytes of the page, starting 1 byte before the supplied address. Note that our flash-writing libraries only write to even byte addresses, working around this while also using only SPM to write (to workaround above issue) by taking advantage of the fact that writing 0xFF to a non-erased byte will not change them to provide a way to effectively write to odd addresses without issue - this is only a concern if you write your own bootloader or flash-writing routines.

My only thought here is "If SPM instruction requires word aligned access, why not just require writes to be word addressed, and then you could forget about rampz except for the sole case of ELPM, and everone's life would be simpler". I would have no objection to losing ST/STS writes to the flash if it meant that I could take RAMPZ out of the equation while writing to the flash. Hell, imagine if ELPM assumed RAMPZ = 1 and LPM to assume RAMPZ = 0, and all the headaches caused by RAMPZ would be gone!

### Pre-set fuses don't match datasheet
The fuses, as supplied (on AVR128DA, Rev. A6 silicon, at least) do not match the configuration described in the datasheet. Generally speaking, unused bits are set to 1 instead of 0 (except in a few cases), and in the case of OSCCFG, a "reserved" combination is selected.
* BODCFD is set to 0x10, per datasheet this is an invalid BOD level setting. Should not impact functionality, as BOD is disabled by the low 5 bits, and the high bits would be reset if you enabled BOD anyway.
* OSCCFG is set to 0x78, per datasheet low nybble should be 0 or 1, high nybble all 0's (unused). All options other than 0 and 1 for low 4 bits are marked reserved. I guess bit 3 doesn't matter so much?
* SYSCFG0 is set to 0xF2 instead of 0xC8. While we might not care what CRC algorithm the disabled CRC check is using, the fact that it disables reset is certainly disconcerting to discover...
* SYSCFG1 is set to 0xF8 instead of 0x00 - just a case of unused bits set 1 instead of 0.

Certainly makes you wonder about their emphatic "unused bits MUST be set to 0" warning doesn't it? Burn bootloader when you get the parts to make reset work.
On DB-series parts, this only effects parts with a datecode of week 21 or earlier. The same is likely true of DA-series as well, since this can be corrected with just a change to their factory programming process.

### ~UPDI programming issue with 16-bit STS after 24-bit STptr~
It was related to me that this is intended behavior. In which case, the documentation of this fact is utterly abysmal! By which I mean it isn't documented at all.
When programming over UPDI, there are two ways to write and read memory, with direct and indirect addressing. Both of them can use 16 or 24-bit addressing (or 8-bit, but this is useless). The documentation describes the ST (indirect) pointer as being only used for ST/LD, not for STS/LDS. However, if an ST instruction sets the indirect addressing pointer to a 24-bit location (for example, writing to the flash, which from the perspective of UPDI starts at address 0x800000), and then an STS or LDS instruction is used with a 16-bit address, the high byte of the ST pointer will be used as the high byte of the directly-addressed memory address. This was discovered in the course of my work on jtag2updi.... I would use 16-bit STS to configure the NVMCTRL registers, then use ST to write a page to the flash. But then I tried to set NVMCTRL back to NOOP and check the NVMCTRL.STATUS with LDS (with 16-bit address) and ARGH! it was 0xFF! shouldn't ever be that! I eventually discovered I could just write 0's to it and that would "clear" the "error flags"... and I was puzzled by why there were a few bytes set to 0x00 around 0x1000 when I read the flash back out afterwards (because I had not set NVMCTRL back to NOOP - I instead wrote to location 0x1000 of the flash, and also cleared the "error flags" (ie, blank flash) a few bytes later. That cost me like two weeks, and was very discouraging.

For "intened behavior", it's bloody stupid, and the resulting behavior is thoroughly baffling.

### ADC Increased Offset in Single-Ended mode
The ADC has a dramatically higher offset error in single-ended mode than expected: -3 mV. This implies that all readings would be 12 LSB lower than expected at 12 bit accuracy... when the spec is 1.25 LSB. This is pretty ugly for a headline feature! Only mentioned for the 128 DB, and noted as fixed in the A5 rev - A4 parts barely made it out the door - they got this one fixed in a real hurry, as they should have

### OPAMP IRSEL bit read-only
The opamp IRSEL (Input Range SELect) is read-only; input range is always rail-to-rail. Wasn't in any other sizes, and got fixed FAST. A4's are rare and hard to find.

### OPAMPs consume more power than expected
The opamp modules consume 3x the expected (from datasheet) power. Wasn't in any other sizes, and got fixed FAST. A4's are rare and hard to find.

### PLL Status bit on DA and DB-series not set if not requested by a peripheral
Yeah, it always reads 0 unless a peripheral is using it. Pretty much defeats the purpose of that bit. Except that with only a single peripheral that can use it this bit doesn't matter much anyway. Makes you wonder if it was originally supposed to be able to clock something else... though I don't really know what that might be.

### PLL on DB-series doesn't run from external crystal
If the SOURCE bit in CLKCTRL.PLLCTRLA is set to 1, datasheet implies that it should be clocked from external clock or crystal. This does not work - instead, it does not appear to generate any output whatsoever. I doubt anyone here cares, but, I tripped over this almost immediately when I started playing with the crystal as clock on DB-series... (was trying to make sure that clock sources were "requested" so the bits would be set in CLKCTRL.MCLKSTATUS, because of those other errata).

### TCA restart resets counter direction
This erratum is going into every update to the modern AVR errata documents (all of them!). It's not clear to me, however, how that differs from intended behavior...

From Errata:

***Restart Will Reset Counter Direction in NORMAL and FRQ Mode**
When the TCA is configured to a NORMAL or FRQ mode (WGMODE in TCAn.CTRLB is ‘0x0’ or ‘0x1’), a RESTART
command or Restart event will reset the count direction to default. The default is counting upwards.*

From datasheet:

*The software can force a restart of the current waveform period by issuing a RESTART command. In this case the counter, **direction**, and all compare outputs are set to 0*

I can see how this might not be ideal for restart events... but nowhere is any text that would make me think it wasn't supposed to do exactly this.

### Flash endurance 1k, not 10k cycles
This is a "datasheet clarification" instead of an errata, which I take to mean that they don't plan to fix this. Which seems sort of surprising, frankly, if the DD's are going to be released this fixed, when you just know most of the errata here are unlikely to ever get a correction. Maybe the DD's are going to get released with 1k flash write endurance, and they've decided that it's not practical to fix on the DD while keeping word writes?

## Interesting things removed from early io headers
Between the initial releases of the io headers (eg, `ioavr128da64.h`), and more recent ones, of course, they corrected an assortment of errors, typos, missing information - the usual... And also, it would appear, the accidental inclusion of references to features not described my the datasheet? Nothing **SUPER** interesting, but...

### EVSYS_USEROSCTEST

```c++
#define EVSYS_USEROSCTEST  _SFR_MEM8(0x024B)
```
An extra event user... OSCTEST? I wonder what it does! I guess there's only one way to find out, and that's to monitor the system clock carefully with that set, and then trigger the event! The real question is whether it is to test some special oscillator feature... or just test if the oscillator is "safe" at it's current speed by stressing it (and you'd want that to be an event so you can have it executing code maybe? I dunno!)  Someone go try it out and let me know if they can make it do anything....

### CLKCTRL_MULFAC_4x_gc
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

#### Thoughts on this and the internal oscillator's 32 MHz option
This (and the internal oscillator working up to 32 MHz) raises a question - what speed were they originally targeting during design? What if 24 MHz / 48 MHz wasn't the planned maximum? We know the core has a lot of AVRxm DNA - and the xMega's say they run at 32 MHz - say that was the original target, with 64 MHz PLL maximum? Maybe they had to back off to after they found that their process wasn't quite as good as they hoped, that is, under certain temperature and voltage conditions, running at 32 MHz, it didn't give them high enough yields, or they had problems with the upper end of the temperature range... If it had been targeting 32 MHz with a 64 MHz PLL, all of the mutiplication factors would make sense again: 32 MHz x 2 == 64 MHz. 20 MHz like a tinyAVR x 3 = 60, just shy of the maximum... 24 MHz could only go 2x but 48 MHz is a magic speed for USB - and the 4x would get you the maximum 64 MHz from the ever-popular 16 MHz system clock.
So that's my theory - they designed it in the hopes of being able to do 32 MHz and 64 MHz on the PLL. Then one of two things happened: Either the temperature specs were always this high (125C maximum!) and they realized their process just couldn't produce chips that would run at 32/64 at temperatures well above the boiling point of water at high yield - or they'd been originally targeting more modest temperatures, but in discussions with customers, heard that the customer would rather they lower the maximum speed (because really, nobody uses an 8-bit AVR if they need computational power) to raise the maximum operating temperature. Either way, they then dropped back to 24 MHz and 48 MHz PLL, thus giving them the specs they needed for the USB functionality on the DU without further clock improvmements,  I don't know if that's how it went down, but whyever it's there, I'm glad the 32 MHz internal option exists.

As it happens, at room temperature, I've had no difficulties clocking TCD from the PLL at 4x multiplier.... even when the system is running at 32 MHz (hence, I was getting PWM derived from a 128 MHz clock; attempting the same with a 40 MHz clock proved too much to ask, and, particularly when the duty cycle was changed timer output would randomy skip cycles, though of course 3x multiplier worked. 48 MHz system clock with external clock worked (I haven't played with the PLL there) so a 2:1 overclock on CLK_PER and nearly 3:1 on the PLL, at room temperature with no special steps other than using exetended temperature range parts? That's pretty damned good if you ask me. Clearly the message is that at room temperature, these have a crazy amount of headroom for overclocking, and I have some personal projects that will requiire 32-40 MHz to achieve performance targets. It also works at well below the required minimum input PLL clock speed.

Note that this is not an endorsement of overclocking in production systems; that is foolish and potentially fraudulent depending on claims made to customers. Obviously, in safety critical systems, it would constitute gross negligence.

### AC_POWER_PROFILE3_gc
Analog Comparators had a third option for power profile planned... The higher the option, the lower the power consumption, but the higher the latency. If I had to place a bet on it, my money would be that like the 4x multiplier, the third power profile was removed only from the headers, and that it's still in the silicon - but it may not work so well.
```c++
    AC_POWER_PROFILE3_gc = (0x03<<3),  /* Power profile 3 */
```
This is once more listed on the AVR EA-series initial headers, so it may be that they got it sorted out during the intervening time. Or maybe it'll go away again.

### USART_RS485_gm
Finally, the USART had the two RS485 modes listed, like tinyAVR 0/1-series, instead of just the one. The second mode raised all sorts of quesions about how the chip ought to behave - and was one of those terse-and-useles sections that answers little. If I correctly understand the intent - it was meant to listen to XDIR and only output data when XDIR was in the correct state. So what does it do when the pin transitioned to "no sending" state while it was in the middle of a byte? Is there any answer to that that will result in correct behavior in applicable use-cases? I think they thought about that one, realized the answer was "probably not in the general case..." maybe re-read what they'd said about it in the datasheets of the parts that had been released (very little), and figured that nobody was going to be heartbroken to see it go.
