/* This file contains the sort of things that were defined in the Arduino API files
 * I needed to remove them there, but didn't want to put their replacements there,
 * since that would just cause confusion.
 *
 * Written by Spence Konde and MX682X in late 2021.
 *
 * This file is part of DxCore, which is free software (LGPL 2.1)
 * See License.md for more information
 *
 * These are values that get passed to the second argument of Serial.begin in
 * DxCore 1.4.0 and later and some future version of megaTinyCore
 */

#ifndef UART_CONSTANTS_H
  #define UART_CONSTANTS_H

  #define SERIAL_PARITY_EVEN   (USART_PMODE_EVEN_gc)
  #define SERIAL_PARITY_ODD    (USART_PMODE_ODD_gc)
  #define SERIAL_PARITY_NONE   (USART_PMODE_DISABLED_gc)
  #define SERIAL_PARITY_MASK   (USART_PMODE_gm)

  #define SERIAL_STOP_BIT_1    (USART_SBMODE_1BIT_gc)
  #define SERIAL_STOP_BIT_2    (USART_SBMODE_2BIT_gc)
  #define SERIAL_STOP_BIT_MASK (USART_SBMODE_gm)

  #define SERIAL_DATA_5        (USART_CHSIZE_5BIT_gc)
  #define SERIAL_DATA_6        (USART_CHSIZE_6BIT_gc)
  #define SERIAL_DATA_7        (USART_CHSIZE_7BIT_gc)
  #define SERIAL_DATA_8        (USART_CHSIZE_8BIT_gc)
  #define SERIAL_DATA_MASK     (USART_CHSIZE_gm)
/* 9-bit is a can of worms. Aggressive ones with sharp teeth,
 * hungry for soft fle-- oh, hm, it seems to be a typo, it says "flash"
 * Nothing uses 9-bit serial! Including our implementation. */

  #define SERIAL_MODE_MSPI     (USART_CMODE_MSPI_gc)
  #define SERIAL_MODE_IRCOM    (USART_CMODE_IRCOM_gc)
  #define SERIAL_MODE_SYNC     (USART_CMODE_SYNCHRONOUS_gc)
/* MSPI doesn't Parity, Stop/Start bits or the size field.
 * Sync and IRCOM modes do.
 */

/* CTRLA is interrupt flags, plus 3 some options relevant to RS485
 *
 * | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 | USARTn.CTRLA
 * |---|---|---|---|---|---|---|---|
 * |RXC|TXC|DRE|RXS| - |ABE| ? | - |  Interrupt Enable bits
 * | -   -   -   - |LBM| - | RS485 |
 *
 * LBME - Loopback Mode Enable turns off RX and ties that channel to TX
 * RS485 - RS485 mode tells it to drive XDIR pin HIGH while transmitting in
 *   order to control an external RS485 line driver. 1 bit of guard time on
 *   either side.
 *
 * RS485 field is strange - on 0/1-series, bit 1 was used was described as
 * an transmit enable field for the UART, so an external device would use
 * that to tell the device when it was allowed to drive the pin. But that
 * has not been described in recent datasheets and the bit is marked as
 * reserved, and only 1 RS485 mode was supported.
 * Just as well - I've never tried it and am unsure of what it was supposed
 * to do, but I suspect you that could debate how something like that "should"
 * work in all the corner cases for weeks. It brings up all a great many
 * issues, was probably a nightmare for designers, and considering the amount
 * of errata on those parts, it probably didn't work very well.
 */

/* CTRLB configures a few major aspects of configuration:
 *
 * | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 | USARTn.CTRLB
 * |-------|---|---|---|-------|---|
 * | TX RX | - |SFD|ODM|RX MODE|MPCM
 * |ENABLE | - | Configuration |   |
 *
 * TXEN - enables transmit functionality. When it transitions from enable to
 *    disabled, the pin is forcibly set to an input.
 * RXEN - enables receive functionality
 * SFDEN - enables Start of Frame detection to wake from sleep.
 * ODME - enables Open Drain Mode. Tx pin must be INPUT instead of OUTPUT!
 * MPCM - Multi Processor Communication Mode. A weird Atmel-only feature that
 *   turns a UART into a multidrop bus. I doubt it is in widespread use anywhere.
 *   Looks like a the bastard son of UART and I2C that never got enough love or
 *   uptake to prosper - or a backup plan dating to a legal scrap with Phillips
 * RXMODE - Controls the baud rate for both sending and receiving. 4 modes:
 *   Normal
 *   U2X - double-speed USART - much less important now with the fractional
 *   baud rate generator, but does the same thing. Serial.begin() chooses this
 *   if it needs to in order to generate the required baud rate only.
 *   GENAUTO - generic auto-baud mode
 *   LINAUTO - LIN constrained auto-baud mode
 * Autobaud mode requires a "break" to be sent follwed by a sync (0x55).
 *   LIN mode has more constraints in accordance with that stadatd, while
 *   generic mode has clearly just disabled the checks and added a WFB or
 *   "wait for break" option so you can use autobaud without having to
 *   guess what baud it's using. Serial implements no support for this.
 *   These parts have no trouble keeping a clock accurate enough for serial
 *   and since you need to have control over the software on the the device
 *   that it's talking so you can make it send the break/sync, and you are
 *   also programming this device, wherever it is viable, you know the baud
 *   rate that both ends are set to and don't need it. It's great if you need
 *   to meet the LIN specifications, of course, but that's not a typical or
 *   advisable Arduino use case...
 *
 */

