#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <avr/pgmspace.h>
#include "timers.h"

#define DEFAULT_28PIN_PINOUT

// Arduino pin macros
#define PIN_PA0 0
#define PIN_PA1 1
#define PIN_PA2 2
#define PIN_PA3 3
#define PIN_PA4 4
#define PIN_PA5 5
#define PIN_PA6 6
#define PIN_PA7 7
#define PIN_PC0 8
#define PIN_PC1 9
#define PIN_PC2 10
#define PIN_PC3 11
#define PIN_PD0 12   // On DB/DD-series parts with under 48 pins, PD0 is not actually a real pin, but is left in the sequence for compatibility.
#define PIN_PD1 13
#define PIN_PD2 14
#define PIN_PD3 15
#define PIN_PD4 16
#define PIN_PD5 17
#define PIN_PD6 18
#define PIN_PD7 19
#define PIN_PF0 20
#define PIN_PF1 21
#define PIN_PF6 22

#define PINS_COUNT                     23
#define NUM_DIGITAL_PINS               PINS_COUNT
#define NUM_ANALOG_INPUTS              10
#define NUM_RESERVED_PINS              0
#define NUM_INTERNALLY_USED_PINS       0
#define NUM_I2C_PINS                   2
#define NUM_SPI_PINS                   3
#define NUM_TOTAL_FREE_PINS            PINS_COUNT
#define NUM_TOTAL_PINS                 PINS_COUNT
#define ANALOG_INPUT_OFFSET            12
#define LED_BUILTIN                    PIN_PA7
#define EXTERNAL_NUM_INTERRUPTS        47
#define digitalPinToAnalogInput(p)     (((p)>=PIN_PC3 && (p)<PIN_PF0)?(p) - PIN_PD0:((p)<PIN_PF6?((p)-4):NOT_A_PIN))
#define digitalOrAnalogPinToDigital(p) (((p)<=NUM_DIGITAL_PINS)?(p):((((p)&0x7F)<8)?(((p)&0x7F)+PIN_PD0):))

// PWM pins
#ifdef MVIO
#define FIRST_TCA_PWM PIN_PD1
#else
#define FIRST_TCA_PWM PIN_PD0
#endif

#if defined(MILLIS_USE_TIMERB0)
#define digitalPinHasPWM(p)  ((((p) >= FIRST_TCA_PWM) && ((p) <= PIN_PD5)) || ((p) == PIN_PA4) || ((p) == PIN_PA5) ||\
                              ((p) == PIN_PC0) || ((p) == PIN_PA3))
#elif defined(MILLIS_USE_TIMERB1)
#define digitalPinHasPWM(p)  ((((p) >= FIRST_TCA_PWM) && ((p) <= PIN_PD5)) || ((p) == PIN_PA4) || ((p) == PIN_PA5) ||\
                                ((p) == PIN_PC0) || ((p) == PIN_PA2))
#elif defined(MILLIS_USE_TIMERB2)
#define digitalPinHasPWM(p)  ((((p) >= FIRST_TCA_PWM) && ((p) <= PIN_PD5)) || ((p) == PIN_PA4) || ((p) == PIN_PA5) ||\
                                ((p) == PIN_PA2) || ((p) == PIN_PA3))
#else //no TCB's are used for PWM
#define digitalPinHasPWM(p)  ((((p) >= FIRST_TCA_PWM) && ((p) <= PIN_PD5)) || ((p) == PIN_PA4) || ((p) == PIN_PA5) ||\
                              ((p) == PIN_PC0) || ((p) == PIN_PA2) || ((p) == PIN_PA3))
#endif


// Timer pin mapping
#define TCA0_PINS PORTMUX_TCA0_PORTD_gc   // TCA0 output on PD[0:5]
#define TCB0_PINS 0x00                    // TCB0 output on PA2 (default), not PF4
#define TCB1_PINS 0x00                    // TCB1 output on PA3 (default), not PF5
#define TCB2_PINS 0x00                    // TCB2 output on PC0 (default), not PB4
#define TCD0_PINS PORTMUX_TCD0_DEFAULT_gc  // Only default port option works!

