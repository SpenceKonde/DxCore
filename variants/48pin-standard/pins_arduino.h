#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <avr/pgmspace.h>
#include "timers.h"

#define DEFAULT_48PIN_PINOUT

// Arduino pin macros
#define PIN_PA0 0
#define PIN_PA1 1
#define PIN_PA2 2
#define PIN_PA3 3
#define PIN_PA4 4
#define PIN_PA5 5
#define PIN_PA6 6
#define PIN_PA7 7
#define PIN_PB0 8
#define PIN_PB1 9
#define PIN_PB2 10
#define PIN_PB3 11
#define PIN_PB4 12
#define PIN_PB5 13
#define PIN_PC0 14
#define PIN_PC1 15
#define PIN_PC2 16
#define PIN_PC3 17
#define PIN_PC4 18
#define PIN_PC5 19
#define PIN_PC6 20
#define PIN_PC7 21
#define PIN_PD0 22
#define PIN_PD1 23
#define PIN_PD2 24
#define PIN_PD3 25
#define PIN_PD4 26
#define PIN_PD5 27
#define PIN_PD6 28
#define PIN_PD7 29
#define PIN_PE0 30
#define PIN_PE1 31
#define PIN_PE2 32
#define PIN_PE3 33
#define PIN_PF0 34
#define PIN_PF1 35
#define PIN_PF2 36
#define PIN_PF3 37
#define PIN_PF4 38
#define PIN_PF5 39
#define PIN_PF6 40

#define PINS_COUNT                     41
#define NUM_DIGITAL_PINS               PINS_COUNT
#define NUM_ANALOG_INPUTS              18
#define NUM_RESERVED_PINS              0
#define NUM_INTERNALLY_USED_PINS       0
#define NUM_I2C_PINS                   2
#define NUM_SPI_PINS                   3
#define NUM_TOTAL_FREE_PINS            NUM_DIGITAL_PINS
#define NUM_TOTAL_PINS                 NUM_DIGITAL_PINS
//#define ANALOG_INPUT_OFFSET            22 //Hopefully not used elsewhere!
#define LED_BUILTIN                    (PIN_PA7)
#define EXTERNAL_NUM_INTERRUPTS        47
#define digitalPinToAnalogInput(p)     (((p)>PIN_PC7 && (p)<PIN_PF0)? ((p)-PIN_PD0):((p)<PIN_PF6?((p)-18) : NOT_A_PIN))
#define digitalOrAnalogPinToDigital(p) (((p)<=NUM_DIGITAL_PINS)?(p):NOT_A_PIN)


// PWM pins
#if defined(MILLIS_USE_TIMERB0)
#define digitalPinHasPWM(p)  ((((p) >= PIN_PB0) && ((p) <= PIN_PC5)) || ((p) == PIN_PF2) || ((p) == PIN_PF3) ||\
                              ((p) == PIN_PC0) || ((p) == PIN_PC1) || ((p) == PIN_PF5))
#elif defined(MILLIS_USE_TIMERB1)
#define digitalPinHasPWM(p)  ((((p) >= PIN_PB0) && ((p) <= PIN_PC5)) || ((p) == PIN_PF2) || ((p) == PIN_PF3) ||\
                                ((p) == PIN_PC0) || ((p) == PIN_PC1) || ((p) == PIN_PF4))
#elif defined(MILLIS_USE_TIMERB2)
#define digitalPinHasPWM(p)  ((((p) >= PIN_PB0) && ((p) <= PIN_PC5)) || ((p) == PIN_PF2) || ((p) == PIN_PF3) ||\
                                ((p) == PIN_PC1) || ((p) == PIN_PF4) || ((p) == PIN_PF5))
#elif defined(MILLIS_USE_TIMERB3)
#define digitalPinHasPWM(p)  ((((p) >= PIN_PB0) && ((p) <= PIN_PC5)) || ((p) == PIN_PF2) || ((p) == PIN_PF3) ||\
                                ((p) == PIN_PC0) || ((p) == PIN_PF4) || ((p) == PIN_PF5))
#else //no TCB's are used for PWM
#define digitalPinHasPWM(p)  ((((p) >= PIN_PB0) && ((p) <= PIN_PC5)) || ((p) == PIN_PF2) || ((p) == PIN_PF3) ||\
                              ((p) == PIN_PC0) || ((p) == PIN_PC1) || ((p) == PIN_PF4) || ((p) == PIN_PF5))
#endif

