/* pinswap.h- a part of Arduino.h for DxCore 1.3.7 and later
 * This is directly included by Arduino.h AFTER pins_arduino.h
 * It's function is to define macros and data structures required
 * for managing portmux in a manner that doesn't make users and
 * more urgently, the maintainer of the core, go nuts.
 *
 * It's existence was mostly forced by the expanded options on the
 * AVR DD-series, which among other things forces a total reimplementation
 * of the serial port portmux in a manner that isn't quite so stupid.
 *
 * SPI and Wire retain their rather ugly implementations since there
 * is only one of each, and the consequences are just that the
 * library files are ugly. With HardwareSerial, it makes the
 * calls to instantiate the classes unreasonably large. And they
 * take RAM too. A class constructor should not have 30+ arguments,
 * of which, in most cases, 20 or so would be a null and all of
 * which are uniquely specified by one of them..
 *
 * This also provides human-friendly ways to get current pins for
 * the one-of-a-kind classes (SPI and Wire).
 *
 * Spence Konde 2021 - megaTinyCore is free software (LGPL 2.1)
 * See LICENSE.txt for full legal boilerplate if you must
 ****************************************************************/
#ifndef PINSWAP_H
#define PINSWAP_H
/* These handle SDA and SCL. The SDA_NOW and SCL_NOW give you
 * the current pins. I've experimented with replacing the SCL/SDA
 * but it caused compatibility problerm */

#ifdef PIN_WIRE_SCL_PINSWAP_1
  static const uint8_t SDA_ALT1 = PIN_WIRE_SDA_PINSWAP_1;
  static const uint8_t SCL_ALT1 = PIN_WIRE_SCL_PINSWAP_1;
#endif
#ifdef PIN_WIRE_SCL_PINSWAP_2
  static const uint8_t SDA_ALT2 = PIN_WIRE_SDA_PINSWAP_2;
  static const uint8_t SCL_ALT2 = PIN_WIRE_SCL_PINSWAP_2;
#endif
#ifdef PIN_WIRE_SCL_PINSWAP_3
  static const uint8_t SDA_ALT3 = PIN_WIRE_SDA_PINSWAP_3;
  static const uint8_t SCL_ALT3 = PIN_WIRE_SCL_PINSWAP_3;
#endif
static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;


// Only care about alt 2 and 3, as they have different pins. 1 is a nothing right now.
#ifdef PIN_WIRE_SCL_PINSWAP_2
  #ifdef PIN_WIRE_SCL_PINSWAP_3 // all three
    #define SDA_NOW ((uint8_t) ((PORTMUX.TWIROUTEA & PORTMUX_TWI0_gm) == 2 ? SDA_ALT2 : (PORTMUX.TWIROUTEA & PORTMUX_TWI0_gm) == 3 ? SDA_ALT3 : PIN_WIRE_SDA))
    #define SCL_NOW ((uint8_t) ((PORTMUX.TWIROUTEA & PORTMUX_TWI0_gm) == 2 ? SCL_ALT2 : (PORTMUX.TWIROUTEA & PORTMUX_TWI0_gm) == 3 ? SCL_ALT3 : PIN_WIRE_SCL))
  #else // 2, no 3
    #define SDA_NOW ((uint8_t) ((PORTMUX.TWIROUTEA & PORTMUX_TWI0_gm) == 2 ? SDA_ALT2 : PIN_WIRE_SDA))
    #define SCL_NOW ((uint8_t) ((PORTMUX.TWIROUTEA & PORTMUX_TWI0_gm) == 2 ? SCL_ALT2 : PIN_WIRE_SCL))
  #endif
#else   // no 2, yes 3
  #ifdef PIN_WIRE_SCL_PINSWAP_2
    #define SDA_NOW ((uint8_t) ((PORTMUX.TWIROUTEA & PORTMUX_TWI0_gm) == 3 ? SDA_ALT3 : PIN_WIRE_SDA))
    #define SCL_NOW ((uint8_t) ((PORTMUX.TWIROUTEA & PORTMUX_TWI0_gm) == 3 ? SCL_ALT3 : PIN_WIRE_SCL))
  #else // no 1, no 2, no 3
    #define SDA_NOW (PIN_WIRE_SDA)
    #define SCL_NOW (PIN_WIRE_SCL)
  #endif
