/*  (C) Spence Konde 2021 open source (LGPL2.1 see LICENSE.md) based on existing Arduino cores.*/
//                                                                                    *INDENT-OFF*
/*
 ###  #     # ####      ####   ###      # ####  ####      #  #  ###
#   # #     # #   #     #   # #   #    #  #   # #   #     #  # #   #          #
#####  #   #  ####      #   # #####   #   #   # ####      ####  ###  ### ###     ###
#   #   # #   #  #      #   # #   #  #    #   # #   #        # #   #     #  # #  #  #
#   #    #    #   #     ####  #   # #     ####  ####         #  ###      ###  #  #  #
=====================================================     ----------     #
Variant definition file for generic DA/DB parts with 48 pins.            #

Part Numbers:
AVR128DA48 AVR64DA48 AVR32DA48
AVR128DB48 AVR64DB48 AVR32DB32

See VariantTemplate.h in extras folder an extensively annotated copy.

Include guard and include basic libraries. We are normally including this inside Arduino.h */

#ifndef Pins_Arduino_h
#define Pins_Arduino_h
#include <avr/pgmspace.h>
#include "timers.h"

#define DEFAULT_48PIN_PINOUT

 /*##  ### #   #  ###
 #   #  #  ##  # #
 ####   #  # # #  ###
 #      #  #  ##     #
 #     ### #   #  #*/

#define PIN_PA0 (0)
#define PIN_PA1 (1)
#define PIN_PA2 (2)
#define PIN_PA3 (3)
#define PIN_PA4 (4)
#define PIN_PA5 (5)
#define PIN_PA6 (6)
#define PIN_PA7 (7)
#define PIN_PB0 (8)
#define PIN_PB1 (9)
#define PIN_PB2 (10)
#define PIN_PB3 (11)
#define PIN_PB4 (12)
#define PIN_PB5 (13)
#define PIN_PC0 (14)
#define PIN_PC1 (15)
#define PIN_PC2 (16)
#define PIN_PC3 (17)
#define PIN_PC4 (18)
#define PIN_PC5 (19)
#define PIN_PC6 (20)
#define PIN_PC7 (21)
#define PIN_PD0 (22)
#define PIN_PD1 (23)
#define PIN_PD2 (24)
#define PIN_PD3 (25)
#define PIN_PD4 (26)
#define PIN_PD5 (27)
#define PIN_PD6 (28)
#define PIN_PD7 (29)
#define PIN_PE0 (30)
#define PIN_PE1 (31)
#define PIN_PE2 (32)
#define PIN_PE3 (33)
#define PIN_PF0 (34)
#define PIN_PF1 (35)
#define PIN_PF2 (36)
#define PIN_PF3 (37)
#define PIN_PF4 (38)
#define PIN_PF5 (39)
#define PIN_PF6 (40) /* RESET can be used as an input via fuse setting. It poses no reprogramming challenges, and has no output drivers. */
/*      PIN_PF7 (41) UPDI pin not available for alternative functions */

        /*##   ##   ###  ###  ###  ###
        #   # #  # #      #  #    #
        ####  ####  ###   #  #     ###
        #   # #  #     #  #  #        #
        ####  #  # ####  ###  ###  #*/

#define PINS_COUNT                     41     // Last pin number +1
#define NUM_ANALOG_INPUTS              18
// #define NUM_RESERVED_PINS            0     // These may at your option be defined,
// #define NUM_INTERNALLY_USED_PINS     0     // They will be filled in with defaults otherwise
// Autocalculated are :
// NUM_DIGITAL_PINS = PINS_COUNT - NUM_RESERVED_PINS
// TOTAL_FREE_OPINS = NUM_DIGITAL_PINS - NUM_INTERNALLY_USED_PINS
// Count of I2C and SPI pins will be defined as 2 and 3 but not used in further calculations. If you
// for some reason need to change this, define them here. Only ones not defined here get automatically set.


// Here we can override the default pin for the LED with a command line option or something opassed through boards.txt or platform.txt.
#if !defined(LED_BUILTIN)
  #define LED_BUILTIN                  PIN_PA7