// Timer pin swaps
#define TCA0_PINS PORTMUX_TCA0_PORTC_gc // TCA0 output on PC[0:5]
#define TCA1_PINS 0x00 // TCA0 output on PB[0:5] (no pinswap)
#define TCB0_PINS PORTMUX_TCB0_bm       // TCB0 output on PF4 instead of PA2 (default)
#define TCB1_PINS PORTMUX_TCB1_bm       // TCB1 output on PF5 instead of PA3 (default)
#define TCB2_PINS 0x00                  // TCB2 output on PC0 (default) instead of PB4 (either way not used)
#define TCB3_PINS 0x00                  // TCB3 output on PB5 (default) instead of PC1 (either way not used)
#define TCD0_PINS PORTMUX_TCD0_DEFAULT_gc  // Only default port option works!

#define PIN_TCA0_WO0 PIN_PC0
#define PIN_TCA1_WO0 PIN_PB0
#define PIN_TCD0_WOA PIN_PA4

#define USE_TIMERD0_PWM
#define NO_GLITCH_TIMERD0

// SPI 0
// No pinswap enabled by default
// Pinswap 2 not available
#define SPI_INTERFACES_COUNT   1
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
static const uint8_t MOSI =    PIN_SPI_MOSI;
static const uint8_t MISO =    PIN_SPI_MISO;
static const uint8_t SCK  =    PIN_SPI_SCK;
static const uint8_t SS   =    PIN_SPI_SS;



// SPI 1
// No pinswap enabled by default
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
static const uint8_t MOSI1 = PIN_SPI1_MOSI;
static const uint8_t MISO1 = PIN_SPI1_MISO;
static const uint8_t SCK1  = PIN_SPI1_SCK;
static const uint8_t SS1   = PIN_SPI1_SS;

// TWI 0
// No pinswap enabled by default
#define PIN_WIRE_SDA           PIN_PA2
#define PIN_WIRE_SCL           PIN_PA3
#define PIN_WIRE_SDA_PINSWAP_2 PIN_PC2
#define PIN_WIRE_SCL_PINSWAP_2 PIN_PC3
static const uint8_t SDA =     PIN_WIRE_SDA;
static const uint8_t SCL =     PIN_WIRE_SCL;


// TWI 1
// No pinswap available
#define PIN_WIRE1_SDA           PIN_PF2
#define PIN_WIRE1_SCL           PIN_PF3
#define PIN_WIRE1_SDA_PINSWAP_2 PIN_PB2
#define PIN_WIRE1_SCL_PINSWAP_2 PIN_PB3
static const uint8_t SDA1 =     PIN_WIRE1_SDA;
static const uint8_t SCL1 =     PIN_WIRE1_SCL;

// USART 0
// No pinswap by default
#define HWSERIAL0                       &USART0
#define HWSERIAL0_DRE_VECTOR            USART0_DRE_vect
#define HWSERIAL0_DRE_VECTOR_NUM        USART0_DRE_vect_num
#define HWSERIAL0_RXC_VECTOR            USART0_RXC_vect
#define HWSERIAL0_MUX                   PORTMUX_USART0_DEFAULT_gc
#define HWSERIAL0_MUX_PINSWAP_1         PORTMUX_USART0_ALT1_gc
#define PIN_HWSERIAL0_TX                PIN_PA0
#define PIN_HWSERIAL0_RX                PIN_PA1
#define PIN_HWSERIAL0_TX_PINSWAP_1      PIN_PA4
#define PIN_HWSERIAL0_RX_PINSWAP_1      PIN_PA5

// USART1
// No pinswap by default
#define HWSERIAL1                       &USART1
#define HWSERIAL1_DRE_VECTOR            USART1_DRE_vect
#define HWSERIAL1_DRE_VECTOR_NUM        USART1_DRE_vect_num
#define HWSERIAL1_RXC_VECTOR            USART1_RXC_vect
#define HWSERIAL1_MUX                   PORTMUX_USART1_DEFAULT_gc
#define HWSERIAL1_MUX_PINSWAP_1         PORTMUX_USART1_ALT1_gc
#define PIN_HWSERIAL1_TX                PIN_PC0
#define PIN_HWSERIAL1_RX                PIN_PC1
#define PIN_HWSERIAL1_TX_PINSWAP_1      PIN_PC4
#define PIN_HWSERIAL1_RX_PINSWAP_1      PIN_PC5

// USART 2
// No pinswap by default
#define HWSERIAL2                       &USART2
#define HWSERIAL2_DRE_VECTOR            USART2_DRE_vect
#define HWSERIAL2_DRE_VECTOR_NUM        USART2_DRE_vect_num
#define HWSERIAL2_RXC_VECTOR            USART2_RXC_vect
#define HWSERIAL2_MUX                   PORTMUX_USART2_DEFAULT_gc
#define HWSERIAL2_MUX_PINSWAP_1         PORTMUX_USART2_ALT1_gc
#define PIN_HWSERIAL2_TX                PIN_PF0
#define PIN_HWSERIAL2_RX                PIN_PF1
#define PIN_HWSERIAL2_TX_PINSWAP_1      PIN_PF4
#define PIN_HWSERIAL2_RX_PINSWAP_1      PIN_PF5

