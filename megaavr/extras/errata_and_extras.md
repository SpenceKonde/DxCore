# Errata and Extras
The errata for the DA128 parts was a rather depressing document. A large number of issues were present, many of them rather serious. Worse still, the document released in April 2020 by Microchip is not even complete - and it certainly does a poor job of explaining some of the most important issues that it does mention. Microchip does a very poor job of documenting this errata and updating the silicon errata sheets as new issues are discovered.
As hardware which is not impacted by these issues becomes available, we will provide methods to determine the "silcon revision" and hence whether a part is effected:
Issue    | Severity | Source    | 128DA | 64DA | 32DA |  128DB
---------|----------|-----------|-------|------|------|--------
Vector table is wrong     | 5        | AVRFreaks | NO    | NO   | YES | NO
ADC disables digital input| 2        | Microchip | YES   | YES  | YES | NO
Memory mapped flash issues| 3        | Microchip | YES   | NO   | N/A | NO
TCA1 Remap on PORTE/G     | 1        | Microchip | YES   | NO?  | N/A | NO
TWI Pins must be LOW      | 1        | Microchip | YES   | YES  | YES | YES
SPI SSD only works on alt pins | 2   | Microchip | YES   | NO?  | NO? | NO?
USART Open Drain TX must be INPUT | 1| Microchip | YES   | YES  | YES | YES
TWI SDA Hold Times        | 1        | Microchip | YES   | NO?  | NO? | NO?
ZCD Output remapping broken| 1       | Microchip | NO?   | YES  | YES | A4 only
No Event on PB6,7 PE4,5,6,7 | 3      | Microchip | YES   | NO?  | N/A | NO?
All CCL LUTs enable-locked to CCL| 2 | Microchip | Likely| Likely|Likely | YES
CCL3 on 32/28-pin no LINK input| 2   | Microchip | YES   | YES  | YES | A4 only
Initial fuses don't match datasheet | 1 | Microchip | A6 | ???  | ??? | A4 week 21 and older
TCD0 portmux options broken | 3        | Microchip | Yes | Likely | Likely | Yes
ADC increased offset in single-ended | 4 | Microchip | N/A | N/A | N/A | A4 only
OPAMP power consumption 3x higher than expected | 1 | Microchip | ??? | ??? | ??? | A4 only
OPAMP IRSEL bit read-only | 1 | Microchip | N/A | N/A | N/A | A4 only
PLL doesn't work from ext. xtal | 1 | Microchip | N/A | N/A | N/A | Yes

NO? means not mentioned in errata, but has not been confirmed as not being present in that size of chip.
N/A for the 32DA: There's no AVR32DA64, hence the 64-pin-only issues don't apply.The flash mapping is likewise not an issue there as they only have one section of flash.
The * indicates that this issue is universal and applies to every modern AVR

Alas - 9 months after the release of the DA-series,we have seen only the AVR128DB get a new silicon rev that fixed a significant number of issues.

## Determining silicon rev
Read SYSCFG.REVID; SYSCFG.REVID&0xF0 is the major rev (the letter), SYSCFG.REVID&0x0F is the minor rev.
See [ModernRevSerial](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/libraries/DxCore/examples/ModernRevSer/ModernRevSer.ino) for a sample (reads serial number, silicon rev)

## Descriptions
All written my me, not copied from Microchip.

### Vector Table is Wrong on AVR32DA
Remember how the ATmega808 and 809 had 4-byte interrupt vectors in hardware, but for a while the toolchain tried to generate 2-byte vector tables for it, unsurprisingly that didn't work. Well this is much worse: it's the other way around! Not only does the toolchain generate binaries that don't work, it cannot be fixed with a toolchain update, because the hardware only supports a 2-byte (rjmp) vector, but the chip has 32k of flash, so only a quarter of the chip would be accessible for interrupt vectors. **Impacted AVR32DA parts are not usable**, and they are apparently being recalled, according to a Microchip employee posting on the AVRFreaks forum. However, that was late October; as of early Febuary the fact that the chips are impacted by an errata that renders them completely unusable is still not mentioned on the silicon errata. It really raises some questions about their test proceedures (They produced a new chip, packaged them, and shipped them, advertising them as fit for use in life safety critical applications without having tested even the equivalent of blink - AND it was a screwup that they did the opposite of on the smallest flash version of the prior product line, so they should have been specifically checking for it...) This definitely impacts the A4 silicon revision. It is unclear whether the A5 revision is impacted.

