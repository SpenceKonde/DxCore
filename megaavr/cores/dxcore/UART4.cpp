/*
  UART4.cpp - Hardware serial library for Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Modified 23 November 2006 by David A. Mellis
  Modified 28 September 2010 by Mark Sproul
  Modified 14 August 2012 by Alarus
  Modified 3 December 2013 by Matthijs Kooijman
  Modified November 2021 by Spence Konde
*/

#include "Arduino.h"
#include "UART.h"
#include "UART_private.h"

#if defined(USART4)

// Each UartClass is defined in its own file, sine the linker pulls
// in the entire file when any element inside is used. --gc-sections can
// additionally cause unused symbols to be dropped, but ISRs have the
// "used" attribute so are never dropped and they keep the
// UartClass instance in as well. Putting each instance in its own
// file prevents the linker from pulling in any unused instances in the
// first place.


ISR(USART4_RXC_vect) {
  UartClass::_rx_complete_irq(Serial4);
}
ISR(USART4_TXC_vect) { //only called for half duplex mode, so we don't get all of the characters we sent.
  uint8_t ctrla;
  while (USART4.STATUS & USART_RXCIF_bm) {
    // dump these these, using local var as trashcan.
    // Used only in half duplex - this int means switching from send to receive.
    ctrla = USART4.RXDATAL;
  }
  ctrla = USART4.CTRLA;
  ctrla |= USART_RXCIE_bm; // turn on receive complete
  ctrla &= ~USART_TXCIE_bm; // turn off transmit complete
  USART4.CTRLA = ctrla;
}
ISR(USART4_DRE_vect) {
  UartClass::_tx_data_empty_irq(Serial4);
}

UartClass Serial4(HWSERIAL4, (uint8_t*)_usart4_pins, MUXCOUNT_USART4, MUX_DEFAULT_USART4);


#endif  // HWSERIAL4
