/*
  UART.cpp - Hardware serial library for Wiring
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

  Created: 09.11.2017 07:29:09
  Author: M44307
  Modified considerably by Spence Konde 2018-2021, with particularly
  large changes in late 2020 to remove the interrupt priority stuff
  which could (very rarely) cause Serial to block in adverse
  conditions.
  Notable contriubutions from MX682X late 2021 to improve pin swap.
  Which I originally cribbed from MCUDude's MegaCoreX.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <util/atomic.h>
#include <avr/io.h>
#include "Arduino.h"

#include "UART.h"
#include "UART_private.h"
#include "UART_swap.h"

// this next line disables the entire UART.cpp if there's no hardware serial
#if defined(USART0) || defined(USART1) || defined(USART2) || defined(USART3) || defined(USART4) || defined(USART5)

// As of 1.3.3 serialEvent is no longer exposed through the core. It is deprecated on official core - screw it.
  // SerialEvent functions are weak, so when the user doesn't define them,
  // the linker just sets their address to 0 (which is checked below).
  // The Serialx_available is just a wrapper around Serialx.available(),
  // but we can refer to it weakly so we don't pull in the entire
  // UART instance if the user doesn't also refer to it.
  #if defined(ENABLE_SERIAL_EVENT)
    #if defined(HWSERIAL0)
      void serialEvent() __attribute__((weak));
      bool Serial0_available() __attribute__((weak));
    #endif

    #if defined(HWSERIAL1)
      void serialEvent1() __attribute__((weak));
      bool Serial1_available() __attribute__((weak));
    #endif

    #if defined(HWSERIAL2)
      void serialEvent2() __attribute__((weak));
      bool Serial2_available() __attribute__((weak));
    #endif

    #if defined(HWSERIAL3)
      void serialEvent3() __attribute__((weak));
      bool Serial3_available() __attribute__((weak));
    #endif

    #if defined(HWSERIAL4)
      void serialEvent4() __attribute__((weak));
      bool Serial4_available() __attribute__((weak));
    #endif

    #if defined(HWSERIAL5)
      void serialEvent5() __attribute__((weak));
      bool Serial5_available() __attribute__((weak));
    #endif

    void serialEventRun(void) {
    #if defined(HWSERIAL0)
      if (Serial0_available && serialEvent && Serial0_available()) serialEvent();
    #endif
    #if defined(HWSERIAL1)
      if (Serial1_available && serialEvent1 && Serial1_available()) serialEvent1();
    #endif
    #if defined(HWSERIAL2)
      if (Serial2_available && serialEvent2 && Serial2_available()) serialEvent2();
    #endif
    #if defined(HWSERIAL3)
      if (Serial3_available && serialEvent3 && Serial3_available()) serialEvent3();
    #endif
    #if defined(HWSERIAL4)
      if (Serial4_available && serialEvent4 && Serial4_available()) serialEvent4();
    #endif
    #if defined(HWSERIAL5)
      if (Serial5_available && serialEvent5 && Serial5_available()) serialEvent5();
    #endif
    }
  #endif

// macro to guard critical sections when needed for large TX buffer sizes
#if (SERIAL_TX_BUFFER_SIZE > 256)
#define TX_BUFFER_ATOMIC ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
#else
#define TX_BUFFER_ATOMIC
#endif

// Actual interrupt handlers //////////////////////////////////////////////////////////////

void UartClass::_rx_complete_irq(UartClass& uartClass) {
  // if (bit_is_clear(*_rxdatah, USART_PERR_bp)) {
  uint8_t rxDataH = uartClass._hwserial_module->RXDATAH;
  uint8_t c = uartClass._hwserial_module->RXDATAL;  // no need to read the data twice. read it, then decide what to do
  rx_buffer_index_t rxHead = uartClass._rx_buffer_head;

  if (!(rxDataH & USART_PERR_bm)) {
    // No Parity error, read byte and store it in the buffer if there is room
    // unsigned char c = uartClass._hwserial_module->RXDATAL;
    rx_buffer_index_t i = (unsigned int)(rxHead + 1) % SERIAL_RX_BUFFER_SIZE;

    // if we should be storing the received character into the location
    // just before the tail (meaning that the head would advance to the
    // current location of the tail), we're about to overflow the buffer
    // and so we don't write the character or advance the head.
    if (i != uartClass._rx_buffer_tail) {
      uartClass._rx_buffer[rxHead] = c;
      uartClass._rx_buffer_head = i;
    }
  }   // else {
    // Parity error, read byte but discard it
    // uartClass._hwserial_module->RXDATAL;
  //}
}


void UartClass::_tx_data_empty_irq(UartClass& uartClass) {
  USART_t* usartModule = (USART_t*)uartClass._hwserial_module;  // reduces size a little bit
  tx_buffer_index_t txTail = uartClass._tx_buffer_tail;
/*
  // Check if tx buffer already empty. when called by _poll_tx_data_empty()
  if (uartClass._tx_buffer_head == txTail) {
    // Buffer empty, so disable "data register empty" interrupt
    usartModule->CTRLA &= (~USART_DREIE_bm);
    return;
  } //moved to poll function to make ISR smaller and faster
*/

  // There must be more data in the output
  // buffer. Send the next byte
  uint8_t c = uartClass._tx_buffer[txTail];

  // clear the TXCIF flag -- "can be cleared by writing a one to its bit
  // location". This makes sure flush() won't return until the bytes
  // actually got written. It is critical to do this BEFORE we write the next byte
  usartModule->STATUS = USART_TXCIF_bm;
  usartModule->TXDATAL = c;

  txTail = (txTail + 1) & (SERIAL_TX_BUFFER_SIZE-1);  //% SERIAL_TX_BUFFER_SIZE;

  if (uartClass._tx_buffer_head == txTail) {
    // Buffer empty, so disable "data register empty" interrupt
    usartModule->CTRLA &= (~USART_DREIE_bm);
  }
  uartClass._tx_buffer_tail = txTail;