#endif
/* Until the legacy attach interrupt has been completely obsoleted */
#ifdef CORE_ATTACH_OLD
  #define EXTERNAL_NUM_INTERRUPTS        47
#endif

       /*   #  ###   ### ####   ###   ###
        ## ## #   # #    #   # #   # #
        # # # ##### #    ####  #   #  ###
        #   # #   # #    # #   #   #     #
        #   # #   #  ### #  #   ###   ##*/
// If you change the number of pins in any way or if the part has ADC on different pins from the board you are adapting
// you must ensure that these will do what they say they will do.

#define digitalPinToAnalogInput(p)        (((p) > PIN_PC7 && (p) < PIN_PF0) ? ((p) - PIN_PD0) : ((p) < PIN_PF6 ? ((p) - 18) : NOT_A_PIN))
#define analogChannelToDigitalPin(p)      (((p) < 12) ? ((p) + PIN_PD0) : (((p) < 16 || p > 21) ? NOT_A_PIN : ((p) + PIN_PF0 - 16)))
#define analogInputToDigitalPin(p)                        analogChannelToDigitalPin((p) & 0x7F)
#define digitalOrAnalogPinToDigital(p)    (((p) & 0x80) ? analogChannelToDigitalPin((p) & 0x7f) : (((p)<=NUM_DIGITAL_PINS) ? (p) : NOT_A_PIN))

#define portToPinZero(port)               (((port) < PF) ? (((port) * 8) - (((port) > 1) ? 2 : 0) : ((port) == PF ? PIN_PF0 : NOT_A_PIN))

// Timer pin swaps
#define TCA0_PINS PORTMUX_TCA0_PORTC_gc     // TCA0 output on PC[0:5] by default, but just write the portmux to change it
#define TCA1_PINS 0x00                      // TCA1 output on PB[0:5]
#define TCB0_PINS PORTMUX_TCB0_bm           // TCB0 output on PF4 instead of PA2 (default)
#define TCB1_PINS PORTMUX_TCB1_bm           // TCB1 output on PF5 instead of PA3 (default)
#define TCB2_PINS 0x00                      // TCB2 output on PC0 (default) instead of PB4
#define TCB3_PINS PORTMUX_TCB3_bm           // TCB3 output on PC1 instead of PB5 (default)
#define TCD0_PINS PORTMUX_TCD0_PORTA        // Only default port option works!

#if defined(MILLIS_USE_TIMERB0)
  #define digitalPinHasPWMTCB(p) (((p) == PIN_PB4) || ((p) == PIN_PB5) || ((p) == PIN_PF5))
#elif defined(MILLIS_USE_TIMERB1)
  #define digitalPinHasPWMTCB(p) (((p) == PIN_PB4) || ((p) == PIN_PB5) || ((p) == PIN_PF4))
#elif defined(MILLIS_USE_TIMERB2)
  #define digitalPinHasPWMTCB(p) (((p) == PIN_PB5) || ((p) == PIN_PF4) || ((p) == PIN_PF5))
#elif defined(MILLIS_USE_TIMERB3)
  #define digitalPinHasPWMTCB(p) (((p) == PIN_PB4) || ((p) == PIN_PF4) || ((p) == PIN_PF5))
#else //no TCB's are used for millis
  #define digitalPinHasPWMTCB(p) (((p) == PIN_PB4) || ((p) == PIN_PB5) || ((p) == PIN_PF4) || ((p) == PIN_PF5))
#endif


#define PIN_TCA0_WO0_INIT PIN_PC0
#define PIN_TCA1_WO0_INIT PIN_PB0
#define PIN_TCB0_WO_INIT  PIN_PF4
#define PIN_TCB1_WO_INIT  PIN_PF5
#define PIN_TCB2_WO_INIT  PIN_PC0
#define PIN_TCB3_WO_INIT  PIN_PC1
#define PIN_TCD0_WOA_INIT PIN_PA4

//#define USE_TIMERD0_PWM is automatically set unless defined as 0 or 1; it will be enabled UNLESS TIMERD0_CLOCK_SETTING is and neither TIMERD0_TOP_SETTING nor F_TCD is.
#define NO_GLITCH_TIMERD0

