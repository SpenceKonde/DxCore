#ifndef Pins_Arduino_h
#define Pins_Arduino_h


#include <avr/pgmspace.h>
#include "timers.h"

#define DEFAULT_64PIN_PINOUT

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
#define PIN_PB6 14
#define PIN_PB7 15
#define PIN_PC0 16
#define PIN_PC1 17
#define PIN_PC2 18
#define PIN_PC3 19
#define PIN_PC4 20
#define PIN_PC5 21
#define PIN_PC6 22
#define PIN_PC7 23
#define PIN_PD0 24
#define PIN_PD1 25
#define PIN_PD2 26
#define PIN_PD3 27
#define PIN_PD4 28
#define PIN_PD5 29
#define PIN_PD6 30
#define PIN_PD7 31
#define PIN_PE0 32
#define PIN_PE1 33
#define PIN_PE2 34
#define PIN_PE3 35
#define PIN_PE4 36
#define PIN_PE5 37
#define PIN_PE6 38
#define PIN_PE7 39
#define PIN_PF0 40
#define PIN_PF1 41
#define PIN_PF2 42
#define PIN_PF3 43
#define PIN_PF4 44
#define PIN_PF5 45
#define PIN_PG0 46
#define PIN_PG1 47
#define PIN_PG2 48
#define PIN_PG3 49
#define PIN_PG4 50
#define PIN_PG5 51
#define PIN_PG6 52
#define PIN_PG7 53
#define PIN_PF6 54


#define NUM_DIGITAL_PINS               55
#define NUM_ANALOG_INPUTS              22
#define NUM_RESERVED_PINS              0
#define NUM_INTERNALLY_USED_PINS       0
#define NUM_I2C_PINS                   2 // (SDA / SCL)
#define NUM_SPI_PINS                   3 // (MISO / MOSI / SCK)
#define NUM_TOTAL_FREE_PINS            (NUM_DIGITAL_PINS)
#define NUM_TOTAL_PINS                 (NUM_DIGITAL_PINS)
#define ANALOG_INPUT_OFFSET            24 //Hopefully not used elsewhere!
#define LED_BUILTIN                    (PIN_PA7)
#define digitalPinToAnalogInput(p)     ((((p)>=PIN_PC7) && ((p)< PIN_PF6))? ((p)-PIN_PD0) : NOT_A_PIN)
#define digitalOrAnalogPinToDigital(p) (((p)<=NUM_DIGITAL_PINS)?(p):NOT_A_PIN)


#define EXTERNAL_NUM_INTERRUPTS     (56) //needs one extra - see WInterrupts; the low 3 bits are the bit-within-port, rest is port number.

#if defined(MILLIS_USE_TIMERB0)
#define digitalPinHasPWM(p)  ((((p) >= PIN_PB0) && ((p) <= PIN_PB5)) || (((p) >= PIN_PC0) && ((p) <= PIN_PC5)) || ((p) == PIN_PA4) || ((p) == PIN_PA5) ||\
  ((p) == PIN_PF5) || ((p) == PIN_PG3))
#elif defined(MILLIS_USE_TIMERB1)
#define digitalPinHasPWM(p)  ((((p) >= PIN_PB0) && ((p) <= PIN_PB5)) || (((p) >= PIN_PC0) && ((p) <= PIN_PC5)) || ((p) == PIN_PA4) || ((p) == PIN_PA5) ||\
  ((p) == PIN_PF4) || ((p) == PIN_PG3))
#elif defined(MILLIS_USE_TIMERB2)
#define digitalPinHasPWM(p)  ((((p) >= PIN_PB0) && ((p) <= PIN_PB5)) || (((p) >= PIN_PC0) && ((p) <= PIN_PC5)) || ((p) == PIN_PA4) || ((p) == PIN_PA5) ||\
  ((p) == PIN_PF4) || ((p) == PIN_PF5) || ((p) == PIN_PG3))
#elif defined(MILLIS_USE_TIMERB3)
#define digitalPinHasPWM(p)  ((((p) >= PIN_PB0) && ((p) <= PIN_PB5)) || (((p) >= PIN_PC0) && ((p) <= PIN_PC5)) || ((p) == PIN_PA4) || ((p) == PIN_PA5) ||\
  ((p) == PIN_PF4) || ((p) == PIN_PF5) || ((p) == PIN_PG3))
