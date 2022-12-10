/*  (C) Spence Konde 2021 open source (LGPL2.1 see LICENSE.md) based on existing Arduino cores.*/
//                                                                                    *INDENT-OFF*
/*
 ###  #     # ####      ####   ###      # ####  ####        #   #  #
#   # #     # #   #     #   # #   #    #  #   # #   #      #    #  #          #
#####  #   #  ####      #   # #####   #   #   # ####      ####  #### ### ###     ###
#   #   # #   #  #      #   # #   #  #    #   # #   #     #   #    #     #  # #  #  #
#   #    #    #   #     ####  #   # #     ####  ####       ###     #     ###  #  #  #
=====================================================     ----------     #
Variant Definition file for generic DA/DB parts with 64 pins             #

Part Numbers:
AVR128DA64, AVR128DB64, AVR64DA64 and AVR64DB64

See VariantTemplate.h in extras folder an extensively annotated copy.

Include guard and include basic libraries. We are normally including this inside Arduino.h */

#ifndef Pins_Arduino_h
#define Pins_Arduino_h
#include <avr/pgmspace.h>
#include "timers.h"

#define DEFAULT_64PIN_PINOUT

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
#define PIN_PB6 (14)
#define PIN_PB7 (15)
#define PIN_PC0 (16)
#define PIN_PC1 (17)
#define PIN_PC2 (18)
#define PIN_PC3 (19)
#define PIN_PC4 (20)
#define PIN_PC5 (21)
#define PIN_PC6 (22)
#define PIN_PC7 (23)
#define PIN_PD0 (24)
#define PIN_PD1 (25)
#define PIN_PD2 (26)
#define PIN_PD3 (27)
#define PIN_PD4 (28)
#define PIN_PD5 (29)
#define PIN_PD6 (30)
#define PIN_PD7 (31)
#define PIN_PE0 (32)
#define PIN_PE1 (33)
#define PIN_PE2 (34)
#define PIN_PE3 (35)
#define PIN_PE4 (36)
#define PIN_PE5 (37)
#define PIN_PE6 (38)
#define PIN_PE7 (39)
#define PIN_PF0 (40)
#define PIN_PF1 (41)
#define PIN_PF2 (42)
#define PIN_PF3 (43)
#define PIN_PF4 (44)
#define PIN_PF5 (45)
#define PIN_PG0 (46)
#define PIN_PG1 (47)
#define PIN_PG2 (48)
#define PIN_PG3 (49)
#define PIN_PG4 (50)
#define PIN_PG5 (51)
#define PIN_PG6 (52)
#define PIN_PG7 (53)
#define PIN_PF6 (54) /* RESET can be used as an input via fuse setting. It poses no reprogramming challenges, and has no output drivers. */
/*      PIN_PF7 (55) UPDI pin not available for alternative functions */

        /*##   ##   ###  ###  ###  ###
        #   # #  # #      #  #    #
        ####  ####  ###   #  #     ###
        #   # #  #     #  #  #        #
        ####  #  # ####  ###  ###  #*/

#define NUM_DIGITAL_PINS                  (55)
#define NUM_ANALOG_INPUTS                 (22)
// #define NUM_RESERVED_PINS            0     // These may at your option be defined,
// #define NUM_INTERNALLY_USED_PINS     0     // They will be filled in with defaults otherwise
// Autocalculated are :
// NUM_DIGITAL_PINS = PINS_COUNT - NUM_RESERVED_PINS
// TOTAL_FREE_OPINS = NUM_DIGITAL_PINS - NUM_INTERNALLY_USED_PINS
// Count of I2C and SPI pins will be defined as 2 and 3 but not used in further calculations. If you
// for some reason need to change this, define them here. Only ones not defined here get automatically set.


// Here we can override the default pin for the LED with a command line option or something opassed through boards.txt or platform.txt.

#if !defined(LED_BUILTIN)
  #define LED_BUILTIN                     PIN_PA7
#endif

/* Until the legacy attach interrupt has been completely obsoleted */
#ifdef CORE_ATTACH_OLD
  #define EXTERNAL_NUM_INTERRUPTS           (56)
#endif


       /*   #  ###   ### ####   ###   ###
        ## ## #   # #    #   # #   # #
        # # # ##### #    ####  #   #  ###
        #   # #   # #    # #   #   #     #
        #   # #   #  ### #  #   ###   ##*/
