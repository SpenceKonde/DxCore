# Errata
The errata for the DA128 parts was a rather depressing document. A large number of issues were present, many of them rather serious. Reasonably complete errata sheets were not available until 8 months after release - and the errata sheet does a poor job of explaining some of the most important issues that it does mention. Where a revision is named, that is the last revision which exhibits this errata q
As hardware which is not impacted by these issues becomes available, we will provide methods to determine the "silcon revision" and hence whether a part is effected:
Issue                          | Severity | Source    | 128DA | 64DA | 32DA    |  128DB  | 64DB  | 32DB  | 64DD  | 32DD  | 16DD  | Notes                              |
--------------------------------------|---|-----------|-------|------|---------|---------|-------|-------|-------|-------|-------|------------------------------------|
Increased Current if Vdd falls < 2.1V | 2 | Microchip | NO?   | NO?  | NO?     | YES     | YES   | YES   |       |       |       |                                    |
BOD registers not reset if UPDI enable| 1 | Microchip | YES   | YES  | YES     | NO      | NO    | NO    |       |       |       |
Vector table is wrong                 | 5 | AVRFreaks | NO    | NO   |RECALLED | NO      | NO    | NO    |       |       |       |                                    |
CRC check on reset non-functional     | 1 | Microchip | A6    | A3   | A3      | NO      | NO    | NO    |       |       |       | I suspect this is what got A7 out so fast. |
ADC disables digital input            | 2 | Microchip | YES   | YES  | YES     | NO      | NO    | NO    |       |       |       | Likely fixed for DD                |
Memory mapped flash issues            | 3 | Microchip | YES   | NO   | N/A     | NO      | NO    | N/A   |       | N/A   | N/A   |                                    |
TCA restart resets counter direction  | 2 | Microchip | YES   | YES  | YES     | YES     | YES   | N/A   |       |       |       | Has impacted all modern AVRs       |
TCB CCMPH/CCMPL act as 16-bit in PWM  | 2 | Microchip | YES   | YES  | YES     | YES     | YES   | YES   |       |       |       | Has impacted all modern AVRs       |
TCA1 Remap on PORTE/G                 | 3 | Microchip | YES   | NO   | N/A     | NO      | NO    | N/A   | N/A   | N/A   | N/A   | This is a real pain now that we support alt mappings! |
TWI Pins must be LOW                  | 1 | Microchip | YES   | YES  | YES     | YES     | NO??  | NO??  |       |       |       | Has impacted all modern AVRs       |
SPI SSD only works on alt pins        | 2 | Microchip | YES   | NO?  | NO?     | NO?     | NO?   | NO?   |       |       |       |                                    |
USART Open Drain TX must be INPUT     | 1 | Microchip | YES   | YES  | YES     | YES     | YES   | YES   |       |       |       | Has impacted all modern AVRs       |
USART start of frame detect in active | 3 | Microchip | YES   | YES  | YES     | YES     | YES   | YES   |       |       |       | Has impacted all modern AVRs       |
TWI SDA Hold Times                    | 1 | Microchip | YES   | NO   | NO      | NO      | NO    | NO    |       |       |       |                                    |
ZCD Output remapping broken           | 1 | Microchip | NO?   | YES  | YES     | A4      | A4    | NO    | N/A   | N/A   | N/A   | Can be worked around with events   |
No Event on PB6,7 PE4,5,6,7           | 3 | Microchip | YES   | NO   | N/A     | NO      | NO    | N/A   | N/A   | N/A   | N/A   | Use different pins.                |
All CCL LUTs enable-locked to CCL     | 2 | Microchip | YES   | YES  | YES     | YES     | YES   | YES   |       |       |       | Has impacted all modern AVRs       |
CCL3 on 32/28-pin no LINK input       | 2 | Microchip | YES   | YES  | YES     | A4      | NO    | NO    |       |       |       | Likely fixed for DD                |
Initial fuses don't match datasheet   | 1 | Microchip | A6    | A3   | A3      | A4      | NO    | NO    |       |       |       | No practical implications          |
TCD count prescale breaks async input | 1 | Microchip | YES   | YES  | YES     | YES     | YES   | YES   |       |       |       | Has impacted all modern AVRs       |
TCD0 portmux options broken           | 3 | Microchip | YES   | YES  | NO??    | YES     | YES   | YES   |       |       |       | Intensely frustrating!             |
ADC increased offset in single-ended  | 4 | Microchip | N/A   | N/A  | N/A     | A4      | NO    | NO    |       |       |       | Fixed mighty fast!                 |
OPAMP power consumption 3x higher     | 1 | Microchip | N/A   | N/A  | N/A     | A4      | NO    | NO    | N/A   | N/A   | N/A   | Fixed mighty fast!                 |
OPAMP IRSEL bit read-only             | 1 | Microchip | N/A   | N/A  | N/A     | A4      | NO    | NO    | N/A   | N/A   | N/A   | Fixed mighty fast!                 |
PLL doesn't work from ext. xtal       | 1 | Microchip | N/A   | N/A  | N/A     | Yes     | YES   | YES   |       |       |       |                                    |

