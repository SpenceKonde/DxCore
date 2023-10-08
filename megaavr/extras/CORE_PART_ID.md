# CORE_PART_ID
For advanced users, CORE_PART_ID may be useful; Unless you actually need this level of detail, or need a single byte capturing all the information of the part (if two bytes identifying the part are fine too, *don't use this* - use `((SIGNATURE_1 << 8) + SIGNATURE_2)` - this is unique across the whole AVR product line, excepting a few cases of very closely related parts, mostly classic AVRs with and without the A at the end of the part number), and is officially sanctioned. `SIGNATURE_1` indicates flash size, and `SIGNATURE_2` values are assigned sequentially by release date; `SIGNATURE_0` is not useful for identifying parts, since it is 0x1E on every AVR device ever manufactured. The CORE_PART_ID might also be useful as part of some complicated self-organizing protocol in which parts must describe their hardware to other 8-bit AVR with similarly constrained resources, likely over a low bandwidth connection of some sort.

CORE_PART_ID may also be particularly useful when checking for errata, which are usually specific to family and flash size, but not pincount - but you still need to check `SYSCFG.REVID` (at runtime) to see if it's impacted or not. Well, hopefully, some day, when the errata gets fixed....

## Be careful with macro names
Do not get them confused with things you test with `#ifdef` or `#if defined()` to find out about the chip that the code is being compiled for.

## Flash size and Series
The 5 high bits identify the flash size and series
| Flash   |  DA  |  DB  |  DD  |
|---------|------|------|------|
|    128k | 0x20 | 0x28 |  --  |
|     64k | 0x10 | 0x18 | 0x60 |
|     32k | 0x00 | 0x08 | 0x50 |
|     16k |  --  |  --  | 0x40 |
Where high bit is 1, it is a not a Dx part.
The following are defined to help mastching these:
* `ID_MASK_SERIES`
* `ID_AVR_DA`
* `ID_AVR_DB`
* `ID_AVR_DD`

## Pincount
The low 3 bits identify pincount:

| Pincount | pin bits |        equal to |
|----------|----------|-----------------|
|  8       |     0x00 |     `ID_8_PINS` |
| 14       |     0x01 |    `ID_14_PINS` |
| 20       |     0x02 |    `ID_20_PINS` |
| 24       |     0x03 |    `ID_24_PINS` |
| 28       |     0x04 |    `ID_28_PINS` |
| 32       |     0x05 |    `ID_32_PINS` |
| 48       |     0x06 |    `ID_48_PINS` |
| 64       |     0x07 |    `ID_64_PINS` |
So AVR64DA48 is 0x16, and AVR128DB64 is 0x2F. Masks and defines to test against this are provided:

Use `ID_MASK_PINS` to match only the pincount bits and compare to the above constants.

## Correct usage
```c++
#if (CORE_PART_ID & ID_MASK_SERIES) == ID_AVR_DA
Serial.println("I am a DA-series"); //RIGHT - will be included when the series ID bits are equal to ID_AVR_DA (which happens to be 0b000).
#endif

#if (CORE_PART_ID & ID_MASK_FLASH) == 0
Serial.print("I am the smallest-flash member of this series ");  //RIGHT - will be included when the flash bits are 0, indicating the smallest available version of the part.
Serial.print(PROGMEM_SIZE);
Serial.println(" bytes.")
#endif


#if (CORE_PART_ID & ID_MASK_PINS) > ID_28_PINS
Serial.println("I have more than 28 pins"); // WORKS, but likely BAD - You should probably test  the number of I/O pins with NUM_DIGITAL_PINS, which works on all cores, classic or modern.
#endif

#if (CORE_PART_ID & (ID_MASK_FLASH && ID_MASK_SERIES)) == 0x20  // RIGHT - checking
#warning "This library uses 'st' to self-program the flash, AVR128DA errata mean that on the A8 silicon revision, the BOOTCODE section is mirrored across all flash sections, writes to the first page(s) in a flash section will fail on these devices. Silicon revision can only be detected at runtime."
// code that checks silicon revision and - if possible - communicates that the hardware is unsupported.
// Note that it really would be better to just implement the flash writing with SPM instead of ST, which is not effected by that bug.
#endif
```

## Incorrect usage
```c++
#if defined(ID_20_PINS)
Serial.println("I have 20 pins!"); // WRONG - ID_20_PINS is always defined as 0x02.
#endif

#if ID_48_PINS && ID_AVR_DA
Serial.println("I am an AVR DA-series with 48 pins"); // WRONG - Both are always defined,  ID_48_PINS as 0x06, and ID_AVR_DA as 0.
#endif
```
