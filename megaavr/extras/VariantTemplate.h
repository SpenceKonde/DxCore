/*  (C) Spence Konde 2021 open source (LGPL2.1 see LICENSE.md).

 ###  #     # ####      ####   ###      # ####  ####      #  #  ###
#   # #     # #   #     #   # #   #    #  #   # #   #     #  # #   #          #
#####  #   #  ####      #   # #####   #   #   # ####      ####  ###  ### ###     ###
#   #   # #   #  #      #   # #   #  #    #   # #   #        # #   #     #  # #  #  #
#   #    #    #   #     ####  #   # #     ####  ####         #  ###      ###  #  #  #
=====================================================     ----------     #
example of a variant definition file with notes for variant              #
creators who might be making their own

List the part numbers (if applicable) or what board this variant supports.


*/
#ifndef Pins_Arduino_h
#define Pins_Arduino_h
#include <avr/pgmspace.h>
#include "timers.h"

#define EXAMPLE_48PIN_PINOUT // if your pin numbering or general mapping has changed
// define something like this so people can detect and react programmatically.

// NOW FOR PIN NUMBERS

/* There are a set of rules that MUST be followed when choosing the pin numbers.
      When those rules are not observed, malfunctions will occuur
 * These are constrained for reasons not immediately obvious.
 * These parts have an internanal logical order for the pins.
   * This pattern is constant on all non-tinyAVR parts
   * Serial ports most certainly assume that for each mux options, the pins are adjacent and in order
      Tx -> Rx -> XCK -> XDIR.
 * Thus ALL VARIANTS MUST FOLLOW THE RULES in order to function without further (hard) modification.
 * The numbering shall begin at PA0, with PIN_P0 = 0.
    * PIN_A0 and PIN_A1 must be included in the numbering, even if the board has a crystal or clock.
 * Subsequent pins in port A are numbered in order.
 * After a port is finished, Look to the next letter of the alphabet.
    Is there a port associated with that letter which has one or more pins on it?
 * If there are one or more pins on that port available, mark it down with the next number, as if it were a pin.
    * a 28-pin DB DOES get a PIN_PD0, because it has PD1-PD7.
    * a 28-pin DB does NOT get a PIN_PB0 - there are no pins on PORTB in that pincount.
 * After the phantom pin, if any at the start of the port (PC0 and PD0 are the potential phantoms.
 * A pin that is not pin 0 within a part is skipped and it's number used for the next pin that is present.
 * There is some flexibility:
    * If UPDI pin cannot be set to act as IO, it should not have a declared number - it's not a pin as far as the part is concerned.
    * If Reset, but not UPDI can ber set to act as I/O, you may choose to skip numbering it, and give it the last number.
    * If Reset and UPDI can both be set as I/O, they should be numbered consecutively. It is the option of the implementor whether
        these two pins, PF6 and PF7 should be locasted at at the end of the pin numbers, or at the end of PORTF (this is only different on 64-pin parts. )
 * No pins shalt be arbitrarily renumbered to make the pin numbers "more like an Uno" or for any other purpose.
      This depraved and barbarous practice has no place in modern society. In the bad old days, when peripherals on classic AVRs were strewn
      haphazard across ports, it was justifiable. That is not the case here. The pinout makes sense, and generally demonstrates that at least
      some of the team was of sound mind during the design process. Again this is a contrast to classic AVRs.
 * Regarding those above mentioned pins that get numberd but can't be used, see the pin array section below.
/*

        /*##  ### #   #  ###
        #   #  #  ##  # #
        ####   #  # # #  ###
        #      #  #  ##     #
        #     ### #   #  #*/

