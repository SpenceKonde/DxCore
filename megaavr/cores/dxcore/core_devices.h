/* core_devices - a part of Arduino.h for DxCore 1.3.0 and later
 * This is directly included by Arduino.h and nothing else; it just moves
 * clutter out of that file.
 *
 * Spence Konde 2021 - DxCore is free software (LGPL 2.1)
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
  #define PROGMEM_SECTION2 __attribute__(( __section__(".FLMAP_SECTION0")))
  #define PROGMEM_SECTION3 __attribute__(( __section__(".FLMAP_SECTION1")))
#elif (__AVR_ARCH__ == 102)
  #define PROGMEM_MAPPED   __attribute__(( __section__(".FLMAP_SECTION1")))
  #define PROGMEM_SECTION0 __attribute__(( __section__(".FLMAP_SECTION0")))
  #define PROGMEM_SECTION1 __attribute__(( __section__(".FLMAP_SECTION1")))
#else
  // __AVR_ARCH__ == 103, so all of the flash is memory mapped, and the linker
  // will automatically leave const variables in flash.
  #define PROGMEM_MAPPED
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

// #define BACKWARD_COMBATIBILITY_MODE

#if !defined(BACKWARD_COMBATIBILITY_MODE)
  // We default to seeking compatibility - every name

  #if defined(RTC_CLKSEL)
  /* Man they just *HAD* to change the names of these values that get assigned to the same register and do the same thing didn't they?
   * Worse still we can't even verify that they are present... just blindly define and pray. Enums can't be seen by macros   */
    #define RTC_CLKSEL_INT32K_gc              RTC_CLKSEL_OSC32K_gc
    #define RTC_CLKSEL_INT1K_gc               RTC_CLKSEL_OSC1K_gc
    #define RTC_CLKSEL_TOSC32K_gc             RTC_CLKSEL_XOSC32K_gc
    #define RTC_CLKSEL_XTAL32K_gc             RTC_CLKSEL_XOSC32K_gc
    // #define RTC_CLKSEL_XOSC32K_gc          RTC_CLKSEL_XTAL32K_gc
    // for when they notice the IO header doesn't match the datasheet and have to "fix" something...
  #endif
  /* General Purpose Register names, GPR.GPRn, vs GPIORn vs GPIOn
   * They now appear to have decided they don't like either of the previous conventions, one just a few years old. Now they are grouping
   * them under a "General Purpose Register". "peripheral". I cannot argue that GPR doesn't make more sense, as there's not really any
   * I/O occurring here (ofc they were referring to the IN and OUT instructions, which can be used on these), but I certainly wouldn't
   * have changed a convention like this, at least not when I had just done so a few years prior. */

  // All non-xmega pre-Dx-series parts call them GPIORn instead of GPR.GPRn/GPR_GPRn
  #ifndef GPIOR0
    #define GPIOR0                           (GPR_GPR0)
    #define GPIOR1                           (GPR_GPR1)
    #define GPIOR2                           (GPR_GPR2)
    #define GPIOR3                           (GPR_GPR3)
  #endif

  /* In one xMega AVR, they were GPIOn, rather than GPIORn
   * One? Yup: The ATxmega32d4. Not the 32d3, nor the 32e5, nor anything else. All the xmega's have GPIORs
   * and their headers list the GPIOn names too. But.... there is only a single header
   * file with them not marked as "Deprecated": ATxmega32D4
   * 24 of the 46 xmega parts with headers in the compiler packages (including the 32d3 and 32e5) had the
   * 4 GPIOR's that we have, and had GPIOn and GPIO_GPIOn present but marked as deprecated.
   * On those parts, these are at addresses 0x0000-0x003, and 0x0004-0x000F do not appear to be used.
   * The other 22.... had THE ENTIRE FIRST HALF OF THE LOW I/O SPACE as GPIOR0-GPIORF!
   * Which ones got all of them and which ones only got 4 seems to have been chosen in typical
   * Atmel fashion (in other words, randomly). No apparent pattern in time or other parameters.
   * Either way, that left them with space for only 4 VPORT register sets (like the ones we got)
   * These had to be configured to point to the desired port.
   * I'm sure everyone is grateful for the fact that the folks designing the Dx-series have their
   * heads screwed on properly and realized that 4 GPIOR-- excuse me, GPRs, 4 awkward VPORTs and
   * 12 unused addresses in the low I/O space was maybe not the best design decision made in the
   * xmega line, and decided that wasn't a winning formula */
  #ifndef GPIO0
    #define GPIO0                           (GPR_GPR0)
    #define GPIO_GPIO0                      (GPR_GPR0)
    #define GPIO1                           (GPR_GPR1)
    #define GPIO_GPIO1                      (GPR_GPR1)
    #define GPIO2                           (GPR_GPR2)
    #define GPIO_GPIO2                      (GPR_GPR2)
    #define GPIO3                           (GPR_GPR3)
    #define GPIO_GPIO3                      (GPR_GPR3)
  #endif
  /* They are are the 4 registers in the GPR "peripheral", GPR.GPR0, GPR.GPR1, GPR.GPR2, and GPR.GPR3!
   * Let's not split hairs about whether calling 4 registers that do absolutely nothing other than being
   * located at addresses 0x1C, 0x1D, 0x1E and 0x1F allowing use of all the glorious instructions that brings
   * SBI, CBI, SBIS, SBIC, IN, and OUT, is enough to qualify as a peripheral.
   * Anyway - the flat names were used because if we don't, in some situations that winds up causing weird
   * problems. */

  /* Code written for tinyAVR's TCA EVACT, which is identical to EVACTA on newer parts, would not work
   * even though they have the same functionality
   */
  #define TCA_SINGLE_CNTEI_bm               TCA_SINGLE_CNTAEI_bm
  #define TCA_SINGLE_CNTEI_bp               TCA_SINGLE_CNTAEI_bp
  #define TCA_SINGLE_EVACT_gm               TCA_SINGLE_EVACTA_gm
  #define TCA_SINGLE_EVACT_gp               TCA_SINGLE_EVACTA_gp
  #define TCA_SINGLE_EVACT_CNT_POSEDGE_gc   TCA_SINGLE_EVACTA_CNT_POSEDGE_gc
  #define TCA_SINGLE_EVACT_CNT_ANYEDGE_gc   TCA_SINGLE_EVACTA_CNT_ANYEDGE_gc
  #define TCA_SINGLE_EVACT_CNT_HIGHLVL_gc   TCA_SINGLE_EVACTA_CNT_HIGHLVL_gc
  #define TCA_SINGLE_EVACT_UPDOWN_gc        TCA_SINGLE_EVACTA_UPDOWN_gc

  #if defined (CLKCTRL_SELHF_bm)
    /* They changed the damned name after selling the part for 6 months!
     * annoyingly you can't even test if it's using the new version of the headers because it's an enum! */
    #define CLKCTRL_SELHF_CRYSTAL_gc CLKCTRL_SELHF_XTAL_gc
  #endif
  /* And one version later they did it again... */
  #if !defined(CLKCTRL_FREQSEL_gm) && defined(CLKCTRL_FRQSEL_gm)
    #define CLKCTRL_FREQSEL_gm     CLKCTRL_FRQSEL_gm        /*    Group Mask                  */
    #define CLKCTRL_FREQSEL_gp     CLKCTRL_FRQSEL_gp        /*    Group Position              */
    //                                                      /*    Group Codes                 */
    #define CLKCTRL_FREQSEL_1M_gc  CLKCTRL_FRQSEL_1M_gc     /*  1 MHz system clock            */
    #define CLKCTRL_FREQSEL_2M_gc  CLKCTRL_FRQSEL_2M_gc     /*  2 MHz system clock            */
    #define CLKCTRL_FREQSEL_3M_gc  CLKCTRL_FRQSEL_3M_gc     /*  3 MHz system clock            */
    #define CLKCTRL_FREQSEL_4M_gc  CLKCTRL_FRQSEL_4M_gc     /*  4 MHz system clock default    */
    #define CLKCTRL_FREQSEL_8M_gc  CLKCTRL_FRQSEL_8M_gc     /*  8 MHz system clock            */
    #define CLKCTRL_FREQSEL_12M_gc CLKCTRL_FRQSEL_12M_gc    /* 12 MHz system clock            */
    #define CLKCTRL_FREQSEL_16M_gc CLKCTRL_FRQSEL_16M_gc    /* 16 MHz system clock            */
    #define CLKCTRL_FREQSEL_20M_gc CLKCTRL_FRQSEL_20M_gc    /* 20 MHz system clock            */
    #define CLKCTRL_FREQSEL_24M_gc CLKCTRL_FRQSEL_24M_gc    /* 24 MHz system clock            */
    #define CLKCTRL_FREQSEL_28M_gc CLKCTRL_FRQSEL_28M_gc    /* 28 MHz system clock unofficial - this will just error out if used since it will replace one undefined symbol with another */
    #define CLKCTRL_FREQSEL_32M_gc CLKCTRL_FRQSEL_32M_gc    /* 32 MHz system clock unofficial - this will just error out if used since it will replace one undefined symbol with another */
  #elif defined(CLKCTRL_FRQSEL_gm) && !defined(CLKCTRL_FREQSEL_gm)
    #define CLKCTRL_FRQSEL_gm     CLKCTRL_FREQSEL_gm        /*    Group Mask                  */
    #define CLKCTRL_FRQSEL_gp     CLKCTRL_FREQSEL_gp        /*    Group Position              */
    //                                                      /*    Group Codes                 */
    #define CLKCTRL_FRQSEL_1M_gc  CLKCTRL_FREQSEL_1M_gc     /*  1 MHz system clock            */
    #define CLKCTRL_FRQSEL_2M_gc  CLKCTRL_FREQSEL_2M_gc     /*  2 MHz system clock            */
    #define CLKCTRL_FRQSEL_3M_gc  CLKCTRL_FREQSEL_3M_gc     /*  3 MHz system clock            */
    #define CLKCTRL_FRQSEL_4M_gc  CLKCTRL_FREQSEL_4M_gc     /*  4 MHz system clock default    */
    #define CLKCTRL_FRQSEL_8M_gc  CLKCTRL_FREQSEL_8M_gc     /*  8 MHz system clock            */
    #define CLKCTRL_FRQSEL_12M_gc CLKCTRL_FREQSEL_12M_gc    /* 12 MHz system clock            */
    #define CLKCTRL_FRQSEL_16M_gc CLKCTRL_FREQSEL_16M_gc    /* 16 MHz system clock            */
    #define CLKCTRL_FRQSEL_20M_gc CLKCTRL_FREQSEL_20M_gc    /* 20 MHz system clock            */
    #define CLKCTRL_FRQSEL_24M_gc CLKCTRL_FREQSEL_24M_gc    /* 24 MHz system clock            */
    #define CLKCTRL_FRQSEL_28M_gc CLKCTRL_FREQSEL_28M_gc    /* 28 MHz system clock unofficial - this will just error out if used since it will replace one undefined symbol with another */
    #define CLKCTRL_FRQSEL_32M_gc CLKCTRL_FREQSEL_32M_gc    /* 32 MHz system clock unofficial - this will just error out if used since it will replace one undefined symbol with another */
  #endif
  // Note that it is intended to not hide the fact that 28 and 32 MHz are not official. If you choose it from the menu, it says "Overclocked" next to the speed too.
