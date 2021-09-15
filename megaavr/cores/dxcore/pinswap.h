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
  #ifdef SCL_ALT3 //all three
    #define SDA_NOW ((uint8_t) ((PORTMUX.TWIROUTEA & PORTMUX_TWI0_gm) == 2 ? SDA_ALT2 : (PORTMUX.TWIROUTEA & PORTMUX_TWI0_gm) == 3 ? SDA_ALT3 : PIN_WIRE_SDA))
    #define SCL_NOW ((uint8_t) ((PORTMUX.TWIROUTEA & PORTMUX_TWI0_gm) == 2 ? SCL_ALT2 : (PORTMUX.TWIROUTEA & PORTMUX_TWI0_gm) == 3 ? SCL_ALT3 : PIN_WIRE_SCL))
  #else // 2, no 3
    #define SDA_NOW ((uint8_t) ((PORTMUX.TWIROUTEA & PORTMUX_TWI0_gm) == 2 ? SDA_ALT2 : PIN_WIRE_SDA))
    #define SCL_NOW ((uint8_t) ((PORTMUX.TWIROUTEA & PORTMUX_TWI0_gm) == 2 ? SCL_ALT2 : PIN_WIRE_SCL))
  #endif
#else   // no 2, 3
  #ifdef SCL_ALT3
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
#ifdef PIN_WIRE1_SCL
  static const uint8_t SDA1 = PIN_WIRE1_SDA;
  static const uint8_t SCL1 = PIN_WIRE1_SCL;
#endif

#ifdef PIN_WIRE1_SCL
  #ifdef PIN_WIRE1_SCL_PINSWAP_2
    #define SDA1_NOW ((uint8_t) ((PORTMUX.TWIROUTEA & PORTMUX_TWI1_gm) == 8 ? SDA1_ALT2 : SDA))
    #define SCL1_NOW ((uint8_t) ((PORTMUX.TWIROUTEA & PORTMUX_TWI1_gm) == 8 ? SCL1_ALT2 : SCL))
  #else
    #define SDA1_NOW (PIN_WIRE1_SDA)
    #define SCL1_NOW (PIN_WIRE1_SCL)
  #endif
#endif

/* Now for a doozy - SPI0 - which has all 7 possible options, and up to 5 on some parts! */

#ifdef PIN_SPI_SCK_PINSWAP_1
  static const uint8_t SS_ALT1   = PIN_SPI_SS_PINSWAP_1;
  static const uint8_t MOSI_ALT1 = PIN_SPI_MOSI_PINSWAP_1;
  static const uint8_t MISO_ALT1 = PIN_SPI_MISO_PINSWAP_1;
  static const uint8_t SCK_ALT1  = PIN_SPI_SCK_PINSWAP_1;
#endif
#ifdef PIN_SPI_SCK_PINSWAP_2
  static const uint8_t SS_ALT2   = PIN_SPI_SS_PINSWAP_2;
  static const uint8_t MOSI_ALT2 = PIN_SPI_MOSI_PINSWAP_2;
  static const uint8_t MISO_ALT2 = PIN_SPI_MISO_PINSWAP_2;
  static const uint8_t SCK_ALT2  = PIN_SPI_SCK_PINSWAP_2;
#endif
#ifdef PIN_SPI_SCK_PINSWAP_3
  static const uint8_t SS_ALT3   = PIN_SPI_SS_PINSWAP_3;
  static const uint8_t MOSI_ALT3 = PIN_SPI_MOSI_PINSWAP_3;
  static const uint8_t MISO_ALT3 = PIN_SPI_MISO_PINSWAP_3;
  static const uint8_t SCK_ALT3  = PIN_SPI_SCK_PINSWAP_3;
#endif
#ifdef PIN_SPI_SCK_PINSWAP_4
  static const uint8_t SS_ALT4   = PIN_SPI_SS_PINSWAP_4;
  static const uint8_t MOSI_ALT4 = PIN_SPI_MOSI_PINSWAP_4;
  static const uint8_t MISO_ALT4 = PIN_SPI_MISO_PINSWAP_4;
  static const uint8_t SCK_ALT4  = PIN_SPI_SCK_PINSWAP_4;
