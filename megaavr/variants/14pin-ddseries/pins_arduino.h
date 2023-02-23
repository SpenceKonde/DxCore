/*  (C) Spence Konde 2021-2022 open source (LGPL2.1 see LICENSE.md) based on existing Arduino cores.*/
//                                                                                    *INDENT-OFF*
/*
 ###  #     # ####      ####  ####        #   #  #
#   # #     # #   #     #   # #   #      ##   #  #          #
#####  #   #  ####      #   # #   #       #   #### ### ###     ###
#   #   # #   #  #      #   # #   #       #      #     #  # #  #  #
#   #    #    #   #     ####  ####       ###     #     ###  #  #  #
===================================     ----------     #
Variant Definition file for generic DD parts           #
with 14 pins.

Part Numbers:
AVR64DD14 AVR32DD14 AVR16DD14

See VariantTemplate.h in extras folder an extensively annotated copy.

Include guard and include basic libraries. We are normally including this inside Arduino.h
*/

#ifndef Pins_Arduino_h
#define Pins_Arduino_h
#include <avr/pgmspace.h>
#include "timers.h"

#define DD_14PIN_PINOUT

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

#define PIN_PA0 (0)  // Not available if using HF crystal.
#define PIN_PA1 (1)  // Not available if using HF crystal.
// No PA2-PA7 (pins 2-7)   It is debatable whether skipping them was the right move.
// No port B on any DD-series
#define PIN_PC0 (8)  // NOT_A_PIN
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
// No PF0-PF6 with < 28 pins - again because 6 pins being skipped, we are not putting a hole in the numbers here.
#define PIN_PF6 (20) // RESET
#define PIN_PF7 (21) // UPDI

#define FAKE_PIN_PD0
#define FAKE_PIN_PC0

        /*##   ##   ###  ###  ###  ###
        #   # #  # #      #  #    #
        ####  ####  ###   #  #     ###
        #   # #  #     #  #  #        #
        ####  #  # ####  ###  ###  #*/

#define PINS_COUNT                     (11)
#define NUM_ANALOG_INPUTS              ( 4)
#define NUM_TOTAL_PINS                 (21)
// #define NUM_RESERVED_PINS           ( 0)    // These may at your option be defined,
// #define NUM_INTERNALLY_USED_PINS    ( 0)    // They will be filled in with defaults otherwise
// Autocalculated are :
#define NUM_DIGITAL_PINS               (21) //NUM_TOTAL_PINS = highest number of any valid pin. NOT the number of pins!
// TOTAL_FREE_OPINS = PINS_COUNT - NUM_INTERNALLY_USED_PINS
// Count of I2C and SPI pins will be defined as 2 and 3 but not used in further calculations. If you
// for some reason need to change this, define them here. Only ones not defined here get automatically set.

#if !defined(LED_BUILTIN)
  #define LED_BUILTIN                  PIN_PD6 /* warning: gets overridden when using Serial1 on 14-pin parts, as that uses PD4. */
#endif
/* Until the legacy attach interrupt has been completely obsoleted - this is such a waste here! */
#ifdef CORE_ATTACH_OLD
  #define EXTERNAL_NUM_INTERRUPTS        48
#endif

       /*   #  ###   ### ####   ###   ###
        ## ## #   # #    #   # #   # #
        # # # ##### #    ####  #   #  ###
        #   # #   # #    # #   #   #     #
        #   # #   #  ### #  #   ###   ##*/