#endif /* this is the end of the backwards compatibility defines */

/* Chip families
 *
 * 0b ffssfppp
 *
 * ff__f is a 3-bit family code 00__0 is the DA, 00__1 is DB,
 * 01__0 is DD. Dx-series grows up from bottom, Ex-series down
 * from top in order of availability of silicon. So the next two
 * are going to be 11__1 for the EA and 01__1 for the DU
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
#define ID_AVR8EA       (0xB8)
// We could cover an 8 pin part! Or a 100-pin one. Neither seems likely
#define ID_14_PINS      (0x01)
#define ID_20_PINS      (0x02)
#define ID_24_PINS      (0x03)
#define ID_28_PINS      (0x04)
#define ID_32_PINS      (0x05)
#define ID_48_PINS      (0x06)
#define ID_64_PINS      (0x07)

#define ID_AVR_DA       (0x00)
#define ID_AVR_DB       (0x08)
#define ID_AVR_DD       (0x40) // Almost released - can't want to get my hands on those DD's!
#define ID_AVR_DU       (0x48) // Product brief posted then deleted. Who knows if it will exist.
/*      ID_AVR_??       (0x80) */ // We don't yet know the letters that further series will have but I'm pretty confident that the last one
/*      ID_AVR_??       (0x88) */ // this system can support won't be a low pincount, fancy-ADC bearing "ED-series"...  though it
/*      ID_AVR_??       (0xC0) */ // would be an amusing contrast to the sexy lettering for the low-pincount Dx parts
#define ID_AVR_EA       (0xC8) // Not yet released

