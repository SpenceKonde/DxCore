/* UART4.cpp - Hardware serial library
 * This library is free software under LGPL 2.1. See License.md
 * for more information. This file is part of DxCore.
 *
 * Copyright (c) 2006 Nicholas Zambetti, Modified by
 * 11/23/2006 David A. Mellis, 9/20/2010 Mark Sproul,
 * 8/24/2012 Alarus, 12/3/2013 Matthijs Kooijman
 * unknown others 2013-2020, 2020-2021 Spence Konde
 */

#include "Arduino.h"
#include "UART.h"
#include "UART_private.h"

#if defined(USART4)
  ISR(USART4_TXC_vect) {
    uint8_t ctrla;
    while (USART4.STATUS & USART_RXCIF_bm) {
      ctrla = USART4.RXDATAL;
    }
    ctrla = USART4.CTRLA;
    ctrla |= USART_RXCIE_bm; // turn on receive complete
    ctrla &= ~USART_TXCIE_bm; // turn off transmit complete
    USART4.CTRLA = ctrla;
  }

  ISR(USART4_RXC_vect) {
    UartClass::_rx_complete_irq(Serial4);
  }

  ISR(USART4_DRE_vect) {
    UartClass::_tx_data_empty_irq(Serial4);
  }

  UartClass Serial4(&USART4, (uint8_t*)_usart4_pins, MUXCOUNT_USART4, HWSERIAL4_MUX_DEFAULT);
#endif  // HWSERIAL4
