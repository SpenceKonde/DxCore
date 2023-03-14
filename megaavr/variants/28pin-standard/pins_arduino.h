/*  (C) Spence Konde 2021 open source (LGPL2.1 see LICENSE.md) based on existing Arduino cores.*/
//                                                                                    *INDENT-OFF*
/*
 ###  #     # ####      ####   ###      # ####  ####       ##   ###
#   # #     # #   #     #   # #   #    #  #   # #   #     #  # #   #          #
#####  #   #  ####      #   # #####   #   #   # ####        #   ###  ### ###     ###
#   #   # #   #  #      #   # #   #  #    #   # #   #      #   #   #     #  # #  #  #
#   #    #    #   #     ####  #   # #     ####  ####      ####  ###      ###  #  #  #
=====================================================     ----------     #
Variant Definition file for generic DA/DB parts with 28 pins.            #


Part Numbers:
AVR128DA28 AVR64DA28 AVR32DA28
AVR128DB28 AVR64DB28 AVR32DB28

See VariantTemplate.h in extras folder an extensively annotated copy.

Include guard and include basic libraries. We are normally including this inside Arduino.h */

#ifndef Pins_Arduino_h
#define Pins_Arduino_h
#include <avr/pgmspace.h>
#include "timers.h"

#define DEFAULT_28PIN_PINOUT

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
#define PIN_PC0 (8)
#define PIN_PC1 (9)
#define PIN_PC2 (10)
#define PIN_PC3 (11)
#define PIN_PD0 (12) /* On DB28, this isn't a pin - there, everything else NOT_A_PIN's pin 12 but we can add 3 to it to get PIN_PD3. */
#define PIN_PD1 (13)
#define PIN_PD2 (14)
#define PIN_PD3 (15)
#define PIN_PD4 (16)
#define PIN_PD5 (17)
#define PIN_PD6 (18)
#define PIN_PD7 (19)
#define PIN_PF0 (20)
#define PIN_PF1 (21)
#define PIN_PF6 (22) /* RESET can be used as an input via fuse setting. It poses no reprogramming challenges, and has no output drivers. */
//#define PIN_PF7 (23) /*UPDI pin not available for alternative functions on DA/DB */

#if defined(MVIO)
  #define FAKE_PIN_PD0
#endif

        /*##   ##   ###  ###  ###  ###
        #   # #  # #      #  #    #
        ####  ####  ###   #  #     ###
        #   # #  #     #  #  #        #
        ####  #  # ####  ###  ###  #*/


#define PINS_COUNT                     23
#define NUM_DIGITAL_PINS               PINS_COUNT
#define NUM_ANALOG_INPUTS              10
#define NUM_RESERVED_PINS              0
#define NUM_INTERNALLY_USED_PINS       0
#define NUM_I2C_PINS                   2
#define NUM_SPI_PINS                   3
#define NUM_TOTAL_FREE_PINS            PINS_COUNT
#define NUM_TOTAL_PINS                 PINS_COUNT

#if !defined(LED_BUILTIN)
  #define LED_BUILTIN                  PIN_PA7
#endif

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

#define digitalPinToAnalogInput(p)     (((p) >= PIN_PC3 && (p) < PIN_PF0) ? (p) - PIN_PD0 : ((p) < PIN_PF6 ? ((p) - 4) : NOT_A_PIN))
#define analogChannelToDigitalPin(p)        ((p) < 8 ? ((p) + PIN_PD0) : (((p) == 16 ? PIN_PF0) : ((p) == 17 ? PIN_PF1 : NOT_A_PIN)))
#define analogInputToDigitalPin(p)                        analogChannelToDigitalPin((p) & 0x7F)
#define digitalOrAnalogPinToDigital(p)    (((p) & 0x80) ? analogChannelToDigitalPin((p) & 0x7F) : (((p)<=NUM_DIGITAL_PINS) ? (p) : NOT_A_PIN))
#define portToPinZero(port)               ((port) == PA ? PIN_PA0 : ((port)== PC ? PIN_PC0 : ((port)== PD ? PIN_PD0 : ((port)== PF ? PIN_PF0 : NOT_A_PIN))))


// PWM pins
#if defined(MILLIS_USE_TIMERB0)
  #define digitalPinHasPWMTCB(p) (((p) == PIN_PA3) || ((p) == PIN_PC0))
#elif defined(MILLIS_USE_TIMERB1)
  #define digitalPinHasPWMTCB(p) (((p) == PIN_PA2) || ((p) == PIN_PC0))
#elif defined(MILLIS_USE_TIMERB2)
  #define digitalPinHasPWMTCB(p) (((p) == PIN_PA2) || ((p) == PIN_PA3))
