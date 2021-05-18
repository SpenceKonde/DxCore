#ifndef Core_Parts_h
#define Core_Parts_h
#include <avr/io.h>

/*
 * Access to flash-mapped program memory!
 * 104 is a 128k part like AVR128DA
 * 102 is a 64k part like AVR64DB, or AVR64DD
 * 103 is a 48k or less part like the AVR32DA, or the AVR16DD - or the tinyAVR 0/1/2-series, or megaAVR 0-series.
 * the importrant difference for 103 parts is that they have the entire flash
 * mapped to the data address space, while 102 and 104 only have a 32k chunk of it mapped.
 * 102's have 2 sections, 104's have 4 sections.
 */
#if (__AVR_ARCH__ == 104)
  #define MAPPED_PROGMEM __attribute__ (( __section__(".FLMAP_SECTION3")))
#elif (__AVR_ARCH__ == 102)
  #define MAPPED_PROGMEM __attribute__ (( __section__(".FLMAP_SECTION1")))
#else
  // __AVR_ARCH__ == 103, so all of the flash is memory mapped, and the linker
  // will automatically leave const variables in flash.
  #define MAPPED_PROGMEM
#endif


/*
 * Compatibility - General Purpose Register names, GPR.GPRn, vs GPIORn vs GPIOn
 * They now appear to have decided they don't like either of these conventions, and are grouping them under a "General Purpose Register"
 * "peripheral". I cannot argue that GPR doesn't make more sense, as there's not really any I/O occurring here (ofc they were referring
 * to the IN and OUT instructions, which can be used on these), but I certainly wouldn't have changed a convention like this. And if I
 * really had to... I would be too ashamed to do it again in just a couple of years because I realized I didn't like the first change
 * much either. Then again, maybe this just brings to mind that old line about talking cookware...
 */

// Pre-Dx-series parts call them GPIORn instead of GPR.GPRn/GPR_GPRn .
#ifndef GPIOR0
  #define GPIOR0 (GPR_GPR0)
  #define GPIOR1 (GPR_GPR1)
  #define GPIOR2 (GPR_GPR2)
  #define GPIOR3 (GPR_GPR3)
#endif
// For a while, these were called GPIO in the i/o headers...
#ifndef GPIO0
  #define GPIO0 (GPR_GPR0)
  #define GPIO1 (GPR_GPR1)
  #define GPIO2 (GPR_GPR2)
  #define GPIO3 (GPR_GPR3)
#endif

// Chip families
// 0b ffssfppp
// ff__f is a 3-bit family code 00__0 is the DA, 00__1 is DB,
// 01__0 is DD.
// ss is flash size; 0 is smallest flash in family, 1 second smallest
// (generally 2x smallest) 2 for next size up, and 3 for an even larger
// one.
// ppp is code for the pincount.
// interestingly enough this range can extend to cover all pincounts used
// in recent times on AVR devices except the 100-pin '2560. There  is
// only one smaller one, the 8-pin of the '85  and 'xy2 - 000
// while the gap at 0x03 is for the 24-pin package.
// I wonder if we will see another 100-pin monster AVR? There are some
// significant issues involved in that (they're out of addresses in
// low IO space for VPORTs), so I'm not going to include a


#define ID_AVR128DA     0x20
#define ID_AVR64DA      0x10
#define ID_AVR32DA      0x00
#define ID_AVR128DB     0x28
#define ID_AVR64DB      0x18
#define ID_AVR32DB      0x08
#define ID_AVR64DD      0x60
#define ID_AVR32DD      0x50
#define ID_AVR16DD      0x40
#define ID_8_PINS       0x00
#define ID_14_PINS      0x01
#define ID_20_PINS      0x02
#define ID_24_PINS      0x03
#define ID_28_PINS      0x04
#define ID_32_PINS      0x05
#define ID_48_PINS      0x06
#define ID_64_PINS      0x07
#define ID_AVR_DB       0x08
#define ID_AVR_DD       0x40
#define ID_AVR_DA       0x00

#define ID_MASK_SERIES  0xC8
#define ID_MASK_FLASH   0x30
#define ID_MASK_PINS    0x07