// an attempt to put the annoying part in assembler. can NOT considered to be reliable.
// also, since the compiler has to push/pop r28/r29 it ends up as big as the C version
// Is probably faster though, since every Y+x here is replaced by ADIW X, x; ST/LD X, y; SBIW X, x in C.
// the assembly basically cuts 4 cycles on every memory access...
// It would be better to implement the assembler interrupt routines in a separate .S file
/*
  asm volatile (
    "MOVW R30, %0   \n\t"   // make sure the uartClass is actually in Z
    "LDD  R28, Z+12 \n\t"   // load USART module pointer
    "LDD  R29, Z+13 \n\t"   // same as above, high byte
    "LDI  R18, 0x40 \n\t"   // load USART_TXCIF_bm
    "STD  Y+4, R18  \n\t"   // and store to STATUS
    "STD  Y+2, %1   \n\t"   // store c
    "LDD  R18, Z+21 \n\t"   // load head
    "CPSE R18, %2   \n\t"   // compare with new tail
    "RJMP end       \n\t"
    "LDD  R18, Y+5  \n\t"   // load CTRLA
    "ANDI R18, 0xDF \n\t"   // &= (~USART_DREIE_bm)
    "STD  Y+5, R18  \n\t"   // store to CTRLA
    "end:           \n\t"
    "STD  Z+22, %2  \n\t"   // store new tail to uartClass._tx_buffer_tail
    :
    : "m" (uartClass), "r" (c), "r" (txTail)
    : "r18", "r28", "r29", "r30", "r31"
    );
*/
}