### ADC disables digital input
*If an input pin is selected to be analog input, the digital input function for those pins is automatically disabled*

In my opinion, poorly described. What they mean is: `While ADC0 is enabled, if ADC0.MUXPOS is set to a value which selects a pin as the positive input to the ADC, the PORTx.IN register is not updated for that pin` This is hardly fatal - and easily worked around - but it is also nearly impossible to ignore if the core does not work around it. The most disconcerting part is that since the core will enable the ADC for you in init(), if it doesn't also work around this, you will immediately find that AIN0 is non-functional as a digital pin! This caused considerable consternation during development. Despite the frightening-sounding "Workaround: None" in the errata sheet, there is a simple workaround for most use cases - just point the mux somewhere else; if it is set to some reserved value that does not correspond to any pin, it will not disrupt any input.

This core implements this simple workaround - on startup, and every every call to analogRead(), the mux is set to 127: `ADC0.MUXPOS=0x7F` That is a reserved value which does not correspond to any pin (it appears to be a floating input if the ADC is actually asked to measure it).

Just because this is worked around does not mean you can totally ignore it though; there are still a few cases where it becomes relevant.
* If you are taking over the ADC - for example to use free running mode, the window compaerator, etc, you must remember than digitalRead() will not work on the pin you are doing analog readings on.
* Pin interrupts will not be triggered on the pin being used for analog readings (even with the workaround, they won't be triggered until AFTER the ADC cycle is completed and we move the ADC mux)

### SPIn.SSD required for master mode in default SPI pin mapping
If the SSD (slave select disable) bit is not set, if an SPI interface is used with the default pin mapping, master mode will not be operable. Not much of a concern for Arduino users, since the only SPI library around for the modern AVR parts only supports master mode and always sets this bit.

### USART Open Drain mode
Yeah, you can't have TX pin as OUTPUT if using open drain mode, because it can drive the pin high. Setting it INPUT works. Just like on every other part with the new peripherals since their introduction. They should really just amend the datasheet, remove it from the errata, and pretend they meant it if they're not going to fix it.

### TCA1 remap options 2 and 3 not functional
Remapping TCA1 to PORTE and PORTG does not work. Note that this is only relevant for 64-pin parts, as the effected pins are not present on 48-pin parts. Kind of a bummer though that we can't get more use out of PORTG!

### Event System not connected to PB6, PB7, PE4, PE5, PE6, PE7
Another slap-in-the-face for 64-pin users: 6 of the pins only present on the 64-pin versions aren't enabled for the event system. Of course, you can always use them for other things (just not PWM output, per above)...

### ZCD Output remapping non-functional
The ZCD0 bit in PORTMUX.ZCDROUTEA effects all three ZCD's, not just ZCD0, the other two bits do nothing. If you were counting on having more than one output from a ZCD, is a modest inconvenience (you can make it up with event system). Likely few if any Arduino folks are interested in this anyway. This can be worked around by using the event system to control the desired pin.

### All LUTs enable locked to entire CCL module
The LUTs (all of them) cannot be modified if the CCL (the whole thing) is enabled, instead of the LUT in question. This is a rather nasty one if you want to use different LUTs for different purposes. Only listed on errata for 128 DB-series, but am not optimistic about the DA-series parts.

### CCL on 32 and 28-pin parts, LINK input to LUT3 does not work
It's not just the owners of 64-pin parts who got special presents from the errata fairy - the LINK input to LUT3 is not connected to the output of LUT0 on the 28 and 32-pin parts (one imagines it's connected to the output of LUT4, except that there is no LUT4-5 on the 28 and 32-pin parts). Most of us probably don't need a LINK input on every LUT, if we're using CCL at all, so this is likely not a problem... annoying, but only that. Microchip claims it was fixed in A5 rev of 128 DB.