NO? means not mentioned in errata, but has not been confirmed as not being present in that size of chip.
NO?? means the same, but I am particularly doubtful.
N/A for the 32DA: There's no AVR32DA64, hence the 64-pin-only issues don't apply. The flash mapping is likewise not an issue there as they only have one section of flash.


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

Sounds like this confuses the internal voltage regulator...?

### BOD registers not reset if UPDI enable
*If the UPDI is enabled, the VLMCTRL, INTCTRL, and INTFLAGS registers in BOD will not be reset by other reset sources than POR*

"If the UPDI is enabled" means that the UPDI peripheral is actually running. Only particularly relevant because applies when the programmer resets the chip. So those registers are
not reset until the programming session has ended, and the chip is reset by other means. Could be absolutely maddening to figure out in the event that you found yourself in this corner case, but very few people will trip over it...


### Vector Table is Wrong on early AVR32DA
Remember how the ATmega808 and 809 had 4-byte interrupt vectors in hardware, but for a while the toolchain tried to generate 2-byte vector tables for it, unsurprisingly that didn't work. Well this is much worse: it's the other way around! Not only does the toolchain generate binaries that don't work, it cannot be fixed with a toolchain update, because the hardware only supports a 2-byte (rjmp) vector, but the chip has 32k of flash, so only a quarter of the chip would be accessible for interrupt vectors. **Impacted AVR32DA parts are not usable**, and they are apparently being recalled, according to a Microchip employee posting on the AVRFreaks forum. However, that was late October; as of early February the fact that the chips are impacted by an errata that renders them completely unusable is still not mentioned on the silicon errata. It really raises some questions about their test procedures (They produced a new chip, packaged them, and shipped them, advertising them as fit for use in life safety critical applications without having tested even the equivalent of blink - AND it was a screwup that they did the opposite of on the smallest flash version of the prior product line, so they should have been specifically checking for it...). Effected chips were pulled from the market and that's why between the fall and late winter, the 32 k parts were unavailable.

### ADC disables digital input
*If an input pin is selected to be analog input, the digital input function for those pins is automatically disabled*

In my opinion, poorly described. What they mean is: `While ADC0 is enabled, if ADC0.MUXPOS is set to a value which selects a pin as the positive input to the ADC, the PORTx.IN register is not updated for that pin` This is hardly fatal - and easily worked around - but it is also nearly impossible to ignore if the core does not work around it. The most disconcerting part is that since the core will enable the ADC for you in init(), if it doesn't also work around this, you will immediately find that AIN0 is non-functional as a digital pin! This caused considerable consternation during development. Despite the frightening-sounding "Workaround: None" in the errata sheet, there is a simple workaround for most use cases - just point the mux somewhere else; if it is set to some reserved value that does not correspond to any pin, it will not disrupt any input.

