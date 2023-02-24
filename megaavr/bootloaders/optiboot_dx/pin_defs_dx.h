/*
 * pin_defs.h
 * optiboot helper defining the default pin assignments (LED, SOFT_UART)
 * for the various chips that are supported.  This also has some ugly macros
 * for selecting among various UARTs and LED possibilities using command-line
 * defines like "UART=2 LED=B5"
 *
 * Copyright 2013-2015 by Bill Westfield.
 * Copyright 2010 by Peter Knight.
 * This software is licensed under version 2 of the Gnu Public Licence.
 * See optiboot.c for details.
 */


/*
 * ------------------------------------------------------------------------
 * A bunch of macros to enable the LED to be specified as "B5" for bit 5
 * of port B, and similar.
 * We define symbols for all the legal combination of port/bit on a chip,
 * and do pre-processor tests to see if there's a match.  This ends up
 * being very verbose, but it is pretty easy to generate semi-automatically.
 * (We wouldn't need this if the preprocessor could do string compares.)
 */

// Symbols for each PortA bit.
#define A0 0x100
#define A1 0x101
#define A2 0x102
#define A3 0x103
#define A4 0x104
#define A5 0x105
#define A6 0x106
#define A7 0x107
// If there is no PORTA on this chip, don't allow these to be used
//   (and indicate the error by redefining LED)
#if !defined(PORTA)
  #if LED >= A0 && LED <= A7
    #undef LED
    #define LED -1
  #endif
#endif

#define B0 0x200
#define B1 0x201
#define B2 0x202
#define B3 0x203
#define B4 0x204
#define B5 0x205
#define B6 0x206
#define B7 0x207
#if !defined(PORTB)
  #if LED >= B0 && LED <= B7
    #undef LED
    #define LED -1
  #endif
#endif

#define C0 0x300
#define C1 0x301
#define C2 0x302
#define C3 0x303
#define C4 0x304
#define C5 0x305
#define C6 0x306
#define C7 0x307
#if !(defined(PORTC))
  #if LED >= C0 && LED <= C7
    #undef LED
    #define LED -1
  #endif
#endif

#define D0 0x400
#define D1 0x401
#define D2 0x402
#define D3 0x403
#define D4 0x404
#define D5 0x405
#define D6 0x406
#define D7 0x407
#if !(defined(PORTD))
  #if LED >= D0 && LED <= D7
    #undef LED
    #define LED -1
  #endif
#endif

#define E0 0x500
#define E1 0x501
#define E2 0x502
#define E3 0x503
#define E4 0x504
#define E5 0x505
#define E6 0x506
#define E7 0x507
#if !(defined(PORTE))
  #if LED >= E0 && LED <= E7
    #undef LED
    #define LED -1
  #endif
#endif

#define F0 0x600
#define F1 0x601
#define F2 0x602
#define F3 0x603
#define F4 0x604
#define F5 0x605
#define F6 0x606
#define F7 0x607
#if !(defined(PORTF))
  #if LED >= F0 && LED <= F7
    #undef LED
    #define LED -1
  #endif
#endif

#define G0 0x700
#define G1 0x701
#define G2 0x702
#define G3 0x703
#define G4 0x704
#define G5 0x705
#define G6 0x706
#define G7 0x707
#if !defined(PORTG)
  #if LED >= G0 && LED <= G7
    #undef LED
    #define LED -1
  #endif
#endif


/*
 * A statement like "#if LED == B0" will evaluation (in the preprocessor)
 * to #if C0 == B0, and then to #if 0x301 == 0x201
 */
#if LED == A0
  #define LED_NAME "A0"
  #undef LED
  #define LED_PORT    VPORTA
  #define LED     (1 << PORT0)
#elif LED == A1
  #define LED_NAME "A1"
  #undef LED
  #define LED_PORT    VPORTA
  #define LED     (1 << PORT1)
#elif LED == A2
  #define LED_NAME "A2"
  #undef LED
  #define LED_PORT    VPORTA
  #define LED     (1 << PORT2)