// To invoke data empty "interrupt" via a call, use this method
void UartClass::_poll_tx_data_empty(void) {
  if ((!(SREG & CPU_I_bm)) ||  CPUINT.STATUS) {
    // 3/25/21
    // We're here because we're waiting for space in the buffer *or* we're in flush
    // and waiting for the last byte to leave, yet we're either in an ISR, or
    // interrupts are disabled so the ISR can't fire on it's own.
    //
    // Interrupts are disabled either globally or for data register empty,
    // or we are in another ISR. (It doesn't matter *which* ISR we are in
    // whether it's another level 0, the priority one, or heaven help us
    // the NMI, if the user code says to print something or flush the buffer
    // we might as well do it. It is entirely plausible that an NMI might
    // attempt to print out some sort of record of what happened. -Spence 10/23/20
    //
    // so we'll have to poll the "data register empty" flag ourselves.
    // If it is set, pretend an interrupt has happened and call the handler
    // to free up space for us.

    // Invoke interrupt handler only if conditions data register is empty
    if ((*_hwserial_module).STATUS & USART_DREIF_bm) {
      if (_tx_buffer_head != _tx_buffer_tail) {
        // Buffer empty, so disable "data register empty" interrupt
        (*_hwserial_module).CTRLA &= (~USART_DREIE_bm);
        return;
      }
      _tx_data_empty_irq(*this);
    }
  }
  // In case interrupts are enabled, the interrupt routine will be invoked by itself
}


// Public Methods //////////////////////////////////////////////////////////////

// Invoke this function before 'begin' to define the pins used
bool UartClass::pins(uint8_t tx, uint8_t rx) {
  uint8_t ret_val = _pins_to_swap(_usart_pins, _mux_count, tx, rx);   // return 127 when correct swap number wasn't found
  return swap(ret_val);
}

bool UartClass::swap(uint8_t state) {
  if (state < _mux_count) {
    _pin_set = state;
    return true;
  } else if (state == MUX_NONE) {  //128 codes for MUX_NONE
    _pin_set = _mux_count;
    return true;
  } else {
    _pin_set = 0;
  }
  return false;
}

void UartClass::begin(unsigned long baud, uint16_t options) {
  if (__builtin_constant_p(baud)) {
    if (baud > (F_CPU / 8)) badArg("Unachievable baud, too high - must be less than F_CPU/8");
    if (baud < (F_CPU / 16800)) badArg("Unachievable baud, too low - must be more than F_CPU/16800 (16384 plus allowable error)");
  }
  // Make sure no transmissions are ongoing and USART is disabled in case begin() is called by accident
  // without first calling end()
  if (_written) {
    this->end();
  }

  uint8_t ctrlb = 0xc0;
  if (baud > F_CPU / 16) {
    ctrlb |= 0x02;
    if (baud > F_CPU / 8) {
      baud = F_CPU/8;
    } else {
      baud >>= 1;
    };
  }
  uint16_t baud_setting = (((4 * F_CPU) / baud));
  uint8_t oldSREG = SREG;
  cli();
  (*_hwserial_module).CTRLB=0;
  (*_hwserial_module).BAUD = baud_setting;
  // Set USART mode of operation
  (*_hwserial_module).CTRLC = (uint8_t) options;
  uint8_t t = (uint8_t) (options >> 8);
  t &= 0x09;
  (*_hwserial_module).CTRLA |= (USART_RXCIE_bm | t);
  if (t & 0x20) {
    (*_hwserial_module).EVCTRL = 1;
  } else {
    (*_hwserial_module).EVCTRL = 0;
  }
  uint8_t ctlb = 0xC0;
  if (t & 0x04) {
    ctlb |= 0x08;
  }
  if (t & 0x10) {
    ctlb |= 0x02;
  }
  t &= 0xC0;
  ctlb ^= (0xC0 & t);
  _set_pins(_usart_pins, _mux_count, _pin_set, (ctlb & 0xC0));
  (*_hwserial_module).CTRLB = ctlb;
  SREG=oldSREG;
}