This core implements this simple workaround - on startup, and every every call to analogRead(), the mux is set to 0x40 (GND): `ADC0.MUXPOS=0x40` It is necessary to have started a conversion at least once, though you don't need to complete it, for this to work.

Just because this is worked around does not mean you can totally ignore it though; there are still a few cases where it becomes relevant.
* If you are taking over the ADC - for example to use free running mode, the window compaerator, etc, you must remember than digitalRead() will not work on the pin you are doing analog readings on.
* Pin interrupts will not be triggered on the pin being used for analog readings (even with the workaround, they won't be triggered until AFTER the ADC cycle is completed and we move the ADC mux)

### SPIn.SSD required for master mode in default SPI pin mapping
If the SSD (slave select disable) bit is not set, if an SPI interface is used with the default pin mapping, master mode will not be operable. Not much of a concern for Arduino users, since the only SPI library around for the modern AVR parts only supports master mode and always sets this bit.

### USART Open Drain mode
Yeah, you can't have TX pin as OUTPUT if using open drain mode, because it can drive the pin high. Setting it INPUT works. Just like on every other part with the new peripherals since their introduction. They should really just amend the datasheet, remove it from the errata, and pretend they meant it if they're not going to fix it.

### TCA1 remap options 2 and 3 not functional
Remapping TCA1 to PORTE and PORTG does not work. Note that this is only relevant for 64-pin parts, as the effected pins are not present on 48-pin parts. Kind of a bummer though that we can't get more use out of PORTG! This issue is only present on the DA however.

### Event System not connected to PB6, PB7, PE4, PE5, PE6, PE7
Another slap-in-the-face for 64-pin users: 6 of the pins only present on the 64-pin versions aren't enabled for the event system. Of course, you can always use them for other things (just not PWM output, per above)...

### TCD0 on non-default port doesn't work
When PORTMUX.TCDROUTEA is set to anything other than default (PORTA), all four outputs are controlled by the TCD0.FAULTCTRL CMPAEN bit.

### ZCD Output remapping non-functional
The ZCD0 bit in PORTMUX.ZCDROUTEA effects all three ZCD's, not just ZCD0, the other two bits do nothing. If you were counting on having more than one output from a ZCD, is a modest inconvenience (you can make it up with event system). Likely few if any Arduino folks are interested in this anyway. This can be worked around by using the event system to control the desired pin.

### All LUTs enable locked to entire CCL module
The LUTs (all of them) cannot be modified if the CCL (the whole thing) is enabled, instead of the LUT in question. This is a rather nasty one if you want to use different LUTs for different purposes. Only listed on errata for 128 DB-series, but am not optimistic about the DA-series parts.

### CCL on 32 and 28-pin parts, LINK input to LUT3 does not work
It's not just the owners of 64-pin parts who got special presents from the errata fairy - the LINK input to LUT3 is not connected to the output of LUT0 on the 28 and 32-pin parts (one imagines it's connected to the output of LUT4, except that there is no LUT4-5 on the 28 and 32-pin parts). Most of us probably don't need a LINK input on every LUT, if we're using CCL at all, so this is likely not a problem... annoying, but only that. Microchip claims it was fixed in A5 rev of 128 DB.

### TWI pin output override not working correctly
The TWI is able to override the output data direction - but not the output value. Hence, the pins used to TWI must not be set HIGH otherwise the peripheral cannot drive them low (note that the inverse is not a problem - TWI is an open drain bus, and the devices on it only ever drive the lines low, or let them be pulled up). DxCore has implemented a workaround in the Wire library which makes sure the relevant pins are always set LOW when the TWI peripheral is initialized.