// If you change the number of pins in any way or if the part has ADC on different pins from the board you are adapting
// you must ensure that these will do what they say they will do.

#define digitalPinToAnalogInput(p)        ((((p) >= PIN_PC7) && ((p) < PIN_PF6)) ? ((p) - PIN_PD0) : NOT_A_PIN)
#define analogChannelToDigitalPin(p)      ((p) < 22 ? ((p) + PIN_PD0) : NOT_A_PIN)
#define analogInputToDigitalPin(p)                        analogChannelToDigitalPin((p) & 0x7F)
#define digitalOrAnalogPinToDigital(p)    (((p) & 0x80) ? analogChannelToDigitalPin((p) & 0x7f) : (((p)<=NUM_DIGITAL_PINS) ? (p) : NOT_A_PIN)) /* handle analog channels too */
#define portToPinZero(port)               (((port) < PG) ? (port) * 8 : (port == PG ? 46 : NOT_A_PIN));

#if defined(MILLIS_USE_TIMERB0)
  #define digitalPinHasPWMTCB(p) (((p) == PIN_PB4) || ((p) == PIN_PB5) || ((p) == PIN_PF5) || ((p) == PIN_PG3))
#elif defined(MILLIS_USE_TIMERB1)
  #define digitalPinHasPWMTCB(p) (((p) == PIN_PB4) || ((p) == PIN_PB5) || ((p) == PIN_PF4) || ((p) == PIN_PG3))
#elif defined(MILLIS_USE_TIMERB2)
  #define digitalPinHasPWMTCB(p) (((p) == PIN_PB5) || ((p) == PIN_PF4) || ((p) == PIN_PF5) || ((p) == PIN_PG3))
#elif defined(MILLIS_USE_TIMERB3)
  #define digitalPinHasPWMTCB(p) (((p) == PIN_PB4) || ((p) == PIN_PF4) || ((p) == PIN_PF5) || ((p) == PIN_PG3))
#elif defined(MILLIS_USE_TIMERB4)
  #define digitalPinHasPWMTCB(p) (((p) == PIN_PB4) || ((p) == PIN_PF4) || ((p) == PIN_PF5) || ((p) == PIN_PB5))
#else //no TCB's are used for millis
  #define digitalPinHasPWMTCB(p) (((p) == PIN_PB4) || ((p) == PIN_PB5) || ((p) == PIN_PF4) || ((p) == PIN_PF5) || ((p) == PIN_PG3))
#endif

// Timer pin swaps
#define TCA0_PINS PORTMUX_TCA0_PORTC_gc     // TCA0 output on PC[0:5] after init() (alt 2)
#define TCA1_PINS 0x00                      // TCA1 output on PB[0:5] after init() (no pinswap)
#define TCB0_PINS PORTMUX_TCB0_bm           // TCB0 output on PF4 instead of PA2 (default)
#define TCB1_PINS PORTMUX_TCB1_bm           // TCB1 output on PF5 instead of PA3 (default)
#define TCB2_PINS 0x00                      // TCB2 output on PC0 (default) instead of PB4.
#define TCB3_PINS PORTMUX_TCB3_bm           // TCB3 output on PC1 instead of PB5 (default)
#define TCB4_PINS 0x00                      // TCB4 output on PG3 (default) instead of PC6
#define TCD0_PINS PORTMUX_TCD0_PORTA        // TCD0 output on PA[4:7] (default - only default port option works on DA, DB parts.
// Note that we need to use the numerical value, not the _gc constant here. Group codes (_gc) are defined as enumerated types,
// which are not understood by the preprocessor and can't be tested for by preprocessor conditionals.
// Setting TCB2 and TCB3 for PWM on PC0, PC1 doesn't sound optimal for the AVR DB, but you shouldn't be using TCBs for PWM anyway!
// They are there as a last resort, not a first resort.
#define PIN_TCA0_WO0_INIT PIN_PC0
#define PIN_TCA1_WO0_INIT PIN_PB0
#define PIN_TCB0_WO_INIT  PIN_PF4
#define PIN_TCB1_WO_INIT  PIN_PF5
#define PIN_TCB2_WO_INIT  PIN_PC0
#define PIN_TCB3_WO_INIT  PIN_PC1
#define PIN_TCB4_WO_INIT  PIN_PG3
#define PIN_TCD0_WOA_INIT PIN_PA4

