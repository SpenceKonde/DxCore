# Dx-series Flash Writer **EXPERIMENTAL**
This library provides methods write to the application section of flash from within the application. This interface provides a uniform frontend for three different cases depending on configuration:

1. If using Optiboot, the bootloader will contain a special entry point to execute the one protected command, and these methods call that.

2. If not using Optiboot, but configured to allow writes "anywhere in the flash", during UPDI upload, we define a fake bootloader section encompassing the first page of flash, since only BOOTCODE can write to APPCODE.

3. If not using Optiboot, but configured to allow writes only to a limited section at the end of flash (configurable via tools submenu, in increments of 16k -

This now works with OR without the bootloader - without the bootloader, it relies upon the tools submenu for flash writing being configured appropriately. In all cases if enabled, the first page of flash is marked as "bootloader" (though the vectors are moved back down to starting at 0 prior to setup being called - it's not used like that); this is required for any self programming because if `BOOTSIZE = 0` the entire flash is considered "bootloader" section, which can never be written by self programming. For menu options providing specified amounts of writable flash at high addresses, that is made available by configuring `CODESIZE` fuse. Your code must be running from the `APPCODE` section, not the `APPDATA` one. For the "Anywhere" option, no part of the code is marked `APPDATA` - it's all `APPCODE`, and in order to make that writable, nearly the same trick that was used for bootloader-dependent version - an spm z+ ret sequence is created and given attributes such that it will be placed in the lowest page of flash (recall that that's bootloader section) containing inline assembly with a label which the compiler is miraculously able to pick up on and direct a call instruction to.

I've created an issue for discussion of UX and what the API should be like here:
[Feedback](https://github.com/SpenceKonde/DxCore/issues/57)

## Warnings
This is new, and there ~may~ will be future changes to the API.

This library also has **NO GUARD RAILS ANYWHERE** - you can erase the running app. You can scribble over any part of the flash other than the bootloader section and the very last page of flash (which is reserved for future use by the core to optimize performance via lookup tables)

If running at low voltage or on batteries, consider using brown-out detection to prevent flash corruption if your batteries are dying and it tries to write to the flash.

The `FLASHWRITE_FAIL` code is returned when we actually attempt interacting with the NVM controller to write or erase flash, but after that, the NVMCTRL.STATUS showed an error. I would be surprised if this wound up happening with this library.

## Feedback Wanted
Please let me know if this does or does work out for you - In addition to bug reports which I always welcome (though I think this may be free of true bugs - though it most definitely has plenty of "features" that could be alarming to the uninitiated, and it is "the responsibility of the user to ensure that" many considerations are observed. I am particularly interested in whether the API provided is fit for purposem, and if not, what I should to to fix it.

## Core reserved area
The core reserves the top page of flash unless disabled from the tools menu (1.4.5+) Future updates planned for the 1.5.0 timeframe require a block in the top corner of the flash in order to permit specific optimizations.
(it will contain a number of variant-specific pin mapping information, where knowing where the values are, and where the boundaries of the low byte allows several key optimizations not otherwise possible - this speeds up turning on and off PWM and will be key to avoiding undue overhead for giving TCD the same treatment as TCA when we get working hardware.

## Usage Guide

### Checking Compatibility
In order for this to work, if using Optiboot, the version of optiboot must be since the 1.3.0 releases. The `Flash.checkWritable()` function can be used to determine if we can write to flash with the current configuration on the part. If it fails, you need to correct this, probably by burning the new bootloader.

If not using optiboot, there are requirements of the fuse settings, though these should be automatically handled.

All functions provided assume that, if using a bootloader, the bootloader is compatible. Otherwise it may behave in an unexpected manner (with an incompatible bootloader, it will probably just reset; without any bootloader, it will just return the error code). So, unless certain that the bootloader currently installed is from 1.3.0 (1/18/2021 or later - earlier 1.3.0-dev bootloaders don't support this) call this to make sure.

### Return Codes
All of these functions return a status code; 0 is a success. The high byte indicates the broad category of problem (such as a bad argument to the function call, or an error reported by NVM controller), while the low byte identifies the specific error condition. All of the errors are described below.

### Don't trash your app
The application code (your sketch) is uploaded starting from `0x00200` (the end of the bootloader) and extends towards higher addresses. If you have placed constants in memory-mapped flash using the `PROGMEM_MAPPED` declaration, they will be placed starting at `0x18000` (128k parts) or `0x08000` (64k parts), likewise extending upwards. Thus, to avoid bumping into your sketch code, start from ~the high end of the address space and work downwards~ 512b before the high end of the address space (from 1.5.x onwards, top page is planned to be used for core reserved functionality) - unless you have a ton of `PROGMEM_MAPPED` that you need to avoid, but your sketch is still small - in that case you could use the section of flash before the mapped section (`0x10000~0x18000` on 128k parts).

### Don't write NVM from an ISR
Surprisingly, this code doesn't need interrupts disabled - *unless those interrupts also write to EEPROM or Flash*. If you are writing to the NVM from ISRs, you are doing something wrong!
Considering widely known best practices for Arduino, I would argue that if you were trying to wrtite to the EEPROM or Flash from within an ISR, you deserve what you get! Remember how "ISRs should run fast"? Writing a word of flash takes 70us according to the datasheet. At 24 MHz, that's 1680 clock cycles. Per byte or word written. That is not fast - but it could be worse. You could be writing EEPROM: it is spec'ed at **11ms per ERASE-WRITE byte** (`NVMCTRL.CTRLA` = `NVMCTRL_CMD_EEERWR_gc`) -  pointing out that that's around a quarter million system clocks (at 24 MHz) probably doesn't contribute much to the discussion; if "11ms" didn't dissuade you, "a quarter million clocks" probably won't either...

### An added benefit of the `PROGMEM_MAPPED` section
Since it's memory mapped... you can cast to a pointer and use it like a normal constant variable!
```c++
Flash.writeWord(myAddress,1234);
if (myAddress > (PROGMEM_SIZE - 0x8000)) {
  // For sake of example, check that the address really is mapped...
  if (PROGMEM_SIZE == 0x8000 ) {
    myAddress+=0x8000;
    // 32k parts map program memory address 0x0000~0x7FFF onto
    // 0x8000~0xFFFF, so gotta correct for that here
  }
  // 64k parts map 0x8000~0xFFFF onto 0x8000~0xFFFF and
  // 128k parts map 0x18000~0x1FFFF - but when you cast
  // that to a uint16_t, that stray bit is truncated and
  // everything works the same.
} else {
  // The parts default to the highest 32k of flash being mapped (DxCore doesn't change that)
  // If you're here, then myAddress wasn't in the mapped section...
  Serial.println("Umm, not going to work... ");
}

uint16_t* newPtr=(uint16_t*)(uint16_t)myAddress;
Serial.println(*newPtr);
```

Note that it works in the other direction too... but it's less convenient. You could, for example:
```c++
uint32_t targetAddress &hasWrongValue;
if (targetAddress < 0x8000) {
  Serial("This is in RAM, not mapped flash!")
} else {
  if (PROGMEM_SIZE == 0x8000) {
    targetAddress-=0x80000; // lose the high bit to convert mapped addresses to flash addresses...
  } else if (PROGMEM_SIZE == 0x20000) {
    targetAddress+=0x10000; // gotta add 0x10000 to it to get right address on 128k parts...
  }
  // Now - if we have any other variables in PROGMEM_MAPPED... we have a problem now. We can't
  // erase just our variable without trashing the whole 512b page (we don't know where any other
  // variables are, either - though the linker fills from the lowest address upwards; it's likely
  // that if you have any other variabnle s in MAPPED PROGMEM, they're on the chopping block.
  // Of course, if you don't erase, then all you can store is bitwise and of existing data
  // and new data...
  // Assuming there's nothing else there though, you could just:
  Serial.println(Flash.erasePage(targetAddress));
  // Otherwise, you'd have to read the whole 512 byte page, modify the relevant bytes, erase that page, then write it back.
}
```

### Reading Values
As noted above, data stored in the memory mapped section of flash can be read directly, just like that.

For addresses from 0x0000 through 0xFFFF, you can use the `pgm_read_*_near()` macros from `avr/pgmspace.h` to access it - anything beyond that requires  `pgm_read_*_far()`. Particularly relevant for the area from 0x10000 where the `pgm_read_*_near()` macros stop, to to 0x18000 where the mapped progmem starts by default). One could change the FLMAP bits of NVMCTRL.CTRLB, but the interaction of variables declared `PROGMEM_MAPPED` and the optimizer has no guarantee of correctness.

### Last warnings
* If you're going to be fiddling with `PROGMEM_MAPPED` variables, don't declare them `const` - unlike normal `PROGMEM` declarations, the compiler lets you do that.
* There is definitely the potential for the compiler to make heinous "optimizations" that result in values being assumed or cached if you're modifying variables declared `PROGMEM_MAPPED` and `const`. This could be not reading it while it's been changed back in flash - or assuming it would never change at all, and throwing out code to test for or handle different values. So - be careful here, test stuff, etc. I have NOT done much testing!
  * Worst case, declaring them `volatile` would keep the compiler from opitizing in a way that break stuff.
* Optiboot is lazy! **When a new sketch is uploaded through Optiboot, it only erases what the new sketch requires for code + `PROGMEM`/`PROGMEM_MAPPED` variables!** There are pieces of your old code and data sitting after you upload something new. Be prepared to - if your code requires that bytes not written yet be 0xFF - you'll need to be sure that your sketch does erase it after first upload.

## API

### Compatibility Check - Flash.checkWritable()

```c++
uint8_t Flash.checkWritable()
```

```c++
uint8_t result = Flash.checkWritable();
switch (result) {
  case FLASHWRITE_OK:
    Serial.println("Flash can be written from application");
    break;
  case FLASHWRITE_OLD:
    Serial.println("Bootloader is pre-1.3.0 - must be updated");
    break;
  case FLASHWRITE_FUSES:
    Serial.println("Sketch was compiled for bootloader-free operation across limited sections of the flash, but fuses are set such that there is no writable flash.");
    break;
}
```

```c++
Return Values:
FLASHWRITE_OK           = 0x00 // Bootloader present and supports this.
FLASHWRITE_OLD          = 0x01 // If bootloader version is old, we know it won't work.
FLASHWRITE_NOBOOT       = 0x10 // No bootloader was found (Returned only if USING_OPTIBOOT = 1; the most liekly reason to see this is a third party IDE that requires manually specifying defines like this.
FLASHWRITE_DISABLED     = 0x02 // Bootloader would support it, but it was explicitly disabled (APP_NOSPM)
FLASHWRITE_UNRECOGNIZED = 0x03 // Some weird unrecognized bootloader is installed
FLASHWRITE_FUSES        = 0x14 // Fuse settings do not support writing to flash without a bootloader (non-Optiboot configurations only).
```

If ((returnvalue) & 0x10), we are not using a bootloader to write the flash, but rather a "fake" bootloader section (for unrestricted flash write) or we are using the app data section.

### Page Erase - Flash.erasePage()

```c++
uint8_t Flash.erasePage(uint32_t address, uint8_t size = 1);
```

```c++
// Erase the second-to-last page on an AVR128DA/DB
// Any target address 0x1FC00~0x1FDFF will do the same thing jere
uint8_t returnval = Flash.erasePage(0x1FC00);
if (returnval) {
  Serial.print("Error: ");
} else {
  Serial.print("Success!")
}

// Erase the last 16k of flash on an AVR128DA/DB, using a maximum size multi-page erase
// Any target address 0x1C000~0x1FFFF will do the same thing.
uint8_t returnval = Flash.erasePage(0x1C000,32);
if (returnval) {
  Serial.print("Error: ");
} else {
  Serial.print("Success!")
}

```

This will erase the page of flash speecified - this ignores the lower 9 bits (page size is 512 bytes on these parts). These parts support multipage erase; valid options for size are 1, 2, 4, 8, 16, or 32. Be aware that there are no "guard rails" here, other than the fact the bootloader cannot erase itself - that is to say, you can erase the running application out from under yourself if you have write to anywhere permitted! You should take care not to do that, unless a self-destructing app is desired.

**Errata warning** If you have APPDATA write protected, there is an errata impacting some parts which allows a multipage erase for which the first page is not write protected to be performed which erases part or all of the write-protected APPDATA section.

```c++
Return Values:
FLASHWRITE_OK                 = 0x00 // Requested flash erased.
FLASHWRITE_BADADDR            = 0x40 // An invalid address was specified (eg, location not on the flash)
FLASHWRITE_BADSIZE            = 0x41 // An invalid size was specified for a multipage erase.
FLASHWRITE_PROTECT            = 0x42 // Address points to a location that is not writable (ex, the bootloader)
FLASHWRITE_RESERVED_BY_CORE   = 0x48 // In a future version of the core either the first or last page of the mapped flash may be reserved, Decision TBD.
// It would poermit some dramatic optimizations of basic core API stuff while reducing SRAM footprint of the core.
FLASHWRITE_BOOTSECTOR         = 0x49 // You are attempting to write to the fist page of the flash. This is always BOOTCODE (this core only ever has BOOTSIZE = 1
// or BOOTSIZE = 0. if BOOTSIZE = 0, the whole flash is BOOTCODE and you can't use this library - and then you would have gotten FLASHWRITE_FUSS)
FLASHWRITE_NOBOOT             = 0x10 // In 1.3.0, bootloader is required for this library.
FLASHWRITE_FAIL_x             = 0x8x // This was attempted, but NVMCTRL.STATUS showed an error.
```


### Writing Words or Bytes - Flash.write_____()
```c++
uint8_t Flash.writeWord(uint32_t address, uint16_t data)
uint8_t Flash.writeByte(uint32_t address, uint8_t data)
```

```c++

uint16_t toWrite = 0xFEED;
uint8_t returnval=Flash.writeWord(0x1FFFE,toWrite);
if (retval) {
  Serial.print("Error: ");
}

```

This writes a single word to the flash at the location specified. You can only write words, not bytes, and you are responsible for ensuring that the the address has been erased. Writing locations that are already written will result in the flash at that address storing the value `OldEData & NewData` - weiting the flash can only turn 1's into 0's, and only an erase cycle can turn 0's into 1's.

The address is byte oriented. When writing words, you must align with word boundaries (address must be even).

`Flash.writeByte` writes a byte at a time; this is done by writing `0xFF__` or `0x__FF` (where the __ is your data) as a word, so the "empty" byte will not change anything that might be in the "other half" of the word.

```c++
Return Values:
FLASHWRITE_OK                 = 0x00 // Requested flash written
FLASHWRITE_BADADDR            = 0x40 // An invalid address was specified (eg, location not on the flash)
FLASHWRITE_BADSIZE            = 0x41 // An invalid size was specified
FLASHWRITE_PROTECT            = 0x42 // Address points to a location that is not writable (ex, the bootloader)
FLASHWRITE_ALIGN              = 0x43 // Address must be aligned with word boundaries when calling writeWord
FLASHWRITE_RESERVED_BY_CORE   = 0x48 // In a future version of the core either the first or last page of the mapped flash may be reserved, Decision TBD.
// It would poermit some dramatic optimizations of basic core API stuff while reducing SRAM footprint of the core.
FLASHWRITE_BOOTSECTOR         = 0x49 // You are attempting to write to the fist page of the flash. This is always BOOTCODE (this core only ever has BOOTSIZE = 1
// or BOOTSIZE = 0. if BOOTSIZE = 0, the whole flash is BOOTCODE and you can't use this library - and then you would have gotten FLASHWRITE_FUSS)
FLASHWRITE_NOBOOT             = 0x10 // In 1.3.0, bootloader is required for this library.
FLASHWRITE_FAIL_x             = 0x80 // This was attempted, but NVMCTRL.STATUS showed an error
```

### Writing Arrays - Flash.writeWords(), Flash.writeBytes()
```c++
Flash.writeWords(uint32_t address, uint16_t* data, uint16_t length)
Flash.writeBytes(uint32_t address, uint8_t* data, uint16_t length)
```

This will write the supplied data to flash, starting from `address`.  Length is the number of words being written (the number of elements of the uint16 array) If you need to write other datatypes, cast the pointer you're passing to a `uint16_t *` and be sure that you recalculate the length.

When writing an array of words with `Flash.writeWord`, the address must be even, (as we write one word at a time), but there is no alignment requirement beyond that - writes can even cross the 64K boundary. No erase operation is performed by `Flash.writeWords()`, and the array passed must have at least `length` elements. `Flash.writeBytes` has no alignment requirement.


```c++
Return Values:
// Ad writeByte and writeWord, plus:
FLASHWRITE_TOOBIG             = 0x44 // If the length is correct, this will overflow or cross the 64k barrier.
FLASHWRITE_0LENGTH            = 0x46 // If you passed in length of 0.
```

## Reading - Flash.readByte(), Flash.readWord()

```c++
Flash.readByte(uint32_t address)
Flash.readWord(uint32_t address)
```

As the name implies, these will read either a byte or word from the flash - this is the same as using the `pgm_read_...` macros (in fact, that's what this library uses) - this is just a convenience function (also makes it easier to write code that works without modification across different flash sizes - >64k parts require the `_far` version, which isn't provided (for obvious reasons) on parts with less than 64k flash.)

## Utility - Flash.mappedPointer(), Flash.flashAddress(), Flash.lock()
```c++
Flash.lock(FLASH_LOCK_TYPE_t lock);
Flash.lock();
Flash.setMap(FLASH_MAP_t map);
```
With an argument, lock() will lock the specified section or try to.
```c++
  LOCK_FLASH_MAP               = 0x80 // Prevent changes to the flash mapping
  LOCK_APPCODE                 = 0x01 // Prebent changes to the appcode section
  LOCK_APPDATA                 = 0x04 // Prevent changes to the appdata section
```
With or without arguments, it will return the current value of the register, including additional information (all bitwise or'ed together)
```c++
  LOCK_FLASH_MAP               = 0x80 // Prevent changes to the flash mapping
  LOCK_APPCODE                 = 0x01 // Prebent changes to the appcode section
  LOCK_APPDATA                 = 0x04 // Prevent changes to the appdata section
  LOCK_BOOTREAD                = 0x02 // The boot section is read-protected. This is read only from within the app and prevents any read or execution of the boot section.
  // This means that any SPM from app menu option permitting writes anywhere will not work.
  // If an attempt to lock something that is invalid is attempted, an error will be returned.
  LOCK_INVALID                 = 0xFF // The requested section is a valid section or sections
```
Likewise setMap() will attempt to configure the current map if given with an argument, and without one, it will simply return the current value.
Note that a future version of the core may have an option to lock the flash map (tied into the above mentioned feature that would take one page of mapped flash for core functionality)

```c++
  // The return value will also include the current flash map setting
  FLASH_MAP3                   = 0x30 // Default - the highest section flash is mapped, the default. 128k parts only.
  FLASH_MAP2                   = 0x20 // 0x10000 - 0x17FFF mapped. 128k parts only
  FLASH_MAP1                   = 0x10 // 0x08000 - 0x0FFFF mapped. Default on 64k parts
  FLASH_MAP0                   = 0x00 // 0x00000 - 0x07FFF mapped, or a 32k part.
  FLASH_MAP3_LOCKED            = 0xB0 // Flash map is set to 3, and is locked.
  FLASH_MAP2_LOCKED            = 0xA0 // Flash map is set to 2, and is locked.
  FLASH_MAP1_LOCKED            = 0x90 // Flash map is set to 1, and is locked.
  FLASH_MAP0_LOCKED            = 0x80 // Flash map is set to 0, and is locked.
  FLASH_MAP_INVALID            = 0xFF // An invalid flash mapping was requested for this part, or this part has 32k or less flash and hence no mapping to speak of.
```

```c++
Flash.mappedPointer(uint32_t address)
Flash.flashAddress(uint8_t* ptr)
```
**Always check that pointers and addresses aren't `NULL`/zero***
Flash.mappedPointer(), when passed an address in program memory that lies within the section currently mapped to memory, will return a `uint8_t *` pointing to it. Otherwise, it will return a NULL pointer.
Flash.flashAddress() will go the opposite direction - passed a pointer to a location in mapped flash, it will return the address in flash that it's pointing to, considering current FLMAP.


### FLASHWRITE_FAIL_x codes
In the event that an attempted flash write to a board with a compatible bootloader fails, the applicable function will return `FLASHWRITE_FAIL_x` which has a numeric value between `0x80` and `0x87`. The three low bits are the values of the `ERROR` bitfields within `NVMCTRL.STATUS` (right-shifted 4 places, of course) at the conclusion of the failed write attempt; per the datasheet, these represent:

```c++
0x80 // FLASHWRITE_FAIL - this is never returned as an error code - bitwise and with this to check for any of these errors.
0x81 // FLASHWRITE_FAIL_INVALID - Invalid Command. Whatever command was wrtten to the NVMCTRL.CTRLA register was invalid. Likely using this library on an unsupported part.
0x82 // FLASHWRITE_FAIL_PROTECT - Write Protect
0x83 // FLASHWRITE_FAIL_COLLISION - Command Collision (command not written to 0 between commands. This should not be possible using this library!)
0x84 // FLASHWRITE_FAIL_ONGOING - Ongoing Programming (Not in datasheet, but mentioned in io headers. Likely for future use when RWW is supported
0x85 // FLASHWRITE_FAIL_5 - NVMCTRL.STATUS returned an unknown error, please report!
0x86 // FLASHWRITE_FAIL_6 - NVMCTRL.STATUS returned an unknown error, please report!
0x87 // FLASHWRITE_FAIL_7 - NVMCTRL.STATUS returned an unknown error, please report!

Appropriate use might look like:
if (code_returned & FLASHWRITE_FAIL) {
  Serial.println(code_returned);
}


Or even just:
if (code_returned != FLASHWRITE_OK) {
  Serial.println(code_returned);
}

```

If you receive any of these, and it is not apparent why it is not working, please do not hesitate to report it as a Github issue.


## Known Limitations
* Library does not verify that flash was written correctly, or that it targeted flash that had been erased.
* Library leaves NVMCTRL.CTRLA set; In supported configurations, this should be safe except for the case of user code that has to issue other `NVMCTRL` commands - and doesn't defensively set to `NOOP` first. That's probably a bad course of action, as it places faith in other code behaving the way you would - especially since, as it happens, other code in the wild *doesn't* behave that way I don't think the fact that it `NVMCTRL.CTRLA` is left on a command is in and of itself a risk, since only the one SPM instruction on that one page of flash can execute it; You could only get there via JMP/RJMP/CALL/RCALL - which are targeted at compile time (ie, they are your entry point, or the libraries entry point, and set the command register anyway)... or they would get there from some "wild pointer" that ends up pointing an IJMP or ICALL there - but in that case, the pointer that directs those is the Z pointer, which also targets SPM - so the Z-pointer would be pointing to the first page of flash... which cannot write to itself!
* Library provides no facility to "update" a page of memory. This would probably be useful.
* Library lacks convenience functions and defines for taking full advantage of PROGMEM_MAPPED and the like...
* No provision for wear leveling (I would suggest this be a build on top of a library like this, once the API stabilitzes.


## Future developments
* I am sympathetic to the idea of "guard rails" - could a variable be placed at the end of the application, and another at the end of the section of the mapped flash constants declared by the application? If we could get that in (I can add linker commands to the platform.txt), the addresses of those could be used to set "keep out" zones for the flash writing library...
* As noted above, a flashUpdatePage() of some sort would be good... thinking about how to implement it robustly...
* Please point out bugs, defects and poor decisions. As noted above, the API may change in future versions; I hope to talk with some users, make sure the API and such is solid, and then remove that warning.