#define digitalPinHasPWM(p)               (digitalPinHasPWMTCB(p) || ((p) >= PIN_PA4 && (p) <= PIN_PC5))

        /*##   ###  ####  ##### #   # #   # #   #
        #   # #   # #   #   #   ## ## #   #  # #
        ####  #   # ####    #   # # # #   #   #
        #     #   # #  #    #   #   # #   #  # #
        #      ###  #   #   #   #   #  ###  #   */

#define SPI_INTERFACES_COUNT   1 /* 2 SPI peripherals but SPI.h only uses one at a time, see that readme for details.*/
#define NUM_HWSERIAL_PORTS     5

// SPI 0
#define SPI_MUX                PORTMUX_SPI0_DEFAULT_gc
#define SPI_MUX_PINSWAP_1      PORTMUX_SPI0_ALT1_gc
#define PIN_SPI_MOSI           PIN_PA4
#define PIN_SPI_MISO           PIN_PA5
#define PIN_SPI_SCK            PIN_PA6
#define PIN_SPI_SS             PIN_PA7
#define PIN_SPI_MOSI_PINSWAP_1 PIN_PE0
#define PIN_SPI_MISO_PINSWAP_1 PIN_PE1
#define PIN_SPI_SCK_PINSWAP_1  PIN_PE2
#define PIN_SPI_SS_PINSWAP_1   PIN_PE3


// SPI 1
#define SPI1_MUX                (PORTMUX_SPI1_DEFAULT_gc)
#define SPI1_MUX_PINSWAP_1      (PORTMUX_SPI1_ALT1_gc)
#define PIN_SPI1_MOSI           PIN_PC0
#define PIN_SPI1_MISO           PIN_PC1
#define PIN_SPI1_SCK            PIN_PC2
#define PIN_SPI1_SS             PIN_PA3
#define PIN_SPI1_MOSI_PINSWAP_1 PIN_PC4
#define PIN_SPI1_MISO_PINSWAP_1 PIN_PC5
#define PIN_SPI1_SCK_PINSWAP_1  PIN_PC6
#define PIN_SPI1_SS_PINSWAP_1   PIN_PC7


// TWI 0
#define PIN_WIRE_SDA           PIN_PA2
#define PIN_WIRE_SCL           PIN_PA3
#define PIN_WIRE_SDA_PINSWAP_1 PIN_PA2
#define PIN_WIRE_SCL_PINSWAP_1 PIN_PA3
#define PIN_WIRE_SDA_PINSWAP_2 PIN_PC2
#define PIN_WIRE_SCL_PINSWAP_2 PIN_PC3


// TWI 1
#define PIN_WIRE1_SDA           PIN_PF2
#define PIN_WIRE1_SCL           PIN_PF3
#define PIN_WIRE1_SDA_PINSWAP_2 PIN_PB2
#define PIN_WIRE1_SCL_PINSWAP_2 PIN_PB3


// USART 0
#define HWSERIAL0_MUX                   PORTMUX_USART0_DEFAULT_gc
#define HWSERIAL0_MUX_PINSWAP_1         PORTMUX_USART0_ALT1_gc
#define HWSERIAL0_MUX_PINSWAP_NONE      PORTMUX_USART0_NONE_gc
#define PIN_HWSERIAL0_TX                PIN_PA0
#define PIN_HWSERIAL0_RX                PIN_PA1
#define PIN_HWSERIAL0_XCK               PIN_PA2
#define PIN_HWSERIAL0_XDIR              PIN_PA3
#define PIN_HWSERIAL0_TX_PINSWAP_1      PIN_PA4
#define PIN_HWSERIAL0_RX_PINSWAP_1      PIN_PA5
#define PIN_HWSERIAL0_XCK_PINSWAP_1     PIN_PA6
#define PIN_HWSERIAL0_XDIR_PINSWAP_1    PIN_PA7

