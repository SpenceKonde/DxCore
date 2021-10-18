/* core_devices - a part of Arduino.h for DxCore 1.3.0 and later
 * This is directly included by Arduino.h and nothing else; it just moves
 * clutter out of that file.
 *
 * Spence Konde 2021 - DxCore is free software (LGPL 2.1)
 * See LICENSE.txt for full legal boilerplate if you must */

#ifndef Core_Devices_h
#define Core_Devices_h
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
  #define MAPPED_PROGMEM
#endif


/* Microchip has shown a tendency to rename registers bitfields and similar between product lines, even when the behavior is identical.
 * This is a major hindrance to writing highly portable code. This is not expected to present any sort of issue. HOWEVER if it does,
 * The line above can be uncommented to disable all of the places where we find this and add define the old names to point to new ones.
 * The spelling of here is intentional. With it undefined, we will define the backwards compatible names, but if not.
 * If instead the user seeks to thwart compatibility (or alert themselves to code where they used old names) they are seeing the
 * opposite of backward compatibility, what some wags have terms "backward combatibility" */

// #define BACKWARD_COMBATIBILITY_MODE


#if !defined(BACKWARD_COMBATIBILITY_MODE)
  #if defined(RTC_CLKSEL)
  /* Man they just *HAD* to change the names of these values that get assigned to the same register and do the same thing didn't they?
   * Worse still we can't even verify that they are present... just blindly definr and pray. Enums can't be seen by macros   */
    #define RTC_CLKSEL_INT32K_gc  RTC_CLKSEL_OSC32K_gc
    #define RTC_CLKSEL_OSC1K_gc   RTC_CLKSEL_INT1K_gc
    #define RTC_CLKSEL_TOSC32K_gc RTC_CLKSEL_XTAL32K_gc
  #endif

  /* General Purpose Register names, GPR.GPRn, vs GPIORn vs GPIOn
   * They now appear to have decided they don't like either of the previous conventions, one just a few years old. Now they are grouping
   * them under a "General Purpose Register". "peripheral". I cannot argue that GPR doesn't make more sense, as there's not really any
   * I/O occurring here (ofc they were referring to the IN and OUT instructions, which can be used on these), but I certainly wouldn't
   * have changed a convention like this, at least not when I had just done so a few years prior. */

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

  // Of course in these more enlightened times, we know better!
  // They are are the 4 registers in the GPR peripheral, GPR.GPR0, GPR.GPR1, GPR.GPR2, and GPR.GPR3!
  // but if we dont use the flat name, it could cause problems when substituted in under some conditions.
#endif






// Chip families
// 0b ffssfppp
// ff__f is a 3-bit family code 00__0 is the DA, 00__1 is DB,
// 01__0 is DD. Dx-series grows up from bottom, Ex-series down
// from top in order of availability of silicon. So the next two
// are going to be 11__1 for the EA and 01__1 for the DU
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


#if defined(__AVR_DA__)
  #define _AVR_FAMILY "DA"
#elif defined(__AVR_DB__)
  #define _AVR_FAMILY "DB"
#elif defined(__AVR_DD__)
  #define _AVR_FAMILY "DD"
#elif defined(__AVR_DU__)
  #define _AVR_FAMILY "DU"
#elif defined(__AVR_EA__)
  #define _AVR_FAMILY "EA"
#else
  #define _AVR_FAMILY "UNKNOWN"
#endif

#if defined(DX_14_PINS )
  #define _AVR_PINCOUNT 14
#elif defined(DX_20_PINS )
  #define _AVR_PINCOUNT 29
#elif defined(DX_28_PINS )
  #define _AVR_PINCOUNT 28
#elif defined(DX_32_PINS )
  #define _AVR_PINCOUNT 32
#elif defined(DX_48_PINS )
  #define _AVR_PINCOUNT 48
#elif defined(DX_64_PINS )
  #define _AVR_PINCOUNT 64
#endif

#if PROGMEM_SIZE ==   0x20000
  #define _AVR_FLASH 128
#elif PROGMEM_SIZE == 0x10000
  #define _AVR_FLASH 64
#elif PROGMEM_SIZE == 0x8000
  #define _AVR_FLASH 32
#elif PROGMEM_SIZE == 0x4000
  #define _AVR_FLASH 16
#elif PROGMEM_SIZE == 0x2000
  #define _AVR_FLASH 8
#elif PROGMEM_SIZE == 0x1000
  #define _AVR_FLASH 4
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

#define CORE_HAS_FASTIO 1                /* DxCore has the digitalReadFast() and digitalWriteFast()              */
#define CORE_HAS_OPENDRAIN 1             /* DxCore has openDrain() and openDrainFast()                           */
#define CORE_HAS_PINCONFIG 1             /* pinConfigure is now implemented                                      */
#define CORE_HAS_TIMER_TAKEOVER 1        /* DxCore has takeOverTCA0(), takeOverTCA0() and takeOverTCD0()         */
#define CORE_HAS_TIMER_RESUME 1          /* DxCore has resumeTCA0(), resumeTCAq() and resumeTCD0()               */
#define CORE_SUPPORT_LONG_TONES 1             /* tone()s specifying duration are timed by counting the oscillations.
 * Frequency is in Hz, while duration is in ms, so (2 * frequency * duration)/1000 is the number of transitions
 * before it should write the pin low and turn off the timer. Obviously the 2 can be factored, but it will still
 * overflow when frequency * duration > 4.2b. A high-pitched tone of 20 kHz would overflow if a delay of longer
 * than around 7 minutes was requested (prior to this update, the maximum was a factor of two lower than that)
 * On parts like the Dx-series where there's no problem with flash space, we now, when duration > (2^16) ms (a
 * necessary precondition for overflow to occur) do ((frequency / 5) * (duration/100)) at cost of ~100b flash .  */
