#ifndef FLASHWRITE_H
/* flashWrite.h for DxCore 1.3.0
 * A library to easily write to the flash of an AVR Dx-series part from within an Arduino sketch
 * Requires Optiboot from DxCore 1.3.0.
 *
 * This is a non-class-interface'ed version of the library.
 * See also Flash, which is the same exact internals, but as a class.
 */

uint8_t flashCheckWritable();

uint8_t flashErasePage(const uint32_t address, const uint8_t size = 1);

uint8_t flashWriteWord(const uint32_t address, const uint16_t data);

uint8_t flashWriteByte(const uint32_t address, const uint8_t data);

uint8_t flashWriteWords(const uint32_t address, const uint16_t* data, uint16_t length);


typedef enum FLASHWRITE_RETURN_VALUES
{
  FLASHWRITE_OK             = (0x00),

  /* 0x10 - Non-optiboot problem   */
  FLASHWRITE_NOBOOT         = (0x11),
  FLASHWRITE_FUSES          = (0x14),
  /* May be handled in future version
  If we're trying to write without a
  bootloader, the APPDATAWP bit in
  NVMCTRL.CTRLB would block all
  attempts at writing the flash.
  FLASHWRITE_APPDATAWP		= (0x1F),*/
  FLASHWRITE_NYI            = (0x1F),


  /* 0x20 - Problem with Optiboot  */
  FLASHWRITE_OLD            = (0x21),
  FLASHWRITE_DISABLED       = (0x22),
  FLASHWRITE_UNRECOGNIZED   = (0x23),
  /* May be handled in future version
  FLASHWRITE_APPCODEWP		= (0x2E),
  Currently if you try to write to
  APPCODE section, but it's write
  protected, you'll get the PROTECT
  error code below. This should maybe
  be given if checkFlash() is called,
  and the combination of WP bits and
  CODESIZE fuse is such that none of
  the flash can be written.
  FLASHWRITE_BOOTRP			= (0x2F),
  Currently if BOOTRP is set, the
  you'll just get FLASHWRITE_UNRECOGNIZED
  which is accurate, since BOOTRP is
  not a fuse, but a bit set by the
  bootloader; one which supported
  this functionality surely wouldn't
  be setting that bit!			       */

  /* 0x30 -Can occur in either mode*/
  FLASHWRITE_SPM_NOT_USED   = (0x31),
  /* If bootloader/vector has
     SPM instead of SPM Z+
  FLASHWRITE_NO_Z_INC       = (0x32),
  Don't think these exist!         */

  /* 0x40 - Bad argument to call   */
  FLASHWRITE_BADARG         = (0x40),
  FLASHWRITE_BADADDR        = (0x41),
  FLASHWRITE_BADSIZE        = (0x42),
  FLASHWRITE_PROTECT        = (0x43),
  FLASHWRITE_ALIGN          = (0x44),
  FLASHWRITE_TOOBIG         = (0x45),
  FLASHWRITE_0LENGTH        = (0x46),

  /* 0x80 - NVMCTRL complained     */
  FLASHWRITE_FAIL           = (0x80),
  FLASHWRITE_FAIL_INVALID   = (0x81),
  FLASHWRITE_FAIL_PROTECT   = (0x82),
  FLASHWRITE_FAIL_COLLISION = (0x83),
  FLASHWRITE_FAIL_ONGOING   = (0x84),
  FLASHWRITE_FAIL_OTHER_5   = (0x84),
  FLASHWRITE_FAIL_OTHER_6   = (0x84),
  FLASHWRITE_FAIL_OTHER_7   = (0x84)
} FLASHWRITE_CODE_t;


#endif