// If you change the number of pins in any way or if the part has ADC on different pins from the board you are adapting
// you must ensure that these will do what they say they will do.
#if !defined(USING_OPTIBOOT) || defined(ASSUME_MVIO_FUSE) /* When not using a bootloader, we know if MVIO is enabled because the fuse is set on upload */
  #if defined(MVIO_ENABLED) /* MVIO disables ADC on PORTC */
    #define IS_MVIO_ENABLED()              (1)
    #define digitalPinToAnalogInput(p)     (((p) >= PIN_PD4 && (p) <= PIN_PD7) ? (p) - PIN_PD0 : NOT_A_PIN)
    #define analogChannelToDigitalPin(p)   (((p) <  8       && (p) > 3       ) ? (p) + PIN_PD0 : NOT_A_PIN)
  #else
    #define IS_MVIO_ENABLED()              (0)
    #define digitalPinToAnalogInput(p)     (((p) >= PIN_PD4 && (p) <= PIN_PD7) ? ((p) - PIN_PD0) : (((p) > PIN_PC0 &&  (p) <= PIN_PC3)   ? ((p) + 20) : NOT_A_PIN))
    #define analogChannelToDigitalPin(p)   (((p) <  8       && (p) > 3       ) ? ((p) + PIN_PD0) : (((p) < 28              || (p) > 31 ) ? ((p) - 20) : NOT_A_PIN)
  #endif
#else /* If we ARE using a bootloader, we can't be sure if MVIO is enabled :-( */
  // strange indentation chosen intentionally to highlight symmetry
  #define IS_MVIO_ENABLED() ((FUSE.SYSCFG1 & 0x01) == 0)
  #define digitalPinToAnalogInput(p)      ((p) >= PIN_PD4           ?            ((p) >  PIN_PD7 ? NOT_A_PIN : (p) - PIN_PD0) : (((p) > PIN_PA1 && !(IS_MVIO_ENABLED() && (p) >= PIN_PC0) ? ((p) + 20) : NOT_A_PIN)))
  #define analogChannelToDigitalPin(p)    ((p) > (IS_MVIO_ENABLED() ? 27 : 31) || (p) != 28      ? NOT_A_PIN : (p) < 8        ?  ((p) + PIN_PD0) : ( (p) >      21                        ? ((p) - 20) : NOT_A_PIN))
#endif
#define analogInputToDigitalPin(p)                        analogChannelToDigitalPin((p) & 0x7F)
#define digitalOrAnalogPinToDigital(p)    (((p) & 0x80) ? analogChannelToDigitalPin((p) & 0x7F) : digitalPinToBitMask(p) ? (p) : NOT_A_PIN)
#define portToPinZero(port)               ((port) == PD ? PIN_PD0: ((port)== PC ? PIN_PC0 : ((port)== PA ? PIN_PA0 : NOT_A_PIN)))


// PWM pins


#define digitalPinHasPWMTCB(p) (false)      // Not a single TCB timer output pin available!


// Timer pin mapping
#if !defined(_CORE_TCA0_PINS)
  #define TCA0_PINS (0x00)                  // EVERY option here sucks; this one gives us... 3 PWM channels...
#else
  #define TCA0_PINS (_CORE_TCA0_PINS)
#endif
#define TCB0_PINS (0x00)                  // TCB0 output on PA2 (Doesn't exist here) or PF4 (Doesn't exist here)? - decisions decisions!
#define TCB1_PINS (0x00)                  // TCB0 output on PA3 (Doesn't exist here) or PF5 (Doesn't exist here)? - decisions decisions!
#define TCD0_PINS (PORTMUX_TCD0_PORTAD)

#define PIN_TCA0_WO0_INIT (PIN_PA0)
#define PIN_TCB0_WO_INIT  (NOT_A_PIN)
#define PIN_TCB1_WO_INIT  (NOT_A_PIN)
#define PIN_TCD0_WOA_INIT (NOT_A_PIN)

#define NO_GLITCH_TIMERD0

#define digitalPinHasPWM(p)               (((p) >= PIN_PC1 && (p) <= PIN_PC3) || (p) == PIN_PD4 || (p) == PIN_PD5)

        /*##   ###  ####  ##### #   # #   # #   #
        #   # #   # #   #   #   ## ## #   #  # #
        ####  #   # ####    #   # # # #   #   #
        #     #   # #  #    #   #   # #   #  # #
        #      ###  #   #   #   #   #  ###  #   */

#define SPI_INTERFACES_COUNT   1