#else //no TCB's are used for millis
  #define digitalPinHasPWMTCB(p) (((p) == PIN_PA2) || ((p) == PIN_PA3) || ((p) == PIN_PC0))
#endif

// Timer pin mapping
#define TCA0_PINS 0x03                      // TCA0 output on PD[0:5]
#define TCB0_PINS 0x00                      // TCB0 output on PA2 (default), not PF4 (Doesn't exist here)
#define TCB1_PINS 0x00                      // TCB1 output on PA3 (default), not PF5 (Doesn't exist here)
#define TCB2_PINS 0x00                      // TCB2 output on PC0 (default), not PB4 (Doesn't exist here)
#define TCD0_PINS PORTMUX_TCD0_PORTA        //

#define PIN_TCA0_WO0_INIT PIN_PD0
#define PIN_TCB0_WO_INIT  PIN_PA2
#define PIN_TCB1_WO_INIT  PIN_PA3
#define PIN_TCB2_WO_INIT  PIN_PC0
#define PIN_TCD0_WOA_INIT PIN_PA4


//#define USE_TIMERD0_PWM is automatically set unless defined as 0 or 1; it will be enabled UNLESS TIMERD0_CLOCK_SETTING is and neither TIMERD0_TOP_SETTING nor F_TCD is.
#define NO_GLITCH_TIMERD0

#define digitalPinHasPWM(p)               (digitalPinHasPWMTCB(p) || ((p) >= PIN_PA4 && (p) <= PIN_PA7) || ((p) >= PIN_PD0 && (p) < PIN_PD6))


        /*##   ###  ####  ##### #   # #   # #   #
        #   # #   # #   #   #   ## ## #   #  # #
        ####  #   # ####    #   # # # #   #   #
        #     #   # #  #    #   #   # #   #  # #
        #      ###  #   #   #   #   #  ###  #   */

#define SPI_INTERFACES_COUNT   1 /* 2 SPI peripherals but SPI.h only uses one at a time, see that readme for details.*/
#define NUM_HWSERIAL_PORTS     3

// SPI 0
#define SPI_MUX                PORTMUX_SPI0_DEFAULT_gc
#define PIN_SPI_MISO           PIN_PA5
#define PIN_SPI_SCK            PIN_PA6
#define PIN_SPI_MOSI           PIN_PA4
#define PIN_SPI_SS             PIN_PA7

// SPI 1
#define SPI1_MUX                PORTMUX_SPI1_DEFAULT_gc
#define PIN_SPI1_MOSI           PIN_PC0
#define PIN_SPI1_MISO           PIN_PC1
#define PIN_SPI1_SCK            PIN_PC2
#define PIN_SPI1_SS             PIN_PC3

// TWI 0
#define PIN_WIRE_SDA           PIN_PA2
#define PIN_WIRE_SCL           PIN_PA3
#define PIN_WIRE_SDA_PINSWAP_2 PIN_PC2
#define PIN_WIRE_SCL_PINSWAP_2 PIN_PC3

// TWI 1 not available - no supported pins present!

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
#define HWSERIAL1_MUX_PINSWAP_NONE      PORTMUX_USART1_NONE_gc
#define PIN_HWSERIAL1_TX                PIN_PC0
#define PIN_HWSERIAL1_RX                PIN_PC1
#define PIN_HWSERIAL1_XCK               PIN_PC2
#define PIN_HWSERIAL1_XDIR              PIN_PC3

// USART 2
#define HWSERIAL2_MUX                   PORTMUX_USART2_DEFAULT_gc
#define HWSERIAL2_MUX_PINSWAP_NONE      PORTMUX_USART2_NONE_gc
#define PIN_HWSERIAL2_TX                PIN_PF0
#define PIN_HWSERIAL2_RX                PIN_PF1
#define PIN_HWSERIAL2_XCK               NOT_A_PIN
#define PIN_HWSERIAL2_XDIR              NOT_A_PIN

        /*##  #   #  ###  #     ###   ###      ####  ### #   #  ###
        #   # ##  # #   # #    #   # #         #   #  #  ##  # #
        ##### # # # ##### #    #   # #  ##     ####   #  # # #  ###
        #   # #  ## #   # #    #   # #   #     #      #  #  ##     #
        #   # #   # #   # ####  ###   ###      #     ### #   #  #*/

#ifndef MVIO
  // 32-pin parts with MVIO don't have an A0 or a PD0, as that physical pin is used for VDDIO2
  #define PIN_A0   PIN_PD0
  static const uint8_t A0  = PIN_A0;