#endif
#ifdef PIN_SPI_SCK_PINSWAP_5
  static const uint8_t SS_ALT5   = PIN_SPI_SS_PINSWAP_5;
  static const uint8_t MOSI_ALT5 = PIN_SPI_MOSI_PINSWAP_5;
  static const uint8_t MISO_ALT5 = PIN_SPI_MISO_PINSWAP_5;
  static const uint8_t SCK_ALT5  = PIN_SPI_SCK_PINSWAP_5;
#endif
#ifdef PIN_SPI_SCK_PINSWAP_6
  static const uint8_t SS_ALT6   = PIN_SPI_SS_PINSWAP_6;
  static const uint8_t MOSI_ALT6 = PIN_SPI_MOSI_PINSWAP_6;
  static const uint8_t MISO_ALT6 = PIN_SPI_MISO_PINSWAP_6;
  static const uint8_t SCK_ALT6  = PIN_SPI_SCK_PINSWAP_6;
#endif
#ifndef SPI_MUX /* special case for the DD14 */
  static const uint8_t SS   = SS_ALT4;
  static const uint8_t MOSI = MOSI_ALT4;
  static const uint8_t MISO = MISO_ALT4;
  static const uint8_t SCK  = SCK_ALT4;
#else
  static const uint8_t SS   = PIN_SPI_SS;
  static const uint8_t MOSI = PIN_SPI_MOSI;
  static const uint8_t MISO = PIN_SPI_MISO;
  static const uint8_t SCK  = PIN_SPI_SCK;
#endif

// take advantage of knowledge that DDs don't have 1 or 2, DA/DB don't have 3+
// and we test 4. 6. 3, 5 for the DD's that have 0, 3, 4, 5, 6 first because on DD14, those are checked first and will minimize execution time...
// we can't rule out cases where some but not all of the pins are not present, even essential pins like SCK, because when operating as master, they can use event system to output a clock on EVOUT or something.

#ifdef SPI_MUX_PINSWAP_4
  #define PIN_SS_NOW_3456   ((PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm) == SPI_MUX_PINSWAP_4 ?   SS_ALT4 : ((PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm) == SPI_MUX_PINSWAP_6 ?   SS_ALT6  : ((PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm) == SPI_MUX_PINSWAP_3 ?   SS_ALT3  :   SS_ALT5)))
  #define PIN_MOSI_NOW_3456 ((PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm) == SPI_MUX_PINSWAP_4 ? MOSI_ALT4 : ((PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm) == SPI_MUX_PINSWAP_6 ? MOSI_ALT6  : ((PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm) == SPI_MUX_PINSWAP_3 ? MOSI_ALT3  : MOSI_ALT5)))
  #define PIN_MISO_NOW_3456 ((PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm) == SPI_MUX_PINSWAP_4 ? MISO_ALT4 : ((PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm) == SPI_MUX_PINSWAP_6 ? MISO_ALT6  : ((PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm) == SPI_MUX_PINSWAP_3 ? MISO_ALT3  : MISO_ALT5)))
  #define PIN_SCK_NOW_3456  ((PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm) == SPI_MUX_PINSWAP_4 ?  SCK_ALT4 : ((PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm) == SPI_MUX_PINSWAP_6 ?  SCK_ALT6  : ((PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm) == SPI_MUX_PINSWAP_3 ?  SCK_ALT3  :  SCK_ALT5)))