#else //no TCB's are used for millis
#define digitalPinHasPWM(p)  ((((p) >= PIN_PB0) && ((p) <= PIN_PB5)) || (((p) >= PIN_PC0) && ((p) <= PIN_PC5)) || ((p) == PIN_PA4) || ((p) == PIN_PA5) ||\
  ((p) == PIN_PF4) || ((p) == PIN_PF5) || ((p) == PIN_PG3))
#endif

// Timer pin swaps
#define TCA0_PINS PORTMUX_TCA0_PORTC_gc // TCA0 output on PC[0:5]
#define TCA1_PINS 0x00 // TCA0 output on PB[0:5] (no pinswap)
#define TCB0_PINS PORTMUX_TCB0_bm       // TCB0 output on PF4 instead of PA2 (default)
#define TCB1_PINS PORTMUX_TCB1_bm       // TCB1 output on PF5 instead of PA3 (default)
#define TCB2_PINS 0x00                  // TCB2 output on PC0 (default) instead of PB4 (either way not used)
#define TCB3_PINS 0x00                  // TCB3 output on PB5 (default) instead of PC1 (either way not used)
#define TCB4_PINS 0x00                  // TCB4 output on PG3 (default) instead of PC6
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
#define SPI_MUX                (PORTMUX_SPI0_DEFAULT_gc)
#define SPI_MUX_PINSWAP_1      (PORTMUX_SPI0_ALT1_gc)
#define SPI_MUX_PINSWAP_2      (PORTMUX_SPI0_ALT2_gc)
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
static const uint8_t SS   = PIN_SPI_SS;
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK  = PIN_SPI_SCK;

#define SPI1_MUX                (PORTMUX_SPI1_DEFAULT_gc)
#define SPI1_MUX_PINSWAP_1      (PORTMUX_SPI1_ALT1_gc)
#define SPI1_MUX_PINSWAP_2      (PORTMUX_SPI1_ALT2_gc)
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
static const uint8_t MOSI1 = PIN_SPI1_MOSI;
static const uint8_t MISO1 = PIN_SPI1_MISO;
static const uint8_t SCK1  = PIN_SPI1_SCK;
static const uint8_t SS1   = PIN_SPI1_SS;


// TWI 0
// No pinswap enabled by default
#define PIN_WIRE_SDA           (PIN_PA2)
#define PIN_WIRE_SCL           (PIN_PA3)
#define PIN_WIRE_SDA_PINSWAP_2 (PIN_PC2)
#define PIN_WIRE_SCL_PINSWAP_2 (PIN_PC3)
static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;
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
// No pinswap by default
#define HWSERIAL4                       &USART4
#define HWSERIAL4_DRE_VECTOR            USART4_DRE_vect
#define HWSERIAL4_DRE_VECTOR_NUM        USART4_DRE_vect_num
#define HWSERIAL4_RXC_VECTOR            USART4_RXC_vect
#define HWSERIAL4_MUX                   PORTMUX_USART4_DEFAULT_gc
#define HWSERIAL4_MUX_PINSWAP_1         PORTMUX_USART4_ALT1_gc
#define PIN_HWSERIAL4_TX                PIN_PE0
#define PIN_HWSERIAL4_RX                PIN_PE1
#define PIN_HWSERIAL4_TX_PINSWAP_1      PIN_PE4
#define PIN_HWSERIAL4_RX_PINSWAP_1      PIN_PE5

// USART 5
// No pinswap by default
#define HWSERIAL5                       &USART5
#define HWSERIAL5_DRE_VECTOR            USART5_DRE_vect
#define HWSERIAL5_DRE_VECTOR_NUM        USART5_DRE_vect_num
#define HWSERIAL5_RXC_VECTOR            USART5_RXC_vect
#define HWSERIAL5_MUX                   PORTMUX_USART5_DEFAULT_gc
#define HWSERIAL5_MUX_PINSWAP_1         PORTMUX_USART5_ALT1_gc
#define PIN_HWSERIAL5_TX                PIN_PG0
#define PIN_HWSERIAL5_RX                PIN_PG1
#define PIN_HWSERIAL5_TX_PINSWAP_1      PIN_PG4
#define PIN_HWSERIAL5_RX_PINSWAP_1      PIN_PG5

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