#define ADC_DIFFERENTIAL 1               /* Basic modern-AVR differential ADC                                    */
#define CORE_HAS_ANALOG_ENH 1            /* DxCore has analogReadEnh()                                           */
#define CORE_HAS_ANALOG_DIFF 1           /* DxCore has analogReadDiff()                                          */
#define ADC_MAX_OVERSAMPLED_RESOLUTION 15 /* DxCore has 15 bit maximum resolution via oversampling and decimation*/

#ifdef OPAMP0
  #ifndef ADC_MAXIMUM_GAIN
    #define ADC_MAXIMUM_GAIN -1            /* DB-series can use their OPAMPs as a PGA         */
  #endif
  #define PIN_OPAMP0_INP PIN_PD1
  #define PIN_OPAMP0_OUT PIN_PD2
  #define PIN_OPAMP0_INN PIN_PD3
  #ifdef OPAMP1
    #define PIN_OPAMP1_INP PIN_PD4
    #define PIN_OPAMP1_OUT PIN_PD5
    #define PIN_OPAMP1_INN PIN_PD7
  #endif
  #ifdef OPAMP2
    #define PIN_OPAMP2_INP PIN_PE1
    #define PIN_OPAMP2_OUT PIN_PE2
    #define PIN_OPAMP2_INN PIN_PE3
  #endif
#else
  #define ADC_MAXIMUM_GAIN 0                /* DA and DD series don't have any                */
#endif

// If not otherwise specified, we will assume the DAC outputs on PD6 - No product has
// been announced with it anywhere else, nor has any product been announced with more than 1.
#ifdef DAC0
  #ifndef PIN_DACOUT
    #define PIN_DACOUT PIN_PD6
  #endif
#endif

// These pieces of version numbers get passed in as command line arguments by platform.txt.
#define DXCORE_NUM ((DXCORE_MAJOR<<24)+(DXCORE_MINOR<<16)+(DXCORE_PATCH<<8)+DXCORE_RELEASED)

// Sometimes this define, passed in from command line, has gotten lost entirely.
// However, without it, things don't know that they are working with DxCore.
// So we put in a placeholder so defined(DXCORE) is true.
#ifndef DXCORE
  #define DXCORE "Unknown 1.3.7+"
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
#if (defined(__AVR_DB__) && defined(__AVR_DD__))
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

#if !defined(CLKCTRL_FREQSEL_gm)
  // And one version later they did it again...
  #define CLKCTRL_FREQSEL_gm CLKCTRL_FRQSEL_gm
#endif

#if !defined(CLKCTRL_FREQSEL_gp)
  // This impacts all frequency group codes too; those aren't corrected: use the new spelling!
  #define CLKCTRL_FREQSEL_gp CLKCTRL_FRQSEL_gp
#endif

#if defined(__AVR_DA__) || defined(__AVR_DB__)
  // No device has been released that doesn't have this bug!
  #define ERRATA_TCB_CCMP 1
#endif

#if defined(__AVR_DA__) || defined(__AVR_DB__)
  #define ERRATA_TCD_PORTMUX 1
#endif

/* This macro is for when you want to set the internal to whatever F_CPU is, for
 * example to react to crystal not being found on startoup, when you have one selected
 * The default behavior is to hang and proceed no further while issuing a blink code,
 * but you might instead want to ignore that failure, and instead use the less
 * accurate (but still pretty damned good) internal one. See the DxCore clock source referenc */
#if (F_CPU == 32000000)
  #define _switchInternalToF_CPU() _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x0B << 2))
#elif (F_CPU == 28000000)
  #define _switchInternalToF_CPU() _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x0A << 2))
#elif (F_CPU == 24000000)
  #define _switchInternalToF_CPU() _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x09 << 2))
#elif (F_CPU == 20000000)
  #define switchInternalToF_CPU() _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x08 << 2))
#elif (F_CPU == 16000000)
  #define _switchInternalToF_CPU() _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x07 << 2))
#elif (F_CPU == 12000000)
  #define _switchInternalToF_CPU() _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x06 << 2))
#elif (F_CPU == 8000000)
  #define _switchInternalToF_CPU() _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x05 << 2))
#elif (F_CPU == 4000000)
  #define _switchInternalToF_CPU() _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x03 << 2))
#elif (F_CPU == 3000000)
  #define _switchInternalToF_CPU() _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x02 << 2))
#elif (F_CPU == 2000000)
  #define _switchInternalToF_CPU() _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x01 << 2))
#elif (F_CPU == 1000000)
  #define _switchInternalToF_CPU() _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x00 << 2))
#elif (F_CPU == 10000000)
  #define _switchInternalToF_CPU() {_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB,  (CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm)); _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x08 << 2));}
#elif (F_CPU == 5000000)
  #define _switchInternalToF_CPU() {_PROTECTED_WRITE(CLKCTRL_MCLKCTRLB,  (CLKCTRL_PDIV_4X_gc | CLKCTRL_PEN_bm)); _PROTECTED_WRITE(CLKCTRL_OSCHFCTRLA, (0x08 << 2));}
#else
  #define _switchInternalToF_CPU() badCall("The switchInternalToF_CPU() macro can only set the internal oscillator to speeds that are supported by it.")
#endif

#endif