//#define USE_TIMERD0_PWM is automatically set unless defined as 0 or 1; it will be enabled UNLESS TIMERD0_CLOCK_SETTING is and neither TIMERD0_TOP_SETTING nor F_TCD is.
#define NO_GLITCH_TIMERD0

#define digitalPinHasPWM(p)               (digitalPinHasPWMTCB(p) || ((p) >= PIN_PA4 && (p) <= PIN_PC5 && (p) != PIN_PB6 && (p) != PIN_PB7))


        /*##   ###  ####  ##### #   # #   # #   #
        #   # #   # #   #   #   ## ## #   #  # #
        ####  #   # ####    #   # # # #   #   #
        #     #   # #  #    #   #   # #   #  # #
        #      ###  #   #   #   #   #  ###  #   */

#define SPI_INTERFACES_COUNT   1 /* 2 SPI peripherals but SPI.h only uses one at a time, see that readme for details.*/
#define NUM_HWSERIAL_PORTS     6

// SPI 0
#define SPI_MUX                (PORTMUX_SPI0_DEFAULT_gc)
#define SPI_MUX_PINSWAP_1      (PORTMUX_SPI0_ALT1_gc)
#define SPI_MUX_PINSWAP_2      (PORTMUX_SPI0_ALT2_gc)
#define SPI_MUX_PINSWAP_NONE   (PORTMUX_SPI0_NONE_gc)
#define PIN_SPI_MOSI           PIN_PA4
#define PIN_SPI_MISO           PIN_PA5
#define PIN_SPI_SCK            PIN_PA6
#define PIN_SPI_SS             PIN_PA7

#define PIN_SPI_MOSI_PINSWAP_1 PIN_PE0
#define PIN_SPI_MISO_PINSWAP_1 PIN_PE1
#define PIN_SPI_SCK_PINSWAP_1  PIN_PE2
#define PIN_SPI_SS_PINSWAP_1   PIN_PE3

#define PIN_SPI_MOSI_PINSWAP_2 PIN_PG4
#define PIN_SPI_MISO_PINSWAP_2 PIN_PG5
#define PIN_SPI_SCK_PINSWAP_2  PIN_PG6
#define PIN_SPI_SS_PINSWAP_2   PIN_PG7

// SPI 1
#define SPI1_MUX                (PORTMUX_SPI1_DEFAULT_gc)
#define SPI1_MUX_PINSWAP_1      (PORTMUX_SPI1_ALT1_gc)
#define SPI1_MUX_PINSWAP_2      (PORTMUX_SPI1_ALT2_gc)
#define SPI1_MUX_PINSWAP_NONE   (PORTMUX_SPI1_NONE_gc)
#define PIN_SPI1_MOSI           PIN_PC0
#define PIN_SPI1_MISO           PIN_PC1
#define PIN_SPI1_SCK            PIN_PC2
#define PIN_SPI1_SS             PIN_PC3

#define PIN_SPI1_MOSI_PINSWAP_1 PIN_PC4
#define PIN_SPI1_MISO_PINSWAP_1 PIN_PC5
#define PIN_SPI1_SCK_PINSWAP_1  PIN_PC6
#define PIN_SPI1_SS_PINSWAP_1   PIN_PC7

#define PIN_SPI1_MOSI_PINSWAP_2 PIN_PB4
#define PIN_SPI1_MISO_PINSWAP_2 PIN_PB5
#define PIN_SPI1_SCK_PINSWAP_2  PIN_PB6
#define PIN_SPI1_SS_PINSWAP_2   PIN_PB7

// TWI 0
#define PIN_WIRE_SDA           PIN_PA2
#define PIN_WIRE_SCL           PIN_PA3
#define PIN_WIRE_SDA_PINSWAP_1 PIN_PC2
#define PIN_WIRE_SCL_PINSWAP_1 PIN_PC3
#define PIN_WIRE_SDA_PINSWAP_2 PIN_PC2
#define PIN_WIRE_SCL_PINSWAP_2 PIN_PC3


// TWI 1
#define PIN_WIRE1_SDA           PIN_PF2
#define PIN_WIRE1_SCL           PIN_PF3
#define PIN_WIRE1_SDA_PINSWAP_2 PIN_PB2
#define PIN_WIRE1_SCL_PINSWAP_2 PIN_PB3