#define ID_MASK_SERIES  (0xC8)
#define ID_MASK_FLASH   (0x30)
#define ID_MASK_PINS    (0x07)

//#defines to identify part families
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
  #define HAS_28_PINS
  #define DX_28_PINS
  #define Dx_28_PINS
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
  #define HAS_14_PINS
  #define DX_14_PINS
  #define CORE_PART_ID_LOW (ID_14_PINS | ID_AVR_DD)
  #define __AVR_DD__
#elif defined(__AVR_AVR64DU32__)  || defined(__AVR_AVR32DU32__) || defined(__AVR_AVR16DU32__)
  #define DU_32_PINS
  #define DX_32_PINS
  #define Dx_32_PINS
  #define CORE_PART_ID_LOW (ID_32_PINS | ID_AVR_DU)
  #define __AVR_DU__
#elif defined(__AVR_AVR64DU28__)  || defined(__AVR_AVR32DU28__) || defined(__AVR_AVR16DU28__)
  #define DU_28_PINS
  #define DX_28_PINS
  #define Dx_28_PINS
  #define CORE_PART_ID_LOW (ID_28_PINS | ID_AVR_DU)
  #define __AVR_DU__
#elif defined(__AVR_AVR32DU20__)  || defined(__AVR_AVR16DU20__)
  #define DU_20_PINS
  #define DX_20_PINS
  #define CORE_PART_ID_LOW (ID_20_PINS | ID_AVR_DU)
  #define __AVR_DU__
