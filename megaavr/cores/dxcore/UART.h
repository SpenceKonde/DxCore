/* UART.h - Hardware serial library, main header.
 * This library is free software released under LGPL 2.1.
 * See License.md for more information.
 * This file is part of megaTinyCore.
 *
 * Copyright (c) 2006 Nicholas Zambetti, Modified by
 * 11/23/2006 David A. Mellis, 9/20/2010 Mark Sproul,
 * 8/24/2012 Alarus, 12/3/2013 Matthijs Kooijman
 * Others (unknown) 2013-2017, 2017-2021 Spence Konde
 * and 2021 MX682X
 *
 * Modified 28 September 2010 by Mark Sproul
 * Modified 14 August 2012 by Alarus
 * Modified 3 December 2013 by Matthijs Kooijman
 * Modified by SOMEONE around 2016-2017; hardware seriel didn't port itself to the megaAVR 0-series.
 * Modified 2017-2021 by Spence Konde for megaTinyCore and DxCore.
 * Modified late 2021 by Spence Konde and MX682X for DxCore
 * 12/26/21: Correct bug introduced in my ASM macros. -Spence
 * 12/30/21: Clean up tests for conditional compilation
             tests for defined(USE_ASM_*) removed UART.h tests that test both defined and the value of
             USE_ASM_* macros. We check if they're defined and define them if they're not defined already
             so whenever UART.h has been loaded, those three macros are defined as either 1, or wharever
             value the user overode them with, likely 0. Also high byte of UART address always 0x08, so replace
             2-clock ldd with 1 clock ldi. - Spence
*/

#pragma once

#include <inttypes.h>
#include "api/HardwareSerial.h"
#include "pins_arduino.h"
#include "UART_swap.h"

/* Define constants and variables for buffering incoming serial data.  We're
 * using a ring buffer in which head is the index of the location to which
 * to write the next incoming character and tail is the index of the
 * location from which to read.
 * NOTE: a "power of 2" buffer size is **REQUIRED** - the compiler
 * was missing optimizations, and there's no particular reason to have
 * a weird sized buffer. The argument was at least plausible on the
 * tinyAVRs, though the impetus for not allowing it much stronger.
 * Here you could have 256b buffers on most parts without noticing.
 * More than 256b on TX imposes a considerable performance penalty.
 * The atomic block block is costly - it's a macro for cli and sei
 * implemented in inline assembly, which sounds fast. But the optimizer
 * can reorder instructions *and isn't smart enough not to here* without the
 *  ": memory" clobber to create a memory barrier. This ensures that it
 * is atomic, but significantly hurts performance. (theoretical worst case
 * is 94 clocks, real-world is usually far less, but I'll only say "less"
 * The functions in question have considerable register pressure).
 *
 * The USE_ASM_* options can be disabled by defining them as 0 either in variant pins_arduino.h
 * The buffer sizes can be overridden in by defining SERIAL_TX_BUFFER either in variant file (
 * as defines in pins_arduino.h) or boards.txt as (By passing them as extra flags).
 * Note that buffer sizes must be powers of 2 only no matter how you override it.
 * The alternative is doing division to get the modulo every time instead of a single clock bitwise and, which further lowers the
 * maximum RX speed (which, without the ASM implementation, is already lower than what the receiver is capable of).
 * The defaults below are only used if the relevant macro isn't already defined.
 * Since the USE_ASM_* = 1 option is apparently working, we do not recommend disabling it, as it will waste flash and hurt performance.
 */
#if !defined(USE_ASM_TXC)
  #define USE_ASM_TXC 1    // This *appears* to work? It's the easy one. saves 6b for 1 USART, 50 for 2.
#endif

#if !defined(USE_ASM_RXC)
  #define USE_ASM_RXC 1    // This now works. Saves only 4b for 1 usart but 102 for 2.
#endif

#if !defined(USE_ASM_DRE)
  #define USE_ASM_DRE 1      // This is the hard one...Depends on BOTH buffers, and has that other method of calling it. saves 34b for 1 USART 102 for 2
#endif
#if !defined(SERIAL_TX_BUFFER_SIZE)   // could be overridden by boards.txt
  #if   (INTERNAL_SRAM_SIZE  < 1024)  // 128/256b/512b RAM
    #define SERIAL_TX_BUFFER_SIZE 16
  #elif (INTERNAL_SRAM_SIZE < 2048)   // 1k RAM
    #define SERIAL_TX_BUFFER_SIZE 32
  #else
    #define SERIAL_TX_BUFFER_SIZE 64  // 2k/3k RAM
  #endif
#endif
#if !defined(SERIAL_RX_BUFFER_SIZE)   // could be overridden by boards.txt
  #if   (INTERNAL_SRAM_SIZE <  512)  // 128/256b RAM
    #define SERIAL_RX_BUFFER_SIZE 16
    // current tx buffer position = SerialClass + txtail + 37
  #elif (INTERNAL_SRAM_SIZE < 1024)  // 512b RAM
    #define SERIAL_RX_BUFFER_SIZE 32
    // current tx buffer position = SerialClass + txtail + 53
  #else
    #define SERIAL_RX_BUFFER_SIZE 64  // 1k+ RAM
    // current tx buffer position = SerialClass + txtail + 85
    // rx buffer position always = SerialClass + rxhead + 21
  #endif
