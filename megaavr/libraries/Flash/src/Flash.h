#ifndef FLASH_H
#include <Arduino.h>
#define FLASH_H
//*INDENT-OFF*
/* Flash.h for DxCore 1.5.0
 * A library to easily write to the flash of an AVR Dx-series part from within an Arduino sketch
 * Requires Optiboot from DxCore 1.3.0+ for use in bootloader-mode.
 * Requires DxCore 1.3.1+ with Flash Writing option selected from tools menu for non-bootloader-mode.
 * This is part of DxCore - github.com/SpenceKonde/DxCore
 * This is free software, GPL 2.1 see ../../../LICENSE.md for details.
 */

class FlashClass {
  public:
    uint8_t checkWritable();
    uint8_t  erasePage(const uint32_t address, const uint8_t   size = 1);
    uint8_t  writeWord(const uint32_t address, const uint16_t  data);
    uint8_t  writeByte(const uint32_t address, const uint8_t   data);
    uint8_t writeWords(const uint32_t address, const uint16_t* data,  uint16_t length);
    uint8_t writeBytes(const uint32_t address, const uint8_t*  data,  uint16_t length);
    uint8_t   readByte(const uint32_t address);
    uint16_t  readWord(const uint32_t address);
    uint32_t flashAddress(uint8_t* mappedPtr);
    uint8_t* mappedPointer(const uint32_t address);
    /*
    // would this be better? I think maybe it would?
    void* mappedPointer(const uint32_t address);
    uint8_t* mappedPointerByte(const uint32_t address) {
      // juat syntactic
      return (uint8_t*) mappedPointer(address);
    }
    uint16_t* mappedPointerWord(const uint32_t address);
      // juat syntactic
      return (uint16_t*) mappedPointer(address);
    }
    */
};

extern FlashClass Flash;

#define NO_CORE_RESERVED

typedef enum FlashLockZones {
  LOCK_FLASH_MAP               = (0x80),
  LOCK_APPCODE                 = (0x01),
  LOCK_BOOTREAD                = (0x02),
  LOCK_APPDATA                 = (0x04)
} FLASH_LOCK_TYPE_t;