#define PIN_PA0 (0)
#define PIN_PA1 (1)
#define PIN_PA2 (2)
#define PIN_PA3 (3)
(snip)
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
// #define NUM_RESERVED_PINS            0     // One might argue that PA0 and PA1 on a board that always uses a crystal count here.
// #define NUM_INTERNALLY_USED_PINS     0     // And if your board has pins that are hardwired to stuff and can't be otherwise used
//                                            // that's what these are.
// Autocalculated are :
// NUM_DIGITAL_PINS = PINS_COUNT - NUM_RESERVED_PINS
// TOTAL_FREE_OPINS = NUM_DIGITAL_PINS - NUM_INTERNALLY_USED_PINS
//
// Count of I2C and SPI pins will be defined as 2 and 3 but are not used in further calculations. What that is supposed to represent is ill-defined
// If you for some reason need to change any of this, define them here. Only ones not defined here get automatically set.
//
// I am on a crusade todrive from this world the scourge of acting like an analog pin is something other
// than a digital pin with ADC input caoability. Same goes for the I2C and SPI pins. That foolish analog pin
// naming causes so much confusion for new users
//
//  BUILTIN LEDS AND BUTTONS.
// Here we can override the default pin for the LED with a command line option or something opassed through boards.txt or platform.txt.
// These shall always be defined using PIN_Pxn notation.  A board (rather than generic chip) might not check if it was defined.
#if !defined(LED_BUILTIN)
  #define LED_BUILTIN                  PIN_PA7
#endif
/* Until the legacy attach interrupt has been completely obsoleted, leave this line in
 * EXTERNAL_NUM_INTERRUPTS: Count the number of ports on the part. The last port counts for the number of I/O pins in it.
 * All other ports count for 8.. */
#ifdef CORE_ATTACH_OLD
  #define EXTERNAL_NUM_INTERRUPTS        47
#endif

      /*   #  ###   ### ####   ###   ###
       ## ## #   # #    #   # #   # #
       # # # ##### #    ####  #   #  ###
       #   # #   # #    # #   #   #     #
       #   # #   #  ### #  #   ###   ##*/

/* THIS IS IMPORTANT - also often tricky */
/* These macros map between different pin-like-things. Any renumbering of pins or changes to mappings will impact these*/

/* An analog **channel** is the number corresponding to a pin that goes into the MUXPOS. Try to avoid ever working with these
 * because it is oh so easy to treat it like a pin, when it's not a pin. **NOR CAN CODE TELL THAT IT IS NOT A PIN**
 * An Analog Input or analog Channel Identifier is ADC_CH(channel number), or (channel number | 0x80). That way, the code can
 * tell that something is an analog input, not a pin. Most analog inputs are pins, but not all are.
 * Thankfully on the Dx and tinyAVR parts, there is enough similarity that these can be automatically defined in core_parts/core_devices
 * whidh are files holding information common to all or a large fraction of supported parts.
 */

#define digitalPinToAnalogInput(p)        (((p) > PIN_PC7 && (p) < PIN_PF0) ? ((p) - PIN_PD0) : ((p) < PIN_PF6 ? ((p) - 18) : NOT_A_PIN))
#define analogChannelToDigitalPin(p)      (((p) < 12) ? ((p) + PIN_PD0) : (((p) < 16 || p > 21) ? NOT_A_PIN : ((p) + PIN_PF0 - 16)))
#define analogInputToDigitalPin(p)                        analogChannelToDigitalPin((p) & 0x7F)
#define digitalOrAnalogPinToDigital(p)    (((p) & 0x80) ? analogChannelToDigitalPin((p) & 0x7f) : (((p)<=NUM_DIGITAL_PINS) ? (p) : NOT_A_PIN))
/* You need to have a macro to convert porrs to the pin number of the zero pin. */
#define portToPinZero(port)               (((port) < PF) ? (((port) * 8) - (((port) > 1) ? 2 : 0) : ((port) == PF ? PIN_PF0 : NOT_A_PIN))

// Timer pin swaps TCA mux is only the default on DxCore - if you change PORTMUX, the PWM will follow.
#define TCA0_PINS PORTMUX_TCA0_PORTC_gc     // TCA0 output on PD[0:5] by default, vbut just write the portmux to change it
#define TCA1_PINS 0x00                      // Same as TCA0; in this case only the PB and PG portmux options are available.
#define TCB0_PINS PORTMUX_TCB0_bm           // TCB0 output on PF4 instead of PA2 (default)
#define TCB1_PINS PORTMUX_TCB1_bm           // TCB1 output on PF5 instead of PA3 (default)
#define TCB2_PINS 0x00                      // TCB2 output on PC0 (default) instead of PB4
#define TCB3_PINS PORTMUX_TCB3_bm           // TCB3 output on PC1 instead of PB5 (default)
#define TCD0_PINS PORTMUX_TCD0_DEFAULT_gc   // Only default port option works!

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

/* Tell code where it should expect these waveform outputs would be pointed initially */
#define PIN_TCA0_WO0_INIT PIN_PC0
#define PIN_TCA1_WO0_INIT PIN_PB0
#define PIN_TCD0_WOA_INIT PIN_PA4