// USART 0
#define HWSERIAL0_MUX                   PORTMUX_USART0_DEFAULT_gc  /* Nope, redundant */
#define HWSERIAL0_MUX_PINSWAP_1         PORTMUX_USART0_ALT1_gc
#define HWSERIAL0_MUX_PINSWAP_NONE      PORTMUX_USART0_NONE_gc
#define PIN_HWSERIAL0_TX                PIN_PA0               /* All the pins stay */
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
#define PIN_HWSERIAL3_XCK_PINSWAP_1     PIN_PB6
#define PIN_HWSERIAL3_XDIR_PINSWAP_1    PIN_PB7

// USART 4
#define HWSERIAL4_MUX                   PORTMUX_USART4_DEFAULT_gc
#define HWSERIAL4_MUX_PINSWAP_1         PORTMUX_USART4_ALT1_gc
#define HWSERIAL4_MUX_PINSWAP_NONE      PORTMUX_USART4_NONE_gc
#define PIN_HWSERIAL4_TX                PIN_PE0
#define PIN_HWSERIAL4_RX                PIN_PE1
#define PIN_HWSERIAL4_XCK               PIN_PE2
#define PIN_HWSERIAL4_XDIR              PIN_PE3
#define PIN_HWSERIAL4_TX_PINSWAP_1      PIN_PE4
#define PIN_HWSERIAL4_RX_PINSWAP_1      PIN_PE5
#define PIN_HWSERIAL4_XCK_PINSWAP_1     PIN_PE6
#define PIN_HWSERIAL4_XDIR_PINSWAP_1    PIN_PE7

// USART 5
#define HWSERIAL5_MUX                   PORTMUX_USART5_DEFAULT_gc
#define HWSERIAL5_MUX_PINSWAP_1         PORTMUX_USART5_ALT1_gc
#define HWSERIAL5_MUX_PINSWAP_NONE      PORTMUX_USART5_NONE_gc
#define PIN_HWSERIAL5_TX                PIN_PG0
#define PIN_HWSERIAL5_RX                PIN_PG1
#define PIN_HWSERIAL5_XCK               PIN_PG2
#define PIN_HWSERIAL5_XDIR              PIN_PG3
#define PIN_HWSERIAL5_TX_PINSWAP_1      PIN_PG4
#define PIN_HWSERIAL5_RX_PINSWAP_1      PIN_PG5
#define PIN_HWSERIAL5_XCK_PINSWAP_1     PIN_PG6
#define PIN_HWSERIAL5_XDIR_PINSWAP_1    PIN_PG7

        /*##  #   #  ###  #     ###   ###      ####  ### #   #  ###
        #   # ##  # #   # #    #   # #         #   #  #  ##  # #
        ##### # # # ##### #    #   # #  ##     ####   #  # # #  ###
        #   # #  ## #   # #    #   # #   #     #      #  #  ##     #
        #   # #   # #   # ####  ###   ###      #     ### #   #  #*/