#endif
#if !defined(__AVR_DD__)
  #ifdef SPI_MUX_PINSWAP_1
    #ifdef SPI_MUX_PINSWAP_2
      #define SS_NOW    ((uint8_t) ((PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm) == SPI_MUX_PINSWAP_1 ? SS_ALT1    : ((PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm) == SPI_MUX_PINSWAP_2 ? SS_ALT2    :  PIN_SPI_SS)))
      #define MOSI_NOW  ((uint8_t) ((PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm) == SPI_MUX_PINSWAP_1 ? MOSI_ALT1  : ((PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm) == SPI_MUX_PINSWAP_2 ? MOSI_ALT2  : PIN_SPI_MOSI)))
      #define MISO_NOW  ((uint8_t) ((PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm) == SPI_MUX_PINSWAP_1 ? MISO_ALT1  : ((PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm) == SPI_MUX_PINSWAP_2 ? MISO_ALT2  : PIN_SPI_MISO)))
      #define SCK_NOW   ((uint8_t) ((PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm) == SPI_MUX_PINSWAP_1 ? SCK_ALT1   : ((PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm) == SPI_MUX_PINSWAP_2 ? SCK_ALT2   : PIN_SPI_SCK)))
    #else // DA/DB with 48 or fewer pins
      #define SS_NOW    ((uint8_t) ((PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm) == SPI_MUX_PINSWAP_1 ? SS_ALT1    : PIN_SPI_SS))
      #define MOSI_NOW  ((uint8_t) ((PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm) == SPI_MUX_PINSWAP_1 ? MOSI_ALT1  : PIN_SPI_MOSI))
      #define MISO_NOW  ((uint8_t) ((PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm) == SPI_MUX_PINSWAP_1 ? MISO_ALT1  : PIN_SPI_MISO))
      #define SCK_NOW   ((uint8_t) ((PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm) == SPI_MUX_PINSWAP_1 ? SCK_ALT1   : PIN_SPI_SCK))
    #endif
  #else // No pinswap 1 or 2 - but not a DD
    #define SS_NOW    PIN_SPI_SS
    #define MOSI_NOW  PIN_SPI_MOSI
    #define MISO_NOW  PIN_SPI_MISO
    #define SCK_NOW   PIN_SPI_SCK
  #endif
#else // default available means it's not a DD14
  #ifdef SPI_MUX
    #define SS_NOW    ((uint8_t) ((PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm) != SPI_MUX ?   PIN_SS_3456 : PIN_SPI_SS))
    #define MOSI_NOW  ((uint8_t) ((PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm) != SPI_MUX ? PIN_MOSI_3456 : PIN_SPI_MOSI))
    #define MISO_NOW  ((uint8_t) ((PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm) != SPI_MUX ? PIN_MISO_3456 : PIN_SPI_MISO))
    #define SCK_NOW   ((uint8_t) ((PORTMUX.SPIROUTEA & PORTMUX_SPI0_gm) != SPI_MUX ?  PIN_SCK_3456 : PIN_SPI_SCK))
  #else
    #define SS_NOW    ((uint8_t) PIN_SS_3456
    #define MOSI_NOW  ((uint8_t) PIN_MOSI_3456
    #define MISO_NOW  ((uint8_t) PIN_MISO_3456
    #define SCK_NOW   ((uint8_t) PIN_SCK_3456
  #endif
#endif

/* SPI1 is easy-peasy */

#ifdef PIN_SPI1_SCK_PINSWAP_1
  static const uint8_t SS1_ALT1   = PIN_SPI1_SS_PINSWAP_1;
  static const uint8_t MOSI1_ALT1 = PIN_SPI1_MOSI_PINSWAP_1;
  static const uint8_t MISO1_ALT1 = PIN_SPI1_MISO_PINSWAP_1;
  static const uint8_t SCK1_ALT1  = PIN_SPI1_SCK_PINSWAP_1;
#endif
#ifdef PIN_SPI1_SCK_PINSWAP_2
  static const uint8_t SS1_ALT2   = PIN_SPI1_SS_PINSWAP_2;
  static const uint8_t MOSI1_ALT2 = PIN_SPI1_MOSI_PINSWAP_2;
  static const uint8_t MISO1_ALT2 = PIN_SPI1_MISO_PINSWAP_2;
  static const uint8_t SCK1_ALT2  = PIN_SPI1_SCK_PINSWAP_2;