// In contrast to DA/DB with no pinswap options available, the DD has them in spades!
// defining SPI_MUX_PINSWAP_n is how we signal to SPI.h that a given option is valid.
// SPI 0


#define SPI_MUX_PINSWAP_4      PORTMUX_SPI0_ALT4_gc
#define SPI_MUX_PINSWAP_5      PORTMUX_SPI0_ALT5_gc
#define SPI_MUX_PINSWAP_6      PORTMUX_SPI0_ALT6_gc
#define SPI_MUX_PINSWAP_NONE   PORTMUX_SPI0_NONE_gc
#define PIN_SPI_MOSI           NOT_A_PIN
#define PIN_SPI_MISO           NOT_A_PIN
#define PIN_SPI_SCK            NOT_A_PIN
#define PIN_SPI_SS             NOT_A_PIN
#define PIN_SPI_MOSI_PINSWAP_4 PIN_PD4
#define PIN_SPI_MISO_PINSWAP_4 PIN_PD5
#define PIN_SPI_SCK_PINSWAP_4  PIN_PD6
#define PIN_SPI_SS_PINSWAP_4   PIN_PD7
#define PIN_SPI_MOSI_PINSWAP_5 NOT_A_PIN
#define PIN_SPI_MISO_PINSWAP_5 PIN_PC1
#define PIN_SPI_SCK_PINSWAP_5  PIN_PC2
#define PIN_SPI_SS_PINSWAP_5   PIN_PC3
#define PIN_SPI_MOSI_PINSWAP_6 PIN_PC1
#define PIN_SPI_MISO_PINSWAP_6 PIN_PC2
#define PIN_SPI_SCK_PINSWAP_6  PIN_PC3
#define PIN_SPI_SS_PINSWAP_6   PIN_PF7 //(UPDI)


// TWI 0
#define PIN_WIRE_SDA           NOT_A_PIN
#define PIN_WIRE_SCL           NOT_A_PIN
#define PIN_WIRE_SDA_PINSWAP_1 NOT_A_PIN
#define PIN_WIRE_SCL_PINSWAP_1 NOT_A_PIN
#define PIN_WIRE_SDA_PINSWAP_2 PIN_PC2
#define PIN_WIRE_SCL_PINSWAP_2 PIN_PC3
#define PIN_WIRE_SDA_PINSWAP_3 PIN_PA0
#define PIN_WIRE_SCL_PINSWAP_3 PIN_PA1

#define NUM_HWSERIAL_PORTS              2

// USART 0
#define HWSERIAL0_MUX                   PORTMUX_USART0_DEFAULT_gc
#define HWSERIAL0_MUX_PINSWAP_3         PORTMUX_USART0_ALT3_gc
#define HWSERIAL0_MUX_PINSWAP_4         PORTMUX_USART0_ALT4_gc
#define HWSERIAL0_MUX_PINSWAP_NONE      PORTMUX_USART0_NONE_gc
#define PIN_HWSERIAL0_TX                PIN_PA0
#define PIN_HWSERIAL0_RX                PIN_PA1
#define PIN_HWSERIAL0_XCK               NOT_A_PIN
#define PIN_HWSERIAL0_XDIR              NOT_A_PIN
#define PIN_HWSERIAL0_TX_PINSWAP_3      PIN_PD4
#define PIN_HWSERIAL0_RX_PINSWAP_3      PIN_PD5
#define PIN_HWSERIAL0_XCK_PINSWAP_3     PIN_PD6
#define PIN_HWSERIAL0_XDIR_PINSWAP_3    PIN_PD7
#define PIN_HWSERIAL0_TX_PINSWAP_4      PIN_PC1
#define PIN_HWSERIAL0_RX_PINSWAP_4      PIN_PC2
#define PIN_HWSERIAL0_XCK_PINSWAP_4     PIN_PC3
#define PIN_HWSERIAL0_XDIR_PINSWAP_4    NOT_A_PIN


