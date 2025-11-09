/*  (C) Spence Konde 2021-2025 open source (LGPL2.1 see LICENSE.md) based on existing Arduino cores.*/
//                                                                                    *INDENT-OFF*
/*
 ###  #     # ####      ####  ####        #   #  #
#   # #     # #   #     #     #   #      ##   #  #          #
#####  #   #  ####      ###   ####        #   #### ### ###     ###
#   #   # #   #  #      #     #   #       #      #     #  # #  #  #
#   #    #    #   #     ####  ####       ###     #     ###  #  #  #
===================================      ---------     #
Variant Definition file for generic EB parts           #
with 14 pins.

Part Numbers:
AVR32EB14 AVR16EB14

See VariantTemplate.h in extras folder an extensively annotated copy.

Include guard and include basic libraries. We are normally including this inside Arduino.h
*/

#ifndef Pins_Arduino_h
#define Pins_Arduino_h
#include <avr/pgmspace.h>
#include "timers.h"

#define EB_14PIN_PINOUT

        /*##  ### #   #  ###
        #   #  #  ##  # #
        ####   #  # # #  ###
        #      #  #  ##     #
        #     ### #   #  #*/

/* The extreme low pincount, and desire to not have the majority of
 * the pin tables filled with NOT_A_PIN/NOT_ON_TIMER/etc nulls dictates a
 * more constrained pin numbering scheme.
 * PD0 and PD1, PD2, and PD3 are empty holes, but the there is no hole for PF0.
 */

#define PIN_PA0 (0)
#define PIN_PA1 (1)
// No PA2 (2)
// No PA3 (3)
// No PA4 (4)
// No PA5 (5)
// No PA6 (6)
// No PA7 (7)
#define PIN_PC0 (8)
#define PIN_PC1 (9)
#define PIN_PC2 (10)
#define PIN_PC3 (11)  // A31
#define PIN_PD0 (12)  // NOT_A_PIN
// No PD1 (13)
// No PD2 (14)
// No PD3 (15)
#define PIN_PD4 (16)
#define PIN_PD5 (17)
#define PIN_PD6 (18)
#define PIN_PD7 (19)
#define PIN_PF6 (20) // RESET
#define PIN_PF7 (21) // UPDI

#define FAKE_PD0

        /*##   ##   ###  ###  ###  ###
        #   # #  # #      #  #    #
        ####  ####  ###   #  #     ###
        #   # #  #     #  #  #        #
        ####  #  # ####  ###  ###  #*/

#define PINS_COUNT                        (12)
#define NUM_ANALOG_INPUTS                 ( 8)

#if !defined(LED_BUILTIN)
  #define LED_BUILTIN                     (PIN_PD6)
#endif
/* Until the legacy attach interrupt has been completely obsoleted - this is such a waste here! */
#ifdef CORE_ATTACH_OLD
  #define EXTERNAL_NUM_INTERRUPTS         (48)
#endif

       /*   #  ###   ### ####   ###   ###
        ## ## #   # #    #   # #   # #
        # # # ##### #    ####  #   #  ###
        #   # #   # #    # #   #   #     #
        #   # #   #  ### #  #   ###   ##*/
#define IS_MVIO_ENABLED()              (0)
#define digitalPinToAnalogInput(p)     (((p) >= PIN_PD4 && (p) <= PIN_PD7) ? (p) - PIN_PD0 : (((p) > PIN_PC0 && (p) <= PIN_PC3) ? (p) + 20 : NOT_A_PIN))
#define analogChannelToDigitalPin(p)   (((p) <  8       && (p) > 3       ) ? (p) + PIN_PD0 : (((p) >= 28     || (p) <= 31)    ) ? (p) - 20 : NOT_A_PIN)
#define analogInputToDigitalPin(p)                        analogChannelToDigitalPin((p) & 0x7F)
#define digitalOrAnalogPinToDigital(p)    (((p) & 0x80) ? analogChannelToDigitalPin((p) & 0x7F) : digitalPinToBitMask(p) ? (p) : NOT_A_PIN)
#define portToPinZero(port)               ((port) == PD ? PIN_PD0: ((port)== PC ? PIN_PC0 : ((port)== PA ? PIN_PA0 : NOT_A_PIN)))


