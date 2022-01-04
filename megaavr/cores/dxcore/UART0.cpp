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
 * There are now two versions of each ISR. Except for TXC, which is trivial,
 * both versions are stubs to call the real code (so it is not duplicated).
 * The ASM versions are more efficient that calling normally can be because
 * they completely ignore the ABI rules and do it the most efficient way
 * they can. They'll stop working if anything changes, and it's not
 * entirely clear whether the trick of dropping out of assembly while naked
 * to grab the address is legal, though there's no reason it shouldn't be.
 * The assembly implementations over in UART.cpp depends on the structure
 * of the SerialClass. Any changes to the class member variables will
 * require changes to the asm to match.
 */
#include "Arduino.h"
#include "UART.h"
#include "UART_private.h"


#if defined(USART0)
  #if USE_ASM_TXC == 1
    ISR(USART0_TXC_vect, ISR_NAKED) {
      __asm__ __volatile__(
            "push  r30"         "\n\t" // push the low byte of Z
            "ldi r30, 0x00"     "\n\t" // and put the low bit of this USART there - 0x20 * n
#if PROGMEM_SIZE > 8192
            "jmp _do_txc"       "\n\t"
#else
            "rjmp _do_txc"      "\n\t"
#endif // _do_txc pushes the other necessary registers and loads 0x08 into the high byte.
            :::);
    }
  #else
    ISR(USART0_TXC_vect) {
      // only enabled in half duplex mode - we disable RX interrupt while sending.
      // When we are done sending, we re-enable the RX interrupt and disable this one.
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
  #endif
  #if !(USE_ASM_RXC == 1 && (SERIAL_RX_BUFFER_SIZE == 256 || SERIAL_RX_BUFFER_SIZE == 128 || SERIAL_RX_BUFFER_SIZE == 64 || SERIAL_RX_BUFFER_SIZE == 32 || SERIAL_RX_BUFFER_SIZE == 16))
    ISR(USART0_RXC_vect) {
      UartClass::_rx_complete_irq(Serial0);
    }
  #else
    ISR(USART0_RXC_vect, ISR_NAKED) {
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
            ::"z"(&Serial0));
      __builtin_unreachable();
  }
  #endif
  #if !(USE_ASM_DRE == 1 && (SERIAL_RX_BUFFER_SIZE == 256 || SERIAL_RX_BUFFER_SIZE == 128 || SERIAL_RX_BUFFER_SIZE == 64 || SERIAL_RX_BUFFER_SIZE == 32 || SERIAL_RX_BUFFER_SIZE == 16) && \
                            (SERIAL_TX_BUFFER_SIZE == 256 || SERIAL_TX_BUFFER_SIZE == 128 || SERIAL_TX_BUFFER_SIZE == 64 || SERIAL_TX_BUFFER_SIZE == 32 || SERIAL_TX_BUFFER_SIZE == 16))
    ISR(USART0_DRE_vect) {
      UartClass::_tx_data_empty_irq(Serial0);
    }
  #else
    ISR(USART0_DRE_vect, ISR_NAKED) {
      __asm__ __volatile__(
                "push  r30"    "\n\t"
                "push  r31"    "\n\t"
                :::);
      __asm__ __volatile__(
#if PROGMEM_SIZE > 8192
                "jmp _do_dre"  "\n\t"
#else
                "rjmp _do_dre" "\n\t"
#endif
                ::"z"(&Serial0));
      __builtin_unreachable();
    }
  #endif
  UartClass Serial0(&USART0, (uint8_t*)_usart0_pins, MUXCOUNT_USART0, HWSERIAL0_MUX_DEFAULT);
#endif