#elif LED == A3
  #define LED_NAME "A3"
  #undef LED
  #define LED_PORT    VPORTA
  #define LED     (1 << PORT3)
#elif LED == A4
  #define LED_NAME "A4"
  #undef LED
  #define LED_PORT    VPORTA
  #define LED     (1 << PORT4)
#elif LED == A5
  #define LED_NAME "A5"
  #undef LED
  #define LED_PORT    VPORTA
  #define LED     (1 << PORT5)
#elif LED == A6
  #define LED_NAME "A6"
  #undef LED
  #define LED_PORT    VPORTA
  #define LED     (1 << PORT6)
#elif LED == A7
  #define LED_NAME "A7"
  #undef LED
  #define LED_PORT    VPORTA
  #define LED     (1 << PORT7)

#elif LED == B0
  #define LED_NAME "B0"
  #undef LED
  #define LED_PORT    VPORTB
  #define LED     (1 << PORT0)
#elif LED == B1
  #define LED_NAME "B1"
  #undef LED
  #define LED_PORT    VPORTB
  #define LED     (1 << PORT1)
#elif LED == B2
  #define LED_NAME "B2"
  #undef LED
  #define LED_PORT    VPORTB
  #define LED     (1 << PORT2)
#elif LED == B3
  #define LED_NAME "B3"
  #undef LED
  #define LED_PORT    VPORTB
  #define LED     (1 << PORT3)
#elif LED == B4
  #define LED_NAME "B4"
  #undef LED
  #define LED_PORT    VPORTB
  #define LED     (1 << PORT4)
#elif LED == B5
  #define LED_NAME "B5"
  #undef LED
  #define LED_PORT    VPORTB
  #define LED     (1 << PORT5)
#elif LED == B6
  #define LED_NAME "B6"
  #undef LED
  #define LED_PORT    VPORTB
  #define LED     (1 << PORT6)
#elif LED == B7
  #define LED_NAME "B7"
  #undef LED
  #define LED_PORT    VPORTB
  #define LED     (1 << PORT7)

#elif LED == C0
  #define LED_NAME "C0"
  #undef LED
  #define LED_PORT    VPORTC
  #define LED     (1 << PORT0)
#elif LED == C1
  #define LED_NAME "C1"
  #undef LED
  #define LED_PORT    VPORTC
  #define LED     (1 << PORT1)
#elif LED == C2
  #define LED_NAME "C2"
  #undef LED
  #define LED_PORT    VPORTC
  #define LED     (1 << PORT2)
#elif LED == C3
  #define LED_NAME "C3"
  #undef LED
  #define LED_PORT    VPORTC
  #define LED     (1 << PORT3)
#elif LED == C4
  #define LED_NAME "C4"
  #undef LED
  #define LED_PORT    VPORTC
  #define LED     (1 << PORT4)
#elif LED == C5
  #define LED_NAME "C5"
  #undef LED
  #define LED_PORT    VPORTC
  #define LED     (1 << PORT5)
#elif LED == C6
  #define LED_NAME "C6"
  #undef LED
  #define LED_PORT    VPORTC
  #define LED     (1 << PORT6)
#elif LED == C7
  #define LED_NAME "C7"
  #undef LED
  #define LED_PORT    VPORTC
  #define LED     (1 << PORT7)

#elif LED == D0
  #define LED_NAME "D0"
  #undef LED
  #define LED_PORT    VPORTD
  #define LED     (1 << PORT0)
#elif LED == D1
  #define LED_NAME "D1"
  #undef LED
  #define LED_PORT    VPORTD
  #define LED     (1 << PORT1)
#elif LED == D2
  #define LED_NAME "D2"
  #undef LED
  #define LED_PORT    VPORTD
  #define LED     (1 << PORT2)
#elif LED == D3
  #define LED_NAME "D3"
  #undef LED
  #define LED_PORT    VPORTD
  #define LED     (1 << PORT3)