typedef enum FLASHWRITE_RETURN_VALUES {
  FLASHWRITE_OK                = (0x00),
  /* 0x10 - Non-optiboot problem      */
  FLASHWRITE_NOBOOTSIZE        = (0x11),
/* If BOOTSIZE = 0x00, then the whole
 * flash is BOOTCODE and none is
 * writable via self programming.
 * Note that this should not be possible
 * except using a third party IDE that
 * ignores parts of platform.txt/boards.txt.
 * We always write the BOOTSIZE and
 * CODESIZE fuses - this also applies to the next two.
 */
  FLASHWRITE_NOCODESIZE        = (0x12),
/* If CODESIZE = 0x00, then the whole
 * flash is APPCODE other than the boot
 * section. A boot section was detected
 * but the selected menu option is
 * set to something other than allow
 * writes to anywhere, and hence we are
 * limited to APPDATA, but the fuses are
 * set to not have an APPDATA section
 * Should not be possible:
 */
  FLASHWRITE_CFGMISMATCH       = (0x13),
/* CODESIZE fuse and SPM_FROM_APP do
 * not agree about how much flash is
 * writable. Should not be possible, because
 */
  FLASHWRITE_BOGUSENTRY        = (0x14),
/* This means SPM_FROM_APP=-1 so there
 * should be an entry point in first
 * 512b of flash. There was no spm z+
 * instruction in the first 512b, but
 * there is later on. Sketch -> Export
 * Compiled Binary, zip/tar the .ino,
 * .hex, and .lst and submit with issue
 * in github or email to
 * spencekonde@gmail.com
 * This indicates that the magic routine
 * is being generated, but you've found
 * a way to outfox the tricks I use to
 * ensure it ends up at an address where
 * it can be used from.
 */
  FLASHWRITE_NOENTRY           = (0x15),
/* This means SPM_FROM_APP=-1 and no
 * entry point in first page as above.
 * But to provide better debug output
 * we kept looking for a misplaced one,
 * and we couldn't find it anywhere and
 * looked all the way to 0x8000. Follow same
 * instructions as above; it indicates
 * that the magic routine is not being
 * included at all
 */
  /* 0x20 - Problem with Optiboot     */
  FLASHWRITE_NOBOOT            = (0x20),
 /* Library compiled expecting writes to be
  * mediated by a bootloader, but none found */
  FLASHWRITE_OLD               = (0x21),
 /* Library requires a more recent bootloader */
  FLASHWRITE_DISABLED          = (0x22),
 /* The bootloader was intentionally compiled
  * without support for this */
  FLASHWRITE_UNRECOGNIZED      = (0x23),
 /* The bootloader doesn't even look like it
  * could be optiboot */

  /* 0x30 -Can occur in either mode   */
  FLASHWRITE_SPM_NOT_USED      = (0x31),
  FLASHWRITE_NOT_WRITTEN       = (0x31),
/*FLASHWRITE_NO_Z_INC          = (0x32),*/
/*FLASHWRITE_APPCODEWP         = (0x3E),*/
/*FLASHWRITE_BOOTRP            = (0x3F),*/
/* May be handled in future version
 * Currently we aren't making sure their
 * code isn't owning themselves. And if
 * BOOTRP is set, their bootloader set it
 * so: A) we don't know what it is and
 * and B) we know it don't support this!
 */

  /* 0x40 - Bad argument to call      */
  FLASHWRITE_BADARG            = (0x40),
  /* One or more argument was invalid */
  FLASHWRITE_BADADDR           = (0x41),
  /* Address located outside of flash */
  FLASHWRITE_BADSIZE           = (0x42),
  /* Invalid size for erase command */
  FLASHWRITE_PROTECT           = (0x43),
 /* Reserved - future versions will detect
  * hardware protected regions.
  */
  FLASHWRITE_ALIGN             = (0x44),
 /* You must always write to an address
  * that is divisible by 2. Never an
  * odd address. The low bit is NOT ignored
  * like the datasheet explicitly states.
  * Instead if appears to work until you
  * do it across a page boundary, and then
  * writes one of the bytes to the wrong place.
  */
  FLASHWRITE_TOOBIG            = (0x45),
 /* Specified write would run off the
  * end of the flash or into potected sections.
  */
  FLASHWRITE_0LENGTH           = (0x46),
 /* You akked to write something but
  * told us it was 0 length.
  */
  FLASHWRITE_RESERVED_1        = (0x47),
  FLASHWRITE_RESERVED_BY_CORE  = (0x48),
 /* Don't use the last page of flash.
  * Currently the core does not use it for
  * anything, but it is very possible that
  * that will change
  */
  FLASHWRITE_BOOT_SECT         = (0x49),
 /* Even the bootoader can't rewrite to
  * BOOTCODE section of flash.
  * 0x80 - NVMCTRL complained        */
  FLASHWRITE_FAIL              = (0x80),
  // Test & FLASHWRITE_FAIL to test
  // if the NVMCTRL gave an error.
  FLASHWRITE_FAIL_INVALID      = (0x81),
 /* The command written to NVMCTRL.CTRLA
  * is not a valid command.
  */
  FLASHWRITE_FAIL_PROTECT      = (0x82),
 /* The operation could not be completed
  * because the memory section is write
  * protected.
  */
  FLASHWRITE_FAIL_COLLISION    = (0x83),
 /* NVMCTRL reported a command collision. This should not be possibleunless other libraries are interrupting the write process and interacting with NVMCTRL.
  */
  FLASHWRITE_FAIL_WRONGSECTION = (0x84),
 /* An attempt to write to a location that was not flash slipped through the library code.
  */
  FLASHWRITE_FAIL_RESERVED_1   = (0x85),
  FLASHWRITE_FAIL_RESERVED_2   = (0x86),
  FLASHWRITE_FAIL_RESERVED_3   = (0x87)
} FLASHWRITE_CODE_t;


#endif