// USART1
#define HWSERIAL1_MUX                   PORTMUX_USART1_DEFAULT_gc
#define HWSERIAL1_MUX_PINSWAP_2         PORTMUX_USART1_ALT2_gc
#define HWSERIAL1_MUX_PINSWAP_NONE      PORTMUX_USART1_NONE_gc
#define PIN_HWSERIAL1_TX                NOT_A_PIN
#define PIN_HWSERIAL1_RX                PIN_PC1
#define PIN_HWSERIAL1_XCK               PIN_PC2
#define PIN_HWSERIAL1_XDIR              PIN_PC3
#define PIN_HWSERIAL1_TX_PINSWAP_2      PIN_PD6
#define PIN_HWSERIAL1_RX_PINSWAP_2      PIN_PD7
#define PIN_HWSERIAL1_XCK_PINSWAP_2     NOT_A_PIN
#define PIN_HWSERIAL1_XDIR_PINSWAP_2    NOT_A_PIN

        /*##  #   #  ###  #     ###   ###      ####  ### #   #  ###
        #   # ##  # #   # #    #   # #         #   #  #  ##  # #
        ##### # # # ##### #    #   # #  ##     ####   #  # # #  ###
        #   # #  ## #   # #    #   # #   #     #      #  #  ##     #
        #   # #   # #   # ####  ###   ###      #     ### #   #  #*/

#define PIN_A0   NOT_A_PIN /* Doesn't exist on DD-series */
#define PIN_A1   NOT_A_PIN
#define PIN_A2   NOT_A_PIN
#define PIN_A3   NOT_A_PIN
#define PIN_A4   PIN_PD4
#define PIN_A5   PIN_PD5
#define PIN_A6   PIN_PD6
#define PIN_A7   PIN_PD7
#define PIN_A8   NOT_A_PIN
#define PIN_A9   NOT_A_PIN
#define PIN_A10  NOT_A_PIN
#define PIN_A11  NOT_A_PIN
#define PIN_A12  NOT_A_PIN
#define PIN_A13  NOT_A_PIN
#define PIN_A14  NOT_A_PIN
#define PIN_A15  NOT_A_PIN
#define PIN_A16  NOT_A_PIN
#define PIN_A17  NOT_A_PIN
#define PIN_A18  NOT_A_PIN
#define PIN_A19  NOT_A_PIN
#define PIN_A20  NOT_A_PIN
#define PIN_A21  NOT_A_PIN
#define PIN_A22  NOT_A_PIN
#define PIN_A23  NOT_A_PIN
#define PIN_A24  NOT_A_PIN
#define PIN_A25  NOT_A_PIN
#define PIN_A26  NOT_A_PIN
#define PIN_A27  NOT_A_PIN
#define PIN_A28  NOT_A_PIN
#define PIN_A29  PIN_PC1
#define PIN_A30  PIN_PC2
#define PIN_A31  PIN_PC3

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
static const uint8_t A28 = NOT_A_PIN;
static const uint8_t A29 = PIN_A29;
static const uint8_t A30 = PIN_A30;
static const uint8_t A31 = PIN_A31;

#define AIN0  NOT_A_PIN
#define AIN1  NOT_A_PIN
#define AIN2  NOT_A_PIN
#define AIN3  NOT_A_PIN
#define AIN4  ADC_CH(4)
#define AIN5  ADC_CH(5)
#define AIN6  ADC_CH(6)
#define AIN7  ADC_CH(7)
#define AIN8  NOT_A_PIN
#define AIN9  NOT_A_PIN
#define AIN10 NOT_A_PIN
#define AIN11 NOT_A_PIN
#define AIN12 NOT_A_PIN
#define AIN13 NOT_A_PIN
#define AIN14 NOT_A_PIN
#define AIN15 NOT_A_PIN
#define AIN16 NOT_A_PIN
#define AIN17 NOT_A_PIN
#define AIN18 NOT_A_PIN
#define AIN19 NOT_A_PIN
#define AIN20 NOT_A_PIN
#define AIN21 NOT_A_PIN
#define AIN22 NOT_A_PIN
#define AIN23 NOT_A_PIN
#define AIN24 NOT_A_PIN
#define AIN25 NOT_A_PIN
#define AIN26 NOT_A_PIN
#define AIN27 NOT_A_PIN
#define AIN28 NOT_A_PIN
#define AIN29 ADC_CH(29)
#define AIN30 ADC_CH(30)
#define AIN31 ADC_CH(31)


        /*##  ### #   #      ###  ####  ####   ###  #   #  ###
        #   #  #  ##  #     #   # #   # #   # #   #  # #  #
        ####   #  # # #     ##### ####  ####  #####   #    ###
        #      #  #  ##     #   # #  #  #  #  #   #   #       #
        #     ### #   #     #   # #   # #   # #   #   #    #*/