#elif LED == D4
  #define LED_NAME "D4"
  #undef LED
  #define LED_PORT    VPORTD
  #define LED     (1 << PORT4)
#elif LED == D5
  #define LED_NAME "D5"
  #undef LED
  #define LED_PORT    VPORTD
  #define LED     (1 << PORT5)
#elif LED == D6
  #define LED_NAME "D6"
  #undef LED
  #define LED_PORT    VPORTD
  #define LED     (1 << PORT6)
#elif LED == D7
  #define LED_NAME "D7"
  #undef LED
  #define LED_PORT    VPORTD
  #define LED     (1 << PORT7)

#elif LED == E0
  #define LED_NAME "E0"
  #undef LED
  #define LED_PORT    VPORTE
  #define LED     (1 << PORT0)
#elif LED == E1
  #define LED_NAME "E1"
  #undef LED
  #define LED_PORT    VPORTE
  #define LED     (1 << PORT1)
#elif LED == E2
  #define LED_NAME "E2"
  #undef LED
  #define LED_PORT    VPORTE
  #define LED     (1 << PORT2)
#elif LED == E3
  #define LED_NAME "E3"
  #undef LED
  #define LED_PORT    VPORTE
  #define LED     (1 << PORT3)
#elif LED == E4
  #define LED_NAME "E4"
  #undef LED
  #define LED_PORT    VPORTE
  #define LED     (1 << PORT4)
#elif LED == E5
  #define LED_NAME "E5"
  #undef LED
  #define LED_PORT    VPORTE
  #define LED     (1 << PORT5)
#elif LED == E6
  #define LED_NAME "E6"
  #undef LED
  #define LED_PORT    VPORTE
  #define LED     (1 << PORT6)
#elif LED == E7
  #define LED_NAME "E7"
  #undef LED
  #define LED_PORT    VPORTE
  #define LED     (1 << PORT7)

#elif LED == F0
  #define LED_NAME "F0"
  #undef LED
  #define LED_PORT    VPORTF
  #define LED     (1 << PORT0)
#elif LED == F1
  #define LED_NAME "F1"
  #undef LED
  #define LED_PORT    VPORTF
  #define LED     (1 << PORT1)
#elif LED == F2
  #define LED_NAME "F2"
  #undef LED
  #define LED_PORT    VPORTF
  #define LED     (1 << PORT2)
#elif LED == F3
  #define LED_NAME "F3"
  #undef LED
  #define LED_PORT    VPORTF
  #define LED     (1 << PORT3)
#elif LED == F4
  #define LED_NAME "F4"
  #undef LED
  #define LED_PORT    VPORTF
  #define LED     (1 << PORT4)
#elif LED == F5
  #define LED_NAME "F5"
  #undef LED
  #define LED_PORT    VPORTF
  #define LED     (1 << PORT5)
#elif LED == F6
  #define LED_NAME "F6"
  #undef LED
  #define LED_PORT    VPORTF
  #define LED     (1 << PORT6)
#elif LED == F7
  #define LED_NAME "F7"
  #undef LED
  #define LED_PORT    VPORTF
  #define LED     (1 << PORT7)

#elif LED == G0
  #define LED_NAME "G0"
  #undef LED
  #define LED_PORT    VPORTG
  #define LED     (1 << PORT0)
#elif LED == G1
  #define LED_NAME "G1"
  #undef LED
  #define LED_PORT    VPORTG
  #define LED     (1 << PORT1)
#elif LED == G2
  #define LED_NAME "G2"
  #undef LED
  #define LED_PORT    VPORTG
  #define LED     (1 << PORT2)
#elif LED == G3
  #define LED_NAME "G3"
  #undef LED
  #define LED_PORT    VPORTG
  #define LED     (1 << PORT3)
#elif LED == G4
  #define LED_NAME "G4"
  #undef LED
  #define LED_PORT    VPORTG
  #define LED     (1 << PORT4)
#elif LED == G5
  #define LED_NAME "G5"
  #undef LED
  #define LED_PORT    VPORTG
  #define LED     (1 << PORT5)