#endif
static const uint8_t SS1   = PIN_SPI1_SS;
static const uint8_t MOSI1 = PIN_SPI1_MOSI;
static const uint8_t MISO1 = PIN_SPI1_MISO;
static const uint8_t SCK1  = PIN_SPI1_SCK;

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
  #else  //Only one position!
    #define SS1_NOW    PIN_SPI1_SS
    #define MOSI1_NOW  PIN_SPI1_MOSI
    #define MISO1_NOW  PIN_SPI1_MISO
    #define SCK1_NOW   PIN_SPI1_SCK
  #endif
#endif

// Hardware Serial

typedef enum PIN_HWSERIAL_enum
{
  PIN_TX    = (0),
  PIN_RX    = (1),
  PIN_XCK   = (2),
  PIN_XDIR  = (3),
  MUX_VALUE = (4)
} PIN_HWSERIAL_t;

typedef enum MUX_HWSERIAL_enum
{
  MUX_DEFAULT = (0x00),
  MUX_ALT1    = (0x01),
  MUX_ALT2    = (0x02),
  MUX_ALT3    = (0x03),
  MUX_ALT4    = (0x04),
  MUX_ALT5    = (0x05),
  MUX_ALT6    = (0x06),
  MUX_NONE    = (0xF7)
} MUX_HWSERIAL_t;
/*
typedef struct usart_pinset_struct {
  uint8_t mux_gc;
  uint8_t tx;
  uint8_t rx;
  uint8_t xck;
  uint8_t xdir;
} usart_pinset_t;
*/
typedef struct usart_port_struct {
  volatile uint8_t *muxreg;
  uint8_t mux_gm;
  uint8_t mux_count;
  uint8_t mux_none;
  uint8_t mux_none_gc;
  const uint8_t *pins;
} usart_port_t;

bool hwserial_mux(uint8_t port_num, MUX_HWSERIAL_t muxlevel, bool enact);
void write_hwserial_mux(uint8_t port_num, uint8_t muxlevel);
uint8_t mux_from_pins(uint8_t port_num, uint8_t tx, uint8_t rx);
uint8_t getPin(uint8_t port_num, MUX_HWSERIAL_t muxlevel, PIN_HWSERIAL_t pin);