### Flash mapping does not work correctly
When accessing the flash via mapping to data memory (according to the FLMAP bits in NVMCTRL.CTRL), the inter-section flash protection (which prevents the application from scribbling over itself or the bootloader) fails to account for the FLMAP bits when deciding if access to an address is to be blocked. This means that the BOOT section is "mirrored" across all sections (presumably likewise with the other sections, which probably means that appdata section is unwritable from appcode section too, unless the appdata section includes flash in the first 32k). If reading of the bootloader section is disabled, that is similarly effected (otherwise, reading works fine). Thankfully this only impacts access using LD/ST instructions - you can still write and read from everywhere you're supposed to be able to using LPM and SPM - which is of course considerably less convenient. This is mostly a nuisance to bootloader authors, who now have a harder time of updating their bootloaders to work on these parts (as if the new NVMCTRL wasn't enough of an impediment there). Application code reading from the flash would not be impacted unless the bootloader locked down reads of the bootloader section. And I don't think most of us use memory mapping anyway... Only impacts the AVR128DA, but by now bootloaders have been written to use

### Pre-set fuses don't match datasheet
The fuses, as supplied (on AVR128DA, Rev. A6 silicon, at least) do not match the configuration described in the datasheet. Generally speaking, unused bits are set to 1 instead of 0 (except in a few cases), and in the case of OSCCFG, a "reserved" combination is selected.
* BODCFD is set to 0x10, per datasheet this is an invalid BOD level setting. Should not impact functionality, as BOD is disabled by the low 5 bits, and the high bits would be reset if you enabled BOD anyway.
* OSCCFG is set to 0x78, per datasheet low nybble should be 0 or 1, high nybble all 0's (unused). All options other than 0 and 1 for low 4 bits are marked reserved. I guess bit 3 doesn't matter so much?
* SYSCFG0 is set to 0xF2 instead of 0xC8. While we might not care what CRC algorithm the disabled CRC check is using, the fact that it disables reset is certainly disconcerting to discover...
* SYSCFG1 is set to 0xF8 instead of 0x00 - just a case of unused bits set 1 instead of 0.

Certainly makes you wonder about their emphatic "unused bits MUST be set to 0" warning doesn't it? Burn bootloader when you get the parts to make reset work.
On DB-series parts, this only effects parts with a datecode of week 21 or earlier. The same is likely true of DA-series as well, since this can be corrected with just a change to their factory programming process.