/* Everything contigured by the SERIAL_xPs constants goes right into CTRLC:
 *
 * | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 | USARTn.CTRLC
 * |---|---|---|---|---|---|---|---|
 * |  MODE | PARITY| SB| CHAR SIZE |
 *
 *
 * Plus there's there's the IRCOM - it is able to take an event input, and
 * you can turn off the pulse coding that nobody's going to ue in Arduino-land
 * IRDA has been dead for years - we don't support using that, but we want event input.
 *
 * Additionally there are reasons to want to control to some bits from CTRLA + CTRLB too...
 *
 * It's also not clear yet how it actually behaves when disabled, namely:
 *   Can the event input be used in all modes, or must it be in IRCOM mode?
 *   Do we have to do something that applies the 115200 baud maximum to get it?
 *
 * We want a way to request that only TX, or only RX be enabled, but default should be both.
 * And we want to control Open Drain, RS485 and LoopBack options independently of eachother
 *
 * In summary:
 *
 * | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
 * |---|---|---|---|---|---|---|---|
 * | x   x   x |   |LBM|       |485|  CTRLA
 * |TXRX EN|       |ODM| x  __ |   |  CTRLB
 * |                           |IREI  EVCTRL
 * Bits used actively for basic UART are marked with an x.
 *
 * That means 6 bits will be needed. Plus, we sometimes use use one of
 * them internally, and the tinyAVR parts have that funky backwards
 * RS485 option
 * All the ones CTRLA default 0, and so we can AND with 0x09 (or 0x0B on tinyAVR) and OR with other CTRLA settings.
 * And we want the two inverted CTRLB ones in their final locations so we can AND with 0xC0 and XOR the settings
 * we're going to write to CTRLB. The other CTRLB one and the interal need to be done individually, so it doesn't matter
 * that needs to set other options too.
 */
  #define SERIAL_LOOPBACK       (((uint16_t) USART_LBME_bm)  << 8)
  #define SERIAL_RS485          (((uint16_t) USART_RS485_bm) << 8)
  //#define SERIAL_RS485_REVERSE(((uint16_t) USART_RS485_bm) << 8) // tinyAVR 0/1
  #define SERIAL_TX_ONLY        (((uint16_t) USART_RXEN_bm)  << 8) // The TXEN/RXEN bits are swapped - we invert the meaning of this bit.
  #define SERIAL_RX_ONLY        (((uint16_t) USART_TXEN_bm)  << 8) // so if not specified, you get a serial port with both pins, a pinless one.
  #define SERIAL_OPENDRAIN       ((uint16_t) 0x0400)
  #define SERIAL_INTERNAL        ((uint16_t) 0x1000)               // Internal use only - setting this is likely to break baud calculations.
  #define SERIAL_EVENT_RX        ((uint16_t) 0x2000)
  #define SERIAL_HALF_DUPLEX     (SERIAL_LOOPBACK | SERIAL_OPENDRAIN)


  #define SERIAL_5N1  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_1 | SERIAL_PARITY_NONE | SERIAL_DATA_5)
  #define SERIAL_6N1  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_1 | SERIAL_PARITY_NONE | SERIAL_DATA_6)
  #define SERIAL_7N1  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_1 | SERIAL_PARITY_NONE | SERIAL_DATA_7)
  #define SERIAL_8N1  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_1 | SERIAL_PARITY_NONE | SERIAL_DATA_8)

  #define SERIAL_5N2  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_2 | SERIAL_PARITY_NONE | SERIAL_DATA_5)
  #define SERIAL_6N2  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_2 | SERIAL_PARITY_NONE | SERIAL_DATA_6)
  #define SERIAL_7N2  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_2 | SERIAL_PARITY_NONE | SERIAL_DATA_7)
  #define SERIAL_8N2  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_2 | SERIAL_PARITY_NONE | SERIAL_DATA_8)

  #define SERIAL_5E1  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_1 | SERIAL_PARITY_EVEN | SERIAL_DATA_5)
  #define SERIAL_6E1  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_1 | SERIAL_PARITY_EVEN | SERIAL_DATA_6)
  #define SERIAL_7E1  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_1 | SERIAL_PARITY_EVEN | SERIAL_DATA_7)
  #define SERIAL_8E1  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_1 | SERIAL_PARITY_EVEN | SERIAL_DATA_8)

  #define SERIAL_5E2  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_2 | SERIAL_PARITY_EVEN | SERIAL_DATA_5)
  #define SERIAL_6E2  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_2 | SERIAL_PARITY_EVEN | SERIAL_DATA_6)
  #define SERIAL_7E2  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_2 | SERIAL_PARITY_EVEN | SERIAL_DATA_7)
  #define SERIAL_8E2  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_2 | SERIAL_PARITY_EVEN | SERIAL_DATA_8)

  #define SERIAL_5O1  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_1 |  SERIAL_PARITY_ODD | SERIAL_DATA_5)
  #define SERIAL_6O1  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_1 |  SERIAL_PARITY_ODD | SERIAL_DATA_6)
  #define SERIAL_7O1  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_1 |  SERIAL_PARITY_ODD | SERIAL_DATA_7)
  #define SERIAL_8O1  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_1 |  SERIAL_PARITY_ODD | SERIAL_DATA_8)

  #define SERIAL_5O2  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_2 |  SERIAL_PARITY_ODD | SERIAL_DATA_5)
  #define SERIAL_6O2  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_2 |  SERIAL_PARITY_ODD | SERIAL_DATA_6)
  #define SERIAL_7O2  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_2 |  SERIAL_PARITY_ODD | SERIAL_DATA_7)
  #define SERIAL_8O2  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_2 |  SERIAL_PARITY_ODD | SERIAL_DATA_8)
  /* These are not usable on these parts. badCall() gets pulled in if you use them. */

  #define SERIAL_PARITY_MARK   (badArg("Mark as 'parity' is not supported, use an extra stop bit"),0)
  #define SERIAL_PARITY_SPACE  (badArg("Space as 'parity' is not supported on AVR"),0)
  #define SERIAL_STOP_BIT_1_5  (badArg("1.5 stop bits is not supported on AVR"),0)
  #define SERIAL_5M1  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_1 | SERIAL_PARITY_MARK | SERIAL_DATA_5)
  #define SERIAL_6M1  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_1 | SERIAL_PARITY_MARK | SERIAL_DATA_6)
  #define SERIAL_7M1  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_1 | SERIAL_PARITY_MARK | SERIAL_DATA_7)
  #define SERIAL_8M1  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_1 | SERIAL_PARITY_MARK | SERIAL_DATA_8)
  #define SERIAL_5M2  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_2 | SERIAL_PARITY_MARK | SERIAL_DATA_5)
  #define SERIAL_6M2  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_2 | SERIAL_PARITY_MARK | SERIAL_DATA_6)
  #define SERIAL_7M2  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_2 | SERIAL_PARITY_MARK | SERIAL_DATA_7)
  #define SERIAL_8M2  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_2 | SERIAL_PARITY_MARK | SERIAL_DATA_8)
  #define SERIAL_5S1  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_1 |SERIAL_PARITY_SPACE | SERIAL_DATA_5)
  #define SERIAL_6S1  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_1 |SERIAL_PARITY_SPACE | SERIAL_DATA_6)
  #define SERIAL_7S1  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_1 |SERIAL_PARITY_SPACE | SERIAL_DATA_7)
  #define SERIAL_8S1  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_1 |SERIAL_PARITY_SPACE | SERIAL_DATA_8)
  #define SERIAL_5S2  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_2 |SERIAL_PARITY_SPACE | SERIAL_DATA_5)
  #define SERIAL_6S2  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_2 |SERIAL_PARITY_SPACE | SERIAL_DATA_6)
  #define SERIAL_7S2  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_2 |SERIAL_PARITY_SPACE | SERIAL_DATA_7)
  #define SERIAL_8S2  (USART_CMODE_ASYNCHRONOUS_gc | SERIAL_STOP_BIT_2 |SERIAL_PARITY_SPACE | SERIAL_DATA_8)

#endif