#endif

#ifdef PIN_WIRE1_SCL_PINSWAP_1
  static const uint8_t SDA1_ALT1 = PIN_WIRE1_SDA_PINSWAP_1;
  static const uint8_t SCL1_ALT1 = PIN_WIRE1_SCL_PINSWAP_1;
#endif
#ifdef PIN_WIRE1_SCL_PINSWAP_2
  static const uint8_t SDA1_ALT2 = PIN_WIRE1_SDA_PINSWAP_2;
  static const uint8_t SCL1_ALT2 = PIN_WIRE1_SCL_PINSWAP_2;
#endif

#ifdef TWI1
  static const uint8_t SDA1 = PIN_WIRE1_SDA;
  static const uint8_t SCL1 = PIN_WIRE1_SCL;
  #ifdef PIN_WIRE1_SCL
    #ifdef PIN_WIRE1_SCL_PINSWAP_2
      #define SDA1_NOW ((uint8_t) ((PORTMUX.TWIROUTEA & PORTMUX_TWI1_gm) == 8 ? SDA1_ALT2 : SDA))
      #define SCL1_NOW ((uint8_t) ((PORTMUX.TWIROUTEA & PORTMUX_TWI1_gm) == 8 ? SCL1_ALT2 : SCL))
    #else
      #define SDA1_NOW (PIN_WIRE1_SDA)
      #define SCL1_NOW (PIN_WIRE1_SCL)
    #endif
  #endif
#endif
/* Now for a doozy - SPI0 - which has all 7 possible options, and up to 5 on some parts!
 * The specific pins are in the variants files. PIN_SPI_SCK_*/

#ifdef SPI_MUX_PINSWAP_1
  #if defined(PIN_SPI_SS_PINSWAP_1)
    #define SS_ALT1    PIN_SPI_SS_PINSWAP_1;
  #else
    #define SS_ALT1     NOT_A_PIN;
  #endif
  #if defined(PIN_SPI_MOSI_PINSWAP_1)
    #define MOSI_ALT1  PIN_SPI_MOSI_PINSWAP_1;
  #else
    #define MOSI_ALT1   NOT_A_PIN;
  #endif
  #if defined(PIN_SPI_MISO_PINSWAP_1)
    #define MISO_ALT1  PIN_SPI_MISO_PINSWAP_1;
  #else
    #define MISO_ALT1   NOT_A_PIN;
  #endif
  #if defined(PIN_SPI_SCK_PINSWAP_1)
    #define SCK_ALT1   PIN_SPI_SCK_PINSWAP_1;
  #else
    #define SCK_ALT1    NOT_A_PIN;
  #endif
#endif

#ifdef SPI_MUX_PINSWAP_2
  #if defined(PIN_SPI_SS_PINSWAP_2)
    #define SS_ALT2    PIN_SPI_SS_PINSWAP_2;
  #else
    #define SS_ALT2     NOT_A_PIN;
  #endif
  #if defined(PIN_SPI_MOSI_PINSWAP_2)
    #define MOSI_ALT2  PIN_SPI_MOSI_PINSWAP_2;
  #else
    #define MOSI_ALT2   NOT_A_PIN;
  #endif
  #if defined(PIN_SPI_MISO_PINSWAP_2)
    #define MISO_ALT2  PIN_SPI_MISO_PINSWAP_2;
  #else
    #define MISO_ALT2   NOT_A_PIN;
  #endif
  #if defined(PIN_SPI_SCK_PINSWAP_2)
    #define SCK_ALT2   PIN_SPI_SCK_PINSWAP_2;
  #else
    #define SCK_ALT2    NOT_A_PIN;
  #endif
#endif

#ifdef SPI_MUX_PINSWAP_3
  #if defined(PIN_SPI_SS_PINSWAP_3)
      #define SS_ALT3    PIN_SPI_SS_PINSWAP_3;
  #else
    #define SS_ALT3     NOT_A_PIN;
  #endif
  #if defined(PIN_SPI_MOSI_PINSWAP_3)
    #define MOSI_ALT3  PIN_SPI_MOSI_PINSWAP_3;
  #else
    #define MOSI_ALT3   NOT_A_PIN;
  #endif
  #if defined(PIN_SPI_MISO_PINSWAP_3)
    #define MISO_ALT3  PIN_SPI_MISO_PINSWAP_3;
  #else
    #define MISO_ALT3   NOT_A_PIN;
  #endif
  #if defined(PIN_SPI_SCK_PINSWAP_3)
    #define SCK_ALT3   PIN_SPI_SCK_PINSWAP_3;
  #else
    #define SCK_ALT3    NOT_A_PIN;
  #endif
