/* UART2.cpp - Hardware serial library
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

#if defined(USART2)
  ISR(USART2_TXC_vect) {
    uint8_t ctrla;
    while (USART2.STATUS & USART_RXCIF_bm) {
      ctrla = USART2.RXDATAL;
    }
    ctrla = USART2.CTRLA;
    ctrla |= USART_RXCIE_bm; // turn on receive complete
    ctrla &= ~USART_TXCIE_bm; // turn off transmit complete
    USART2.CTRLA = ctrla;
  }

  ISR(USART2_RXC_vect) {
    UartClass::_rx_complete_irq(Serial2);
  }

  ISR(USART2_DRE_vect) {
    UartClass::_tx_data_empty_irq(Serial2);
  }

  UartClass Serial2(&USART2, (uint8_t*)_usart2_pins, MUXCOUNT_USART2, HWSERIAL2_MUX_DEFAULT);
#endif
