/* UART0.cpp - Hardware serial library
 * This library is free software under LGPL 2.1. See License.md
 * for more information. This file is part of DxCore.
 *
 * Copyright (c) 2006 Nicholas Zambetti, Modified by
 * 11/23/2006 David A. Mellis, 9/20/2010 Mark Sproul,
 * 8/24/2012 Alarus, 12/3/2013 Matthijs Kooijman
 * unknown others 2013-2020, 2020-2021 Spence Konde
 */

/* Each UartClass is defined in its own file, sine the linker pulls
 * in the entire file when any element inside is used. --gc-sections can
 * additionally cause unused symbols to be dropped, but ISRs have the
 * "used" attribute so are never dropped and they keep the
 * UartClass instance in as well. Putting each instance in its own
 * file prevents the linker from pulling in any unused instances in the
 * first place.
 */
#include "Arduino.h"
#include "UART.h"
#include "UART_private.h"


#if defined(USART0)
  ISR(USART0_TXC_vect) {
    // only enabled in half duplex mode - we disable RX interrupt while sending.
    // When we are done sending, we reenable the RX interrupt and disable this one.
    // Note that we do NOT clear TXC flag, which the flush() method relies on.
    uint8_t ctrla;
    while (USART0.STATUS & USART_RXCIF_bm) {
      ctrla = USART0.RXDATAL;   // We sent these, so dump them, using local var as trashcan.
    }
    ctrla = USART0.CTRLA;       // Get current CTRLA
    ctrla |= USART_RXCIE_bm;    // turn on receive complete
    ctrla &= ~USART_TXCIE_bm;   // turn off transmit complete
    USART0.CTRLA = ctrla;       // Write it back to CTRLA.
  }

  ISR(USART0_RXC_vect) {
    UartClass::_rx_complete_irq(Serial0);
  }

  ISR(USART0_DRE_vect) {
    UartClass::_tx_data_empty_irq(Serial0);
  }

  UartClass Serial0(&USART0, (uint8_t*)_usart0_pins, MUXCOUNT_USART0, HWSERIAL0_MUX_DEFAULT);
#endif
