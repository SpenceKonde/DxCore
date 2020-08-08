# Errata and Extras
The errata for the DA128 parts was a rather depressing document. A large number of issues were present, many of them rather serious. Worse still, the document released in April 2020 by Microchip is not even complete - and it certainly does a poor job of explaining some of the most important issues that it does mention.
As hardware which is not impacted by these issues becomes available, we will provide methods to determine the "silcon revision" and hence whether a part is effected:
Issue    | Severity | Source    | 128DA | 64DA | 32DA
---------|----------|-----------|-------|------|--------
ADC disables digital input| 4        | Microchip | YES   | YES  | YES
Memory mapped flash issues| 3 (4 in )| Microchip | YES   | NO?  | N/A
TCA1 Rema| 1 (2 on 64)| Microchip | YES | NO?  | N/A
TWI Pins must be LOW | 1        | Microchip | YES   | YES  | YES
SPI SSD only works on alt pins | 2        | Microchip | YES   | NO?  | NO?
USART Open Drain TX must be INPUT | 1        | Microchip | YES   | YES  | YES
TWI SDA Hold Times| 1        | Microchip | YES   | NO?  | NO?
ZCD Output remapping broken| 0-1      | Microchip | NO?   | YES  | YES
No Event on PB6,7 PE4,5,6,7 | 3        | Microchip | YES   | NO?  | N/A
CCL3 on 32/28-pin no LINK input| 2        | Microchip | NO?   | YES  | NO?
Initial fuses don't match datasheet | 1        | Spence K. | YES,A6| ???  | ???
TCD0 portmux options broken | 3        | Spence K. | YES,A6| Likely | Likely
UPDI 24-bit STptr followed by 16-bit STS | 3 or 0   | Spence K. | YES,A6| Likely | Likely

NO? means not mentioned in errata, but has not been confirmed as not being present in that size of chip.
N/A for the 32DA: There's no AVR32DA64, hence the 64-pin-only issues don't apply.The flash mapping is likewise not an issue there as they only have one section of flash.
* ZCD issue is mighty bad if you happened

## Determining silicon rev
Not important... yet... since there is only one revision available for 32k and 64k parts, and for 128k parts, while there are ostensibly two versions, they both have the same errata...

## Descriptions
All written my me, not copied from Microchip. Waiting for complete errata documents to do that

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
Yeah, you cant have TX pin as OUTPUT if using open drain mode, because it can drive the pin high. Setting it INPUT works. Just like on every other part with the new peripherals since their introduction. They should really just amend the datasheet, remove it from the errata, and pretend they meant it if they're not going to fix it.

### TCA1 remap options 2 and 3 not functional
Remapping TCA1 to PORTE and PORTG does not work. Note that this is only relevant for 64-pin parts, as the effected pins are not present on 48-pin parts. Kind of a bummer though that we can't get more use out of PORTG!

### Event System not connected to PB6, PB7, PE4, PE5, PE6, PE7
Another slap-in-the-face for 64-pin users: 6 of the pins only present on the 64-pin versions aren't enabled for the event system. Of course, you can always use them for other things (just not PWM output, per above)...

### ZCD Output remapping non-functional
The ZCD0 bit in PORTMUX.ZCDROUTEA effects all three ZCD's, not just ZCD0, the other two bits do nothing. If you were counting on having more than one output from a ZCD, is a modest inconvenience (but you can make it up with event system!). Likely few if any Arduino folks are interested in this anyway.

### CCL on 32 and 28-pin parts, LINK input to LUT3 does not work
It's not just the owners of 64-pin parts who got special presents from the errata fairy - the LINK input to LUT3 is not connected to the output of LUT0 on the 28 and 32-pin parts (one imagines it's connected to the output of LUT4, except that there is no LUT4-5 on the 28 and 32-pin parts). Most of us probably don't need a LINK input on every LUT, if we're using CCL at all, so this is likely not a problem... annoying, but only that.

### TWI pin output override not working correctly
The TWI is able to override the output data direction - but not the output value. Hence, the pins used to TWI must not be set HIGH otherwise the peripheral cannot drive them low (note that the inverse is not a problem - TWI is an open drain bus, and the devices on it only ever drive the lines low, or let them be pulled up). DxCore has implemented a workaround in the Wire library which makes sure the relevant pins are always set LOW when the TWI peripheral is initialized.

### Flash mapping does not work correctly
When accessing the flash via mapping to data memory (according to the FLMAP bits in NVMCTRL.CTRL), the inter-section flash protection (which prevents the application from scribbling over itself or the bootloader) fails to account for the FLMAP bits when deciding if access to an address is to be blocked. This means that the BOOT section is "mirrored" across all sections (presumably likewise with the other sections, which probably means that appdata section is unwritable from appcode section too, unless the appdata section includes flash in the first 32k). If reading of the bootloader section is disabled, that is similarly effected (otherwise, reading works fine). Thankfully this only impacts access using LD/ST instructions - you can still write and read from everywhere you're supposed to be able to using LPM and SPM - which is of course considerably less convenient. This is mostly a nuisance to bootloader authors, who now have a harder time of updating their bootloaders to work on these parts (as if the new NVMCTRL wasn't enough of an impediment there) - at least for Arduino folks, where no bootloader is going lock down reads of the bootloader section. Likely more of a problem in production...