// PWM pins


#define digitalPinHasPWMTCB(p) (false)      // Not a single TCB timer output pin available!


// Timer pin mapping



#define TCB0_PINS           (0x00)                  // TCB0 output on PA2 (Doesn't exist here) or PF4 (Doesn't exist here)? - decisions decisions!
#define TCB1_PINS           (0x00)                  // TCB0 output on PA3 (Doesn't exist here) or PF5 (Doesn't exist here)? - decisions decisions!

#define TCE0_PINS           (0x02)                  // PORTC is clearly the right port on a 14-pin part.
#define TCF0_PINS           (0x00)                  // Connect pins that exist to TCF.
#define PIN_TCB0_WO_INIT    (NOT_A_PIN)
#define PIN_TCB1_WO_INIT    (NOT_A_PIN)


#define digitalPinHasPWM(p)               (((p) == PIN_PA0) || ((p) == PIN_PA1)) // TCE PWM is handled by digitalPinHasPWMnow()

        /*##   ###  ####  ##### #   # #   # #   #
        #   # #   # #   #   #   ## ## #   #  # #
        ####  #   # ####    #   # # # #   #   #
        #     #   # #  #    #   #   # #   #  # #
        #      ###  #   #   #   #   #  ###  #   */


#define SPI_INTERFACES_COUNT            (1)

#define SPI_MUX_PINSWAP_4               (0x04)
#define SPI_MUX_PINSWAP_5               (0x05)
#define SPI_MUX_PINSWAP_6               (0x06)
#define SPI_MUX_PINSWAP_NONE            (PORTMUX_SPI0_NONE_gc)
#define PIN_SPI_MOSI                    (NOT_A_PIN)
#define PIN_SPI_MISO                    (NOT_A_PIN)
#define PIN_SPI_SCK                     (NOT_A_PIN)
#define PIN_SPI_SS                      (NOT_A_PIN)
#define PIN_SPI_MOSI_PINSWAP_4          (PIN_PD4)
#define PIN_SPI_MISO_PINSWAP_4          (PIN_PD5)
#define PIN_SPI_SCK_PINSWAP_4           (PIN_PD6)
#define PIN_SPI_SS_PINSWAP_4            (PIN_PD7)
#define PIN_SPI_MOSI_PINSWAP_5          (PIN_PC0)
#define PIN_SPI_MISO_PINSWAP_5          (PIN_PC1)
#define PIN_SPI_SCK_PINSWAP_5           (PIN_PC2)
#define PIN_SPI_SS_PINSWAP_5            (PIN_PC3)
#define PIN_SPI_MOSI_PINSWAP_6          (PIN_PC1)
#define PIN_SPI_MISO_PINSWAP_6          (PIN_PC2)
#define PIN_SPI_SCK_PINSWAP_6           (PIN_PC3)
#define PIN_SPI_SS_PINSWAP_6            (PIN_PF7)


// TWI 0
#define PIN_WIRE_SDA                    (NOT_A_PIN)
#define PIN_WIRE_SCL                    (NOT_A_PIN)
#define PIN_WIRE_SDA_PINSWAP_2          (PIN_PC2)
#define PIN_WIRE_SCL_PINSWAP_2          (PIN_PC3)
#define PIN_WIRE_SDA_PINSWAP_3          (PIN_PA0)
#define PIN_WIRE_SCL_PINSWAP_3          (PIN_PA1)