#endif

#ifdef SPI_MUX_PINSWAP_4
  #if defined(PIN_SPI_SS_PINSWAP_4)
    #define SS_ALT4    PIN_SPI_SS_PINSWAP_4;
  #else
    #define SS_ALT4     NOT_A_PIN;
  #endif
  #if defined(PIN_SPI_MOSI_PINSWAP_4)
    #define MOSI_ALT4  PIN_SPI_MOSI_PINSWAP_4;
  #else
    #define MOSI_ALT4   NOT_A_PIN;
  #endif
  #if defined(PIN_SPI_MISO_PINSWAP_4)
    #define MISO_ALT4  PIN_SPI_MISO_PINSWAP_4;
  #else
    #define MISO_ALT4   NOT_A_PIN;
  #endif
  #if defined(PIN_SPI_SCK_PINSWAP_4)
    #define SCK_ALT4   PIN_SPI_SCK_PINSWAP_4;
  #else
    #define SCK_ALT4    NOT_A_PIN;
  #endif
#endif

#ifdef SPI_MUX_PINSWAP_5
  #if defined(PIN_SPI_SS_PINSWAP_5)
    #define SS_ALT5    PIN_SPI_SS_PINSWAP_5;
  #else
    #define SS_ALT5     NOT_A_PIN;
  #endif
  #if defined(PIN_SPI_MOSI_PINSWAP_5)
    #define MOSI_ALT5  PIN_SPI_MOSI_PINSWAP_5;
  #else
    #define MOSI_ALT5   NOT_A_PIN;
  #endif
  #if defined(PIN_SPI_MISO_PINSWAP_5)
    #define MISO_ALT5  PIN_SPI_MISO_PINSWAP_5;
  #else
    #define MISO_ALT5   NOT_A_PIN;
  #endif
  #if defined(PIN_SPI_SCK_PINSWAP_5)
    #define SCK_ALT5   PIN_SPI_SCK_PINSWAP_5;
  #else
    #define SCK_ALT5    NOT_A_PIN;
  #endif
#endif

#ifdef PIN_SPI_MUX_PINSWAP_6
  #if defined(PIN_SPI_SS_PINSWAP_6)
    #define SS_ALT6    PIN_SPI_SS_PINSWAP_6;
  #else
    #define SS_ALT6     NOT_A_PIN;
  #endif
  #if defined(PIN_SPI_MOSI_PINSWAP_6)
    #define MOSI_ALT6  PIN_SPI_MOSI_PINSWAP_6;
  #else
    #define MOSI_ALT6   NOT_A_PIN;
  #endif
  #if defined(PIN_SPI_MISO_PINSWAP_6)
    #define MISO_ALT6  PIN_SPI_MISO_PINSWAP_6;
  #else
    #define MISO_ALT6   NOT_A_PIN;
  #endif
  #if defined(PIN_SPI_SCK_PINSWAP_6)
    #define SCK_ALT6   PIN_SPI_SCK_PINSWAP_6;
  #else
    #define SCK_ALT6    NOT_A_PIN;
  #endif
#endif

#ifndef SPI_MUX /* special case for the DD14 */
  static const uint8_t SS   = SS_ALT4;
  static const uint8_t MOSI = MOSI_ALT4;
  static const uint8_t MISO = MISO_ALT4;
  static const uint8_t SCK  = SCK_ALT4;
