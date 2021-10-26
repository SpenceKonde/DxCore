/*
  UART_private.h - Hardware serial library for Wiring
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
*/

#include "wiring_private.h"

// this next line disables the entire UART.cpp,
// this is so I can support Attiny series and any other chip without an uart
#if defined(HAVE_HWSERIAL0) || defined(HAVE_HWSERIAL1) || defined(HAVE_HWSERIAL2) || defined(HAVE_HWSERIAL3)

// Constructors ////////////////////////////////////////////////////////////////

// no need to set the other variables to zero, init script already does that. Saves some flash
UartClass::UartClass(volatile USART_t *hwserial_module, uint8_t *usart_pins, uint8_t mux_count, uint8_t mux_defualt) :
    _hwserial_module(hwserial_module), _usart_pins(usart_pins), _mux_count(mux_count), _pin_set(mux_defualt) {
}

// Actual interrupt handlers //////////////////////////////////////////////////////////////
// Moved to UART.cpp
//  void UartClass::_rx_complete_irq(UartClass& uartClass) {
//  //if (bit_is_clear(*_rxdatah, USART_PERR_bp)) {
//  uint8_t rxDataH = uartClass._hwserial_module->RXDATAH;
//  if (!(rxDataH & USART_PERR_bm)) {
//    // No Parity error, read byte and store it in the buffer if there is
//    // room
//    unsigned char c = uartClass._hwserial_module->RXDATAL;
//    rx_buffer_index_t i = (unsigned int)(uartClass._rx_buffer_head + 1) % SERIAL_RX_BUFFER_SIZE;
//
//    // if we should be storing the received character into the location
//    // just before the tail (meaning that the head would advance to the
//    // current location of the tail), we're about to overflow the buffer
//    // and so we don't write the character or advance the head.
//    if (i != uartClass._rx_buffer_tail) {
//      uartClass._rx_buffer[uartClass._rx_buffer_head] = c;
//      uartClass._rx_buffer_head = i;
//    }
//  } else {
//    // Parity error, read byte but discard it
//    uartClass._hwserial_module->RXDATAL;
//  }
//}

#endif  // whole file