### ~UPDI programming issue with 16-bit STS after 24-bit STptr~
It was related to me that this is intended behavior. In which case, the documentation is utterly abysmal!
Microchip seemed unaware of this issue when I reported it to them, and forwarded it to their engineering team. When programming over UPDI, there are two ways to write and read memory, with direct and indirect addressing. Both of them can use 16 or 24-bit addressing (or 8-bit, but this is useless). The documentation describes the ST (indirect) pointer as being only used for ST/LD, not for STS/LDS. However, if an ST instruction sets the indirect addressing pointer to a 24-bit location (for example, writing to the flash, which from the perspective of UPDI starts at address 0x800000), and then an STS or LDS instruction is used with a 16-bit address, the high byte of the ST pointer will be used as the high byte of the directly-addressed memory address. This was discovered in the course of my work on jtag2updi.... I would use 16-bit STS to configure the NVMCTRL registers, then use ST to write a page to the flash. But then I tried to set NVMCTRL back to NOOP and check the NVMCTRL.STATUS with LDS (with 16-bit address) and ARGH! it was 0xFF! shouldn't ever be that! I eventually discovered I could just write 0's to it and that would "clear" the "error flags"... and I was puzzled by why there were a few bytes set to 0x00 around 0x1000 when I read the flash back out afterwards (because I had not set NVMCTRL back to NOOP - I instead wrote to location 0x1000 of the flash, and also cleared the "error flags" (ie, blank flash) a few bytes later. That cost me like two weeks, and was very discouraging.

### ADC Increased Offset in Single-Ended mode
The ADC has a dramatically higher offset error in single-ended mode than expected: -3 mV. This implies that all readings would be 12 LSB lower than expected at 12 bit accuracy... when the spec is 1.25 LSB. This is pretty ugly for a headline feature! Only mentioned for the 128 DB, and noted as fixed in the A5 rev. This really ought to be verified as not present in the DA-series....

### OPAMP IRSEL bit read-only
The opamp IRSEL (Input Range SELect) is read-only; input range is always rail-to-rail. Worse than were it stuck the other way around I reckon... Fixed in A5 rev on 128DB, hopefully won't be in the other sizes at all, and with luck, the A5's will quickly push the A4's out of the supply chain.

### OPAMPs consume more power than expected
The opamp modules consume 3x the expected (from datasheet) power. Fixed in A5 rev on 128DB, hopefully won't be in the other sizes at all, and with luck, the A5's will quickly push the A4's out of the supply chain.

### PLL on DB-series doesn't run from external crystal
If the SOURCE bit in CLKCTRL.PLLCTRLA is set to 1, datasheet implies that it should be clocked from external clock or crystal; with external crystal, it does not appear to generate any output. I doubt anyone here cares, but, I tripped over this almost immediately when I started playing with the crystal as clock on DB-series... (was trying to make sure that clock sources were "requested" so the bits would be set in CLKCTRL.MCLKSTATUS.

## Interesting things removed from early io headers
Between the initial releases of the io headers (eg, `ioavr128da64.h`), and more recent ones, of course, they corrected an assortment of errors, typos, missing information - the usual... And also, it would appear, the accidental inclusion of references to features not described my the datasheet? Nothing **SUPER** interesting, but...

```
#define EVSYS_USEROSCTEST  _SFR_MEM8(0x024B)
```
An extra event user... OSCTEST? I wonder what it does! I guess there's only one way to find out, and that's to monitor the system clock carefully with that set, and then trigger the event! The real question is whether it is to test some special oscillator feature... or just test if the oscillator is "safe" at it's current speed by stressing it (and you'd want that to be an event so you can have it executing code maybe? I dunno!)

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
That last like was removed in later versions, to match the datasheet... But it does indeed work!

This (and the internal oscillator working up to 32 MHz) raises a question - what speed were they targeting during design? What if 24 MHz / 48 MHz wasn't the planned max, but  they had to back off to after they found that their process wasn't quite as good as they hoped, that is, under certain temperature and voltage conditions, it didn't meet the strict requirements for reliability that something you are going to put in your car or aeroplane and expect to work 100% of the time - or they realized that there was simply more demand among big customers for wider operating temperature ranges...

If `CLK_PER` was meant to be rated up to 32 MHz, and and PLL up to 64? 32 MHz x 2 == 64 MHz. 20 MHz like a tinyAVR x 3 = 60... 24 MHz could only go 2x - but who would run 24 MHz if 32 MHz was an option? - and the 4x would get you 64 MHz from the ever popular 16 MHz system clock. So that's my theory - they designed it in the hopes of being able to do 32 MHz and 64 MHz on the PLL, it didn't meet reliability requirements under pessimal conditions, so they dropped back to 24 MHz and 48 MHz PLL. I'm sure glad they left the 32 MHz oscillator option in place though!

Analog Comparators had a third option for power profile... The higher the option, the lower the power consumption, but the higher the latency. I wonder if it actually works?
```
    AC_POWER_PROFILE3_gc = (0x03<<3),  /* Power profile 3 */
```

## One "oddity" that's pretty much neutral
Despite the datasheet saying otherwise (quite explicitly), the lowest bit of the address is not ignored when self-programming the flash with SPM! Except at the boundary between pages, the low bit is treated as one might expect. If address provided is the last byte in a page, however, the high byte will be written at the first address in the next page... and the low byte to the second address in the next page! Note that our flash-writing libraries only write to even byte addresses (ie, we zero the lowest bit and adjust if needed) - this is only a concern if you write your own bootloader or flash-writing routines.