// USART 0
#define HWSERIAL0_MUX                   (0x00 /* PORTMUX_USART0_DEFAULT_gc */)
#define HWSERIAL0_MUX_PINSWAP_3         (0x03 /* PORTMUX_USART0_ALT3_gc */)
#define HWSERIAL0_MUX_PINSWAP_4         (0x04 /* PORTMUX_USART0_ALT4_gc */)
#define HWSERIAL0_MUX_PINSWAP_5         (0x06 /* PORTMUX_USART0_ALT6_gc */)
#define HWSERIAL0_MUX_PINSWAP_NONE      (0x07)
#define PIN_HWSERIAL0_TX                (PIN_PA0)
#define PIN_HWSERIAL0_RX                (PIN_PA1)
#define PIN_HWSERIAL0_XCK               (NOT_A_PIN)
#define PIN_HWSERIAL0_XDIR              (NOT_A_PIN)
#define PIN_HWSERIAL0_TX_PINSWAP_3      (PIN_PD4)
#define PIN_HWSERIAL0_RX_PINSWAP_3      (PIN_PD5)
#define PIN_HWSERIAL0_XCK_PINSWAP_3     (PIN_PD6)
#define PIN_HWSERIAL0_XDIR_PINSWAP_3    (PIN_PD7)
#define PIN_HWSERIAL0_TX_PINSWAP_4      (PIN_PC1)
#define PIN_HWSERIAL0_RX_PINSWAP_4      (PIN_PC2)
#define PIN_HWSERIAL0_XCK_PINSWAP_4     (PIN_PC3)
#define PIN_HWSERIAL0_XDIR_PINSWAP_4    (NOT_A_PIN)
#define PIN_HWSERIAL0_TX_PINSWAP_5      (PIN_PF7)
#define PIN_HWSERIAL0_RX_PINSWAP_5      (PIN_PF6)
#define PIN_HWSERIAL0_XCK_PINSWAP_5     (NOT_A_PIN)
#define PIN_HWSERIAL0_XDIR_PINSWAP_5    (NOT_A_PIN)

        /*##  #   #  ###  #     ###   ###      ####  ### #   #  ###
        #   # ##  # #   # #    #   # #         #   #  #  ##  # #
        ##### # # # ##### #    #   # #  ##     ####   #  # # #  ###
        #   # #  ## #   # #    #   # #   #     #      #  #  ##     #
        #   # #   # #   # ####  ###   ###      #     ### #   #  #*/

#define PIN_A0            (NOT_A_PIN)
#define PIN_A1            (NOT_A_PIN)
#define PIN_A2            (NOT_A_PIN)
#define PIN_A3            (NOT_A_PIN)
#define PIN_A4            (PIN_PD4)
#define PIN_A5            (PIN_PD5)
#define PIN_A6            (PIN_PD6)
#define PIN_A7            (PIN_PD7)
#define PIN_A8            (NOT_A_PIN)
#define PIN_A9            (NOT_A_PIN)
#define PIN_A10           (NOT_A_PIN)
#define PIN_A11           (NOT_A_PIN)
#define PIN_A12           (NOT_A_PIN)
#define PIN_A13           (NOT_A_PIN)
#define PIN_A14           (NOT_A_PIN)
#define PIN_A15           (NOT_A_PIN)
#define PIN_A16           (NOT_A_PIN)
#define PIN_A17           (NOT_A_PIN)
#define PIN_A18           (NOT_A_PIN)
#define PIN_A19           (NOT_A_PIN)
#define PIN_A20           (NOT_A_PIN)
#define PIN_A21           (NOT_A_PIN)
#define PIN_A22           (NOT_A_PIN)
#define PIN_A23           (NOT_A_PIN)
#define PIN_A24           (NOT_A_PIN)
#define PIN_A25           (NOT_A_PIN)
#define PIN_A26           (NOT_A_PIN)
#define PIN_A27           (NOT_A_PIN)
#define PIN_A28           (PIN_PC0)
#define PIN_A29           (PIN_PC1)
#define PIN_A30           (PIN_PC2)
#define PIN_A31           (PIN_PC3)