### Pre-set fuses don't match datasheet
The fuses, as supplied (on AVR128DA, Rev. A6 silicon, at least) do not match the configuration described in the datasheet. Generally speaking, unused bits are set to 1 instead of 0 (except in a few cases), and in the case of OSCCFG, a "reserved" combination is selected.
* OSCCFG is set to 0x78, per datasheet low nybble should be 0 or 1, high nybble all 0's (unused). All options other than 0 and 1 for low 4 bits are marked reserved. I guess bit 3 doesn't matter so much?
* SYSCFG0 is set to 0xF2 instead of 0xC8. While we might not care what CRC algorithm the disabled CRC check is using, the fact that it disables reset is certainly disconcerting to discover...
* SYSCFG1 is set to 0xF8 instead of 0x00 - just a case of unused bits set 1 instead of 0.

Certainly makes you wonder about their emphatic "unused bits MUST be set to 0" warning doesn't it? Burn bootloader when you get the parts to make reset work.

### TCD0 on non-default port doesn't work
When PORTMUX.TCDROUTEA is set to anything other than default (PORTA), all four outputs are controlled by the TCD0.FAULTCTRL CMPAEN bit. Confirmed on AVR128DA Rev A6 silicon, all sizes. The core only uses PA4 and PA5 as TCD0 PWM outputs because of this issue (this may be changed if/when silicon that corrects this issue is made available). Was reported to Microchip support on 8/4/2020 which implies that it had not been previously reported...

### UPDI programming issue with 16-bit STS after 24-bit STptr
Microchip seemed unaware of this issue when I reported it to them, and forwarded it to their engineering team. When programming over UPDI, there are two ways to write and read memory, with direct and indirect addressing. Both of them can use 16 or 24-bit addressing (or 8-bit, but this is useless). The documentation describes the ST (indirect) pointer as being only used for ST/LD, not for STS/LDS. However, if an ST instruction sets the indirect addressing pointer to a 24-bit location (for example, writing to the flash, which from the perspective of UPDI starts at address 0x800000), and then an STS or LDS instruction is used with a 16-bit address, the high byte of the ST pointer will be used as the high byte of the directly-addressed memory address. This was discovered in the course of my work on jtag2updi.... I would use 16-bit STS to configure the NVMCTRL registers, then use ST to write a page to the flash. But then I tried to set NVMCTRL back to NOOP and check the NVMCTRL.STATUS with LDS (with 16-bit address) and ARGH! it was 0xFF! shouldn't ever be that! I eventually discovered I could just write 0's to it and that would "clear" the "error flags"... and I was puzzled by why there were a few bytes set to 0x00 around 0x1000 when I read the flash back out afterwards (because I had not set NVMCTRL back to NOOP - I instead wrote to location 0x1000 of the flash, and also cleared the "error flags" (ie, blank flash) a few bytes later. That cost me a lot of time, and had I not been stymied by jtag2updi for so long, y'all might have been using this core in may instead of now (I was so burned out of these things that I needed to take some time away from them...)

## Extras (anti-errata)
Well, that was depressing wasn't it?

Here's the good news - there is also a little bit of undocumented behavior that is favorable too.

### Overclocking
You don't even need an external clock source to overclock these bad boys! You can do it... just by setting the CLKCTRL.OSCHFCTRLA FREQSEL bits higher! You can only go up to 32 MHz that way - after that (0x0C-0x0F), it goes back to the settings from 0x08-0x0B - 20-32 MHz all over again - but an extra 33% clock speed is hard to argue with - of course, these are not guaranteed to work, probably won't work across the whole voltage range, and so on... but they do work for me on all the parts I've tried it on at 5v and room temperature. It also gives one hope that - armed with an external clock source - there's more room to crank up the speed on these - if you for some reason weren't satisfied with ~24~ 32 MHz in practice, 24 MHz guaranteed at 1.8-5.5V on an architecture that until this year was limited to 20 MHz at 4.5-5.5V and a mere 4 MHz at 1.8V...

Using DxCore, you don't even need to go to all that effort - you can just select 28 MHz or 32 MHz from the Tools -> Clock Speed menu, and upload your sketch and it will run at the higher clock speed!

### Overclocking the PLL
The system clock isn't the only thing that has breathing room either (at least at 5v and 25C, on the parts I tested) - the PLL, which is spec'ed for 16-24 MHz internal HF oscillator frequency as input only, 48 MHz max frequency (24 MHz multiplied by 2)? It runs at the 3x multiplier, all the way up to 32 MHz system clock... and I checked, TDC0 really was ticking over at 96 MHz! It seems to work at a lower max frequency than they spec, too - I was getting perfectly good output at 8 MHz in (tripled). Now, how useful is this crazy clock speed when all you can do with it is run one async timer? Okay, it's not the most useful feature ever.... but it is good for something. Will take someone with sensitive current meters and too much time on their hands to figure out if it could, for example, be used to maintain PWM frequency while saving power with lower system clock, or things like that...

It didn't work with the oscillator at 4 MHz though.
