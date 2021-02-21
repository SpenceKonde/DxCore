#ifndef FLASH_H
#define FLASH_H
//*INDENT-OFF*
/* Flash.h for DxCore 1.3.2
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
      return (uint16_t*) mappedPointer(address);
    }
    uint16_t* mappedPointerWord(const uint32_t address);
      // juat syntactic
      return (uint16_t*) mappedPointer(address);
    }
    */
};

extern FlashClass Flash;

typedef enum FLASHWRITE_RETURN_VALUES {
  FLASHWRITE_OK                = (0x00),
  /* 0x10 - Non-optiboot problem      */
  FLASHWRITE_NOBOOTSIZE        = (0x11),
/* If BOOTSIZE = 0x00, then the whole
 * flash is BOOTCODE and none is
 * writable via self programming
 */
  FLASHWRITE_NOCODESIZE        = (0x12),
/* If CODESIZE = 0x00, then the whole
 * flash is APPCODE other than the boot
 * section, but we're not using magic
 * entry point because SPM_FROM_APP is
 * not -1 */
  FLASHWRITE_CFGMISMATCH       = (0x13),
/* CODESIZE fuse and SPM_FROM_APP do
 * not agree about how much flash is
 * writable */
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
 */
  FLASHWRITE_NOENTRY           = (0x15),
/* This means SPM_FROM_APP=-1 and no
 * entry point in first page as above
 * but we couldn't find it anywhere and
 * looked all the way to 0x8000.
 */
  /* 0x20 - Problem with Optiboot     */
  FLASHWRITE_NOBOOT            = (0x20),
  FLASHWRITE_OLD               = (0x21),
  FLASHWRITE_DISABLED          = (0x22),
  FLASHWRITE_UNRECOGNIZED      = (0x23),

  /* 0x30 -Can occur in either mode   */
  FLASHWRITE_SPM_NOT_USED      = (0x31),
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
  FLASHWRITE_BADADDR           = (0x41),
  FLASHWRITE_BADSIZE           = (0x42),
  FLASHWRITE_PROTECT           = (0x43),
  FLASHWRITE_ALIGN             = (0x44),
  FLASHWRITE_TOOBIG            = (0x45),
  FLASHWRITE_0LENGTH           = (0x46),

  /* 0x80 - NVMCTRL complained        */
  FLASHWRITE_FAIL              = (0x80),
  FLASHWRITE_FAIL_INVALID      = (0x81),
  FLASHWRITE_FAIL_PROTECT      = (0x82),
  FLASHWRITE_FAIL_COLLISION    = (0x83),
  FLASHWRITE_FAIL_ONGOING      = (0x84),
  FLASHWRITE_FAIL_OTHER_5      = (0x85),
  FLASHWRITE_FAIL_OTHER_6      = (0x86),
  FLASHWRITE_FAIL_OTHER_7      = (0x87)
} FLASHWRITE_CODE_t;


#endif