#else
  #define PIN_A0   NOT_A_PIN
  static const uint8_t A0  = NOT_A_PIN;
#endif
#define PIN_A1   PIN_PD1
#define PIN_A2   PIN_PD2
#define PIN_A3   PIN_PD3
#define PIN_A4   PIN_PD4
#define PIN_A5   PIN_PD5
#define PIN_A6   PIN_PD6
#define PIN_A7   PIN_PD7
#define PIN_A16  PIN_PF0
#define PIN_A17  PIN_PF1
static const uint8_t A1  = PIN_A1;
static const uint8_t A2  = PIN_A2;
static const uint8_t A3  = PIN_A3;
static const uint8_t A4  = PIN_A4;
static const uint8_t A5  = PIN_A5;
static const uint8_t A6  = PIN_A6;
static const uint8_t A7  = PIN_A7;
static const uint8_t A16 = PIN_A16;
static const uint8_t A17 = PIN_A17;
/* Internal and library use only - see 64-pin standard version for details.*/
#define  AIN0  ADC_CH(0)
#define  AIN0  ADC_CH(0)
#define  AIN1  ADC_CH(1)
#define  AIN2  ADC_CH(2)
#define  AIN3  ADC_CH(3)
#define  AIN4  ADC_CH(4)
#define  AIN5  ADC_CH(5)
#define  AIN6  ADC_CH(6)
#define  AIN7  ADC_CH(7)
#define  AIN16 ADC_CH(16)
#define  AIN17 ADC_CH(17)

        /*##  ### #   #      ###  ####  ####   ###  #   #  ###
        #   #  #  ##  #     #   # #   # #   # #   #  # #  #
        ####   #  # # #     ##### ####  ####  #####   #    ###
        #      #  #  ##     #   # #  #  #  #  #   #   #       #
        #     ### #   #     #   # #   # #   # #   #   #    #*/

#ifdef ARDUINO_MAIN
/* this guards against multiple definition errors, since this file gets included by everything,
   but only in one case should these tables be generated */
const uint8_t digital_pin_to_port[] = {
  PA, //  0 PA0/USART0_Tx/CLKIN
  PA, //  1 PA1/USART0_Rx
  PA, //  2 PA2/SDA
  PA, //  3 PA3/SCL
  PA, //  4 PA4/MOSI
  PA, //  5 PA5/MISO
  PA, //  6 PA6/SCK
  PA, //  7 PA7/SS/CLKOUT/LED_BUILTIN
  PC, //  8 PC0/USART1_Tx/TCA0 PWM
  PC, //  9 PC1/USART1_Rx/TCA0 PWM
  PC, // 10 PC2/TCA0 PWM
  PC, // 11 PC3/TCA0 PWM
  PD, // 12 PD0/AIN0 - bit position and bit mask aren't defined if the pin doesn't exist, but we sometimes need to get port since it is the zero-pin.
  PD, // 13 PD1/AIN1
  PD, // 14 PD2/AIN2
  PD, // 15 PD3/AIN3/LED_BUILTIN
  PD, // 16 PD4/AIN4
  PD, // 17 PD5/AIN5
  PD, // 18 PD6/AIN6
  PD, // 19 PD7/AIN7/AREF
  PF, // 20 PF0/AIN16/USART2_Tx/TOSC1/XTAL0
  PF, // 21 PF1/AIN17/USART2_Rx/TOSC2/XTAL1
  PF, // 22 PF6 RESET
 // PF  // 23 PF7 UPDI
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
  #endif
  PIN2_bp, //  2 PA2/SDA
  PIN3_bp, //  3 PA3/SCL
  PIN4_bp, //  4 PA4/MOSI
  PIN5_bp, //  5 PA5/MISO
  PIN6_bp, //  6 PA6/SCK
  PIN7_bp, //  7 PA7/SS/CLKOUT
  PIN0_bp, //  8 PC0/USART1_Tx
  PIN1_bp, //  9 PC1/USART1_Rx
  PIN2_bp, // 10 PC2
  PIN3_bp, // 11 PC3
  #ifndef MVIO
    PIN0_bp, // 12 PD0/AIN0
  #else
    NOT_A_PIN,
  #endif
  PIN1_bp, // 13 PD1/AIN1
  PIN2_bp, // 14 PD2/AIN2
  PIN3_bp, // 15 PD3/AIN3/LED_BUILTIN
  PIN4_bp, // 16 PD4/AIN4
  PIN5_bp, // 17 PD5/AIN5
  PIN6_bp, // 18 PD6/AIN6
  PIN7_bp, // 19 PD7/AIN7/AREF
  PIN0_bp, // 20 PF0/USART2_Tx/TOSC1
  PIN1_bp, // 21 PF1/USART2_Rx/TOSC2
  PIN6_bp  // 22 PF6 RESET
// PIN7_bp  // 23 PF7 UPDI
};

