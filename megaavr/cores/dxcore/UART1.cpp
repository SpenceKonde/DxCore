/* UART1.cpp - Hardware serial library
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

#if defined(USART1)
  ISR(USART1_TXC_vect) {
    uint8_t ctrla;
    while (USART1.STATUS & USART_RXCIF_bm) {
      ctrla = USART1.RXDATAL;
    }
    ctrla = USART1.CTRLA;
    ctrla |= USART_RXCIE_bm; // turn on receive complete
    ctrla &= ~USART_TXCIE_bm; // turn off transmit complete
    USART1.CTRLA = ctrla;
  }

  ISR(USART1_RXC_vect) {
    UartClass::_rx_complete_irq(Serial1);
  }

  ISR(USART1_DRE_vect) {
    UartClass::_tx_data_empty_irq(Serial1);
  }

  UartClass Serial1(&USART1, (uint8_t*)_usart1_pins, MUXCOUNT_USART1, HWSERIAL1_MUX_DEFAULT);
#endif  // HWSERIAL1