#define PIN_A0   PIN_PD0
#define PIN_A1   PIN_PD1
#define PIN_A2   PIN_PD2
#define PIN_A3   PIN_PD3
#define PIN_A4   PIN_PD4
#define PIN_A5   PIN_PD5
#define PIN_A6   PIN_PD6
#define PIN_A7   PIN_PD7
#define PIN_A8   PIN_PE0
#define PIN_A9   PIN_PE1
#define PIN_A10  PIN_PE2
#define PIN_A11  PIN_PE3
#define PIN_A12  PIN_PE4
#define PIN_A13  PIN_PE5
#define PIN_A14  PIN_PE6
#define PIN_A15  PIN_PE7
#define PIN_A16  PIN_PF0
#define PIN_A17  PIN_PF1
#define PIN_A18  PIN_PF2
#define PIN_A19  PIN_PF3
#define PIN_A20  PIN_PF4
#define PIN_A21  PIN_PF5
static const uint8_t A0  = PIN_A0;
static const uint8_t A1  = PIN_A1;
static const uint8_t A2  = PIN_A2;
static const uint8_t A3  = PIN_A3;
static const uint8_t A4  = PIN_A4;
static const uint8_t A5  = PIN_A5;
static const uint8_t A6  = PIN_A6;
static const uint8_t A7  = PIN_A7;
static const uint8_t A8  = PIN_A8;
static const uint8_t A9  = PIN_A9;
static const uint8_t A10 = PIN_A10;
static const uint8_t A11 = PIN_A11;
static const uint8_t A12 = PIN_A12;
static const uint8_t A13 = PIN_A13;
static const uint8_t A14 = PIN_A14;
static const uint8_t A15 = PIN_A15;
static const uint8_t A16 = PIN_A16;
static const uint8_t A17 = PIN_A17;
static const uint8_t A18 = PIN_A18;
static const uint8_t A19 = PIN_A19;
static const uint8_t A20 = PIN_A20;
static const uint8_t A21 = PIN_A21;
#define  AIN0  ADC_CH(0)
#define  AIN1  ADC_CH(1)
#define  AIN2  ADC_CH(2)
#define  AIN3  ADC_CH(3)
#define  AIN4  ADC_CH(4)
#define  AIN5  ADC_CH(5)
#define  AIN6  ADC_CH(6)
#define  AIN7  ADC_CH(7)
#define  AIN8  ADC_CH(8)
#define  AIN9  ADC_CH(9)
#define AIN10 ADC_CH(10)
#define AIN11 ADC_CH(11)
#define AIN12 ADC_CH(12)
#define AIN13 ADC_CH(13)
#define AIN14 ADC_CH(14)
#define AIN15 ADC_CH(15)
#define AIN16 ADC_CH(16)
#define AIN17 ADC_CH(17)
#define AIN18 ADC_CH(18)
#define AIN19 ADC_CH(19)
#define AIN20 ADC_CH(20)
#define AIN21 ADC_CH(21)


        /*##  ### #   #      ###  ####  ####   ###  #   #  ###
        #   #  #  ##  #     #   # #   # #   # #   #  # #  #
        ####   #  # # #     ##### ####  ####  #####   #    ###
        #      #  #  ##     #   # #  #  #  #  #   #   #       #
        #     ### #   #     #   # #   # #   # #   #   #    #*/

#ifdef ARDUINO_MAIN
/* this guards against multiple definition errors, since this file gets included by everything,
 * but only in one case should these tables be generated */