void UartClass::end() {
  // wait for transmission of outgoing data
  flush();

  // Disable receiver and transmitter as well as the RX complete and
  // data register empty interrupts.
  (*_hwserial_module).CTRLB &= ~(USART_RXEN_bm | USART_TXEN_bm);
  (*_hwserial_module).CTRLA &= ~(USART_RXCIE_bm | USART_DREIE_bm);

  // clear any received data
  _rx_buffer_head = _rx_buffer_tail;

  // Note: Does not change output pins
  _written = false;
}

int UartClass::available(void) {
  return ((unsigned int)(SERIAL_RX_BUFFER_SIZE + _rx_buffer_head - _rx_buffer_tail)) & (SERIAL_RX_BUFFER_SIZE-1);   //% SERIAL_RX_BUFFER_SIZE;
}

int UartClass::peek(void) {
  if (_rx_buffer_head == _rx_buffer_tail) {
    return -1;
  } else {
    return _rx_buffer[_rx_buffer_tail];
  }
}

int UartClass::read(void) {
  // if the head isn't ahead of the tail, we don't have any characters
  if (_rx_buffer_head == _rx_buffer_tail) {
    return -1;
  } else {
    unsigned char c = _rx_buffer[_rx_buffer_tail];
    _rx_buffer_tail = (rx_buffer_index_t)(_rx_buffer_tail + 1) & (SERIAL_RX_BUFFER_SIZE-1);   // % SERIAL_RX_BUFFER_SIZE;
    return c;
  }
}

int UartClass::availableForWrite(void) {
  tx_buffer_index_t head;
  tx_buffer_index_t tail;

  TX_BUFFER_ATOMIC {
    head = _tx_buffer_head;
    tail = _tx_buffer_tail;
  }
  if (head >= tail) {
    return SERIAL_TX_BUFFER_SIZE - 1 - head + tail;
  }
  return tail - head - 1;
}

void UartClass::flush() {
  // If we have never written a byte, no need to flush. This special
  // case is needed since there is no way to force the TXCIF (transmit
  // complete) bit to 1 during initialization
  if (!_written) {
    return;
  }

  // Check if we are inside an ISR already (e.g. connected to a different peripheral then UART), in which case the UART ISRs will not be called.
  // Spence 10/23/20: Changed _poll_tx_data_empty() to instead call the ISR directly in this case too
  // Why elevate the interrupt if we're going to go into a busywait loop checking if the interrupt is disabled and if so, check for the bit and
  // manually call the ISR if the bit is set... *anyway*? Plus, in write(), this mode will be enabled upon a write of a single character from an ISR
  // and will stay that way until the buffer is empty, which would mean that the fairly long and slow UART TX ISR would have priority over a
  // potentially very fast interrupt that the user may have set to priority level 1. Just because a whizz-bang feature is there doesn't mean
  // it's appropriate to use for applications where it has only very small benefits, and significant risk of surprising the user and causing
  // breakage of code that would otherwise work. Finally, the previous implementation didn't check if it was called from the current lvl1 ISR
  // and in that case flush(), and write() with full buffer would just straight up hang...

  // Spin until the data-register-empty-interrupt is disabled and TX complete interrupt flag is raised
  while (((*_hwserial_module).CTRLA & USART_DREIE_bm) || (!((*_hwserial_module).STATUS & USART_TXCIF_bm))) {
    // poll this, which will ensure that bytes keep getting sent even if interrupts are disabled or something.
    _poll_tx_data_empty();
  }
  // When we get here, nothing is queued anymore (DREIE is disabled) and
  // the hardware finished transmission (TXCIF is set).
}