// USART1
#define HWSERIAL1_MUX                   PORTMUX_USART1_DEFAULT_gc
#define HWSERIAL1_MUX_PINSWAP_1         PORTMUX_USART1_ALT1_gc
#define HWSERIAL1_MUX_PINSWAP_NONE      PORTMUX_USART1_NONE_gc
#define PIN_HWSERIAL1_TX                PIN_PC0
#define PIN_HWSERIAL1_RX                PIN_PC1
#define PIN_HWSERIAL1_XCK               PIN_PC2
#define PIN_HWSERIAL1_XDIR              PIN_PC3
#define PIN_HWSERIAL1_TX_PINSWAP_1      PIN_PC4
#define PIN_HWSERIAL1_RX_PINSWAP_1      PIN_PC5
#define PIN_HWSERIAL1_XCK_PINSWAP_1     PIN_PC6
#define PIN_HWSERIAL1_XDIR_PINSWAP_1    PIN_PC7

// USART 2
#define HWSERIAL2_MUX                   PORTMUX_USART2_DEFAULT_gc
#define HWSERIAL2_MUX_PINSWAP_1         PORTMUX_USART2_ALT1_gc
#define HWSERIAL2_MUX_PINSWAP_NONE      PORTMUX_USART2_NONE_gc
#define PIN_HWSERIAL2_TX                PIN_PF0
#define PIN_HWSERIAL2_RX                PIN_PF1
#define PIN_HWSERIAL2_XCK               PIN_PF2
#define PIN_HWSERIAL2_XDIR              PIN_PF3
#define PIN_HWSERIAL2_TX_PINSWAP_1      PIN_PF4
#define PIN_HWSERIAL2_RX_PINSWAP_1      PIN_PF5
#define PIN_HWSERIAL2_XCK_PINSWAP_1     NOT_A_PIN
#define PIN_HWSERIAL2_XDIR_PINSWAP_1    NOT_A_PIN

// USART 3
#define HWSERIAL3_MUX                   PORTMUX_USART3_DEFAULT_gc
#define HWSERIAL3_MUX_PINSWAP_1         PORTMUX_USART3_ALT1_gc
#define HWSERIAL3_MUX_PINSWAP_NONE      PORTMUX_USART3_NONE_gc
#define PIN_HWSERIAL3_TX                PIN_PB0
#define PIN_HWSERIAL3_RX                PIN_PB1
#define PIN_HWSERIAL3_XCK               PIN_PB2
#define PIN_HWSERIAL3_XDIR              PIN_PB3
#define PIN_HWSERIAL3_TX_PINSWAP_1      PIN_PB4
#define PIN_HWSERIAL3_RX_PINSWAP_1      PIN_PB5
#define PIN_HWSERIAL3_XCK_PINSWAP_1     NOT_A_PIN
#define PIN_HWSERIAL3_XDIR_PINSWAP_1    NOT_A_PIN

// USART 4
#define HWSERIAL4_MUX                   PORTMUX_USART4_DEFAULT_gc
#define HWSERIAL4_MUX_PINSWAP_NONE      PORTMUX_USART4_NONE_gc
#define PIN_HWSERIAL4_TX                PIN_PE0
#define PIN_HWSERIAL4_RX                PIN_PE1
#define PIN_HWSERIAL4_XCK               PIN_PE2
#define PIN_HWSERIAL4_XDIR              PIN_PE3

        /*##  #   #  ###  #     ###   ###      ####  ### #   #  ###
        #   # ##  # #   # #    #   # #         #   #  #  ##  # #
        ##### # # # ##### #    #   # #  ##     ####   #  # # #  ###
        #   # #  ## #   # #    #   # #   #     #      #  #  ##     #
        #   # #   # #   # ####  ###   ###      #     ### #   #  #*/