#endif
/* Use INTERNAL_SRAM_SIZE instead of RAMEND - RAMSTART, which is vulnerable to
 * a fencepost error. */
#if (SERIAL_TX_BUFFER_SIZE > 256)
  typedef uint16_t tx_buffer_index_t;
#else
  typedef uint8_t  tx_buffer_index_t;
#endif
// I am not convinced > 256b is safe for the RX buffer....
#if (SERIAL_RX_BUFFER_SIZE > 256)
  typedef uint16_t rx_buffer_index_t;
#else
  typedef uint8_t  rx_buffer_index_t;
#endif
// As noted above, forcing the sizes to be a power of two saves a small
// amount of flash, and there's no compelling reason to NOT have them be
// a power of two, while the size penalty is not negligible, and the speed penalty significant.
#if (SERIAL_TX_BUFFER_SIZE & (SERIAL_TX_BUFFER_SIZE - 1))
  #error "ERROR: TX buffer size must be a power of two."
#endif
#if (SERIAL_RX_BUFFER_SIZE & (SERIAL_RX_BUFFER_SIZE - 1))
  #error "ERROR: RX buffer size must be a power of two."
#endif

#if USE_ASM_RXC == 1 && !(SERIAL_RX_BUFFER_SIZE == 256 || SERIAL_RX_BUFFER_SIZE == 128 || SERIAL_RX_BUFFER_SIZE == 64 || SERIAL_RX_BUFFER_SIZE == 32 || SERIAL_RX_BUFFER_SIZE == 16)
  #error "Assembly RX Complete (RXC) ISR is only supported when RX buffer size are 256, 128, 64, 32 or 16 bytes"
#endif

#if USE_ASM_DRE == 1 && !((SERIAL_RX_BUFFER_SIZE == 256 || SERIAL_RX_BUFFER_SIZE == 128 || SERIAL_RX_BUFFER_SIZE == 64 || SERIAL_RX_BUFFER_SIZE == 32 || SERIAL_RX_BUFFER_SIZE == 16) && \
                          (SERIAL_TX_BUFFER_SIZE == 256 || SERIAL_TX_BUFFER_SIZE == 128 || SERIAL_TX_BUFFER_SIZE == 64 || SERIAL_TX_BUFFER_SIZE == 32 || SERIAL_TX_BUFFER_SIZE == 16))
  #error "Assembly Data Register Empty (DRE) ISR is only supported when both TX and RX buffer sizes are 256, 128, 64, 32 or 16 bytes"
#endif


/* Macros to help the rare few who want sync or MSPI mode */
#define syncBegin(port, baud, config, syncopts) ({\
  if ((config & 0xC0) == 0x40)                    \
    {pinConfigure(port.getPin(2), syncopts);      \
    port.begin(baud >> 3, config);                \
  }})

#define mspiBegin(port, baud, config, invert) ({  \
  if ((config & 0xC0) == 0xC0) {                  \
    pinConfigure(port.getPin(2), invert);         \
    port.begin(baud >> 3, config);                \
  }})