### TWI pin output override not working correctly
The TWI is able to override the output data direction - but not the output value. Hence, the pins used to TWI must not be set HIGH otherwise the peripheral cannot drive them low (note that the inverse is not a problem - TWI is an open drain bus, and the devices on it only ever drive the lines low, or let them be pulled up). DxCore has implemented a workaround in the Wire library which makes sure the relevant pins are always set LOW when the TWI peripheral is initialized.

### Flash mapping does not work correctly
When accessing the flash via mapping to data memory (according to the FLMAP bits in NVMCTRL.CTRL), the inter-section flash protection (which prevents the application from scribbling over itself or the bootloader) fails to account for the FLMAP bits when deciding if access to an address is to be blocked. This means that the BOOT section is "mirrored" across all sections (presumably likewise with the other sections, which probably means that appdata section is unwritable from appcode section too, unless the appdata section includes flash in the first 32k). If reading of the bootloader section is disabled, that is similarly effected (otherwise, reading works fine). Thankfully this only impacts access using LD/ST instructions - you can still write and read from everywhere you're supposed to be able to using LPM and SPM - which is of course considerably less convenient. This is mostly a nuisance to bootloader authors, who now have a harder time of updating their bootloaders to work on these parts (as if the new NVMCTRL wasn't enough of an impediment there) - at least for Arduino folks, where no bootloader is going lock down reads of the bootloader section. Likely more of a problem in production... Not mentioned on AVR128DB, nor AVR64DA errata - so this was hopefully sorted out quickly and will be fixed in the 128 DA-series with a silicon rev sooner rather than later.

### Pre-set fuses don't match datasheet
The fuses, as supplied (on AVR128DA, Rev. A6 silicon, at least) do not match the configuration described in the datasheet. Generally speaking, unused bits are set to 1 instead of 0 (except in a few cases), and in the case of OSCCFG, a "reserved" combination is selected.
* BODCFD is set to 0x10, per datasheet this is an invalid BOD level setting. Should not impact functionality, as BOD is disabled by the low 5 bits, and the high bits would be reset if you enabled BOD anyway.
* OSCCFG is set to 0x78, per datasheet low nybble should be 0 or 1, high nybble all 0's (unused). All options other than 0 and 1 for low 4 bits are marked reserved. I guess bit 3 doesn't matter so much?
* SYSCFG0 is set to 0xF2 instead of 0xC8. While we might not care what CRC algorithm the disabled CRC check is using, the fact that it disables reset is certainly disconcerting to discover...
* SYSCFG1 is set to 0xF8 instead of 0x00 - just a case of unused bits set 1 instead of 0.

Certainly makes you wonder about their emphatic "unused bits MUST be set to 0" warning doesn't it? Burn bootloader when you get the parts to make reset work.
On DB-series parts, this only effects parts with a datecode of week 21 or earlier. The same is likely true of DA-series as well, since this can be corrected with just a change to their factory programming process.

### TCD0 on non-default port doesn't work on AVR128DA
When PORTMUX.TCDROUTEA is set to anything other than default (PORTA), all four outputs are controlled by the TCD0.FAULTCTRL CMPAEN bit. Confirmed on AVR128DA Rev A6 silicon, all sizes. The core only uses PA4 and PA5 as TCD0 PWM outputs because of this issue (this may be changed if/when silicon that corrects this issue is made available). Was reported to Microchip support on 8/4/2020 and their response implied that it had not been previously reported. According to Microchip, this issue has "only been confirmed" on the AVR128DA parts.