#elif LED == G6
  #define LED_NAME "G6"
  #undef LED
  #define LED_PORT    VPORTG
  #define LED     (1 << PORT6)
#elif LED == G7
  #define LED_NAME "G7"
  #undef LED
  #define LED_PORT    VPORTG
  #define LED     (1 << PORT7)

  // PORTs after G will require a Microchip do something different about the VPORT registers, because that's it for the I/O space!

#else
  // Stop compilation right away, so we don't get more errors.
  #if LED == -1
    #error Unrecognized LED name.  Should be like "B5"
    // Stop compilation right away, so we don't get more errors.
    #pragma GCC diagnostic warning "-Wfatal-errors"
    #error Nonexistent LED PORT.  Check datasheet.
  #endif
  #pragma GCC diagnostic warning "-Wfatal-errors"
  #error Unrecognized LED name.  Should be like "B5"
#endif


/*
 * Handle devices with up to 4 uarts.  Rather inelegantly.
 */
#define USART_ALTPMUX 1

// *INDENT-OFF* - astyle doesn't complain about the last one, but doesn't like this one.
#if \
  defined(__AVR_ATmega4809__) || defined(__AVR_ATmega4808__) || \
  defined(__AVR_ATmega3209__) || defined(__AVR_ATmega3208__) || \
  defined(__AVR_ATmega1609__) || defined(__AVR_ATmega1608__) || \
  defined(__AVR_ATmega809__)  || defined(__AVR_ATmega808__)  || \
  defined(__AVR_AVR128DA64__) || defined(__AVR_AVR64DA48__)  || \
  defined(__AVR_AVR128DB64__) || defined(__AVR_AVR64DB48__)  || \
  defined(__AVR_AVR124DA48__) || defined(__AVR_AVR32DA48__)  || \
  defined(__AVR_AVR128DB48__) || defined(__AVR_AVR32DB48__)  || \
  defined(__AVR_AVR64DB64__)  || defined(__AVR_AVR64DA64__)  || \
  defined(__AVR_AVR128DA32__) || defined(__AVR_AVR64DA32__)  || \
  defined(__AVR_AVR128DB32__) || defined(__AVR_AVR64DB32__)  || \
  defined(__AVR_AVR128DA28__) || defined(__AVR_AVR64DA28__)  || \
  defined(__AVR_AVR128DB28__) || defined(__AVR_AVR64DB28__)

  #define __AVR_DA__
  #define __AVR_DB__
  #if (UARTTX == A0)
    #define UART_NAME "A0"
    #ifndef USART0
      #error Pin on USART0, but no USART0 exists
    #endif
    #define MYUART USART0
    #define MYUART_RXPINCTRL (PORTA.PIN1CTRL)
    #define MYUART_TXPORT VPORTA
    #define MYUART_TXPIN (1 << PORT0)
    #define MYUART_PMUX_VAL (0)
    #define MYPMUX_REG (PORTMUX.USARTROUTEA)
  #elif (UARTTX == A4)
    #define UART_NAME "A4"
    #ifndef USART0
      #error Pin on USART0, but no USART0 exists
    #endif
    #define MYUART USART0
    #define MYUART_RXPINCTRL (PORTA.PIN5CTRL)
    #define MYUART_TXPORT VPORTA
    #define MYUART_TXPIN (1 << PORT4)
    #define MYUART_PMUX_VAL (0x01)
    #define MYPMUX_REG (PORTMUX.USARTROUTEA)
  #elif (UARTTX == B0)
    #define UART_NAME "B0"
    #ifndef USART3
      #error Pin on USART3, but no USART3 exists
    #endif
    #define MYUART USART3
    #define MYUART_RXPINCTRL (PORTB.PIN1CTRL)
    #define MYUART_TXPORT VPORTB
    #define MYUART_TXPIN (1 << PORT0)
    #define MYUART_PMUX_VAL (0)
    #define MYPMUX_REG (PORTMUX.USARTROUTEA)
  #elif (UARTTX == B4)
    #define UART_NAME "B4"
    #ifndef USART4
      #error Pin on USART3, but no USART3 exists
    #endif
    #define MYUART USART3
    #define MYUART_RXPINCTRL (PORTB.PIN5CTRL)
    #define MYUART_TXPORT VPORTB
    #define MYUART_TXPIN (1 << PORT4)
    #define MYUART_PMUX_VAL (0x40)
    #define MYPMUX_REG (PORTMUX.USARTROUTEA)
  #elif (UARTTX == C0)
    #define UART_NAME "C0"
    #ifndef USART1
      #error Pin on USART3, but no USART1 exists
    #endif
    #define MYUART USART1
    #define MYUART_RXPINCTRL (PORTC.PIN1CTRL)
    #define MYUART_TXPORT VPORTC
    #define MYUART_TXPIN (1 << PORT0)
    #define MYUART_PMUX_VAL (0)
    #define MYPMUX_REG (PORTMUX.USARTROUTEA)
  #elif (UARTTX == C4)
    #define UART_NAME "C4"
    #ifndef USART1
      #error Pin on USART1, but no USART1 exists
    #endif
    #define MYUART USART1
    #define MYUART_RXPINCTRL (PORTC.PIN5CTRL)
    #define MYUART_TXPORT VPORTC
    #define MYUART_TXPIN (1 << PORT4)
    #define MYUART_PMUX_VAL (0x04)
    #define MYPMUX_REG (PORTMUX.USARTROUTEA)
  #elif (UARTTX == F0)
    #define UART_NAME "F0"
    #ifndef USART2
      #error Pin on USART2, but no USART2 exists
    #endif
    #define MYUART USART2
    #define MYUART_RXPINCTRL (PORTF.PIN1CTRL)
    #define MYUART_TXPORT VPORTF
    #define MYUART_TXPIN (1 << PORT0)
    #define MYUART_PMUX_VAL (0)
    #define MYPMUX_REG (PORTMUX.USARTROUTEA)
  #elif (UARTTX == F4)
    #define UART_NAME "F4"
    #ifndef USART2
      #error Pin on USART2, but no USART2 exists
    #endif
    #define MYUART USART2
    #define MYUART_RXPINCTRL (PORTF.PIN5CTRL)
    #define MYUART_TXPORT VPORTF
    #define MYUART_TXPIN (1 << PORT4)
    #define MYUART_PMUX_VAL (0x10)
    #define MYPMUX_REG (PORTMUX.USARTROUTEA)
  #elif (UARTTX == E0)
    #define UART_NAME "E0"
    #ifndef USART4
      #error Pin on USART4, but no USART4 exists
    #endif
    #define MYUART USART4
    #define MYUART_RXPINCTRL (PORTE.PIN5CTRL)
    #define MYUART_TXPORT VPORTE
    #define MYUART_TXPIN (1 << PORT0)
    #define MYUART_PMUX_VAL (0)
    #define MYPMUX_REG (PORTMUX.USARTROUTEB)
  #elif (UARTTX == E4)
    #define UART_NAME "E4"
    #ifndef USART4
      #error Pin on USART4, but no USART4 exists
    #endif
    #define MYUART USART4
    #define MYUART_RXPINCTRL (PORTE.PIN5CTRL)
    #define MYUART_TXPORT VPORTE
    #define MYUART_TXPIN (1 << PORT4)
    #define MYUART_PMUX_VAL (0x01)
    #define MYPMUX_REG (PORTMUX.USARTROUTEB)
  #elif (UARTTX == G0)
    #define UART_NAME "G0"
    #ifndef USART5
      #error Pin on USART5, but no USART5 exists
    #endif
    #define MYUART USART5
    #define MYUART_RXPINCTRL (PORTG.PIN1CTRL)
    #define MYUART_TXPORT VPORTG
    #define MYUART_TXPIN (1 << PORT0)
    #define MYUART_PMUX_VAL (0)
    #define MYPMUX_REG (PORTMUX.USARTROUTEB)
  #elif (UARTTX == G4)
    #define UART_NAME "G4"
    #ifndef USART2
      #error Pin on USART5, but no USART5 exists
    #endif
    #define MYUART USART5
    #define MYUART_RXPINCTRL (PORTG.PIN5CTRL)
    #define MYUART_TXPORT VPORTG
    #define MYUART_TXPIN (1 << PORT4)
    #define MYUART_PMUX_VAL (0x04)
    #define MYPMUX_REG (PORTMUX.USARTROUTEB)
  #endif
