/*  (C) Spence Konde 2021 open source (LGPL2.1 see LICENSE.md) based on exisisting Arduino cores.*/
//                                                                                    *INDENT-OFF*
/*
 ###  #     # ####      ####  ####        #   #  #
#   # #     # #   #     #   # #   #      ##   #  #          #
#####  #   #  ####      #   # #   #       #   #### ### ###     ###
#   #   # #   #  #      #   # #   #       #      #     #  # #  #  #
#   #    #    #   #     ####  ####       ###     #     ###  #  #  #
===================================     ----------     #
Variant Definition file for generic DD parts           #
with 32 pins.

Part Numbers:
AVR64DA14 AVR32DA14 AVR16DD14

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
 * PD0 and PD1, PD2, and PD3 are empty holes, but the there is no hole for PC0 or PF0.
 */

#define PIN_PA0 (0)  // Not available if using HF crystal.
#define PIN_PA1 (1)  // Not available if using HF crystal.
// We aren't going to skip 6 pin numbers for PA2-PA6
// No port B on any DD-series
// No PC0 (2) on 14/20-pin
#define PIN_PC1 (3)
#define PIN_PC2 (4)
#define PIN_PC3 (5)  //A31
#define PIN_PD0 (6)  // phantom pin - because of it's position as the reference point for the start of the analog pins, we keep this one in the numbering.
// No PD1 (7)
// No PD2 (8)
// No PD3 (9)
#define PIN_PD4 (10)
#define PIN_PD5 (11)
#define PIN_PD6 (12)
#define PIN_PD7 (13)
// No PF0-PF6 with < 28 pins - again because 6 pins being skipped, we are not putting a hole in the numbers here.
#define PIN_PF6 (14) // RESET
#define PIN_PF7 (15) // UPDI

#define FAKE_PIN_PD0

        /*##   ##   ###  ###  ###  ###
        #   # #  # #      #  #    #
        ####  ####  ###   #  #     ###
        #   # #  #     #  #  #        #
        ####  #  # ####  ###  ###  #*/

#define PINS_COUNT                     11
#define NUM_ANALOG_INPUTS
// #define NUM_RESERVED_PINS            0     // These may at your option be defined,
// #define NUM_INTERNALLY_USED_PINS     0     // They will be filled in with defaults otherwise
// Autocalculated are :
// NUM_DIGITAL_PINS and NUM_TOTAL_PINS = highest number of any valid pin. NOT the number of pins!
// TOTAL_FREE_OPINS = PINS_COUNT - NUM_INTERNALLY_USED_PINS
// Count of I2C and SPI pins will be defined as 2 and 3 but not used in further calculations. If you
// for some reason need to change this, define them here. Only ones not defined here get automatically set.

#if !defined(LED_BUILTIN)
  #define LED_BUILTIN                  PIN_D7
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

#define digitalPinToAnalogInput(p)        ((p) >= PIN_PD0 ? ((p) < PIN_PF6) ? (p) - PIN_PD0 : NOT_A_PIN) : ((p) >= PIN_PC1 ? (p) + 27 : NOT_A_PIN)  // (analog pins are PD4~7 and PC1~3)
#define analogChannelToDigitalPin(p)      ((p) > 31 ? NOT_A_PIN : ((p) < 8 ? ((p) + PIN_PD0) : (p) > 28 ? (p) + 27 :  NOT_A_PIN))
#define analogInputToDigitalPin(p)                        analogChannelToDigitalPin((p) & 0x7F)
#define digitalOrAnalogPinToDigital(p)    (((p) & 0x80) ? analogChannelToDigitalPin((p) & 0x7F) : (((p) <= PIN_PF7 && (p >= PIN_PD4 || p < PIN_PD0)) ? (p) : NOT_A_PIN))
#define portToPinZero(port)               ((port) == PA ? PIN_PA0 : ((port)== PD ? PIN_PD0 : NOT_A_PIN))


// PWM pins