void UartClass::_mux_set(uint8_t* mux_table_ptr, uint8_t mux_count, uint8_t mux_code) {
#if HWSERIAL_MUX_REG_COUNT > 1  // for big pincount devices that have more then one USART PORTMUX register
  uint8_t* mux_info_ptr = mux_table_ptr + (mux_count * USART_PINS_WIDTH) + 1;
  uint16_t mux_options_off_gm = pgm_read_word_near(mux_info_ptr);  /* pointer offset to the second columun
  at the end bottom row of the table, with info about the mux options, rather than a specefic option
  Low byte is the offset from USARTROUTEA, second byte is the group mask. */
  volatile uint8_t* portmux  = (uint8_t*)(HWSERIAL_MUX_REGISTER_BASE + (uint8_t)mux_options_off_gm); //offset
  uint8_t temp   = *portmux;
  temp          &= ~((uint8_t) (mux_options_off_gm >> 8)); //Group Mask
  temp          |= mux_code;
  *portmux       = temp;
#else
  uint8_t* mux_info_ptr = mux_table_ptr + (mux_count * USART_PINS_WIDTH) + 2;
  /* Only one register, so no offset, so only read a byte */
  uint8_t mux_mask = pgm_read_byte_near(mux_info_ptr);     // only read the group mask
  volatile uint8_t* portmux = (uint8_t*)(HWSERIAL_MUX_REGISTER_BASE);
  uint8_t temp   = *portmux;
  temp          &= ~(mux_mask);
  temp          |= mux_code;
  *portmux       = temp;
#endif
}


void UartClass::_set_pins(uint8_t* mux_table_ptr, uint8_t mux_count, uint8_t mux_setting, uint8_t enmask) {
  uint8_t* mux_row_ptr = mux_table_ptr + (mux_setting * USART_PINS_WIDTH);
  uint16_t mux_row_gc_tx = pgm_read_word_near(mux_row_ptr); // Clever trick for faster PGM reads of consecutive bytes!
  uint8_t mux_group_code = (uint8_t) (mux_row_gc_tx); // this is the mux
  if (mux_setting < mux_count) {  // if false, pinmux none was selected
    uint8_t mux_pin_tx =   (uint8_t) (mux_row_gc_tx >> 8);
    if (enmask & 0x80) {
      pinMode(mux_pin_tx++, OUTPUT);
    }
    if (enmask & 0x40){
      pinMode(mux_pin_tx, INPUT_PULLUP);
    }
  }
  _mux_set(mux_table_ptr, mux_count, mux_group_code);
}


uint8_t UartClass::_pins_to_swap(uint8_t* mux_table_ptr, uint8_t mux_count, uint8_t tx_pin, uint8_t rx_pin) {
  if (tx_pin == NOT_A_PIN && rx_pin == NOT_A_PIN) {
    return  128;            // get MUX_NONE
  } else {
    rx_pin -=tx_pin;        // Test if thes *could* match eachother.
    if(rx_pin == 1) {       // thus far no parts where the pins are not adjacent! */
      mux_table_ptr++;                                              // prepare to read the information with one byte offset
      for (uint8_t i = 0; i < mux_count; i++) {                     // until we hit the end of this USART's mux...
        uint8_t mux_tx_pin = pgm_read_byte_near(mux_table_ptr);     // read tx pin, which we are now pointing at.
        if (tx_pin == mux_tx_pin) { //if it's the tx pin, and we know that the rx pin matches it.
          return i;                 // return the swap number. The first line checked that the rx pin's must match this, since rx pins on all parts thus far are tx + 1.
        }
        mux_table_ptr += USART_PINS_WIDTH; //otherwise try the next mux option
      }
    }
    return NOT_A_MUX; // At this point, we have check all group codes for this peripheral. It aint there. return NOT_A_MUX.
  }
}