#define PIN_TCA0_WO0 PIN_PD0
#define PIN_TCD0_WOA PIN_PA4

#define USE_TIMERD0_PWM
#define NO_GLITCH_TIMERD0

// SPI 0
// No pinswap available
#define SPI_INTERFACES_COUNT   1
#define SPI_MUX                PORTMUX_SPI0_DEFAULT_gc
#define PIN_SPI_MISO           PIN_PA5
#define PIN_SPI_SCK            PIN_PA6
#define PIN_SPI_MOSI           PIN_PA4
#define PIN_SPI_SS             PIN_PA7
static const uint8_t SS   =    PIN_SPI_SS;
static const uint8_t MOSI =    PIN_SPI_MOSI;
static const uint8_t MISO =    PIN_SPI_MISO;
static const uint8_t SCK  =    PIN_SPI_SCK;

// SPI 1
// No pinswap available
#define SPI1_MUX                PORTMUX_SPI1_DEFAULT_gc
#define PIN_SPI1_MISO           PIN_PC0
#define PIN_SPI1_SCK            PIN_PC1
#define PIN_SPI1_MOSI           PIN_PC2
#define PIN_SPI1_SS             PIN_PC3
static const uint8_t SS1   =    PIN_SPI1_SS;
static const uint8_t MOSI1 =    PIN_SPI1_MOSI;
static const uint8_t MISO1 =    PIN_SPI1_MISO;
static const uint8_t SCK1  =    PIN_SPI1_SCK;

// TWI 0
// No pinswap enabled by default
#define PIN_WIRE_SDA           PIN_PA2
#define PIN_WIRE_SCL           PIN_PA3
#define PIN_WIRE_SDA_PINSWAP_2 PIN_PC2
#define PIN_WIRE_SCL_PINSWAP_2 PIN_PC3
static const uint8_t SDA =     PIN_WIRE_SDA;
static const uint8_t SCL =     PIN_WIRE_SCL;

// TWI 1 not available - no supported pins present!

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
// No pinswap available
#define HWSERIAL1                       &USART1
#define HWSERIAL1_DRE_VECTOR            USART1_DRE_vect
#define HWSERIAL1_DRE_VECTOR_NUM        USART1_DRE_vect_num
#define HWSERIAL1_RXC_VECTOR            USART1_RXC_vect
#define HWSERIAL1_MUX                   PORTMUX_USART1_DEFAULT_gc
#define HWSERIAL1_MUX_PINSWAP_1         PORTMUX_USART1_DEFAULT_gc
#define PIN_HWSERIAL1_TX                PIN_PC0
#define PIN_HWSERIAL1_RX                PIN_PC1
#define PIN_HWSERIAL1_TX_PINSWAP_1      PIN_PC0
#define PIN_HWSERIAL1_RX_PINSWAP_1      PIN_PC1

// USART 2
// No pinswap available
#define HWSERIAL2                       &USART2
#define HWSERIAL2_DRE_VECTOR            USART2_DRE_vect
#define HWSERIAL2_DRE_VECTOR_NUM        USART2_DRE_vect_num
#define HWSERIAL2_RXC_VECTOR            USART2_RXC_vect
#define HWSERIAL2_MUX                   PORTMUX_USART2_DEFAULT_gc
#define HWSERIAL2_MUX_PINSWAP_1         PORTMUX_USART2_DEFAULT_gc
#define PIN_HWSERIAL2_TX                PIN_PF0
#define PIN_HWSERIAL2_RX                PIN_PF1
#define PIN_HWSERIAL2_TX_PINSWAP_1      PIN_PF0
#define PIN_HWSERIAL2_RX_PINSWAP_1      PIN_PF1

// Analog pins