#else
  #if defined(PIN_SPI_SS)
    static const uint8_t SS   = PIN_SPI_SS;
  #else
    static const uint8_t SS   =  NOT_A_PIN;
  #endif
  #if defined(PIN_SPI_MOSI)
    static const uint8_t MOSI = PIN_SPI_MOSI;
  #else
    static const uint8_t MOSI =  NOT_A_PIN;
  #endif
  #if defined(PIN_SPI_MISO)
    static const uint8_t MISO = PIN_SPI_MISO;
  #else
    static const uint8_t MISO =  NOT_A_PIN;
  #endif
  #if defined(PIN_SPI_SCK)
    static const uint8_t SCK  = PIN_SPI_SCK;
  #else
    static const uint8_t SCK  =  NOT_A_PIN;
  #endif
#endif


#define SPI_MUX_NOW() (PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm)

// take advantage of knowledge that DDs don't have 1 or 2, DA/DB don't have 3.
// and we test 4. 6. 3, 5 for the DD's that have 0, 3, 4, 5, 6 first because on DD14, those are checked first and will minimize execution time...
// we can't rule out cases where some but not all of the pins are not present, even essential pins like SCK, because when operating as master, they can use event system to output a clock on EVOUT or something.

#ifdef SPI_MUX_PINSWAP_4
  #define PIN_SS_NOW_3456   (spimux == SPI_MUX_PINSWAP_4 ?   SS_ALT4 : (spimux == SPI_MUX_PINSWAP_6 ?   SS_ALT6  : (spimux == SPI_MUX_PINSWAP_3 ?   SS_ALT3  :   SS_ALT5)))
  #define PIN_MOSI_NOW_3456 (spimux == SPI_MUX_PINSWAP_4 ? MOSI_ALT4 : (spimux == SPI_MUX_PINSWAP_6 ? MOSI_ALT6  : (spimux == SPI_MUX_PINSWAP_3 ? MOSI_ALT3  : MOSI_ALT5)))
  #define PIN_MISO_NOW_3456 (spimux == SPI_MUX_PINSWAP_4 ? MISO_ALT4 : (spimux == SPI_MUX_PINSWAP_6 ? MISO_ALT6  : (spimux == SPI_MUX_PINSWAP_3 ? MISO_ALT3  : MISO_ALT5)))
  #define PIN_SCK_NOW_3456  (spimux == SPI_MUX_PINSWAP_4 ?  SCK_ALT4 : (spimux == SPI_MUX_PINSWAP_6 ?  SCK_ALT6  : (spimux == SPI_MUX_PINSWAP_3 ?  SCK_ALT3  :  SCK_ALT5)))
#endif
#if !defined(__AVR_DD__)
  #ifdef SPI_MUX_PINSWAP_1
    #ifdef SPI_MUX_PINSWAP_2
      #define SS_NOW_IMPL()   ((uint8_t) (spimux == SPI_MUX_PINSWAP_1 ? SS_ALT1    : (spimux == SPI_MUX_PINSWAP_2 ? SS_ALT2    :  PIN_SPI_SS)))
      #define MOSI_NOW_IMPL() ((uint8_t) (spimux == SPI_MUX_PINSWAP_1 ? MOSI_ALT1  : (spimux == SPI_MUX_PINSWAP_2 ? MOSI_ALT2  : PIN_SPI_MOSI)))
      #define MISO_NOW_IMPL() ((uint8_t) (spimux == SPI_MUX_PINSWAP_1 ? MISO_ALT1  : (spimux == SPI_MUX_PINSWAP_2 ? MISO_ALT2  : PIN_SPI_MISO)))
      #define SCK_NOW_IMPL()  ((uint8_t) (spimux == SPI_MUX_PINSWAP_1 ? SCK_ALT1   : (spimux == SPI_MUX_PINSWAP_2 ? SCK_ALT2   : PIN_SPI_SCK)))
    #else // DA/DB with 48 or fewer pins
      #define SS_NOW_IMPL()   ((uint8_t) (spimux == SPI_MUX_PINSWAP_1 ? SS_ALT1    : PIN_SPI_SS))
      #define MOSI_NOW_IMPL() ((uint8_t) (spimux == SPI_MUX_PINSWAP_1 ? MOSI_ALT1  : PIN_SPI_MOSI))
      #define MISO_NOW_IMPL() ((uint8_t) (spimux == SPI_MUX_PINSWAP_1 ? MISO_ALT1  : PIN_SPI_MISO))
      #define SCK_NOW_IMPL()  ((uint8_t) (spimux == SPI_MUX_PINSWAP_1 ? SCK_ALT1   : PIN_SPI_SCK))
    #endif
  #else // No pinswap 1 or 2 - but not a DD
    #define SS_NOW_IMPL()   PIN_SPI_SS
    #define MOSI_NOW_IMPL() PIN_SPI_MOSI
    #define MISO_NOW_IMPL() PIN_SPI_MISO
    #define SCK_NOW_IMPL()  PIN_SPI_SCK
  #endif
