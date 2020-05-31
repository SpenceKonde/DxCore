# DA-series notes

Be sure to read the AVR128DA errata, there are some silicon bugs in the first release of these :
* TCA1 PWM output can only be mappend to ports B and C - E and G are unavailable (this core uses port B on all parts)
* The memory mapping applies the bootloader section protections to the memory mapped flash, regardless of which page (ie, if bootloader was set to be the first 512b of flash, the first 512b of EVERY page would be treated as bootloader memory when accessing via memory map) - not an issue when using with Arduino typically, though.
* TWI doesn't work if the pins aren't set low - Wire.h will be adapted to insure that Wire.Begin() configures this correctly.
* SPI with default pin mapping MUST disable SS Detect in order to operate in master. Currently the SPI.h library does this anyway and does not support slave mode anyway.
* PB6, PB7, PE4~PE7 are not connected to the event system, both output and input. Hence, alternate locations for EVOUTB and EVOUTE do not work, nor will using those pins as event generators.
* Digital input disabled when pin used for analog input. This will be compensated for by the core (unless this is more serious than their app notes indicate) and should not be seen if using the digital/analogRead functions
* Open Drain mode for Serial requires the TX pin to NOT be set as an output, otherwise it can drive the pin high despite being in open drain mode.

Several of those give the impression that most of the development was done with the 48-pin version (Port G, and the extra pins on Ports B and E are only on the 64-pin version)

## Discoveries
* The fuse listing in datasheet is apparently inaccurate or incomplete, as the parts come with an impossible fuse combination set. So far, no problems resulting from this have been encountered. More to come as this is further investigated.
* Datasheet says that if you write a value to the NVM.CTRLA register when it is not NOOP or NOCMD, it will not honor the command. At least in the case of an invalid command sent via UPDI, instead NVM.CTRLA and NVM.STATUS subsequently read as 0xFF (though writing them both 0 resolves it) - yet the command is honored!