#ifdef ARDUINO_MAIN
const uint8_t digital_pin_to_port[] = {
  PA,         //  0 PA0/USART0_Tx/CLKIN
  PA,         //  1 PA1/USART0_Rx
  NOT_A_PORT, //    NOT_A_PIN
  NOT_A_PORT, //    NOT_A_PIN
  NOT_A_PORT, //    NOT_A_PIN
  NOT_A_PORT, //    NOT_A_PIN
  NOT_A_PORT, //    NOT_A_PIN
  NOT_A_PORT, //    NOT_A_PIN
  PC,         //  8 PC0 Phantom pin
  PC,         //  9 PC1/USART1_Rx/TCA0 PWM
  PC,         // 10 PC2/TCA0 PWM
  PC,         // 11 PC3/TCA0 PWM
  PD,         // 12 PD0 Phantom pin
  NOT_A_PORT, //    NOT_A_PIN
  NOT_A_PORT, //    NOT_A_PIN
  NOT_A_PORT, //    NOT_A_PIN
  PD,         // 16 PD4/AIN4
  PD,         // 17 PD5/AIN5
  PD,         // 18 PD6/AIN6
  PD,         // 19 PD7/AIN7/AREF
  PF,         // 20 PF6 RESET
  PF          // 21 PF7 UPDI
};

/* Use this for accessing PINnCTRL register */
const uint8_t digital_pin_to_bit_position[] = { // *INDENT-OFF*
  #if CLOCK_SOURCE == 0 // PA0 used for external clock and crystal.
    PIN0_bp,//   0 PA0
  #else
    NOT_A_PIN,
  #endif
  #if CLOCK_SOURCE == 1   // PA1 also used for crystal
    NOT_A_PIN,
  #else // PA1 used for external crystal.
    PIN1_bp,//   1 PA1
  #endif    // *INDENT-ON*
  NOT_A_PIN, //    NOT_A_PIN
  NOT_A_PIN, //    NOT_A_PIN
  NOT_A_PIN, //    NOT_A_PIN
  NOT_A_PIN, //    NOT_A_PIN
  NOT_A_PIN, //    NOT_A_PIN
  NOT_A_PIN, //    NOT_A_PIN
  NOT_A_PIN, //    PC0 phantom pin
  PIN1_bp,   //  9 PC1/USART1_Rx
  PIN2_bp,   // 10 PC2
  PIN3_bp,   // 11 PC3
  NOT_A_PIN, //    PD0 Phantom pin
  NOT_A_PIN, //    NOT_A_PIN
  NOT_A_PIN, //    NOT_A_PIN
  NOT_A_PIN, //    NOT_A_PIN
  PIN4_bp,   // 16 PD4/AIN4
  PIN5_bp,   // 17 PD5/AIN5
  PIN6_bp,   // 18 PD6/AIN6
  PIN7_bp,   // 19 PD7/AIN7/AREF
  PIN6_bp,   // 20 PF6 RESET
  PIN7_bp    // 21 PD7 UPDI
};