#else // default available means it's not a DD14
  #ifdef SPI_MUX
    #define SS_NOW_IMPL()   ((uint8_t) (spimux != SPI_MUX ?   PIN_SS_3456 : PIN_SPI_SS))
    #define MOSI_NOW_IMPL() ((uint8_t) (spimux != SPI_MUX ? PIN_MOSI_3456 : PIN_SPI_MOSI))
    #define MISO_NOW_IMPL() ((uint8_t) (spimux != SPI_MUX ? PIN_MISO_3456 : PIN_SPI_MISO))
    #define SCK_NOW_IMPL()  ((uint8_t) (spimux != SPI_MUX ?  PIN_SCK_3456 : PIN_SPI_SCK))
  #else
    #define SS_NOW_IMPL()   ((uint8_t) PIN_SS_3456
    #define MOSI_NOW_IMPL() ((uint8_t) PIN_MOSI_3456
    #define MISO_NOW_IMPL() ((uint8_t) PIN_MISO_3456
    #define SCK_NOW_IMPL()  ((uint8_t) PIN_SCK_3456
  #endif
#endif

#if !defined(__AVR_DD__) && !defined(SPI_MUX_PINSWAP_1) && !defined(SPI_MUX_PINSWAP_2)
  #define SS_NOW(...)   ({  SS_NOW_IMPL();})
  #define MOSI_NOW(...) ({MOSI_NOW_IMPL();})
  #define MISO_NOW(...) ({MISO_NOW_IMPL();})
  #define SCK_NOW(...)  ({ SCK_NOW_IMPL();})
#else
  #define SS_NOW(...)   ({uint8_t spimux=(PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm);   SS_NOW_IMPL(); })
  #define MOSI_NOW(...) ({uint8_t spimux=(PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm); MOSI_NOW_IMPL(); })
  #define MISO_NOW(...) ({uint8_t spimux=(PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm); MISO_NOW_IMPL(); })
  #define SCK_NOW(...)  ({uint8_t spimux=(PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm);  SCK_NOW_IMPL(); })
#endif

/* SPI1 is easy-peasy */
#ifdef PIN_SPI1_SCK_PINSWAP_1
#if defined(PIN_SPI1_SS_PINSWAP_1)
  #define SS1_ALT1    PIN_SPI1_SS_PINSWAP_1;
#else
  #define SS1_ALT1     NOT_A_PIN;
#endif
#if defined(PIN_SPI1_MOSI_PINSWAP_1)
  #define MOSI1_ALT1  PIN_SPI1_MOSI_PINSWAP_1;
#else
  #define MOSI1_ALT1   NOT_A_PIN;
#endif
#if defined(PIN_SPI1_MISO_PINSWAP_1)
  #define MISO1_ALT1  PIN_SPI1_MISO_PINSWAP_1;
#else
  #define MISO1_ALT1   NOT_A_PIN;
#endif
#if defined(PIN_SPI1_SCK_PINSWAP_1)
  #define SCK1_ALT1   PIN_SPI1_SCK_PINSWAP_1;
#else
  #define SCK1_ALT1    NOT_A_PIN;
#endif
#endif
#ifdef PIN_SPI1_SCK_PINSWAP_2
#if defined(PIN_SPI1_SS_PINSWAP_2)
  #define SS1_ALT2    PIN_SPI1_SS_PINSWAP_2;
#else
  #define SS1_ALT2     NOT_A_PIN;
#endif
#if defined(PIN_SPI1_MOSI_PINSWAP_2)
  #define MOSI1_ALT2  PIN_SPI1_MOSI_PINSWAP_2;
#else
  #define MOSI1_ALT2   NOT_A_PIN;
#endif
#if defined(PIN_SPI1_MISO_PINSWAP_2)
  #define MISO1_ALT2  PIN_SPI1_MISO_PINSWAP_2;