### ~UPDI programming issue with 16-bit STS after 24-bit STptr~
It was related to me that this is intended behavior. In which case, the documentation is utterly abysmal!
Microchip seemed unaware of this issue when I reported it to them, and forwarded it to their engineering team. When programming over UPDI, there are two ways to write and read memory, with direct and indirect addressing. Both of them can use 16 or 24-bit addressing (or 8-bit, but this is useless). The documentation describes the ST (indirect) pointer as being only used for ST/LD, not for STS/LDS. However, if an ST instruction sets the indirect addressing pointer to a 24-bit location (for example, writing to the flash, which from the perspective of UPDI starts at address 0x800000), and then an STS or LDS instruction is used with a 16-bit address, the high byte of the ST pointer will be used as the high byte of the directly-addressed memory address. This was discovered in the course of my work on jtag2updi.... I would use 16-bit STS to configure the NVMCTRL registers, then use ST to write a page to the flash. But then I tried to set NVMCTRL back to NOOP and check the NVMCTRL.STATUS with LDS (with 16-bit address) and ARGH! it was 0xFF! shouldn't ever be that! I eventually discovered I could just write 0's to it and that would "clear" the "error flags"... and I was puzzled by why there were a few bytes set to 0x00 around 0x1000 when I read the flash back out afterwards (because I had not set NVMCTRL back to NOOP - I instead wrote to location 0x1000 of the flash, and also cleared the "error flags" (ie, blank flash) a few bytes later. That cost me a lot of time, and had I not been stymied by jtag2updi for so long, y'all might have been using this core in may instead of now (I was so burned out of these things that I needed to take some time away from them...)

### ADC Increased Offset in Single-Ended mode
The ADC has a dramatically higher offset error in single-ended mode than expected: -3 mV. This implies that all readings would be 12 LSB lower than expected at 12 bit accuracy... when the spec is 1.25 LSB. This is pretty ugly for a headline feature! Only mentioned for the 128 DB, and noted as fixed in the A5 rev. This really ought to be verified as not present in the DA-series....

### OPAMP IRSEL bit read-only
The opamp IRSEL (Input Range SELect) is read-only; input range is always rail-to-rail. Worse than were it stuck the other way around I reckon... Fixed in A5 rev on 128DB, hopefully won't be in the other sizes at all, and with luck, the A5's will quickly push the A4's out of the supply chain.

### OPAMPs consume more power than expected
The opamp modules consume 3x the expected (from datasheet) power. Fixed in A5 rev on 128DB, hopefully won't be in the other sizes at all, and with luck, the A5's will quickly push the A4's out of the supply chain.

### PLL on DB-series doesn't run from external crystal
If the SOURCE bit in CLKCTRL.PLLCTRLA is set to 1, datasheet implies that it should be clocked from external clock or crystal; with external crystal, it does not appear to generate any output. I doubt anyone here cares, but, I tripped over this almost immediately when I started playing with the crystal as clock on DB-series... (was trying to make sure that clock sources were "requested" so the bits would be set in CLKCTRL.MCLKSTATUS.

## Extras (anti-errata)
Well, that was depressing wasn't it?

Here's the good news - there is also a little bit of undocumented behavior that is favorable too (I feel like there was another one here, but can't remember what it is)

### Overclocking
You don't even need an external clock source to overclock these bad boys! You can do it... just by setting the CLKCTRL.OSCHFCTRLA FREQSEL bits higher! You can only go up to 32 MHz that way - after that (0x0C-0x0F), it goes back to the settings from 0x08-0x0B - 20-32 MHz all over again - but an extra 33% clock speed is hard to argue with - of course, these are not guaranteed to work, probably won't work across the whole temperature range, and so on... but they do work for me on all the parts I've tried it on room temperature. It also gives one hope that - armed with an external clock source - there's more room to crank up the speed on these - if you for some reason weren't satisfied with ~24~ 32 MHz in practice, 24 MHz guaranteed at 1.8-5.5V on an architecture that until this year was limited to 20 MHz at 4.5-5.5V and a mere 4 MHz at 1.8V...

Using DxCore, you don't even need to go to all that effort - you can just select 28 MHz or 32 MHz from the Tools -> Clock Speed menu, and upload your sketch and it will run at the higher clock speed!

### Overclocking the PLL
The system clock isn't the only thing that has breathing room either (at least at 5v and 25C, on the parts I tested) - the PLL, which is spec'ed for 16-24 MHz internal HF oscillator frequency as input only, 48 MHz max frequency (24 MHz multiplied by 2)? It runs at the 3x multiplier, all the way up to 32 MHz system clock... and I checked, TDC0 really was ticking over at 96 MHz! It seems to work at a lower max frequency than they spec, too - I was getting perfectly good output at 8 MHz in (tripled). Now, how useful is this crazy clock speed when all you can do with it is run one async timer? Okay, it's not the most useful feature ever.... but it is good for something. Will take someone with sensitive current meters and too much time on their hands to figure out if it could, for example, be used to maintain PWM frequency while saving power with lower system clock, or things like that...

Also, note the removed option in the io header - apparently it can be run at x4 multiplication factor. In fact, it works at 32 MHz x 4 = 128 MHz at room temperature...

The PLL did not work at all work with the oscillator at 4 MHz (though it worked at 8 MHz and 12 MHz). These parts seem to have a huge safety margin on their clock subsystem.

### Interesting things removed from early io headers
Between the initial releases of the io headers (eg, `ioavr128da64.h`), and more recent ones, of course, they corrected an assortment of errors, typos, missing information - the usual... And also, it would appear, the accidental inclusion of references to features not described my the datasheet? Nothing **SUPER** interesting, but...

```
#define EVSYS_USEROSCTEST  _SFR_MEM8(0x024B)
```
An extra event user... OSCTEST? I wonder what it does! I guess there's only one way to find out, and that's to monitor the system clock carefully with that set, and then trigger the event! The real question is whether it is to test some special oscillator feature... or just test if the oscillator is "safe" at it's current speed by stressing it (and you'd want that to be an event so you can have it executing code maybe? I dunno!)