#define PIN_A0              PIN_PD0
#define PIN_A1              PIN_PD1
#define PIN_A2              PIN_PD2
#define PIN_A3              PIN_PD3
#define PIN_A4              PIN_PD4
#define PIN_A5              PIN_PD5
#define PIN_A6              PIN_PD6
#define PIN_A7              PIN_PD7
#define PIN_A8              PIN_PE0
#define PIN_A9              PIN_PE1
#define PIN_A10             PIN_PE2
#define PIN_A11             PIN_PE3
#define PIN_A16             PIN_PF0
#define PIN_A17             PIN_PF1
#define PIN_A18             PIN_PF2
#define PIN_A19             PIN_PF3
#define PIN_A20             PIN_PF4
#define PIN_A21             PIN_PF5
static const uint8_t A0  =  PIN_A0;
static const uint8_t A1  =  PIN_A1;
static const uint8_t A2  =  PIN_A2;
static const uint8_t A3  =  PIN_A3;
static const uint8_t A4  =  PIN_A4;
static const uint8_t A5  =  PIN_A5;
static const uint8_t A6  =  PIN_A6;
static const uint8_t A7  =  PIN_A7;
static const uint8_t A8  =  PIN_A8;
static const uint8_t A9  =  PIN_A9;
static const uint8_t A10 =  PIN_A10;
static const uint8_t A11 =  PIN_A11;
static const uint8_t A16 =  PIN_A16;
static const uint8_t A17 =  PIN_A17;
static const uint8_t A18 =  PIN_A18;
static const uint8_t A19 =  PIN_A19;
static const uint8_t A20 =  PIN_A20;
static const uint8_t A21 =  PIN_A21;
/* Internal and library use only - see 64-pin standard version for details.*/
#define AIN0               ADC_CH(0)
#define AIN1               ADC_CH(1)
#define AIN2               ADC_CH(2)
#define AIN3               ADC_CH(3)
#define AIN4               ADC_CH(4)
#define AIN5               ADC_CH(5)
#define AIN6               ADC_CH(6)
#define AIN7               ADC_CH(7)
#define AIN8               ADC_CH(8)
#define AIN9               ADC_CH(9)
#define AIN10              ADC_CH(10)
#define AIN11              ADC_CH(11)
#define AIN16              ADC_CH(16)
#define AIN17              ADC_CH(17)
#define AIN18              ADC_CH(18)
#define AIN19              ADC_CH(19)
#define AIN20              ADC_CH(20)
#define AIN21              ADC_CH(21)

        /*##  ### #   #      ###  ####  ####   ###  #   #  ###
        #   #  #  ##  #     #   # #   # #   # #   #  # #  #
        ####   #  # # #     ##### ####  ####  #####   #    ###
        #      #  #  ##     #   # #  #  #  #  #   #   #       #
        #     ### #   #     #   # #   # #   # #   #   #    #*/

#ifdef ARDUINO_MAIN
  /* this guards against multiple definition errors, since this file gets included by everything,
     but only in one case should these tables be generated */