#else
  #define MISO1_ALT2   NOT_A_PIN;
#endif
#if defined(PIN_SPI1_SCK_PINSWAP_2)
  #define SCK1_ALT2   PIN_SPI1_SCK_PINSWAP_2;
#else
  #define SCK1_ALT2    NOT_A_PIN;
#endif
#endif
#if defined(PIN_SPI1_SS)
#define SS1    PIN_SPI1_SS;
#else
#define SS1     NOT_A_PIN;
#endif
#if defined(PIN_SPI1_MOSI)
#define MOSI1  PIN_SPI1_MOSI;
#else
#define MOSI1   NOT_A_PIN;
#endif
#if defined(PIN_SPI1_MISO)
#define MISO1  PIN_SPI1_MISO;
#else
#define MISO1   NOT_A_PIN;
#endif
#if defined(PIN_SPI1_SCK)
#define SCK1   PIN_SPI1_SCK;
#else
#define SCK1    NOT_A_PIN;
#endif

#if defined(SPI1_MUX)
  #ifdef SPI1_MUX_PINSWAP_1
    #ifdef SPI1_MUX_PINSWAP_2
      #define SS1_NOW    ((uint8_t) ((PORTMUX.SPIROUTEA & PORTMUX_SPI1_gm) == SPI1_MUX_PINSWAP_1 ? SS1_ALT1    : ((PORTMUX.SPIROUTEA & PORTMUX_SPI1_gm) == SPI1_MUX_PINSWAP_2 ? SS1_ALT2    : PIN_SPI1_SS)))
      #define MOSI1_NOW  ((uint8_t) ((PORTMUX.SPIROUTEA & PORTMUX_SPI1_gm) == SPI1_MUX_PINSWAP_1 ? MOSI1_ALT1  : ((PORTMUX.SPIROUTEA & PORTMUX_SPI1_gm) == SPI1_MUX_PINSWAP_2 ? MOSI1_ALT2  : PIN_SPI1_MOSI)))
      #define MISO1_NOW  ((uint8_t) ((PORTMUX.SPIROUTEA & PORTMUX_SPI1_gm) == SPI1_MUX_PINSWAP_1 ? MISO1_ALT1  : ((PORTMUX.SPIROUTEA & PORTMUX_SPI1_gm) == SPI1_MUX_PINSWAP_2 ? MISO1_ALT2  : PIN_SPI1_MISO)))
      #define SCK1_NOW   ((uint8_t) ((PORTMUX.SPIROUTEA & PORTMUX_SPI1_gm) == SPI1_MUX_PINSWAP_1 ? SCK1_ALT1   : ((PORTMUX.SPIROUTEA & PORTMUX_SPI1_gm) == SPI1_MUX_PINSWAP_2 ? SCK1_ALT2   : PIN_SPI1_SCK)))
    #else // DA/DB with 48 or fewer pins
      #define SS1_NOW    ((uint8_t) ((PORTMUX.SPIROUTEA & PORTMUX_SPI1_gm) == SPI1_MUX_PINSWAP_1 ? SS1_ALT1    : PIN_SPI1_SS))
      #define MOSI1_NOW  ((uint8_t) ((PORTMUX.SPIROUTEA & PORTMUX_SPI1_gm) == SPI1_MUX_PINSWAP_1 ? MOSI1_ALT1  : PIN_SPI1_MOSI))
      #define MISO1_NOW  ((uint8_t) ((PORTMUX.SPIROUTEA & PORTMUX_SPI1_gm) == SPI1_MUX_PINSWAP_1 ? MISO1_ALT1  : PIN_SPI1_MISO))
      #define SCK1_NOW   ((uint8_t) ((PORTMUX.SPIROUTEA & PORTMUX_SPI1_gm) == SPI1_MUX_PINSWAP_1 ? SCK1_ALT1   : PIN_SPI1_SCK))
    #endif
  #else  // Only one position!
    #define SS1_NOW    PIN_SPI1_SS
    #define MOSI1_NOW  PIN_SPI1_MOSI
    #define MISO1_NOW  PIN_SPI1_MISO
    #define SCK1_NOW   PIN_SPI1_SCK
  #endif
#endif

#endif // PINSWAP_H