const uint8_t digital_pin_to_bit_mask[] = {
  #if CLOCK_SOURCE == 0 // PA0 used for external clock and crystal.
    PIN0_bm,            // PA0
  #else
    NOT_A_PIN,
  #endif
  #if CLOCK_SOURCE == 1 // PA1 also used for crystal
    NOT_A_PIN,  //   1 PA1
  #else
    PIN1_bm,
  #endif
  PIN2_bm, //  2 PA2/SDA
  PIN3_bm, //  3 PA3/SCL
  PIN4_bm, //  4 PA4/MOSI
  PIN5_bm, //  5 PA5/MISO
  PIN6_bm, //  6 PA6/SCK
  PIN7_bm, //  7 PA7/SS/CLKOUT
  PIN0_bm, //  8 PC0/USART1_Tx
  PIN1_bm, //  9 PC1/USART1_Rx
  PIN2_bm, // 10 PC2
  PIN3_bm, // 11 PC3
  #ifndef MVIO
    PIN0_bm, // 12 PD0/AIN0
  #else
    NOT_A_PIN,
  #endif
  PIN1_bm, // 13 PD1/AIN1
  PIN2_bm, // 14 PD2/AIN2
  PIN3_bm, // 15 PD3/AIN3/LED_BUILTIN
  PIN4_bm, // 16 PD4/AIN4
  PIN5_bm, // 17 PD5/AIN5
  PIN6_bm, // 18 PD6/AIN6
  PIN7_bm, // 19 PD7/AIN7/AREF
  PIN0_bm, // 20 PF0/USART2_Tx/TOSC1
  PIN1_bm, // 21 PF1/USART2_Rx/TOSC2
  PIN6_bm, // 22 PF6 RESET
//PIN7_bm  // 23 PF7 UPDI
};         // *INDENT-ON*

const uint8_t digital_pin_to_timer[] = {
  NOT_ON_TIMER, //  0 PA0/USART0_Tx/CLKIN
  NOT_ON_TIMER, //  1 PA1/USART0_Rx
  TIMERB0,      //  2 PA2/SDA
  TIMERB1,      //  3 PA3/SCL
  TIMERD0,      //  4 PA4/MOSI      WOA
  TIMERD0,      //  5 PA5/MISO      WOB
  TIMERD0,      //  6 PA6/SCK       WOC mirrors WOA
  TIMERD0,      //  7 PA7/SS/CLKOUT WOD mirrors WOB
  TIMERB2,      //  8 PC0/USART1_Tx
  NOT_ON_TIMER, //  9 PC1/USART1_Rx
  NOT_ON_TIMER, // 10 PC2
  NOT_ON_TIMER, // 11 PC3
  NOT_ON_TIMER, // 12 (NOT A PIN on DB, DD)
  NOT_ON_TIMER, // 13 PD1/AIN1
  NOT_ON_TIMER, // 14 PD2/AIN2
  NOT_ON_TIMER, // 15 PD3/AIN3/LED_BUILTIN
  NOT_ON_TIMER, // 16 PD4/AIN4
  NOT_ON_TIMER, // 17 PD5/AIN5
  DACOUT,       // 18 PD6/AIN6
  NOT_ON_TIMER, // 19 PD7/AIN7/AREF
  NOT_ON_TIMER, // 20 PF0/USART2_Tx/TOSC1
  NOT_ON_TIMER, // 21 PF1/USART2_Rx/TOSC2
  NOT_ON_TIMER, // 22 PF6 RESET
//NOT_ON_TIMER  // 22 PF7 RESET
};

#endif  // These are used for CI testing. They should *not* *ever* be used except for CI-testing where we need to pick a viable pin to compile a sketch with that won't generate compile errors (we don't care whether it would;d actually work, we are concerned with )
  #if CLOCK_SOURCE != 0
    #define _VALID_DIGITAL_PIN(pin)  ((pin) >= && (pin) < 4 ? ((pin) + 2)
  #else
    #define _VALID_DIGITAL_PIN(pin)  ((pin) >= && (pin) < 4 ? ((pin) + 0 ): NOT_A_PIN)
  #endif
  #define    _VALID_ANALOG_PIN(pin)  ((pin) >= 0 && ((pin) <= 4) ?                     ((pin) + PIN_PD1) : NOT_A_PIN)
#endif