#endif
/* AVR DD-series has only 2 USARTs, but they've got more mux options
 * to compensate for the small number of pins. More surprisingly, the
 * expanded mux options appear to be getting carried over to more
 * pin-rich devices; they are featured on the AVR-EA product brief too.
 * However, that's all - they didn't expand the portmux for anything
 * that didn't get the treatment on the DD. They do the same thing
 * with the SPI and I2C ports too. And AC0. In fact, the only pin
 * option that was only feaured on the DD was the ability to move
 * the watch crystal to PA0/PA1 (same as HF XTAL) for parts that
 * don't have PF0, PF1.
 * They had to reshape the PORTMUX registers, but it turns out
 * that this only changes matters in the case of USART1 in ALT2.
 * Otherwise, we set the whole register to zero and are done.
 * There aren't any parts with an alt1 for USART1.
 * And of coure, for USART0, as there is no alt 2 for any earlier parts
 * that and alt3 were added.
 * DA+DB share files and have 66 binaries for 22 parts
 * If DD can take same binaries for 32 and 16 (I think it can)
 * the that means 2 (flash sizes) * 8 (usart options) *2 (for the
 * 1 sec vs 8 sec timing), for 32 new builds to add 12 parts. But
 * if not, it'll be 48 builds.
 * This brings the total to 98 or 114 binaries for 34 parts.
 * megaTinyCore needs just 22 binaries for all 37 parts!
 * 0/1-series account for 9, 2-series needs another 9
 * and the remaining 4 are just for microchip boards and all they
 * do is move the LED to match the official demo boards.
 * So tinyAVR has a ratio of 18:37 = 0.48, vs our 98:34 = 2.88
 * It could be worse though...
 * ATTinyCore needs 169 hex files for the 14 parts that we
 * provide a bootloader for a horrifying 169:14 = 12.07 ratio!
 * The fact that the clock is set at runtime on modern parts is
 * a huge build saver - ATTinyCore has up to 10 bootloaders for
 * a part with a single serial port (not even - it's a software
 * UART), in order to support 3 internal speeds... and 7 crystals
 * including such common and popular frequencies as 14.4756 MHz
 * and 11.0592 MHz. Thank God, or more properly, whoever designed
 * that at Microchip, for the fractional baud rate generator!
 */