/* I don't thiiink we want this? */
#if defined(DEFINE_UNUSED_Pxn_PINS)
  #ifndef PIN_PA0
    #define PIN_PA0 NOT_A_PIN
  #endif
  #ifndef PIN_PA1
    #define PIN_PA1 NOT_A_PIN
  #endif
  #ifndef PIN_PA2
    #define PIN_PA2 NOT_A_PIN
  #endif
  #ifndef PIN_PA3
    #define PIN_PA3 NOT_A_PIN
  #endif
  #ifndef PIN_PA4
    #define PIN_PA4 NOT_A_PIN
  #endif
  #ifndef PIN_PA5
    #define PIN_PA5 NOT_A_PIN
  #endif
  #ifndef PIN_PA6
    #define PIN_PA6 NOT_A_PIN
  #endif
  #ifndef PIN_PA7
    #define PIN_PA7 NOT_A_PIN
  #endif
  #ifndef PIN_PB0
    #define PIN_PB0 NOT_A_PIN
  #endif
  #ifndef PIN_PB1
    #define PIN_PB1 NOT_A_PIN
  #endif
  #ifndef PIN_PB2
    #define PIN_PB2 NOT_A_PIN
  #endif
  #ifndef PIN_PB3
    #define PIN_PB3 NOT_A_PIN
  #endif
  #ifndef PIN_PB4
    #define PIN_PB4 NOT_A_PIN
  #endif
  #ifndef PIN_PB5
    #define PIN_PB5 NOT_A_PIN
  #endif
  #ifndef PIN_PB6
    #define PIN_PB6 NOT_A_PIN
  #endif
  #ifndef PIN_PB7
    #define PIN_PB7 NOT_A_PIN
  #endif
  #ifndef PIN_PC0
    #define PIN_PC0 NOT_A_PIN
  #endif
  #ifndef PIN_PC1
    #define PIN_PC1 NOT_A_PIN
  #endif
  #ifndef PIN_PC2
    #define PIN_PC2 NOT_A_PIN
  #endif
  #ifndef PIN_PC3
    #define PIN_PC3 NOT_A_PIN
  #endif
  #ifndef PIN_PC4
    #define PIN_PC4 NOT_A_PIN
  #endif
  #ifndef PIN_PC5
    #define PIN_PC5 NOT_A_PIN
  #endif
  #ifndef PIN_PC6
    #define PIN_PC6 NOT_A_PIN
  #endif
  #ifndef PIN_PC7
    #define PIN_PC7 NOT_A_PIN
  #endif
  #ifndef PIN_PD0
    #define PIN_PD0 NOT_A_PIN
  #endif
  #ifndef PIN_PD1
    #define PIN_PD1 NOT_A_PIN
  #endif
  #ifndef PIN_PD2
    #define PIN_PD2 NOT_A_PIN
  #endif
  #ifndef PIN_PD3
    #define PIN_PD3 NOT_A_PIN
  #endif
  #ifndef PIN_PD4
    #define PIN_PD4 NOT_A_PIN
  #endif
  #ifndef PIN_PD5
    #define PIN_PD5 NOT_A_PIN
  #endif
  #ifndef PIN_PD6
    #define PIN_PD6 NOT_A_PIN
  #endif
  #ifndef PIN_PD7
    #define PIN_PD7 NOT_A_PIN
  #endif
  #ifndef PIN_PE0
    #define PIN_PE0 NOT_A_PIN
  #endif
  #ifndef PIN_PE1
    #define PIN_PE1 NOT_A_PIN
  #endif
  #ifndef PIN_PE2
    #define PIN_PE2 NOT_A_PIN
  #endif
  #ifndef PIN_PE3
    #define PIN_PE3 NOT_A_PIN
  #endif
  #ifndef PIN_PE4
    #define PIN_PE4 NOT_A_PIN
  #endif
  #ifndef PIN_PE5
    #define PIN_PE5 NOT_A_PIN
  #endif
  #ifndef PIN_PE6
    #define PIN_PE6 NOT_A_PIN
  #endif
  #ifndef PIN_PE7
    #define PIN_PE7 NOT_A_PIN
  #endif
  #ifndef PIN_PF0
    #define PIN_PF0 NOT_A_PIN
  #endif
  #ifndef PIN_PF1
    #define PIN_PF1 NOT_A_PIN
  #endif
  #ifndef PIN_PF2
    #define PIN_PF2 NOT_A_PIN
  #endif
  #ifndef PIN_PF3
    #define PIN_PF3 NOT_A_PIN
  #endif
  #ifndef PIN_PF4
    #define PIN_PF4 NOT_A_PIN
  #endif
  #ifndef PIN_PF5
    #define PIN_PF5 NOT_A_PIN
  #endif
  #ifndef PIN_PF6
    #define PIN_PF6 NOT_A_PIN
  #endif
  #ifndef PIN_PF7
    #define PIN_PF7 NOT_A_PIN
  #endif
  #ifndef PIN_PG0
    #define PIN_PG0 NOT_A_PIN
  #endif
  #ifndef PIN_PG1
    #define PIN_PG1 NOT_A_PIN
  #endif
  #ifndef PIN_PG2
    #define PIN_PG2 NOT_A_PIN
  #endif
  #ifndef PIN_PG3
    #define PIN_PG3 NOT_A_PIN
  #endif
  #ifndef PIN_PG4
    #define PIN_PG4 NOT_A_PIN
  #endif
  #ifndef PIN_PG5
    #define PIN_PG5 NOT_A_PIN
  #endif
  #ifndef PIN_PG6
    #define PIN_PG6 NOT_A_PIN
  #endif
  #ifndef PIN_PG7
    #define PIN_PG7 NOT_A_PIN
  #endif
#endif
#endif //PINSWAP_H
