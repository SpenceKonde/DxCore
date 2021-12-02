/* UART_private.h - Constructor for UartClass
 * This library is free software released under LGPL 2.1.
 * See License.md for more information.
 * This file is part of DxCore.
 *
 * Totally rewrittem 2021 by Spence Konde and MX682X.
 * The original version was written in 2006 by Nicholas
 * Zambetti, and modified 11/23/2006 by David A. Mellis,
 * 9/20/2010 by Mark Sproul, 8/24/2012 by Alarus, and
 * Others (unknown) 2013-2017.
 */

#include "wiring_private.h"

#if defined(USART0) || defined(USART1) || defined(USART2) || defined(USART3) || defined(USART4) || defined(USART5)

// Constructor
// no need to set the other variables to zero, init script already does that. Saves some flash
UartClass::UartClass(volatile USART_t *hwserial_module, uint8_t *usart_pins, uint8_t mux_count, uint8_t mux_default) :
    _hwserial_module(hwserial_module), _usart_pins(usart_pins), _mux_count(mux_count), _pin_set(mux_default) {
}

#endif  // whole file