#if \
  defined(__AVR_AVR64DD32__) || defined(__AVR_AVR64DD28__) || \
  defined(__AVR_AVR32DD32__) || defined(__AVR_AVR32DD28__) || \
  defined(__AVR_AVR16DD32__) || defined(__AVR_AVR16DD28__)
  #define __AVR_DD__
  #if (UARTTX == A0)
    #define UART_NAME "A0"
    #ifndef USART0
      #error Pin on USART0, but no USART0 exists
    #endif
    #define MYUART USART0
    #define MYUART_RXPINCTRL (PORTA.PIN1CTRL)
    #define MYUART_TXPORT VPORTA
    #define MYUART_TXPIN (1 << PORT0)
    #define MYUART_PMUX_VAL (0)
    #define MYPMUX_REG (PORTMUX.USARTROUTEA)
  #elif (UARTTX == A4)
    #define UART_NAME "A4"
    #ifndef USART0
      #error Pin on USART0, but no USART0 exists
    #endif
    #define MYUART USART0
    #define MYUART_RXPINCTRL (PORTA.PIN5CTRL)
    #define MYUART_TXPORT VPORTA
    #define MYUART_TXPIN (1 << PORT4)
    #define MYUART_PMUX_VAL (1)
    #define MYPMUX_REG (PORTMUX.USARTROUTEA)
  #elif (UARTTX == A2)
    #define UART_NAME "A2"
    #ifndef USART0
      #error Pin on USART0, but no USART0 exists
    #endif
    #define MYUART USART0
    #define MYUART_RXPINCTRL (PORTA.PIN3CTRL)
    #define MYUART_TXPORT VPORTA
    #define MYUART_TXPIN (1 << PORT2)
    #define MYUART_PMUX_VAL (2)
    #define MYPMUX_REG (PORTMUX.USARTROUTEA)
  #elif (UARTTX == D4)
    #define UART_NAME "D4"
    #ifndef USART0
      #error Pin on USART0, but no USART0 exists
    #endif
    #define MYUART USART0
    #define MYUART_RXPINCTRL (PORTD.PIN5CTRL)
    #define MYUART_TXPORT VPORTD
    #define MYUART_TXPIN (1 << PORT4)
    #define MYUART_PMUX_VAL (3)
    #define MYPMUX_REG (PORTMUX.USARTROUTEA)
  #elif (UARTTX == C1)
    #define UART_NAME "C1"
    #ifndef USART0
      #error Pin on USART0, but no USART0 exists
    #endif
    #define MYUART USART0
    #define MYUART_RXPINCTRL (PORTC.PIN2CTRL)
    #define MYUART_TXPORT VPORTC
    #define MYUART_TXPIN (1 << PORT1)
    #define MYUART_PMUX_VAL (4)
    #define MYPMUX_REG (PORTMUX.USARTROUTEA)

  #elif (UARTTX == C0)
    #define UART_NAME "C0"
    #ifndef USART1
      #error Pin on USART1, but no USART1 exists
    #endif
    #define MYUART USART1
    #define MYUART_RXPINCTRL (PORTC.PIN1CTRL)
    #define MYUART_TXPORT VPORTC
    #define MYUART_TXPIN (1 << PORT0)
    #define MYUART_PMUX_VAL (0)
    #define MYPMUX_REG (PORTMUX.USARTROUTEA)
  #elif (UARTTX == D6)
    #define UART_NAME "D6"
    #ifndef USART1
      #error Pin on USART1, but no USART1 exists
    #endif
    #define MYUART USART1
    #define MYUART_RXPINCTRL (PORTD.PIN7CTRL)
    #define MYUART_TXPORT VPORTD
    #define MYUART_TXPIN (1 << PORT6)
    #define MYUART_PMUX_VAL (0x04)
    #define MYPMUX_REG (PORTMUX.USARTROUTEA)
  #endif