const uint8_t digital_pin_to_port[] = {
  PA,  //   0 PA0/XTAL0/CLKIN   (Note that it's not a pin when using external clock, and bitmask and position will come back as NOT_A_PIN)
  PA,  //   1 PA1/XTAL1 (If using a crystal, same as above.)
  PA,  //   2 PA2
  PA,  //   3 PA3
  PA,  //   4 PA4
  PA,  //   5 PA5
  PA,  //   6 PA6
  PA,  //   7 PA7
  PB,  //   8 PB0
  PB,  //   9 PB1
  PB,  //  10 PB2
  PB,  //  11 PB3
  PB,  //  12 PB4
  PB,  //  13 PB5
  PB,  //  14 PB6
  PB,  //  15 PB7
  PC,  //  16 PC0
  PC,  //  17 PC1
  PC,  //  18 PC2
  PC,  //  19 PC3
  PC,  //  20 PC4
  PC,  //  21 PC5
  PC,  //  22 PC6
  PC,  //  23 PC7
  PD,  //  24 PD0
  PD,  //  25 PD1
  PD,  //  26 PD2
  PD,  //  27 PD3
  PD,  //  28 PD4
  PD,  //  29 PD5
  PD,  //  30 PD6
  PD,  //  31 PD7
  PE,  //  32 PE0
  PE,  //  33 PE1
  PE,  //  34 PE2
  PE,  //  35 PE3
  PE,  //  36 PE4
  PE,  //  37 PE5
  PE,  //  38 PE6
  PE,  //  39 PE7
  PF,  //  40 PF0
  PF,  //  41 PF1
  PF,  //  42 PF2
  PF,  //  43 PF3
  PF,  //  44 PF4
  PF,  //  45 PF5
  PG,  //  46 PG0
  PG,  //  47 PG1
  PG,  //  48 PG2
  PG,  //  49 PG3
  PG,  //  50 PG4
  PG,  //  51 PG5
  PG,  //  52 PG6
  PG,  //  53 PG7
  PF,  //  54 PF6
  //PF,//  55 PF7 (UPDI)
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
  PIN2_bp,  //   2 PA2
  PIN3_bp,  //   3 PA3
  PIN4_bp,  //   4 PA4
  PIN5_bp,  //   5 PA5
  PIN6_bp,  //   6 PA6
  PIN7_bp,  //   7 PA7
  PIN0_bp,  //   8 PB0
  PIN1_bp,  //   9 PB1
  PIN2_bp,  //  10 PB2
  PIN3_bp,  //  11 PB3
  PIN4_bp,  //  12 PB4
  PIN5_bp,  //  13 PB5
  PIN6_bp,  //  14 PB6
  PIN7_bp,  //  15 PB7
  PIN0_bp,  //  16 PC0
  PIN1_bp,  //  17 PC1
  PIN2_bp,  //  18 PC2
  PIN3_bp,  //  19 PC3
  PIN4_bp,  //  20 PC4
  PIN5_bp,  //  21 PC5
  PIN6_bp,  //  22 PC6
  PIN7_bp,  //  23 PC7
  PIN0_bp,  //  24 PD0
  PIN1_bp,  //  25 PD1
  PIN2_bp,  //  26 PD2
  PIN3_bp,  //  27 PD3
  PIN4_bp,  //  28 PD4
  PIN5_bp,  //  29 PD5
  PIN6_bp,  //  30 PD6
  PIN7_bp,  //  31 PD7
  PIN0_bp,  //  32 PE0
  PIN1_bp,  //  33 PE1
  PIN2_bp,  //  34 PE2
  PIN3_bp,  //  35 PE3
  PIN4_bp,  //  36 PE4
  PIN5_bp,  //  37 PE5
  PIN6_bp,  //  38 PE6
  PIN7_bp,  //  39 PE7
  PIN0_bp,  //  40 PF0
  PIN1_bp,  //  41 PF1
  PIN2_bp,  //  42 PF2
  PIN3_bp,  //  43 PF3
  PIN4_bp,  //  44 PF4
  PIN5_bp,  //  45 PF5
  PIN0_bp,  //  46 PG0
  PIN1_bp,  //  47 PG1
  PIN2_bp,  //  48 PG2
  PIN3_bp,  //  49 PG3
  PIN4_bp,  //  50 PG4
  PIN5_bp,  //  51 PG5
  PIN6_bp,  //  52 PG6
  PIN7_bp,  //  53 PG7
  PIN6_bp,  //  54 PF6
  //PIN7_bp,//  55 PF7 (UPDI)
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
  PIN2_bm,  //   2 PA2
  PIN3_bm,  //   3 PA3
  PIN4_bm,  //   4 PA4
  PIN5_bm,  //   5 PA5
  PIN6_bm,  //   6 PA6
  PIN7_bm,  //   7 PA7
  PIN0_bm,  //   8 PB0
  PIN1_bm,  //   9 PB1
  PIN2_bm,  //  10 PB2
  PIN3_bm,  //  11 PB3
  PIN4_bm,  //  12 PB4
  PIN5_bm,  //  13 PB5
  PIN6_bm,  //  14 PB6
  PIN7_bm,  //  15 PB7
  PIN0_bm,  //  16 PC0
  PIN1_bm,  //  17 PC1
  PIN2_bm,  //  18 PC2
  PIN3_bm,  //  19 PC3
  PIN4_bm,  //  20 PC4
  PIN5_bm,  //  21 PC5
  PIN6_bm,  //  22 PC6
  PIN7_bm,  //  23 PC7
  PIN0_bm,  //  24 PD0
  PIN1_bm,  //  25 PD1
  PIN2_bm,  //  26 PD2
  PIN3_bm,  //  27 PD3
  PIN4_bm,  //  28 PD4
  PIN5_bm,  //  29 PD5
  PIN6_bm,  //  30 PD6
  PIN7_bm,  //  31 PD7
  PIN0_bm,  //  32 PE0
  PIN1_bm,  //  33 PE1
  PIN2_bm,  //  34 PE2
  PIN3_bm,  //  35 PE3
  PIN4_bm,  //  36 PE4
  PIN5_bm,  //  37 PE5
  PIN6_bm,  //  38 PE6
  PIN7_bm,  //  39 PE7
  PIN0_bm,  //  40 PF0
  PIN1_bm,  //  41 PF1
  PIN2_bm,  //  42 PF2
  PIN3_bm,  //  43 PF3
  PIN4_bm,  //  44 PF4
  PIN5_bm,  //  45 PF5
  PIN0_bm,  //  46 PG0
  PIN1_bm,  //  47 PG1
  PIN2_bm,  //  48 PG2
  PIN3_bm,  //  49 PG3
  PIN4_bm,  //  50 PG4
  PIN5_bm,  //  51 PG5
  PIN6_bm,  //  52 PG6
  PIN7_bm,  //  53 PG7
  PIN6_bm,  //  54 PF6
  //PIN7_bm,//  55 PF7 (UPDI)
};
/* This only covers type B amd (for now) type D timer, so it will be a "timers of last resort" list.
   that will not cover type D timers once working silicon (TCD portmux issue) is available. */