static const uint8_t A0  = NOT_A_PIN;
static const uint8_t A1  = NOT_A_PIN;
static const uint8_t A2  = NOT_A_PIN;
static const uint8_t A3  = NOT_A_PIN;
static const uint8_t A4  = PIN_A4;
static const uint8_t A5  = PIN_A5;
static const uint8_t A6  = PIN_A6;
static const uint8_t A7  = PIN_A7;
static const uint8_t A8  = NOT_A_PIN;
static const uint8_t A9  = NOT_A_PIN;
static const uint8_t A10 = NOT_A_PIN;
static const uint8_t A11 = NOT_A_PIN;
static const uint8_t A12 = NOT_A_PIN;
static const uint8_t A13 = NOT_A_PIN;
static const uint8_t A14 = NOT_A_PIN;
static const uint8_t A15 = NOT_A_PIN;
static const uint8_t A16 = NOT_A_PIN;
static const uint8_t A17 = NOT_A_PIN;
static const uint8_t A18 = NOT_A_PIN;
static const uint8_t A19 = NOT_A_PIN;
static const uint8_t A20 = NOT_A_PIN;
static const uint8_t A21 = NOT_A_PIN;
static const uint8_t A22 = NOT_A_PIN;
static const uint8_t A23 = NOT_A_PIN;
static const uint8_t A24 = NOT_A_PIN;
static const uint8_t A25 = NOT_A_PIN;
static const uint8_t A26 = NOT_A_PIN;
static const uint8_t A27 = NOT_A_PIN;
static const uint8_t A28 = PIN_A28;
static const uint8_t A29 = PIN_A29;
static const uint8_t A30 = PIN_A30;
static const uint8_t A31 = PIN_A31;

#define AIN4               ADC_CH(4)
#define AIN5               ADC_CH(5)
#define AIN6               ADC_CH(6)
#define AIN7               ADC_CH(7)
#define AIN28              ADC_CH(28)
#define AIN29              ADC_CH(29)
#define AIN30              ADC_CH(30)
#define AIN31              ADC_CH(31)


        /*##  ### #   #      ###  ####  ####   ###  #   #  ###
        #   #  #  ##  #     #   # #   # #   # #   #  # #  #
        ####   #  # # #     ##### ####  ####  #####   #    ###
        #      #  #  ##     #   # #  #  #  #  #   #   #       #
        #     ### #   #     #   # #   # #   # #   #   #    #*/

#ifdef ARDUINO_MAIN
  const uint8_t digital_pin_to_port[] = {
    PA,         //  0 PA0/USART0_Tx/CLKIN
    PA,         //  1 PA1/USART0_Rx
    NOT_A_PORT, //  2 NOT_A_PIN
    NOT_A_PORT, //  3 NOT_A_PIN
    NOT_A_PORT, //  4 NOT_A_PIN
    NOT_A_PORT, //  5 NOT_A_PIN
    NOT_A_PORT, //  6 NOT_A_PIN
    NOT_A_PORT, //  7 NOT_A_PIN
    PC,         //  8 PC0
    PC,         //  9 PC1
    PC,         // 10 PC2
    PC,         // 11 PC3
    PD,         // 12 NOT_A_PIN
    NOT_A_PORT, // 13 NOT_A_PIN
    NOT_A_PORT, // 14 NOT_A_PIN
    NOT_A_PORT, // 15 NOT_A_PIN
    PD,         // 16 PD4/AIN4
    PD,         // 17 PD5/AIN5
    PD,         // 18 PD6/AIN6
    PD,         // 19 PD7/AIN7/AREF
    PF,         // 20 PF6 RESET
    PF          // 21 PF7 UPDI
  };

  /* Use this for accessing PINnCTRL register */
  const uint8_t digital_pin_to_bit_position[] = { // *INDENT-OFF*
    #if ((CLOCK_SOURCE & 0x03) == 0) // PA0 used for external clock
      PIN0_bp,//   0 PA0
    #else
      NOT_A_PIN,
    #endif    // *INDENT-ON*=
    PIN1_bp,    // 1 PA1
    NOT_A_PIN,
    NOT_A_PIN,
    NOT_A_PIN,
    NOT_A_PIN,
    NOT_A_PIN,
    NOT_A_PIN,
    PIN0_bp,   //  2 PC0
    PIN1_bp,   //  3 PC1
    PIN2_bp,   //  4 PC2
    PIN3_bp,   //  5 PC3
    NOT_A_PIN, //  6 PD0 Phantom pin
    NOT_A_PIN, //
    NOT_A_PIN, //
    NOT_A_PIN, //
    PIN4_bp,   // 10 PD4/AIN4
    PIN5_bp,   // 11 PD5/AIN5
    PIN6_bp,   // 12 PD6/AIN6
    PIN7_bp,   // 13 PD7/AIN7/AREF
    PIN6_bp,   // 14 PF6 RESET
    PIN7_bp    // 15 PD7 UPDI
  };

  const uint8_t digital_pin_to_bit_mask[] = { // *INDENT-OFF*
    #if ((CLOCK_SOURCE & 0x03) == 0) // PA0 used for external clock
      PIN0_bm, //  0 PA0
    #else
      NOT_A_PIN,
    #endif     // *INDENT-ON*
    PIN1_bm,   //  1 PA1
    NOT_A_PIN,
    NOT_A_PIN,
    NOT_A_PIN,
    NOT_A_PIN,
    NOT_A_PIN,
    NOT_A_PIN,
    PIN0_bm,   //  8 PC0
    PIN1_bm,   //  9 PC1
    PIN2_bm,   // 10 PC2
    PIN3_bm,   // 11 PC3
    NOT_A_PIN, //
    NOT_A_PIN, //
    NOT_A_PIN, //
    NOT_A_PIN, //
    PIN4_bm,   // 16 PD4/AIN4
    PIN5_bm,   // 17 PD5/AIN5
    PIN6_bm,   // 18 PD6/AIN6
    PIN7_bm,   // 19 PD7/AIN7/AREF
    PIN6_bm,   // 20 PF6 RESET
    PIN7_bm    // 21 PF7 UPDI
  };