#endif
#if defined(__AVR_AVR64DD20__) || defined(__AVR_AVR32DD20__) || defined(__AVR_AVR16DD20__)
  #define __AVR_DD__
  #if (UARTTX == A0)
    #define UART_NAME "A0"
    #ifndef USART0
      #error Pin on USART0, but no USART0 exists
    #endif
    #define MYUART USART0
    #define MYUART_TXPORT VPORTA
    #define MYUART_TXPIN (1 << PORT0)
    #define MYUART_PMUX_VAL (0)
    #define MYPMUX_REG (PORTMUX.USARTROUTEA)
  #elif (UARTTX == A4)
    #define UART_NAME "A4"
    #ifndef USART0
      #error Pin on USART0, but no USART0 exists
    #endif
    #define MYUART USART0
    #define MYUART_TXPORT VPORTA
    #define MYUART_TXPIN (1 << PORT4)
    #define MYUART_PMUX_VAL (1)
    #define MYPMUX_REG (PORTMUX.USARTROUTEA)
  #elif (UARTTX == A2)
    #define UART_NAME "A2"
    #ifndef USART0
      #error Pin on USART0, but no USART0 exists
    #endif
    #define MYUART USART0
    #define MYUART_TXPORT VPORTA
    #define MYUART_TXPIN (1 << PORT2)
    #define MYUART_PMUX_VAL (2)
    #define MYPMUX_REG (PORTMUX.USARTROUTEA)
  #elif (UARTTX == D4)
    #define UART_NAME "D4"
    #ifndef USART0
      #error Pin on USART0, but no USART0 exists
    #endif
    #define MYUART USART0
    #define MYUART_TXPORT VPORTD
    #define MYUART_TXPIN (1 << PORT4)
    #define MYUART_PMUX_VAL (3)
    #define MYPMUX_REG (PORTMUX.USARTROUTEA)
  #elif (UARTTX == C0)
    #define UART_NAME "C0"
    #error Requested USART was USART1 mux 0, but only mux2 is available on 20-pin parts
  #elif (UARTTX == D6)
    #define UART_NAME "D6"
    #ifndef USART1
      #error Pin on USART1, but no USART1 exists
    #endif
    #define MYUART USART1
    #define MYUART_TXPORT VPORTD
    #define MYUART_TXPIN (1 << PORT6)
    #define MYUART_PMUX_VAL (2 << 3)
    #define MYPMUX_REG (PORTMUX.USARTROUTEA)
  #endif