//#defines to identify part families
#if defined(__AVR_AVR128DA64__) || defined(__AVR_AVR64DA64__)
  #define DA_64_PINS
  #define DX_64_PINS
  #define Dx_64_PINS
  #define CORE_PART_ID_LOW ID_64_PINS
  #define __AVR_DA__
#elif defined(__AVR_AVR128DA48__) || defined(__AVR_AVR64DA48__) || defined(__AVR_AVR32DA48__)
  #define DA_48_PINS
  #define DX_48_PINS
  #define Dx_48_PINS
  #define CORE_PART_ID_LOW ID_48_PINS
  #define __AVR_DA__
#elif defined(__AVR_AVR128DA32__) || defined(__AVR_AVR64DA32__) || defined(__AVR_AVR32DA32__)
  #define DA_32_PINS
  #define DX_32_PINS
  #define Dx_32_PINS
  #define CORE_PART_ID_LOW ID_32_PINS
  #define __AVR_DA__
#elif defined(__AVR_AVR128DA28__) || defined(__AVR_AVR64DA28__) || defined(__AVR_AVR32DA28__)
  #define DA_28_PINS
  #define DX_28_PINS
  #define Dx_28_PINS
  #define CORE_PART_ID_LOW ID_28_PINS
  #define __AVR_DA__
#elif defined(__AVR_AVR128DB64__) || defined(__AVR_AVR64DB64__)
  #define DB_64_PINS
  #define DX_64_PINS
  #define Dx_64_PINS
  #define CORE_PART_ID_LOW ID_64_PINS | ID_AVR_DB
  #define __AVR_DB__
#elif defined(__AVR_AVR128DB48__) || defined(__AVR_AVR64DB48__) || defined(__AVR_AVR32DB48__)
  #define DB_48_PINS
  #define DX_48_PINS
  #define Dx_48_PINS
  #define CORE_PART_ID_LOW ID_48_PINS | ID_AVR_DB
  #define __AVR_DB__
#elif defined(__AVR_AVR128DB32__) || defined(__AVR_AVR64DB32__) || defined(__AVR_AVR32DB32__)
  #define DB_32_PINS
  #define DX_32_PINS
  #define Dx_32_PINS
  #define CORE_PART_ID_LOW ID_32_PINS | ID_AVR_DB
  #define __AVR_DB__
#elif defined(__AVR_AVR128DB28__) || defined(__AVR_AVR64DB28__) || defined(__AVR_AVR32DB28__)
  #define DB_28_PINS
  #define DX_28_PINS
  #define Dx_28_PINS
  #define CORE_PART_ID_LOW ID_28_PINS | ID_AVR_DB
  #define __AVR_DB__
#elif defined(__AVR_AVR64DD32__)  || defined(__AVR_AVR32DD32__) || defined(__AVR_AVR16DD32__)
  #define DD_32_PINS
  #define DX_32_PINS
  #define Dx_32_PINS
  #define CORE_PART_ID_LOW ID_32_PINS | ID_AVR_DD
  #define __AVR_DD__
#elif defined(__AVR_AVR64DD28__)  || defined(__AVR_AVR32DD28__) || defined(__AVR_AVR16DD28__)
  #define DD_28_PINS
  #define DX_28_PINS
  #define Dx_28_PINS
  #define CORE_PART_ID_LOW ID_28_PINS | ID_AVR_DD
  #define __AVR_DD__
#elif defined(__AVR_AVR64DD20__)  || defined(__AVR_AVR32DD20__) || defined(__AVR_AVR16DD20__)
  #define DD_20_PINS
  #define DX_20_PINS
  #define CORE_PART_ID_LOW ID_20_PINS | ID_AVR_DD
  #define __AVR_DD__
#elif defined(__AVR_AVR64DD14__)  || defined(__AVR_AVR32DD14__) || defined(__AVR_AVR16DD14__)
  #define DD_14_PINS
  #define DX_14_PINS
  #define CORE_PART_ID_LOW ID_14_PINS | ID_AVR_DD
  #define __AVR_DD__
#else
  #error "Can't-happen: unknown chip somehow being used"
#endif