// USART 3
// No pinswap by default
#define HWSERIAL3                       &USART3
#define HWSERIAL3_DRE_VECTOR            USART3_DRE_vect
#define HWSERIAL3_DRE_VECTOR_NUM        USART3_DRE_vect_num
#define HWSERIAL3_RXC_VECTOR            USART3_RXC_vect
#define HWSERIAL3_MUX                   PORTMUX_USART3_DEFAULT_gc
#define HWSERIAL3_MUX_PINSWAP_1         PORTMUX_USART3_ALT1_gc
#define PIN_HWSERIAL3_TX                PIN_PB0
#define PIN_HWSERIAL3_RX                PIN_PB1
#define PIN_HWSERIAL3_TX_PINSWAP_1      PIN_PB4
#define PIN_HWSERIAL3_RX_PINSWAP_1      PIN_PB5

// USART 4
// No pinswap available
#define HWSERIAL4                       &USART4
#define HWSERIAL4_DRE_VECTOR            USART4_DRE_vect
#define HWSERIAL4_DRE_VECTOR_NUM        USART4_DRE_vect_num
#define HWSERIAL4_RXC_VECTOR            USART4_RXC_vect
#define HWSERIAL4_MUX                   PORTMUX_USART4_DEFAULT_gc
#define HWSERIAL4_MUX_PINSWAP_1         PORTMUX_USART4_DEFAULT_gc
#define PIN_HWSERIAL4_TX                PIN_PE0
#define PIN_HWSERIAL4_RX                PIN_PE1
#define PIN_HWSERIAL4_TX_PINSWAP_1      PIN_PE0
#define PIN_HWSERIAL4_RX_PINSWAP_1      PIN_PE1

// Analog pins
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
static const uint8_t A16 = PIN_A16;
static const uint8_t A17 = PIN_A17;
static const uint8_t A18 = PIN_A18;
static const uint8_t A19 = PIN_A19;
static const uint8_t A20 = PIN_A20;
static const uint8_t A21 = PIN_A21;

#ifdef ARDUINO_MAIN

const uint8_t digital_pin_to_port[] = {
  PA, //  0 PA0/USART0_Tx/CLKIN
  PA, //  1 PA1/USART0_Rx
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
  PF  // 40 PF6 RESET
};

/* Use this for accessing PINnCTRL register */
const uint8_t digital_pin_to_bit_position[] = {
  PIN0_bp, //  0 PIN_bp0/USART0_Tx/CLKIN
  PIN1_bp, //  1 PA1/USART0_Rx
  PIN2_bp, //  2 PA2/SDA
  PIN3_bp, //  3 PA3/SCL
  PIN4_bp, //  4 PA4/MOSI
  PIN5_bp, //  5 PA5/MISO
  PIN6_bp, //  6 PA6/SCK
  PIN7_bp, //  7 PA7/SS/CLKOUT
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
  PIN5_bp, // 39 PF5/AIN15/LED_BUILTIN
  PIN6_bp  // 40 PF6 RESET
};

/* Use this for accessing PINnCTRL register */
const uint8_t digital_pin_to_bit_mask[] = {
  PIN0_bm, //  0 PIN_bp0/USART0_Tx/CLKIN
  PIN1_bm, //  1 PA1/USART0_Rx
  PIN2_bm, //  2 PA2/SDA
  PIN3_bm, //  3 PA3/SCL
  PIN4_bm, //  4 PA4/MOSI
  PIN5_bm, //  5 PA5/MISO
  PIN6_bm, //  6 PA6/SCK
  PIN7_bm, //  7 PA7/SS/CLKOUT
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
  PIN5_bm, // 39 PF5/AIN15/LED_BUILTIN
  PIN6_bm  // 40 PF6 RESET
};