const uint8_t digital_pin_to_port[] = {
  PA, //  0 PA0/XTAL0/CLKIN
  PA, //  1 PA1/XTAL1
  PA, //  2 PA2/SDA
  PA, //  3 PA3/SCL
  PA, //  4 PA4/MOSI
  PA, //  5 PA5/MISO
  PA, //  6 PA6/SCK
  PA, //  7 PA7/SS/CLKOUT/LED_BUILTIN
  PB, //  8 PB0/USART3_Tx
  PB, //  9 PB1/USART3_Rx
  PB, // 10 PB2
  PB, // 11 PB3
  PB, // 12 PB4/(TCB2 PWM)
  PB, // 13 PB5
  PC, // 14 PC0/USART1_Tx
  PC, // 15 PC1/USART1_Rx
  PC, // 16 PC2
  PC, // 17 PC3
  PC, // 18 PC4
  PC, // 19 PC5
  PC, // 20 PC6
  PC, // 21 PC7
  PD, // 22 PD0/AIN0
  PD, // 23 PD1/AIN1
  PD, // 24 PD2/AIN2
  PD, // 25 PD3/AIN3
  PD, // 26 PD4/AIN4
  PD, // 27 PD5/AIN5
  PD, // 28 PD6/AIN6
  PD, // 29 PD7/AIN7/AREF
  PE, // 30 PE0/AIN8
  PE, // 31 PE1/AIN9
  PE, // 32 PE2/AIN10
  PE, // 33 PE3/AIN11
  PF, // 34 PF0/AIN16/USART2_Tx/TOSC1
  PF, // 35 PF1/AIN17/USART2_Rx/TOSC2
  PF, // 36 PF2/AIN18
  PF, // 37 PF3/AIN19
  PF, // 38 PF4/AIN20
  PF, // 39 PF5/AIN21
  PF, // 40 PF6 RESET
  //PF, // 55 PF7 (UPDI)
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
  PIN2_bp, //  2 PA2/SDA
  PIN3_bp, //  3 PA3/SCL
  PIN4_bp, //  4 PA4/MOSI
  PIN5_bp, //  5 PA5/MISO
  PIN6_bp, //  6 PA6/SCK
  PIN7_bp, //  7 PA7/SS/CLKOUT/LED_BUILTIN
  PIN0_bp, //  8 PB0/USART3_Tx
  PIN1_bp, //  9 PB1/USART3_Rx
  PIN2_bp, // 10 PB2
  PIN3_bp, // 11 PB3
  PIN4_bp, // 12 PB4/(TCB2 PWM)
  PIN5_bp, // 13 PB5
  PIN0_bp, // 14 PC0/USART1_Tx
  PIN1_bp, // 15 PC1/USART1_Rx
  PIN2_bp, // 16 PC2
  PIN3_bp, // 17 PC3
  PIN4_bp, // 18 PC4
  PIN5_bp, // 19 PC5
  PIN6_bp, // 20 PC6
  PIN7_bp, // 21 PC7
  PIN0_bp, // 22 PD0/AIN0
  PIN1_bp, // 23 PD1/AIN1
  PIN2_bp, // 24 PD2/AIN2
  PIN3_bp, // 25 PD3/AIN3
  PIN4_bp, // 26 PD4/AIN4
  PIN5_bp, // 27 PD5/AIN5
  PIN6_bp, // 28 PD6/AIN6
  PIN7_bp, // 29 PD7/AIN7/AREF
  PIN0_bp, // 30 PE0/AIN8
  PIN1_bp, // 31 PE1/AIN9
  PIN2_bp, // 32 PE2/AIN10
  PIN3_bp, // 33 PE3/AIN11
  PIN0_bp, // 34 PF0/USART2_Tx/TOSC1
  PIN1_bp, // 35 PF1/USART2_Rx/TOSC2
  PIN2_bp, // 36 PF2/AIN12
  PIN3_bp, // 37 PF3/AIN13
  PIN4_bp, // 38 PF4/AIN14
  PIN5_bp, // 39 PF5/AIN15
  PIN6_bp, // 40 PF6 RESET
  //PIN7_bm// 55 PF7 (UPDI)
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
  PIN2_bm, //  2 PA2/SDA
  PIN3_bm, //  3 PA3/SCL
  PIN4_bm, //  4 PA4/MOSI
  PIN5_bm, //  5 PA5/MISO
  PIN6_bm, //  6 PA6/SCK
  PIN7_bm, //  7 PA7/SS/CLKOUT/LED_BUILTIN
  PIN0_bm, //  8 PB0/USART3_Tx
  PIN1_bm, //  9 PB1/USART3_Rx
  PIN2_bm, // 10 PB2
  PIN3_bm, // 11 PB3
  PIN4_bm, // 12 PB4/(TCB2 PWM)
  PIN5_bm, // 13 PB5
  PIN0_bm, // 14 PC0/USART1_Tx
  PIN1_bm, // 15 PC1/USART1_Rx
  PIN2_bm, // 16 PC2
  PIN3_bm, // 17 PC3
  PIN4_bm, // 18 PC4
  PIN5_bm, // 19 PC5
  PIN6_bm, // 20 PC6
  PIN7_bm, // 21 PC7
  PIN0_bm, // 22 PD0/AIN0
  PIN1_bm, // 23 PD1/AIN1
  PIN2_bm, // 24 PD2/AIN2
  PIN3_bm, // 25 PD3/AIN3
  PIN4_bm, // 26 PD4/AIN4
  PIN5_bm, // 27 PD5/AIN5
  PIN6_bm, // 28 PD6/AIN6
  PIN7_bm, // 29 PD7/AIN7/AREF
  PIN0_bm, // 30 PE0/AIN8
  PIN1_bm, // 31 PE1/AIN9
  PIN2_bm, // 32 PE2/AIN10
  PIN3_bm, // 33 PE3/AIN11
  PIN0_bm, // 34 PF0/USART2_Tx/TOSC1
  PIN1_bm, // 35 PF1/USART2_Rx/TOSC2
  PIN2_bm, // 36 PF2/AIN12
  PIN3_bm, // 37 PF3/AIN13
  PIN4_bm, // 38 PF4/AIN14
  PIN5_bm, // 39 PF5/AIN15
  PIN6_bm, // 40 PF6 RESET
  //PIN7_bm// 55 PF7 (UPDI)
};
const uint8_t digital_pin_to_timer[] = {
  NOT_ON_TIMER, //  0 PA0/USART0_Tx/CLKIN
  NOT_ON_TIMER, //  1 PA1/USART0_Rx
  NOT_ON_TIMER, //  2 PA2/SDA
  NOT_ON_TIMER, //  3 PA3/SCL
  TIMERD0,      //  4 PA4/MOSI      WOA
  TIMERD0,      //  5 PA5/MISO      WOB
  TIMERD0,      //  6 PA6/SCK       WOC mirrors WOA
  TIMERD0,      //  7 PA7/SS/CLKOUT WOD mirrors WOB
  NOT_ON_TIMER, //  8 PB0
  NOT_ON_TIMER, //  9 PB1
  NOT_ON_TIMER, // 10 PB2
  NOT_ON_TIMER, // 11 PB3
  NOT_ON_TIMER, // 12 PB4
  NOT_ON_TIMER, // 13 PB5
  TIMERB2,      // 16 PC0
  TIMERB3,      // 17 PC1
  NOT_ON_TIMER, // 18 PC2
  NOT_ON_TIMER, // 19 PC3
  NOT_ON_TIMER, // 20 PC4
  NOT_ON_TIMER, // 21 PC5
  NOT_ON_TIMER, // 20 PC6
  NOT_ON_TIMER, // 21 PC7
  NOT_ON_TIMER, // 22 PD0/AIN0
  NOT_ON_TIMER, // 23 PD1/AIN1
  NOT_ON_TIMER, // 24 PD2/AIN2
  NOT_ON_TIMER, // 25 PD3/AIN3
  NOT_ON_TIMER, // 26 PD4/AIN4
  NOT_ON_TIMER, // 27 PD5/AIN5
  DACOUT,       // 28 PD6/AIN6
  NOT_ON_TIMER, // 29 PD7/AIN7/AREF
  NOT_ON_TIMER, // 30 PE0/AIN8
  NOT_ON_TIMER, // 31 PE1/AIN9
  NOT_ON_TIMER, // 32 PE2/AIN10
  NOT_ON_TIMER, // 33 PE3/AIN11
  NOT_ON_TIMER, // 34 PF0/USART2_Tx/TOSC1
  NOT_ON_TIMER, // 35 PF1/USART2_Rx/TOSC2
  NOT_ON_TIMER, // 36 PF2/AIN12
  NOT_ON_TIMER, // 37 PF3/AIN13
  TIMERB0,      // 38 PF4/AIN14
  TIMERB1,      // 39 PF5/AIN15
  NOT_ON_TIMER, // 40 PF6 RESET
  //NOT_ON_TIMER// 55 PF7 (UPDI)
};

#endif

// These are used for CI testing. They should *not* *ever* be used except for CI-testing where we need to pick a viable pin to compile a sketch with that won't generate compile errors (we don't care whether it would;d actually work, we are concerned with )
  #if CLOCK_SOURCE != 0
    #define _VALID_DIGITAL_PIN(pin)  ((pin) >= 0 &&  (pin) <  4  ? ((pin) + 2)                  )
  #else
    #define _VALID_DIGITAL_PIN(pin)  ((pin) >= 0 &&  (pin) <  4  ? ((pin) + 0)       : NOT_A_PIN)
  #endif
#define      _VALID_ANALOG_PIN(pin)  ((pin) >= 0 && ((pin) <= 4) ? ((pin) + PIN_PD1) : NOT_A_PIN)

#endif