#ifdef __AVR_DD__
  #error "The AVR DD series is not supported yet because the datasheet is not available. It should not be possible to see this message, as when boards.txt entries are added, this message would be removed"
  #define ADC_MAXIMUM_PIN_CHANNEL 31
  #define ADC_MAXIMUM_NEGATIVE_PIN 15 /* unconfirmed - io headers are not updated for the ADC */
#else
  #define ADC_MAXIMUM_PIN_CHANNEL 21
  #define ADC_MAXIMUM_NEGATIVE_PIN 15
#endif


#if   (PROGMEM_SIZE == 0x20000 && !defined(__AVR_DD__)) || (PROGMEM_SIZE == 0x10000 && (CORE_PART_ID_LOW & ID_AVR_DD))
  #define CORE_PART_ID (CORE_PART_ID_LOW | 0x20)
#elif (PROGMEM_SIZE == 0x10000 && !defined(__AVR_DD__)) || (PROGMEM_SIZE ==  0x8000 && (CORE_PART_ID_LOW & ID_AVR_DD))
  #define CORE_PART_ID (CORE_PART_ID_LOW | 0x10)
#elif (PROGMEM_SIZE ==  0x8000 && !defined(__AVR_DD__)) || (PROGMEM_SIZE ==  0x4000 && (CORE_PART_ID_LOW & ID_AVR_DD))
  #define CORE_PART_ID (CORE_PART_ID_LOW | 0x00)
#else
  #error "Unrecognized combination of flash size and chip type"
#endif

#define PORTMUX_TCA0 2 //1 = each wave output cannnel can be moved individually, like tinyAVRs


/* after defining the PART_ID, use it to determine which timer output pins exist and give you a define for them. */
#include "device_timer_pins.h"






// These pieces of version numbers get passed in as command line arguments by platform.txt.
#define DXCORE_NUM ((DXCORE_MAJOR<<24)+(DXCORE_MINOR<<16)+(DXCORE_PATCH<<8)+DXCORE_RELEASED)

// Sometimes this define, passed in from command line, has gotten lost entirely.
// However, without it, things don't know that they are working with DxCore.
// So we put in a placeholder so defined(DXCORE) is true.
#ifndef DXCORE
  #define DXCORE "Unknown 1.3.6+"
#endif


 /* HARDWARE FEATURES - Used by #ifdefs and as constants in calculations in
  * the core and in libraries; it is hoped that these are at least somewhat
  * useful to users, as well. These are described in more detail in the
  * README. */

#define ADC_NATIVE_RESOLUTION 12
#define ADC_NATIVE_RESOLUTION_LOW 10
// Maximum burst accumulation
#define ADC_MAXIMUM_ACCUMULATE 128
// Maximum SAMPLEN or SAMPDUR
#define ADC_MAXIMUM_SAMPDUR 0xFF
// ADC Result Size (bits)
#define ADC_RESULT_SIZE 16
// if (ADC_NATIVE_RESOLUTION + Log2(ADC_MAXIMUM_ACCUMULATE)) > ADC_RESULT_SIZE, long accumulations are truncated.
// with maximum accumulation of Dx, for example, 12 + 7 = 19, so the internal representation would be a 19-bit number
// but only the 16 most significant bits are presented in ADC0.RES. analogReadEnh() accounts for this when
// asked to oversample and decimate.
#ifndef __AVR_DA__
  // DB-series and DD-series parts have an INLVL bit on the PINnCTRL registers. If set, pin is in TTL-input mode and the voltage considered high/low does not depend on Vdd
  #define PORT_ID_INLVL 1
#else
  #define PORT_ID_INLVL 0
#endif

#if defined(__AVR_DA__)
  #define ERRATA_ADC_PIN_DISABLE 1
  #define ERRATA_TCA1_PORTMUX 1
  #define ID_ADC_BUG ERRATA_ADC_PIN_DISABLE
#endif

#if defined (__AVR_DB__)
  // They changed the damned name after selling the part for 6 months!
  // annoyingly you can't even test if it's using the new version of the headers because it's an enum
  #define CLKCTRL_SELHF_CRYSTAL_gc CLKCTRL_SELHF_XTAL_gc
#endif

#if defined(__AVR_DA__) || defined(__AVR_DB__)
  #define ERRATA_TCD_PORTMUX 1
#endif

#endif