#endif
#if defined(__AVR_AVR64DD14__) || defined(__AVR_AVR32DD14__) || defined(__AVR_AVR16DD14__)
  #define __AVR_DD__
  #if (UARTTX == A0)
    #define UART_NAME "A0"
    #ifndef USART0
      #error Pin on USART0, but no USART0 exists
    #endif
    #define MYUART USART0
    #define MYUART_RXPINCTRL (PORTA.PIN1CTRL)
    #define MYUART_TXPORT VPORTA
    #define MYUART_TXPIN (1 << PORT0)
    #define MYUART_PMUX_VAL (0)
    #define MYPMUX_REG (PORTMUX.USARTROUTEA)
  #elif (UARTTX == A4)
    #define UART_NAME "A4"
    #error Requested USART was USART1 mux 1, but only mux0, 3 and 4 are available on 14-pin parts
  #elif (UARTTX == A2)
    #define UART_NAME "A2"
    #error Requested USART was USART1 mux 2, but only mux0, 3 and 4 are available on 14-pin parts
  #elif (UARTTX == D4)
    #define UART_NAME "D4"
    #ifndef USART0
      #error Pin on USART0, but no USART0 exists
    #endif
    #define MYUART USART0
    #define MYUART_RXPINCTRL (PORTD.PIN5CTRL)
    #define MYUART_TXPORT VPORTD
    #define MYUART_TXPIN (1 << PORT4)
    #define MYUART_PMUX_VAL (3)
    #define MYPMUX_REG (PORTMUX.USARTROUTEA)
  #elif (UARTTX == C1)
    #define UART_NAME "C1"
    #ifndef USART0
      #error Pin on USART0, but no USART0 exists
    #endif
    #define MYUART USART0
    #define MYUART_RXPINCTRL (PORTC.PIN2CTRL)
    #define MYUART_TXPORT VPORTC
    #define MYUART_TXPIN (1 << PORT1)
    #define MYUART_PMUX_VAL (4)
    #define MYPMUX_REG (PORTMUX.USARTROUTEA)

  #elif (UARTTX == C0)
    #define UART_NAME "C0"
    #error Requested USART was USART1 mux 0, but only mux2 is available on 14-pin parts
  #elif (UARTTX == D6)
    #define UART_NAME "D6"
    #ifndef USART1
      #error Pin on USART1, but no USART1 exists
    #endif
    #define MYUART USART1
    #define MYUART_RXPINCTRL (PORTA.PIN7CTRL)
    #define MYUART_TXPORT VPORTD
    #define MYUART_TXPIN (1 << PORT6)
    #define MYUART_PMUX_VAL (2 << 4)
    #define MYPMUX_REG (PORTMUX.USARTROUTEA)
  #endif
#endif
#ifndef MYUART
  #warning No UARTTX pin specified, or specified pin does not correspond to a port or mux option on this part family.
#endif