const uint8_t digital_pin_to_timer[] = {
  NOT_ON_TIMER, //   0 PA0
  NOT_ON_TIMER, //   1 PA1
  NOT_ON_TIMER, //   2 PA2
  NOT_ON_TIMER, //   3 PA3
  TIMERD0_0WOA, //   4 PA4/MOSI      WOA
  TIMERD0_0WOB, //   5 PA5/MISO      WOB
  TIMERD0_0WOC, //   6 PA6/SCK       WOC mirrors WOA
  TIMERD0_0WOD, //   7 PA7/SS/CLKOUT WOD mirrors WOB
  NOT_ON_TIMER, //   8 PB0
  NOT_ON_TIMER, //   9 PB1
  NOT_ON_TIMER, //  10 PB2
  NOT_ON_TIMER, //  11 PB3
  NOT_ON_TIMER, //  12 PB4
  NOT_ON_TIMER, //  13 PB5
  NOT_ON_TIMER, //  14 PB6
  NOT_ON_TIMER, //  15 PB7
  TIMERB2,      //  16 PC0
  TIMERB3,      //  17 PC1
  NOT_ON_TIMER, //  18 PC2
  NOT_ON_TIMER, //  19 PC3
  NOT_ON_TIMER, //  20 PC4
  NOT_ON_TIMER, //  21 PC5
  NOT_ON_TIMER, //  22 PC6
  NOT_ON_TIMER, //  23 PC7
  NOT_ON_TIMER, //  24 PD0
  NOT_ON_TIMER, //  25 PD1
  NOT_ON_TIMER, //  26 PD2
  NOT_ON_TIMER, //  27 PD3
  NOT_ON_TIMER, //  28 PD4
  NOT_ON_TIMER, //  29 PD5
  DACOUT,       //  30 PD6
  NOT_ON_TIMER, //  31 PD7
  NOT_ON_TIMER, //  32 PE0
  NOT_ON_TIMER, //  33 PE1
  NOT_ON_TIMER, //  34 PE2
  NOT_ON_TIMER, //  35 PE3
  NOT_ON_TIMER, //  36 PE4
  NOT_ON_TIMER, //  37 PE5
  NOT_ON_TIMER, //  38 PE6
  NOT_ON_TIMER, //  39 PE7
  NOT_ON_TIMER, //  40 PF0 (TOSC1)
  NOT_ON_TIMER, //  41 PF1 (TOSC2)
  NOT_ON_TIMER, //  42 PF2
  NOT_ON_TIMER, //  43 PF3
  TIMERB0,      //  44 PF4
  TIMERB1,      //  45 PF5
  NOT_ON_TIMER, //  46 PG0
  NOT_ON_TIMER, //  47 PG1
  NOT_ON_TIMER, //  48 PG2
  TIMERB4,      //  49 PG3
  NOT_ON_TIMER, //  50 PG4
  NOT_ON_TIMER, //  51 PG5
  NOT_ON_TIMER, //  52 PG6
  NOT_ON_TIMER, //  53 PG7
  NOT_ON_TIMER, //  54 PF6 (RESET)
  //NOT_ON_TIMER, //55 PF7 (UPDI)
};
#endif
  // These are used for CI testing. They should *not* *ever* be used except for CI-testing where we need to pick a viable pin to compile a sketch with that won't generate compile errors (we don't care whether it would;d actually work, we are concerned with )
  #if CLOCK_SOURCE != 0
    #define _VALID_DIGITAL_PIN(pin)  ((pin) >= && (pin) < 4 ? ((pin) + 2)
  #else
    #define _VALID_DIGITAL_PIN(pin)  ((pin) >= && (pin) < 4 ? ((pin) + 0 ): NOT_A_PIN)
  #endif
  #define    _VALID_ANALOG_PIN(pin)  ((pin) >= 0 && ((pin) <= 4) ?                     ((pin) + PIN_PD1) : NOT_A_PIN)
#endif
