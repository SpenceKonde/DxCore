/* core_devices  - a part of Arduino.h for megaTinyCore 2.3.0 and later and DxCore 1.4.0, but
 * which takes ion much greater importance (and girth) with the toolchain update in 2.6 and 1.6)
 * This is directly included by Arduino.h and nothing else; it just moves
 * clutter out of that file. You should not directly include this file ever.
 *
 * (C) Spence Konde 2021 - 2023. megaTinyCore and DxCore are free software (LGPL 2.1)
 * See LICENSE.txt for full legal boilerplate if you must */

#ifndef Core_Devices_h
#define Core_Devices_h
#include <avr/io.h>
#include <core_parameters.h>
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
  #define PROGMEM_MAPPED   __attribute__(( __section__(".FLMAP_SECTION3")))
  #define PROGMEM_SECTION0 __attribute__(( __section__(".FLMAP_SECTION0")))
  #define PROGMEM_SECTION1 __attribute__(( __section__(".FLMAP_SECTION1")))
  #define PROGMEM_SECTION2 __attribute__(( __section__(".FLMAP_SECTION2")))
  #define PROGMEM_SECTION3 __attribute__(( __section__(".FLMAP_SECTION3")))
#elif (__AVR_ARCH__ == 102)
  #define PROGMEM_MAPPED   __attribute__(( __section__(".FLMAP_SECTION1")))
  #define PROGMEM_SECTION0 __attribute__(( __section__(".FLMAP_SECTION0")))
  #define PROGMEM_SECTION1 __attribute__(( __section__(".FLMAP_SECTION1")))
#else
  // __AVR_ARCH__ == 103, so all of the flash is memory mapped, and the linker
  // will automatically leave const variables in flash.
  #define PROGMEM_MAPPED
#endif

/* Chip families
 *
 * 0b ffssfppp
 *
 * ff__f is a 3-bit family code 00__0 is the DA, 00__1 is DB,
 * 01__0 is DD. Dx-series grows up from bottom, Ex-series down
 * from top in order of availability of silicon. So the next two
 * are going to be 11__1 for the EA and 01__1 for the DU (if it exists)
 * ss is flash size; 0 is smallest flash in family, 1 second smallest
 * (generally 2x smallest) 2 for next size up, and 3 for an even larger
 * one.
 * ppp is code for the pincount.
 * interestingly enough this range can extend to cover all pincounts used
 * in recent times on AVR devices except the 100-pin '2560. There  is
 * only one smaller one, the 8-pin of the '85  and 'xy2 - 000
 * while the gap at 0x03 is for the 24-pin package.
 *
 * I wonder if we will see another 100-pin monster AVR? There are some
 * significant issues involved in that (they're out of addresses in
 * low IO space for VPORTs). There is precedent in the ATmega2560 for
 * the extra ports just not having atomic single cycle bit access.
 * They didn't even put the PORTx/DDRx/PINx registers into the high I/O
 * they got stuck off in the LD/ST swamp. And back on those parts, even with
 * st, writing to them was 2 clocks. People would probably expect 256k flash
 * from it though, cause they'd want to replace roloes held by 2560's, and
 * that introduces another layer of problems (1 extra clock cycle for every
 * call, rcall, reti and ret - because it's an extra thing to push onto the
 * stack and pop off the stack as part of those instructions...
 *
 * That would put them back in the position that xmega found itself in when they launched
 * the series with high hopes and large flash sizes. It's hard to imagine that Microchip hasn't studied that debacle
 *
 * I imagine Microchip isn't in a hurry to place itself in such a "zugswang"
 * situation where it is compelled to make a design decision that will
 * ultimately suck (the phrase comes from chess, a player in zugswang may
 * appear to have options, but none of them have any hope of victory)
 */


#define ID_AVR128DA     (0x20)
#define ID_AVR64DA      (0x10)
#define ID_AVR32DA      (0x00)
#define ID_AVR128DB     (0x28)
#define ID_AVR64DB      (0x18)
#define ID_AVR32DB      (0x08)
#define ID_AVR64DD      (0x60)
#define ID_AVR32DD      (0x50)
#define ID_AVR16DD      (0x40)
#define ID_AVR64DU      (0x68)
#define ID_AVR32DU      (0x58)
#define ID_AVR16DU      (0x48)
#define ID_AVR64EA      (0xF8)
#define ID_AVR32EA      (0xE8)
#define ID_AVR16EA      (0xD8)
#define ID_AVR8EA       (0xC8)
#define ID_AVR32EB      (0xF0)
#define ID_AVR16EB      (0xE0)
#define ID_AVR8EB       (0xD0)
// We could cover an 8 pin part! Or a 100-pin one. Neither seems likely - but if you told me there would be a 14-pin Dx-series the day before the DD brief was posted, I'd have laughed at the idea.
// That's tinyAVR territory. Or it was.
#define ID_14_PINS      (0x01)
#define ID_20_PINS      (0x02)
#define ID_24_PINS      (0x03)
#define ID_28_PINS      (0x04)
#define ID_32_PINS      (0x05)
#define ID_48_PINS      (0x06)
#define ID_64_PINS      (0x07)

#define ID_AVR_DA       (0x00)
#define ID_AVR_DB       (0x08)
#define ID_AVR_DD       (0x40)
/*      ID_AVR_??       (0x48) */ // This was earmarked for the DU, but with the brief having vanished many moons ago, it's fate is now in question.
/*      ID_AVR_??       (0x80) */ // The question I want to know the answer to is... "Will anyone point out what unsexy medical condition 'ED' refers to before they name a part family that?"
/*      ID_AVR_??       (0x88) */ // TBD
#define ID_AVR_EB       (0xC0) // Not yet released
#define ID_AVR_EA       (0xC8) // Not yet released

#define ID_MASK_SERIES  (0xC8)
#define ID_MASK_FLASH   (0x30)
#define ID_MASK_PINS    (0x07)
/* Thus this 1 byte part ID encodes which family (with 3 bits = 8 possibilities) pincount (again, 3 bits 8 possibilities) and flash size (up to 4 options per family).
 * So far neither Microchip nor Atmel has released any family of parts with more than 4 flash sizes, and they usually limit it to 3.
 * As long as the part isn't a tinyAVR of course. But this file is part of DxCore not megaTinyCore.
 */

//#defines to identify part families
// Note that these are legacy and deprecated
// Use the __AVR_xx__ defines to test which family it is or _AVR_FAMILY to get it as a string to print.
// Use _AVR_PINCOUNT to test how many pins. PROGMEM_SIZE gives flash size as a number of bytes, and
// _AVR_FLASH gives the flash size as a num
#if defined(__AVR_AVR128DA64__) || defined(__AVR_AVR64DA64__)
  #define DA_64_PINS
  #define HAS_64_PINS
  #define DX_64_PINS
  #define Dx_64_PINS
  #define CORE_PART_ID_LOW ID_64_PINS
  #define __AVR_DA__
#elif defined(__AVR_AVR128DA48__) || defined(__AVR_AVR64DA48__) || defined(__AVR_AVR32DA48__)
  #define DA_48_PINS
  #define HAS_48_PINS
  #define DX_48_PINS
  #define Dx_48_PINS
  #define CORE_PART_ID_LOW ID_48_PINS
  #define __AVR_DA__
#elif defined(__AVR_AVR128DA32__) || defined(__AVR_AVR64DA32__) || defined(__AVR_AVR32DA32__)
  #define DA_32_PINS
  #define HAS_32_PINS
  #define DX_32_PINS
  #define Dx_32_PINS
  #define CORE_PART_ID_LOW ID_32_PINS
  #define __AVR_DA__
#elif defined(__AVR_AVR128DA28__) || defined(__AVR_AVR64DA28__) || defined(__AVR_AVR32DA28__)
  #define DA_28_PINS
  #define HAS_28_PINS
  #define DX_28_PINS
  #define Dx_28_PINS
  #define CORE_PART_ID_LOW ID_28_PINS
  #define __AVR_DA__
#elif defined(__AVR_AVR128DB64__) || defined(__AVR_AVR64DB64__)
  #define DB_64_PINS
  #define HAS_64_PINS
  #define DX_64_PINS
  #define Dx_64_PINS
  #define CORE_PART_ID_LOW (ID_64_PINS | ID_AVR_DB)
  #define __AVR_DB__
#elif defined(__AVR_AVR128DB48__) || defined(__AVR_AVR64DB48__) || defined(__AVR_AVR32DB48__)
  #define DB_48_PINS
  #define HAS_48_PINS
  #define DX_48_PINS
  #define Dx_48_PINS
  #define CORE_PART_ID_LOW (ID_48_PINS | ID_AVR_DB)
  #define __AVR_DB__
#elif defined(__AVR_AVR128DB32__) || defined(__AVR_AVR64DB32__) || defined(__AVR_AVR32DB32__)
  #define DB_32_PINS
  #define HAS_32_PINS
  #define DX_32_PINS
  #define Dx_32_PINS
  #define CORE_PART_ID_LOW (ID_32_PINS | ID_AVR_DB)
  #define __AVR_DB__
#elif defined(__AVR_AVR128DB28__) || defined(__AVR_AVR64DB28__) || defined(__AVR_AVR32DB28__)
  #define DB_28_PINS
  #define HAS_28_PINS
  #define DX_28_PINS
  #define Dx_28_PINS
  #define CORE_PART_ID_LOW (ID_28_PINS | ID_AVR_DB)
  #define __AVR_DB__
#elif defined(__AVR_AVR64DD32__)  || defined(__AVR_AVR32DD32__) || defined(__AVR_AVR16DD32__)
  #define DD_32_PINS
  #define HAS_32_PINS
  #define DX_32_PINS
  #define Dx_32_PINS
  #define CORE_PART_ID_LOW (ID_32_PINS | ID_AVR_DD)
  #define __AVR_DD__
#elif defined(__AVR_AVR64DD28__)  || defined(__AVR_AVR32DD28__) || defined(__AVR_AVR16DD28__)
  #define DD_28_PINS
  #define DX_28_PINS
  #define Dx_28_PINS
  #define HAS_28_PINS
  #define CORE_PART_ID_LOW (ID_28_PINS | ID_AVR_DD)
  #define __AVR_DD__
#elif defined(__AVR_AVR64DD20__)  || defined(__AVR_AVR32DD20__) || defined(__AVR_AVR16DD20__)
  #define DD_20_PINS
  #define HAS_20_PINS
  #define DX_20_PINS
  #define CORE_PART_ID_LOW (ID_20_PINS | ID_AVR_DD)
  #define __AVR_DD__
#elif defined(__AVR_AVR64DD14__)  || defined(__AVR_AVR32DD14__) || defined(__AVR_AVR16DD14__)
  #define DD_14_PINS
  #define DX_14_PINS
  #define HAS_14_PINS
  #define CORE_PART_ID_LOW (ID_14_PINS | ID_AVR_DD)
  #define __AVR_DD__
#elif defined(__AVR_AVR64DU32__)  || defined(__AVR_AVR32DU32__) || defined(__AVR_AVR16DU32__)
  #define DU_32_PINS
  #define DX_32_PINS
  #define Dx_32_PINS
  #define HAS_32_PINS
  #define CORE_PART_ID_LOW (ID_32_PINS | ID_AVR_DU)
  #define __AVR_DU__
#elif defined(__AVR_AVR64DU28__)  || defined(__AVR_AVR32DU28__) || defined(__AVR_AVR16DU28__)
  #define DU_28_PINS
  #define DX_28_PINS
  #define Dx_28_PINS
  #define HAS_28_PINS
  #define CORE_PART_ID_LOW (ID_28_PINS | ID_AVR_DU)
  #define __AVR_DU__
#elif defined(__AVR_AVR32DU20__)  || defined(__AVR_AVR16DU20__)
  #define DU_20_PINS
  #define HAS_20_PINS
  #define DX_20_PINS
  #define CORE_PART_ID_LOW (ID_20_PINS | ID_AVR_DU)
  #define __AVR_DU__
#elif defined(__AVR_AVR32DU14__)  || defined(__AVR_AVR16DU14__)
  #define DU_14_PINS
  #define HAS_14_PINS
  #define DX_14_PINS
  #define CORE_PART_ID_LOW (ID_14_PINS | ID_AVR_DU)
  #define __AVR_DU__
#elif defined(__AVR_AVR8EA48__)   || defined(__AVR_AVR16EA48__) || defined(__AVR_AVR64EA48__) || defined(__AVR_AVR32EA48__)
  #define EA_48_PINS
  #define HAS_48_PINS
  #define EX_48_PINS
  #define Ex_48_PINS
  #define CORE_PART_ID_LOW (ID_48_PINS | ID_AVR_EA)
  #define __AVR_EA__
#elif defined(__AVR_AVR8EA32__)   || defined(__AVR_AVR16EA32__) || defined(__AVR_AVR64EA32__) || defined(__AVR_AVR32EA32__)
  #define EA_32_PINS
  #define HAS_32_PINS
  #define EX_32_PINS
  #define Ex_32_PINS
  #define CORE_PART_ID_LOW (ID_32_PINS | ID_AVR_EA)
  #define __AVR_EA__
#elif defined(__AVR_AVR8EA28__)   || defined(__AVR_AVR16EA28__) || defined(__AVR_AVR64EA28__) || defined(__AVR_AVR32EA28__)
  #define EA_28_PINS
  #define HAS_28_PINS
  #define EX_28_PINS
  #define Ex_28_PINS
  #define CORE_PART_ID_LOW (ID_28_PINS | ID_AVR_EA)
  #define __AVR_EA__
#else
  #error "Can't-happen: unknown chip somehow being used - and we detect every Dx and Ex chip announced!"
#endif

#if   defined(__AVR_DA__)
  #define     _AVR_FAMILY       "DA"
#elif defined(__AVR_DB__)
  #define     _AVR_FAMILY       "DB"
#elif defined(__AVR_DD__)
  #define     _AVR_FAMILY       "DD"
#elif defined(__AVR_DU__)
  #define     _AVR_FAMILY       "DU"
  #error "The AVR DU-series is not available. The product brief has been retracted and it's fate is uncertain. Where did you get toolchain support for it?"
#elif defined(__AVR_EA__)
  #define     _AVR_FAMILY       "EA"
  #error "The AVR EA-series is not available, though both the brief and headers are. Support will be added once datasheet or silicon is available."
#elif defined(__AVR_EB__)
  #define     _AVR_FAMILY       "EB"
  //
  #error "The AVR EB-series is not available, and is known only from the brief. Support will be added once datasheet or silicon is available."
#else
  #error "Unrecognized part, this should not be possible"
  #define     _AVR_FAMILY       "UNKNOWN"
#endif

#if   defined(DX_14_PINS)
     #define                  _AVR_PINCOUNT (14)
#elif defined(DX_20_PINS)
     #define                  _AVR_PINCOUNT (20)
#elif defined(DX_28_PINS)
     #define                  _AVR_PINCOUNT (28)
#elif defined(DX_32_PINS)
     #define                  _AVR_PINCOUNT (32)
#elif defined(DX_48_PINS)
     #define                  _AVR_PINCOUNT (48)
#elif defined(DX_64_PINS)
     #define                  _AVR_PINCOUNT (64)
#elif defined(EX_14_PINS)
     #define                  _AVR_PINCOUNT (14)
#elif defined(EX_20_PINS)
     #define                  _AVR_PINCOUNT (20)
#elif defined(EX_28_PINS)
     #define                  _AVR_PINCOUNT (28)
#elif defined(EX_32_PINS)
     #define                  _AVR_PINCOUNT (32)
#elif defined(EX_48_PINS)
     #define                  _AVR_PINCOUNT (48)
#endif

#if   PROGMEM_SIZE == 0x20000
  #define                     _AVR_FLASH   (128)
#elif PROGMEM_SIZE == 0x10000
  #define                     _AVR_FLASH    (64)
#elif PROGMEM_SIZE == 0x8000
  #define                     _AVR_FLASH    (32)
#elif PROGMEM_SIZE == 0x4000
  #define                     _AVR_FLASH    (16)
#elif PROGMEM_SIZE == 0x2000
  #define                     _AVR_FLASH     (8)
#elif PROGMEM_SIZE == 0x1000
  #define                     _AVR_FLASH     (4) /* Unlikely to be seen on Dx */
#endif
/* Normal interrupt mode on a part with windowed flash is the same as the unnamed mode on those without that feature */
#if (defined(__AVR_DD__))
  #define AC_INTMODE_NORMAL_BOTHEDGE_gc AC_INTMODE_BOTHEDGE_gc
  #define AC_INTMODE_NORMAL_NEGEDGE_gc AC_INTMODE_NEGEDGE_gc
  #define AC_INTMODE_NORMAL_POSEDGE_gc AC_INTMODE_POSEDGE_gc
  #define AC_INTMODE_NORMAL_t AC_INTMODE_t
#else
  #define AC_INTMODE_BOTHEDGE_gc AC_INTMODE_NORMAL_BOTHEDGE_gc
  #define AC_INTMODE_NEGEDGE_gc AC_INTMODE_NORMAL_NEGEDGE_gc
  #define AC_INTMODE_POSEDGE_gc AC_INTMODE_NORMAL_POSEDGE_gc
  #define AC_INTMODE_t AC_INTMODE_NORMAL_t
#endif

#if (defined(__AVR_EA__)) /* 4 sizes of flash instead of 3 like Dx */
  #if   (PROGMEM_SIZE == 0x10000) // 64k
    #define CORE_PART_ID (CORE_PART_ID_LOW | 0x30)
  #elif (PROGMEM_SIZE == 0x8000)  // 32k
    #define CORE_PART_ID (CORE_PART_ID_LOW | 0x20)
  #elif (PROGMEM_SIZE == 0x4000)  // 16k
    #define CORE_PART_ID (CORE_PART_ID_LOW | 0x10)
  #elif (PROGMEM_SIZE == 0x2000)  // 8k
    #define CORE_PART_ID (CORE_PART_ID_LOW | 0x00)
  #endif
#elif   (PROGMEM_SIZE == 0x20000 && (defined(__AVR_DA__) || defined(__AVR_DB__))) || (PROGMEM_SIZE == 0x10000 && (defined(__AVR_DD__) || defined(__AVR_DU__)))
  #define   CORE_PART_ID (CORE_PART_ID_LOW | 0x20) /* 128k DA/DB, 64k DD/DU */
#elif   (PROGMEM_SIZE == 0x10000 && (defined(__AVR_DA__) || defined(__AVR_DB__))) || (PROGMEM_SIZE ==  0x8000 && (defined(__AVR_DD__) || defined(__AVR_DU__)))
  #define   CORE_PART_ID (CORE_PART_ID_LOW | 0x10) /*  32k DA/DB, 16k DD/DU */
#elif   (PROGMEM_SIZE ==  0x8000 && (defined(__AVR_DA__) || defined(__AVR_DB__))) || (PROGMEM_SIZE ==  0x4000 && (defined(__AVR_DD__) || defined(__AVR_DU__)))
  #define   CORE_PART_ID (CORE_PART_ID_LOW | 0x00)
#else
  #error "Unrecognized combination of flash size and chip type"
#endif

#if   defined(AC2)
  #define _AVR_AC_COUNT      (3)
#elif defined(AC1)
  #define _AVR_AC_COUNT      (2)
#elif defined(AC0)
  #define _AVR_AC_COUNT      (1)
#else
  #define _AVR_AC_COUNT      (0)
  #error "No AC? No supported parts exist without one, something is wrong"
#endif

#if   defined(ADC1)
  #define _AVR_ADC_COUNT     (2)
#elif defined(ADC0)
  #define _AVR_ADC_COUNT     (1)
#else
  #define _AVR_ADC_COUNT     (0)
  #error "No ADC? No supported parts exist without one, something is wrong"
#endif

/* EVSYS:
 * Count up the event channels.
 * There are only 2 possibilities for tinyAVRs with the wacko channels.
 * Since they're not making more of those, test for those two cases only, otherwise, might as well check every option. */

#if defined (EVSYS_ASYNCCH3) // tinyAVR 1
  #define _AVR_EVSYS_COUNT  (6)
  #define _AVR_EVSYS_ASYNC  (4)
  #define _AVR_EVSYS_SYNC   (2)
#elif defined (EVSYS_ASYNCCH1) // tinyAVR 0
  #define _AVR_EVSYS_COUNT  (3)
  #define _AVR_EVSYS_ASYNC  (2)
  #define _AVR_EVSYS_SYNC   (1)
#elif defined(EVSYS_CHANNEL15)
  #define _AVR_EVSYS_COUNT  (16)
#elif defined(EVSYS_CHANNEL14)
  #define _AVR_EVSYS_COUNT  (15)
#elif defined(EVSYS_CHANNEL13)
  #define _AVR_EVSYS_COUNT  (14)
#elif defined(EVSYS_CHANNEL12)
  #define _AVR_EVSYS_COUNT  (13)
#elif defined(EVSYS_CHANNEL11)
  #define _AVR_EVSYS_COUNT  (12)
#elif defined(EVSYS_CHANNEL10)
  #define _AVR_EVSYS_COUNT  (11)
#elif defined(EVSYS_CHANNEL9)
  #define _AVR_EVSYS_COUNT  (10)
#elif defined(EVSYS_CHANNEL8)
  #define _AVR_EVSYS_COUNT  (9)
#elif defined(EVSYS_CHANNEL7)
  #define _AVR_EVSYS_COUNT  (8)
#elif defined(EVSYS_CHANNEL6)
  #define _AVR_EVSYS_COUNT  (7)
#elif defined(EVSYS_CHANNEL5)
  #define _AVR_EVSYS_COUNT  (6)
#elif defined(EVSYS_CHANNEL4)
  #define _AVR_EVSYS_COUNT  (5)
#elif defined(EVSYS_CHANNEL3)
  #define _AVR_EVSYS_COUNT  (4)
#elif defined(EVSYS_CHANNEL2)
  #define _AVR_EVSYS_COUNT  (3)
#elif defined(EVSYS_CHANNEL1)
  #define _AVR_EVSYS_COUNT  (2)
#elif defined(EVSYS_CHANNEL0)
  #define _AVR_EVSYS_COUNT  (1)
#else
  #error "No EVSYS detected? All supported parts have one, something is wrong"
#endif

/* We should also check what kind of evsys we have, as they are quite different from each other.
 * Provide a define indicating which revision of EVSYS this is. 1 and 2 differ only in naming of strobe register.
 * 3 separates the decision of which pin(s) within a port will be used aas event input and which of those to use
 * with the former being configured with PORTx.EVGENCTRL. This allows the number of generators to drop from 8/port to 2/port, and the number of RTC generators to likewise drop to 2 from 16 with 8 available per channel
 * In exchange for this, we achieve our longtime dream: Equality between all generator channels, because the redused number of
 * generators allows them to add both options for all ports and both RTC options to all generator channels
 * Too bad they released so many parts with the other versions :-/ */

#if defined(PORTA_EVGENCTRL) // Ex-series, with EVGENCTRL registers on RTC and PORT.
  #define _AVR_EVSYS_VERSION   (3)
#elif defined(EVSYS_STROBE) // mega0 - basically Dx, but different name for strobe.
  #define _AVR_EVSYS_VERSION   (1)
#elif !defined(EVSYS_ASYNCCH0) // AVR Dx, 2-series
  #define _AVR_EVSYS_VERSION   (2)
#else // tinyAVR 0/1-series with the two kinds of channels and boneheaded generator layout.
  #define _AVR_EVSYS_VERSION   (0)
#endif


#if defined(OPAMP)
  /* OPAMPS:
   * Allow for future chip with more opamps. There's room for 6 in the struct
   * which has 64 bytes - 8 per OPAMP, and 16 at the start used for global settings
   * At time of writing, on the only parts with OPAMPs, only 4 of the global bytes are used
   * 6 of the 8 bytes for each OPAMP are used, 2 are spares. */
  #if defined(OPAMP_OP5CTRLA)
    #define _AVR_OPAMP_COUNT   (6)
  #elif defined(OPAMP_OP4CTRLA)
    #define _AVR_OPAMP_COUNT   (5)
  #elif defined(OPAMP_OP3CTRLA)
    #define _AVR_OPAMP_COUNT   (4)
  #elif defined(OPAMP_OP2CTRLA)
    #define _AVR_OPAMP_COUNT   (3)
  #elif defined(OPAMP_OP1CTRLA)
    #define _AVR_OPAMP_COUNT   (2)
  #elif defined(OPAMP_OP0CTRLA)
    #define _AVR_OPAMP_COUNT   (1)
  #else
    #error "We have an OPAMP peripheral, but no opamps? Something is wrong"
  #endif
#else
  #define _AVR_OPAMP_COUNT   (0)
#endif

#if defined(CCL)
  #if   defined(CCL_TRUTH6)
    #define _AVR_LUT_COUNT     (8)
  #elif defined(CCL_TRUTH4)
    #define _AVR_LUT_COUNT     (6)
  #elif defined(CCL_TRUTH2)
    #define _AVR_LUT_COUNT     (4)
  #elif defined(CCL_TRUTH0)
    #define _AVR_LUT_COUNT     (2)
  #else
    #error "We have a CCL peripheral, but no truth tables? Something is wrong"
  #endif
#else
  #error "No CCL? No supported parts exist without one, something is wrong"
#endif

#if   defined(TCA1)
  #define _AVR_TCA_COUNT     (2)
#elif defined(TCA0)
  #define _AVR_TCA_COUNT     (1)
#else
  #define _AVR_TCA_COUNT     (0) // I fear something terrible happened to the TCA on the EB-series... and I think the TCE and that WEX Luther guy he's always with know something about it.
#endif

#if   defined(TCB7)
  #define _AVR_TCB_COUNT     (8)
#elif defined(TCB6)
  #define _AVR_TCB_COUNT     (7)
#elif defined(TCB5)
  #define _AVR_TCB_COUNT     (6)
#elif defined(TCB4)
  #define _AVR_TCB_COUNT     (5)
#elif defined(TCB3)
  #define _AVR_TCB_COUNT     (4)
#elif defined(TCB2)
  #define _AVR_TCB_COUNT     (3)
#elif defined(TCB1)
  #define _AVR_TCB_COUNT     (2)
#elif defined(TCB0)
  #define _AVR_TCB_COUNT     (1)
#else
  #error "No TCBs? No supported parts exist without one, something is wrong"
#endif


#if   defined(TCD0)
  #define _AVR_TCD_COUNT     (1) // Only on Dx-series and tinyAVR
#else
  #define _AVR_TCD_COUNT     (0)
#endif

#if   defined(TCE0)
  #define _AVR_TCE_COUNT     (1) // first appears on the EB-series, 16-bit. Some sort of 8-channeled monster who is always with the one they call WEX. I haven't heard from TCA0 after they showed up and started doing
#else                            // PWM on the same pins. I have a bad feeling that TCA0 is either tied up in the basement, or dead in a wooded area. With the TCE's skill at motor control, they could easily have
  #define _AVR_TCE_COUNT     (0) // used power-tools to dismember bury the body.... Anyway, whether these guys are as useful in the silicon as they look  on paper will depend a lot on the whether those
#endif                           // 8-channels are independent, and whether they need to split like TCA did to handle 8 WO's if so. And, of course on how flexible their clocking options are.

#if   defined(TCF0)
  #define _AVR_TCF_COUNT     (1) // Even more enigmatic than the TCE. First appears on the EB-series, this previously unseen timer is said to be 24-bit! Curious how that will work and what clock sources it can use.
#else                            // a 24-bit timer clocked from the CPU core, at only 20 MHz would need it's period choked way back, sacrificing all that resolution, in order to get PWM rather than a blinking light.
  #define _AVR_TCF_COUNT     (0) // 2^24 is in the neighborhood of 17 million, so if CLK_PER was it's max, a lot of these frequencies it could generate would be a touch on the slow side. Even if we can get them up to
#endif                           // 32 MHz like tiny-2's, we'd need to use only 1-2 bits of that last byte to avoid flicker if you wanted to use for PWM, which Arduino people will.

#if   defined(TWI1)
  #define _AVR_TWI_COUNT     (2)
#elif defined(TWI0)
  #define _AVR_TWI_COUNT     (1)
#else
  #error "No TWI? No supported parts exist without one, something is wrong"
#endif

#if   defined(SPI1)
  #define _AVR_SPI_COUNT     (2)
#elif defined(SPI0)
  #define _AVR_SPI_COUNT     (1)
#else
  #error "No SPI? No supported parts exist without one, something is wrong"
#endif

#if   defined(USART7)
  #define _AVR_USART_COUNT     (8)
#elif defined(USART6)
  #define _AVR_USART_COUNT     (7)
#elif defined(USART5)
  #define _AVR_USART_COUNT     (6)
#elif defined(USART4)
  #define _AVR_USART_COUNT     (5)
#elif defined(USART3)
  #define _AVR_USART_COUNT     (4)
#elif defined(USART2)
  #define _AVR_USART_COUNT     (3)
#elif defined(USART1)
  #define _AVR_USART_COUNT     (2)
#elif defined(USART0)
  #define _AVR_USART_COUNT     (1)
#else
  #error "No USARTs? No supported parts exist without one, something is wrong"
#endif


#if   defined(ZCD3)
  #if !defined(ZCD0)
    #define _AVR_ZCD_COUNT   (1) /* DD-series */
  #else
    #define _AVR_ZCD_COUNT   (4)
  #endif
#elif defined(ZCD2)
  #define _AVR_ZCD_COUNT     (3)
#elif defined(ZCD1)
  #define _AVR_ZCD_COUNT     (2)
#elif defined(ZCD0)
  #define _AVR_ZCD_COUNT     (1)
#else
  #define _AVR_ZCD_COUNT     (0)
#endif

#if   defined(DAC2)
  #define _AVR_DAC_COUNT     (3)
#elif defined(DAC1)
  #define _AVR_DAC_COUNT     (2)
#elif defined(DAC0)
  #define _AVR_DAC_COUNT     (1) /* Note that thus far, no DAC other than DAC0 has ever been able to output data. DAC1 and DAC2 are just the DACREFs for AC1 and AC2 on tinyAVR 1-series parts.*/
#else
  #define _AVR_DAC_COUNT     (0)
#endif

#ifdef OPAMP
  #if defined(OPAMP_OP0CTRLA)
    #define PIN_OPAMP0_INP            PIN_PD1 /* Starts at PD1 since DB's with 28 or 32 pins have no PD0. */
    #define PIN_OPAMP0_OUT            PIN_PD2
    #define PIN_OPAMP0_INN            PIN_PD3
  #endif
  #if defined(OPAMP_OP1CTRLA)
    #define PIN_OPAMP1_INP            PIN_PD4
    #define PIN_OPAMP1_OUT            PIN_PD5
    #define PIN_OPAMP1_INN            PIN_PD7 /* Skips PD6 because that's the DAC output */
  #endif
  #if defined(OPAMP_OP2CTRLA)
    #define PIN_OPAMP2_INP            PIN_PE1 /* Likely skips PE0 for consistency with OPAMP0 */
    #define PIN_OPAMP2_OUT            PIN_PE2
    #define PIN_OPAMP2_INN            PIN_PE3
  #endif
#endif


#ifdef DAC0
  #if   defined(DAC_OUTRANGE_gm) // Ex-series - 10-bit, and OUTRANGE, the strange option for selection the range of DATA values that can be output.
    #define _AVR_DAC_VERSION            (2)
  #elif defined(DAC0_DATAH) // Dx-series - 10-bit
    #define _AVR_DAC_VERSION            (1)
  #else // tinyAVR 1 - 8-bit
    #define _AVR_DAC_VERSION            (0)
  #endif
  #ifndef PIN_DACOUT
    #if _AVR_DAC_VERSION == 0
      #define PIN_DACOUT PIN_PA6 // different on tinyAVR!
    #else
      #define PIN_DACOUT PIN_PD6
    #endif
  #endif
#endif

#if defined(PORT_INLVL_bm) // Check if we have inlvl option
  #define _AVR_HAS_INLVL                 (1)
#else
  #define _AVR_HAS_INLVL                 (0)
#endif

/* Hardware incapabilities: Errata */
// 0 = = this erratum does not apply to a part
// 1 = this erratum applies to all of these parts
// any other value: This erratum applies to parts before this silicon REVID.

#define ERRATA_IRREL                 (-128)
#define ERRATA_APPLIES                  (1)
#define ERRATA_DOES_NOT_APPLY           (0)

#define checkErrata(errata)  (errata == ERRATA_DOES_NOT_APPLY ? ERRATA_DOES_NOT_APPLY : (errata==ERRATA_IRREL ? ERRATA_DOES_NOT_APPLY :  (errata > SYSCFG_REVID ? ERRATA_DOES_APPLY : ERRATA_DOES_NOT_APPLY)))


/* When they're fixed, we'll replace these with a macro to check REVID and return 1 or 0 appropriately.    */
/* aaahahahah! Sorry...
... I meant, *if* they're ever fixed.

Highly unlikely events come in groups, so if they get fixed, maybe put on a jacket or something as a precaution....
just in case a pig coming in for a landing collides with you on your way to claim your lottery jackpot, killing you...
And if in that unlikely event, contrary to your expectations, you end up in hell, yet find, instead of fire and brimstone there's just ice as far as you
can see, the jacket will make etenal damnation to a frozen-over hell a bit less miserable.

That's how pessimistic I am left feeling about the prospects for errata fixes by this point*/

/* Now, what errata are we talking about here? */
// The 128DA had a few unique and nasty ones.
#if defined(__AVR_DA__) && (_AVR_FLASH == 128) /* Only the Rev. A8 has shipped. We are all wondering where the die rev is....                              */
  #define ERRATA_TCA1_PORTMUX            (1) /* DA128's up to Rev. A8 have only the first two pinmapping options working                                   */
  #define ERRATA_PORTS_B_E_EVSYS         (1) /* DA128's up to Rev. A8 have no EVSYS on PB6, PB7, and PE4~7                                                 */
  #define ERRATA_NVM_ST_BUG              (1) /* DA128's up to Rev. A8 apply bootloader/app protection neglecting FLMAP bits when writing with ST. Use SPM. */
  #define ERRATA_2V1_EXCESS_IDD          (0) /* This nasty one didn't show up until the DB's arrived                                                       */
#else
  #define ERRATA_TCA1_PORTMUX            (0) /* TCA1 portmux works and always has on DB                                                                    */
  #define ERRATA_PORTS_B_E_EVSYS         (0) /* Works everywhere else                                                                                      */
  #define ERRATA_NVM_ST_BUG              (0) /* only present on DA128!                                                                                     */
  #define ERRATA_2V1_EXCESS_IDD          (0) /* But he pretty well blows up most hope of low power/low voltage DBs                                         */
#endif
// A few were present on all the DA's, but fixed for the DBs.
#if defined(__AVR_DA__)
  #define ERRATA_CCL_LINK                (1)
  #define ERRATA_PLL_RUNSTBY             (1)
  #define ERRATA_ADC_PIN_DISABLE         (1)
  #define ERRATA_PLL_XTAL     (ERRATA_IRREL)
#else
  #define ERRATA_CCL_LINK                (0)
  #define ERRATA_PLL_RUNSTBY             (0)
  #define ERRATA_ADC_PIN_DISABLE         (0)
#endif

//The new crystal clock support brought a bug along though.
#if defined(__AVR_DB__)
  #define ERRATA_PLL_XTAL                (1) /* DB BUG */
#endif

// And both DA and DB had a whole slew of issues
#if defined(__AVR_DA__) || defined(__AVR_DB__)
  #define ERRATA_TCD_PORTMUX             (1) // *thud* *thud* *thud* - the sound of an embedded developer banging his head on the desk leaving a dent.
  #define ERRATA_DAC_DRIFT               (1) // How much drift? I dunno - enough for Microchip to feel a need to add an erratum about it, but too much for them to be comfortable sharing any numbers.
  #define ERRATA_TCA_RESTART             (1) // Is resets the direction, like the database said. Appaently both the documentation and the silicon were wrong.
  #define ERRATA_CCL_PROTECTION          (1) // Busted on all pre-DD parts
  #define ERRATA_TCD_ASYNC_COUNTPSC      (1) // Busted on all pre-DD parts
  #define ERRATA_TCB_CCMP                (1) // Busted on all pre-DD parts
  #define ERRATA_TWI_PINS                (1) // Busted on all pre-DD parts
  #define ERRATA_USART_ONEWIRE_PINS      (1) // Busted on all pre-DD parts.
  #define ERRATA_TWI_FLUSH               (1) // Unclear whether actually present on older parts, or if it was added when dual mode went in.
  #define ERRATA_USART_ISFIF             (1) // All devices seem to have this, though they only discovered it quite recently.
  #define ERRATA_USART_WAKE              (1) // Present almost everywhere... except possibly very early modern AVRs. It may have been added when fixing one of the other bugs.
#else
  // but most were fixed in the DD
  #define ERRATA_PLL_XTAL                (0) // Okay, that's cool I guess, but really small audience.
  #define ERRATA_TCD_PORTMUX             (0) // *dance dance dance*
  #define ERRATA_DAC_DRIFT               (0) // Fixed?
  #define ERRATA_ADC_PIN_DISABLE         (0) // One less annoying thing.
  #define ERRATA_TCA_RESTART             (0) // Direction NOT reset
  #define ERRATA_CCL_PROTECTION          (0) // *dance dance dance*
  #define ERRATA_TCD_ASYNC_COUNTPSC      (0) // Cool, uh, I guess. Probably matters if you are driving really serious business motors.
  #define ERRATA_TWI_PINS                (0) // What? Really? Okay... I thought this one was just gonna sit there.
  #define ERRATA_USART_ONEWIRE_PINS      (0) // I guess they were looking at port-override stuff.
  #define ERRATA_USART_ISFIF             (1) // This one looks like it wasn't found soon enough for the DD's.
  #define ERRATA_USART_WAKE              (0) // Okay I guess this is cool, since it's fixed we dgaf, but always good to see things getting fixed.
  #define ERRATA_TWI_FLUSH               (0) // I still don't know if this matters a lot, a little or not at all.
  #define ERRATA_2V1_EXCESS_IDD          (0) // Thankfully he seems to be gone now
#endif

#if defined(__AVR_DA__) || defined(__AVR_DB__) || defined(__AVR_DD__)
  #define ERRATA_FLASH_MULTIPAGE         (1)
  // If write protection is enabled on the APPDATA section, but
  // it is not aligned on a 16k boundary, 32-page erase targeting the APPCODE section before it reached APPDATA, a chunk of APPCODE can be erased.
  // Takes some dedicated contriving to come up with a scenario to make this relevant without positing an individual just trying to score points by
  // demonstrating bugs
  #define ERRATA_TCD_HALTANDRESTART      (1) // Since I've never seen a good description of what the envisioned use cases are for TCD's event modes, I don't even know if I should care!
#else
  #warning "Unrecognized part - even if this compiles, something is mondo wrong with your IDE or system. Behavior w/unknown part is undefined, and may result in demons flying out of your nose"
#endif

/***********************************************************************************************************************************************\
 * CORE AND HARDWARE FEATURE SUPPORT                                                                                                             *
 * CORE_HAS_FASTIO is 1 when digitalReadFast and digitalWriteFast are supplied, and 2 when openDrainFast and pinModeFast are as well.            *
 * CORE_HAS_OPENDRAIN                                                                                                                            *
 * CORE_HAS_PINCONFIG       is 1 if pinConfigure() is supplied. The allows full configuration of any pin.                                        *
 *                          is 2 if pinConfigure(pin,option1,option2,...option) sort of syntax is also valid.                                    *
 * CORE_HAS_FASTPINMODE     is 1 if pinModeFast is supplied                                                                                      *
 * CORE_HAS_ANALOG_ENH      is 0 if no analogReadEnh is supplied, 1 if it is, and 2 if it is supplied and both core and hardware support a PGA.  *
 * CORE_HAS_ANALOG_DIFF     is 0 if no analogReadDiff is supplied, 1 if it's DX-like (Vin < VREF), and 2 if it's a proper                        *
 * differential ADC, supported in both hardware and software. The value -1 is also valid and indicates it's a classic AVR with a  * differential *
 *                                  ADC, see the documentation for the core.                                                                     *
 * CORE_HAS_TIMER_TAKEOVER  is 1 if takeOverTCxn functions are provided to tell the core not to automatically use the type A and D timers.       *
 * CORE_HAS_TIMER_RESUME    is 1 if resumeTCAn functions are provided to hand control back to the core and reinitialize them.                    *
 * CORE_DETECTS_TCD_PORTMUX is 1 if the TCD portmux works correctly on the hardware and is used by the core                                      *
 *                             0 if doesn't, and the core thus does not attempt to use it.                                                       *
 * DEVICE_PORTMUX_TCA       is 1 if the TCA portmux is tinyAVR like, with 1 bit per channel, 2 if the device is anti-tinyAVR-like with portmux   *
 *                                  for TCA assigning pins in groups.                                                                            *
 * DEVICE_PORTMUX_TCD       is 0 or undefined if there is no portmux register for TCD (like a tinyAVR).                                          *
 *                             1 for a DA/DB-series mux (4 options 1 of which works),                                                            *
 *                             2 for a DD-series mux (5 option, all of which work except 1 and 3 which aren't expected to work because the pins  *
 *                                  don't exist)                                                                                                 *
 * DEVICE_PORTMUX_UART      is 0 if there is tinyAVR-like single bit muxing for USART pins.                                                      *
 *                             1 for 2-bit wide bitfiesd for each USART, 0-3 in USARTROUTEA and 4-7 in USARTROUTEB                               *
 *                             2 for 3-bit wide bitfield for USART0, and at least 2 bits for USART1 (like a DD-series). It is not known yet what *
 *                                  will be done about further USARTs                                                                            *
 *        I can imagine a future with only 0 ever getting 3-bit bit wide muxes, (hence TCAROUTEA = 0, 1, 2, TCAOUTEB = 3, 4, 5, 6)               *
 *        I can imagine a future with 2 USART on PORTMUXA (0, 1, 2) having three bits, and the other 2. This could be followed by 3, 4. 5, and 7 on *
 *                                  USARTROUTEB, or a permanent shift to 3:2:3 or 3:3:2                                                          *
 *        The strangest possility is 3:2:2:1 with the first 4 USARTs still on USARTROUTEA, PORT3 losing it's "none" option, and retaining the 4  *
 *                                  unmolested bits for either USART6 and USART7 If we see a USART3 on USARTROUTEA or USART4 in Bit bosition 0   *
 *                                  of USARTROUTEB, that is a strong indicantion of a part in the works with >64 pins, and 7 serial ports.       *
 *                                  with no other USART having more than 3 pin mapping. Other options would give more hope of bigger parts.      *                                                          *
 *                                                                                                                                               *
 * CORE_SUPPORT_LONG_TONES is 1 if the core supports the three argument tone for unreasonably long tones.                                        *
 ************************************************************************************************************************************************/
#define CORE_HAS_FASTIO                 (2)
#define CORE_HAS_OPENDRAIN              (1) /* DxCore has openDrain() and openDrainFast()                           */
#define CORE_HAS_PINCONFIG              (3) /* pinConfigure is now implemented                                      */
#define CORE_HAS_FASTPINMODE            (1)
#if defined(__AVR_DD__)                     /* On the few parts where it works...*/
  #define CORE_DETECTS_TCD_PORTMUX      (1) /* we support using it */
#else
  #define CORE_DETECTS_TCD_PORTMUX      (0) /* For the DA/DB parts where portmux is busted? No, we don;t use the same logic there */
#endif
#define CORE_HAS_TIMER_TAKEOVER         (1)
#define CORE_HAS_TIMER_RESUME           (1)
#define CORE_SUPPORT_LONG_TONES         (1)
#define CORE_HAS_ANALOG_ENH             (1)
#define CORE_HAS_ANALOG_DIFF            (1)
#if defined(TCD0)
  #if defined(__AVR_DD__)
    #define DEVICE_PORTMUX_TCD          (2)
  #else
    #define DEVICE_PORTMUX_TCD          (1)
  #endif
#endif
#if defined(__AVR_DD__)
  #define DEVICE_PORTMUX_USART          (2)
#else
  #define DEVICE_PORTMUX_USART          (1)
#endif
#define DEVICE_PORTMUX_TCA              (2) /* 1 = each wave output cannnel can be moved individually, like tinyAVRs
                                               2 = all wave output channels move together
                                               3 - as above, but TCA1 has the 4th and 5th channel options*/
#define CORE_DETECTS_TCA_PORTMUX        (2) /* If this is 1, core is recognizes the current portmux setting, and analogWrite works wherever it's pointed
//                                           * If this is 2, as above, but on TCA1, the three pin options are also recognized
//                                           * Note that there are only two three-pin mappings for TCA is mux = 2, 4 if mux = 3
                                             */


#if defined(__AVR_EA__)
  #define EVSYS_VERSION_TWO                 /* EA series has markedly different event system that is expected to  */
                                            /* replace the current one. It brings channel uniformity at the cost  */
                                            /* being limitd to two event inputs per port and two RTC PIT derived  */
                                            /* inputs                                                             */
#endif
/* Hardware capabilities (ADC)
 * ADC_DIFFERENTIAL is 1 for a half-way differential ADC like DX-serie has, 2 for a real one like EA-series will    *
 * ADC_MAX_OVERSAMPLED_RESOLUTION is the maximum resolution attainable by oversampling and decimation               *
 * ADC_NATIVE_RESOLUTION is the higher of the two native ADC resolutions. Either 10 or 12                           *
 * ADC_NATIVE_RESOLUTION_LOW is the lower of the two native ADC resolutions. Either 8 or 10. Can't be deduced from  *
 * above. All permutations where the "native resolution" is higher are extant somewhere                             *
 * ADC_MAXIMUM_ACCUMULATE is the maximum number of sameples that can be accumulated by the burst accumulation mode  *
 * ADC_MAXIMUM_SAMPDUR is the maximum sample duration value. Refer to the core documentation or datasheet for detail*
 * ADC_RESULT_SIZE is the size (in bits) of the registers that hold the ADC result. V2.0 ADC has 32, others have 16 *
 * ADC_MAXIMUM_GAIN is defined if there is a way to amplify the input to the ADC. If you have to use onchip opamps  *
 * and the chip has them, it's -1, otherwise it is the maximum multiplier
 * ADC_REFERENCE_SET is 1 if the references are the weird ones that tinyAVR 0 and 1 use, and 2 if they are 1.024,   *
 * 2.048, 4.096 and 2.5V like civilized parts */

#define ADC_DIFFERENTIAL                (1)
#define ADC_MAX_OVERSAMPLED_RESOLUTION (15)
#define ADC_NATIVE_RESOLUTION          (12)
#define ADC_NATIVE_RESOLUTION_LOW      (10)
#define ADC_MAXIMUM_ACCUMULATE        (128)
#define ADC_MAXIMUM_SAMPDUR          (0xFF)
#define ADC_RESULT_SIZE                (16)
#if defined(__AVR_DD__) || defined(__AVR_EA__)
  #define ADC_MAXIMUM_PIN_CHANNEL      (31)
  #define ADC_MAXIMUM_NEGATIVE_PIN     (31)
#else                                       /* negative inputs! The EA even has a decent differential ADC!          */
  #define ADC_MAXIMUM_PIN_CHANNEL      (21)
  #define ADC_MAXIMUM_NEGATIVE_PIN     (15)
#endif
#if defined(ADC0_PGACTRL)                   /* The product briefs do not mention either way                         */
  #define ADC_MAXIMUM_GAIN             (16)
#elif defined(OPAMP0)
  #ifndef ADC_MAXIMUM_GAIN
    #define ADC_MAXIMUM_GAIN           (-1)  /* DB-series can use their OPAMPs as a PGA                             */
  #endif
#endif

/* _switchInternalToF_CPU()
 *
 * This macro is for when you want to set the internal to whatever F_CPU is, after it has eitehr been scribbled over,
 * not set at startup because an external clock source was selected - but that clock source is broken. If your application is such
 * that this culd happen and you wanted it to fall back to using the internal oscillator at the same speed, this is how you
 * could easily do that (see Ref_Callbacks.md)
 * The default behavior is to hang and proceed no further while issuing a blink code,
 * but you might instead want to ignore that failure, and instead use the less
 * accurate (but still pretty damned good) internal one. See the DxCore clock source reference
 * Setting it to any other speed is not recommended all the timing elsewhere will be totally busted.
 */

#define  _setPrescale2x()         (_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, ( CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm)))
#define  _setPrescale4x()         (_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, ( CLKCTRL_PDIV_4X_gc | CLKCTRL_PEN_bm)))
#define  _setPrescale8x()         (_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, ( CLKCTRL_PDIV_8X_gc | CLKCTRL_PEN_bm)))
#define _setPrescale16x()         (_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PDIV_16X_gc | CLKCTRL_PEN_bm)))
#define _setPrescale32x()         (_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PDIV_32X_gc | CLKCTRL_PEN_bm)))
#define _setPrescale64x()         (_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PDIV_64X_gc | CLKCTRL_PEN_bm)))
#define  _setPrescale6x()         (_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, ( CLKCTRL_PDIV_6X_gc | CLKCTRL_PEN_bm)))
#define _setPrescale10x()         (_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PDIV_10X_gc | CLKCTRL_PEN_bm)))
#define _setPrescale12x()         (_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PDIV_12X_gc | CLKCTRL_PEN_bm)))
#define _setPrescale24x()         (_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PDIV_24X_gc | CLKCTRL_PEN_bm)))
#define _setPrescale48x()         (_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB, (CLKCTRL_PDIV_48X_gc | CLKCTRL_PEN_bm)))


#if (F_CPU == 32000000)
  #define _switchInternalToF_CPU()                     _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x0B << 2))
#elif (F_CPU == 28000000)
  #define _switchInternalToF_CPU()                     _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x0A << 2))
#elif (F_CPU == 24000000)
  #define _switchInternalToF_CPU()                     _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x09 << 2))
#elif (F_CPU == 20000000)
  #define _switchInternalToF_CPU()                     _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x08 << 2))
#elif (F_CPU == 16000000)
  #define _switchInternalToF_CPU()                     _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x07 << 2))
#elif (F_CPU == 12000000)
  #define _switchInternalToF_CPU()                     _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x06 << 2))
#elif (F_CPU == 8000000)
  #define _switchInternalToF_CPU()                     _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x05 << 2))
// this setting is mysteriously not used, and it is unknown how the chip will behave if 0x04 is written to this bitfield
//#define _switchInternalToF_CPU()                     _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x04 << 2))
#elif (F_CPU == 4000000)
  #define _switchInternalToF_CPU()                     _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x03 << 2))
#elif (F_CPU == 3000000)
  #define _switchInternalToF_CPU()                     _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x02 << 2))
#elif (F_CPU == 2000000)
  #define _switchInternalToF_CPU()                     _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x01 << 2))
#elif (F_CPU == 1000000)
  #define _switchInternalToF_CPU()                     _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x00 << 2))
#elif (F_CPU == 10000000)
  #define _switchInternalToF_CPU() { _setPrescale2x(); _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x08 << 2));}
#elif (F_CPU == 5000000)
  #define _switchInternalToF_CPU() { _setPrescale4x(); _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x08 << 2));}
#else
  #define _switchInternalToF_CPU() badCall("The _switchInternalToF_CPU() macro can only set the internal oscillator to a value which is supported by the core")
#endif

#if defined(TCD0)
  //_gc's are enumerated types, the preprocessor doesn't understand them - but we need to do conditional compilation based on them
  // and this is way down here so that we can take into account errata.
  #define PORTMUX_TCD0_PORTA  (0x00) // PORTMUX_TCD0_DEFAULT_gc
  #if (defined(ERRATA_TCD_PORTMUX) && ERRATA_TCD_PORTMUX == 0)
    #if defined(PORTB)
      #define PORTMUX_TCD0_PORTB  (0x01) // PORTMUX_TCD0_ALT1_gc
    #endif
    #define   PORTMUX_TCD0_PORTF  (0x02) // PORTMUX_TCD0_ALT2_gc
    #if defined(PORTG)
      #define PORTMUX_TCD0_PORTG  (0x03) // PORTMUX_TCD0_ALT3_gc
    #endif
    #if defined(__AVR_DD__)
      #define PORTMUX_TCD0_PORTAD (0x04) // PORTMUX_TCD0_ALT4_gc
    #endif
  #endif
#endif



/* Microchip has shown a tendency to rename registers bitfields and similar between product lines, even when the behavior is identical.
 * This is a major hindrance to writing highly portable code which I assume is what most people wish to do. It certainly beats having
 * to run code through find replace making trivial changes, forcing a fork where you would rather not have one.
 * Since having to adjust code to match the arbitrary and capricious whims of the header generation scheme kinda sucks, we try to catch
 * all the places they do this and provide a macro for backwards compatibility. For some bizarre reason you may wish to turn this off
 * maybe in preparation for jumping to another development environment like Microchip Studio that does not use Arduino cores.
 * Instead of backwards compatibilily, you want the opposite, which some wags have called "Backwards combatibility"
 * Defining BACKWARD_COMBATIBILITY_MODE turns off all of these definitions that paper over name changes.
 */

// In backwards "combatability" mode, none of these helper definitions are provided
// in order to force you to use the canonical names for these registers and options.
// Otherwise, we try our best paper over the stupid bitfield and groupcode name changes.

// #define BACKWARD_COMBATIBILITY_MODE

#if !defined(BACKWARD_COMBATIBILITY_MODE)
  // We default to seeking compatibility. for COMBATability you would uncomment that #define, and that turns all these off.

  #if defined(RTC_CLKSEL)
    /* Man they just *HAD* to change the names of these values that get assigned to the same register and do the same thing didn't they?
     * Worse still we can't even verify that they are present... just blindly define and pray. Enums can't be seen by macros
     */
     // tinyAVR has TOSC32K (tinyOscillator?)
    #if defined(MEGATINYCORE)
      #define RTC_CLKSEL_OSC32K_gc            RTC_CLKSEL_INT32K_gc
      #define RTC_CLKSEL_OSC1K_gc             RTC_CLKSEL_INT1K_gc
      #define RTC_CLKSEL_XTAL32K_gc           RTC_CLKSEL_TOSC32K_gc
      #define RTC_CLKSEL_XOSC32K_gc           RTC_CLKSEL_TOSC32K_gc
    #else
  // Dx has an XOSC32K
      #define RTC_CLKSEL_INT32K_gc            RTC_CLKSEL_OSC32K_gc
      #define RTC_CLKSEL_INT1K_gc             RTC_CLKSEL_OSC1K_gc
      #define RTC_CLKSEL_TOSC32K_gc           RTC_CLKSEL_XOSC32K_gc
      #define RTC_CLKSEL_XTAL32K_gc           RTC_CLKSEL_XOSC32K_gc
    #endif
  #endif
  /* General Purpose Register names, GPR.GPRn, vs GPIORn vs GPIOn
   * They now appear to have decided they don't like either of the previous conventions, one just a few years old. Now they are grouping
   * them under a "General Purpose Register". "peripheral". I cannot argue that GPR doesn't make more sense, as there's not really any
   * I/O occurring here (ofc they were referring to the IN and OUT instructions, which can be used on these), but I certainly wouldn't
   * have changed a convention like this, at least not when I had just done so a few years prior. */

  // All non-xmega pre-Dx-series modern AVR parts call them GPIORn instead of GPR.GPRn/GPR_GPRn
  // Some classics called them GPIOn.

  /* In one xMega AVR, they were GPIOn, rather than GPIORn
   * One? Yup: The ATxmega32d4. Not the 32d3, nor the 32e5, nor anything else. All the xmega's have GPIORs
   * and their headers list the GPIOn names too. But.... there is only a single header
   * file with them not marked as "Deprecated". And that's the ATxmega32d4
   * 24 of the 46 xmega parts with headers in the compiler packages (including the 32d3 and 32e5) had the
   * 4 GPIOR's that we have, and had GPIOn and GPIO_GPIOn present but marked as deprecated.
   * On those parts, these are at addresses 0x0000-0x003, and 0x0004-0x000F do not appear to be used.
   * The other 22.... had THE ENTIRE FIRST HALF OF THE LOW I/O SPACE as GPIOR0-GPIORF!
   * Which ones got all of them and which ones only got 4 seems to have been chosen in typical
   * Atmel fashion (hint: it involves flipping a coin). No apparent pattern in time or other parameters.
   * Either way, that left them with space for only 4 VPORT register sets (like the ones we got)
   * These had to be configured to point to the desired port.
   * I'm sure everyone is grateful for the fact that the folks designing the Dx-series have their
   * heads screwed on properly and realized that 4 GPIOR-- excuse me, GPRs, 4 awkward VPORTs and
   * 12 unused addresses in the low I/O space was maybe not the best design decision made in the
   * xmega line, particularly in light of the profound usefulness of the VPORT for interacting with
   * hardware in a performant and flash-efficient manner. This is clearly not why xMega failed
   * in the market resulting in Microchip pretending they don't exist and why they were
   * scavenged for parts during modern AVR development; there were more fundamental issues.
   * But it didn't help, and this sort of halfassery with the treatment of the registers
   * changing thrice during the xMega's run didn't help either.
   */
  #if !defined(GPIOR0)
    #define GPIOR0                            (_SFR_MEM8(0x001C))
    #define GPIOR1                            (_SFR_MEM8(0x001D))
    #define GPIOR2                            (_SFR_MEM8(0x001E))
    #define GPIOR3                            (_SFR_MEM8(0x001F))
  #endif
  #if !defined(GPIO0)
    #define GPIO0                             (_SFR_MEM8(0x001C))
    #define GPIO1                             (_SFR_MEM8(0x001D))
    #define GPIO2                             (_SFR_MEM8(0x001E))
    #define GPIO3                             (_SFR_MEM8(0x001F))
  #endif
  #if !defined(GPIO_GPIOR0)
    #define GPIO_GPIO0                        (_SFR_MEM8(0x001C))
    #define GPIO_GPIO1                        (_SFR_MEM8(0x001D))
    #define GPIO_GPIO2                        (_SFR_MEM8(0x001E))
    #define GPIO_GPIO3                        (_SFR_MEM8(0x001F))
  #endif
  #if !defined(GPR_GPR0)
    #define GPR_GPR0                          (_SFR_MEM8(0x001C))
    #define GPR_GPR1                          (_SFR_MEM8(0x001D))
    #define GPR_GPR2                          (_SFR_MEM8(0x001E))
    #define GPR_GPR3                          (_SFR_MEM8(0x001F))
  #endif
 // Most people who know what they are call them GPIORs


  #if defined (CLKCTRL_SELHF_bm)
    /* They changed the damned name after selling the part for 6 months!
     * annoyingly you can't even test if it's using the new version of the headers because it's an enum! */
    #define CLKCTRL_SELHF_CRYSTAL_gc CLKCTRL_SELHF_XTAL_gc
  #endif
  /* And one version later they did it again... */
  #if !defined(CLKCTRL_FREQSEL_gm) && defined(CLKCTRL_FRQSEL_gm)
    #define CLKCTRL_FREQSEL_gm     (CLKCTRL_FRQSEL_gm)      /*    Group Mask                  */
    #define CLKCTRL_FREQSEL_gp     (CLKCTRL_FRQSEL_gp)      /*    Group Position              */
    //                                                      /*    Group Codes                 */
    #define CLKCTRL_FREQSEL_1M_gc  (CLKCTRL_FRQSEL_1M_gc)   /*  1 MHz system clock            */
    #define CLKCTRL_FREQSEL_2M_gc  (CLKCTRL_FRQSEL_2M_gc)   /*  2 MHz system clock            */
    #define CLKCTRL_FREQSEL_3M_gc  (CLKCTRL_FRQSEL_3M_gc)   /*  3 MHz system clock            */
    #define CLKCTRL_FREQSEL_4M_gc  (CLKCTRL_FRQSEL_4M_gc)   /*  4 MHz system clock default    */
    #define CLKCTRL_FREQSEL_8M_gc  (CLKCTRL_FRQSEL_8M_gc)   /*  8 MHz system clock            */
    #define CLKCTRL_FREQSEL_12M_gc (CLKCTRL_FRQSEL_12M_gc)  /* 12 MHz system clock            */
    #define CLKCTRL_FREQSEL_16M_gc (CLKCTRL_FRQSEL_16M_gc)  /* 16 MHz system clock            */
    #define CLKCTRL_FREQSEL_20M_gc (CLKCTRL_FRQSEL_20M_gc)  /* 20 MHz system clock            */
    #define CLKCTRL_FREQSEL_24M_gc (CLKCTRL_FRQSEL_24M_gc)  /* 24 MHz system clock            */
    #define CLKCTRL_FREQSEL_28M_gc (CLKCTRL_FRQSEL_28M_gc)  /* 28 MHz system clock unofficial - this will just error out if used since it will replace one undefined symbol with another */
    #define CLKCTRL_FREQSEL_32M_gc (CLKCTRL_FRQSEL_32M_gc)  /* 32 MHz system clock unofficial - this will just error out if used since it will replace one undefined symbol with another */
  #elif defined(CLKCTRL_FRQSEL_gm) && !defined(CLKCTRL_FREQSEL_gm)
    #define CLKCTRL_FRQSEL_gm     (CLKCTRL_FREQSEL_gm)      /*    Group Mask                  */
    #define CLKCTRL_FRQSEL_gp     (CLKCTRL_FREQSEL_gp)      /*    Group Position              */
    //                                                      /*    Group Codes                 */
    #define CLKCTRL_FRQSEL_1M_gc  (CLKCTRL_FREQSEL_1M_gc)   /*  1 MHz system clock            */
    #define CLKCTRL_FRQSEL_2M_gc  (CLKCTRL_FREQSEL_2M_gc)   /*  2 MHz system clock            */
    #define CLKCTRL_FRQSEL_3M_gc  (CLKCTRL_FREQSEL_3M_gc)   /*  3 MHz system clock            */
    #define CLKCTRL_FRQSEL_4M_gc  (CLKCTRL_FREQSEL_4M_gc)   /*  4 MHz system clock default    */
    #define CLKCTRL_FRQSEL_8M_gc  (CLKCTRL_FREQSEL_8M_gc)   /*  8 MHz system clock            */
    #define CLKCTRL_FRQSEL_12M_gc (CLKCTRL_FREQSEL_12M_gc)  /* 12 MHz system clock            */
    #define CLKCTRL_FRQSEL_16M_gc (CLKCTRL_FREQSEL_16M_gc)  /* 16 MHz system clock            */
    #define CLKCTRL_FRQSEL_20M_gc (CLKCTRL_FREQSEL_20M_gc)  /* 20 MHz system clock            */
    #define CLKCTRL_FRQSEL_24M_gc (CLKCTRL_FREQSEL_24M_gc)  /* 24 MHz system clock            */
    #define CLKCTRL_FRQSEL_28M_gc (CLKCTRL_FREQSEL_28M_gc)  /* 28 MHz system clock unofficial - this will just error out if used since it will replace one undefined symbol with another */
    #define CLKCTRL_FRQSEL_32M_gc (CLKCTRL_FREQSEL_32M_gc)  /* 32 MHz system clock unofficial - this will just error out if used since it will replace one undefined symbol with another */
  #endif
  // Note that it is intended to not hide the fact that 28 and 32 MHz are not official. If you choose it from the menu, it says "Overclocked" next to the speed too. We refer to them with the numeric constants in the wiring.c, so it doesn't matter when used that way.
  // And now the most freaking boneheaded move from Microchip in a long while: They realized that they should have had some sort of delimiter between the bit number within a bitfield, and the name of the bitfield, since the names of many bitfields end in numbers,
  // So they went ahead and made that change. That is what's called a "breaking change", really for no reason except codes style. Most companies even if they decided to go that route, would never do that without introducuing a compatibility layer.
  // That wanton disregard for backwards compatibility is not acceptable in an Arduino core nor in a commercial product.
  // Using the old names will produce warnings. These deprecated names should be fixed as support for these FOUR THOUSAND LINES of bandaids WILL BE REMOBVED in 1.6.0!
  //typedef const uint8_t __attribute__ ((deprecated("\nMicrochip changed the spelling of bits within a bitfiels (macros that end in the bitnumber followed by _bm or _bp), you are using the old name, ex PERIPH_BITFIRLD1_bm.\nYou should use PERIPH_BITFIELD_1_bm; we do not guarantee that this 4000-line bandaid will not be removed in the future.\r\nWhy did they do this? Beats me. Ask their support folks - if enough of us do it, they might hesitate next time they have the urge to mass rename things in their headers")))  deprecated_constant_name;

  // Okay, well that fix didn't work so well. back to plan A.


  /* Add a feature - yay!
   * Rename registers so people can't carry code back and forth - booo!
   */
  // TCA V1.0 - tinyAVR 0/1, megaAVR 0
  // this only has one event input, but code needs to be able to flow smoothly
  // so we define macros named after he the new version pointing to the old version of event input A.
  // Obviously, we can't do anything about the unfortunate soul who tries to use input B.
  #if !defined(TCA_SINGLE_CNTAEI_bm)
    #define TCA_SINGLE_CNTAEI_bm TCA_SINGLE_CNTEI_bm
    #define TCA_SINGLE_EVACTA_POSEDGE_gc TCA_SINGLE_EVACTA_CNT_POSEDGE_gc
    #define TCA_SINGLE_EVACTA_CNT_ANYEDGE_gc TCA_SINGLE_EVACTA_CNT_ANYEDGE_gc
    #define TCA_SINGLE_EVACTA_CNT_HIGHLVL_gc TCA_SINGLE_EVACTA_CNT_HIGHLVL_gc
    #define TCA_SINGLE_EVACTA_UPDOWN_gc TCA_SINGLE_EVACTA_UPDOWN_gc
  #endif
  // TCA V1.1 - DA, DB, tinyAVR 2?
  //  with two inputs changes the names the existing ones to specify channel A
  // We add in the non-postfixed ana
  #if !defined(TCA_SINGLE_CNTEI_bm)
    #define _TCA_
    #define TCA_SINGLE_CNTEI_bm TCA_SINGLE_CNTAEI_bm
    #define TCA_SINGLE_EVACT_POSEDGE_gc TCA_SINGLE_EVACTA_CNT_POSEDGE_gc
    #define TCA_SINGLE_EVACT_CNT_ANYEDGE_gc TCA_SINGLE_EVACTA_CNT_ANYEDGE_gc
    #define TCA_SINGLE_EVACT_CNT_HIGHLVL_gc TCA_SINGLE_EVACTA_CNT_HIGHLVL_gc
    #define TCA_SINGLE_EVACT_UPDOWN_gc TCA_SINGLE_EVACTA_UPDOWN_gc
  #endif

  #if (!defined(MEGATINYCORE) || MEGATINYCORE_SERIES >= 2)
    #define TCB_CLKSEL_CLKDIV1_gc TCB_CLKSEL_DIV1_gc
    #define TCB_CLKSEL_CLKDIV2_gc TCB_CLKSEL_DIV2_gc
    #define TCB_CLKSEL_CLKTCA_gc TCB_CLKSEL_TCA0_gc
  #else
    #define TCB_CLKSEL_DIV1_gc  TCB_CLKSEL_CLKDIV1_gc
    #define TCB_CLKSEL_DIV2_gc  TCB_CLKSEL_CLKDIV2_gc
    #define TCB_CLKSEL_TCA0_gc  TCB_CLKSEL_CLKTCA_gc
    #define TCB_CLKSEL_EVENT_gc  (badCall("This processor does not support TCB count on event mode. Only Dx, Ex, and 2-series tiny support that"))
  #endif
  /* Make sure we error out quickly if told to use an RTC timing option that isn't available. */
  #if (defined(MILLIS_USE_TIMERRTC_XTAL) || defined(MILLIS_USE_TIMERRTC_XOSC))
    #if (MEGATINYCORE_SERIES == 0 || defined(__AVR_ATtinyxy2__))
      #error "Only the tinyAVR 1-series and 2-series parts with at least 14 pins support external RTC timebase"
    #endif
  #endif
  // And now, it it appears that they realized that they should have had some sort of delimiter between the bit number within a bitfield, and the name of the bitfield, since the names of many bitfields end in numbers,
  // So they went ahead and made that change. Without any compatibility layer.

  // Well, I'd wanted to make deprecation warnings come up only if they were used. I was unuccessful.

  // typedef const uint8_t __attribute__ ((deprecated("\nMicrochip changed the spelling of bits within a bitfiels (macros that end in the bitnumber followed by _bm or _bp), you are using the old name, ex PERIPH_BITFIRLD1_bm.\nYou should use PERIPH_BITFIELD_1_bm; we do not guarantee that this 4000-line bandaid will not be removed in the future.\r\nWhy did they do this? Beats me. Ask their support folks - if enough of us do it, they might hesitate next time they have the urge to mass rename things in their headers")))  deprecated_constant_name;

  // Okay, well that fix didn't work so well. back to plan A.
  /* ======= ACs ======= */
  #if !defined(AC_HYSMODE_0_bm) && defined(AC_HYSMODE0_bm)
    #define AC_HYSMODE_0_bm AC_HYSMODE0_bm
  #elif defined(AC_HYSMODE_0_bm)
    //deprecated_constant_name AC_HYSMODE0_bm = AC_HYSMODE_0_bm;
    #define AC_HYSMODE0_bm AC_HYSMODE_0_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_HYSMODE_0_bp) && defined(AC_HYSMODE0_bp)
    #define AC_HYSMODE_0_bp AC_HYSMODE0_bp
  #elif defined(AC_HYSMODE_0_bp)
    //deprecated_constant_name AC_HYSMODE0_bp = AC_HYSMODE_0_bp;
    #define AC_HYSMODE0_bp AC_HYSMODE_0_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_HYSMODE_1_bm) && defined(AC_HYSMODE1_bm)
    #define AC_HYSMODE_1_bm AC_HYSMODE1_bm
  #elif defined(AC_HYSMODE_1_bm)
    //deprecated_constant_name AC_HYSMODE1_bm = AC_HYSMODE_1_bm;
    #define AC_HYSMODE1_bm AC_HYSMODE_1_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_HYSMODE_1_bp) && defined(AC_HYSMODE1_bp)
    #define AC_HYSMODE_1_bp AC_HYSMODE1_bp
  #elif defined(AC_HYSMODE_1_bp)
    //deprecated_constant_name AC_HYSMODE1_bp = AC_HYSMODE_1_bp;
    #define AC_HYSMODE1_bp AC_HYSMODE_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_POWER_0_bm) && defined(AC_POWER0_bm)
    #define AC_POWER_0_bm AC_POWER0_bm
  #elif defined(AC_POWER_0_bm)
    //deprecated_constant_name AC_POWER0_bm = AC_POWER_0_bm;
    #define AC_POWER0_bm AC_POWER_0_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_POWER_0_bp) && defined(AC_POWER0_bp)
    #define AC_POWER_0_bp AC_POWER0_bp
  #elif defined(AC_POWER_0_bp)
    //deprecated_constant_name AC_POWER0_bp = AC_POWER_0_bp;
    #define AC_POWER0_bp AC_POWER_0_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_POWER_1_bm) && defined(AC_POWER1_bm)
    #define AC_POWER_1_bm AC_POWER1_bm
  #elif defined(AC_POWER_1_bm)
    //deprecated_constant_name AC_POWER1_bm = AC_POWER_1_bm;
    #define AC_POWER1_bm AC_POWER_1_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_POWER_1_bp) && defined(AC_POWER1_bp)
    #define AC_POWER_1_bp AC_POWER1_bp
  #elif defined(AC_POWER_1_bp)
    //deprecated_constant_name AC_POWER1_bp = AC_POWER_1_bp;
    #define AC_POWER1_bp AC_POWER_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_WINSEL_0_bm) && defined(AC_WINSEL0_bm)
    #define AC_WINSEL_0_bm AC_WINSEL0_bm
  #elif defined(AC_WINSEL_0_bm)
    //deprecated_constant_name AC_WINSEL0_bm = AC_WINSEL_0_bm;
    #define AC_WINSEL0_bm AC_WINSEL_0_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_WINSEL_0_bp) && defined(AC_WINSEL0_bp)
    #define AC_WINSEL_0_bp AC_WINSEL0_bp
  #elif defined(AC_WINSEL_0_bp)
    //deprecated_constant_name AC_WINSEL0_bp = AC_WINSEL_0_bp;
    #define AC_WINSEL0_bp AC_WINSEL_0_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_WINSEL_1_bm) && defined(AC_WINSEL1_bm)
    #define AC_WINSEL_1_bm AC_WINSEL1_bm
  #elif defined(AC_WINSEL_1_bm)
    //deprecated_constant_name AC_WINSEL1_bm = AC_WINSEL_1_bm;
    #define AC_WINSEL1_bm AC_WINSEL_1_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_WINSEL_1_bp) && defined(AC_WINSEL1_bp)
    #define AC_WINSEL_1_bp AC_WINSEL1_bp
  #elif defined(AC_WINSEL_1_bp)
    //deprecated_constant_name AC_WINSEL1_bp = AC_WINSEL_1_bp;
    #define AC_WINSEL1_bp AC_WINSEL_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_MUXNEG_0_bm) && defined(AC_MUXNEG0_bm)
    #define AC_MUXNEG_0_bm AC_MUXNEG0_bm
  #elif defined(AC_MUXNEG_0_bm)
    //deprecated_constant_name AC_MUXNEG0_bm = AC_MUXNEG_0_bm;
    #define AC_MUXNEG0_bm AC_MUXNEG_0_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_MUXNEG_0_bp) && defined(AC_MUXNEG0_bp)
    #define AC_MUXNEG_0_bp AC_MUXNEG0_bp
  #elif defined(AC_MUXNEG_0_bp)
    //deprecated_constant_name AC_MUXNEG0_bp = AC_MUXNEG_0_bp;
    #define AC_MUXNEG0_bp AC_MUXNEG_0_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_MUXNEG_1_bm) && defined(AC_MUXNEG1_bm)
    #define AC_MUXNEG_1_bm AC_MUXNEG1_bm
  #elif defined(AC_MUXNEG_1_bm)
    //deprecated_constant_name AC_MUXNEG1_bm = AC_MUXNEG_1_bm;
    #define AC_MUXNEG1_bm AC_MUXNEG_1_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_MUXNEG_1_bp) && defined(AC_MUXNEG1_bp)
    #define AC_MUXNEG_1_bp AC_MUXNEG1_bp
  #elif defined(AC_MUXNEG_1_bp)
    //deprecated_constant_name AC_MUXNEG1_bp = AC_MUXNEG_1_bp;
    #define AC_MUXNEG1_bp AC_MUXNEG_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_MUXNEG_2_bm) && defined(AC_MUXNEG2_bm)
    #define AC_MUXNEG_2_bm AC_MUXNEG2_bm
  #elif defined(AC_MUXNEG_2_bm)
    //deprecated_constant_name AC_MUXNEG2_bm = AC_MUXNEG_2_bm;
    #define AC_MUXNEG2_bm AC_MUXNEG_2_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_MUXNEG_2_bp) && defined(AC_MUXNEG2_bp)
    #define AC_MUXNEG_2_bp AC_MUXNEG2_bp
  #elif defined(AC_MUXNEG_2_bp)
    //deprecated_constant_name AC_MUXNEG2_bp = AC_MUXNEG_2_bp;
    #define AC_MUXNEG2_bp AC_MUXNEG_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_MUXPOS_0_bm) && defined(AC_MUXPOS0_bm)
    #define AC_MUXPOS_0_bm AC_MUXPOS0_bm
  #elif defined(AC_MUXPOS_0_bm)
    //deprecated_constant_name AC_MUXPOS0_bm = AC_MUXPOS_0_bm;
    #define AC_MUXPOS0_bm AC_MUXPOS_0_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_MUXPOS_0_bp) && defined(AC_MUXPOS0_bp)
    #define AC_MUXPOS_0_bp AC_MUXPOS0_bp
  #elif defined(AC_MUXPOS_0_bp)
    //deprecated_constant_name AC_MUXPOS0_bp = AC_MUXPOS_0_bp;
    #define AC_MUXPOS0_bp AC_MUXPOS_0_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_MUXPOS_1_bm) && defined(AC_MUXPOS1_bm)
    #define AC_MUXPOS_1_bm AC_MUXPOS1_bm
  #elif defined(AC_MUXPOS_1_bm)
    //deprecated_constant_name AC_MUXPOS1_bm = AC_MUXPOS_1_bm;
    #define AC_MUXPOS1_bm AC_MUXPOS_1_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_MUXPOS_1_bp) && defined(AC_MUXPOS1_bp)
    #define AC_MUXPOS_1_bp AC_MUXPOS1_bp
  #elif defined(AC_MUXPOS_1_bp)
    //deprecated_constant_name AC_MUXPOS1_bp = AC_MUXPOS_1_bp;
    #define AC_MUXPOS1_bp AC_MUXPOS_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_MUXPOS_2_bm) && defined(AC_MUXPOS2_bm)
    #define AC_MUXPOS_2_bm AC_MUXPOS2_bm
  #elif defined(AC_MUXPOS_2_bm)
    //deprecated_constant_name AC_MUXPOS2_bm = AC_MUXPOS_2_bm;
    #define AC_MUXPOS2_bm AC_MUXPOS_2_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_MUXPOS_2_bp) && defined(AC_MUXPOS2_bp)
    #define AC_MUXPOS_2_bp AC_MUXPOS2_bp
  #elif defined(AC_MUXPOS_2_bp)
    //deprecated_constant_name AC_MUXPOS2_bp = AC_MUXPOS_2_bp;
    #define AC_MUXPOS2_bp AC_MUXPOS_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_INTMODE_NORMAL_0_bm) && defined(AC_INTMODE_NORMAL0_bm)
    #define AC_INTMODE_NORMAL_0_bm AC_INTMODE_NORMAL0_bm
  #elif defined(AC_INTMODE_NORMAL_0_bm)
    //deprecated_constant_name AC_INTMODE_NORMAL0_bm = AC_INTMODE_NORMAL_0_bm;
    #define AC_INTMODE_NORMAL0_bm AC_INTMODE_NORMAL_0_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_INTMODE_NORMAL_0_bp) && defined(AC_INTMODE_NORMAL0_bp)
    #define AC_INTMODE_NORMAL_0_bp AC_INTMODE_NORMAL0_bp
  #elif defined(AC_INTMODE_NORMAL_0_bp)
    //deprecated_constant_name AC_INTMODE_NORMAL0_bp = AC_INTMODE_NORMAL_0_bp;
    #define AC_INTMODE_NORMAL0_bp AC_INTMODE_NORMAL_0_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_INTMODE_NORMAL_1_bm) && defined(AC_INTMODE_NORMAL1_bm)
    #define AC_INTMODE_NORMAL_1_bm AC_INTMODE_NORMAL1_bm
  #elif defined(AC_INTMODE_NORMAL_1_bm)
    //deprecated_constant_name AC_INTMODE_NORMAL1_bm = AC_INTMODE_NORMAL_1_bm;
    #define AC_INTMODE_NORMAL1_bm AC_INTMODE_NORMAL_1_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_INTMODE_NORMAL_1_bp) && defined(AC_INTMODE_NORMAL1_bp)
    #define AC_INTMODE_NORMAL_1_bp AC_INTMODE_NORMAL1_bp
  #elif defined(AC_INTMODE_NORMAL_1_bp)
    //deprecated_constant_name AC_INTMODE_NORMAL1_bp = AC_INTMODE_NORMAL_1_bp;
    #define AC_INTMODE_NORMAL1_bp AC_INTMODE_NORMAL_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_INTMODE_WINDOW_0_bm) && defined(AC_INTMODE_WINDOW0_bm)
    #define AC_INTMODE_WINDOW_0_bm AC_INTMODE_WINDOW0_bm
  #elif defined(AC_INTMODE_WINDOW_0_bm)
    //deprecated_constant_name AC_INTMODE_WINDOW0_bm = AC_INTMODE_WINDOW_0_bm;
    #define AC_INTMODE_WINDOW0_bm AC_INTMODE_WINDOW_0_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_INTMODE_WINDOW_0_bp) && defined(AC_INTMODE_WINDOW0_bp)
    #define AC_INTMODE_WINDOW_0_bp AC_INTMODE_WINDOW0_bp
  #elif defined(AC_INTMODE_WINDOW_0_bp)
    //deprecated_constant_name AC_INTMODE_WINDOW0_bp = AC_INTMODE_WINDOW_0_bp;
    #define AC_INTMODE_WINDOW0_bp AC_INTMODE_WINDOW_0_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_INTMODE_WINDOW_1_bm) && defined(AC_INTMODE_WINDOW1_bm)
    #define AC_INTMODE_WINDOW_1_bm AC_INTMODE_WINDOW1_bm
  #elif defined(AC_INTMODE_WINDOW_1_bm)
    //deprecated_constant_name AC_INTMODE_WINDOW1_bm = AC_INTMODE_WINDOW_1_bm;
    #define AC_INTMODE_WINDOW1_bm AC_INTMODE_WINDOW_1_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_INTMODE_WINDOW_1_bp) && defined(AC_INTMODE_WINDOW1_bp)
    #define AC_INTMODE_WINDOW_1_bp AC_INTMODE_WINDOW1_bp
  #elif defined(AC_INTMODE_WINDOW_1_bp)
    //deprecated_constant_name AC_INTMODE_WINDOW1_bp = AC_INTMODE_WINDOW_1_bp;
    #define AC_INTMODE_WINDOW1_bp AC_INTMODE_WINDOW_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_WINSTATE_0_bm) && defined(AC_WINSTATE0_bm)
    #define AC_WINSTATE_0_bm AC_WINSTATE0_bm
  #elif defined(AC_WINSTATE_0_bm)
    //deprecated_constant_name AC_WINSTATE0_bm = AC_WINSTATE_0_bm;
    #define AC_WINSTATE0_bm AC_WINSTATE_0_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_WINSTATE_0_bp) && defined(AC_WINSTATE0_bp)
    #define AC_WINSTATE_0_bp AC_WINSTATE0_bp
  #elif defined(AC_WINSTATE_0_bp)
    //deprecated_constant_name AC_WINSTATE0_bp = AC_WINSTATE_0_bp;
    #define AC_WINSTATE0_bp AC_WINSTATE_0_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_WINSTATE_1_bm) && defined(AC_WINSTATE1_bm)
    #define AC_WINSTATE_1_bm AC_WINSTATE1_bm
  #elif defined(AC_WINSTATE_1_bm)
    //deprecated_constant_name AC_WINSTATE1_bm = AC_WINSTATE_1_bm;
    #define AC_WINSTATE1_bm AC_WINSTATE_1_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(AC_WINSTATE_1_bp) && defined(AC_WINSTATE1_bp)
    #define AC_WINSTATE_1_bp AC_WINSTATE1_bp
  #elif defined(AC_WINSTATE_1_bp)
    //deprecated_constant_name AC_WINSTATE1_bp = AC_WINSTATE_1_bp;
    #define AC_WINSTATE1_bp AC_WINSTATE_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= ADC ======= */
  #if !defined(ADC_RESSEL_0_bm) && defined(ADC_RESSEL0_bm)
    #define ADC_RESSEL_0_bm ADC_RESSEL0_bm
  #elif defined(ADC_RESSEL_0_bm)
    #undef ADC_RESSEL0_bm /* this special case does have a compatibility layer, but the deprecation is only provided by a comement so users will not be warned that the compatibility layer may be removed in the future. So we undefine in and handle as above */
    //deprecated_constant_name ADC_RESSEL0_bm = ADC_RESSEL_0_bm;
    #define ADC_RESSEL0_bm ADC_RESSEL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_RESSEL_0_bp) && defined(ADC_RESSEL0_bp)
    #define ADC_RESSEL_0_bp ADC_RESSEL0_bp
  #elif defined(ADC_RESSEL_0_bp)
    #undef ADC_RESSEL0_bp /* this special case does have a compatibility layer, but the deprecation is only provided by a comement so users will not be warned that the compatibility layer may be removed in the future. So we undefine in and handle as above */
    //deprecated_constant_name ADC_RESSEL0_bp = ADC_RESSEL_0_bp;
    #define ADC_RESSEL0_bp ADC_RESSEL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_RESSEL_1_bm) && defined(ADC_RESSEL1_bm)
    #define ADC_RESSEL_1_bm ADC_RESSEL1_bm
  #elif defined(ADC_RESSEL_1_bm)
    #undef ADC_RESSEL1_bm /* this special case does have a compatibility layer, but the deprecation is only provided by a comement so users will not be warned that the compatibility layer may be removed in the future. So we undefine in and handle as above */
    //deprecated_constant_name ADC_RESSEL1_bm = ADC_RESSEL_1_bm;
    #define ADC_RESSEL1_bm ADC_RESSEL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_RESSEL_1_bp) && defined(ADC_RESSEL1_bp)
    #define ADC_RESSEL_1_bp ADC_RESSEL1_bp
  #elif defined(ADC_RESSEL_1_bp)
    #undef ADC_RESSEL1_bp /* this special case does have a compatibility layer, but the deprecation is only provided by a comement so users will not be warned that the compatibility layer may be removed in the future. So we undefine in and handle as above */
    //deprecated_constant_name ADC_RESSEL1_bp = ADC_RESSEL_1_bp;
    #define ADC_RESSEL1_bp ADC_RESSEL_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(ADC_SAMPNUM_0_bm) && defined(ADC_SAMPNUM0_bm)
    #define ADC_SAMPNUM_0_bm ADC_SAMPNUM0_bm
  #elif defined(ADC_SAMPNUM_0_bm)
    //deprecated_constant_name ADC_SAMPNUM0_bm = ADC_SAMPNUM_0_bm;
    #define ADC_SAMPNUM0_bm ADC_SAMPNUM_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_SAMPNUM_0_bp) && defined(ADC_SAMPNUM0_bp)
    #define ADC_SAMPNUM_0_bp ADC_SAMPNUM0_bp
  #elif defined(ADC_SAMPNUM_0_bp)
    //deprecated_constant_name ADC_SAMPNUM0_bp = ADC_SAMPNUM_0_bp;
    #define ADC_SAMPNUM0_bp ADC_SAMPNUM_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_SAMPNUM_1_bm) && defined(ADC_SAMPNUM1_bm)
    #define ADC_SAMPNUM_1_bm ADC_SAMPNUM1_bm
  #elif defined(ADC_SAMPNUM_1_bm)
    //deprecated_constant_name ADC_SAMPNUM1_bm = ADC_SAMPNUM_1_bm;
    #define ADC_SAMPNUM1_bm ADC_SAMPNUM_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_SAMPNUM_1_bp) && defined(ADC_SAMPNUM1_bp)
    #define ADC_SAMPNUM_1_bp ADC_SAMPNUM1_bp
  #elif defined(ADC_SAMPNUM_1_bp)
    //deprecated_constant_name ADC_SAMPNUM1_bp = ADC_SAMPNUM_1_bp;
    #define ADC_SAMPNUM1_bp ADC_SAMPNUM_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_SAMPNUM_2_bm) && defined(ADC_SAMPNUM2_bm)
    #define ADC_SAMPNUM_2_bm ADC_SAMPNUM2_bm
  #elif defined(ADC_SAMPNUM_2_bm)
    //deprecated_constant_name ADC_SAMPNUM2_bm = ADC_SAMPNUM_2_bm;
    #define ADC_SAMPNUM2_bm ADC_SAMPNUM_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_SAMPNUM_2_bp) && defined(ADC_SAMPNUM2_bp)
    #define ADC_SAMPNUM_2_bp ADC_SAMPNUM2_bp
  #elif defined(ADC_SAMPNUM_2_bp)
    //deprecated_constant_name ADC_SAMPNUM2_bp = ADC_SAMPNUM_2_bp;
    #define ADC_SAMPNUM2_bp ADC_SAMPNUM_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(ADC_PRESC_0_bm) && defined(ADC_PRESC0_bm)
    #define ADC_PRESC_0_bm ADC_PRESC0_bm
  #elif defined(ADC_PRESC_0_bm)
    //deprecated_constant_name ADC_PRESC0_bm = ADC_PRESC_0_bm;
    #define ADC_PRESC0_bm ADC_PRESC_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_PRESC_0_bp) && defined(ADC_PRESC0_bp)
    #define ADC_PRESC_0_bp ADC_PRESC0_bp
  #elif defined(ADC_PRESC_0_bp)
    //deprecated_constant_name ADC_PRESC0_bp = ADC_PRESC_0_bp;
    #define ADC_PRESC0_bp ADC_PRESC_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_PRESC_1_bm) && defined(ADC_PRESC1_bm)
    #define ADC_PRESC_1_bm ADC_PRESC1_bm
  #elif defined(ADC_PRESC_1_bm)
    //deprecated_constant_name ADC_PRESC1_bm = ADC_PRESC_1_bm;
    #define ADC_PRESC1_bm ADC_PRESC_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_PRESC_1_bp) && defined(ADC_PRESC1_bp)
    #define ADC_PRESC_1_bp ADC_PRESC1_bp
  #elif defined(ADC_PRESC_1_bp)
    //deprecated_constant_name ADC_PRESC1_bp = ADC_PRESC_1_bp;
    #define ADC_PRESC1_bp ADC_PRESC_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_PRESC_2_bm) && defined(ADC_PRESC2_bm)
    #define ADC_PRESC_2_bm ADC_PRESC2_bm
  #elif defined(ADC_PRESC_2_bm)
    //deprecated_constant_name ADC_PRESC2_bm = ADC_PRESC_2_bm;
    #define ADC_PRESC2_bm ADC_PRESC_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_PRESC_2_bp) && defined(ADC_PRESC2_bp)
    #define ADC_PRESC_2_bp ADC_PRESC2_bp
  #elif defined(ADC_PRESC_2_bp)
    //deprecated_constant_name ADC_PRESC2_bp = ADC_PRESC_2_bp;
    #define ADC_PRESC2_bp ADC_PRESC_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_PRESC_3_bm) && defined(ADC_PRESC3_bm)
    #define ADC_PRESC_3_bm ADC_PRESC3_bm
  #elif defined(ADC_PRESC_3_bm)
    //deprecated_constant_name ADC_PRESC3_bm = ADC_PRESC_3_bm;
    #define ADC_PRESC3_bm ADC_PRESC_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_PRESC_3_bp) && defined(ADC_PRESC3_bp)
    #define ADC_PRESC_3_bp ADC_PRESC3_bp
  #elif defined(ADC_PRESC_3_bp)
    //deprecated_constant_name ADC_PRESC3_bp = ADC_PRESC_3_bp;
    #define ADC_PRESC3_bp ADC_PRESC_3_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(ADC_SAMPDLY_0_bm) && defined(ADC_SAMPDLY0_bm)
    #define ADC_SAMPDLY_0_bm ADC_SAMPDLY0_bm
  #elif defined(ADC_SAMPDLY_0_bm)
    //deprecated_constant_name ADC_SAMPDLY0_bm = ADC_SAMPDLY_0_bm;
    #define ADC_SAMPDLY0_bm ADC_SAMPDLY_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_SAMPDLY_0_bp) && defined(ADC_SAMPDLY0_bp)
    #define ADC_SAMPDLY_0_bp ADC_SAMPDLY0_bp
  #elif defined(ADC_SAMPDLY_0_bp)
    //deprecated_constant_name ADC_SAMPDLY0_bp = ADC_SAMPDLY_0_bp;
    #define ADC_SAMPDLY0_bp ADC_SAMPDLY_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_SAMPDLY_1_bm) && defined(ADC_SAMPDLY1_bm)
    #define ADC_SAMPDLY_1_bm ADC_SAMPDLY1_bm
  #elif defined(ADC_SAMPDLY_1_bm)
    //deprecated_constant_name ADC_SAMPDLY1_bm = ADC_SAMPDLY_1_bm;
    #define ADC_SAMPDLY1_bm ADC_SAMPDLY_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_SAMPDLY_1_bp) && defined(ADC_SAMPDLY1_bp)
    #define ADC_SAMPDLY_1_bp ADC_SAMPDLY1_bp
  #elif defined(ADC_SAMPDLY_1_bp)
    //deprecated_constant_name ADC_SAMPDLY1_bp = ADC_SAMPDLY_1_bp;
    #define ADC_SAMPDLY1_bp ADC_SAMPDLY_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_SAMPDLY_2_bm) && defined(ADC_SAMPDLY2_bm)
    #define ADC_SAMPDLY_2_bm ADC_SAMPDLY2_bm
  #elif defined(ADC_SAMPDLY_2_bm)
    //deprecated_constant_name ADC_SAMPDLY2_bm = ADC_SAMPDLY_2_bm;
    #define ADC_SAMPDLY2_bm ADC_SAMPDLY_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_SAMPDLY_2_bp) && defined(ADC_SAMPDLY2_bp)
    #define ADC_SAMPDLY_2_bp ADC_SAMPDLY2_bp
  #elif defined(ADC_SAMPDLY_2_bp)
    //deprecated_constant_name ADC_SAMPDLY2_bp = ADC_SAMPDLY_2_bp;
    #define ADC_SAMPDLY2_bp ADC_SAMPDLY_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_SAMPDLY_3_bm) && defined(ADC_SAMPDLY3_bm)
    #define ADC_SAMPDLY_3_bm ADC_SAMPDLY3_bm
  #elif defined(ADC_SAMPDLY_3_bm)
    //deprecated_constant_name ADC_SAMPDLY3_bm = ADC_SAMPDLY_3_bm;
    #define ADC_SAMPDLY3_bm ADC_SAMPDLY_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_SAMPDLY_3_bp) && defined(ADC_SAMPDLY3_bp)
    #define ADC_SAMPDLY_3_bp ADC_SAMPDLY3_bp
  #elif defined(ADC_SAMPDLY_3_bp)
    //deprecated_constant_name ADC_SAMPDLY3_bp = ADC_SAMPDLY_3_bp;
    #define ADC_SAMPDLY3_bp ADC_SAMPDLY_3_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(ADC_INITDLY_0_bm) && defined(ADC_INITDLY0_bm)
    #define ADC_INITDLY_0_bm ADC_INITDLY0_bm
  #elif defined(ADC_INITDLY_0_bm)
    //deprecated_constant_name ADC_INITDLY0_bm = ADC_INITDLY_0_bm;
    #define ADC_INITDLY0_bm ADC_INITDLY_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_INITDLY_0_bp) && defined(ADC_INITDLY0_bp)
    #define ADC_INITDLY_0_bp ADC_INITDLY0_bp
  #elif defined(ADC_INITDLY_0_bp)
    //deprecated_constant_name ADC_INITDLY0_bp = ADC_INITDLY_0_bp;
    #define ADC_INITDLY0_bp ADC_INITDLY_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_INITDLY_1_bm) && defined(ADC_INITDLY1_bm)
    #define ADC_INITDLY_1_bm ADC_INITDLY1_bm
  #elif defined(ADC_INITDLY_1_bm)
    //deprecated_constant_name ADC_INITDLY1_bm = ADC_INITDLY_1_bm;
    #define ADC_INITDLY1_bm ADC_INITDLY_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_INITDLY_1_bp) && defined(ADC_INITDLY1_bp)
    #define ADC_INITDLY_1_bp ADC_INITDLY1_bp
  #elif defined(ADC_INITDLY_1_bp)
    //deprecated_constant_name ADC_INITDLY1_bp = ADC_INITDLY_1_bp;
    #define ADC_INITDLY1_bp ADC_INITDLY_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_INITDLY_2_bm) && defined(ADC_INITDLY2_bm)
    #define ADC_INITDLY_2_bm ADC_INITDLY2_bm
  #elif defined(ADC_INITDLY_2_bm)
    //deprecated_constant_name ADC_INITDLY2_bm = ADC_INITDLY_2_bm;
    #define ADC_INITDLY2_bm ADC_INITDLY_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_INITDLY_2_bp) && defined(ADC_INITDLY2_bp)
    #define ADC_INITDLY_2_bp ADC_INITDLY2_bp
  #elif defined(ADC_INITDLY_2_bp)
    //deprecated_constant_name ADC_INITDLY2_bp = ADC_INITDLY_2_bp;
    #define ADC_INITDLY2_bp ADC_INITDLY_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(ADC_WINCM_0_bm) && defined(ADC_WINCM0_bm)
    #define ADC_WINCM_0_bm ADC_WINCM0_bm
  #elif defined(ADC_WINCM_0_bm)
    //deprecated_constant_name ADC_WINCM0_bm = ADC_WINCM_0_bm;
    #define ADC_WINCM0_bm ADC_WINCM_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_WINCM_0_bp) && defined(ADC_WINCM0_bp)
    #define ADC_WINCM_0_bp ADC_WINCM0_bp
  #elif defined(ADC_WINCM_0_bp)
    //deprecated_constant_name ADC_WINCM0_bp = ADC_WINCM_0_bp;
    #define ADC_WINCM0_bp ADC_WINCM_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_WINCM_1_bm) && defined(ADC_WINCM1_bm)
    #define ADC_WINCM_1_bm ADC_WINCM1_bm
  #elif defined(ADC_WINCM_1_bm)
    //deprecated_constant_name ADC_WINCM1_bm = ADC_WINCM_1_bm;
    #define ADC_WINCM1_bm ADC_WINCM_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_WINCM_1_bp) && defined(ADC_WINCM1_bp)
    #define ADC_WINCM_1_bp ADC_WINCM1_bp
  #elif defined(ADC_WINCM_1_bp)
    //deprecated_constant_name ADC_WINCM1_bp = ADC_WINCM_1_bp;
    #define ADC_WINCM1_bp ADC_WINCM_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_WINCM_2_bm) && defined(ADC_WINCM2_bm)
    #define ADC_WINCM_2_bm ADC_WINCM2_bm
  #elif defined(ADC_WINCM_2_bm)
    //deprecated_constant_name ADC_WINCM2_bm = ADC_WINCM_2_bm;
    #define ADC_WINCM2_bm ADC_WINCM_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(ADC_WINCM_2_bp) && defined(ADC_WINCM2_bp)
    #define ADC_WINCM_2_bp ADC_WINCM2_bp
  #elif defined(ADC_WINCM_2_bp)
    //deprecated_constant_name ADC_WINCM2_bp = ADC_WINCM_2_bp;
    #define ADC_WINCM2_bp ADC_WINCM_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= BOD ======= */
  #if !defined(BOD_SLEEP_0_bm) && defined(BOD_SLEEP0_bm)
    #define BOD_SLEEP_0_bm BOD_SLEEP0_bm
  #elif defined(BOD_SLEEP_0_bm)
    //deprecated_constant_name BOD_SLEEP0_bm = BOD_SLEEP_0_bm;
    #define BOD_SLEEP0_bm BOD_SLEEP_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_SLEEP_0_bp) && defined(BOD_SLEEP0_bp)
    #define BOD_SLEEP_0_bp BOD_SLEEP0_bp
  #elif defined(BOD_SLEEP_0_bp)
    //deprecated_constant_name BOD_SLEEP0_bp = BOD_SLEEP_0_bp;
    #define BOD_SLEEP0_bp BOD_SLEEP_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_SLEEP_1_bm) && defined(BOD_SLEEP1_bm)
    #define BOD_SLEEP_1_bm BOD_SLEEP1_bm
  #elif defined(BOD_SLEEP_1_bm)
    //deprecated_constant_name BOD_SLEEP1_bm = BOD_SLEEP_1_bm;
    #define BOD_SLEEP1_bm BOD_SLEEP_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_SLEEP_1_bp) && defined(BOD_SLEEP1_bp)
    #define BOD_SLEEP_1_bp BOD_SLEEP1_bp
  #elif defined(BOD_SLEEP_1_bp)
    //deprecated_constant_name BOD_SLEEP1_bp = BOD_SLEEP_1_bp;
    #define BOD_SLEEP1_bp BOD_SLEEP_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(BOD_ACTIVE_0_bm) && defined(BOD_ACTIVE0_bm)
    #define BOD_ACTIVE_0_bm BOD_ACTIVE0_bm
  #elif defined(BOD_ACTIVE_0_bm)
    //deprecated_constant_name BOD_ACTIVE0_bm = BOD_ACTIVE_0_bm;
    #define BOD_ACTIVE0_bm BOD_ACTIVE_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_ACTIVE_0_bp) && defined(BOD_ACTIVE0_bp)
    #define BOD_ACTIVE_0_bp BOD_ACTIVE0_bp
  #elif defined(BOD_ACTIVE_0_bp)
    //deprecated_constant_name BOD_ACTIVE0_bp = BOD_ACTIVE_0_bp;
    #define BOD_ACTIVE0_bp BOD_ACTIVE_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_ACTIVE_1_bm) && defined(BOD_ACTIVE1_bm)
    #define BOD_ACTIVE_1_bm BOD_ACTIVE1_bm
  #elif defined(BOD_ACTIVE_1_bm)
    //deprecated_constant_name BOD_ACTIVE1_bm = BOD_ACTIVE_1_bm;
    #define BOD_ACTIVE1_bm BOD_ACTIVE_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_ACTIVE_1_bp) && defined(BOD_ACTIVE1_bp)
    #define BOD_ACTIVE_1_bp BOD_ACTIVE1_bp
  #elif defined(BOD_ACTIVE_1_bp)
    //deprecated_constant_name BOD_ACTIVE1_bp = BOD_ACTIVE_1_bp;
    #define BOD_ACTIVE1_bp BOD_ACTIVE_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(BOD_LVL_0_bm) && defined(BOD_LVL0_bm)
    #define BOD_LVL_0_bm BOD_LVL0_bm
  #elif defined(BOD_LVL_0_bm)
    //deprecated_constant_name BOD_LVL0_bm = BOD_LVL_0_bm;
    #define BOD_LVL0_bm BOD_LVL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_LVL_0_bp) && defined(BOD_LVL0_bp)
    #define BOD_LVL_0_bp BOD_LVL0_bp
  #elif defined(BOD_LVL_0_bp)
    //deprecated_constant_name BOD_LVL0_bp = BOD_LVL_0_bp;
    #define BOD_LVL0_bp BOD_LVL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_LVL_1_bm) && defined(BOD_LVL1_bm)
    #define BOD_LVL_1_bm BOD_LVL1_bm
  #elif defined(BOD_LVL_1_bm)
    //deprecated_constant_name BOD_LVL1_bm = BOD_LVL_1_bm;
    #define BOD_LVL1_bm BOD_LVL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_LVL_1_bp) && defined(BOD_LVL1_bp)
    #define BOD_LVL_1_bp BOD_LVL1_bp
  #elif defined(BOD_LVL_1_bp)
    //deprecated_constant_name BOD_LVL1_bp = BOD_LVL_1_bp;
    #define BOD_LVL1_bp BOD_LVL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_LVL_2_bm) && defined(BOD_LVL2_bm)
    #define BOD_LVL_2_bm BOD_LVL2_bm
  #elif defined(BOD_LVL_2_bm)
    //deprecated_constant_name BOD_LVL2_bm = BOD_LVL_2_bm;
    #define BOD_LVL2_bm BOD_LVL_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_LVL_2_bp) && defined(BOD_LVL2_bp)
    #define BOD_LVL_2_bp BOD_LVL2_bp
  #elif defined(BOD_LVL_2_bp)
    //deprecated_constant_name BOD_LVL2_bp = BOD_LVL_2_bp;
    #define BOD_LVL2_bp BOD_LVL_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(BOD_VLMLVL_0_bm) && defined(BOD_VLMLVL0_bm)
    #define BOD_VLMLVL_0_bm BOD_VLMLVL0_bm
  #elif defined(BOD_VLMLVL_0_bm)
    //deprecated_constant_name BOD_VLMLVL0_bm = BOD_VLMLVL_0_bm;
    #define BOD_VLMLVL0_bm BOD_VLMLVL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_VLMLVL_0_bp) && defined(BOD_VLMLVL0_bp)
    #define BOD_VLMLVL_0_bp BOD_VLMLVL0_bp
  #elif defined(BOD_VLMLVL_0_bp)
    //deprecated_constant_name BOD_VLMLVL0_bp = BOD_VLMLVL_0_bp;
    #define BOD_VLMLVL0_bp BOD_VLMLVL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_VLMLVL_1_bm) && defined(BOD_VLMLVL1_bm)
    #define BOD_VLMLVL_1_bm BOD_VLMLVL1_bm
  #elif defined(BOD_VLMLVL_1_bm)
    //deprecated_constant_name BOD_VLMLVL1_bm = BOD_VLMLVL_1_bm;
    #define BOD_VLMLVL1_bm BOD_VLMLVL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_VLMLVL_1_bp) && defined(BOD_VLMLVL1_bp)
    #define BOD_VLMLVL_1_bp BOD_VLMLVL1_bp
  #elif defined(BOD_VLMLVL_1_bp)
    //deprecated_constant_name BOD_VLMLVL1_bp = BOD_VLMLVL_1_bp;
    #define BOD_VLMLVL1_bp BOD_VLMLVL_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(BOD_VLMCFG_0_bm) && defined(BOD_VLMCFG0_bm)
    #define BOD_VLMCFG_0_bm BOD_VLMCFG0_bm
  #elif defined(BOD_VLMCFG_0_bm)
    //deprecated_constant_name BOD_VLMCFG0_bm = BOD_VLMCFG_0_bm;
    #define BOD_VLMCFG0_bm BOD_VLMCFG_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_VLMCFG_0_bp) && defined(BOD_VLMCFG0_bp)
    #define BOD_VLMCFG_0_bp BOD_VLMCFG0_bp
  #elif defined(BOD_VLMCFG_0_bp)
    //deprecated_constant_name BOD_VLMCFG0_bp = BOD_VLMCFG_0_bp;
    #define BOD_VLMCFG0_bp BOD_VLMCFG_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_VLMCFG_1_bm) && defined(BOD_VLMCFG1_bm)
    #define BOD_VLMCFG_1_bm BOD_VLMCFG1_bm
  #elif defined(BOD_VLMCFG_1_bm)
    //deprecated_constant_name BOD_VLMCFG1_bm = BOD_VLMCFG_1_bm;
    #define BOD_VLMCFG1_bm BOD_VLMCFG_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(BOD_VLMCFG_1_bp) && defined(BOD_VLMCFG1_bp)
    #define BOD_VLMCFG_1_bp BOD_VLMCFG1_bp
  #elif defined(BOD_VLMCFG_1_bp)
    //deprecated_constant_name BOD_VLMCFG1_bp = BOD_VLMCFG_1_bp;
    #define BOD_VLMCFG1_bp BOD_VLMCFG_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= CCL ======= */
  #if !defined(CCL_SEQSEL_0_bm) && defined(CCL_SEQSEL0_bm)
    #define CCL_SEQSEL_0_bm CCL_SEQSEL0_bm
  #elif defined(CCL_SEQSEL_0_bm)
    //deprecated_constant_name CCL_SEQSEL0_bm = CCL_SEQSEL_0_bm;
    #define CCL_SEQSEL0_bm CCL_SEQSEL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_SEQSEL_0_bp) && defined(CCL_SEQSEL0_bp)
    #define CCL_SEQSEL_0_bp CCL_SEQSEL0_bp
  #elif defined(CCL_SEQSEL_0_bp)
    //deprecated_constant_name CCL_SEQSEL0_bp = CCL_SEQSEL_0_bp;
    #define CCL_SEQSEL0_bp CCL_SEQSEL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_SEQSEL_1_bm) && defined(CCL_SEQSEL1_bm)
    #define CCL_SEQSEL_1_bm CCL_SEQSEL1_bm
  #elif defined(CCL_SEQSEL_1_bm)
    //deprecated_constant_name CCL_SEQSEL1_bm = CCL_SEQSEL_1_bm;
    #define CCL_SEQSEL1_bm CCL_SEQSEL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_SEQSEL_1_bp) && defined(CCL_SEQSEL1_bp)
    #define CCL_SEQSEL_1_bp CCL_SEQSEL1_bp
  #elif defined(CCL_SEQSEL_1_bp)
    //deprecated_constant_name CCL_SEQSEL1_bp = CCL_SEQSEL_1_bp;
    #define CCL_SEQSEL1_bp CCL_SEQSEL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_SEQSEL_2_bm) && defined(CCL_SEQSEL2_bm)
    #define CCL_SEQSEL_2_bm CCL_SEQSEL2_bm
  #elif defined(CCL_SEQSEL_2_bm)
    //deprecated_constant_name CCL_SEQSEL2_bm = CCL_SEQSEL_2_bm;
    #define CCL_SEQSEL2_bm CCL_SEQSEL_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_SEQSEL_2_bp) && defined(CCL_SEQSEL2_bp)
    #define CCL_SEQSEL_2_bp CCL_SEQSEL2_bp
  #elif defined(CCL_SEQSEL_2_bp)
    //deprecated_constant_name CCL_SEQSEL2_bp = CCL_SEQSEL_2_bp;
    #define CCL_SEQSEL2_bp CCL_SEQSEL_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(CCL_INTMODE0_0_bm) && defined(CCL_INTMODE00_bm)
    #define CCL_INTMODE0_0_bm CCL_INTMODE00_bm
  #elif defined(CCL_INTMODE0_0_bm)
    //deprecated_constant_name CCL_INTMODE00_bm = CCL_INTMODE0_0_bm;
    #define CCL_INTMODE00_bm CCL_INTMODE0_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE0_0_bp) && defined(CCL_INTMODE00_bp)
    #define CCL_INTMODE0_0_bp CCL_INTMODE00_bp
  #elif defined(CCL_INTMODE0_0_bp)
    //deprecated_constant_name CCL_INTMODE00_bp = CCL_INTMODE0_0_bp;
    #define CCL_INTMODE00_bp CCL_INTMODE0_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE0_1_bm) && defined(CCL_INTMODE01_bm)
    #define CCL_INTMODE0_1_bm CCL_INTMODE01_bm
  #elif defined(CCL_INTMODE0_1_bm)
    //deprecated_constant_name CCL_INTMODE01_bm = CCL_INTMODE0_1_bm;
    #define CCL_INTMODE01_bm CCL_INTMODE0_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE0_1_bp) && defined(CCL_INTMODE01_bp)
    #define CCL_INTMODE0_1_bp CCL_INTMODE01_bp
  #elif defined(CCL_INTMODE0_1_bp)
    //deprecated_constant_name CCL_INTMODE01_bp = CCL_INTMODE0_1_bp;
    #define CCL_INTMODE01_bp CCL_INTMODE0_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE1_0_bm) && defined(CCL_INTMODE10_bm)
    #define CCL_INTMODE1_0_bm CCL_INTMODE10_bm
  #elif defined(CCL_INTMODE1_0_bm)
    //deprecated_constant_name CCL_INTMODE10_bm = CCL_INTMODE1_0_bm;
    #define CCL_INTMODE10_bm CCL_INTMODE1_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE1_0_bp) && defined(CCL_INTMODE10_bp)
    #define CCL_INTMODE1_0_bp CCL_INTMODE10_bp
  #elif defined(CCL_INTMODE1_0_bp)
    //deprecated_constant_name CCL_INTMODE10_bp = CCL_INTMODE1_0_bp;
    #define CCL_INTMODE10_bp CCL_INTMODE1_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE1_1_bm) && defined(CCL_INTMODE11_bm)
    #define CCL_INTMODE1_1_bm CCL_INTMODE11_bm
  #elif defined(CCL_INTMODE1_1_bm)
    //deprecated_constant_name CCL_INTMODE11_bm = CCL_INTMODE1_1_bm;
    #define CCL_INTMODE11_bm CCL_INTMODE1_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE1_1_bp) && defined(CCL_INTMODE11_bp)
    #define CCL_INTMODE1_1_bp CCL_INTMODE11_bp
  #elif defined(CCL_INTMODE1_1_bp)
    //deprecated_constant_name CCL_INTMODE11_bp = CCL_INTMODE1_1_bp;
    #define CCL_INTMODE11_bp CCL_INTMODE1_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE2_0_bm) && defined(CCL_INTMODE20_bm)
    #define CCL_INTMODE2_0_bm CCL_INTMODE20_bm
  #elif defined(CCL_INTMODE2_0_bm)
    //deprecated_constant_name CCL_INTMODE20_bm = CCL_INTMODE2_0_bm;
    #define CCL_INTMODE20_bm CCL_INTMODE2_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE2_0_bp) && defined(CCL_INTMODE20_bp)
    #define CCL_INTMODE2_0_bp CCL_INTMODE20_bp
  #elif defined(CCL_INTMODE2_0_bp)
    //deprecated_constant_name CCL_INTMODE20_bp = CCL_INTMODE2_0_bp;
    #define CCL_INTMODE20_bp CCL_INTMODE2_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE2_1_bm) && defined(CCL_INTMODE21_bm)
    #define CCL_INTMODE2_1_bm CCL_INTMODE21_bm
  #elif defined(CCL_INTMODE2_1_bm)
    //deprecated_constant_name CCL_INTMODE21_bm = CCL_INTMODE2_1_bm;
    #define CCL_INTMODE21_bm CCL_INTMODE2_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE2_1_bp) && defined(CCL_INTMODE21_bp)
    #define CCL_INTMODE2_1_bp CCL_INTMODE21_bp
  #elif defined(CCL_INTMODE2_1_bp)
    //deprecated_constant_name CCL_INTMODE21_bp = CCL_INTMODE2_1_bp;
    #define CCL_INTMODE21_bp CCL_INTMODE2_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE3_0_bm) && defined(CCL_INTMODE30_bm)
    #define CCL_INTMODE3_0_bm CCL_INTMODE30_bm
  #elif defined(CCL_INTMODE3_0_bm)
    //deprecated_constant_name CCL_INTMODE30_bm = CCL_INTMODE3_0_bm;
    #define CCL_INTMODE30_bm CCL_INTMODE3_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE3_0_bp) && defined(CCL_INTMODE30_bp)
    #define CCL_INTMODE3_0_bp CCL_INTMODE30_bp
  #elif defined(CCL_INTMODE3_0_bp)
    //deprecated_constant_name CCL_INTMODE30_bp = CCL_INTMODE3_0_bp;
    #define CCL_INTMODE30_bp CCL_INTMODE3_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE3_1_bm) && defined(CCL_INTMODE31_bm)
    #define CCL_INTMODE3_1_bm CCL_INTMODE31_bm
  #elif defined(CCL_INTMODE3_1_bm)
    //deprecated_constant_name CCL_INTMODE31_bm = CCL_INTMODE3_1_bm;
    #define CCL_INTMODE31_bm CCL_INTMODE3_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE3_1_bp) && defined(CCL_INTMODE31_bp)
    #define CCL_INTMODE3_1_bp CCL_INTMODE31_bp
  #elif defined(CCL_INTMODE3_1_bp)
    //deprecated_constant_name CCL_INTMODE31_bp = CCL_INTMODE3_1_bp;
    #define CCL_INTMODE31_bp CCL_INTMODE3_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE4_0_bm) && defined(CCL_INTMODE40_bm)
    #define CCL_INTMODE4_0_bm CCL_INTMODE40_bm
  #elif defined(CCL_INTMODE4_0_bm)
    //deprecated_constant_name CCL_INTMODE40_bm = CCL_INTMODE4_0_bm;
    #define CCL_INTMODE40_bm CCL_INTMODE4_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE4_0_bp) && defined(CCL_INTMODE40_bp)
    #define CCL_INTMODE4_0_bp CCL_INTMODE40_bp
  #elif defined(CCL_INTMODE4_0_bp)
    //deprecated_constant_name CCL_INTMODE40_bp = CCL_INTMODE4_0_bp;
    #define CCL_INTMODE40_bp CCL_INTMODE4_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE4_1_bm) && defined(CCL_INTMODE41_bm)
    #define CCL_INTMODE4_1_bm CCL_INTMODE41_bm
  #elif defined(CCL_INTMODE4_1_bm)
    //deprecated_constant_name CCL_INTMODE41_bm = CCL_INTMODE4_1_bm;
    #define CCL_INTMODE41_bm CCL_INTMODE4_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE4_1_bp) && defined(CCL_INTMODE41_bp)
    #define CCL_INTMODE4_1_bp CCL_INTMODE41_bp
  #elif defined(CCL_INTMODE4_1_bp)
    //deprecated_constant_name CCL_INTMODE41_bp = CCL_INTMODE4_1_bp;
    #define CCL_INTMODE41_bp CCL_INTMODE4_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE5_0_bm) && defined(CCL_INTMODE50_bm)
    #define CCL_INTMODE5_0_bm CCL_INTMODE50_bm
  #elif defined(CCL_INTMODE5_0_bm)
    //deprecated_constant_name CCL_INTMODE50_bm = CCL_INTMODE5_0_bm;
    #define CCL_INTMODE50_bm CCL_INTMODE5_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE5_0_bp) && defined(CCL_INTMODE50_bp)
    #define CCL_INTMODE5_0_bp CCL_INTMODE50_bp
  #elif defined(CCL_INTMODE5_0_bp)
    //deprecated_constant_name CCL_INTMODE50_bp = CCL_INTMODE5_0_bp;
    #define CCL_INTMODE50_bp CCL_INTMODE5_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE5_1_bm) && defined(CCL_INTMODE51_bm)
    #define CCL_INTMODE5_1_bm CCL_INTMODE51_bm
  #elif defined(CCL_INTMODE5_1_bm)
    //deprecated_constant_name CCL_INTMODE51_bm = CCL_INTMODE5_1_bm;
    #define CCL_INTMODE51_bm CCL_INTMODE5_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INTMODE5_1_bp) && defined(CCL_INTMODE51_bp)
    #define CCL_INTMODE5_1_bp CCL_INTMODE51_bp
  #elif defined(CCL_INTMODE5_1_bp)
    //deprecated_constant_name CCL_INTMODE51_bp = CCL_INTMODE5_1_bp;
    #define CCL_INTMODE51_bp CCL_INTMODE5_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(CCL_INT_0_bm) && defined(CCL_INT0_bm)
    #define CCL_INT_0_bm CCL_INT0_bm
  #elif defined(CCL_INT_0_bm)
    #undef CCL_INT0_bm /* Another round of placews where they decided to put a compatibility layer the wrong way*/
    //deprecated_constant_name CCL_INT0_bm = CCL_INT_0_bm;
    #define CCL_INT0_bm CCL_INT_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INT_0_bp) && defined(CCL_INT0_bp)
    #define CCL_INT_0_bp CCL_INT0_bp
  #elif defined(CCL_INT_0_bp)
    #undef CCL_INT0_bp /* Another round of placews where they decided to put a compatibility layer the wrong way*/
    //deprecated_constant_name CCL_INT0_bp = CCL_INT_0_bp;
    #define CCL_INT0_bp CCL_INT_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INT_1_bm) && defined(CCL_INT1_bm)
    #define CCL_INT_1_bm CCL_INT1_bm
  #elif defined(CCL_INT_1_bm)
    #undef CCL_INT1_bm /* Another round of placews where they decided to put a compatibility layer the wrong way*/
    //deprecated_constant_name CCL_INT1_bm = CCL_INT_1_bm;
    #define CCL_INT1_bm CCL_INT_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INT_1_bp) && defined(CCL_INT1_bp)
    #define CCL_INT_1_bp CCL_INT1_bp
  #elif defined(CCL_INT_1_bp)
    #undef CCL_INT1_bp /* Another round of placews where they decided to put a compatibility layer the wrong way*/
    //deprecated_constant_name CCL_INT1_bp = CCL_INT_1_bp;
    #define CCL_INT1_bp CCL_INT_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INT_2_bm) && defined(CCL_INT2_bm)
    #define CCL_INT_2_bm CCL_INT2_bm
  #elif defined(CCL_INT_2_bm)
    #undef CCL_INT2_bm /* Another round of placews where they decided to put a compatibility layer the wrong way*/
    //deprecated_constant_name CCL_INT2_bm = CCL_INT_2_bm;
    #define CCL_INT2_bm CCL_INT_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INT_2_bp) && defined(CCL_INT2_bp)
    #define CCL_INT_2_bp CCL_INT2_bp
  #elif defined(CCL_INT_2_bp)
    #undef CCL_INT2_bp /* Another round of placews where they decided to put a compatibility layer the wrong way*/
    //deprecated_constant_name CCL_INT2_bp = CCL_INT_2_bp;
    #define CCL_INT2_bp CCL_INT_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INT_3_bm) && defined(CCL_INT3_bm)
    #define CCL_INT_3_bm CCL_INT3_bm
  #elif defined(CCL_INT_3_bm)
    #undef CCL_INT3_bm /* Another round of placews where they decided to put a compatibility layer the wrong way*/
    //deprecated_constant_name CCL_INT3_bm = CCL_INT_3_bm;
    #define CCL_INT3_bm CCL_INT_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INT_3_bp) && defined(CCL_INT3_bp)
    #define CCL_INT_3_bp CCL_INT3_bp
  #elif defined(CCL_INT_3_bp)
    #undef CCL_INT3_bp /* Another round of placews where they decided to put a compatibility layer the wrong way*/
    //deprecated_constant_name CCL_INT3_bp = CCL_INT_3_bp;
    #define CCL_INT3_bp CCL_INT_3_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INT_4_bm) && defined(CCL_INT4_bm)
    #define CCL_INT_4_bm CCL_INT4_bm
  #elif defined(CCL_INT_4_bm)
    #undef CCL_INT4_bm /* Another round of placews where they decided to put a compatibility layer the wrong way*/
    //deprecated_constant_name CCL_INT4_bm = CCL_INT_4_bm;
    #define CCL_INT4_bm CCL_INT_4_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INT_4_bp) && defined(CCL_INT4_bp)
    #define CCL_INT_4_bp CCL_INT4_bp
  #elif defined(CCL_INT_4_bp)
    #undef CCL_INT4_bp /* Another round of placews where they decided to put a compatibility layer the wrong way*/
    //deprecated_constant_name CCL_INT4_bp = CCL_INT_4_bp;
    #define CCL_INT4_bp CCL_INT_4_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INT_5_bm) && defined(CCL_INT5_bm)
    #define CCL_INT_5_bm CCL_INT5_bm
  #elif defined(CCL_INT_5_bm)
    #undef CCL_INT5_bm /* Another round of placews where they decided to put a compatibility layer the wrong way*/
    //deprecated_constant_name CCL_INT5_bm = CCL_INT_5_bm;
    #define CCL_INT5_bm CCL_INT_5_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INT_5_bp) && defined(CCL_INT5_bp)
    #define CCL_INT_5_bp CCL_INT5_bp
  #elif defined(CCL_INT_5_bp)
    #undef CCL_INT5_bp /* Another round of placews where they decided to put a compatibility layer the wrong way*/
    //deprecated_constant_name CCL_INT5_bp = CCL_INT_5_bp;
    #define CCL_INT5_bp CCL_INT_5_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_CLKSRC_0_bm) && defined(CCL_CLKSRC0_bm)
    #define CCL_CLKSRC_0_bm CCL_CLKSRC0_bm
  #elif defined(CCL_CLKSRC_0_bm)
    //deprecated_constant_name CCL_CLKSRC0_bm = CCL_CLKSRC_0_bm;
    #define CCL_CLKSRC0_bm CCL_CLKSRC_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_CLKSRC_0_bp) && defined(CCL_CLKSRC0_bp)
    #define CCL_CLKSRC_0_bp CCL_CLKSRC0_bp
  #elif defined(CCL_CLKSRC_0_bp)
    //deprecated_constant_name CCL_CLKSRC0_bp = CCL_CLKSRC_0_bp;
    #define CCL_CLKSRC0_bp CCL_CLKSRC_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_CLKSRC_1_bm) && defined(CCL_CLKSRC1_bm)
    #define CCL_CLKSRC_1_bm CCL_CLKSRC1_bm
  #elif defined(CCL_CLKSRC_1_bm)
    //deprecated_constant_name CCL_CLKSRC1_bm = CCL_CLKSRC_1_bm;
    #define CCL_CLKSRC1_bm CCL_CLKSRC_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_CLKSRC_1_bp) && defined(CCL_CLKSRC1_bp)
    #define CCL_CLKSRC_1_bp CCL_CLKSRC1_bp
  #elif defined(CCL_CLKSRC_1_bp)
    //deprecated_constant_name CCL_CLKSRC1_bp = CCL_CLKSRC_1_bp;
    #define CCL_CLKSRC1_bp CCL_CLKSRC_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_CLKSRC_2_bm) && defined(CCL_CLKSRC2_bm)
    #define CCL_CLKSRC_2_bm CCL_CLKSRC2_bm
  #elif defined(CCL_CLKSRC_2_bm)
    //deprecated_constant_name CCL_CLKSRC2_bm = CCL_CLKSRC_2_bm;
    #define CCL_CLKSRC2_bm CCL_CLKSRC_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_CLKSRC_2_bp) && defined(CCL_CLKSRC2_bp)
    #define CCL_CLKSRC_2_bp CCL_CLKSRC2_bp
  #elif defined(CCL_CLKSRC_2_bp)
    //deprecated_constant_name CCL_CLKSRC2_bp = CCL_CLKSRC_2_bp;
    #define CCL_CLKSRC2_bp CCL_CLKSRC_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_FILTSEL_0_bm) && defined(CCL_FILTSEL0_bm)
    #define CCL_FILTSEL_0_bm CCL_FILTSEL0_bm
  #elif defined(CCL_FILTSEL_0_bm)
    //deprecated_constant_name CCL_FILTSEL0_bm = CCL_FILTSEL_0_bm;
    #define CCL_FILTSEL0_bm CCL_FILTSEL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_FILTSEL_0_bp) && defined(CCL_FILTSEL0_bp)
    #define CCL_FILTSEL_0_bp CCL_FILTSEL0_bp
  #elif defined(CCL_FILTSEL_0_bp)
    //deprecated_constant_name CCL_FILTSEL0_bp = CCL_FILTSEL_0_bp;
    #define CCL_FILTSEL0_bp CCL_FILTSEL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_FILTSEL_1_bm) && defined(CCL_FILTSEL1_bm)
    #define CCL_FILTSEL_1_bm CCL_FILTSEL1_bm
  #elif defined(CCL_FILTSEL_1_bm)
    //deprecated_constant_name CCL_FILTSEL1_bm = CCL_FILTSEL_1_bm;
    #define CCL_FILTSEL1_bm CCL_FILTSEL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_FILTSEL_1_bp) && defined(CCL_FILTSEL1_bp)
    #define CCL_FILTSEL_1_bp CCL_FILTSEL1_bp
  #elif defined(CCL_FILTSEL_1_bp)
    //deprecated_constant_name CCL_FILTSEL1_bp = CCL_FILTSEL_1_bp;
    #define CCL_FILTSEL1_bp CCL_FILTSEL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL0_0_bm) && defined(CCL_INSEL00_bm)
    #define CCL_INSEL0_0_bm CCL_INSEL00_bm
  #elif defined(CCL_INSEL0_0_bm)
    //deprecated_constant_name CCL_INSEL00_bm = CCL_INSEL0_0_bm;
    #define CCL_INSEL00_bm CCL_INSEL0_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL0_0_bp) && defined(CCL_INSEL00_bp)
    #define CCL_INSEL0_0_bp CCL_INSEL00_bp
  #elif defined(CCL_INSEL0_0_bp)
    //deprecated_constant_name CCL_INSEL00_bp = CCL_INSEL0_0_bp;
    #define CCL_INSEL00_bp CCL_INSEL0_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL0_1_bm) && defined(CCL_INSEL01_bm)
    #define CCL_INSEL0_1_bm CCL_INSEL01_bm
  #elif defined(CCL_INSEL0_1_bm)
    //deprecated_constant_name CCL_INSEL01_bm = CCL_INSEL0_1_bm;
    #define CCL_INSEL01_bm CCL_INSEL0_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL0_1_bp) && defined(CCL_INSEL01_bp)
    #define CCL_INSEL0_1_bp CCL_INSEL01_bp
  #elif defined(CCL_INSEL0_1_bp)
    //deprecated_constant_name CCL_INSEL01_bp = CCL_INSEL0_1_bp;
    #define CCL_INSEL01_bp CCL_INSEL0_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL0_2_bm) && defined(CCL_INSEL02_bm)
    #define CCL_INSEL0_2_bm CCL_INSEL02_bm
  #elif defined(CCL_INSEL0_2_bm)
    //deprecated_constant_name CCL_INSEL02_bm = CCL_INSEL0_2_bm;
    #define CCL_INSEL02_bm CCL_INSEL0_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL0_2_bp) && defined(CCL_INSEL02_bp)
    #define CCL_INSEL0_2_bp CCL_INSEL02_bp
  #elif defined(CCL_INSEL0_2_bp)
    //deprecated_constant_name CCL_INSEL02_bp = CCL_INSEL0_2_bp;
    #define CCL_INSEL02_bp CCL_INSEL0_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL0_3_bm) && defined(CCL_INSEL03_bm)
    #define CCL_INSEL0_3_bm CCL_INSEL03_bm
  #elif defined(CCL_INSEL0_3_bm)
    //deprecated_constant_name CCL_INSEL03_bm = CCL_INSEL0_3_bm;
    #define CCL_INSEL03_bm CCL_INSEL0_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL0_3_bp) && defined(CCL_INSEL03_bp)
    #define CCL_INSEL0_3_bp CCL_INSEL03_bp
  #elif defined(CCL_INSEL0_3_bp)
    //deprecated_constant_name CCL_INSEL03_bp = CCL_INSEL0_3_bp;
    #define CCL_INSEL03_bp CCL_INSEL0_3_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL1_0_bm) && defined(CCL_INSEL10_bm)
    #define CCL_INSEL1_0_bm CCL_INSEL10_bm
  #elif defined(CCL_INSEL1_0_bm)
    //deprecated_constant_name CCL_INSEL10_bm = CCL_INSEL1_0_bm;
    #define CCL_INSEL10_bm CCL_INSEL1_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL1_0_bp) && defined(CCL_INSEL10_bp)
    #define CCL_INSEL1_0_bp CCL_INSEL10_bp
  #elif defined(CCL_INSEL1_0_bp)
    //deprecated_constant_name CCL_INSEL10_bp = CCL_INSEL1_0_bp;
    #define CCL_INSEL10_bp CCL_INSEL1_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL1_1_bm) && defined(CCL_INSEL11_bm)
    #define CCL_INSEL1_1_bm CCL_INSEL11_bm
  #elif defined(CCL_INSEL1_1_bm)
    //deprecated_constant_name CCL_INSEL11_bm = CCL_INSEL1_1_bm;
    #define CCL_INSEL11_bm CCL_INSEL1_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL1_1_bp) && defined(CCL_INSEL11_bp)
    #define CCL_INSEL1_1_bp CCL_INSEL11_bp
  #elif defined(CCL_INSEL1_1_bp)
    //deprecated_constant_name CCL_INSEL11_bp = CCL_INSEL1_1_bp;
    #define CCL_INSEL11_bp CCL_INSEL1_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL1_2_bm) && defined(CCL_INSEL12_bm)
    #define CCL_INSEL1_2_bm CCL_INSEL12_bm
  #elif defined(CCL_INSEL1_2_bm)
    //deprecated_constant_name CCL_INSEL12_bm = CCL_INSEL1_2_bm;
    #define CCL_INSEL12_bm CCL_INSEL1_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL1_2_bp) && defined(CCL_INSEL12_bp)
    #define CCL_INSEL1_2_bp CCL_INSEL12_bp
  #elif defined(CCL_INSEL1_2_bp)
    //deprecated_constant_name CCL_INSEL12_bp = CCL_INSEL1_2_bp;
    #define CCL_INSEL12_bp CCL_INSEL1_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL1_3_bm) && defined(CCL_INSEL13_bm)
    #define CCL_INSEL1_3_bm CCL_INSEL13_bm
  #elif defined(CCL_INSEL1_3_bm)
    //deprecated_constant_name CCL_INSEL13_bm = CCL_INSEL1_3_bm;
    #define CCL_INSEL13_bm CCL_INSEL1_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL1_3_bp) && defined(CCL_INSEL13_bp)
    #define CCL_INSEL1_3_bp CCL_INSEL13_bp
  #elif defined(CCL_INSEL1_3_bp)
    //deprecated_constant_name CCL_INSEL13_bp = CCL_INSEL1_3_bp;
    #define CCL_INSEL13_bp CCL_INSEL1_3_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL2_0_bm) && defined(CCL_INSEL20_bm)
    #define CCL_INSEL2_0_bm CCL_INSEL20_bm
  #elif defined(CCL_INSEL2_0_bm)
    //deprecated_constant_name CCL_INSEL20_bm = CCL_INSEL2_0_bm;
    #define CCL_INSEL20_bm CCL_INSEL2_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL2_0_bp) && defined(CCL_INSEL20_bp)
    #define CCL_INSEL2_0_bp CCL_INSEL20_bp
  #elif defined(CCL_INSEL2_0_bp)
    //deprecated_constant_name CCL_INSEL20_bp = CCL_INSEL2_0_bp;
    #define CCL_INSEL20_bp CCL_INSEL2_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL2_1_bm) && defined(CCL_INSEL21_bm)
    #define CCL_INSEL2_1_bm CCL_INSEL21_bm
  #elif defined(CCL_INSEL2_1_bm)
    //deprecated_constant_name CCL_INSEL21_bm = CCL_INSEL2_1_bm;
    #define CCL_INSEL21_bm CCL_INSEL2_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL2_1_bp) && defined(CCL_INSEL21_bp)
    #define CCL_INSEL2_1_bp CCL_INSEL21_bp
  #elif defined(CCL_INSEL2_1_bp)
    //deprecated_constant_name CCL_INSEL21_bp = CCL_INSEL2_1_bp;
    #define CCL_INSEL21_bp CCL_INSEL2_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL2_2_bm) && defined(CCL_INSEL22_bm)
    #define CCL_INSEL2_2_bm CCL_INSEL22_bm
  #elif defined(CCL_INSEL2_2_bm)
    //deprecated_constant_name CCL_INSEL22_bm = CCL_INSEL2_2_bm;
    #define CCL_INSEL22_bm CCL_INSEL2_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL2_2_bp) && defined(CCL_INSEL22_bp)
    #define CCL_INSEL2_2_bp CCL_INSEL22_bp
  #elif defined(CCL_INSEL2_2_bp)
    //deprecated_constant_name CCL_INSEL22_bp = CCL_INSEL2_2_bp;
    #define CCL_INSEL22_bp CCL_INSEL2_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL2_3_bm) && defined(CCL_INSEL23_bm)
    #define CCL_INSEL2_3_bm CCL_INSEL23_bm
  #elif defined(CCL_INSEL2_3_bm)
    //deprecated_constant_name CCL_INSEL23_bm = CCL_INSEL2_3_bm;
    #define CCL_INSEL23_bm CCL_INSEL2_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CCL_INSEL2_3_bp) && defined(CCL_INSEL23_bp)
    #define CCL_INSEL2_3_bp CCL_INSEL23_bp
  #elif defined(CCL_INSEL2_3_bp)
    //deprecated_constant_name CCL_INSEL23_bp = CCL_INSEL2_3_bp;
    #define CCL_INSEL23_bp CCL_INSEL2_3_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= CLKCTRL ======= */
  #if !defined(CLKCTRL_FRQSEL_0_bm) && defined(CLKCTRL_FRQSEL0_bm)
    #define CLKCTRL_FRQSEL_0_bm CLKCTRL_FRQSEL0_bm
  #elif defined(CLKCTRL_FRQSEL_0_bm)
    //deprecated_constant_name CLKCTRL_FRQSEL0_bm = CLKCTRL_FRQSEL_0_bm;
    #define CLKCTRL_FRQSEL0_bm CLKCTRL_FRQSEL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CLKCTRL_FRQSEL_0_bp) && defined(CLKCTRL_FRQSEL0_bp)
    #define CLKCTRL_FRQSEL_0_bp CLKCTRL_FRQSEL0_bp
  #elif defined(CLKCTRL_FRQSEL_0_bp)
    //deprecated_constant_name CLKCTRL_FRQSEL0_bp = CLKCTRL_FRQSEL_0_bp;
    #define CLKCTRL_FRQSEL0_bp CLKCTRL_FRQSEL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CLKCTRL_FRQSEL_1_bm) && defined(CLKCTRL_FRQSEL1_bm)
    #define CLKCTRL_FRQSEL_1_bm CLKCTRL_FRQSEL1_bm
  #elif defined(CLKCTRL_FRQSEL_1_bm)
    //deprecated_constant_name CLKCTRL_FRQSEL1_bm = CLKCTRL_FRQSEL_1_bm;
    #define CLKCTRL_FRQSEL1_bm CLKCTRL_FRQSEL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CLKCTRL_FRQSEL_1_bp) && defined(CLKCTRL_FRQSEL1_bp)
    #define CLKCTRL_FRQSEL_1_bp CLKCTRL_FRQSEL1_bp
  #elif defined(CLKCTRL_FRQSEL_1_bp)
    //deprecated_constant_name CLKCTRL_FRQSEL1_bp = CLKCTRL_FRQSEL_1_bp;
    #define CLKCTRL_FRQSEL1_bp CLKCTRL_FRQSEL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CLKCTRL_FRQSEL_2_bm) && defined(CLKCTRL_FRQSEL2_bm)
    #define CLKCTRL_FRQSEL_2_bm CLKCTRL_FRQSEL2_bm
  #elif defined(CLKCTRL_FRQSEL_2_bm)
    //deprecated_constant_name CLKCTRL_FRQSEL2_bm = CLKCTRL_FRQSEL_2_bm;
    #define CLKCTRL_FRQSEL2_bm CLKCTRL_FRQSEL_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CLKCTRL_FRQSEL_2_bp) && defined(CLKCTRL_FRQSEL2_bp)
    #define CLKCTRL_FRQSEL_2_bp CLKCTRL_FRQSEL2_bp
  #elif defined(CLKCTRL_FRQSEL_2_bp)
    //deprecated_constant_name CLKCTRL_FRQSEL2_bp = CLKCTRL_FRQSEL_2_bp;
    #define CLKCTRL_FRQSEL2_bp CLKCTRL_FRQSEL_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CLKCTRL_FRQSEL_3_bm) && defined(CLKCTRL_FRQSEL3_bm)
    #define CLKCTRL_FRQSEL_3_bm CLKCTRL_FRQSEL3_bm
  #elif defined(CLKCTRL_FRQSEL_3_bm)
    //deprecated_constant_name CLKCTRL_FRQSEL3_bm = CLKCTRL_FRQSEL_3_bm;
    #define CLKCTRL_FRQSEL3_bm CLKCTRL_FRQSEL_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CLKCTRL_FRQSEL_3_bp) && defined(CLKCTRL_FRQSEL3_bp)
    #define CLKCTRL_FRQSEL_3_bp CLKCTRL_FRQSEL3_bp
  #elif defined(CLKCTRL_FRQSEL_3_bp)
    //deprecated_constant_name CLKCTRL_FRQSEL3_bp = CLKCTRL_FRQSEL_3_bp;
    #define CLKCTRL_FRQSEL3_bp CLKCTRL_FRQSEL_3_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CLKCTRL_MULFAC_0_bm) && defined(CLKCTRL_MULFAC0_bm)
    #define CLKCTRL_MULFAC_0_bm CLKCTRL_MULFAC0_bm
  #elif defined(CLKCTRL_MULFAC_0_bm)
    //deprecated_constant_name CLKCTRL_MULFAC0_bm = CLKCTRL_MULFAC_0_bm;
    #define CLKCTRL_MULFAC0_bm CLKCTRL_MULFAC_0_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(CLKCTRL_MULFAC_0_bp) && defined(CLKCTRL_MULFAC0_bp)
    #define CLKCTRL_MULFAC_0_bp CLKCTRL_MULFAC0_bp
  #elif defined(CLKCTRL_MULFAC_0_bp)
    //deprecated_constant_name CLKCTRL_MULFAC0_bp = CLKCTRL_MULFAC_0_bp;
    #define CLKCTRL_MULFAC0_bp CLKCTRL_MULFAC_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CLKCTRL_MULFAC_1_bm) && defined(CLKCTRL_MULFAC1_bm)
    #define CLKCTRL_MULFAC_1_bm CLKCTRL_MULFAC1_bm
  #elif defined(CLKCTRL_MULFAC_1_bm)
    //deprecated_constant_name CLKCTRL_MULFAC1_bm = CLKCTRL_MULFAC_1_bm;
    #define CLKCTRL_MULFAC1_bm CLKCTRL_MULFAC_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CLKCTRL_MULFAC_1_bp) && defined(CLKCTRL_MULFAC1_bp)
    #define CLKCTRL_MULFAC_1_bp CLKCTRL_MULFAC1_bp
  #elif defined(CLKCTRL_MULFAC_1_bp)
    //deprecated_constant_name CLKCTRL_MULFAC1_bp = CLKCTRL_MULFAC_1_bp;
    #define CLKCTRL_MULFAC1_bp CLKCTRL_MULFAC_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(CLKCTRL_CSUT_0_bm) && defined(CLKCTRL_CSUT0_bm)
    #define CLKCTRL_CSUT_0_bm CLKCTRL_CSUT0_bm
  #elif defined(CLKCTRL_CSUT_0_bm)
    //deprecated_constant_name CLKCTRL_CSUT0_bm = CLKCTRL_CSUT_0_bm;
    #define CLKCTRL_CSUT0_bm CLKCTRL_CSUT_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CLKCTRL_CSUT_0_bp) && defined(CLKCTRL_CSUT0_bp)
    #define CLKCTRL_CSUT_0_bp CLKCTRL_CSUT0_bp
  #elif defined(CLKCTRL_CSUT_0_bp)
    //deprecated_constant_name CLKCTRL_CSUT0_bp = CLKCTRL_CSUT_0_bp;
    #define CLKCTRL_CSUT0_bp CLKCTRL_CSUT_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CLKCTRL_CSUT_1_bm) && defined(CLKCTRL_CSUT1_bm)
    #define CLKCTRL_CSUT_1_bm CLKCTRL_CSUT1_bm
  #elif defined(CLKCTRL_CSUT_1_bm)
    //deprecated_constant_name CLKCTRL_CSUT1_bm = CLKCTRL_CSUT_1_bm;
    #define CLKCTRL_CSUT1_bm CLKCTRL_CSUT_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(CLKCTRL_CSUT_1_bp) && defined(CLKCTRL_CSUT1_bp)
    #define CLKCTRL_CSUT_1_bp CLKCTRL_CSUT1_bp
  #elif defined(CLKCTRL_CSUT_1_bp)
    //deprecated_constant_name CLKCTRL_CSUT1_bp = CLKCTRL_CSUT_1_bp;
    #define CLKCTRL_CSUT1_bp CLKCTRL_CSUT_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= Fuses ======= */
  #if !defined(FUSE_PERIOD_0_bm) && defined(FUSE_PERIOD0_bm)
    #define FUSE_PERIOD_0_bm FUSE_PERIOD0_bm
  #elif defined(FUSE_PERIOD_0_bm)
    //deprecated_constant_name FUSE_PERIOD0_bm = FUSE_PERIOD_0_bm;
    #define FUSE_PERIOD0_bm FUSE_PERIOD_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_PERIOD_0_bp) && defined(FUSE_PERIOD0_bp)
    #define FUSE_PERIOD_0_bp FUSE_PERIOD0_bp
  #elif defined(FUSE_PERIOD_0_bp)
    //deprecated_constant_name FUSE_PERIOD0_bp = FUSE_PERIOD_0_bp;
    #define FUSE_PERIOD0_bp FUSE_PERIOD_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_PERIOD_1_bm) && defined(FUSE_PERIOD1_bm)
    #define FUSE_PERIOD_1_bm FUSE_PERIOD1_bm
  #elif defined(FUSE_PERIOD_1_bm)
    //deprecated_constant_name FUSE_PERIOD1_bm = FUSE_PERIOD_1_bm;
    #define FUSE_PERIOD1_bm FUSE_PERIOD_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_PERIOD_1_bp) && defined(FUSE_PERIOD1_bp)
    #define FUSE_PERIOD_1_bp FUSE_PERIOD1_bp
  #elif defined(FUSE_PERIOD_1_bp)
    //deprecated_constant_name FUSE_PERIOD1_bp = FUSE_PERIOD_1_bp;
    #define FUSE_PERIOD1_bp FUSE_PERIOD_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_PERIOD_2_bm) && defined(FUSE_PERIOD2_bm)
    #define FUSE_PERIOD_2_bm FUSE_PERIOD2_bm
  #elif defined(FUSE_PERIOD_2_bm)
    //deprecated_constant_name FUSE_PERIOD2_bm = FUSE_PERIOD_2_bm;
    #define FUSE_PERIOD2_bm FUSE_PERIOD_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_PERIOD_2_bp) && defined(FUSE_PERIOD2_bp)
    #define FUSE_PERIOD_2_bp FUSE_PERIOD2_bp
  #elif defined(FUSE_PERIOD_2_bp)
    //deprecated_constant_name FUSE_PERIOD2_bp = FUSE_PERIOD_2_bp;
    #define FUSE_PERIOD2_bp FUSE_PERIOD_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_PERIOD_3_bm) && defined(FUSE_PERIOD3_bm)
    #define FUSE_PERIOD_3_bm FUSE_PERIOD3_bm
  #elif defined(FUSE_PERIOD_3_bm)
    //deprecated_constant_name FUSE_PERIOD3_bm = FUSE_PERIOD_3_bm;
    #define FUSE_PERIOD3_bm FUSE_PERIOD_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_PERIOD_3_bp) && defined(FUSE_PERIOD3_bp)
    #define FUSE_PERIOD_3_bp FUSE_PERIOD3_bp
  #elif defined(FUSE_PERIOD_3_bp)
    //deprecated_constant_name FUSE_PERIOD3_bp = FUSE_PERIOD_3_bp;
    #define FUSE_PERIOD3_bp FUSE_PERIOD_3_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(FUSE_WINDOW_0_bm) && defined(FUSE_WINDOW0_bm)
    #define FUSE_WINDOW_0_bm FUSE_WINDOW0_bm
  #elif defined(FUSE_WINDOW_0_bm)
    //deprecated_constant_name FUSE_WINDOW0_bm = FUSE_WINDOW_0_bm;
    #define FUSE_WINDOW0_bm FUSE_WINDOW_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_WINDOW_0_bp) && defined(FUSE_WINDOW0_bp)
    #define FUSE_WINDOW_0_bp FUSE_WINDOW0_bp
  #elif defined(FUSE_WINDOW_0_bp)
    //deprecated_constant_name FUSE_WINDOW0_bp = FUSE_WINDOW_0_bp;
    #define FUSE_WINDOW0_bp FUSE_WINDOW_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_WINDOW_1_bm) && defined(FUSE_WINDOW1_bm)
    #define FUSE_WINDOW_1_bm FUSE_WINDOW1_bm
  #elif defined(FUSE_WINDOW_1_bm)
    //deprecated_constant_name FUSE_WINDOW1_bm = FUSE_WINDOW_1_bm;
    #define FUSE_WINDOW1_bm FUSE_WINDOW_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_WINDOW_1_bp) && defined(FUSE_WINDOW1_bp)
    #define FUSE_WINDOW_1_bp FUSE_WINDOW1_bp
  #elif defined(FUSE_WINDOW_1_bp)
    //deprecated_constant_name FUSE_WINDOW1_bp = FUSE_WINDOW_1_bp;
    #define FUSE_WINDOW1_bp FUSE_WINDOW_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_WINDOW_2_bm) && defined(FUSE_WINDOW2_bm)
    #define FUSE_WINDOW_2_bm FUSE_WINDOW2_bm
  #elif defined(FUSE_WINDOW_2_bm)
    //deprecated_constant_name FUSE_WINDOW2_bm = FUSE_WINDOW_2_bm;
    #define FUSE_WINDOW2_bm FUSE_WINDOW_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_WINDOW_2_bp) && defined(FUSE_WINDOW2_bp)
    #define FUSE_WINDOW_2_bp FUSE_WINDOW2_bp
  #elif defined(FUSE_WINDOW_2_bp)
    //deprecated_constant_name FUSE_WINDOW2_bp = FUSE_WINDOW_2_bp;
    #define FUSE_WINDOW2_bp FUSE_WINDOW_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_WINDOW_3_bm) && defined(FUSE_WINDOW3_bm)
    #define FUSE_WINDOW_3_bm FUSE_WINDOW3_bm
  #elif defined(FUSE_WINDOW_3_bm)
    //deprecated_constant_name FUSE_WINDOW3_bm = FUSE_WINDOW_3_bm;
    #define FUSE_WINDOW3_bm FUSE_WINDOW_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_WINDOW_3_bp) && defined(FUSE_WINDOW3_bp)
    #define FUSE_WINDOW_3_bp FUSE_WINDOW3_bp
  #elif defined(FUSE_WINDOW_3_bp)
    //deprecated_constant_name FUSE_WINDOW3_bp = FUSE_WINDOW_3_bp;
    #define FUSE_WINDOW3_bp FUSE_WINDOW_3_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(FUSE_SLEEP_0_bm) && defined(FUSE_SLEEP0_bm)
    #define FUSE_SLEEP_0_bm FUSE_SLEEP0_bm
  #elif defined(FUSE_SLEEP_0_bm)
    //deprecated_constant_name FUSE_SLEEP0_bm = FUSE_SLEEP_0_bm;
    #define FUSE_SLEEP0_bm FUSE_SLEEP_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_SLEEP_0_bp) && defined(FUSE_SLEEP0_bp)
    #define FUSE_SLEEP_0_bp FUSE_SLEEP0_bp
  #elif defined(FUSE_SLEEP_0_bp)
    //deprecated_constant_name FUSE_SLEEP0_bp = FUSE_SLEEP_0_bp;
    #define FUSE_SLEEP0_bp FUSE_SLEEP_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_SLEEP_1_bm) && defined(FUSE_SLEEP1_bm)
    #define FUSE_SLEEP_1_bm FUSE_SLEEP1_bm
  #elif defined(FUSE_SLEEP_1_bm)
    //deprecated_constant_name FUSE_SLEEP1_bm = FUSE_SLEEP_1_bm;
    #define FUSE_SLEEP1_bm FUSE_SLEEP_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_SLEEP_1_bp) && defined(FUSE_SLEEP1_bp)
    #define FUSE_SLEEP_1_bp FUSE_SLEEP1_bp
  #elif defined(FUSE_SLEEP_1_bp)
    //deprecated_constant_name FUSE_SLEEP1_bp = FUSE_SLEEP_1_bp;
    #define FUSE_SLEEP1_bp FUSE_SLEEP_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(FUSE_ACTIVE_0_bm) && defined(FUSE_ACTIVE0_bm)
    #define FUSE_ACTIVE_0_bm FUSE_ACTIVE0_bm
  #elif defined(FUSE_ACTIVE_0_bm)
    //deprecated_constant_name FUSE_ACTIVE0_bm = FUSE_ACTIVE_0_bm;
    #define FUSE_ACTIVE0_bm FUSE_ACTIVE_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_ACTIVE_0_bp) && defined(FUSE_ACTIVE0_bp)
    #define FUSE_ACTIVE_0_bp FUSE_ACTIVE0_bp
  #elif defined(FUSE_ACTIVE_0_bp)
    //deprecated_constant_name FUSE_ACTIVE0_bp = FUSE_ACTIVE_0_bp;
    #define FUSE_ACTIVE0_bp FUSE_ACTIVE_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_ACTIVE_1_bm) && defined(FUSE_ACTIVE1_bm)
    #define FUSE_ACTIVE_1_bm FUSE_ACTIVE1_bm
  #elif defined(FUSE_ACTIVE_1_bm)
    //deprecated_constant_name FUSE_ACTIVE1_bm = FUSE_ACTIVE_1_bm;
    #define FUSE_ACTIVE1_bm FUSE_ACTIVE_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_ACTIVE_1_bp) && defined(FUSE_ACTIVE1_bp)
    #define FUSE_ACTIVE_1_bp FUSE_ACTIVE1_bp
  #elif defined(FUSE_ACTIVE_1_bp)
    //deprecated_constant_name FUSE_ACTIVE1_bp = FUSE_ACTIVE_1_bp;
    #define FUSE_ACTIVE1_bp FUSE_ACTIVE_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(FUSE_LVL_0_bm) && defined(FUSE_LVL0_bm)
    #define FUSE_LVL_0_bm FUSE_LVL0_bm
  #elif defined(FUSE_LVL_0_bm)
    //deprecated_constant_name FUSE_LVL0_bm = FUSE_LVL_0_bm;
    #define FUSE_LVL0_bm FUSE_LVL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_LVL_0_bp) && defined(FUSE_LVL0_bp)
    #define FUSE_LVL_0_bp FUSE_LVL0_bp
  #elif defined(FUSE_LVL_0_bp)
    //deprecated_constant_name FUSE_LVL0_bp = FUSE_LVL_0_bp;
    #define FUSE_LVL0_bp FUSE_LVL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_LVL_1_bm) && defined(FUSE_LVL1_bm)
    #define FUSE_LVL_1_bm FUSE_LVL1_bm
  #elif defined(FUSE_LVL_1_bm)
    //deprecated_constant_name FUSE_LVL1_bm = FUSE_LVL_1_bm;
    #define FUSE_LVL1_bm FUSE_LVL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_LVL_1_bp) && defined(FUSE_LVL1_bp)
    #define FUSE_LVL_1_bp FUSE_LVL1_bp
  #elif defined(FUSE_LVL_1_bp)
    //deprecated_constant_name FUSE_LVL1_bp = FUSE_LVL_1_bp;
    #define FUSE_LVL1_bp FUSE_LVL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_LVL_2_bm) && defined(FUSE_LVL2_bm)
    #define FUSE_LVL_2_bm FUSE_LVL2_bm
  #elif defined(FUSE_LVL_2_bm)
    //deprecated_constant_name FUSE_LVL2_bm = FUSE_LVL_2_bm;
    #define FUSE_LVL2_bm FUSE_LVL_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_LVL_2_bp) && defined(FUSE_LVL2_bp)
    #define FUSE_LVL_2_bp FUSE_LVL2_bp
  #elif defined(FUSE_LVL_2_bp)
    //deprecated_constant_name FUSE_LVL2_bp = FUSE_LVL_2_bp;
    #define FUSE_LVL2_bp FUSE_LVL_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(FUSE_CLKSEL_0_bm) && defined(FUSE_CLKSEL0_bm)
    #define FUSE_CLKSEL_0_bm FUSE_CLKSEL0_bm
  #elif defined(FUSE_CLKSEL_0_bm)
    //deprecated_constant_name FUSE_CLKSEL0_bm = FUSE_CLKSEL_0_bm;
    #define FUSE_CLKSEL0_bm FUSE_CLKSEL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_CLKSEL_0_bp) && defined(FUSE_CLKSEL0_bp)
    #define FUSE_CLKSEL_0_bp FUSE_CLKSEL0_bp
  #elif defined(FUSE_CLKSEL_0_bp)
    //deprecated_constant_name FUSE_CLKSEL0_bp = FUSE_CLKSEL_0_bp;
    #define FUSE_CLKSEL0_bp FUSE_CLKSEL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_CLKSEL_1_bm) && defined(FUSE_CLKSEL1_bm)
    #define FUSE_CLKSEL_1_bm FUSE_CLKSEL1_bm
  #elif defined(FUSE_CLKSEL_1_bm)
    //deprecated_constant_name FUSE_CLKSEL1_bm = FUSE_CLKSEL_1_bm;
    #define FUSE_CLKSEL1_bm FUSE_CLKSEL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_CLKSEL_1_bp) && defined(FUSE_CLKSEL1_bp)
    #define FUSE_CLKSEL_1_bp FUSE_CLKSEL1_bp
  #elif defined(FUSE_CLKSEL_1_bp)
    //deprecated_constant_name FUSE_CLKSEL1_bp = FUSE_CLKSEL_1_bp;
    #define FUSE_CLKSEL1_bp FUSE_CLKSEL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_CLKSEL_2_bm) && defined(FUSE_CLKSEL2_bm)
    #define FUSE_CLKSEL_2_bm FUSE_CLKSEL2_bm
  #elif defined(FUSE_CLKSEL_2_bm)
    //deprecated_constant_name FUSE_CLKSEL2_bm = FUSE_CLKSEL_2_bm;
    #define FUSE_CLKSEL2_bm FUSE_CLKSEL_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_CLKSEL_2_bp) && defined(FUSE_CLKSEL2_bp)
    #define FUSE_CLKSEL_2_bp FUSE_CLKSEL2_bp
  #elif defined(FUSE_CLKSEL_2_bp)
    //deprecated_constant_name FUSE_CLKSEL2_bp = FUSE_CLKSEL_2_bp;
    #define FUSE_CLKSEL2_bp FUSE_CLKSEL_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(FUSE_RSTPINCFG_0_bm) && defined(FUSE_RSTPINCFG0_bm)
    #define FUSE_RSTPINCFG_0_bm FUSE_RSTPINCFG0_bm
  #elif defined(FUSE_RSTPINCFG_0_bm)
    //deprecated_constant_name FUSE_RSTPINCFG0_bm = FUSE_RSTPINCFG_0_bm;
    #define FUSE_RSTPINCFG0_bm FUSE_RSTPINCFG_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_RSTPINCFG_0_bp) && defined(FUSE_RSTPINCFG0_bp)
    #define FUSE_RSTPINCFG_0_bp FUSE_RSTPINCFG0_bp
  #elif defined(FUSE_RSTPINCFG_0_bp)
    //deprecated_constant_name FUSE_RSTPINCFG0_bp = FUSE_RSTPINCFG_0_bp;
    #define FUSE_RSTPINCFG0_bp FUSE_RSTPINCFG_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_RSTPINCFG_1_bm) && defined(FUSE_RSTPINCFG1_bm)
    #define FUSE_RSTPINCFG_1_bm FUSE_RSTPINCFG1_bm
  #elif defined(FUSE_RSTPINCFG_1_bm)
    //deprecated_constant_name FUSE_RSTPINCFG1_bm = FUSE_RSTPINCFG_1_bm;
    #define FUSE_RSTPINCFG1_bm FUSE_RSTPINCFG_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_RSTPINCFG_1_bp) && defined(FUSE_RSTPINCFG1_bp)
    #define FUSE_RSTPINCFG_1_bp FUSE_RSTPINCFG1_bp
  #elif defined(FUSE_RSTPINCFG_1_bp)
    //deprecated_constant_name FUSE_RSTPINCFG1_bp = FUSE_RSTPINCFG_1_bp;
    #define FUSE_RSTPINCFG1_bp FUSE_RSTPINCFG_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(FUSE_CRCSRC_0_bm) && defined(FUSE_CRCSRC0_bm)
    #define FUSE_CRCSRC_0_bm FUSE_CRCSRC0_bm
  #elif defined(FUSE_CRCSRC_0_bm)
    //deprecated_constant_name FUSE_CRCSRC0_bm = FUSE_CRCSRC_0_bm;
    #define FUSE_CRCSRC0_bm FUSE_CRCSRC_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_CRCSRC_0_bp) && defined(FUSE_CRCSRC0_bp)
    #define FUSE_CRCSRC_0_bp FUSE_CRCSRC0_bp
  #elif defined(FUSE_CRCSRC_0_bp)
    //deprecated_constant_name FUSE_CRCSRC0_bp = FUSE_CRCSRC_0_bp;
    #define FUSE_CRCSRC0_bp FUSE_CRCSRC_0_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(FUSE_CRCSRC_1_bm) && defined(FUSE_CRCSRC1_bm)
    #define FUSE_CRCSRC_1_bm FUSE_CRCSRC1_bm
  #elif defined(FUSE_CRCSRC_1_bm)
    //deprecated_constant_name FUSE_CRCSRC1_bm = FUSE_CRCSRC_1_bm;
    #define FUSE_CRCSRC1_bm FUSE_CRCSRC_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_CRCSRC_1_bp) && defined(FUSE_CRCSRC1_bp)
    #define FUSE_CRCSRC_1_bp FUSE_CRCSRC1_bp
  #elif defined(FUSE_CRCSRC_1_bp)
    //deprecated_constant_name FUSE_CRCSRC1_bp = FUSE_CRCSRC_1_bp;
    #define FUSE_CRCSRC1_bp FUSE_CRCSRC_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(FUSE_SUT_0_bm) && defined(FUSE_SUT0_bm)
    #define FUSE_SUT_0_bm FUSE_SUT0_bm
  #elif defined(FUSE_SUT_0_bm)
    //deprecated_constant_name FUSE_SUT0_bm = FUSE_SUT_0_bm;
    #define FUSE_SUT0_bm FUSE_SUT_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_SUT_0_bp) && defined(FUSE_SUT0_bp)
    #define FUSE_SUT_0_bp FUSE_SUT0_bp
  #elif defined(FUSE_SUT_0_bp)
    //deprecated_constant_name FUSE_SUT0_bp = FUSE_SUT_0_bp;
    #define FUSE_SUT0_bp FUSE_SUT_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_SUT_1_bm) && defined(FUSE_SUT1_bm)
    #define FUSE_SUT_1_bm FUSE_SUT1_bm
  #elif defined(FUSE_SUT_1_bm)
    //deprecated_constant_name FUSE_SUT1_bm = FUSE_SUT_1_bm;
    #define FUSE_SUT1_bm FUSE_SUT_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_SUT_1_bp) && defined(FUSE_SUT1_bp)
    #define FUSE_SUT_1_bp FUSE_SUT1_bp
  #elif defined(FUSE_SUT_1_bp)
    //deprecated_constant_name FUSE_SUT1_bp = FUSE_SUT_1_bp;
    #define FUSE_SUT1_bp FUSE_SUT_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_SUT_2_bm) && defined(FUSE_SUT2_bm)
    #define FUSE_SUT_2_bm FUSE_SUT2_bm
  #elif defined(FUSE_SUT_2_bm)
    //deprecated_constant_name FUSE_SUT2_bm = FUSE_SUT_2_bm;
    #define FUSE_SUT2_bm FUSE_SUT_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(FUSE_SUT_2_bp) && defined(FUSE_SUT2_bp)
    #define FUSE_SUT_2_bp FUSE_SUT2_bp
  #elif defined(FUSE_SUT_2_bp)
    //deprecated_constant_name FUSE_SUT2_bp = FUSE_SUT_2_bp;
    #define FUSE_SUT2_bp FUSE_SUT_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= NVMCTRL ======= */
  #if !defined(NVMCTRL_CMD_0_bm) && defined(NVMCTRL_CMD0_bm)
    #define NVMCTRL_CMD_0_bm NVMCTRL_CMD0_bm
  #elif defined(NVMCTRL_CMD_0_bm)
    //deprecated_constant_name NVMCTRL_CMD0_bm = NVMCTRL_CMD_0_bm;
    #define NVMCTRL_CMD0_bm NVMCTRL_CMD_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_CMD_0_bp) && defined(NVMCTRL_CMD0_bp)
    #define NVMCTRL_CMD_0_bp NVMCTRL_CMD0_bp
  #elif defined(NVMCTRL_CMD_0_bp)
    //deprecated_constant_name NVMCTRL_CMD0_bp = NVMCTRL_CMD_0_bp;
    #define NVMCTRL_CMD0_bp NVMCTRL_CMD_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_CMD_1_bm) && defined(NVMCTRL_CMD1_bm)
    #define NVMCTRL_CMD_1_bm NVMCTRL_CMD1_bm
  #elif defined(NVMCTRL_CMD_1_bm)
    //deprecated_constant_name NVMCTRL_CMD1_bm = NVMCTRL_CMD_1_bm;
    #define NVMCTRL_CMD1_bm NVMCTRL_CMD_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_CMD_1_bp) && defined(NVMCTRL_CMD1_bp)
    #define NVMCTRL_CMD_1_bp NVMCTRL_CMD1_bp
  #elif defined(NVMCTRL_CMD_1_bp)
    //deprecated_constant_name NVMCTRL_CMD1_bp = NVMCTRL_CMD_1_bp;
    #define NVMCTRL_CMD1_bp NVMCTRL_CMD_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_CMD_2_bm) && defined(NVMCTRL_CMD2_bm)
    #define NVMCTRL_CMD_2_bm NVMCTRL_CMD2_bm
  #elif defined(NVMCTRL_CMD_2_bm)
    //deprecated_constant_name NVMCTRL_CMD2_bm = NVMCTRL_CMD_2_bm;
    #define NVMCTRL_CMD2_bm NVMCTRL_CMD_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_CMD_2_bp) && defined(NVMCTRL_CMD2_bp)
    #define NVMCTRL_CMD_2_bp NVMCTRL_CMD2_bp
  #elif defined(NVMCTRL_CMD_2_bp)
    //deprecated_constant_name NVMCTRL_CMD2_bp = NVMCTRL_CMD_2_bp;
    #define NVMCTRL_CMD2_bp NVMCTRL_CMD_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_CMD_3_bm) && defined(NVMCTRL_CMD3_bm)
    #define NVMCTRL_CMD_3_bm NVMCTRL_CMD3_bm
  #elif defined(NVMCTRL_CMD_3_bm)
    //deprecated_constant_name NVMCTRL_CMD3_bm = NVMCTRL_CMD_3_bm;
    #define NVMCTRL_CMD3_bm NVMCTRL_CMD_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_CMD_3_bp) && defined(NVMCTRL_CMD3_bp)
    #define NVMCTRL_CMD_3_bp NVMCTRL_CMD3_bp
  #elif defined(NVMCTRL_CMD_3_bp)
    //deprecated_constant_name NVMCTRL_CMD3_bp = NVMCTRL_CMD_3_bp;
    #define NVMCTRL_CMD3_bp NVMCTRL_CMD_3_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_CMD_4_bm) && defined(NVMCTRL_CMD4_bm)
    #define NVMCTRL_CMD_4_bm NVMCTRL_CMD4_bm
  #elif defined(NVMCTRL_CMD_4_bm)
    //deprecated_constant_name NVMCTRL_CMD4_bm = NVMCTRL_CMD_4_bm;
    #define NVMCTRL_CMD4_bm NVMCTRL_CMD_4_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_CMD_4_bp) && defined(NVMCTRL_CMD4_bp)
    #define NVMCTRL_CMD_4_bp NVMCTRL_CMD4_bp
  #elif defined(NVMCTRL_CMD_4_bp)
    //deprecated_constant_name NVMCTRL_CMD4_bp = NVMCTRL_CMD_4_bp;
    #define NVMCTRL_CMD4_bp NVMCTRL_CMD_4_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_CMD_5_bm) && defined(NVMCTRL_CMD5_bm)
    #define NVMCTRL_CMD_5_bm NVMCTRL_CMD5_bm
  #elif defined(NVMCTRL_CMD_5_bm)
    //deprecated_constant_name NVMCTRL_CMD5_bm = NVMCTRL_CMD_5_bm;
    #define NVMCTRL_CMD5_bm NVMCTRL_CMD_5_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_CMD_5_bp) && defined(NVMCTRL_CMD5_bp)
    #define NVMCTRL_CMD_5_bp NVMCTRL_CMD5_bp
  #elif defined(NVMCTRL_CMD_5_bp)
    //deprecated_constant_name NVMCTRL_CMD5_bp = NVMCTRL_CMD_5_bp;
    #define NVMCTRL_CMD5_bp NVMCTRL_CMD_5_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_CMD_6_bm) && defined(NVMCTRL_CMD6_bm)
    #define NVMCTRL_CMD_6_bm NVMCTRL_CMD6_bm
  #elif defined(NVMCTRL_CMD_6_bm)
    //deprecated_constant_name NVMCTRL_CMD6_bm = NVMCTRL_CMD_6_bm;
    #define NVMCTRL_CMD6_bm NVMCTRL_CMD_6_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_CMD_6_bp) && defined(NVMCTRL_CMD6_bp)
    #define NVMCTRL_CMD_6_bp NVMCTRL_CMD6_bp
  #elif defined(NVMCTRL_CMD_6_bp)
    //deprecated_constant_name NVMCTRL_CMD6_bp = NVMCTRL_CMD_6_bp;
    #define NVMCTRL_CMD6_bp NVMCTRL_CMD_6_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(NVMCTRL_FLMAP_0_bm) && defined(NVMCTRL_FLMAP0_bm)
    #define NVMCTRL_FLMAP_0_bm NVMCTRL_FLMAP0_bm
  #elif defined(NVMCTRL_FLMAP_0_bm)
    //deprecated_constant_name NVMCTRL_FLMAP0_bm = NVMCTRL_FLMAP_0_bm;
    #define NVMCTRL_FLMAP0_bm NVMCTRL_FLMAP_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_FLMAP_0_bp) && defined(NVMCTRL_FLMAP0_bp)
    #define NVMCTRL_FLMAP_0_bp NVMCTRL_FLMAP0_bp
  #elif defined(NVMCTRL_FLMAP_0_bp)
    //deprecated_constant_name NVMCTRL_FLMAP0_bp = NVMCTRL_FLMAP_0_bp;
    #define NVMCTRL_FLMAP0_bp NVMCTRL_FLMAP_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_FLMAP_1_bm) && defined(NVMCTRL_FLMAP1_bm)
    #define NVMCTRL_FLMAP_1_bm NVMCTRL_FLMAP1_bm
  #elif defined(NVMCTRL_FLMAP_1_bm)
    //deprecated_constant_name NVMCTRL_FLMAP1_bm = NVMCTRL_FLMAP_1_bm;
    #define NVMCTRL_FLMAP1_bm NVMCTRL_FLMAP_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_FLMAP_1_bp) && defined(NVMCTRL_FLMAP1_bp)
    #define NVMCTRL_FLMAP_1_bp NVMCTRL_FLMAP1_bp
  #elif defined(NVMCTRL_FLMAP_1_bp)
    //deprecated_constant_name NVMCTRL_FLMAP1_bp = NVMCTRL_FLMAP_1_bp;
    #define NVMCTRL_FLMAP1_bp NVMCTRL_FLMAP_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(NVMCTRL_ERROR_0_bm) && defined(NVMCTRL_ERROR0_bm)
    #define NVMCTRL_ERROR_0_bm NVMCTRL_ERROR0_bm
  #elif defined(NVMCTRL_ERROR_0_bm)
    //deprecated_constant_name NVMCTRL_ERROR0_bm = NVMCTRL_ERROR_0_bm;
    #define NVMCTRL_ERROR0_bm NVMCTRL_ERROR_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_ERROR_0_bp) && defined(NVMCTRL_ERROR0_bp)
    #define NVMCTRL_ERROR_0_bp NVMCTRL_ERROR0_bp
  #elif defined(NVMCTRL_ERROR_0_bp)
    //deprecated_constant_name NVMCTRL_ERROR0_bp = NVMCTRL_ERROR_0_bp;
    #define NVMCTRL_ERROR0_bp NVMCTRL_ERROR_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_ERROR_1_bm) && defined(NVMCTRL_ERROR1_bm)
    #define NVMCTRL_ERROR_1_bm NVMCTRL_ERROR1_bm
  #elif defined(NVMCTRL_ERROR_1_bm)
    //deprecated_constant_name NVMCTRL_ERROR1_bm = NVMCTRL_ERROR_1_bm;
    #define NVMCTRL_ERROR1_bm NVMCTRL_ERROR_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_ERROR_1_bp) && defined(NVMCTRL_ERROR1_bp)
    #define NVMCTRL_ERROR_1_bp NVMCTRL_ERROR1_bp
  #elif defined(NVMCTRL_ERROR_1_bp)
    //deprecated_constant_name NVMCTRL_ERROR1_bp = NVMCTRL_ERROR_1_bp;
    #define NVMCTRL_ERROR1_bp NVMCTRL_ERROR_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_ERROR_2_bm) && defined(NVMCTRL_ERROR2_bm)
    #define NVMCTRL_ERROR_2_bm NVMCTRL_ERROR2_bm
  #elif defined(NVMCTRL_ERROR_2_bm)
    //deprecated_constant_name NVMCTRL_ERROR2_bm = NVMCTRL_ERROR_2_bm;
    #define NVMCTRL_ERROR2_bm NVMCTRL_ERROR_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(NVMCTRL_ERROR_2_bp) && defined(NVMCTRL_ERROR2_bp)
    #define NVMCTRL_ERROR_2_bp NVMCTRL_ERROR2_bp
  #elif defined(NVMCTRL_ERROR_2_bp)
    //deprecated_constant_name NVMCTRL_ERROR2_bp = NVMCTRL_ERROR_2_bp;
    #define NVMCTRL_ERROR2_bp NVMCTRL_ERROR_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= PORT ======= */
  #if !defined(PORT_INT_0_bm) && defined(PORT_INT0_bm)
    #define PORT_INT_0_bm PORT_INT0_bm
  #elif defined(PORT_INT_0_bm)
    //deprecated_constant_name PORT_INT0_bm = PORT_INT_0_bm;
    #define PORT_INT0_bm PORT_INT_0_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_0_bp) && defined(PORT_INT0_bp)
    #define PORT_INT_0_bp PORT_INT0_bp
  #elif defined(PORT_INT_0_bp)
    //deprecated_constant_name PORT_INT0_bp = PORT_INT_0_bp;
    #define PORT_INT0_bp PORT_INT_0_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_1_bm) && defined(PORT_INT1_bm)
    #define PORT_INT_1_bm PORT_INT1_bm
  #elif defined(PORT_INT_1_bm)
    //deprecated_constant_name PORT_INT1_bm = PORT_INT_1_bm;
    #define PORT_INT1_bm PORT_INT_1_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_1_bp) && defined(PORT_INT1_bp)
    #define PORT_INT_1_bp PORT_INT1_bp
  #elif defined(PORT_INT_1_bp)
    //deprecated_constant_name PORT_INT1_bp = PORT_INT_1_bp;
    #define PORT_INT1_bp PORT_INT_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_2_bm) && defined(PORT_INT2_bm)
    #define PORT_INT_2_bm PORT_INT2_bm
  #elif defined(PORT_INT_2_bm)
    //deprecated_constant_name PORT_INT2_bm = PORT_INT_2_bm;
    #define PORT_INT2_bm PORT_INT_2_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_2_bp) && defined(PORT_INT2_bp)
    #define PORT_INT_2_bp PORT_INT2_bp
  #elif defined(PORT_INT_2_bp)
    //deprecated_constant_name PORT_INT2_bp = PORT_INT_2_bp;
    #define PORT_INT2_bp PORT_INT_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_3_bm) && defined(PORT_INT3_bm)
    #define PORT_INT_3_bm PORT_INT3_bm
  #elif defined(PORT_INT_3_bm)
    //deprecated_constant_name PORT_INT3_bm = PORT_INT_3_bm;
    #define PORT_INT3_bm PORT_INT_3_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_3_bp) && defined(PORT_INT3_bp)
    #define PORT_INT_3_bp PORT_INT3_bp
  #elif defined(PORT_INT_3_bp)
    //deprecated_constant_name PORT_INT3_bp = PORT_INT_3_bp;
    #define PORT_INT3_bp PORT_INT_3_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_4_bm) && defined(PORT_INT4_bm)
    #define PORT_INT_4_bm PORT_INT4_bm
  #elif defined(PORT_INT_4_bm)
    //deprecated_constant_name PORT_INT4_bm = PORT_INT_4_bm;
    #define PORT_INT4_bm PORT_INT_4_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_4_bp) && defined(PORT_INT4_bp)
    #define PORT_INT_4_bp PORT_INT4_bp
  #elif defined(PORT_INT_4_bp)
    //deprecated_constant_name PORT_INT4_bp = PORT_INT_4_bp;
    #define PORT_INT4_bp PORT_INT_4_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_5_bm) && defined(PORT_INT5_bm)
    #define PORT_INT_5_bm PORT_INT5_bm
  #elif defined(PORT_INT_5_bm)
    //deprecated_constant_name PORT_INT5_bm = PORT_INT_5_bm;
    #define PORT_INT5_bm PORT_INT_5_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_5_bp) && defined(PORT_INT5_bp)
    #define PORT_INT_5_bp PORT_INT5_bp
  #elif defined(PORT_INT_5_bp)
    //deprecated_constant_name PORT_INT5_bp = PORT_INT_5_bp;
    #define PORT_INT5_bp PORT_INT_5_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_6_bm) && defined(PORT_INT6_bm)
    #define PORT_INT_6_bm PORT_INT6_bm
  #elif defined(PORT_INT_6_bm)
    //deprecated_constant_name PORT_INT6_bm = PORT_INT_6_bm;
    #define PORT_INT6_bm PORT_INT_6_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_6_bp) && defined(PORT_INT6_bp)
    #define PORT_INT_6_bp PORT_INT6_bp
  #elif defined(PORT_INT_6_bp)
    //deprecated_constant_name PORT_INT6_bp = PORT_INT_6_bp;
    #define PORT_INT6_bp PORT_INT_6_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_7_bm) && defined(PORT_INT7_bm)
    #define PORT_INT_7_bm PORT_INT7_bm
  #elif defined(PORT_INT_7_bm)
    //deprecated_constant_name PORT_INT7_bm = PORT_INT_7_bm;
    #define PORT_INT7_bm PORT_INT_7_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_INT_7_bp) && defined(PORT_INT7_bp)
    #define PORT_INT_7_bp PORT_INT7_bp
  #elif defined(PORT_INT_7_bp)
    //deprecated_constant_name PORT_INT7_bp = PORT_INT_7_bp;
    #define PORT_INT7_bp PORT_INT_7_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_ISC_0_bm) && defined(PORT_ISC0_bm)
    #define PORT_ISC_0_bm PORT_ISC0_bm
  #elif defined(PORT_ISC_0_bm)
    //deprecated_constant_name PORT_ISC0_bm = PORT_ISC_0_bm;
    #define PORT_ISC0_bm PORT_ISC_0_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_ISC_0_bp) && defined(PORT_ISC0_bp)
    #define PORT_ISC_0_bp PORT_ISC0_bp
  #elif defined(PORT_ISC_0_bp)
    //deprecated_constant_name PORT_ISC0_bp = PORT_ISC_0_bp;
    #define PORT_ISC0_bp PORT_ISC_0_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_ISC_1_bm) && defined(PORT_ISC1_bm)
    #define PORT_ISC_1_bm PORT_ISC1_bm
  #elif defined(PORT_ISC_1_bm)
    //deprecated_constant_name PORT_ISC1_bm = PORT_ISC_1_bm;
    #define PORT_ISC1_bm PORT_ISC_1_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_ISC_1_bp) && defined(PORT_ISC1_bp)
    #define PORT_ISC_1_bp PORT_ISC1_bp
  #elif defined(PORT_ISC_1_bp)
    //deprecated_constant_name PORT_ISC1_bp = PORT_ISC_1_bp;
    #define PORT_ISC1_bp PORT_ISC_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_ISC_2_bm) && defined(PORT_ISC2_bm)
    #define PORT_ISC_2_bm PORT_ISC2_bm
  #elif defined(PORT_ISC_2_bm)
    //deprecated_constant_name PORT_ISC2_bm = PORT_ISC_2_bm;
    #define PORT_ISC2_bm PORT_ISC_2_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORT_ISC_2_bp) && defined(PORT_ISC2_bp)
    #define PORT_ISC_2_bp PORT_ISC2_bp
  #elif defined(PORT_ISC_2_bp)
    //deprecated_constant_name PORT_ISC2_bp = PORT_ISC_2_bp;
    #define PORT_ISC2_bp PORT_ISC_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= PORTMUX ======= */
  #if !defined(PORTMUX_USART0_0_bm) && defined(PORTMUX_USART00_bm)
    #define PORTMUX_USART0_0_bm PORTMUX_USART00_bm
  #elif defined(PORTMUX_USART0_0_bm)
    //deprecated_constant_name PORTMUX_USART00_bm = PORTMUX_USART0_0_bm;
    #define PORTMUX_USART00_bm PORTMUX_USART0_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART0_0_bp) && defined(PORTMUX_USART00_bp)
    #define PORTMUX_USART0_0_bp PORTMUX_USART00_bp
  #elif defined(PORTMUX_USART0_0_bp)
    //deprecated_constant_name PORTMUX_USART00_bp = PORTMUX_USART0_0_bp;
    #define PORTMUX_USART00_bp PORTMUX_USART0_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART0_1_bm) && defined(PORTMUX_USART01_bm)
    #define PORTMUX_USART0_1_bm PORTMUX_USART01_bm
  #elif defined(PORTMUX_USART0_1_bm)
    //deprecated_constant_name PORTMUX_USART01_bm = PORTMUX_USART0_1_bm;
    #define PORTMUX_USART01_bm PORTMUX_USART0_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART0_1_bp) && defined(PORTMUX_USART01_bp)
    #define PORTMUX_USART0_1_bp PORTMUX_USART01_bp
  #elif defined(PORTMUX_USART0_1_bp)
    //deprecated_constant_name PORTMUX_USART01_bp = PORTMUX_USART0_1_bp;
    #define PORTMUX_USART01_bp PORTMUX_USART0_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART1_0_bm) && defined(PORTMUX_USART10_bm)
    #define PORTMUX_USART1_0_bm PORTMUX_USART10_bm
  #elif defined(PORTMUX_USART1_0_bm)
    //deprecated_constant_name PORTMUX_USART10_bm = PORTMUX_USART1_0_bm;
    #define PORTMUX_USART10_bm PORTMUX_USART1_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART1_0_bp) && defined(PORTMUX_USART10_bp)
    #define PORTMUX_USART1_0_bp PORTMUX_USART10_bp
  #elif defined(PORTMUX_USART1_0_bp)
    //deprecated_constant_name PORTMUX_USART10_bp = PORTMUX_USART1_0_bp;
    #define PORTMUX_USART10_bp PORTMUX_USART1_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART1_1_bm) && defined(PORTMUX_USART11_bm)
    #define PORTMUX_USART1_1_bm PORTMUX_USART11_bm
  #elif defined(PORTMUX_USART1_1_bm)
    //deprecated_constant_name PORTMUX_USART11_bm = PORTMUX_USART1_1_bm;
    #define PORTMUX_USART11_bm PORTMUX_USART1_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART1_1_bp) && defined(PORTMUX_USART11_bp)
    #define PORTMUX_USART1_1_bp PORTMUX_USART11_bp
  #elif defined(PORTMUX_USART1_1_bp)
    //deprecated_constant_name PORTMUX_USART11_bp = PORTMUX_USART1_1_bp;
    #define PORTMUX_USART11_bp PORTMUX_USART1_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART2_0_bm) && defined(PORTMUX_USART20_bm)
    #define PORTMUX_USART2_0_bm PORTMUX_USART20_bm
  #elif defined(PORTMUX_USART2_0_bm)
    //deprecated_constant_name PORTMUX_USART20_bm = PORTMUX_USART2_0_bm;
    #define PORTMUX_USART20_bm PORTMUX_USART2_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART2_0_bp) && defined(PORTMUX_USART20_bp)
    #define PORTMUX_USART2_0_bp PORTMUX_USART20_bp
  #elif defined(PORTMUX_USART2_0_bp)
    //deprecated_constant_name PORTMUX_USART20_bp = PORTMUX_USART2_0_bp;
    #define PORTMUX_USART20_bp PORTMUX_USART2_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART2_1_bm) && defined(PORTMUX_USART21_bm)
    #define PORTMUX_USART2_1_bm PORTMUX_USART21_bm
  #elif defined(PORTMUX_USART2_1_bm)
    //deprecated_constant_name PORTMUX_USART21_bm = PORTMUX_USART2_1_bm;
    #define PORTMUX_USART21_bm PORTMUX_USART2_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART2_1_bp) && defined(PORTMUX_USART21_bp)
    #define PORTMUX_USART2_1_bp PORTMUX_USART21_bp
  #elif defined(PORTMUX_USART2_1_bp)
    //deprecated_constant_name PORTMUX_USART21_bp = PORTMUX_USART2_1_bp;
    #define PORTMUX_USART21_bp PORTMUX_USART2_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART3_0_bm) && defined(PORTMUX_USART30_bm)
    #define PORTMUX_USART3_0_bm PORTMUX_USART30_bm
  #elif defined(PORTMUX_USART3_0_bm)
    //deprecated_constant_name PORTMUX_USART30_bm = PORTMUX_USART3_0_bm;
    #define PORTMUX_USART30_bm PORTMUX_USART3_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART3_0_bp) && defined(PORTMUX_USART30_bp)
    #define PORTMUX_USART3_0_bp PORTMUX_USART30_bp
  #elif defined(PORTMUX_USART3_0_bp)
    //deprecated_constant_name PORTMUX_USART30_bp = PORTMUX_USART3_0_bp;
    #define PORTMUX_USART30_bp PORTMUX_USART3_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART3_1_bm) && defined(PORTMUX_USART31_bm)
    #define PORTMUX_USART3_1_bm PORTMUX_USART31_bm
  #elif defined(PORTMUX_USART3_1_bm)
    //deprecated_constant_name PORTMUX_USART31_bm = PORTMUX_USART3_1_bm;
    #define PORTMUX_USART31_bm PORTMUX_USART3_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART3_1_bp) && defined(PORTMUX_USART31_bp)
    #define PORTMUX_USART3_1_bp PORTMUX_USART31_bp
  #elif defined(PORTMUX_USART3_1_bp)
    //deprecated_constant_name PORTMUX_USART31_bp = PORTMUX_USART3_1_bp;
    #define PORTMUX_USART31_bp PORTMUX_USART3_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART4_0_bm) && defined(PORTMUX_USART40_bm)
    #define PORTMUX_USART4_0_bm PORTMUX_USART40_bm
  #elif defined(PORTMUX_USART4_0_bm)
    //deprecated_constant_name PORTMUX_USART40_bm = PORTMUX_USART4_0_bm;
    #define PORTMUX_USART40_bm PORTMUX_USART4_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART4_0_bp) && defined(PORTMUX_USART40_bp)
    #define PORTMUX_USART4_0_bp PORTMUX_USART40_bp
  #elif defined(PORTMUX_USART4_0_bp)
    //deprecated_constant_name PORTMUX_USART40_bp = PORTMUX_USART4_0_bp;
    #define PORTMUX_USART40_bp PORTMUX_USART4_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART4_1_bm) && defined(PORTMUX_USART41_bm)
    #define PORTMUX_USART4_1_bm PORTMUX_USART41_bm
  #elif defined(PORTMUX_USART4_1_bm)
    //deprecated_constant_name PORTMUX_USART41_bm = PORTMUX_USART4_1_bm;
    #define PORTMUX_USART41_bm PORTMUX_USART4_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART4_1_bp) && defined(PORTMUX_USART41_bp)
    #define PORTMUX_USART4_1_bp PORTMUX_USART41_bp
  #elif defined(PORTMUX_USART4_1_bp)
    //deprecated_constant_name PORTMUX_USART41_bp = PORTMUX_USART4_1_bp;
    #define PORTMUX_USART41_bp PORTMUX_USART4_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART5_0_bm) && defined(PORTMUX_USART50_bm)
    #define PORTMUX_USART5_0_bm PORTMUX_USART50_bm
  #elif defined(PORTMUX_USART5_0_bm)
    //deprecated_constant_name PORTMUX_USART50_bm = PORTMUX_USART5_0_bm;
    #define PORTMUX_USART50_bm PORTMUX_USART5_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART5_0_bp) && defined(PORTMUX_USART50_bp)
    #define PORTMUX_USART5_0_bp PORTMUX_USART50_bp
  #elif defined(PORTMUX_USART5_0_bp)
    //deprecated_constant_name PORTMUX_USART50_bp = PORTMUX_USART5_0_bp;
    #define PORTMUX_USART50_bp PORTMUX_USART5_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART5_1_bm) && defined(PORTMUX_USART51_bm)
    #define PORTMUX_USART5_1_bm PORTMUX_USART51_bm
  #elif defined(PORTMUX_USART5_1_bm)
    //deprecated_constant_name PORTMUX_USART51_bm = PORTMUX_USART5_1_bm;
    #define PORTMUX_USART51_bm PORTMUX_USART5_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_USART5_1_bp) && defined(PORTMUX_USART51_bp)
    #define PORTMUX_USART5_1_bp PORTMUX_USART51_bp
  #elif defined(PORTMUX_USART5_1_bp)
    //deprecated_constant_name PORTMUX_USART51_bp = PORTMUX_USART5_1_bp;
    #define PORTMUX_USART51_bp PORTMUX_USART5_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORTMUX_SPI0_0_bm) && defined(PORTMUX_SPI00_bm)
    #define PORTMUX_SPI0_0_bm PORTMUX_SPI00_bm
  #elif defined(PORTMUX_SPI0_0_bm)
    //deprecated_constant_name PORTMUX_SPI00_bm = PORTMUX_SPI0_0_bm;
    #define PORTMUX_SPI00_bm PORTMUX_SPI0_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_SPI0_0_bp) && defined(PORTMUX_SPI00_bp)
    #define PORTMUX_SPI0_0_bp PORTMUX_SPI00_bp
  #elif defined(PORTMUX_SPI0_0_bp)
    //deprecated_constant_name PORTMUX_SPI00_bp = PORTMUX_SPI0_0_bp;
    #define PORTMUX_SPI00_bp PORTMUX_SPI0_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_SPI0_1_bm) && defined(PORTMUX_SPI01_bm)
    #define PORTMUX_SPI0_1_bm PORTMUX_SPI01_bm
  #elif defined(PORTMUX_SPI0_1_bm)
    //deprecated_constant_name PORTMUX_SPI01_bm = PORTMUX_SPI0_1_bm;
    #define PORTMUX_SPI01_bm PORTMUX_SPI0_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_SPI0_1_bp) && defined(PORTMUX_SPI01_bp)
    #define PORTMUX_SPI0_1_bp PORTMUX_SPI01_bp
  #elif defined(PORTMUX_SPI0_1_bp)
    //deprecated_constant_name PORTMUX_SPI01_bp = PORTMUX_SPI0_1_bp;
    #define PORTMUX_SPI01_bp PORTMUX_SPI0_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_SPI1_0_bm) && defined(PORTMUX_SPI10_bm)
    #define PORTMUX_SPI1_0_bm PORTMUX_SPI10_bm
  #elif defined(PORTMUX_SPI1_0_bm)
    //deprecated_constant_name PORTMUX_SPI10_bm = PORTMUX_SPI1_0_bm;
    #define PORTMUX_SPI10_bm PORTMUX_SPI1_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_SPI1_0_bp) && defined(PORTMUX_SPI10_bp)
    #define PORTMUX_SPI1_0_bp PORTMUX_SPI10_bp
  #elif defined(PORTMUX_SPI1_0_bp)
    //deprecated_constant_name PORTMUX_SPI10_bp = PORTMUX_SPI1_0_bp;
    #define PORTMUX_SPI10_bp PORTMUX_SPI1_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_SPI1_1_bm) && defined(PORTMUX_SPI11_bm)
    #define PORTMUX_SPI1_1_bm PORTMUX_SPI11_bm
  #elif defined(PORTMUX_SPI1_1_bm)
    //deprecated_constant_name PORTMUX_SPI11_bm = PORTMUX_SPI1_1_bm;
    #define PORTMUX_SPI11_bm PORTMUX_SPI1_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_SPI1_1_bp) && defined(PORTMUX_SPI11_bp)
    #define PORTMUX_SPI1_1_bp PORTMUX_SPI11_bp
  #elif defined(PORTMUX_SPI1_1_bp)
    //deprecated_constant_name PORTMUX_SPI11_bp = PORTMUX_SPI1_1_bp;
    #define PORTMUX_SPI11_bp PORTMUX_SPI1_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORTMUX_TWI0_0_bm) && defined(PORTMUX_TWI00_bm)
    #define PORTMUX_TWI0_0_bm PORTMUX_TWI00_bm
  #elif defined(PORTMUX_TWI0_0_bm)
    //deprecated_constant_name PORTMUX_TWI00_bm = PORTMUX_TWI0_0_bm;
    #define PORTMUX_TWI00_bm PORTMUX_TWI0_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TWI0_0_bp) && defined(PORTMUX_TWI00_bp)
    #define PORTMUX_TWI0_0_bp PORTMUX_TWI00_bp
  #elif defined(PORTMUX_TWI0_0_bp)
    //deprecated_constant_name PORTMUX_TWI00_bp = PORTMUX_TWI0_0_bp;
    #define PORTMUX_TWI00_bp PORTMUX_TWI0_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TWI0_1_bm) && defined(PORTMUX_TWI01_bm)
    #define PORTMUX_TWI0_1_bm PORTMUX_TWI01_bm
  #elif defined(PORTMUX_TWI0_1_bm)
    //deprecated_constant_name PORTMUX_TWI01_bm = PORTMUX_TWI0_1_bm;
    #define PORTMUX_TWI01_bm PORTMUX_TWI0_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TWI0_1_bp) && defined(PORTMUX_TWI01_bp)
    #define PORTMUX_TWI0_1_bp PORTMUX_TWI01_bp
  #elif defined(PORTMUX_TWI0_1_bp)
    //deprecated_constant_name PORTMUX_TWI01_bp = PORTMUX_TWI0_1_bp;
    #define PORTMUX_TWI01_bp PORTMUX_TWI0_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TWI1_0_bm) && defined(PORTMUX_TWI10_bm)
    #define PORTMUX_TWI1_0_bm PORTMUX_TWI10_bm
  #elif defined(PORTMUX_TWI1_0_bm)
    //deprecated_constant_name PORTMUX_TWI10_bm = PORTMUX_TWI1_0_bm;
    #define PORTMUX_TWI10_bm PORTMUX_TWI1_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TWI1_0_bp) && defined(PORTMUX_TWI10_bp)
    #define PORTMUX_TWI1_0_bp PORTMUX_TWI10_bp
  #elif defined(PORTMUX_TWI1_0_bp)
    //deprecated_constant_name PORTMUX_TWI10_bp = PORTMUX_TWI1_0_bp;
    #define PORTMUX_TWI10_bp PORTMUX_TWI1_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TWI1_1_bm) && defined(PORTMUX_TWI11_bm)
    #define PORTMUX_TWI1_1_bm PORTMUX_TWI11_bm
  #elif defined(PORTMUX_TWI1_1_bm)
    //deprecated_constant_name PORTMUX_TWI11_bm = PORTMUX_TWI1_1_bm;
    #define PORTMUX_TWI11_bm PORTMUX_TWI1_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TWI1_1_bp) && defined(PORTMUX_TWI11_bp)
    #define PORTMUX_TWI1_1_bp PORTMUX_TWI11_bp
  #elif defined(PORTMUX_TWI1_1_bp)
    //deprecated_constant_name PORTMUX_TWI11_bp = PORTMUX_TWI1_1_bp;
    #define PORTMUX_TWI11_bp PORTMUX_TWI1_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORTMUX_TCA0_0_bm) && defined(PORTMUX_TCA00_bm)
    #define PORTMUX_TCA0_0_bm PORTMUX_TCA00_bm
  #elif defined(PORTMUX_TCA0_0_bm)
    //deprecated_constant_name PORTMUX_TCA00_bm = PORTMUX_TCA0_0_bm;
    #define PORTMUX_TCA00_bm PORTMUX_TCA0_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCA0_0_bp) && defined(PORTMUX_TCA00_bp)
    #define PORTMUX_TCA0_0_bp PORTMUX_TCA00_bp
  #elif defined(PORTMUX_TCA0_0_bp)
    //deprecated_constant_name PORTMUX_TCA00_bp = PORTMUX_TCA0_0_bp;
    #define PORTMUX_TCA00_bp PORTMUX_TCA0_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCA0_1_bm) && defined(PORTMUX_TCA01_bm)
    #define PORTMUX_TCA0_1_bm PORTMUX_TCA01_bm
  #elif defined(PORTMUX_TCA0_1_bm)
    //deprecated_constant_name PORTMUX_TCA01_bm = PORTMUX_TCA0_1_bm;
    #define PORTMUX_TCA01_bm PORTMUX_TCA0_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCA0_1_bp) && defined(PORTMUX_TCA01_bp)
    #define PORTMUX_TCA0_1_bp PORTMUX_TCA01_bp
  #elif defined(PORTMUX_TCA0_1_bp)
    //deprecated_constant_name PORTMUX_TCA01_bp = PORTMUX_TCA0_1_bp;
    #define PORTMUX_TCA01_bp PORTMUX_TCA0_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCA0_2_bm) && defined(PORTMUX_TCA02_bm)
    #define PORTMUX_TCA0_2_bm PORTMUX_TCA02_bm
  #elif defined(PORTMUX_TCA0_2_bm)
    //deprecated_constant_name PORTMUX_TCA02_bm = PORTMUX_TCA0_2_bm;
    #define PORTMUX_TCA02_bm PORTMUX_TCA0_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCA0_2_bp) && defined(PORTMUX_TCA02_bp)
    #define PORTMUX_TCA0_2_bp PORTMUX_TCA02_bp
  #elif defined(PORTMUX_TCA0_2_bp)
    //deprecated_constant_name PORTMUX_TCA02_bp = PORTMUX_TCA0_2_bp;
    #define PORTMUX_TCA02_bp PORTMUX_TCA0_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCA1_0_bm) && defined(PORTMUX_TCA10_bm)
    #define PORTMUX_TCA1_0_bm PORTMUX_TCA10_bm
  #elif defined(PORTMUX_TCA1_0_bm)
    //deprecated_constant_name PORTMUX_TCA10_bm = PORTMUX_TCA1_0_bm;
    #define PORTMUX_TCA10_bm PORTMUX_TCA1_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCA1_0_bp) && defined(PORTMUX_TCA10_bp)
    #define PORTMUX_TCA1_0_bp PORTMUX_TCA10_bp
  #elif defined(PORTMUX_TCA1_0_bp)
    //deprecated_constant_name PORTMUX_TCA10_bp = PORTMUX_TCA1_0_bp;
    #define PORTMUX_TCA10_bp PORTMUX_TCA1_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCA1_1_bm) && defined(PORTMUX_TCA11_bm)
    #define PORTMUX_TCA1_1_bm PORTMUX_TCA11_bm
  #elif defined(PORTMUX_TCA1_1_bm)
    //deprecated_constant_name PORTMUX_TCA11_bm = PORTMUX_TCA1_1_bm;
    #define PORTMUX_TCA11_bm PORTMUX_TCA1_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCA1_1_bp) && defined(PORTMUX_TCA11_bp)
    #define PORTMUX_TCA1_1_bp PORTMUX_TCA11_bp
  #elif defined(PORTMUX_TCA1_1_bp)
    //deprecated_constant_name PORTMUX_TCA11_bp = PORTMUX_TCA1_1_bp;
    #define PORTMUX_TCA11_bp PORTMUX_TCA1_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCA1_2_bm) && defined(PORTMUX_TCA12_bm)
    #define PORTMUX_TCA1_2_bm PORTMUX_TCA12_bm
  #elif defined(PORTMUX_TCA1_2_bm)
    //deprecated_constant_name PORTMUX_TCA12_bm = PORTMUX_TCA1_2_bm;
    #define PORTMUX_TCA12_bm PORTMUX_TCA1_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCA1_2_bp) && defined(PORTMUX_TCA12_bp)
    #define PORTMUX_TCA1_2_bp PORTMUX_TCA12_bp
  #elif defined(PORTMUX_TCA1_2_bp)
    //deprecated_constant_name PORTMUX_TCA12_bp = PORTMUX_TCA1_2_bp;
    #define PORTMUX_TCA12_bp PORTMUX_TCA1_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(PORTMUX_TCD0_0_bm) && defined(PORTMUX_TCD00_bm)
    #define PORTMUX_TCD0_0_bm PORTMUX_TCD00_bm
  #elif defined(PORTMUX_TCD0_0_bm)
    //deprecated_constant_name PORTMUX_TCD00_bm = PORTMUX_TCD0_0_bm;
    #define PORTMUX_TCD00_bm PORTMUX_TCD0_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCD0_0_bp) && defined(PORTMUX_TCD00_bp)
    #define PORTMUX_TCD0_0_bp PORTMUX_TCD00_bp
  #elif defined(PORTMUX_TCD0_0_bp)
    //deprecated_constant_name PORTMUX_TCD00_bp = PORTMUX_TCD0_0_bp;
    #define PORTMUX_TCD00_bp PORTMUX_TCD0_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCD0_1_bm) && defined(PORTMUX_TCD01_bm)
    #define PORTMUX_TCD0_1_bm PORTMUX_TCD01_bm
  #elif defined(PORTMUX_TCD0_1_bm)
    //deprecated_constant_name PORTMUX_TCD01_bm = PORTMUX_TCD0_1_bm;
    #define PORTMUX_TCD01_bm PORTMUX_TCD0_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCD0_1_bp) && defined(PORTMUX_TCD01_bp)
    #define PORTMUX_TCD0_1_bp PORTMUX_TCD01_bp
  #elif defined(PORTMUX_TCD0_1_bp)
    //deprecated_constant_name PORTMUX_TCD01_bp = PORTMUX_TCD0_1_bp;
    #define PORTMUX_TCD01_bp PORTMUX_TCD0_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCD0_2_bm) && defined(PORTMUX_TCD02_bm)
    #define PORTMUX_TCD0_2_bm PORTMUX_TCD02_bm
  #elif defined(PORTMUX_TCD0_2_bm)
    //deprecated_constant_name PORTMUX_TCD02_bm = PORTMUX_TCD0_2_bm;
    #define PORTMUX_TCD02_bm PORTMUX_TCD0_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(PORTMUX_TCD0_2_bp) && defined(PORTMUX_TCD02_bp)
    #define PORTMUX_TCD0_2_bp PORTMUX_TCD02_bp
  #elif defined(PORTMUX_TCD0_2_bp)
    //deprecated_constant_name PORTMUX_TCD02_bp = PORTMUX_TCD0_2_bp;
    #define PORTMUX_TCD02_bp PORTMUX_TCD0_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= RTC ======= */
  #if !defined(RTC_PRESCALER_0_bm) && defined(RTC_PRESCALER0_bm)
    #define RTC_PRESCALER_0_bm RTC_PRESCALER0_bm
  #elif defined(RTC_PRESCALER_0_bm)
    //deprecated_constant_name RTC_PRESCALER0_bm = RTC_PRESCALER_0_bm;
    #define RTC_PRESCALER0_bm RTC_PRESCALER_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_PRESCALER_0_bp) && defined(RTC_PRESCALER0_bp)
    #define RTC_PRESCALER_0_bp RTC_PRESCALER0_bp
  #elif defined(RTC_PRESCALER_0_bp)
    //deprecated_constant_name RTC_PRESCALER0_bp = RTC_PRESCALER_0_bp;
    #define RTC_PRESCALER0_bp RTC_PRESCALER_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_PRESCALER_1_bm) && defined(RTC_PRESCALER1_bm)
    #define RTC_PRESCALER_1_bm RTC_PRESCALER1_bm
  #elif defined(RTC_PRESCALER_1_bm)
    //deprecated_constant_name RTC_PRESCALER1_bm = RTC_PRESCALER_1_bm;
    #define RTC_PRESCALER1_bm RTC_PRESCALER_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_PRESCALER_1_bp) && defined(RTC_PRESCALER1_bp)
    #define RTC_PRESCALER_1_bp RTC_PRESCALER1_bp
  #elif defined(RTC_PRESCALER_1_bp)
    //deprecated_constant_name RTC_PRESCALER1_bp = RTC_PRESCALER_1_bp;
    #define RTC_PRESCALER1_bp RTC_PRESCALER_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_PRESCALER_2_bm) && defined(RTC_PRESCALER2_bm)
    #define RTC_PRESCALER_2_bm RTC_PRESCALER2_bm
  #elif defined(RTC_PRESCALER_2_bm)
    //deprecated_constant_name RTC_PRESCALER2_bm = RTC_PRESCALER_2_bm;
    #define RTC_PRESCALER2_bm RTC_PRESCALER_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_PRESCALER_2_bp) && defined(RTC_PRESCALER2_bp)
    #define RTC_PRESCALER_2_bp RTC_PRESCALER2_bp
  #elif defined(RTC_PRESCALER_2_bp)
    //deprecated_constant_name RTC_PRESCALER2_bp = RTC_PRESCALER_2_bp;
    #define RTC_PRESCALER2_bp RTC_PRESCALER_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_PRESCALER_3_bm) && defined(RTC_PRESCALER3_bm)
    #define RTC_PRESCALER_3_bm RTC_PRESCALER3_bm
  #elif defined(RTC_PRESCALER_3_bm)
    //deprecated_constant_name RTC_PRESCALER3_bm = RTC_PRESCALER_3_bm;
    #define RTC_PRESCALER3_bm RTC_PRESCALER_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_PRESCALER_3_bp) && defined(RTC_PRESCALER3_bp)
    #define RTC_PRESCALER_3_bp RTC_PRESCALER3_bp
  #elif defined(RTC_PRESCALER_3_bp)
    //deprecated_constant_name RTC_PRESCALER3_bp = RTC_PRESCALER_3_bp;
    #define RTC_PRESCALER3_bp RTC_PRESCALER_3_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(RTC_ERROR_0_bm) && defined(RTC_ERROR0_bm)
    #define RTC_ERROR_0_bm RTC_ERROR0_bm
  #elif defined(RTC_ERROR_0_bm)
    //deprecated_constant_name RTC_ERROR0_bm = RTC_ERROR_0_bm;
    #define RTC_ERROR0_bm RTC_ERROR_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_ERROR_0_bp) && defined(RTC_ERROR0_bp)
    #define RTC_ERROR_0_bp RTC_ERROR0_bp
  #elif defined(RTC_ERROR_0_bp)
    //deprecated_constant_name RTC_ERROR0_bp = RTC_ERROR_0_bp;
    #define RTC_ERROR0_bp RTC_ERROR_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_ERROR_1_bm) && defined(RTC_ERROR1_bm)
    #define RTC_ERROR_1_bm RTC_ERROR1_bm
  #elif defined(RTC_ERROR_1_bm)
    //deprecated_constant_name RTC_ERROR1_bm = RTC_ERROR_1_bm;
    #define RTC_ERROR1_bm RTC_ERROR_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_ERROR_1_bp) && defined(RTC_ERROR1_bp)
    #define RTC_ERROR_1_bp RTC_ERROR1_bp
  #elif defined(RTC_ERROR_1_bp)
    //deprecated_constant_name RTC_ERROR1_bp = RTC_ERROR_1_bp;
    #define RTC_ERROR1_bp RTC_ERROR_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_ERROR_2_bm) && defined(RTC_ERROR2_bm)
    #define RTC_ERROR_2_bm RTC_ERROR2_bm
  #elif defined(RTC_ERROR_2_bm)
    //deprecated_constant_name RTC_ERROR2_bm = RTC_ERROR_2_bm;
    #define RTC_ERROR2_bm RTC_ERROR_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_ERROR_2_bp) && defined(RTC_ERROR2_bp)
    #define RTC_ERROR_2_bp RTC_ERROR2_bp
  #elif defined(RTC_ERROR_2_bp)
    //deprecated_constant_name RTC_ERROR2_bp = RTC_ERROR_2_bp;
    #define RTC_ERROR2_bp RTC_ERROR_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_ERROR_3_bm) && defined(RTC_ERROR3_bm)
    #define RTC_ERROR_3_bm RTC_ERROR3_bm
  #elif defined(RTC_ERROR_3_bm)
    //deprecated_constant_name RTC_ERROR3_bm = RTC_ERROR_3_bm;
    #define RTC_ERROR3_bm RTC_ERROR_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_ERROR_3_bp) && defined(RTC_ERROR3_bp)
    #define RTC_ERROR_3_bp RTC_ERROR3_bp
  #elif defined(RTC_ERROR_3_bp)
    //deprecated_constant_name RTC_ERROR3_bp = RTC_ERROR_3_bp;
    #define RTC_ERROR3_bp RTC_ERROR_3_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_ERROR_4_bm) && defined(RTC_ERROR4_bm)
    #define RTC_ERROR_4_bm RTC_ERROR4_bm
  #elif defined(RTC_ERROR_4_bm)
    //deprecated_constant_name RTC_ERROR4_bm = RTC_ERROR_4_bm;
    #define RTC_ERROR4_bm RTC_ERROR_4_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_ERROR_4_bp) && defined(RTC_ERROR4_bp)
    #define RTC_ERROR_4_bp RTC_ERROR4_bp
  #elif defined(RTC_ERROR_4_bp)
    //deprecated_constant_name RTC_ERROR4_bp = RTC_ERROR_4_bp;
    #define RTC_ERROR4_bp RTC_ERROR_4_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_ERROR_5_bm) && defined(RTC_ERROR5_bm)
    #define RTC_ERROR_5_bm RTC_ERROR5_bm
  #elif defined(RTC_ERROR_5_bm)
    //deprecated_constant_name RTC_ERROR5_bm = RTC_ERROR_5_bm;
    #define RTC_ERROR5_bm RTC_ERROR_5_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_ERROR_5_bp) && defined(RTC_ERROR5_bp)
    #define RTC_ERROR_5_bp RTC_ERROR5_bp
  #elif defined(RTC_ERROR_5_bp)
    //deprecated_constant_name RTC_ERROR5_bp = RTC_ERROR_5_bp;
    #define RTC_ERROR5_bp RTC_ERROR_5_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_ERROR_6_bm) && defined(RTC_ERROR6_bm)
    #define RTC_ERROR_6_bm RTC_ERROR6_bm
  #elif defined(RTC_ERROR_6_bm)
    //deprecated_constant_name RTC_ERROR6_bm = RTC_ERROR_6_bm;
    #define RTC_ERROR6_bm RTC_ERROR_6_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_ERROR_6_bp) && defined(RTC_ERROR6_bp)
    #define RTC_ERROR_6_bp RTC_ERROR6_bp
  #elif defined(RTC_ERROR_6_bp)
    //deprecated_constant_name RTC_ERROR6_bp = RTC_ERROR_6_bp;
    #define RTC_ERROR6_bp RTC_ERROR_6_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(RTC_CLKSEL_0_bm) && defined(RTC_CLKSEL0_bm)
    #define RTC_CLKSEL_0_bm RTC_CLKSEL0_bm
  #elif defined(RTC_CLKSEL_0_bm)
    //deprecated_constant_name RTC_CLKSEL0_bm = RTC_CLKSEL_0_bm;
    #define RTC_CLKSEL0_bm RTC_CLKSEL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_CLKSEL_0_bp) && defined(RTC_CLKSEL0_bp)
    #define RTC_CLKSEL_0_bp RTC_CLKSEL0_bp
  #elif defined(RTC_CLKSEL_0_bp)
    //deprecated_constant_name RTC_CLKSEL0_bp = RTC_CLKSEL_0_bp;
    #define RTC_CLKSEL0_bp RTC_CLKSEL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_CLKSEL_1_bm) && defined(RTC_CLKSEL1_bm)
    #define RTC_CLKSEL_1_bm RTC_CLKSEL1_bm
  #elif defined(RTC_CLKSEL_1_bm)
    //deprecated_constant_name RTC_CLKSEL1_bm = RTC_CLKSEL_1_bm;
    #define RTC_CLKSEL1_bm RTC_CLKSEL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_CLKSEL_1_bp) && defined(RTC_CLKSEL1_bp)
    #define RTC_CLKSEL_1_bp RTC_CLKSEL1_bp
  #elif defined(RTC_CLKSEL_1_bp)
    //deprecated_constant_name RTC_CLKSEL1_bp = RTC_CLKSEL_1_bp;
    #define RTC_CLKSEL1_bp RTC_CLKSEL_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(RTC_PERIOD_0_bm) && defined(RTC_PERIOD0_bm)
    #define RTC_PERIOD_0_bm RTC_PERIOD0_bm
  #elif defined(RTC_PERIOD_0_bm)
    //deprecated_constant_name RTC_PERIOD0_bm = RTC_PERIOD_0_bm;
    #define RTC_PERIOD0_bm RTC_PERIOD_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_PERIOD_0_bp) && defined(RTC_PERIOD0_bp)
    #define RTC_PERIOD_0_bp RTC_PERIOD0_bp
  #elif defined(RTC_PERIOD_0_bp)
    //deprecated_constant_name RTC_PERIOD0_bp = RTC_PERIOD_0_bp;
    #define RTC_PERIOD0_bp RTC_PERIOD_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_PERIOD_1_bm) && defined(RTC_PERIOD1_bm)
    #define RTC_PERIOD_1_bm RTC_PERIOD1_bm
  #elif defined(RTC_PERIOD_1_bm)
    //deprecated_constant_name RTC_PERIOD1_bm = RTC_PERIOD_1_bm;
    #define RTC_PERIOD1_bm RTC_PERIOD_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_PERIOD_1_bp) && defined(RTC_PERIOD1_bp)
    #define RTC_PERIOD_1_bp RTC_PERIOD1_bp
  #elif defined(RTC_PERIOD_1_bp)
    //deprecated_constant_name RTC_PERIOD1_bp = RTC_PERIOD_1_bp;
    #define RTC_PERIOD1_bp RTC_PERIOD_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_PERIOD_2_bm) && defined(RTC_PERIOD2_bm)
    #define RTC_PERIOD_2_bm RTC_PERIOD2_bm
  #elif defined(RTC_PERIOD_2_bm)
    //deprecated_constant_name RTC_PERIOD2_bm = RTC_PERIOD_2_bm;
    #define RTC_PERIOD2_bm RTC_PERIOD_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_PERIOD_2_bp) && defined(RTC_PERIOD2_bp)
    #define RTC_PERIOD_2_bp RTC_PERIOD2_bp
  #elif defined(RTC_PERIOD_2_bp)
    //deprecated_constant_name RTC_PERIOD2_bp = RTC_PERIOD_2_bp;
    #define RTC_PERIOD2_bp RTC_PERIOD_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_PERIOD_3_bm) && defined(RTC_PERIOD3_bm)
    #define RTC_PERIOD_3_bm RTC_PERIOD3_bm
  #elif defined(RTC_PERIOD_3_bm)
    //deprecated_constant_name RTC_PERIOD3_bm = RTC_PERIOD_3_bm;
    #define RTC_PERIOD3_bm RTC_PERIOD_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(RTC_PERIOD_3_bp) && defined(RTC_PERIOD3_bp)
    #define RTC_PERIOD_3_bp RTC_PERIOD3_bp
  #elif defined(RTC_PERIOD_3_bp)
    //deprecated_constant_name RTC_PERIOD3_bp = RTC_PERIOD_3_bp;
    #define RTC_PERIOD3_bp RTC_PERIOD_3_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= SLPCTRL ======= */
  #if !defined(SLPCTRL_SMODE_0_bm) && defined(SLPCTRL_SMODE0_bm)
    #define SLPCTRL_SMODE_0_bm SLPCTRL_SMODE0_bm
  #elif defined(SLPCTRL_SMODE_0_bm)
    //deprecated_constant_name SLPCTRL_SMODE0_bm = SLPCTRL_SMODE_0_bm;
    #define SLPCTRL_SMODE0_bm SLPCTRL_SMODE_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SLPCTRL_SMODE_0_bp) && defined(SLPCTRL_SMODE0_bp)
    #define SLPCTRL_SMODE_0_bp SLPCTRL_SMODE0_bp
  #elif defined(SLPCTRL_SMODE_0_bp)
    //deprecated_constant_name SLPCTRL_SMODE0_bp = SLPCTRL_SMODE_0_bp;
    #define SLPCTRL_SMODE0_bp SLPCTRL_SMODE_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SLPCTRL_SMODE_1_bm) && defined(SLPCTRL_SMODE1_bm)
    #define SLPCTRL_SMODE_1_bm SLPCTRL_SMODE1_bm
  #elif defined(SLPCTRL_SMODE_1_bm)
    //deprecated_constant_name SLPCTRL_SMODE1_bm = SLPCTRL_SMODE_1_bm;
    #define SLPCTRL_SMODE1_bm SLPCTRL_SMODE_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SLPCTRL_SMODE_1_bp) && defined(SLPCTRL_SMODE1_bp)
    #define SLPCTRL_SMODE_1_bp SLPCTRL_SMODE1_bp
  #elif defined(SLPCTRL_SMODE_1_bp)
    //deprecated_constant_name SLPCTRL_SMODE1_bp = SLPCTRL_SMODE_1_bp;
    #define SLPCTRL_SMODE1_bp SLPCTRL_SMODE_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SLPCTRL_PMODE_0_bm) && defined(SLPCTRL_PMODE0_bm)
    #define SLPCTRL_PMODE_0_bm SLPCTRL_PMODE0_bm
  #elif defined(SLPCTRL_PMODE_0_bm)
    //deprecated_constant_name SLPCTRL_PMODE0_bm = SLPCTRL_PMODE_0_bm;
    #define SLPCTRL_PMODE0_bm SLPCTRL_PMODE_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SLPCTRL_PMODE_0_bp) && defined(SLPCTRL_PMODE0_bp)
    #define SLPCTRL_PMODE_0_bp SLPCTRL_PMODE0_bp
  #elif defined(SLPCTRL_PMODE_0_bp)
    //deprecated_constant_name SLPCTRL_PMODE0_bp = SLPCTRL_PMODE_0_bp;
    #define SLPCTRL_PMODE0_bp SLPCTRL_PMODE_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SLPCTRL_PMODE_1_bm) && defined(SLPCTRL_PMODE1_bm)
    #define SLPCTRL_PMODE_1_bm SLPCTRL_PMODE1_bm
  #elif defined(SLPCTRL_PMODE_1_bm)
    //deprecated_constant_name SLPCTRL_PMODE1_bm = SLPCTRL_PMODE_1_bm;
    #define SLPCTRL_PMODE1_bm SLPCTRL_PMODE_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SLPCTRL_PMODE_1_bp) && defined(SLPCTRL_PMODE1_bp)
    #define SLPCTRL_PMODE_1_bp SLPCTRL_PMODE1_bp
  #elif defined(SLPCTRL_PMODE_1_bp)
    //deprecated_constant_name SLPCTRL_PMODE1_bp = SLPCTRL_PMODE_1_bp;
    #define SLPCTRL_PMODE1_bp SLPCTRL_PMODE_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SLPCTRL_PMODE_2_bm) && defined(SLPCTRL_PMODE2_bm)
    #define SLPCTRL_PMODE_2_bm SLPCTRL_PMODE2_bm
  #elif defined(SLPCTRL_PMODE_2_bm)
    //deprecated_constant_name SLPCTRL_PMODE2_bm = SLPCTRL_PMODE_2_bm;
    #define SLPCTRL_PMODE2_bm SLPCTRL_PMODE_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SLPCTRL_PMODE_2_bp) && defined(SLPCTRL_PMODE2_bp)
    #define SLPCTRL_PMODE_2_bp SLPCTRL_PMODE2_bp
  #elif defined(SLPCTRL_PMODE_2_bp)
    //deprecated_constant_name SLPCTRL_PMODE2_bp = SLPCTRL_PMODE_2_bp;
    #define SLPCTRL_PMODE2_bp SLPCTRL_PMODE_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= SPI ======= */
  #if !defined(SPI_PRESC_0_bm) && defined(SPI_PRESC0_bm)
    #define SPI_PRESC_0_bm SPI_PRESC0_bm
  #elif defined(SPI_PRESC_0_bm)
    //deprecated_constant_name SPI_PRESC0_bm = SPI_PRESC_0_bm;
    #define SPI_PRESC0_bm SPI_PRESC_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SPI_PRESC_0_bp) && defined(SPI_PRESC0_bp)
    #define SPI_PRESC_0_bp SPI_PRESC0_bp
  #elif defined(SPI_PRESC_0_bp)
    //deprecated_constant_name SPI_PRESC0_bp = SPI_PRESC_0_bp;
    #define SPI_PRESC0_bp SPI_PRESC_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SPI_PRESC_1_bm) && defined(SPI_PRESC1_bm)
    #define SPI_PRESC_1_bm SPI_PRESC1_bm
  #elif defined(SPI_PRESC_1_bm)
    //deprecated_constant_name SPI_PRESC1_bm = SPI_PRESC_1_bm;
    #define SPI_PRESC1_bm SPI_PRESC_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SPI_PRESC_1_bp) && defined(SPI_PRESC1_bp)
    #define SPI_PRESC_1_bp SPI_PRESC1_bp
  #elif defined(SPI_PRESC_1_bp)
    //deprecated_constant_name SPI_PRESC1_bp = SPI_PRESC_1_bp;
    #define SPI_PRESC1_bp SPI_PRESC_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(SPI_MODE_0_bm) && defined(SPI_MODE0_bm)
    #define SPI_MODE_0_bm SPI_MODE0_bm
  #elif defined(SPI_MODE_0_bm)
    //deprecated_constant_name SPI_MODE0_bm = SPI_MODE_0_bm;
    #define SPI_MODE0_bm SPI_MODE_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SPI_MODE_0_bp) && defined(SPI_MODE0_bp)
    #define SPI_MODE_0_bp SPI_MODE0_bp
  #elif defined(SPI_MODE_0_bp)
    //deprecated_constant_name SPI_MODE0_bp = SPI_MODE_0_bp;
    #define SPI_MODE0_bp SPI_MODE_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SPI_MODE_1_bm) && defined(SPI_MODE1_bm)
    #define SPI_MODE_1_bm SPI_MODE1_bm
  #elif defined(SPI_MODE_1_bm)
    //deprecated_constant_name SPI_MODE1_bm = SPI_MODE_1_bm;
    #define SPI_MODE1_bm SPI_MODE_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(SPI_MODE_1_bp) && defined(SPI_MODE1_bp)
    #define SPI_MODE_1_bp SPI_MODE1_bp
  #elif defined(SPI_MODE_1_bp)
    //deprecated_constant_name SPI_MODE1_bp = SPI_MODE_1_bp;
    #define SPI_MODE1_bp SPI_MODE_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= TCA ======= */
  #if !defined(TCA_SINGLE_CLKSEL_0_bm) && defined(TCA_SINGLE_CLKSEL0_bm)
    #define TCA_SINGLE_CLKSEL_0_bm TCA_SINGLE_CLKSEL0_bm
  #elif defined(TCA_SINGLE_CLKSEL_0_bm)
    //deprecated_constant_name TCA_SINGLE_CLKSEL0_bm = TCA_SINGLE_CLKSEL_0_bm;
    #define TCA_SINGLE_CLKSEL0_bm TCA_SINGLE_CLKSEL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_CLKSEL_0_bp) && defined(TCA_SINGLE_CLKSEL0_bp)
    #define TCA_SINGLE_CLKSEL_0_bp TCA_SINGLE_CLKSEL0_bp
  #elif defined(TCA_SINGLE_CLKSEL_0_bp)
    //deprecated_constant_name TCA_SINGLE_CLKSEL0_bp = TCA_SINGLE_CLKSEL_0_bp;
    #define TCA_SINGLE_CLKSEL0_bp TCA_SINGLE_CLKSEL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_CLKSEL_1_bm) && defined(TCA_SINGLE_CLKSEL1_bm)
    #define TCA_SINGLE_CLKSEL_1_bm TCA_SINGLE_CLKSEL1_bm
  #elif defined(TCA_SINGLE_CLKSEL_1_bm)
    //deprecated_constant_name TCA_SINGLE_CLKSEL1_bm = TCA_SINGLE_CLKSEL_1_bm;
    #define TCA_SINGLE_CLKSEL1_bm TCA_SINGLE_CLKSEL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_CLKSEL_1_bp) && defined(TCA_SINGLE_CLKSEL1_bp)
    #define TCA_SINGLE_CLKSEL_1_bp TCA_SINGLE_CLKSEL1_bp
  #elif defined(TCA_SINGLE_CLKSEL_1_bp)
    //deprecated_constant_name TCA_SINGLE_CLKSEL1_bp = TCA_SINGLE_CLKSEL_1_bp;
    #define TCA_SINGLE_CLKSEL1_bp TCA_SINGLE_CLKSEL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_CLKSEL_2_bm) && defined(TCA_SINGLE_CLKSEL2_bm)
    #define TCA_SINGLE_CLKSEL_2_bm TCA_SINGLE_CLKSEL2_bm
  #elif defined(TCA_SINGLE_CLKSEL_2_bm)
    //deprecated_constant_name TCA_SINGLE_CLKSEL2_bm = TCA_SINGLE_CLKSEL_2_bm;
    #define TCA_SINGLE_CLKSEL2_bm TCA_SINGLE_CLKSEL_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_CLKSEL_2_bp) && defined(TCA_SINGLE_CLKSEL2_bp)
    #define TCA_SINGLE_CLKSEL_2_bp TCA_SINGLE_CLKSEL2_bp
  #elif defined(TCA_SINGLE_CLKSEL_2_bp)
    //deprecated_constant_name TCA_SINGLE_CLKSEL2_bp = TCA_SINGLE_CLKSEL_2_bp;
    #define TCA_SINGLE_CLKSEL2_bp TCA_SINGLE_CLKSEL_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCA_SINGLE_WGMODE_0_bm) && defined(TCA_SINGLE_WGMODE0_bm)
    #define TCA_SINGLE_WGMODE_0_bm TCA_SINGLE_WGMODE0_bm
  #elif defined(TCA_SINGLE_WGMODE_0_bm)
    //deprecated_constant_name TCA_SINGLE_WGMODE0_bm = TCA_SINGLE_WGMODE_0_bm;
    #define TCA_SINGLE_WGMODE0_bm TCA_SINGLE_WGMODE_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_WGMODE_0_bp) && defined(TCA_SINGLE_WGMODE0_bp)
    #define TCA_SINGLE_WGMODE_0_bp TCA_SINGLE_WGMODE0_bp
  #elif defined(TCA_SINGLE_WGMODE_0_bp)
    //deprecated_constant_name TCA_SINGLE_WGMODE0_bp = TCA_SINGLE_WGMODE_0_bp;
    #define TCA_SINGLE_WGMODE0_bp TCA_SINGLE_WGMODE_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_WGMODE_1_bm) && defined(TCA_SINGLE_WGMODE1_bm)
    #define TCA_SINGLE_WGMODE_1_bm TCA_SINGLE_WGMODE1_bm
  #elif defined(TCA_SINGLE_WGMODE_1_bm)
    //deprecated_constant_name TCA_SINGLE_WGMODE1_bm = TCA_SINGLE_WGMODE_1_bm;
    #define TCA_SINGLE_WGMODE1_bm TCA_SINGLE_WGMODE_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_WGMODE_1_bp) && defined(TCA_SINGLE_WGMODE1_bp)
    #define TCA_SINGLE_WGMODE_1_bp TCA_SINGLE_WGMODE1_bp
  #elif defined(TCA_SINGLE_WGMODE_1_bp)
    //deprecated_constant_name TCA_SINGLE_WGMODE1_bp = TCA_SINGLE_WGMODE_1_bp;
    #define TCA_SINGLE_WGMODE1_bp TCA_SINGLE_WGMODE_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_WGMODE_2_bm) && defined(TCA_SINGLE_WGMODE2_bm)
    #define TCA_SINGLE_WGMODE_2_bm TCA_SINGLE_WGMODE2_bm
  #elif defined(TCA_SINGLE_WGMODE_2_bm)
    //deprecated_constant_name TCA_SINGLE_WGMODE2_bm = TCA_SINGLE_WGMODE_2_bm;
    #define TCA_SINGLE_WGMODE2_bm TCA_SINGLE_WGMODE_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_WGMODE_2_bp) && defined(TCA_SINGLE_WGMODE2_bp)
    #define TCA_SINGLE_WGMODE_2_bp TCA_SINGLE_WGMODE2_bp
  #elif defined(TCA_SINGLE_WGMODE_2_bp)
    //deprecated_constant_name TCA_SINGLE_WGMODE2_bp = TCA_SINGLE_WGMODE_2_bp;
    #define TCA_SINGLE_WGMODE2_bp TCA_SINGLE_WGMODE_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCA_SINGLE_CMD_0_bm) && defined(TCA_SINGLE_CMD0_bm)
    #define TCA_SINGLE_CMD_0_bm TCA_SINGLE_CMD0_bm
  #elif defined(TCA_SINGLE_CMD_0_bm)
    //deprecated_constant_name TCA_SINGLE_CMD0_bm = TCA_SINGLE_CMD_0_bm;
    #define TCA_SINGLE_CMD0_bm TCA_SINGLE_CMD_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_CMD_0_bp) && defined(TCA_SINGLE_CMD0_bp)
    #define TCA_SINGLE_CMD_0_bp TCA_SINGLE_CMD0_bp
  #elif defined(TCA_SINGLE_CMD_0_bp)
    //deprecated_constant_name TCA_SINGLE_CMD0_bp = TCA_SINGLE_CMD_0_bp;
    #define TCA_SINGLE_CMD0_bp TCA_SINGLE_CMD_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_CMD_1_bm) && defined(TCA_SINGLE_CMD1_bm)
    #define TCA_SINGLE_CMD_1_bm TCA_SINGLE_CMD1_bm
  #elif defined(TCA_SINGLE_CMD_1_bm)
    //deprecated_constant_name TCA_SINGLE_CMD1_bm = TCA_SINGLE_CMD_1_bm;
    #define TCA_SINGLE_CMD1_bm TCA_SINGLE_CMD_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_CMD_1_bp) && defined(TCA_SINGLE_CMD1_bp)
    #define TCA_SINGLE_CMD_1_bp TCA_SINGLE_CMD1_bp
  #elif defined(TCA_SINGLE_CMD_1_bp)
    //deprecated_constant_name TCA_SINGLE_CMD1_bp = TCA_SINGLE_CMD_1_bp;
    #define TCA_SINGLE_CMD1_bp TCA_SINGLE_CMD_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCA_SINGLE_EVACTA_0_bm) && defined(TCA_SINGLE_EVACTA0_bm)
    #define TCA_SINGLE_EVACTA_0_bm TCA_SINGLE_EVACTA0_bm
  #elif defined(TCA_SINGLE_EVACTA_0_bm)
    //deprecated_constant_name TCA_SINGLE_EVACTA0_bm = TCA_SINGLE_EVACTA_0_bm;
    #define TCA_SINGLE_EVACTA0_bm TCA_SINGLE_EVACTA_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_EVACTA_0_bp) && defined(TCA_SINGLE_EVACTA0_bp)
    #define TCA_SINGLE_EVACTA_0_bp TCA_SINGLE_EVACTA0_bp
  #elif defined(TCA_SINGLE_EVACTA_0_bp)
    //deprecated_constant_name TCA_SINGLE_EVACTA0_bp = TCA_SINGLE_EVACTA_0_bp;
    #define TCA_SINGLE_EVACTA0_bp TCA_SINGLE_EVACTA_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_EVACTA_1_bm) && defined(TCA_SINGLE_EVACTA1_bm)
    #define TCA_SINGLE_EVACTA_1_bm TCA_SINGLE_EVACTA1_bm
  #elif defined(TCA_SINGLE_EVACTA_1_bm)
    //deprecated_constant_name TCA_SINGLE_EVACTA1_bm = TCA_SINGLE_EVACTA_1_bm;
    #define TCA_SINGLE_EVACTA1_bm TCA_SINGLE_EVACTA_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_EVACTA_1_bp) && defined(TCA_SINGLE_EVACTA1_bp)
    #define TCA_SINGLE_EVACTA_1_bp TCA_SINGLE_EVACTA1_bp
  #elif defined(TCA_SINGLE_EVACTA_1_bp)
    //deprecated_constant_name TCA_SINGLE_EVACTA1_bp = TCA_SINGLE_EVACTA_1_bp;
    #define TCA_SINGLE_EVACTA1_bp TCA_SINGLE_EVACTA_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_EVACTA_2_bm) && defined(TCA_SINGLE_EVACTA2_bm)
    #define TCA_SINGLE_EVACTA_2_bm TCA_SINGLE_EVACTA2_bm
  #elif defined(TCA_SINGLE_EVACTA_2_bm)
    //deprecated_constant_name TCA_SINGLE_EVACTA2_bm = TCA_SINGLE_EVACTA_2_bm;
    #define TCA_SINGLE_EVACTA2_bm TCA_SINGLE_EVACTA_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_EVACTA_2_bp) && defined(TCA_SINGLE_EVACTA2_bp)
    #define TCA_SINGLE_EVACTA_2_bp TCA_SINGLE_EVACTA2_bp
  #elif defined(TCA_SINGLE_EVACTA_2_bp)
    //deprecated_constant_name TCA_SINGLE_EVACTA2_bp = TCA_SINGLE_EVACTA_2_bp;
    #define TCA_SINGLE_EVACTA2_bp TCA_SINGLE_EVACTA_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_EVACTB_0_bm) && defined(TCA_SINGLE_EVACTB0_bm)
    #define TCA_SINGLE_EVACTB_0_bm TCA_SINGLE_EVACTB0_bm
  #elif defined(TCA_SINGLE_EVACTB_0_bm)
    //deprecated_constant_name TCA_SINGLE_EVACTB0_bm = TCA_SINGLE_EVACTB_0_bm;
    #define TCA_SINGLE_EVACTB0_bm TCA_SINGLE_EVACTB_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_EVACTB_0_bp) && defined(TCA_SINGLE_EVACTB0_bp)
    #define TCA_SINGLE_EVACTB_0_bp TCA_SINGLE_EVACTB0_bp
  #elif defined(TCA_SINGLE_EVACTB_0_bp)
    //deprecated_constant_name TCA_SINGLE_EVACTB0_bp = TCA_SINGLE_EVACTB_0_bp;
    #define TCA_SINGLE_EVACTB0_bp TCA_SINGLE_EVACTB_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_EVACTB_1_bm) && defined(TCA_SINGLE_EVACTB1_bm)
    #define TCA_SINGLE_EVACTB_1_bm TCA_SINGLE_EVACTB1_bm
  #elif defined(TCA_SINGLE_EVACTB_1_bm)
    //deprecated_constant_name TCA_SINGLE_EVACTB1_bm = TCA_SINGLE_EVACTB_1_bm;
    #define TCA_SINGLE_EVACTB1_bm TCA_SINGLE_EVACTB_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_EVACTB_1_bp) && defined(TCA_SINGLE_EVACTB1_bp)
    #define TCA_SINGLE_EVACTB_1_bp TCA_SINGLE_EVACTB1_bp
  #elif defined(TCA_SINGLE_EVACTB_1_bp)
    //deprecated_constant_name TCA_SINGLE_EVACTB1_bp = TCA_SINGLE_EVACTB_1_bp;
    #define TCA_SINGLE_EVACTB1_bp TCA_SINGLE_EVACTB_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_EVACTB_2_bm) && defined(TCA_SINGLE_EVACTB2_bm)
    #define TCA_SINGLE_EVACTB_2_bm TCA_SINGLE_EVACTB2_bm
  #elif defined(TCA_SINGLE_EVACTB_2_bm)
    //deprecated_constant_name TCA_SINGLE_EVACTB2_bm = TCA_SINGLE_EVACTB_2_bm;
    #define TCA_SINGLE_EVACTB2_bm TCA_SINGLE_EVACTB_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SINGLE_EVACTB_2_bp) && defined(TCA_SINGLE_EVACTB2_bp)
    #define TCA_SINGLE_EVACTB_2_bp TCA_SINGLE_EVACTB2_bp
  #elif defined(TCA_SINGLE_EVACTB_2_bp)
    //deprecated_constant_name TCA_SINGLE_EVACTB2_bp = TCA_SINGLE_EVACTB_2_bp;
    #define TCA_SINGLE_EVACTB2_bp TCA_SINGLE_EVACTB_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCA_SPLIT_CLKSEL_0_bm) && defined(TCA_SPLIT_CLKSEL0_bm)
    #define TCA_SPLIT_CLKSEL_0_bm TCA_SPLIT_CLKSEL0_bm
  #elif defined(TCA_SPLIT_CLKSEL_0_bm)
    //deprecated_constant_name TCA_SPLIT_CLKSEL0_bm = TCA_SPLIT_CLKSEL_0_bm;
    #define TCA_SPLIT_CLKSEL0_bm TCA_SPLIT_CLKSEL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SPLIT_CLKSEL_0_bp) && defined(TCA_SPLIT_CLKSEL0_bp)
    #define TCA_SPLIT_CLKSEL_0_bp TCA_SPLIT_CLKSEL0_bp
  #elif defined(TCA_SPLIT_CLKSEL_0_bp)
    //deprecated_constant_name TCA_SPLIT_CLKSEL0_bp = TCA_SPLIT_CLKSEL_0_bp;
    #define TCA_SPLIT_CLKSEL0_bp TCA_SPLIT_CLKSEL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SPLIT_CLKSEL_1_bm) && defined(TCA_SPLIT_CLKSEL1_bm)
    #define TCA_SPLIT_CLKSEL_1_bm TCA_SPLIT_CLKSEL1_bm
  #elif defined(TCA_SPLIT_CLKSEL_1_bm)
    //deprecated_constant_name TCA_SPLIT_CLKSEL1_bm = TCA_SPLIT_CLKSEL_1_bm;
    #define TCA_SPLIT_CLKSEL1_bm TCA_SPLIT_CLKSEL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SPLIT_CLKSEL_1_bp) && defined(TCA_SPLIT_CLKSEL1_bp)
    #define TCA_SPLIT_CLKSEL_1_bp TCA_SPLIT_CLKSEL1_bp
  #elif defined(TCA_SPLIT_CLKSEL_1_bp)
    //deprecated_constant_name TCA_SPLIT_CLKSEL1_bp = TCA_SPLIT_CLKSEL_1_bp;
    #define TCA_SPLIT_CLKSEL1_bp TCA_SPLIT_CLKSEL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SPLIT_CLKSEL_2_bm) && defined(TCA_SPLIT_CLKSEL2_bm)
    #define TCA_SPLIT_CLKSEL_2_bm TCA_SPLIT_CLKSEL2_bm
  #elif defined(TCA_SPLIT_CLKSEL_2_bm)
    //deprecated_constant_name TCA_SPLIT_CLKSEL2_bm = TCA_SPLIT_CLKSEL_2_bm;
    #define TCA_SPLIT_CLKSEL2_bm TCA_SPLIT_CLKSEL_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SPLIT_CLKSEL_2_bp) && defined(TCA_SPLIT_CLKSEL2_bp)
    #define TCA_SPLIT_CLKSEL_2_bp TCA_SPLIT_CLKSEL2_bp
  #elif defined(TCA_SPLIT_CLKSEL_2_bp)
    //deprecated_constant_name TCA_SPLIT_CLKSEL2_bp = TCA_SPLIT_CLKSEL_2_bp;
    #define TCA_SPLIT_CLKSEL2_bp TCA_SPLIT_CLKSEL_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCA_SPLIT_CMDEN_0_bm) && defined(TCA_SPLIT_CMDEN0_bm)
    #define TCA_SPLIT_CMDEN_0_bm TCA_SPLIT_CMDEN0_bm
  #elif defined(TCA_SPLIT_CMDEN_0_bm)
    //deprecated_constant_name TCA_SPLIT_CMDEN0_bm = TCA_SPLIT_CMDEN_0_bm;
    #define TCA_SPLIT_CMDEN0_bm TCA_SPLIT_CMDEN_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SPLIT_CMDEN_0_bp) && defined(TCA_SPLIT_CMDEN0_bp)
    #define TCA_SPLIT_CMDEN_0_bp TCA_SPLIT_CMDEN0_bp
  #elif defined(TCA_SPLIT_CMDEN_0_bp)
    //deprecated_constant_name TCA_SPLIT_CMDEN0_bp = TCA_SPLIT_CMDEN_0_bp;
    #define TCA_SPLIT_CMDEN0_bp TCA_SPLIT_CMDEN_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SPLIT_CMDEN_1_bm) && defined(TCA_SPLIT_CMDEN1_bm)
    #define TCA_SPLIT_CMDEN_1_bm TCA_SPLIT_CMDEN1_bm
  #elif defined(TCA_SPLIT_CMDEN_1_bm)
    //deprecated_constant_name TCA_SPLIT_CMDEN1_bm = TCA_SPLIT_CMDEN_1_bm;
    #define TCA_SPLIT_CMDEN1_bm TCA_SPLIT_CMDEN_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SPLIT_CMDEN_1_bp) && defined(TCA_SPLIT_CMDEN1_bp)
    #define TCA_SPLIT_CMDEN_1_bp TCA_SPLIT_CMDEN1_bp
  #elif defined(TCA_SPLIT_CMDEN_1_bp)
    //deprecated_constant_name TCA_SPLIT_CMDEN1_bp = TCA_SPLIT_CMDEN_1_bp;
    #define TCA_SPLIT_CMDEN1_bp TCA_SPLIT_CMDEN_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SPLIT_CMD_0_bm) && defined(TCA_SPLIT_CMD0_bm)
    #define TCA_SPLIT_CMD_0_bm TCA_SPLIT_CMD0_bm
  #elif defined(TCA_SPLIT_CMD_0_bm)
    //deprecated_constant_name TCA_SPLIT_CMD0_bm = TCA_SPLIT_CMD_0_bm;
    #define TCA_SPLIT_CMD0_bm TCA_SPLIT_CMD_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SPLIT_CMD_0_bp) && defined(TCA_SPLIT_CMD0_bp)
    #define TCA_SPLIT_CMD_0_bp TCA_SPLIT_CMD0_bp
  #elif defined(TCA_SPLIT_CMD_0_bp)
    //deprecated_constant_name TCA_SPLIT_CMD0_bp = TCA_SPLIT_CMD_0_bp;
    #define TCA_SPLIT_CMD0_bp TCA_SPLIT_CMD_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SPLIT_CMD_1_bm) && defined(TCA_SPLIT_CMD1_bm)
    #define TCA_SPLIT_CMD_1_bm TCA_SPLIT_CMD1_bm
  #elif defined(TCA_SPLIT_CMD_1_bm)
    //deprecated_constant_name TCA_SPLIT_CMD1_bm = TCA_SPLIT_CMD_1_bm;
    #define TCA_SPLIT_CMD1_bm TCA_SPLIT_CMD_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCA_SPLIT_CMD_1_bp) && defined(TCA_SPLIT_CMD1_bp)
    #define TCA_SPLIT_CMD_1_bp TCA_SPLIT_CMD1_bp
  #elif defined(TCA_SPLIT_CMD_1_bp)
    //deprecated_constant_name TCA_SPLIT_CMD1_bp = TCA_SPLIT_CMD_1_bp;
    #define TCA_SPLIT_CMD1_bp TCA_SPLIT_CMD_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= TCBs ======= */
  #if !defined(TCB_CLKSEL_0_bm) && defined(TCB_CLKSEL0_bm)
    #define TCB_CLKSEL_0_bm TCB_CLKSEL0_bm
  #elif defined(TCB_CLKSEL_0_bm)
    //deprecated_constant_name TCB_CLKSEL0_bm = TCB_CLKSEL_0_bm;
    #define TCB_CLKSEL0_bm TCB_CLKSEL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCB_CLKSEL_0_bp) && defined(TCB_CLKSEL0_bp)
    #define TCB_CLKSEL_0_bp TCB_CLKSEL0_bp
  #elif defined(TCB_CLKSEL_0_bp)
    //deprecated_constant_name TCB_CLKSEL0_bp = TCB_CLKSEL_0_bp;
    #define TCB_CLKSEL0_bp TCB_CLKSEL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCB_CLKSEL_1_bm) && defined(TCB_CLKSEL1_bm)
    #define TCB_CLKSEL_1_bm TCB_CLKSEL1_bm
  #elif defined(TCB_CLKSEL_1_bm)
    //deprecated_constant_name TCB_CLKSEL1_bm = TCB_CLKSEL_1_bm;
    #define TCB_CLKSEL1_bm TCB_CLKSEL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCB_CLKSEL_1_bp) && defined(TCB_CLKSEL1_bp)
    #define TCB_CLKSEL_1_bp TCB_CLKSEL1_bp
  #elif defined(TCB_CLKSEL_1_bp)
    //deprecated_constant_name TCB_CLKSEL1_bp = TCB_CLKSEL_1_bp;
    #define TCB_CLKSEL1_bp TCB_CLKSEL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCB_CLKSEL_2_bm) && defined(TCB_CLKSEL2_bm)
    #define TCB_CLKSEL_2_bm TCB_CLKSEL2_bm
  #elif defined(TCB_CLKSEL_2_bm)
    //deprecated_constant_name TCB_CLKSEL2_bm = TCB_CLKSEL_2_bm;
    #define TCB_CLKSEL2_bm TCB_CLKSEL_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCB_CLKSEL_2_bp) && defined(TCB_CLKSEL2_bp)
    #define TCB_CLKSEL_2_bp TCB_CLKSEL2_bp
  #elif defined(TCB_CLKSEL_2_bp)
    //deprecated_constant_name TCB_CLKSEL2_bp = TCB_CLKSEL_2_bp;
    #define TCB_CLKSEL2_bp TCB_CLKSEL_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCB_CNTMODE_0_bm) && defined(TCB_CNTMODE0_bm)
    #define TCB_CNTMODE_0_bm TCB_CNTMODE0_bm
  #elif defined(TCB_CNTMODE_0_bm)
    //deprecated_constant_name TCB_CNTMODE0_bm = TCB_CNTMODE_0_bm;
    #define TCB_CNTMODE0_bm TCB_CNTMODE_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCB_CNTMODE_0_bp) && defined(TCB_CNTMODE0_bp)
    #define TCB_CNTMODE_0_bp TCB_CNTMODE0_bp
  #elif defined(TCB_CNTMODE_0_bp)
    //deprecated_constant_name TCB_CNTMODE0_bp = TCB_CNTMODE_0_bp;
    #define TCB_CNTMODE0_bp TCB_CNTMODE_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCB_CNTMODE_1_bm) && defined(TCB_CNTMODE1_bm)
    #define TCB_CNTMODE_1_bm TCB_CNTMODE1_bm
  #elif defined(TCB_CNTMODE_1_bm)
    //deprecated_constant_name TCB_CNTMODE1_bm = TCB_CNTMODE_1_bm;
    #define TCB_CNTMODE1_bm TCB_CNTMODE_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCB_CNTMODE_1_bp) && defined(TCB_CNTMODE1_bp)
    #define TCB_CNTMODE_1_bp TCB_CNTMODE1_bp
  #elif defined(TCB_CNTMODE_1_bp)
    //deprecated_constant_name TCB_CNTMODE1_bp = TCB_CNTMODE_1_bp;
    #define TCB_CNTMODE1_bp TCB_CNTMODE_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCB_CNTMODE_2_bm) && defined(TCB_CNTMODE2_bm)
    #define TCB_CNTMODE_2_bm TCB_CNTMODE2_bm
  #elif defined(TCB_CNTMODE_2_bm)
    //deprecated_constant_name TCB_CNTMODE2_bm = TCB_CNTMODE_2_bm;
    #define TCB_CNTMODE2_bm TCB_CNTMODE_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCB_CNTMODE_2_bp) && defined(TCB_CNTMODE2_bp)
    #define TCB_CNTMODE_2_bp TCB_CNTMODE2_bp
  #elif defined(TCB_CNTMODE_2_bp)
    //deprecated_constant_name TCB_CNTMODE2_bp = TCB_CNTMODE_2_bp;
    #define TCB_CNTMODE2_bp TCB_CNTMODE_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= TCD ======= */
  #if !defined(TCD_SYNCPRES_0_bm) && defined(TCD_SYNCPRES0_bm)
    #define TCD_SYNCPRES_0_bm TCD_SYNCPRES0_bm
  #elif defined(TCD_SYNCPRES_0_bm)
    //deprecated_constant_name TCD_SYNCPRES0_bm = TCD_SYNCPRES_0_bm;
    #define TCD_SYNCPRES0_bm TCD_SYNCPRES_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_SYNCPRES_0_bp) && defined(TCD_SYNCPRES0_bp)
    #define TCD_SYNCPRES_0_bp TCD_SYNCPRES0_bp
  #elif defined(TCD_SYNCPRES_0_bp)
    //deprecated_constant_name TCD_SYNCPRES0_bp = TCD_SYNCPRES_0_bp;
    #define TCD_SYNCPRES0_bp TCD_SYNCPRES_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_SYNCPRES_1_bm) && defined(TCD_SYNCPRES1_bm)
    #define TCD_SYNCPRES_1_bm TCD_SYNCPRES1_bm
  #elif defined(TCD_SYNCPRES_1_bm)
    //deprecated_constant_name TCD_SYNCPRES1_bm = TCD_SYNCPRES_1_bm;
    #define TCD_SYNCPRES1_bm TCD_SYNCPRES_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_SYNCPRES_1_bp) && defined(TCD_SYNCPRES1_bp)
    #define TCD_SYNCPRES_1_bp TCD_SYNCPRES1_bp
  #elif defined(TCD_SYNCPRES_1_bp)
    //deprecated_constant_name TCD_SYNCPRES1_bp = TCD_SYNCPRES_1_bp;
    #define TCD_SYNCPRES1_bp TCD_SYNCPRES_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCD_CNTPRES_0_bm) && defined(TCD_CNTPRES0_bm)
    #define TCD_CNTPRES_0_bm TCD_CNTPRES0_bm
  #elif defined(TCD_CNTPRES_0_bm)
    //deprecated_constant_name TCD_CNTPRES0_bm = TCD_CNTPRES_0_bm;
    #define TCD_CNTPRES0_bm TCD_CNTPRES_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CNTPRES_0_bp) && defined(TCD_CNTPRES0_bp)
    #define TCD_CNTPRES_0_bp TCD_CNTPRES0_bp
  #elif defined(TCD_CNTPRES_0_bp)
    //deprecated_constant_name TCD_CNTPRES0_bp = TCD_CNTPRES_0_bp;
    #define TCD_CNTPRES0_bp TCD_CNTPRES_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CNTPRES_1_bm) && defined(TCD_CNTPRES1_bm)
    #define TCD_CNTPRES_1_bm TCD_CNTPRES1_bm
  #elif defined(TCD_CNTPRES_1_bm)
    //deprecated_constant_name TCD_CNTPRES1_bm = TCD_CNTPRES_1_bm;
    #define TCD_CNTPRES1_bm TCD_CNTPRES_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CNTPRES_1_bp) && defined(TCD_CNTPRES1_bp)
    #define TCD_CNTPRES_1_bp TCD_CNTPRES1_bp
  #elif defined(TCD_CNTPRES_1_bp)
    //deprecated_constant_name TCD_CNTPRES1_bp = TCD_CNTPRES_1_bp;
    #define TCD_CNTPRES1_bp TCD_CNTPRES_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCD_CLKSEL_0_bm) && defined(TCD_CLKSEL0_bm)
    #define TCD_CLKSEL_0_bm TCD_CLKSEL0_bm
  #elif defined(TCD_CLKSEL_0_bm)
    //deprecated_constant_name TCD_CLKSEL0_bm = TCD_CLKSEL_0_bm;
    #define TCD_CLKSEL0_bm TCD_CLKSEL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CLKSEL_0_bp) && defined(TCD_CLKSEL0_bp)
    #define TCD_CLKSEL_0_bp TCD_CLKSEL0_bp
  #elif defined(TCD_CLKSEL_0_bp)
    //deprecated_constant_name TCD_CLKSEL0_bp = TCD_CLKSEL_0_bp;
    #define TCD_CLKSEL0_bp TCD_CLKSEL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CLKSEL_1_bm) && defined(TCD_CLKSEL1_bm)
    #define TCD_CLKSEL_1_bm TCD_CLKSEL1_bm
  #elif defined(TCD_CLKSEL_1_bm)
    //deprecated_constant_name TCD_CLKSEL1_bm = TCD_CLKSEL_1_bm;
    #define TCD_CLKSEL1_bm TCD_CLKSEL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CLKSEL_1_bp) && defined(TCD_CLKSEL1_bp)
    #define TCD_CLKSEL_1_bp TCD_CLKSEL1_bp
  #elif defined(TCD_CLKSEL_1_bp)
    //deprecated_constant_name TCD_CLKSEL1_bp = TCD_CLKSEL_1_bp;
    #define TCD_CLKSEL1_bp TCD_CLKSEL_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCD_WGMODE_0_bm) && defined(TCD_WGMODE0_bm)
    #define TCD_WGMODE_0_bm TCD_WGMODE0_bm
  #elif defined(TCD_WGMODE_0_bm)
    //deprecated_constant_name TCD_WGMODE0_bm = TCD_WGMODE_0_bm;
    #define TCD_WGMODE0_bm TCD_WGMODE_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_WGMODE_0_bp) && defined(TCD_WGMODE0_bp)
    #define TCD_WGMODE_0_bp TCD_WGMODE0_bp
  #elif defined(TCD_WGMODE_0_bp)
    //deprecated_constant_name TCD_WGMODE0_bp = TCD_WGMODE_0_bp;
    #define TCD_WGMODE0_bp TCD_WGMODE_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_WGMODE_1_bm) && defined(TCD_WGMODE1_bm)
    #define TCD_WGMODE_1_bm TCD_WGMODE1_bm
  #elif defined(TCD_WGMODE_1_bm)
    //deprecated_constant_name TCD_WGMODE1_bm = TCD_WGMODE_1_bm;
    #define TCD_WGMODE1_bm TCD_WGMODE_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_WGMODE_1_bp) && defined(TCD_WGMODE1_bp)
    #define TCD_WGMODE_1_bp TCD_WGMODE1_bp
  #elif defined(TCD_WGMODE_1_bp)
    //deprecated_constant_name TCD_WGMODE1_bp = TCD_WGMODE_1_bp;
    #define TCD_WGMODE1_bp TCD_WGMODE_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCD_CMPAVAL_0_bm) && defined(TCD_CMPAVAL0_bm)
    #define TCD_CMPAVAL_0_bm TCD_CMPAVAL0_bm
  #elif defined(TCD_CMPAVAL_0_bm)
    //deprecated_constant_name TCD_CMPAVAL0_bm = TCD_CMPAVAL_0_bm;
    #define TCD_CMPAVAL0_bm TCD_CMPAVAL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPAVAL_0_bp) && defined(TCD_CMPAVAL0_bp)
    #define TCD_CMPAVAL_0_bp TCD_CMPAVAL0_bp
  #elif defined(TCD_CMPAVAL_0_bp)
    //deprecated_constant_name TCD_CMPAVAL0_bp = TCD_CMPAVAL_0_bp;
    #define TCD_CMPAVAL0_bp TCD_CMPAVAL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPAVAL_1_bm) && defined(TCD_CMPAVAL1_bm)
    #define TCD_CMPAVAL_1_bm TCD_CMPAVAL1_bm
  #elif defined(TCD_CMPAVAL_1_bm)
    //deprecated_constant_name TCD_CMPAVAL1_bm = TCD_CMPAVAL_1_bm;
    #define TCD_CMPAVAL1_bm TCD_CMPAVAL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPAVAL_1_bp) && defined(TCD_CMPAVAL1_bp)
    #define TCD_CMPAVAL_1_bp TCD_CMPAVAL1_bp
  #elif defined(TCD_CMPAVAL_1_bp)
    //deprecated_constant_name TCD_CMPAVAL1_bp = TCD_CMPAVAL_1_bp;
    #define TCD_CMPAVAL1_bp TCD_CMPAVAL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPAVAL_2_bm) && defined(TCD_CMPAVAL2_bm)
    #define TCD_CMPAVAL_2_bm TCD_CMPAVAL2_bm
  #elif defined(TCD_CMPAVAL_2_bm)
    //deprecated_constant_name TCD_CMPAVAL2_bm = TCD_CMPAVAL_2_bm;
    #define TCD_CMPAVAL2_bm TCD_CMPAVAL_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPAVAL_2_bp) && defined(TCD_CMPAVAL2_bp)
    #define TCD_CMPAVAL_2_bp TCD_CMPAVAL2_bp
  #elif defined(TCD_CMPAVAL_2_bp)
    //deprecated_constant_name TCD_CMPAVAL2_bp = TCD_CMPAVAL_2_bp;
    #define TCD_CMPAVAL2_bp TCD_CMPAVAL_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPAVAL_3_bm) && defined(TCD_CMPAVAL3_bm)
    #define TCD_CMPAVAL_3_bm TCD_CMPAVAL3_bm
  #elif defined(TCD_CMPAVAL_3_bm)
    //deprecated_constant_name TCD_CMPAVAL3_bm = TCD_CMPAVAL_3_bm;
    #define TCD_CMPAVAL3_bm TCD_CMPAVAL_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPAVAL_3_bp) && defined(TCD_CMPAVAL3_bp)
    #define TCD_CMPAVAL_3_bp TCD_CMPAVAL3_bp
  #elif defined(TCD_CMPAVAL_3_bp)
    //deprecated_constant_name TCD_CMPAVAL3_bp = TCD_CMPAVAL_3_bp;
    #define TCD_CMPAVAL3_bp TCD_CMPAVAL_3_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPBVAL_0_bm) && defined(TCD_CMPBVAL0_bm)
    #define TCD_CMPBVAL_0_bm TCD_CMPBVAL0_bm
  #elif defined(TCD_CMPBVAL_0_bm)
    //deprecated_constant_name TCD_CMPBVAL0_bm = TCD_CMPBVAL_0_bm;
    #define TCD_CMPBVAL0_bm TCD_CMPBVAL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPBVAL_0_bp) && defined(TCD_CMPBVAL0_bp)
    #define TCD_CMPBVAL_0_bp TCD_CMPBVAL0_bp
  #elif defined(TCD_CMPBVAL_0_bp)
    //deprecated_constant_name TCD_CMPBVAL0_bp = TCD_CMPBVAL_0_bp;
    #define TCD_CMPBVAL0_bp TCD_CMPBVAL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPBVAL_1_bm) && defined(TCD_CMPBVAL1_bm)
    #define TCD_CMPBVAL_1_bm TCD_CMPBVAL1_bm
  #elif defined(TCD_CMPBVAL_1_bm)
    //deprecated_constant_name TCD_CMPBVAL1_bm = TCD_CMPBVAL_1_bm;
    #define TCD_CMPBVAL1_bm TCD_CMPBVAL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPBVAL_1_bp) && defined(TCD_CMPBVAL1_bp)
    #define TCD_CMPBVAL_1_bp TCD_CMPBVAL1_bp
  #elif defined(TCD_CMPBVAL_1_bp)
    //deprecated_constant_name TCD_CMPBVAL1_bp = TCD_CMPBVAL_1_bp;
    #define TCD_CMPBVAL1_bp TCD_CMPBVAL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPBVAL_2_bm) && defined(TCD_CMPBVAL2_bm)
    #define TCD_CMPBVAL_2_bm TCD_CMPBVAL2_bm
  #elif defined(TCD_CMPBVAL_2_bm)
    //deprecated_constant_name TCD_CMPBVAL2_bm = TCD_CMPBVAL_2_bm;
    #define TCD_CMPBVAL2_bm TCD_CMPBVAL_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPBVAL_2_bp) && defined(TCD_CMPBVAL2_bp)
    #define TCD_CMPBVAL_2_bp TCD_CMPBVAL2_bp
  #elif defined(TCD_CMPBVAL_2_bp)
    //deprecated_constant_name TCD_CMPBVAL2_bp = TCD_CMPBVAL_2_bp;
    #define TCD_CMPBVAL2_bp TCD_CMPBVAL_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPBVAL_3_bm) && defined(TCD_CMPBVAL3_bm)
    #define TCD_CMPBVAL_3_bm TCD_CMPBVAL3_bm
  #elif defined(TCD_CMPBVAL_3_bm)
    //deprecated_constant_name TCD_CMPBVAL3_bm = TCD_CMPBVAL_3_bm;
    #define TCD_CMPBVAL3_bm TCD_CMPBVAL_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CMPBVAL_3_bp) && defined(TCD_CMPBVAL3_bp)
    #define TCD_CMPBVAL_3_bp TCD_CMPBVAL3_bp
  #elif defined(TCD_CMPBVAL_3_bp)
    //deprecated_constant_name TCD_CMPBVAL3_bp = TCD_CMPBVAL_3_bp;
    #define TCD_CMPBVAL3_bp TCD_CMPBVAL_3_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCD_CFG_0_bm) && defined(TCD_CFG0_bm)
    #define TCD_CFG_0_bm TCD_CFG0_bm
  #elif defined(TCD_CFG_0_bm)
    //deprecated_constant_name TCD_CFG0_bm = TCD_CFG_0_bm;
    #define TCD_CFG0_bm TCD_CFG_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CFG_0_bp) && defined(TCD_CFG0_bp)
    #define TCD_CFG_0_bp TCD_CFG0_bp
  #elif defined(TCD_CFG_0_bp)
    //deprecated_constant_name TCD_CFG0_bp = TCD_CFG_0_bp;
    #define TCD_CFG0_bp TCD_CFG_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CFG_1_bm) && defined(TCD_CFG1_bm)
    #define TCD_CFG_1_bm TCD_CFG1_bm
  #elif defined(TCD_CFG_1_bm)
    //deprecated_constant_name TCD_CFG1_bm = TCD_CFG_1_bm;
    #define TCD_CFG1_bm TCD_CFG_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_CFG_1_bp) && defined(TCD_CFG1_bp)
    #define TCD_CFG_1_bp TCD_CFG1_bp
  #elif defined(TCD_CFG_1_bp)
    //deprecated_constant_name TCD_CFG1_bp = TCD_CFG_1_bp;
    #define TCD_CFG1_bp TCD_CFG_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCD_INPUTMODE_0_bm) && defined(TCD_INPUTMODE0_bm)
    #define TCD_INPUTMODE_0_bm TCD_INPUTMODE0_bm
  #elif defined(TCD_INPUTMODE_0_bm)
    //deprecated_constant_name TCD_INPUTMODE0_bm = TCD_INPUTMODE_0_bm;
    #define TCD_INPUTMODE0_bm TCD_INPUTMODE_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_INPUTMODE_0_bp) && defined(TCD_INPUTMODE0_bp)
    #define TCD_INPUTMODE_0_bp TCD_INPUTMODE0_bp
  #elif defined(TCD_INPUTMODE_0_bp)
    //deprecated_constant_name TCD_INPUTMODE0_bp = TCD_INPUTMODE_0_bp;
    #define TCD_INPUTMODE0_bp TCD_INPUTMODE_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_INPUTMODE_1_bm) && defined(TCD_INPUTMODE1_bm)
    #define TCD_INPUTMODE_1_bm TCD_INPUTMODE1_bm
  #elif defined(TCD_INPUTMODE_1_bm)
    //deprecated_constant_name TCD_INPUTMODE1_bm = TCD_INPUTMODE_1_bm;
    #define TCD_INPUTMODE1_bm TCD_INPUTMODE_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_INPUTMODE_1_bp) && defined(TCD_INPUTMODE1_bp)
    #define TCD_INPUTMODE_1_bp TCD_INPUTMODE1_bp
  #elif defined(TCD_INPUTMODE_1_bp)
    //deprecated_constant_name TCD_INPUTMODE1_bp = TCD_INPUTMODE_1_bp;
    #define TCD_INPUTMODE1_bp TCD_INPUTMODE_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_INPUTMODE_2_bm) && defined(TCD_INPUTMODE2_bm)
    #define TCD_INPUTMODE_2_bm TCD_INPUTMODE2_bm
  #elif defined(TCD_INPUTMODE_2_bm)
    //deprecated_constant_name TCD_INPUTMODE2_bm = TCD_INPUTMODE_2_bm;
    #define TCD_INPUTMODE2_bm TCD_INPUTMODE_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_INPUTMODE_2_bp) && defined(TCD_INPUTMODE2_bp)
    #define TCD_INPUTMODE_2_bp TCD_INPUTMODE2_bp
  #elif defined(TCD_INPUTMODE_2_bp)
    //deprecated_constant_name TCD_INPUTMODE2_bp = TCD_INPUTMODE_2_bp;
    #define TCD_INPUTMODE2_bp TCD_INPUTMODE_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_INPUTMODE_3_bm) && defined(TCD_INPUTMODE3_bm)
    #define TCD_INPUTMODE_3_bm TCD_INPUTMODE3_bm
  #elif defined(TCD_INPUTMODE_3_bm)
    //deprecated_constant_name TCD_INPUTMODE3_bm = TCD_INPUTMODE_3_bm;
    #define TCD_INPUTMODE3_bm TCD_INPUTMODE_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_INPUTMODE_3_bp) && defined(TCD_INPUTMODE3_bp)
    #define TCD_INPUTMODE_3_bp TCD_INPUTMODE3_bp
  #elif defined(TCD_INPUTMODE_3_bp)
    //deprecated_constant_name TCD_INPUTMODE3_bp = TCD_INPUTMODE_3_bp;
    #define TCD_INPUTMODE3_bp TCD_INPUTMODE_3_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCD_DLYSEL_0_bm) && defined(TCD_DLYSEL0_bm)
    #define TCD_DLYSEL_0_bm TCD_DLYSEL0_bm
  #elif defined(TCD_DLYSEL_0_bm)
    //deprecated_constant_name TCD_DLYSEL0_bm = TCD_DLYSEL_0_bm;
    #define TCD_DLYSEL0_bm TCD_DLYSEL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYSEL_0_bp) && defined(TCD_DLYSEL0_bp)
    #define TCD_DLYSEL_0_bp TCD_DLYSEL0_bp
  #elif defined(TCD_DLYSEL_0_bp)
    //deprecated_constant_name TCD_DLYSEL0_bp = TCD_DLYSEL_0_bp;
    #define TCD_DLYSEL0_bp TCD_DLYSEL_0_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCD_DLYSEL_1_bm) && defined(TCD_DLYSEL1_bm)
    #define TCD_DLYSEL_1_bm TCD_DLYSEL1_bm
  #elif defined(TCD_DLYSEL_1_bm)
    //deprecated_constant_name TCD_DLYSEL1_bm = TCD_DLYSEL_1_bm;
    #define TCD_DLYSEL1_bm TCD_DLYSEL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYSEL_1_bp) && defined(TCD_DLYSEL1_bp)
    #define TCD_DLYSEL_1_bp TCD_DLYSEL1_bp
  #elif defined(TCD_DLYSEL_1_bp)
    //deprecated_constant_name TCD_DLYSEL1_bp = TCD_DLYSEL_1_bp;
    #define TCD_DLYSEL1_bp TCD_DLYSEL_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCD_DLYTRIG_0_bm) && defined(TCD_DLYTRIG0_bm)
    #define TCD_DLYTRIG_0_bm TCD_DLYTRIG0_bm
  #elif defined(TCD_DLYTRIG_0_bm)
    //deprecated_constant_name TCD_DLYTRIG0_bm = TCD_DLYTRIG_0_bm;
    #define TCD_DLYTRIG0_bm TCD_DLYTRIG_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYTRIG_0_bp) && defined(TCD_DLYTRIG0_bp)
    #define TCD_DLYTRIG_0_bp TCD_DLYTRIG0_bp
  #elif defined(TCD_DLYTRIG_0_bp)
    //deprecated_constant_name TCD_DLYTRIG0_bp = TCD_DLYTRIG_0_bp;
    #define TCD_DLYTRIG0_bp TCD_DLYTRIG_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYTRIG_1_bm) && defined(TCD_DLYTRIG1_bm)
    #define TCD_DLYTRIG_1_bm TCD_DLYTRIG1_bm
  #elif defined(TCD_DLYTRIG_1_bm)
    //deprecated_constant_name TCD_DLYTRIG1_bm = TCD_DLYTRIG_1_bm;
    #define TCD_DLYTRIG1_bm TCD_DLYTRIG_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYTRIG_1_bp) && defined(TCD_DLYTRIG1_bp)
    #define TCD_DLYTRIG_1_bp TCD_DLYTRIG1_bp
  #elif defined(TCD_DLYTRIG_1_bp)
    //deprecated_constant_name TCD_DLYTRIG1_bp = TCD_DLYTRIG_1_bp;
    #define TCD_DLYTRIG1_bp TCD_DLYTRIG_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCD_DLYPRESC_0_bm) && defined(TCD_DLYPRESC0_bm)
    #define TCD_DLYPRESC_0_bm TCD_DLYPRESC0_bm
  #elif defined(TCD_DLYPRESC_0_bm)
    //deprecated_constant_name TCD_DLYPRESC0_bm = TCD_DLYPRESC_0_bm;
    #define TCD_DLYPRESC0_bm TCD_DLYPRESC_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYPRESC_0_bp) && defined(TCD_DLYPRESC0_bp)
    #define TCD_DLYPRESC_0_bp TCD_DLYPRESC0_bp
  #elif defined(TCD_DLYPRESC_0_bp)
    //deprecated_constant_name TCD_DLYPRESC0_bp = TCD_DLYPRESC_0_bp;
    #define TCD_DLYPRESC0_bp TCD_DLYPRESC_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYPRESC_1_bm) && defined(TCD_DLYPRESC1_bm)
    #define TCD_DLYPRESC_1_bm TCD_DLYPRESC1_bm
  #elif defined(TCD_DLYPRESC_1_bm)
    //deprecated_constant_name TCD_DLYPRESC1_bm = TCD_DLYPRESC_1_bm;
    #define TCD_DLYPRESC1_bm TCD_DLYPRESC_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYPRESC_1_bp) && defined(TCD_DLYPRESC1_bp)
    #define TCD_DLYPRESC_1_bp TCD_DLYPRESC1_bp
  #elif defined(TCD_DLYPRESC_1_bp)
    //deprecated_constant_name TCD_DLYPRESC1_bp = TCD_DLYPRESC_1_bp;
    #define TCD_DLYPRESC1_bp TCD_DLYPRESC_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCD_DLYVAL_0_bm) && defined(TCD_DLYVAL0_bm)
    #define TCD_DLYVAL_0_bm TCD_DLYVAL0_bm
  #elif defined(TCD_DLYVAL_0_bm)
    //deprecated_constant_name TCD_DLYVAL0_bm = TCD_DLYVAL_0_bm;
    #define TCD_DLYVAL0_bm TCD_DLYVAL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_0_bp) && defined(TCD_DLYVAL0_bp)
    #define TCD_DLYVAL_0_bp TCD_DLYVAL0_bp
  #elif defined(TCD_DLYVAL_0_bp)
    //deprecated_constant_name TCD_DLYVAL0_bp = TCD_DLYVAL_0_bp;
    #define TCD_DLYVAL0_bp TCD_DLYVAL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_1_bm) && defined(TCD_DLYVAL1_bm)
    #define TCD_DLYVAL_1_bm TCD_DLYVAL1_bm
  #elif defined(TCD_DLYVAL_1_bm)
    //deprecated_constant_name TCD_DLYVAL1_bm = TCD_DLYVAL_1_bm;
    #define TCD_DLYVAL1_bm TCD_DLYVAL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_1_bp) && defined(TCD_DLYVAL1_bp)
    #define TCD_DLYVAL_1_bp TCD_DLYVAL1_bp
  #elif defined(TCD_DLYVAL_1_bp)
    //deprecated_constant_name TCD_DLYVAL1_bp = TCD_DLYVAL_1_bp;
    #define TCD_DLYVAL1_bp TCD_DLYVAL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_2_bm) && defined(TCD_DLYVAL2_bm)
    #define TCD_DLYVAL_2_bm TCD_DLYVAL2_bm
  #elif defined(TCD_DLYVAL_2_bm)
    //deprecated_constant_name TCD_DLYVAL2_bm = TCD_DLYVAL_2_bm;
    #define TCD_DLYVAL2_bm TCD_DLYVAL_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_2_bp) && defined(TCD_DLYVAL2_bp)
    #define TCD_DLYVAL_2_bp TCD_DLYVAL2_bp
  #elif defined(TCD_DLYVAL_2_bp)
    //deprecated_constant_name TCD_DLYVAL2_bp = TCD_DLYVAL_2_bp;
    #define TCD_DLYVAL2_bp TCD_DLYVAL_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_3_bm) && defined(TCD_DLYVAL3_bm)
    #define TCD_DLYVAL_3_bm TCD_DLYVAL3_bm
  #elif defined(TCD_DLYVAL_3_bm)
    //deprecated_constant_name TCD_DLYVAL3_bm = TCD_DLYVAL_3_bm;
    #define TCD_DLYVAL3_bm TCD_DLYVAL_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_3_bp) && defined(TCD_DLYVAL3_bp)
    #define TCD_DLYVAL_3_bp TCD_DLYVAL3_bp
  #elif defined(TCD_DLYVAL_3_bp)
    //deprecated_constant_name TCD_DLYVAL3_bp = TCD_DLYVAL_3_bp;
    #define TCD_DLYVAL3_bp TCD_DLYVAL_3_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_4_bm) && defined(TCD_DLYVAL4_bm)
    #define TCD_DLYVAL_4_bm TCD_DLYVAL4_bm
  #elif defined(TCD_DLYVAL_4_bm)
    //deprecated_constant_name TCD_DLYVAL4_bm = TCD_DLYVAL_4_bm;
    #define TCD_DLYVAL4_bm TCD_DLYVAL_4_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_4_bp) && defined(TCD_DLYVAL4_bp)
    #define TCD_DLYVAL_4_bp TCD_DLYVAL4_bp
  #elif defined(TCD_DLYVAL_4_bp)
    //deprecated_constant_name TCD_DLYVAL4_bp = TCD_DLYVAL_4_bp;
    #define TCD_DLYVAL4_bp TCD_DLYVAL_4_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_5_bm) && defined(TCD_DLYVAL5_bm)
    #define TCD_DLYVAL_5_bm TCD_DLYVAL5_bm
  #elif defined(TCD_DLYVAL_5_bm)
    //deprecated_constant_name TCD_DLYVAL5_bm = TCD_DLYVAL_5_bm;
    #define TCD_DLYVAL5_bm TCD_DLYVAL_5_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_5_bp) && defined(TCD_DLYVAL5_bp)
    #define TCD_DLYVAL_5_bp TCD_DLYVAL5_bp
  #elif defined(TCD_DLYVAL_5_bp)
    //deprecated_constant_name TCD_DLYVAL5_bp = TCD_DLYVAL_5_bp;
    #define TCD_DLYVAL5_bp TCD_DLYVAL_5_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_6_bm) && defined(TCD_DLYVAL6_bm)
    #define TCD_DLYVAL_6_bm TCD_DLYVAL6_bm
  #elif defined(TCD_DLYVAL_6_bm)
    //deprecated_constant_name TCD_DLYVAL6_bm = TCD_DLYVAL_6_bm;
    #define TCD_DLYVAL6_bm TCD_DLYVAL_6_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_6_bp) && defined(TCD_DLYVAL6_bp)
    #define TCD_DLYVAL_6_bp TCD_DLYVAL6_bp
  #elif defined(TCD_DLYVAL_6_bp)
    //deprecated_constant_name TCD_DLYVAL6_bp = TCD_DLYVAL_6_bp;
    #define TCD_DLYVAL6_bp TCD_DLYVAL_6_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_7_bm) && defined(TCD_DLYVAL7_bm)
    #define TCD_DLYVAL_7_bm TCD_DLYVAL7_bm
  #elif defined(TCD_DLYVAL_7_bm)
    //deprecated_constant_name TCD_DLYVAL7_bm = TCD_DLYVAL_7_bm;
    #define TCD_DLYVAL7_bm TCD_DLYVAL_7_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DLYVAL_7_bp) && defined(TCD_DLYVAL7_bp)
    #define TCD_DLYVAL_7_bp TCD_DLYVAL7_bp
  #elif defined(TCD_DLYVAL_7_bp)
    //deprecated_constant_name TCD_DLYVAL7_bp = TCD_DLYVAL_7_bp;
    #define TCD_DLYVAL7_bp TCD_DLYVAL_7_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCD_DITHERSEL_0_bm) && defined(TCD_DITHERSEL0_bm)
    #define TCD_DITHERSEL_0_bm TCD_DITHERSEL0_bm
  #elif defined(TCD_DITHERSEL_0_bm)
    //deprecated_constant_name TCD_DITHERSEL0_bm = TCD_DITHERSEL_0_bm;
    #define TCD_DITHERSEL0_bm TCD_DITHERSEL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DITHERSEL_0_bp) && defined(TCD_DITHERSEL0_bp)
    #define TCD_DITHERSEL_0_bp TCD_DITHERSEL0_bp
  #elif defined(TCD_DITHERSEL_0_bp)
    //deprecated_constant_name TCD_DITHERSEL0_bp = TCD_DITHERSEL_0_bp;
    #define TCD_DITHERSEL0_bp TCD_DITHERSEL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DITHERSEL_1_bm) && defined(TCD_DITHERSEL1_bm)
    #define TCD_DITHERSEL_1_bm TCD_DITHERSEL1_bm
  #elif defined(TCD_DITHERSEL_1_bm)
    //deprecated_constant_name TCD_DITHERSEL1_bm = TCD_DITHERSEL_1_bm;
    #define TCD_DITHERSEL1_bm TCD_DITHERSEL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DITHERSEL_1_bp) && defined(TCD_DITHERSEL1_bp)
    #define TCD_DITHERSEL_1_bp TCD_DITHERSEL1_bp
  #elif defined(TCD_DITHERSEL_1_bp)
    //deprecated_constant_name TCD_DITHERSEL1_bp = TCD_DITHERSEL_1_bp;
    #define TCD_DITHERSEL1_bp TCD_DITHERSEL_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TCD_DITHER_0_bm) && defined(TCD_DITHER0_bm)
    #define TCD_DITHER_0_bm TCD_DITHER0_bm
  #elif defined(TCD_DITHER_0_bm)
    //deprecated_constant_name TCD_DITHER0_bm = TCD_DITHER_0_bm;
    #define TCD_DITHER0_bm TCD_DITHER_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DITHER_0_bp) && defined(TCD_DITHER0_bp)
    #define TCD_DITHER_0_bp TCD_DITHER0_bp
  #elif defined(TCD_DITHER_0_bp)
    //deprecated_constant_name TCD_DITHER0_bp = TCD_DITHER_0_bp;
    #define TCD_DITHER0_bp TCD_DITHER_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DITHER_1_bm) && defined(TCD_DITHER1_bm)
    #define TCD_DITHER_1_bm TCD_DITHER1_bm
  #elif defined(TCD_DITHER_1_bm)
    //deprecated_constant_name TCD_DITHER1_bm = TCD_DITHER_1_bm;
    #define TCD_DITHER1_bm TCD_DITHER_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DITHER_1_bp) && defined(TCD_DITHER1_bp)
    #define TCD_DITHER_1_bp TCD_DITHER1_bp
  #elif defined(TCD_DITHER_1_bp)
    //deprecated_constant_name TCD_DITHER1_bp = TCD_DITHER_1_bp;
    #define TCD_DITHER1_bp TCD_DITHER_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DITHER_2_bm) && defined(TCD_DITHER2_bm)
    #define TCD_DITHER_2_bm TCD_DITHER2_bm
  #elif defined(TCD_DITHER_2_bm)
    //deprecated_constant_name TCD_DITHER2_bm = TCD_DITHER_2_bm;
    #define TCD_DITHER2_bm TCD_DITHER_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DITHER_2_bp) && defined(TCD_DITHER2_bp)
    #define TCD_DITHER_2_bp TCD_DITHER2_bp
  #elif defined(TCD_DITHER_2_bp)
    //deprecated_constant_name TCD_DITHER2_bp = TCD_DITHER_2_bp;
    #define TCD_DITHER2_bp TCD_DITHER_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DITHER_3_bm) && defined(TCD_DITHER3_bm)
    #define TCD_DITHER_3_bm TCD_DITHER3_bm
  #elif defined(TCD_DITHER_3_bm)
    //deprecated_constant_name TCD_DITHER3_bm = TCD_DITHER_3_bm;
    #define TCD_DITHER3_bm TCD_DITHER_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TCD_DITHER_3_bp) && defined(TCD_DITHER3_bp)
    #define TCD_DITHER_3_bp TCD_DITHER3_bp
  #elif defined(TCD_DITHER_3_bp)
    //deprecated_constant_name TCD_DITHER3_bp = TCD_DITHER_3_bp;
    #define TCD_DITHER3_bp TCD_DITHER_3_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= TWI ======= */
  #if !defined(TWI_SDAHOLD_0_bm) && defined(TWI_SDAHOLD0_bm)
    #define TWI_SDAHOLD_0_bm TWI_SDAHOLD0_bm
  #elif defined(TWI_SDAHOLD_0_bm)
    //deprecated_constant_name TWI_SDAHOLD0_bm = TWI_SDAHOLD_0_bm;
    #define TWI_SDAHOLD0_bm TWI_SDAHOLD_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_SDAHOLD_0_bp) && defined(TWI_SDAHOLD0_bp)
    #define TWI_SDAHOLD_0_bp TWI_SDAHOLD0_bp
  #elif defined(TWI_SDAHOLD_0_bp)
    //deprecated_constant_name TWI_SDAHOLD0_bp = TWI_SDAHOLD_0_bp;
    #define TWI_SDAHOLD0_bp TWI_SDAHOLD_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_SDAHOLD_1_bm) && defined(TWI_SDAHOLD1_bm)
    #define TWI_SDAHOLD_1_bm TWI_SDAHOLD1_bm
  #elif defined(TWI_SDAHOLD_1_bm)
    //deprecated_constant_name TWI_SDAHOLD1_bm = TWI_SDAHOLD_1_bm;
    #define TWI_SDAHOLD1_bm TWI_SDAHOLD_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_SDAHOLD_1_bp) && defined(TWI_SDAHOLD1_bp)
    #define TWI_SDAHOLD_1_bp TWI_SDAHOLD1_bp
  #elif defined(TWI_SDAHOLD_1_bp)
    //deprecated_constant_name TWI_SDAHOLD1_bp = TWI_SDAHOLD_1_bp;
    #define TWI_SDAHOLD1_bp TWI_SDAHOLD_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TWI_TIMEOUT_0_bm) && defined(TWI_TIMEOUT0_bm)
    #define TWI_TIMEOUT_0_bm TWI_TIMEOUT0_bm
  #elif defined(TWI_TIMEOUT_0_bm)
    //deprecated_constant_name TWI_TIMEOUT0_bm = TWI_TIMEOUT_0_bm;
    #define TWI_TIMEOUT0_bm TWI_TIMEOUT_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_TIMEOUT_0_bp) && defined(TWI_TIMEOUT0_bp)
    #define TWI_TIMEOUT_0_bp TWI_TIMEOUT0_bp
  #elif defined(TWI_TIMEOUT_0_bp)
    //deprecated_constant_name TWI_TIMEOUT0_bp = TWI_TIMEOUT_0_bp;
    #define TWI_TIMEOUT0_bp TWI_TIMEOUT_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_TIMEOUT_1_bm) && defined(TWI_TIMEOUT1_bm)
    #define TWI_TIMEOUT_1_bm TWI_TIMEOUT1_bm
  #elif defined(TWI_TIMEOUT_1_bm)
    //deprecated_constant_name TWI_TIMEOUT1_bm = TWI_TIMEOUT_1_bm;
    #define TWI_TIMEOUT1_bm TWI_TIMEOUT_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_TIMEOUT_1_bp) && defined(TWI_TIMEOUT1_bp)
    #define TWI_TIMEOUT_1_bp TWI_TIMEOUT1_bp
  #elif defined(TWI_TIMEOUT_1_bp)
    //deprecated_constant_name TWI_TIMEOUT1_bp = TWI_TIMEOUT_1_bp;
    #define TWI_TIMEOUT1_bp TWI_TIMEOUT_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TWI_MCMD_0_bm) && defined(TWI_MCMD0_bm)
    #define TWI_MCMD_0_bm TWI_MCMD0_bm
  #elif defined(TWI_MCMD_0_bm)
    //deprecated_constant_name TWI_MCMD0_bm = TWI_MCMD_0_bm;
    #define TWI_MCMD0_bm TWI_MCMD_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_MCMD_0_bp) && defined(TWI_MCMD0_bp)
    #define TWI_MCMD_0_bp TWI_MCMD0_bp
  #elif defined(TWI_MCMD_0_bp)
    //deprecated_constant_name TWI_MCMD0_bp = TWI_MCMD_0_bp;
    #define TWI_MCMD0_bp TWI_MCMD_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_MCMD_1_bm) && defined(TWI_MCMD1_bm)
    #define TWI_MCMD_1_bm TWI_MCMD1_bm
  #elif defined(TWI_MCMD_1_bm)
    //deprecated_constant_name TWI_MCMD1_bm = TWI_MCMD_1_bm;
    #define TWI_MCMD1_bm TWI_MCMD_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_MCMD_1_bp) && defined(TWI_MCMD1_bp)
    #define TWI_MCMD_1_bp TWI_MCMD1_bp
  #elif defined(TWI_MCMD_1_bp)
    //deprecated_constant_name TWI_MCMD1_bp = TWI_MCMD_1_bp;
    #define TWI_MCMD1_bp TWI_MCMD_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TWI_BUSSTATE_0_bm) && defined(TWI_BUSSTATE0_bm)
    #define TWI_BUSSTATE_0_bm TWI_BUSSTATE0_bm
  #elif defined(TWI_BUSSTATE_0_bm)
    //deprecated_constant_name TWI_BUSSTATE0_bm = TWI_BUSSTATE_0_bm;
    #define TWI_BUSSTATE0_bm TWI_BUSSTATE_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_BUSSTATE_0_bp) && defined(TWI_BUSSTATE0_bp)
    #define TWI_BUSSTATE_0_bp TWI_BUSSTATE0_bp
  #elif defined(TWI_BUSSTATE_0_bp)
    //deprecated_constant_name TWI_BUSSTATE0_bp = TWI_BUSSTATE_0_bp;
    #define TWI_BUSSTATE0_bp TWI_BUSSTATE_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_BUSSTATE_1_bm) && defined(TWI_BUSSTATE1_bm)
    #define TWI_BUSSTATE_1_bm TWI_BUSSTATE1_bm
  #elif defined(TWI_BUSSTATE_1_bm)
    //deprecated_constant_name TWI_BUSSTATE1_bm = TWI_BUSSTATE_1_bm;
    #define TWI_BUSSTATE1_bm TWI_BUSSTATE_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_BUSSTATE_1_bp) && defined(TWI_BUSSTATE1_bp)
    #define TWI_BUSSTATE_1_bp TWI_BUSSTATE1_bp
  #elif defined(TWI_BUSSTATE_1_bp)
    //deprecated_constant_name TWI_BUSSTATE1_bp = TWI_BUSSTATE_1_bp;
    #define TWI_BUSSTATE1_bp TWI_BUSSTATE_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TWI_SCMD_0_bm) && defined(TWI_SCMD0_bm)
    #define TWI_SCMD_0_bm TWI_SCMD0_bm
  #elif defined(TWI_SCMD_0_bm)
    //deprecated_constant_name TWI_SCMD0_bm = TWI_SCMD_0_bm;
    #define TWI_SCMD0_bm TWI_SCMD_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_SCMD_0_bp) && defined(TWI_SCMD0_bp)
    #define TWI_SCMD_0_bp TWI_SCMD0_bp
  #elif defined(TWI_SCMD_0_bp)
    //deprecated_constant_name TWI_SCMD0_bp = TWI_SCMD_0_bp;
    #define TWI_SCMD0_bp TWI_SCMD_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_SCMD_1_bm) && defined(TWI_SCMD1_bm)
    #define TWI_SCMD_1_bm TWI_SCMD1_bm
  #elif defined(TWI_SCMD_1_bm)
    //deprecated_constant_name TWI_SCMD1_bm = TWI_SCMD_1_bm;
    #define TWI_SCMD1_bm TWI_SCMD_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_SCMD_1_bp) && defined(TWI_SCMD1_bp)
    #define TWI_SCMD_1_bp TWI_SCMD1_bp
  #elif defined(TWI_SCMD_1_bp)
    //deprecated_constant_name TWI_SCMD1_bp = TWI_SCMD_1_bp;
    #define TWI_SCMD1_bp TWI_SCMD_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(TWI_ADDRMASK_0_bm) && defined(TWI_ADDRMASK0_bm)
    #define TWI_ADDRMASK_0_bm TWI_ADDRMASK0_bm
  #elif defined(TWI_ADDRMASK_0_bm)
    //deprecated_constant_name TWI_ADDRMASK0_bm = TWI_ADDRMASK_0_bm;
    #define TWI_ADDRMASK0_bm TWI_ADDRMASK_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_ADDRMASK_0_bp) && defined(TWI_ADDRMASK0_bp)
    #define TWI_ADDRMASK_0_bp TWI_ADDRMASK0_bp
  #elif defined(TWI_ADDRMASK_0_bp)
    //deprecated_constant_name TWI_ADDRMASK0_bp = TWI_ADDRMASK_0_bp;
    #define TWI_ADDRMASK0_bp TWI_ADDRMASK_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_ADDRMASK_1_bm) && defined(TWI_ADDRMASK1_bm)
    #define TWI_ADDRMASK_1_bm TWI_ADDRMASK1_bm
  #elif defined(TWI_ADDRMASK_1_bm)
    //deprecated_constant_name TWI_ADDRMASK1_bm = TWI_ADDRMASK_1_bm;
    #define TWI_ADDRMASK1_bm TWI_ADDRMASK_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_ADDRMASK_1_bp) && defined(TWI_ADDRMASK1_bp)
    #define TWI_ADDRMASK_1_bp TWI_ADDRMASK1_bp
  #elif defined(TWI_ADDRMASK_1_bp)
    //deprecated_constant_name TWI_ADDRMASK1_bp = TWI_ADDRMASK_1_bp;
    #define TWI_ADDRMASK1_bp TWI_ADDRMASK_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_ADDRMASK_2_bm) && defined(TWI_ADDRMASK2_bm)
    #define TWI_ADDRMASK_2_bm TWI_ADDRMASK2_bm
  #elif defined(TWI_ADDRMASK_2_bm)
    //deprecated_constant_name TWI_ADDRMASK2_bm = TWI_ADDRMASK_2_bm;
    #define TWI_ADDRMASK2_bm TWI_ADDRMASK_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_ADDRMASK_2_bp) && defined(TWI_ADDRMASK2_bp)
    #define TWI_ADDRMASK_2_bp TWI_ADDRMASK2_bp
  #elif defined(TWI_ADDRMASK_2_bp)
    //deprecated_constant_name TWI_ADDRMASK2_bp = TWI_ADDRMASK_2_bp;
    #define TWI_ADDRMASK2_bp TWI_ADDRMASK_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_ADDRMASK_3_bm) && defined(TWI_ADDRMASK3_bm)
    #define TWI_ADDRMASK_3_bm TWI_ADDRMASK3_bm
  #elif defined(TWI_ADDRMASK_3_bm)
    //deprecated_constant_name TWI_ADDRMASK3_bm = TWI_ADDRMASK_3_bm;
    #define TWI_ADDRMASK3_bm TWI_ADDRMASK_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_ADDRMASK_3_bp) && defined(TWI_ADDRMASK3_bp)
    #define TWI_ADDRMASK_3_bp TWI_ADDRMASK3_bp
  #elif defined(TWI_ADDRMASK_3_bp)
    //deprecated_constant_name TWI_ADDRMASK3_bp = TWI_ADDRMASK_3_bp;
    #define TWI_ADDRMASK3_bp TWI_ADDRMASK_3_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_ADDRMASK_4_bm) && defined(TWI_ADDRMASK4_bm)
    #define TWI_ADDRMASK_4_bm TWI_ADDRMASK4_bm
  #elif defined(TWI_ADDRMASK_4_bm)
    //deprecated_constant_name TWI_ADDRMASK4_bm = TWI_ADDRMASK_4_bm;
    #define TWI_ADDRMASK4_bm TWI_ADDRMASK_4_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_ADDRMASK_4_bp) && defined(TWI_ADDRMASK4_bp)
    #define TWI_ADDRMASK_4_bp TWI_ADDRMASK4_bp
  #elif defined(TWI_ADDRMASK_4_bp)
    //deprecated_constant_name TWI_ADDRMASK4_bp = TWI_ADDRMASK_4_bp;
    #define TWI_ADDRMASK4_bp TWI_ADDRMASK_4_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_ADDRMASK_5_bm) && defined(TWI_ADDRMASK5_bm)
    #define TWI_ADDRMASK_5_bm TWI_ADDRMASK5_bm
  #elif defined(TWI_ADDRMASK_5_bm)
    //deprecated_constant_name TWI_ADDRMASK5_bm = TWI_ADDRMASK_5_bm;
    #define TWI_ADDRMASK5_bm TWI_ADDRMASK_5_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_ADDRMASK_5_bp) && defined(TWI_ADDRMASK5_bp)
    #define TWI_ADDRMASK_5_bp TWI_ADDRMASK5_bp
  #elif defined(TWI_ADDRMASK_5_bp)
    //deprecated_constant_name TWI_ADDRMASK5_bp = TWI_ADDRMASK_5_bp;
    #define TWI_ADDRMASK5_bp TWI_ADDRMASK_5_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_ADDRMASK_6_bm) && defined(TWI_ADDRMASK6_bm)
    #define TWI_ADDRMASK_6_bm TWI_ADDRMASK6_bm
  #elif defined(TWI_ADDRMASK_6_bm)
    //deprecated_constant_name TWI_ADDRMASK6_bm = TWI_ADDRMASK_6_bm;
    #define TWI_ADDRMASK6_bm TWI_ADDRMASK_6_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(TWI_ADDRMASK_6_bp) && defined(TWI_ADDRMASK6_bp)
    #define TWI_ADDRMASK_6_bp TWI_ADDRMASK6_bp
  #elif defined(TWI_ADDRMASK_6_bp)
    //deprecated_constant_name TWI_ADDRMASK6_bp = TWI_ADDRMASK_6_bp;
    #define TWI_ADDRMASK6_bp TWI_ADDRMASK_6_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= USART ======= */
  #if !defined(USART_RXMODE_0_bm) && defined(USART_RXMODE0_bm)
    #define USART_RXMODE_0_bm USART_RXMODE0_bm
  #elif defined(USART_RXMODE_0_bm)
    //deprecated_constant_name USART_RXMODE0_bm = USART_RXMODE_0_bm;
    #define USART_RXMODE0_bm USART_RXMODE_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXMODE_0_bp) && defined(USART_RXMODE0_bp)
    #define USART_RXMODE_0_bp USART_RXMODE0_bp
  #elif defined(USART_RXMODE_0_bp)
    //deprecated_constant_name USART_RXMODE0_bp = USART_RXMODE_0_bp;
    #define USART_RXMODE0_bp USART_RXMODE_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXMODE_1_bm) && defined(USART_RXMODE1_bm)
    #define USART_RXMODE_1_bm USART_RXMODE1_bm
  #elif defined(USART_RXMODE_1_bm)
    //deprecated_constant_name USART_RXMODE1_bm = USART_RXMODE_1_bm;
    #define USART_RXMODE1_bm USART_RXMODE_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXMODE_1_bp) && defined(USART_RXMODE1_bp)
    #define USART_RXMODE_1_bp USART_RXMODE1_bp
  #elif defined(USART_RXMODE_1_bp)
    //deprecated_constant_name USART_RXMODE1_bp = USART_RXMODE_1_bp;
    #define USART_RXMODE1_bp USART_RXMODE_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(USART_CHSIZE_0_bm) && defined(USART_CHSIZE0_bm)
    #define USART_CHSIZE_0_bm USART_CHSIZE0_bm
  #elif defined(USART_CHSIZE_0_bm)
    //deprecated_constant_name USART_CHSIZE0_bm = USART_CHSIZE_0_bm;
    #define USART_CHSIZE0_bm USART_CHSIZE_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_CHSIZE_0_bp) && defined(USART_CHSIZE0_bp)
    #define USART_CHSIZE_0_bp USART_CHSIZE0_bp
  #elif defined(USART_CHSIZE_0_bp)
    //deprecated_constant_name USART_CHSIZE0_bp = USART_CHSIZE_0_bp;
    #define USART_CHSIZE0_bp USART_CHSIZE_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_CHSIZE_1_bm) && defined(USART_CHSIZE1_bm)
    #define USART_CHSIZE_1_bm USART_CHSIZE1_bm
  #elif defined(USART_CHSIZE_1_bm)
    //deprecated_constant_name USART_CHSIZE1_bm = USART_CHSIZE_1_bm;
    #define USART_CHSIZE1_bm USART_CHSIZE_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_CHSIZE_1_bp) && defined(USART_CHSIZE1_bp)
    #define USART_CHSIZE_1_bp USART_CHSIZE1_bp
  #elif defined(USART_CHSIZE_1_bp)
    //deprecated_constant_name USART_CHSIZE1_bp = USART_CHSIZE_1_bp;
    #define USART_CHSIZE1_bp USART_CHSIZE_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_CHSIZE_2_bm) && defined(USART_CHSIZE2_bm)
    #define USART_CHSIZE_2_bm USART_CHSIZE2_bm
  #elif defined(USART_CHSIZE_2_bm)
    //deprecated_constant_name USART_CHSIZE2_bm = USART_CHSIZE_2_bm;
    #define USART_CHSIZE2_bm USART_CHSIZE_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_CHSIZE_2_bp) && defined(USART_CHSIZE2_bp)
    #define USART_CHSIZE_2_bp USART_CHSIZE2_bp
  #elif defined(USART_CHSIZE_2_bp)
    //deprecated_constant_name USART_CHSIZE2_bp = USART_CHSIZE_2_bp;
    #define USART_CHSIZE2_bp USART_CHSIZE_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_PMODE_0_bm) && defined(USART_PMODE0_bm)
    #define USART_PMODE_0_bm USART_PMODE0_bm
  #elif defined(USART_PMODE_0_bm)
    //deprecated_constant_name USART_PMODE0_bm = USART_PMODE_0_bm;
    #define USART_PMODE0_bm USART_PMODE_0_bm //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(USART_PMODE_0_bp) && defined(USART_PMODE0_bp)
    #define USART_PMODE_0_bp USART_PMODE0_bp
  #elif defined(USART_PMODE_0_bp)
    //deprecated_constant_name USART_PMODE0_bp = USART_PMODE_0_bp;
    #define USART_PMODE0_bp USART_PMODE_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_PMODE_1_bm) && defined(USART_PMODE1_bm)
    #define USART_PMODE_1_bm USART_PMODE1_bm
  #elif defined(USART_PMODE_1_bm)
    //deprecated_constant_name USART_PMODE1_bm = USART_PMODE_1_bm;
    #define USART_PMODE1_bm USART_PMODE_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_PMODE_1_bp) && defined(USART_PMODE1_bp)
    #define USART_PMODE_1_bp USART_PMODE1_bp
  #elif defined(USART_PMODE_1_bp)
    //deprecated_constant_name USART_PMODE1_bp = USART_PMODE_1_bp;
    #define USART_PMODE1_bp USART_PMODE_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(USART_CMODE_0_bm) && defined(USART_CMODE0_bm)
    #define USART_CMODE_0_bm USART_CMODE0_bm
  #elif defined(USART_CMODE_0_bm)
    //deprecated_constant_name USART_CMODE0_bm = USART_CMODE_0_bm;
    #define USART_CMODE0_bm USART_CMODE_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_CMODE_0_bp) && defined(USART_CMODE0_bp)
    #define USART_CMODE_0_bp USART_CMODE0_bp
  #elif defined(USART_CMODE_0_bp)
    //deprecated_constant_name USART_CMODE0_bp = USART_CMODE_0_bp;
    #define USART_CMODE0_bp USART_CMODE_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_CMODE_1_bm) && defined(USART_CMODE1_bm)
    #define USART_CMODE_1_bm USART_CMODE1_bm
  #elif defined(USART_CMODE_1_bm)
    //deprecated_constant_name USART_CMODE1_bm = USART_CMODE_1_bm;
    #define USART_CMODE1_bm USART_CMODE_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_CMODE_1_bp) && defined(USART_CMODE1_bp)
    #define USART_CMODE_1_bp USART_CMODE1_bp
  #elif defined(USART_CMODE_1_bp)
    //deprecated_constant_name USART_CMODE1_bp = USART_CMODE_1_bp;
    #define USART_CMODE1_bp USART_CMODE_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(USART_ABW_0_bm) && defined(USART_ABW0_bm)
    #define USART_ABW_0_bm USART_ABW0_bm
  #elif defined(USART_ABW_0_bm)
    //deprecated_constant_name USART_ABW0_bm = USART_ABW_0_bm;
    #define USART_ABW0_bm USART_ABW_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_ABW_0_bp) && defined(USART_ABW0_bp)
    #define USART_ABW_0_bp USART_ABW0_bp
  #elif defined(USART_ABW_0_bp)
    //deprecated_constant_name USART_ABW0_bp = USART_ABW_0_bp;
    #define USART_ABW0_bp USART_ABW_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_ABW_1_bm) && defined(USART_ABW1_bm)
    #define USART_ABW_1_bm USART_ABW1_bm
  #elif defined(USART_ABW_1_bm)
    //deprecated_constant_name USART_ABW1_bm = USART_ABW_1_bm;
    #define USART_ABW1_bm USART_ABW_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_ABW_1_bp) && defined(USART_ABW1_bp)
    #define USART_ABW_1_bp USART_ABW1_bp
  #elif defined(USART_ABW_1_bp)
    //deprecated_constant_name USART_ABW1_bp = USART_ABW_1_bp;
    #define USART_ABW1_bp USART_ABW_1_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(USART_TXPL_0_bm) && defined(USART_TXPL0_bm)
    #define USART_TXPL_0_bm USART_TXPL0_bm
  #elif defined(USART_TXPL_0_bm)
    //deprecated_constant_name USART_TXPL0_bm = USART_TXPL_0_bm;
    #define USART_TXPL0_bm USART_TXPL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_0_bp) && defined(USART_TXPL0_bp)
    #define USART_TXPL_0_bp USART_TXPL0_bp
  #elif defined(USART_TXPL_0_bp)
    //deprecated_constant_name USART_TXPL0_bp = USART_TXPL_0_bp;
    #define USART_TXPL0_bp USART_TXPL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_1_bm) && defined(USART_TXPL1_bm)
    #define USART_TXPL_1_bm USART_TXPL1_bm
  #elif defined(USART_TXPL_1_bm)
    //deprecated_constant_name USART_TXPL1_bm = USART_TXPL_1_bm;
    #define USART_TXPL1_bm USART_TXPL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_1_bp) && defined(USART_TXPL1_bp)
    #define USART_TXPL_1_bp USART_TXPL1_bp
  #elif defined(USART_TXPL_1_bp)
    //deprecated_constant_name USART_TXPL1_bp = USART_TXPL_1_bp;
    #define USART_TXPL1_bp USART_TXPL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_2_bm) && defined(USART_TXPL2_bm)
    #define USART_TXPL_2_bm USART_TXPL2_bm
  #elif defined(USART_TXPL_2_bm)
    //deprecated_constant_name USART_TXPL2_bm = USART_TXPL_2_bm;
    #define USART_TXPL2_bm USART_TXPL_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_2_bp) && defined(USART_TXPL2_bp)
    #define USART_TXPL_2_bp USART_TXPL2_bp
  #elif defined(USART_TXPL_2_bp)
    //deprecated_constant_name USART_TXPL2_bp = USART_TXPL_2_bp;
    #define USART_TXPL2_bp USART_TXPL_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_3_bm) && defined(USART_TXPL3_bm)
    #define USART_TXPL_3_bm USART_TXPL3_bm
  #elif defined(USART_TXPL_3_bm)
    //deprecated_constant_name USART_TXPL3_bm = USART_TXPL_3_bm;
    #define USART_TXPL3_bm USART_TXPL_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_3_bp) && defined(USART_TXPL3_bp)
    #define USART_TXPL_3_bp USART_TXPL3_bp
  #elif defined(USART_TXPL_3_bp)
    //deprecated_constant_name USART_TXPL3_bp = USART_TXPL_3_bp;
    #define USART_TXPL3_bp USART_TXPL_3_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_4_bm) && defined(USART_TXPL4_bm)
    #define USART_TXPL_4_bm USART_TXPL4_bm
  #elif defined(USART_TXPL_4_bm)
    //deprecated_constant_name USART_TXPL4_bm = USART_TXPL_4_bm;
    #define USART_TXPL4_bm USART_TXPL_4_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_4_bp) && defined(USART_TXPL4_bp)
    #define USART_TXPL_4_bp USART_TXPL4_bp
  #elif defined(USART_TXPL_4_bp)
    //deprecated_constant_name USART_TXPL4_bp = USART_TXPL_4_bp;
    #define USART_TXPL4_bp USART_TXPL_4_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_5_bm) && defined(USART_TXPL5_bm)
    #define USART_TXPL_5_bm USART_TXPL5_bm
  #elif defined(USART_TXPL_5_bm)
    //deprecated_constant_name USART_TXPL5_bm = USART_TXPL_5_bm;
    #define USART_TXPL5_bm USART_TXPL_5_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_5_bp) && defined(USART_TXPL5_bp)
    #define USART_TXPL_5_bp USART_TXPL5_bp
  #elif defined(USART_TXPL_5_bp)
    //deprecated_constant_name USART_TXPL5_bp = USART_TXPL_5_bp;
    #define USART_TXPL5_bp USART_TXPL_5_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_6_bm) && defined(USART_TXPL6_bm)
    #define USART_TXPL_6_bm USART_TXPL6_bm
  #elif defined(USART_TXPL_6_bm)
    //deprecated_constant_name USART_TXPL6_bm = USART_TXPL_6_bm;
    #define USART_TXPL6_bm USART_TXPL_6_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_6_bp) && defined(USART_TXPL6_bp)
    #define USART_TXPL_6_bp USART_TXPL6_bp
  #elif defined(USART_TXPL_6_bp)
    //deprecated_constant_name USART_TXPL6_bp = USART_TXPL_6_bp;
    #define USART_TXPL6_bp USART_TXPL_6_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_7_bm) && defined(USART_TXPL7_bm)
    #define USART_TXPL_7_bm USART_TXPL7_bm
  #elif defined(USART_TXPL_7_bm)
    //deprecated_constant_name USART_TXPL7_bm = USART_TXPL_7_bm;
    #define USART_TXPL7_bm USART_TXPL_7_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_TXPL_7_bp) && defined(USART_TXPL7_bp)
    #define USART_TXPL_7_bp USART_TXPL7_bp
  #elif defined(USART_TXPL_7_bp)
    //deprecated_constant_name USART_TXPL7_bp = USART_TXPL_7_bp;
    #define USART_TXPL7_bp USART_TXPL_7_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(USART_RXPL_0_bm) && defined(USART_RXPL0_bm)
    #define USART_RXPL_0_bm USART_RXPL0_bm
  #elif defined(USART_RXPL_0_bm)
    //deprecated_constant_name USART_RXPL0_bm = USART_RXPL_0_bm;
    #define USART_RXPL0_bm USART_RXPL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXPL_0_bp) && defined(USART_RXPL0_bp)
    #define USART_RXPL_0_bp USART_RXPL0_bp
  #elif defined(USART_RXPL_0_bp)
    //deprecated_constant_name USART_RXPL0_bp = USART_RXPL_0_bp;
    #define USART_RXPL0_bp USART_RXPL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXPL_1_bm) && defined(USART_RXPL1_bm)
    #define USART_RXPL_1_bm USART_RXPL1_bm
  #elif defined(USART_RXPL_1_bm)
    //deprecated_constant_name USART_RXPL1_bm = USART_RXPL_1_bm;
    #define USART_RXPL1_bm USART_RXPL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXPL_1_bp) && defined(USART_RXPL1_bp)
    #define USART_RXPL_1_bp USART_RXPL1_bp
  #elif defined(USART_RXPL_1_bp)
    //deprecated_constant_name USART_RXPL1_bp = USART_RXPL_1_bp;
    #define USART_RXPL1_bp USART_RXPL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXPL_2_bm) && defined(USART_RXPL2_bm)
    #define USART_RXPL_2_bm USART_RXPL2_bm
  #elif defined(USART_RXPL_2_bm)
    //deprecated_constant_name USART_RXPL2_bm = USART_RXPL_2_bm;
    #define USART_RXPL2_bm USART_RXPL_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXPL_2_bp) && defined(USART_RXPL2_bp)
    #define USART_RXPL_2_bp USART_RXPL2_bp
  #elif defined(USART_RXPL_2_bp)
    //deprecated_constant_name USART_RXPL2_bp = USART_RXPL_2_bp;
    #define USART_RXPL2_bp USART_RXPL_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXPL_3_bm) && defined(USART_RXPL3_bm)
    #define USART_RXPL_3_bm USART_RXPL3_bm
  #elif defined(USART_RXPL_3_bm)
    //deprecated_constant_name USART_RXPL3_bm = USART_RXPL_3_bm;
    #define USART_RXPL3_bm USART_RXPL_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXPL_3_bp) && defined(USART_RXPL3_bp)
    #define USART_RXPL_3_bp USART_RXPL3_bp
  #elif defined(USART_RXPL_3_bp)
    //deprecated_constant_name USART_RXPL3_bp = USART_RXPL_3_bp;
    #define USART_RXPL3_bp USART_RXPL_3_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXPL_4_bm) && defined(USART_RXPL4_bm)
    #define USART_RXPL_4_bm USART_RXPL4_bm
  #elif defined(USART_RXPL_4_bm)
    //deprecated_constant_name USART_RXPL4_bm = USART_RXPL_4_bm;
    #define USART_RXPL4_bm USART_RXPL_4_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXPL_4_bp) && defined(USART_RXPL4_bp)
    #define USART_RXPL_4_bp USART_RXPL4_bp
  #elif defined(USART_RXPL_4_bp)
    //deprecated_constant_name USART_RXPL4_bp = USART_RXPL_4_bp;
    #define USART_RXPL4_bp USART_RXPL_4_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXPL_5_bm) && defined(USART_RXPL5_bm)
    #define USART_RXPL_5_bm USART_RXPL5_bm
  #elif defined(USART_RXPL_5_bm)
    //deprecated_constant_name USART_RXPL5_bm = USART_RXPL_5_bm;
    #define USART_RXPL5_bm USART_RXPL_5_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXPL_5_bp) && defined(USART_RXPL5_bp)
    #define USART_RXPL_5_bp USART_RXPL5_bp
  #elif defined(USART_RXPL_5_bp)
    //deprecated_constant_name USART_RXPL5_bp = USART_RXPL_5_bp;
    #define USART_RXPL5_bp USART_RXPL_5_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXPL_6_bm) && defined(USART_RXPL6_bm)
    #define USART_RXPL_6_bm USART_RXPL6_bm
  #elif defined(USART_RXPL_6_bm)
    //deprecated_constant_name USART_RXPL6_bm = USART_RXPL_6_bm;
    #define USART_RXPL6_bm USART_RXPL_6_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(USART_RXPL_6_bp) && defined(USART_RXPL6_bp)
    #define USART_RXPL_6_bp USART_RXPL6_bp
  #elif defined(USART_RXPL_6_bp)
    //deprecated_constant_name USART_RXPL6_bp = USART_RXPL_6_bp;
    #define USART_RXPL6_bp USART_RXPL_6_bp //Deprecated as of Q2 2022 header change.
  #endif

/* VPORT.INTFLAGS */
  #if !defined(VPORT_INT_0_bm) && defined(VPORT_INT0_bm)
    #define VPORT_INT_0_bm VPORT_INT0_bm
  #elif defined(VPORT_INT_0_bm)
    //deprecated_constant_name VPORT_INT0_bm = VPORT_INT_0_bm;
    #define VPORT_INT0_bm VPORT_INT_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_0_bp) && defined(VPORT_INT0_bp)
    #define VPORT_INT_0_bp VPORT_INT0_bp
  #elif defined(VPORT_INT_0_bp)
    //deprecated_constant_name VPORT_INT0_bp = VPORT_INT_0_bp;
    #define VPORT_INT0_bp VPORT_INT_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_1_bm) && defined(VPORT_INT1_bm)
    #define VPORT_INT_1_bm VPORT_INT1_bm
  #elif defined(VPORT_INT_1_bm)
    //deprecated_constant_name VPORT_INT1_bm = VPORT_INT_1_bm;
    #define VPORT_INT1_bm VPORT_INT_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_1_bp) && defined(VPORT_INT1_bp)
    #define VPORT_INT_1_bp VPORT_INT1_bp
  #elif defined(VPORT_INT_1_bp)
    //deprecated_constant_name VPORT_INT1_bp = VPORT_INT_1_bp;
    #define VPORT_INT1_bp VPORT_INT_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_2_bm) && defined(VPORT_INT2_bm)
    #define VPORT_INT_2_bm VPORT_INT2_bm
  #elif defined(VPORT_INT_2_bm)
    //deprecated_constant_name VPORT_INT2_bm = VPORT_INT_2_bm;
    #define VPORT_INT2_bm VPORT_INT_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_2_bp) && defined(VPORT_INT2_bp)
    #define VPORT_INT_2_bp VPORT_INT2_bp
  #elif defined(VPORT_INT_2_bp)
    //deprecated_constant_name VPORT_INT2_bp = VPORT_INT_2_bp;
    #define VPORT_INT2_bp VPORT_INT_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_3_bm) && defined(VPORT_INT3_bm)
    #define VPORT_INT_3_bm VPORT_INT3_bm
  #elif defined(VPORT_INT_3_bm)
    //deprecated_constant_name VPORT_INT3_bm = VPORT_INT_3_bm;
    #define VPORT_INT3_bm VPORT_INT_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_3_bp) && defined(VPORT_INT3_bp)
    #define VPORT_INT_3_bp VPORT_INT3_bp
  #elif defined(VPORT_INT_3_bp)
    //deprecated_constant_name VPORT_INT3_bp = VPORT_INT_3_bp;
    #define VPORT_INT3_bp VPORT_INT_3_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_4_bm) && defined(VPORT_INT4_bm)
    #define VPORT_INT_4_bm VPORT_INT4_bm
  #elif defined(VPORT_INT_4_bm)
    //deprecated_constant_name VPORT_INT4_bm = VPORT_INT_4_bm;
    #define VPORT_INT4_bm VPORT_INT_4_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_4_bp) && defined(VPORT_INT4_bp)
    #define VPORT_INT_4_bp VPORT_INT4_bp
  #elif defined(VPORT_INT_4_bp)
    //deprecated_constant_name VPORT_INT4_bp = VPORT_INT_4_bp;
    #define VPORT_INT4_bp VPORT_INT_4_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_5_bm) && defined(VPORT_INT5_bm)
    #define VPORT_INT_5_bm VPORT_INT5_bm
  #elif defined(VPORT_INT_5_bm)
    //deprecated_constant_name VPORT_INT5_bm = VPORT_INT_5_bm;
    #define VPORT_INT5_bm VPORT_INT_5_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_5_bp) && defined(VPORT_INT5_bp)
    #define VPORT_INT_5_bp VPORT_INT5_bp
  #elif defined(VPORT_INT_5_bp)
    //deprecated_constant_name VPORT_INT5_bp = VPORT_INT_5_bp;
    #define VPORT_INT5_bp VPORT_INT_5_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_6_bm) && defined(VPORT_INT6_bm)
    #define VPORT_INT_6_bm VPORT_INT6_bm
  #elif defined(VPORT_INT_6_bm)
    //deprecated_constant_name VPORT_INT6_bm = VPORT_INT_6_bm;
    #define VPORT_INT6_bm VPORT_INT_6_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_6_bp) && defined(VPORT_INT6_bp)
    #define VPORT_INT_6_bp VPORT_INT6_bp
  #elif defined(VPORT_INT_6_bp)
    //deprecated_constant_name VPORT_INT6_bp = VPORT_INT_6_bp;
    #define VPORT_INT6_bp VPORT_INT_6_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_7_bm) && defined(VPORT_INT7_bm)
    #define VPORT_INT_7_bm VPORT_INT7_bm
  #elif defined(VPORT_INT_7_bm)
    //deprecated_constant_name VPORT_INT7_bm = VPORT_INT_7_bm;
    #define VPORT_INT7_bm VPORT_INT_7_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VPORT_INT_7_bp) && defined(VPORT_INT7_bp)
    #define VPORT_INT_7_bp VPORT_INT7_bp
  #elif defined(VPORT_INT_7_bp)
    //deprecated_constant_name VPORT_INT7_bp = VPORT_INT_7_bp;
    #define VPORT_INT7_bp VPORT_INT_7_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= VREF ======= */
  #if !defined(VREF_REFSEL_0_bm) && defined(VREF_REFSEL0_bm)
    #define VREF_REFSEL_0_bm VREF_REFSEL0_bm
  #elif defined(VREF_REFSEL_0_bm)
    //deprecated_constant_name VREF_REFSEL0_bm = VREF_REFSEL_0_bm;
    #define VREF_REFSEL0_bm VREF_REFSEL_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VREF_REFSEL_0_bp) && defined(VREF_REFSEL0_bp)
    #define VREF_REFSEL_0_bp VREF_REFSEL0_bp
  #elif defined(VREF_REFSEL_0_bp)
    //deprecated_constant_name VREF_REFSEL0_bp = VREF_REFSEL_0_bp;
    #define VREF_REFSEL0_bp VREF_REFSEL_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VREF_REFSEL_1_bm) && defined(VREF_REFSEL1_bm)
    #define VREF_REFSEL_1_bm VREF_REFSEL1_bm
  #elif defined(VREF_REFSEL_1_bm)
    //deprecated_constant_name VREF_REFSEL1_bm = VREF_REFSEL_1_bm;
    #define VREF_REFSEL1_bm VREF_REFSEL_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VREF_REFSEL_1_bp) && defined(VREF_REFSEL1_bp)
    #define VREF_REFSEL_1_bp VREF_REFSEL1_bp
  #elif defined(VREF_REFSEL_1_bp)
    //deprecated_constant_name VREF_REFSEL1_bp = VREF_REFSEL_1_bp;
    #define VREF_REFSEL1_bp VREF_REFSEL_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VREF_REFSEL_2_bm) && defined(VREF_REFSEL2_bm)
    #define VREF_REFSEL_2_bm VREF_REFSEL2_bm
  #elif defined(VREF_REFSEL_2_bm)
    //deprecated_constant_name VREF_REFSEL2_bm = VREF_REFSEL_2_bm;
    #define VREF_REFSEL2_bm VREF_REFSEL_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(VREF_REFSEL_2_bp) && defined(VREF_REFSEL2_bp)
    #define VREF_REFSEL_2_bp VREF_REFSEL2_bp
  #elif defined(VREF_REFSEL_2_bp)
    //deprecated_constant_name VREF_REFSEL2_bp = VREF_REFSEL_2_bp;
    #define VREF_REFSEL2_bp VREF_REFSEL_2_bp //Deprecated as of Q2 2022 header change.
  #endif

  /* ======= WDT ======= */
  #if !defined(WDT_PERIOD_0_bm) && defined(WDT_PERIOD0_bm)
    #define WDT_PERIOD_0_bm WDT_PERIOD0_bm
  #elif defined(WDT_PERIOD_0_bm)
    //deprecated_constant_name WDT_PERIOD0_bm = WDT_PERIOD_0_bm;
    #define WDT_PERIOD0_bm WDT_PERIOD_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(WDT_PERIOD_0_bp) && defined(WDT_PERIOD0_bp)
    #define WDT_PERIOD_0_bp WDT_PERIOD0_bp
  #elif defined(WDT_PERIOD_0_bp)
    //deprecated_constant_name WDT_PERIOD0_bp = WDT_PERIOD_0_bp;
    #define WDT_PERIOD0_bp WDT_PERIOD_0_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(WDT_PERIOD_1_bm) && defined(WDT_PERIOD1_bm)
    #define WDT_PERIOD_1_bm WDT_PERIOD1_bm
  #elif defined(WDT_PERIOD_1_bm)
    //deprecated_constant_name WDT_PERIOD1_bm = WDT_PERIOD_1_bm;
    #define WDT_PERIOD1_bm WDT_PERIOD_1_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(WDT_PERIOD_1_bp) && defined(WDT_PERIOD1_bp)
    #define WDT_PERIOD_1_bp WDT_PERIOD1_bp
  #elif defined(WDT_PERIOD_1_bp)
    //deprecated_constant_name WDT_PERIOD1_bp = WDT_PERIOD_1_bp;
    #define WDT_PERIOD1_bp WDT_PERIOD_1_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(WDT_PERIOD_2_bm) && defined(WDT_PERIOD2_bm)
    #define WDT_PERIOD_2_bm WDT_PERIOD2_bm
  #elif defined(WDT_PERIOD_2_bm)
    //deprecated_constant_name WDT_PERIOD2_bm = WDT_PERIOD_2_bm;
    #define WDT_PERIOD2_bm WDT_PERIOD_2_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(WDT_PERIOD_2_bp) && defined(WDT_PERIOD2_bp)
    #define WDT_PERIOD_2_bp WDT_PERIOD2_bp
  #elif defined(WDT_PERIOD_2_bp)
    //deprecated_constant_name WDT_PERIOD2_bp = WDT_PERIOD_2_bp;
    #define WDT_PERIOD2_bp WDT_PERIOD_2_bp //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(WDT_PERIOD_3_bm) && defined(WDT_PERIOD3_bm)
    #define WDT_PERIOD_3_bm WDT_PERIOD3_bm
  #elif defined(WDT_PERIOD_3_bm)
    //deprecated_constant_name WDT_PERIOD3_bm = WDT_PERIOD_3_bm;
    #define WDT_PERIOD3_bm WDT_PERIOD_3_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(WDT_PERIOD_3_bp) && defined(WDT_PERIOD3_bp)
    #define WDT_PERIOD_3_bp WDT_PERIOD3_bp
  #elif defined(WDT_PERIOD_3_bp)
    //deprecated_constant_name WDT_PERIOD3_bp = WDT_PERIOD_3_bp;
    #define WDT_PERIOD3_bp WDT_PERIOD_3_bp //Deprecated as of Q2 2022 header change.
  #endif

  #if !defined(WDT_WINDOW_0_bm) && defined(WDT_WINDOW0_bm)
    #define WDT_WINDOW_0_bm WDT_WINDOW0_bm
  #elif defined(WDT_WINDOW_0_bm)
    #define WDT_WINDOW0_bm WDT_WINDOW_0_bm //Deprecated as of Q2 2022 header change.
  #endif
  #if !defined(WDT_WINDOW_0_bp) && defined(WDT_WINDOW0_bp)
    #define WDT_WINDOW_0_bp WDT_WINDOW0_bp
  #elif defined(WDT_WINDOW_0_bp)
    #define WDT_WINDOW0_bp WDT_WINDOW_0_bp; //Deprecated as of Q2 2022 header change
  #endif
  #if !defined(WDT_WINDOW_1_bm) && defined(WDT_WINDOW1_bm)
    #define WDT_WINDOW_1_bm WDT_WINDOW1_bm
  #elif defined(WDT_WINDOW_1_bm)
    #define WDT_WINDOW1_bm WDT_WINDOW_1_bm; //Deprecated as of Q2 2022 header change
  #endif
  #if !defined(WDT_WINDOW_1_bp) && defined(WDT_WINDOW1_bp)
    #define WDT_WINDOW_1_bp WDT_WINDOW1_bp
  #elif defined(WDT_WINDOW_1_bp)
    #define WDT_WINDOW1_bp WDT_WINDOW_1_bp; //Deprecated as of Q2 2022 header change
  #endif
  #if !defined(WDT_WINDOW_2_bm) && defined(WDT_WINDOW2_bm)
    #define WDT_WINDOW_2_bm WDT_WINDOW2_bm
  #elif defined(WDT_WINDOW_2_bm)
    #define WDT_WINDOW2_bm WDT_WINDOW_2_bm; //Deprecated as of Q2 2022 header change
  #endif
  #if !defined(WDT_WINDOW_2_bp) && defined(WDT_WINDOW2_bp)
    #define WDT_WINDOW_2_bp WDT_WINDOW2_bp
  #elif defined(WDT_WINDOW_2_bp)
    #define WDT_WINDOW2_bp WDT_WINDOW_2_bp; //Deprecated as of Q2 2022 header change
  #endif
  #if !defined(WDT_WINDOW_3_bm) && defined(WDT_WINDOW3_bm)
    #define WDT_WINDOW_3_bm WDT_WINDOW3_bm
  #elif defined(WDT_WINDOW_3_bm)
    #define WDT_WINDOW3_bm WDT_WINDOW_3_bm; //Deprecated as of Q2 2022 header change
  #endif
  #if !defined(WDT_WINDOW_3_bp) && defined(WDT_WINDOW3_bp)
    #define WDT_WINDOW_3_bp WDT_WINDOW3_bp
  #elif defined(WDT_WINDOW_3_bp)
    #define WDT_WINDOW3_bp WDT_WINDOW_3_bp; //Deprecated as of Q2 2022 header change
  #endif

  /* ======= ZCD ======= */
  #if !defined(ZCD_INTMODE_0_bm) && defined(ZCD_INTMODE0_bm)
    #define ZCD_INTMODE_0_bm ZCD_INTMODE0_bm
  #elif defined(ZCD_INTMODE_0_bm)
    #define ZCD_INTMODE0_bm ZCD_INTMODE_0_bm; //Deprecated as of Q2 2022 header change
  #endif
  #if !defined(ZCD_INTMODE_0_bp) && defined(ZCD_INTMODE0_bp)
    #define ZCD_INTMODE_0_bp ZCD_INTMODE0_bp
  #elif defined(ZCD_INTMODE_0_bp)
    #define ZCD_INTMODE0_bp ZCD_INTMODE_0_bp; //Deprecated as of Q2 2022 header change
  #endif
  #if !defined(ZCD_INTMODE_1_bm) && defined(ZCD_INTMODE1_bm)
    #define ZCD_INTMODE_1_bm ZCD_INTMODE1_bm
  #elif defined(ZCD_INTMODE_1_bm)
    #define ZCD_INTMODE1_bm ZCD_INTMODE_1_bm; //Deprecated as of Q2 2022 header change
  #endif
  #if !defined(ZCD_INTMODE_1_bp) && defined(ZCD_INTMODE1_bp)
    #define ZCD_INTMODE_1_bp ZCD_INTMODE1_bp
  #elif defined(ZCD_INTMODE_1_bp)
    #define ZCD_INTMODE1_bp ZCD_INTMODE_1_bp; //Deprecated as of Q2 2022 header change
  #endif
#endif /* this is the end of the backwards compatibility defines */
#endif // end of core_devices