#elif defined(__AVR_AVR32DU14__)  || defined(__AVR_AVR16DU14__)
  #define DU_14_PINS
  #define DX_14_PINS
  #define CORE_PART_ID_LOW (ID_14_PINS | ID_AVR_DU)
  #define __AVR_DU__
#elif defined(__AVR_AVR8EA48__)   || defined(__AVR_AVR16EA48__) || defined(__AVR_AVR64EA48__) || defined(__AVR_AVR32EA48__)
  #define EA_48_PINS
  #define HAS_48_PINS
  #define EX_48_PINS
  #define Ex_48_PINS
  #define CORE_PART_ID_LOW (ID_48_PINS)
  #define __AVR_EA__
#elif defined(__AVR_AVR8EA32__)   || defined(__AVR_AVR16EA32__) || defined(__AVR_AVR64EA32__) || defined(__AVR_AVR32EA32__)
  #define EA_32_PINS
  #define HAS_32_PINS
  #define EX_32_PINS
  #define Ex_32_PINS
  #define CORE_PART_ID_LOW (ID_32_PINS)
  #define __AVR_EA__
#elif defined(__AVR_AVR8EA28__)   || defined(__AVR_AVR16EA28__) || defined(__AVR_AVR64EA28__) || defined(__AVR_AVR32EA28__)
  #define EA_28_PINS
  #define HAS_28_PINS
  #define EX_28_PINS
  #define Ex_28_PINS
  #define CORE_PART_ID_LOW (ID_28_PINS)
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
  #error "The AVR EA-series is not available. There is no datasheet yet available, though current indications suggest most changes will be confined to libraries."
#else
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
#elif   (PROGMEM_SIZE == 0x20000 && (defined(__AVR_DA__) || defined(__AVR_DB__))) || (PROGMEM_SIZE == 0x10000 && !(defined(__AVR_DD__) || defined(__AVR_DU__)))
  #define   CORE_PART_ID (CORE_PART_ID_LOW | 0x20) /* 128k DA/DB, 64k DD/DU */