#ifndef MVIO
// 28-pin parts with MVIO don't have an A0 or a PD0, as that physical pin is used for VDDIO2
#define PIN_A0   PIN_PD0
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
#ifndef MVIO
static const uint8_t A0  = PIN_A0;
#endif
static const uint8_t A1  = PIN_A1;
static const uint8_t A2  = PIN_A2;
static const uint8_t A3  = PIN_A3;
static const uint8_t A4  = PIN_A4;
static const uint8_t A5  = PIN_A5;
static const uint8_t A6  = PIN_A6;
static const uint8_t A7  = PIN_A7;
static const uint8_t A16 = PIN_A16;
static const uint8_t A17 = PIN_A17;

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
  PC, //  8 PC0/USART1_Tx/TCA0 PWM
  PC, //  9 PC1/USART1_Rx/TCA0 PWM
  PC, // 10 PC2/TCA0 PWM
  PC, // 11 PC3/TCA0 PWM
#ifndef MVIO
  PD, // 12 PD0/AIN0
#else
  NOT_A_PORT,
#endif
  PD, // 13 PD1/AIN1
  PD, // 14 PD2/AIN2
  PD, // 15 PD3/AIN3/LED_BUILTIN
  PD, // 16 PD4/AIN4
  PD, // 17 PD5/AIN5
  PD, // 18 PD6/AIN6
  PD, // 19 PD7/AIN7/AREF
  PF, // 20 PF0/AIN16/USART2_Tx/TOSC1
  PF, // 21 PF1/AIN17/USART2_Rx/TOSC2
  PF  // 22 PF6 RESET
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
  PIN0_bp, //  8 PC0/USART1_Tx
  PIN1_bp, //  9 PC1/USART1_Rx
  PIN2_bp, // 10 PC2
  PIN3_bp, // 11 PC3
#ifndef MVIO
  PIN0_bp, // 12 PD0/AIN0
#else
  NOT_A_PORT,
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
};

/* Use this for accessing PINnCTRL register */
const uint8_t digital_pin_to_bit_mask[] = {
  PIN0_bm, //  0 PA0/USART0_Tx/CLKIN
  PIN1_bm, //  1 PA1/USART0_Rx
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
  NOT_A_PORT,
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
  PIN6_bm  // 22 PF6 RESET
};

const uint8_t digital_pin_to_timer[] = {
  NOT_ON_TIMER, //  0 PA0/USART0_Tx/CLKIN
  NOT_ON_TIMER, //  1 PA1/USART0_Rx
#if !defined(MILLIS_USE_TIMERB0)
  TIMERB0,      //  2 PA2/SDA
#else
  NOT_ON_TIMER, //  2 PA2/SDA
#endif
#if !defined(MILLIS_USE_TIMERB1)
  TIMERB1,      //  3 PA3/SCL
#else
  NOT_ON_TIMER, //  3 PA3/SCL
#endif
  NOT_ON_TIMER, //  4 PA4/MOSI
  NOT_ON_TIMER, //  5 PA5/MISO
  TIMERD0,      //  6 PA6/SCK
  TIMERD0,      //  7 PA7/SS/CLKOUT
#if !defined(MILLIS_USE_TIMERB2)
  TIMERB2,      //  8 PC0/USART1_Tx
#else
  NOT_ON_TIMER, //  8 PC0/USART1_Tx
#endif
  NOT_ON_TIMER, //  9 PC1/USART1_Rx
  NOT_ON_TIMER, // 10 PC2
  NOT_ON_TIMER, // 11 PC3
#ifndef MVIO
  TIMERA0, // 12 PD0/AIN0
#else
  NOT_ON_TIMER,
#endif
  TIMERA0,      // 13 PD1/AIN1
  TIMERA0,      // 14 PD2/AIN2
  TIMERA0,      // 15 PD3/AIN3/LED_BUILTIN
  TIMERA0,      // 16 PD4/AIN4
  TIMERA0,      // 17 PD5/AIN5
  DACOUT,       // 18 PD6/AIN6
  NOT_ON_TIMER, // 19 PD7/AIN7/AREF
  NOT_ON_TIMER, // 20 PF0/USART2_Tx/TOSC1
  NOT_ON_TIMER, // 21 PF1/USART2_Rx/TOSC2
  NOT_ON_TIMER  // 22 PF6 RESET
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
