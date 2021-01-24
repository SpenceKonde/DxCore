# DA-series notes

Be sure to read the AVR128DA errata, there are some nasty silicon bugs in the first release of these (which is all that's currently available):
* TCA1 PWM output can only be mappend to ports B and C - E and G are unavailable (this core uses port B on all parts)
* The memory mapping applies the bootloader section protections to the memory mapped flash, regardless of which page (ie, if bootloader was set to be the first 512b of flash, the first 512b of EVERY page would be treated as bootloader memory when accessing via memory map) - not an issue when using with Arduino typically, though.
* TWI doesn't work if the pins aren't set low - Wire.h will be adapted to insure that Wire.Begin() configures this correctly.
* SPI with default pin mapping MUST disable SS Detect in order to operate in master. Currently the SPI.h library does this anyway and does not support slave mode anyway.
* PB6, PB7, PE4-PE7 are not connected to the event system, both output and input. Hence, alternate locations for EVOUTB and EVOUTE do not work, nor will using those pins as event generators.
* Digital input disabled when pin used for analog input. This will be compensated for by the core (unless this is more serious than their app notes indicate) and should not be seen if using the digital/analogRead functions
* As with... basically every modern AVR, the USART Open Drain mode for Serial requires the TX pin to NOT be set as an output, otherwise it can drive the pin high despite being in open drain mode... You'd think after 4 years without fixing it, they'd have just changed the datasheet to say that the bit in PORTx.DIR had to be 0 to ensure that it did not drive the pin high...

Several of those give the impression that most of the development was done with the 48-pin version (Port G, and the extra pins on Ports B and E are only on the 64-pin version)

## Discoveries
* The fuse listing in datasheet is apparently inaccurate or incomplete, as the parts come with an impossible fuse combination set. So far, no problems resulting from this have been encountered. More to come as this is further investigated.
* UPDI: On these parts, the 24-bit addressing mode must always be used, even for addressed reachable by the first 16 bits. If the ST_ptr instruction is used, the high byte of the pointer will be used for the high byte of a 16-bit STS instruction. jtag2updi does this correctly.
* You can't view CLKCTRL.OSCHFTUNE register while AutoTune is engaged.
* The tuning range is much narrower. Only 64 values of the OSCHFTUNE are valid and over the whole range the frequency varies within 10% of nominal. Not like classic AVRs where you could get the 8MHz internal going at 12.8, or even 16...
