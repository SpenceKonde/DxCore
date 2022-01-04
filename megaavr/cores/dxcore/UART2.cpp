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
  #if USE_ASM_TXC == 1
    ISR(USART2_TXC_vect, ISR_NAKED) {
      __asm__ __volatile__(
            "push  r30"           "\n\t" // push the low byte of Z
            "ldi r30, 0x40"       "\n\t" // and put the low bit of this USART there - 0x20 * n
#if PROGMEM_SIZE > 8192
            "jmp _do_txc"         "\n\t"
#else
            "rjmp _do_txc"        "\n\t"
#endif // _do_txc pushes the other necessary registers and loads 0x08 into the high byte.
            :::);
    }
  #else
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
  #endif
  #if !(USE_ASM_RXC == 1 && (SERIAL_RX_BUFFER_SIZE == 256 || SERIAL_RX_BUFFER_SIZE == 128 || SERIAL_RX_BUFFER_SIZE == 64 || SERIAL_RX_BUFFER_SIZE == 32 || SERIAL_RX_BUFFER_SIZE == 16))
    ISR(USART2_RXC_vect) {
      UartClass::_rx_complete_irq(Serial2);
    }
  #else
      ISR(USART2_RXC_vect, ISR_NAKED) {
        __asm__ __volatile__(
              "push      r30"     "\n\t"
              "push      r31"     "\n\t"
              :::);
        __asm__ __volatile__(
#if PROGMEM_SIZE > 8192
              "jmp   _do_rxc"     "\n\t"
#else
              "rjmp   _do_rxc"    "\n\t"
#endif
              ::"z"(&Serial2));
        __builtin_unreachable();
    }
  #endif

  #if !(USE_ASM_DRE == 1 && (SERIAL_RX_BUFFER_SIZE == 256 || SERIAL_RX_BUFFER_SIZE == 128 || SERIAL_RX_BUFFER_SIZE == 64 || SERIAL_RX_BUFFER_SIZE == 32 || SERIAL_RX_BUFFER_SIZE == 16) && \
                            (SERIAL_TX_BUFFER_SIZE == 256 || SERIAL_TX_BUFFER_SIZE == 128 || SERIAL_TX_BUFFER_SIZE == 64 || SERIAL_TX_BUFFER_SIZE == 32 || SERIAL_TX_BUFFER_SIZE == 16))
    ISR(USART2_DRE_vect) {
      UartClass::_tx_data_empty_irq(Serial2);
    }
  #else
    ISR(USART2_DRE_vect, ISR_NAKED) {
      __asm__ __volatile__(
                "push  r30"       "\n\t"
                "push  r31"       "\n\t"
                :::);
      __asm__ __volatile__(
#if PROGMEM_SIZE > 8192
                "jmp _do_dre"     "\n\t"
#else
                "rjmp _do_dre"    "\n\t"
#endif
                ::"z"(&Serial2));
      __builtin_unreachable();
    }
  #endif
  UartClass Serial2(&USART2, (uint8_t*)_usart2_pins, MUXCOUNT_USART2, HWSERIAL2_MUX_DEFAULT);
#endif