#elif   (PROGMEM_SIZE == 0x10000 && (defined(__AVR_DA__) || defined(__AVR_DB__))) || (PROGMEM_SIZE ==  0x8000 && !(defined(__AVR_DD__) || defined(__AVR_DU__)))
  #define   CORE_PART_ID (CORE_PART_ID_LOW | 0x10) /*  32k DA/DB, 16k DD/DU */
#elif   (PROGMEM_SIZE ==  0x8000 && (defined(__AVR_DA__) || defined(__AVR_DB__))) || (PROGMEM_SIZE ==  0x4000 && !(defined(__AVR_DD__) || defined(__AVR_DU__)))
  #define   CORE_PART_ID (CORE_PART_ID_LOW | 0x00)
#else
  #error "Unrecognized combination of flash size and chip type"
#endif

 /* HARDWARE FEATURES - Used by #ifdefs and as constants in calculations in
  * the core and in libraries; it is hoped that these are at least somewhat
  * useful to users, as well. These are described in more detail in the
  * README. */

#define DEVICE_PORTMUX_TCA              (2) /* 1 = each wave output cannnel can be moved individually, like tinyAVRs
                                               2 = all wave output channels move together */
#define CORE_HAS_FASTIO                 (1) /* DxCore has the digitalReadFast() and digitalWriteFast()              */
#define CORE_HAS_OPENDRAIN              (1) /* DxCore has openDrain() and openDrainFast()                           */
#define CORE_HAS_PINCONFIG              (1) /* pinConfigure is now implemented                                      */
#define CORE_HAS_TIMER_TAKEOVER         (1) /* DxCore has takeOverTCA0(), takeOverTCA1() and takeOverTCD0()         */
#define CORE_HAS_TIMER_RESUME           (1) /* DxCore has   resumeTCA0(),   resumeTCA1(); there is no resumeTCD0()  */
#define CORE_SUPPORT_LONG_TONES         (1) /* tone()s specifying duration are timed by counting the oscillations.  */
                                            /* This causes problem for long high frequency tones. By rearranging a  */
                                            /* division operator, we can expand the range by a factor of 100, so    */
                                            /* that no reasonable tone will trigger it at cost of 100 bytes or so   */
#define ADC_DIFFERENTIAL                (1) /* Basic modern-AVR differential ADC                                    */
#define CORE_HAS_ANALOG_ENH             (1) /* DxCore has analogReadEnh()                                           */
#define CORE_HAS_ANALOG_DIFF            (1) /* DxCore has analogReadDiff()                                          */
#define ADC_MAX_OVERSAMPLED_RESOLUTION (15) /* DxCore has 15 bit maximum resolution via oversampling and decimation */
#define ADC_NATIVE_RESOLUTION          (12) /*                                                                      */
#define ADC_NATIVE_RESOLUTION_LOW      (10) /*                                                                      */
#define ADC_MAXIMUM_ACCUMULATE        (128) /* Maximum burst accumulation                                           */
#define ADC_MAXIMUM_SAMPDUR          (0xFF) /* Maximum SAMPLEN or SAMPDUR                                           */
#define ADC_RESULT_SIZE                (16) /* ADC Result Size (bits)                                               */
#if defined(__AVR_DD__) || defined(__AVR_EA__)
  #define ADC_MAXIMUM_PIN_CHANNEL      (31) /* Highest number that might be associated with a pin on DD - there are */
  #define ADC_MAXIMUM_NEGATIVE_PIN     (31) /* gaps on lower pincount parts. On DD and EA, all ADC pins work as     */
#else                                       /* negative inputs! The EA even has a decent differential ADC!          */
  #define ADC_MAXIMUM_PIN_CHANNEL      (21) /* At most, there are only 21 analog pins on a DA/DB                    */
  #define ADC_MAXIMUM_NEGATIVE_PIN     (15) /* and only PORTD and PORTE work with MUXNEG for differential reading   */
#endif
#if defined(ADC0_PGACTRL)                   /* The product briefs do not mention either way                         */
  #define ADC_MAXIMUM_GAIN             (16) /* The EA series will have a PGA like the 2-series parts.               */