const uint8_t digital_pin_to_timer[] = { // *INDENT-OFF*
  #if ((CLOCK_SOURCE & 0x03) == 0) // PA0 used for external clock
    TIMERF0_0WO0, //  0 PA0
  #else
    NOT_A_PIN,
  #endif     // *INDENT-ON*
  TIMERF0_0WO1, //   1 PA1/USART0_Rx
  NOT_ON_TIMER, //   NOT_A_PIN
  NOT_ON_TIMER, //   NOT_A_PIN
  NOT_ON_TIMER, //   NOT_A_PIN
  NOT_ON_TIMER, //   NOT_A_PIN
  NOT_ON_TIMER, //   NOT_A_PIN
  NOT_ON_TIMER, //   NOT_A_PIN
  NOT_ON_TIMER, //   8 PC0
  NOT_ON_TIMER, //   9 PC1
  NOT_ON_TIMER, //  10 PC2
  NOT_ON_TIMER, //  11 PC3
  NOT_ON_TIMER, //   NOT_A_PIN
  NOT_ON_TIMER, //   NOT_A_PIN
  NOT_ON_TIMER, //   NOT_A_PIN
  NOT_ON_TIMER, //   NOT_A_PIN
  NOT_ON_TIMER, //  16 PD4/AIN4
  NOT_ON_TIMER, //  17 PD5/AIN5
  NOT_ON_TIMER, //  18 PD6/AIN6
  NOT_ON_TIMER, //  19 PD7/AIN7/AREF
  NOT_ON_TIMER, //  20 PF6 RESET
  NOT_ON_TIMER  //  21 PF7 UPDI
};

// These are used for CI testing. They should *not* *ever* be used except for CI-testing where we need to pick a viable pin to compile for
#if ((CLOCK_SOURCE & 0x03) == 0)
  #define _VALID_DIGITAL_PIN(pin)  ((pin) >= 0  ? ((pin) == 4) ? ((pin) + PIN_PD4) : NOT_A_PIN)
#else
  #define _VALID_DIGITAL_PIN(pin)  ((pin) == 0 || ((pin) == 1) ? (pin) : (pin) < 4 ? (PIN_PC0 + (pin)) : NOT_A_PIN)
#endif
#define    _VALID_ANALOG_PIN(pin)  ((pin) >= 0 && ((pin) <= 4) ?                     ((pin) + PIN_PD4) : NOT_A_PIN)


#endif
#endif