class UartClass : public HardwareSerial {
/* DANGER DANGER DANGER
 * CHANGING THE MEMBER VARIABLES BETWEEN HERE AND THE OTHER SCARY COMMENT WILL COMPLETELY BREAK SERIAL
 * WHEN USE_ASM_DRE or USE_ASM_RXC is used!
 * DANGER DANGER DANGER */
 protected:
    volatile USART_t * _hwserial_module;  // pointer to the USART module, needed to access the correct registers.
    uint8_t * _usart_pins;   // pointer to the pin set, in PROGMEM
    uint8_t _mux_count;     // maximum MUX
    uint8_t _pin_set;       // the active pin set for setting the correct pins for I/O
    uint8_t _state = 0; // LSB = _written. Second bit = half duplex (LBME) and special handling needed.
    volatile rx_buffer_index_t _rx_buffer_head;
    volatile rx_buffer_index_t _rx_buffer_tail;
    volatile tx_buffer_index_t _tx_buffer_head;
    volatile tx_buffer_index_t _tx_buffer_tail;
    // Don't put any members after these buffers, since only the first
    // 32 bytes of this struct can be accessed quickly using the ldd
    // instruction.
    volatile uint8_t _rx_buffer[SERIAL_RX_BUFFER_SIZE];
    volatile uint8_t _tx_buffer[SERIAL_TX_BUFFER_SIZE];
  /* DANGER DANGER DANGER
   * ANY CHANGES BETWEEN OTHER SCARY COMMENT AND THIS ONE WILL BREAK SERIAL when USE_ASM_DRE or USE_ASM_RXC is used!
   * DANGER DANGER DANGER */
 public:
    inline UartClass(volatile USART_t *hwserial_module, uint8_t *usart_pins, uint8_t mux_count, uint8_t mux_default);
    void                   begin(unsigned long   baud) {begin(baud, SERIAL_8N1);}
    void                   begin(unsigned long   baud, uint16_t options                     );
    void                     end(                                                           );
    bool                    pins(uint8_t           tx, uint8_t rx                           );
    bool                    swap(uint8_t        state                                       );
    void                printHex(const uint8_t      b                                       );
    void                printHex(const uint16_t     w, bool s = 0                           );
    void                printHex(const uint32_t     l, bool s = 0                           );
    void                printHex(const int16_t      w, bool s = 0)  {printHex((uint16_t)w, s);}
    void                printHex(const int32_t      l, bool s = 0)  {printHex((uint32_t)l, s);}
    void                printHex(const int8_t       b)              {printHex((uint8_t) b   );}
    void                printHex(const char         b)              {printHex((uint8_t) b   );}
    void              printHexln(const uint8_t      b)              {printHex(          b   ); println();}
    void              printHexln(const uint16_t     w, bool s = 0)  {printHex(          w, s); println();}
    void              printHexln(const uint32_t     l, bool s = 0)  {printHex(          l, s); println();}
    void              printHexln(const int8_t       b)              {printHex((uint8_t) b   ); println();}
    void              printHexln(const char         b)              {printHex((uint8_t) b   ); println();}
    void              printHexln(const int16_t      w, bool s = 0)  {printHex((uint16_t)w, s); println();}
    void              printHexln(const int32_t      l, bool s = 0)  {printHex((uint32_t)l, s); println();}
    uint8_t *           printHex(         uint8_t * p, uint8_t len, char sep = 0            );
    uint16_t *          printHex(         uint16_t* p, uint8_t len, char sep = 0, bool s = 0);
    volatile uint8_t *  printHex(volatile uint8_t * p, uint8_t len, char sep = 0            );
    volatile uint16_t * printHex(volatile uint16_t* p, uint8_t len, char sep = 0, bool s = 0);
    virtual int        available(void                                                       );
    virtual int availableForWrite(void                                                      );
    virtual int             peek(void                                                       );
    virtual int             read(void                                                       );
    virtual void           flush(void                                                       );
    virtual size_t         write(uint8_t b                                                  );
    inline  size_t         write(unsigned long n) {return                   write((uint8_t)n);}
    inline  size_t         write(long          n) {return                   write((uint8_t)n);}
    inline  size_t         write(unsigned int  n) {return                   write((uint8_t)n);}
    inline  size_t         write(int           n) {return                   write((uint8_t)n);}
    using Print::write;   // pull in write(str) and write(buf, size) from Print
    explicit operator       bool()                {return                                true;}
    uint8_t               getPin(uint8_t pin                                                );

    // Interrupt handlers - Not intended to be called externally
    #if !(defined(USE_ASM_RXC) && USE_ASM_RXC == 1 && (SERIAL_RX_BUFFER_SIZE == 128 || SERIAL_RX_BUFFER_SIZE == 64 || SERIAL_RX_BUFFER_SIZE == 32 || SERIAL_RX_BUFFER_SIZE == 16))
      static void _rx_complete_irq(UartClass& uartClass);
    #endif
    #if !(defined(USE_ASM_DRE) && USE_ASM_DRE == 1 && \
         (SERIAL_RX_BUFFER_SIZE == 128 || SERIAL_RX_BUFFER_SIZE == 64 || SERIAL_RX_BUFFER_SIZE == 32 || SERIAL_RX_BUFFER_SIZE == 16) && \
         (SERIAL_TX_BUFFER_SIZE == 128 || SERIAL_TX_BUFFER_SIZE == 64 || SERIAL_TX_BUFFER_SIZE == 32 || SERIAL_TX_BUFFER_SIZE == 16))
      static void _tx_data_empty_irq(UartClass& uartClass);
    #endif

 private:
    void _poll_tx_data_empty(void);
    /* These all concern pin set handling */
    static void        _set_pins(uint8_t* pinInfo, uint8_t mux_count, uint8_t mux_setting,  uint8_t enmask);
    static void         _mux_set(uint8_t* pinInfo, uint8_t mux_count, uint8_t mux_code                    );
    static uint8_t _pins_to_swap(uint8_t* pinInfo, uint8_t mux_count, uint8_t tx_pin,       uint8_t rx_pin);
    static uint8_t       _getPin(uint8_t* pinInfo, uint8_t mux_count, uint8_t mux_setting,  uint8_t pin);
};

#if defined(USART0)
  extern UartClass Serial0;
#endif
#if defined(USART1)
  extern UartClass Serial1;
#endif
#if defined(USART2)
  extern UartClass Serial2;
#endif
#if defined(USART3)
  extern UartClass Serial3;
#endif
#if defined(USART4)
  extern UartClass Serial4;
#endif
#if defined(USART5)
  extern UartClass Serial5;
#endif