#elif defined(OPAMP0)
  #ifndef ADC_MAXIMUM_GAIN
    #define ADC_MAXIMUM_GAIN           (-1)  /* DB-series can use their OPAMPs as a PGA                             */
  #endif
  #define PIN_OPAMP0_INP            PIN_PD1
  #define PIN_OPAMP0_OUT            PIN_PD2
  #define PIN_OPAMP0_INN            PIN_PD3
  #ifdef OPAMP1
    #define PIN_OPAMP1_INP          PIN_PD4
    #define PIN_OPAMP1_OUT          PIN_PD5
    #define PIN_OPAMP1_INN          PIN_PD7
  #endif
  #ifdef OPAMP2
    #define PIN_OPAMP2_INP          PIN_PE1
    #define PIN_OPAMP2_OUT          PIN_PE2
    #define PIN_OPAMP2_INN          PIN_PE3
  #endif
#endif
#if defined(__AVR_EA__)
  #define EVSYS_VERSION_TWO                 /* EA series has markedly different event system that is expected to  */
                                            /* replace the current one. It brings channel uniformity at the cost  */
                                            /* being limitd to two event inputs per port and two RTC PIT derived  */
                                            /* inputs                                                             */
#endif
#ifdef DAC0
  #ifndef PIN_DACOUT
    #define PIN_DACOUT PIN_PD6
  #endif
#endif
#if (defined(__AVR_DB__) || defined(__AVR_DD__) || defined(__AVR_EA__)) // IO headers say EA gets INLVL - looks like it's standard now!
  #define PORT_ID_INLVL                 (1)
#else
  #define PORT_ID_INLVL                 (0)
#endif
/* if (ADC_NATIVE_RESOLUTION + Log4(ADC_MAXIMUM_ACCUMULATE)) > ADC_RESULT_SIZE, long accumulations are truncated.
 * with maximum accumulation of Dx, for example, 12 + 7 = 19, so the internal representation would be a 19-bit number
 * but only the 16 most significant bits are presented in ADC0.RES. analogReadEnh() accounts for this when
 * asked to oversample and decimate.
 *
 * If not otherwise specified, we will assume the DAC outputs on PD6 - No product has
 * been announced with it anywhere else, nor has any product been announced with more than 1. */

/* ERRATA TESTS */
/* Not exhaustive, we'd be going on for thosands of lines to over all the errata. This code assumes all DB are Rev A5, not A4, A4 got pulled fast         */
/* If they're ever fixed, we'll replace these with a macro to check REVID and return 1 or 0 appropriately.    */
#if defined(__AVR_DA__) && (_AVR_FLASH == 128)
  #define ERRATA_TCA1_PORTMUX           (1) /* DA128's up to Rev. A8 have only the first two pinmapping options working                                   */
  #define ERRATA_PORTS_B_E_EVSYS        (1) /* DA128's up to Rev. A8 have no EVSYS on PB6, PB7, and PE4~7                                                 */
  #define ERRATA_NVM_ST_BUG             (1) /* DA128's up to Rev. A8 apply bootloader/app protection neglecting FLMAP bits when writing with ST. Use SPM. */
#endif

#if defined(__AVR_DA__)
  #define ERRATA_DAC_DRIFT              (1)
#endif

#if defined(__AVR_ARCH__)
  // No device has been released that doesn't have this bug!
  #define ERRATA_TCB_CCMP               (1)
  #define ERRATA_CCL_PROTECTION         (1)
#endif

#if defined(__AVR_DA__) || defined(__AVR_DB__)
  // Almost certainly won't be in the DD.
  #define ERRATA_TCD_PORTMUX            (1)
  #define ERRATA_ADC_PIN_DISABLE        (1)
#endif

#if defined(__AVR_DA__) || defined(__AVR_DB__)
  #define ERRATA_TWI_FLUSH
#endif

/* This macro is for when you want to set the internal to whatever F_CPU is, for
 * example to react to crystal not being found on startoup, when you have one selected
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
  #define _switchInternalToF_CPU() badCall("The _switchInternalToF_CPU() macro can only set the internal oscillator to speeds that are supported by it.")
#endif

#endif