This next one is the bitfield that controls what the PLL will multiply the input clock by...
```
/* Multiplication factor select */
typedef enum CLKCTRL_MULFAC_enum
{
    CLKCTRL_MULFAC_DISABLE_gc = (0x00<<0),  /* PLL is disabled */
    CLKCTRL_MULFAC_2x_gc = (0x01<<0),  /* 2 x multiplication factor */
    CLKCTRL_MULFAC_3x_gc = (0x02<<0),  /* 3 x multiplication factor */
    CLKCTRL_MULFAC_4x_gc = (0x03<<0),  /* 4 x multiplication factor */
} CLKCTRL_MULFAC_t;
```
That last like was removed in later versions, to match the datasheet... And it does indeed work!

This (and the internal oscillator working up to 32 MHz) raises a question - what speed were they targeting during design? What if 24 MHz / 48 MHz wasn't the planned max, but what they had to back off to after they found that their process wasn't quite as good as they hoped, that is, under certain temperature and voltage conditions, it didn't meet the strict requirements for reliability that something you are going to put in your car or aeroplane and expect to work 100% of the time. Because with a 48 MHz maximum speed, there isn't a popular clock speed that they might be using the 4x multiplier on...

If `CLK_PER` was meant to be rated up to 32 MHz, and and PLL up to 64? 32 MHz x 2 == 64 MHz. 20 MHz like a tinyAVR x 3 = 60... 24 MHz could only go 2x - but who would run 24 MHz if 32 MHz was an option? - and the 4x would get you 64 MHz from the ever popular 16 MHz system clock. So that's my theory - they designed it in the hopes of being able to do 32 MHz and 64 MHz on the PLL, it didn't meet reliability requirements under pessimal conditions, so they dropped back to 24 MHz and 48 MHz PLL. I'm sure glad they left the 32 MHz oscillator option in place though!

Analog Comparators had a third option for power profile... The higher the option, the lower the power consumption, but the higher the latency. I wonder if it actually works?
```
    AC_POWER_PROFILE3_gc = (0x03<<3),  /* Power profile 3 */
```

## One "oddity" that's pretty much neutral
Despite the datasheet saying otherwise (quite explicitly), the lowest bit of the address is not ignored when self-programming the flash with SPM! Except at the boundary between pages, the low bit is treated as one might expect. If address provided is the last byte in a page, however, the high byte will be written at the first address in the next page... and the low byte to the second address in the next page! Note that our flash-writing libraries only write to even byte addresses (ie, we zero the lowest bit and adjust if needed) - this is only a concern if you write your own bootloader or flash-writing routines.