// USE_TIMERD0_PWM  defaults to 1 (on).
// If it is turned off, analogWrite and turnOffPWM will not work with TCD pins. This may be used to save flash, particularly on
// small flash parts where the type D timer will be taken over anyway. The savings can in excess of 0.5k

// You may define TIMERD0_CLOCK_SETTINGS and TIMERD_TOP_SETTING to configure these directly.
// * If TIMERD0_TOP_SETTING is 254 509, 1019, 2039, or 4079
//   * As long as USE_TIMERD0_PWM is not defined as 0, PWM will be enabled **RUNTIME DETECTION OF TCD TOP WILL BE DISABLED**
//   * If TIMERD0_CLOCK_SETTINGS is defined, it will be used, otherwise the default for that clock speed will be used.
// * If TIMERD0_TOP_SETTING is any other value, analogWrite-mediated PWM will not be available. Combining with USE_TIMERD0_PWM
//    is a compile error.
// * If TIMERD0_TOP_SETTING is not defined, and TIMERD0_CLOCK_SETTINGS is, runtime autodetect of TCD0 TOP will be used however:
//   * TCD0 will be initially set to TOP of 254/
//   * All supported TOP values will be tested for, regardless of system clock speed.
// * If USE_TIMERD0_PWM is set to 0, no configuration of TCD0 will be performed by the core. A future update that allowed it to
//    be used for other purposes would change that, of course.
//
// NO_GLITCH_TIMERD0 leaves pins connected to TCD0 until digitalWrite is called on them.
// This is desirable to get more responsive TCD pwm and avoid short glitches when switching between 0 or 255 and  other duty
// cycles. Costs some flash.
#define NO_GLITCH_TIMERD0

#define digitalPinHasPWM(p)               (digitalPinHasPWMTCB(p) || ((p) >= PIN_PA4 && (p) <= PIN_PC5))
/* The above macro is deprecated, and is only for compatibility, when digitalPinHasPWMNow() is not viable.


       /*###   ###  ####  ##### #   # #   # #   #
        #   # #   # #   #   #   ## ## #   #  # #
        ####  #   # ####    #   # # # #   #   #
        #     #   # #  #    #   #   # #   #  # #
        #      ###  #   #   #   #   #  ###  #   */

/* This section contains an enumeration of the pins that each peripheral can be set to use via portmux.
 * SPI pins must all be listed here, except the SS pin, which we recommend including. The includes SPI.h library, however,
 * does not support acting as SPI slave, and disables SS, so there is nothing special about that pin.
 * I2C pins must all be listed here **except** for dual mode ones, those are handled internally automatically when that is enabled.
 * All SPI portmux options available must be defined.
 * I2C portmux options should not be defined.
 *
 * Hardware serial port mux count is no longer defined here. List port mux options that have available pins,
 * If a mux is to be considered valid, ALL pins applying to it must be defined.
 *
 *
 * VARIANTS SHOULD NOT INCLUDE ANY OFF THESE SORT OF DEFINES:

#define HWSERIAL0                       &USART0
#define HWSERIAL0_DRE_VECTOR            USART0_DRE_vect
#define HWSERIAL0_DRE_VECTOR_NUM        USART0_DRE_vect_num
#define HWSERIAL0_RXC_VECTOR            USART0_RXC_vect
#define HWSERIAL0_MUX_COUNT             5





 */

#define SPI_INTERFACES_COUNT   1 /* Has to be 1 never 2, because the code from other authors and in widespread use will expect the second SPI interface
                                  to be named SPI1, which will collide with the struct from io headers.*/
#define NUM_HWSERIAL_PORTS     5

// List out everty available mapping for the SPI pins.
// All pins should be included. They should be omitted only if
// the pin does not physically exist on the part
// (or, preferably, defined as NOT_A_PIN)
// However if pins are forgotten, they are autodefined to NOT_A_PIN
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


/* Yes, pinswap1 is the same as default. You must still include it or people will not be allowed to swap to it.
 * The slave pins for dual mode are hadled internally to wire. */

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
#define PIN_WIRE1_SDA_PINSWAP_1 PIN_PF2
#define PIN_WIRE1_SCL_PINSWAP_1 PIN_PF3
#define PIN_WIRE1_SDA_PINSWAP_2 PIN_PB2
#define PIN_WIRE1_SCL_PINSWAP_2 PIN_PB3