const uint8_t digital_pin_to_timer[] = {
  NOT_ON_TIMER, //  0 PA0/USART0_Tx/CLKIN
  NOT_ON_TIMER, //  1 PA1/USART0_Rx
  NOT_ON_TIMER, //  2 PA2/SDA
  NOT_ON_TIMER, //  3 PA3/SCL
  NOT_ON_TIMER, //  4 PA4/MOSI
  NOT_ON_TIMER, //  5 PA5/MISO
  TIMERD0,      //  6 PA6/SCK
  TIMERD0,      //  7 PA7/SS/CLKOUT
  TIMERA1,      //  8 PB0/USART3_Tx
  TIMERA1,      //  9 PB1/USART3_Rx
  TIMERA1,      // 10 PB2
  TIMERA1,      // 11 PB3
  TIMERA1,      // 12 PB4
  TIMERA1,      // 13 PB5
  TIMERA0,      // 14 PC0/USART1_Tx/TCA0 PWM
  TIMERA0,      // 15 PC1/USART1_Rx/TCA0 PWM
  TIMERA0,      // 16 PC2/TCA0 PWM
  TIMERA0,      // 17 PC3/TCA0 PWM
  TIMERA0,      // 18 PC4/TCA0 PWM
  TIMERA0,      // 19 PC5/TCA0 PWM
  NOT_ON_TIMER, // 20 PC6
  NOT_ON_TIMER, // 21 PC7
  NOT_ON_TIMER, // 22 PD0/AIN0
  NOT_ON_TIMER, // 23 PD1/AIN1
  NOT_ON_TIMER, // 24 PD2/AIN2
  NOT_ON_TIMER, // 25 PD3/AIN3
  NOT_ON_TIMER, // 26 PD4/AIN4
  NOT_ON_TIMER, // 27 PD5/AIN5
  DACOUT, // 28 PD6/AIN6
  NOT_ON_TIMER, // 29 PD7/AIN7/AREF
  NOT_ON_TIMER, // 30 PE0/AIN8
  NOT_ON_TIMER, // 31 PE1/AIN9
  NOT_ON_TIMER, // 32 PE2/AIN10
  NOT_ON_TIMER, // 33 PE3/AIN11
  NOT_ON_TIMER, // 34 PF0/USART2_Tx/TOSC1
  NOT_ON_TIMER, // 35 PF1/USART2_Rx/TOSC2
  NOT_ON_TIMER,      // 36 PF2/AIN12
  NOT_ON_TIMER,      // 37 PF3/AIN13
#if !defined(MILLIS_USE_TIMERB0)
  TIMERB0,      // 38 PF4/AIN14
#else
  NOT_ON_TIMER, // 38 PF4/AIN14
#endif
#if !defined(MILLIS_USE_TIMERB1)
  TIMERB1,      // 39 PF5/AIN15/LED_BUILTIN
#else
  NOT_ON_TIMER, // 39 PF5/AIN15/LED_BUILTIN
#endif
  NOT_ON_TIMER  // 40 PF6 RESET
};

#endif

// These serial port names are intended to allow libraries and architecture-neutral
// sketches to automatically default to the correct port name for a particular type
// of use.  For example, a GPS module would normally connect to SERIAL_PORT_HARDWARE_OPEN,
// the first hardware serial port whose RX/TX pins are not dedicated to another use.
//
// SERIAL_PORT_MONITOR        Port which normally prints to the Arduino Serial Monitor
//
// SERIAL_PORT_USBVIRTUAL     Port which is USB virtual serial
//
// SERIAL_PORT_LINUXBRIDGE    Port which connects to a Linux system via Bridge library
//
// SERIAL_PORT_HARDWARE       Hardware serial port, physical RX & TX pins.
//
// SERIAL_PORT_HARDWARE_OPEN  Hardware serial ports which are open for use.  Their RX & TX
//                            pins are NOT connected to anything by default.

#define SERIAL_PORT_MONITOR       Serial
#define SERIAL_PORT_HARDWARE      Serial1
#define SERIAL_PORT_HARDWARE_OPEN Serial2

// Spence Konde: Okay, I think this is pretty stupid, but it's also harmless, which is
// better than many of the stupid things that are now part of the "API" - since I can
// support this without kneecapping the core, why not?
// A library that needs to use serial ports for something should *really* demand
// the user pass the desired serial port to use to the constructor or equivalent.
//
// Serial (USART0) as the Serial Monitor serial port.
// Serial2 (USART2) as the "hardware" serial port (I mean, Serial also is... but it
// is the one most likely also connected to a serial monitor.
// Serial1 (USART1) as the "open hardware" serial port on the grounds that it's not the
// serial monitor port, and the only pins for USART2 are also the only pins available
// for a watch crystal for autotuning and keeping time while sleeping on the 28-pin
// parts.
//
// Kept the same parts across larger pincounts for consistency, though I'd be easy
// to convince that this was a bad idea.
//
// If having definitions here ends up making anything worse than it otherwise would
// be, please report it in the issues. My sense is that if a library uses these
// it would either be unusable without them, or it offers the user a way to pass
// in a serial port, and if they don't like these, passing one in would override it.
//
// Note: On DB-series parts, Serial1 is the MVIO serial port; there's no define
// here for that, of course. Which will get a bit awkward, as I think about it,
// because on the DD-series parts, if they've got 14 or 20 pins, Serial/USART0
// on an alternate pin mapping will be the MVIO serial port - they don't have PC0
// so Serial1 on default mapping doesn't have it's TX pin, but there's a USART0
// mapping with TX/RX on PC1/PC2. So a SERIAL_PORT_MVIO define *would* be handy!

#endif