#define digitalPinHasPWMTCB(p) (false)      // Not a single TCB timer output pin available!


// Timer pin mapping
#define TCA0_PINS (PORTMUX_TCA0_PORTC_gc)     // EVERY option here sucks; this one gives us... 3 PWM channels...
#define TCB0_PINS (0x00)                      // TCB0 output on PA2 (Doesn't exist here) or PF4 (Doesn't exist here)? - decisions decisions!
#define TCB1_PINS (0x00)                      // TCB1 output on PA3 (Doesn't exist here) or PF5 (Doesn't exist here)? - decisions decisions!
#define TCD0_PINS (PORTMUX_TCD0_PORTAD)       // TCD0 output on PD4 and PD5, only option for which any pins exist on this part.

#define PIN_TCA0_WO0_INIT                 (PC)
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
  NOT_A_PORT, //
  PC,         //  3 PC1/USART1_Rx/TCA0 PWM
  PC,         //  4 PC2/TCA0 PWM
  PC,         //  5 PC3/TCA0 PWM
  PD,         //  6 PD0 Phantom pin
  NOT_A_PORT, //
  NOT_A_PORT, //
  NOT_A_PORT, //
  PD,         // 10 PD4/AIN4
  PD,         // 11 PD5/AIN5
  PD,         // 12 PD6/AIN6
  PD,         // 13 PD7/AIN7/AREF
  PF,         // 14 PF6 RESET
  PF          // 15 PF7 UPDI
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
  NOT_A_PIN,  //
  PIN1_bp,   //  3 PC1/USART1_Rx
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
  #if CLOCK_SOURCE == 0 // PA0 used for external clock and crystal.
    PIN0_bm,//   0 PA0
  #else
    NOT_A_PIN,
  #endif
  #if CLOCK_SOURCE == 1   // PA1 also used for crystal
    NOT_A_PIN,
  #else // PA1 used for external crystal.
    PIN1_bm,//   1 PA1
  #endif    // *INDENT-ON*=
  NOT_A_PIN, //
  PIN1_bm,   //  3 PC1/USART1_Rx
  PIN2_bm,   //  4 PC2
  PIN3_bm,   //  5 PC3
  NOT_A_PIN, //
  NOT_A_PIN, //
  NOT_A_PIN, //
  NOT_A_PIN, //
  PIN4_bm,   // 10 PD4/AIN4
  PIN5_bm,   // 11 PD5/AIN5
  PIN6_bm,   // 12 PD6/AIN6
  PIN7_bm,   // 13 PD7/AIN7/AREF
  PIN6_bm,   // 14 PF6 RESET
  PIN7_bm    // 15 PF7 UPDI
};

const uint8_t digital_pin_to_timer[] = {
  NOT_ON_TIMER, //   0 PA0/USART0_Tx/CLKIN
  NOT_ON_TIMER, //   1 PA1/USART0_Rx
  NOT_ON_TIMER, // NOT_A_PIN
  NOT_ON_TIMER, //   3 PC1/USART1_Rx  TCA0 WO1 typically
  NOT_ON_TIMER, //   4 PC2            TCA0 WO2 typically
  NOT_ON_TIMER, //   5 PC3            TCA0 WO3 typically
  NOT_ON_TIMER, // NOT_A_PIN
  NOT_ON_TIMER, // NOT_A_PIN
  NOT_ON_TIMER, // NOT_A_PIN
  NOT_ON_TIMER, // NOT_A_PIN
  TIMERD0_4WOC, //  10 PD4/AIN4       TCD0 WOC
  TIMERD0_4WOD, //  11 PD5/AIN5       TCD0 WOD
  DACOUT,       //  12 PD6/AIN6       DAC here as usual
  NOT_ON_TIMER, //  13 PD7/AIN7/AREF
  NOT_ON_TIMER, //  14 PF6 RESET
  NOT_ON_TIMER  //  15 PF7 UPDI
};


#endif