#ifdef ARDUINO_MAIN

const uint8_t digital_pin_to_port[] = {
  PA,  //  0 PA0
  PA,  //  1 PA1
  PA,  //  2 PA2
  PA,  //  3 PA3
  PA,  //  4 PA4
  PA,  //  5 PA5
  PA,  //  6 PA6
  PA,  //  7 PA7
  PB,  //  8 PB0
  PB,  //  9 PB1
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
};

/* Use this for accessing PINnCTRL register */
const uint8_t digital_pin_to_bit_position[] = {
  PIN0_bp,  //  0 PA0
  PIN1_bp,  //  1 PA1
  PIN2_bp,  //  2 PA2
  PIN3_bp,  //  3 PA3
  PIN4_bp,  //  4 PA4
  PIN5_bp,  //  5 PA5
  PIN6_bp,  //  6 PA6
  PIN7_bp,  //  7 PA7
  PIN0_bp,  //  8 PB0
  PIN1_bp,  //  9 PB1
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
};

/* Use this for accessing PINnCTRL register */
const uint8_t digital_pin_to_bit_mask[] = {
  PIN0_bm,  //  0 PA0
  PIN1_bm,  //  1 PA1
  PIN2_bm,  //  2 PA2
  PIN3_bm,  //  3 PA3
  PIN4_bm,  //  4 PA4
  PIN5_bm,  //  5 PA5
  PIN6_bm,  //  6 PA6
  PIN7_bm,  //  7 PA7
  PIN0_bm,  //  8 PB0
  PIN1_bm,  //  9 PB1
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
};

const uint8_t digital_pin_to_timer[] = {
  NOT_ON_TIMER, //  0
  NOT_ON_TIMER, //  1
  NOT_ON_TIMER, //  2
  NOT_ON_TIMER, //  3
  NOT_ON_TIMER, //  4 PA4/MOSI
  NOT_ON_TIMER, //  5 PA5/MISO
  TIMERD0,      //  6 PA6/SCK
  TIMERD0,      //  7 PA7/SS/CLKOUT
  TIMERA1,      //  8 PB0
  TIMERA1,      //  9 PB1
  TIMERA1,      //  10 PB2
  TIMERA1,      //  11 PB3
  TIMERA1,      //  12 PB4
  TIMERA1,      //  13 PB5
  NOT_ON_TIMER, //  14
  NOT_ON_TIMER, //  15
  TIMERA0,      //  16 PC0
  TIMERA0,      //  17 PC1
  TIMERA0,      //  18 PC2
  TIMERA0,      //  19 PC3
  TIMERA0,      //  20 PC4
  TIMERA0,      //  21 PC5
  NOT_ON_TIMER, //  22 PC6
  NOT_ON_TIMER, //  23 PC7
  NOT_ON_TIMER, //  24 PD0
  NOT_ON_TIMER, //  25
  NOT_ON_TIMER, //  26
  NOT_ON_TIMER, //  27
  NOT_ON_TIMER, //  28
  NOT_ON_TIMER, //  29
  DACOUT,       //  30 PD6
  NOT_ON_TIMER, //  31 PD7
  NOT_ON_TIMER, //  32 PE0
  NOT_ON_TIMER, //  33
  NOT_ON_TIMER, //  34
  NOT_ON_TIMER, //  35
  NOT_ON_TIMER, //  36
  NOT_ON_TIMER, //  37
  NOT_ON_TIMER, //  38
  NOT_ON_TIMER, //  39
  NOT_ON_TIMER, //  40 PF0 (TOSC1)
  NOT_ON_TIMER, //  41 PF1 (TOSC2)
  NOT_ON_TIMER, //  42
  NOT_ON_TIMER, //  43
  TIMERB0,      //  44 PF4
  TIMERB1,      //  45 PF5
  NOT_ON_TIMER, //  46 PF6 (RESET)
  NOT_ON_TIMER, //  47 PG0
  NOT_ON_TIMER, //  48
  TIMERB4,      //  49
  NOT_ON_TIMER, //  50
  NOT_ON_TIMER, //  51
  NOT_ON_TIMER, //  52
  NOT_ON_TIMER, //  53
  NOT_ON_TIMER, //  54 PG7
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