/* All mux names must be defined in the form shown below. */
/* That includes NONE. It does not, however, include a
 mux option that is not usable because none of the pins are there */

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


/*     ###  #   #  ###  #     ###   ###      ####  ### #   #  ###
      #   # ##  # #   # #    #   # #         #   #  #  ##  # #
      ##### # # # ##### #    #   # #  ##     ####   #  # # #  ###
      #   # #  ## #   # #    #   # #   #     #      #  #  ##     #
      #   # #   # #   # ####  ###   ###      #     ### #   #  #*/
/* Definitions:
 PIN_An is to be the digital pin that An refers to.
 An is a representation of an analog input channel or a so-called "analog pin"
 This artificial classification as analog or dgital pins continues to cause
 confusion and imprecise language throughout the community. We did not drink
 that kool-aid. analogRead is capable of understanding pin numbers with the
 same representation as digital I/O functions do.
 AINn is NOT for external use, is the channel number with the high bit set, which
 flags it as something that functions can pass around like a pin while knowinng
 that it's actually a channel and that analogChannelToDigitalPin() is needed
 to get the digital pin back. Users should generally not use them.
 AINn = ADC_CH(n) = 0x80 | n = digitalPinToAnalogChanneel(An)                  */
#define PIN_A0   PIN_PD0
(snip)
#define PIN_A10  PIN_PE2
#define PIN_A11  PIN_PE3    //skip analog pins that don't exist
#define PIN_A16  PIN_PF0
#define PIN_A17  PIN_PF1
(snip)
#define PIN_A21  PIN_PF5
static const uint8_t A0  = PIN_A0;
(snip)
static const uint8_t A10 = PIN_A10;
static const uint8_t A11 = PIN_A11;
static const uint8_t A16 = PIN_A16;
static const uint8_t A17 = PIN_A17;
(snip)
static const uint8_t A21 = PIN_A21;
/* Internal and library use only - see 64-pin standard version for details.*/
#define  AIN0  ADC_CH(0)
(snip)
#define AIN10 ADC_CH(10)
#define AIN11 ADC_CH(11)
#define AIN16 ADC_CH(16)
#define AIN17 ADC_CH(17)
(snip)
#define AIN21 ADC_CH(21)

// *INDENT-OFF*
/*    ####  ### #   #      ###  ####  ####   ###  #   #  ###
      #   #  #  ##  #     #   # #   # #   # #   #  # #  #
      ####   #  # # #     ##### ####  ####  #####   #    ###
      #      #  #  ##     #   # #  #  #  #  #   #   #       #
      #     ### #   #     #   # #   # #   # #   #   #    #*/


// All pins with numbers and PIN_Pxn defines should be here/


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
  };
  // But pins that are not usable at all should be marked NOT_A_PIN here
  const uint8_t digital_pin_to_bit_position[] = {
    #if CLOCK_SOURCE == 0 // PA0 used for external clock and crystal.
      PIN0_bp,            // PA0
    #else
      NOT_A_PIN,
    #endif
    #if CLOCK_SOURCE == 1 // PA1 also used for crystal
      NOT_A_PIN,  //   1 PA1
    #else
      // PA1 used for external crystal.
      PIN1_bp,
    #endif
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
    PIN6_bp, // 40 PF6 RESET
  //PIN7_bm, // 55 PF7 (UPDI)
  };

  // and down here
  const uint8_t digital_pin_to_bit_mask[] = {
    #if CLOCK_SOURCE == 0 // PA0 used for external clock and crystal.
      PIN0_bm,            // PA0
    #else
      NOT_A_PIN,
    #endif
    #if CLOCK_SOURCE == 1 // PA1 also used for crystal
      NOT_A_PIN,  //   1 PA1
    #else
      // PA1 used for external crystal.
      PIN1_bm,
    #endif
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
    PIN6_bm, // 40 PF6 RESET
  //PIN7_bm, // 55 PF7 (UPDI)
  };

  /* On DxCore, don't put TCA PWM pins in here. They are found automatically. */
  /* When the errata is fixed, the same will be done for TCD */
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
    NOT_ON_TIMER,  // 40 PF6 RESET
  //NOT_ON_TIMER, //  55 PF7 (UPDI)
  };

#endif

#endif