size_t UartClass::write(uint8_t c) {
  _written = true;

  // If the buffer and the data register is empty, just write the byte
  // to the data register and be done. This shortcut helps
  // significantly improve the effective data rate at high (>
  // 500kbit/s) bit rates, where interrupt overhead becomes a slowdown.
  if ((_tx_buffer_head == _tx_buffer_tail) && ((*_hwserial_module).STATUS & USART_DREIF_bm)) {
    (*_hwserial_module).STATUS = USART_TXCIF_bm;
    // MUST clear TXCIF **before** writing new char, otherwise ill-timed interrupt can cause it to erase the flag after the new charchter has been sent!
    (*_hwserial_module).TXDATAL = c;


/*   * I cannot figure out *HOW* the DRE could be enabled at this point (buffer empty and DRE flag up)
     * When the buffer was emptied, it would have turned off the DREI after it loaded the last byte.
     * Thus, the only possible way this could happen is if an interrupt also tried to write to serial,
     * *immediately* after we checked that the buffer was empty, before we made it not empty. And
     * in that case, without this line it would lose one of the characters... with that line, it could
     * stop servicing DRE until another serial write, AND lose a character. I think it's a full 10 bytes
     * 2 to read with LDS, 1 to modify, 2 to write back with STS. It's gone!    -Spence 4/2021
     * Original comments:
     // Make sure data register empty interrupt is disabled to avoid
     // that the interrupt handler is called in this situation
     (*_hwserial_module).CTRLA &= (~USART_DREIE_bm);
*/

    return 1;
  }

  tx_buffer_index_t i = (_tx_buffer_head + 1) & (SERIAL_TX_BUFFER_SIZE-1);  // % SERIAL_TX_BUFFER_SIZE;

  // If the output buffer is full, there's nothing for it other than to
  // wait for the interrupt handler to empty it a bit (or emulate interrupts)
  VPORTA.OUT &= ~0x80;
  while (i == _tx_buffer_tail) {
    _poll_tx_data_empty();
  }
  VPORTA.OUT |= 0x80;
  _tx_buffer[_tx_buffer_head] = c;
  _tx_buffer_head = i;

  // Enable data "register empty interrupt"
  (*_hwserial_module).CTRLA |= USART_DREIE_bm;

  return 1;
}

void UartClass::printHex(const uint8_t b) {
  char x = (b >> 4) | '0';
  if (x > '9')
    x += 7;
  write(x);
  x = (b & 0x0F) | '0';
  if (x > '9')
    x += 7;
  write(x);
}

void UartClass::printHex(const uint16_t w, bool swaporder) {
  uint8_t *ptr = (uint8_t *) &w;
  if (swaporder) {
    printHex(*(ptr++));
    printHex(*(ptr));
  } else {
    printHex(*(ptr + 1));
    printHex(*(ptr));
  }
}

void UartClass::printHex(const uint32_t l, bool swaporder) {
  uint8_t *ptr = (uint8_t *) &l;
  if (swaporder) {
    printHex(*(ptr++));
    printHex(*(ptr++));
    printHex(*(ptr++));
    printHex(*(ptr));
  } else {
    ptr+=3;
    printHex(*(ptr--));
    printHex(*(ptr--));
    printHex(*(ptr--));
    printHex(*(ptr));
  }
}

uint8_t * UartClass::printHex(uint8_t* p, uint8_t len, char sep) {
  for (byte i = 0; i < len; i++) {
    if (sep && i) write(sep);
    printHex(*p++);
  }
  println();
  return p;
}

uint16_t * UartClass::printHex(uint16_t* p, uint8_t len, char sep, bool swaporder) {
  for (byte i = 0; i < len; i++) {
    if (sep && i) write(sep);
    printHex(*p++, swaporder);
  }
  println();
  return p;
}

/* at minimum, this stops warnings, but I think there are corner cases when the non-volatile implementations gives wrong results when passed pointers to hardware registers.*/
volatile uint8_t * UartClass::printHex(volatile uint8_t* p, uint8_t len, char sep) {
  for (byte i = 0; i < len; i++) {
    if (sep && i) write(sep);
    uint8_t t = *p++;
    printHex(t);
  }
  println();
  return p;
}

volatile uint16_t * UartClass::printHex(volatile uint16_t* p, uint8_t len, char sep, bool swaporder) {
  for (byte i = 0; i < len; i++) {
    if (sep && i) write(sep);
    uint16_t t = *p++;
    printHex(t, swaporder);
  }
  println();
  return p;
}
#endif