const uint8_t digital_pin_to_bit_mask[] = { // *INDENT-OFF*
  #if CLOCK_SOURCE == 0 // PA0 used for external clock and crystal.
    PIN0_bm,//   0 PA0
  #else
    NOT_A_PIN,
  #endif
  #if CLOCK_SOURCE == 1   // PA1 also used for crystal
    NOT_A_PIN,
  #else // PA1 used for external crystal.
    PIN1_bm,//   1 PA1
  #endif    // *INDENT-ON*
  NOT_A_PIN, //    NOT_A_PIN
  NOT_A_PIN, //    NOT_A_PIN
  NOT_A_PIN, //    NOT_A_PIN
  NOT_A_PIN, //    NOT_A_PIN
  NOT_A_PIN, //    NOT_A_PIN
  NOT_A_PIN, //    NOT_A_PIN
  NOT_A_PIN, //    VDDIO2
  PIN1_bm,   //  9 PC1/USART1_Rx
  PIN2_bm,   // 10 PC2
  PIN3_bm,   // 11 PC3
  NOT_A_PIN, //    NOT_A_PIN
  NOT_A_PIN, //    NOT_A_PIN
  NOT_A_PIN, //    NOT_A_PIN
  NOT_A_PIN, //    NOT_A_PIN
  PIN4_bm,   // 16 PD4/AIN4
  PIN5_bm,   // 17 PD5/AIN5
  PIN6_bm,   // 18 PD6/AIN6
  PIN7_bm,   // 19 PD7/AIN7/AREF
  PIN6_bm,   // 20 PF6 RESET
  PIN7_bm    // 21 PF7 UPDI
};

const uint8_t digital_pin_to_timer[] = {
  NOT_ON_TIMER, //   0 PA0/USART0_Tx/CLKIN
  NOT_ON_TIMER, //   1 PA1/USART0_Rx
  NOT_ON_TIMER, //     NOT_A_PIN
  NOT_ON_TIMER, //     NOT_A_PIN
  NOT_ON_TIMER, //     NOT_A_PIN
  NOT_ON_TIMER, //     NOT_A_PIN
  NOT_ON_TIMER, //     NOT_A_PIN
  NOT_ON_TIMER, //     NOT_A_PIN
  NOT_ON_TIMER, //     VDDIO2
  NOT_ON_TIMER, //   9 PC1/USART1_Rx  TCA0 WO1 typically
  NOT_ON_TIMER, //  10 PC2            TCA0 WO2 typically
  NOT_ON_TIMER, //  11 PC3            TCA0 WO3 typically
  NOT_ON_TIMER, //     NOT_A_PIN
  NOT_ON_TIMER, //     NOT_A_PIN
  NOT_ON_TIMER, //     NOT_A_PIN
  NOT_ON_TIMER, //     NOT_A_PIN
  TIMERD0_4WOC, //  16 PD4/AIN4       TCD0 WOC
  TIMERD0_4WOD, //  17 PD5/AIN5       TCD0 WOD
  DACOUT,       //  18 PD6/AIN6       DAC here as usual
  NOT_ON_TIMER, //  19 PD7/AIN7/AREF
  NOT_ON_TIMER, //  20 PF6 RESET
  NOT_ON_TIMER  //  21 PF7 UPDI
};


#endif
  // These are used for CI testing. They should *not* *ever* be used except for CI-testing where we need to pick a viable pin to compile for
  #if CLOCK_SOURCE != 0
    #define _VALID_DIGITAL_PIN(pin)  ((pin) >= 0  ? ((pin) == 4) ? ((pin) + PIN_PD4) : NOT_A_PIN
  #else
    #define _VALID_DIGITAL_PIN(pin)  ((pin) == 0 || ((pin) == 1) ? (pin) : (pin) < 4 ? (PIN_PC0 + (pin)) : NOT_A_PIN
  #endif
  #define    _VALID_ANALOG_PIN(pin)  ((pin) >= 0 && ((pin) <= 4) ?                     ((pin) + PIN_PD4) : NOT_A_PIN
#endif
