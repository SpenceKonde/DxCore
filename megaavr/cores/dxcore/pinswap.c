#include "Arduino.h"

/* Now for HARDWARE SERIAL which is the most important - also the trickiest; we need to totally rearchitect how the serial classes handle pins
 * Interestingly, only 0 and 1 have had any additional mappings declared. The EA doesn't have any new ones for 2 and 3 I guess they didn't need it there
 * but once they'd designed a MUXer, they might as well use it? Compatibility lockin? Hmmmm.... */
#if defined(USART5)
  #define NUM_HWSERIAL_PORTS 6
#elif defined(USART4)
  #define NUM_HWSERIAL_PORTS 5
#elif defined(USART3)
  #define NUM_HWSERIAL_PORTS 4
#elif defined(USART2)
  #define NUM_HWSERIAL_PORTS 3
#elif defined(USART1)
  #define NUM_HWSERIAL_PORTS 2
#elif defined(USART0)
  #define NUM_HWSERIAL_PORTS 1
  #warning "Only one USART detected, all supported parts have at least 2, something is wrong!"
#else
  #error "No USARTs detected, all supported parts have at least 2, something is wrong!"
#endif

/*

#define HWSERIAL0_MUX                   PORTMUX_USART0_DEFAULT_gc
#define HWSERIAL0_MUX_PINSWAP_1         PORTMUX_USART0_ALT1_gc
#define HWSERIAL0_MUX_PINSWAP_2         PORTMUX_USART0_ALT2_gc
#define HWSERIAL0_MUX_PINSWAP_3         PORTMUX_USART0_ALT3_gc
#define HWSERIAL0_MUX_PINSWAP_4         PORTMUX_USART0_ALT4_gc

*/

/*  Group mask, routereg, pinful mux count, mux_none_gc
 * The #ifdef construction makes it absolutely obvious to someone reading this that it won't
 * work if there's a missing USART in the middle, eg. if they ever released something with a
 * USART0 and USART2 but no USART1.                                                       */

const uint8_t _usart_ports[][4] PROGMEM = {
  {PORTMUX_USART0_gm, 0, 2, PORTMUX_USART0_NONE_gc},
  #if defined(USART1)
    {PORTMUX_USART1_gm, 0, 2, PORTMUX_USART1_NONE_gc},
    #if defined(USART2)
      {PORTMUX_USART2_gm, 0, 2, PORTMUX_USART2_NONE_gc},
      #if defined(USART3)
        #if (PORTMUX_USART0_gm == 7)
          {PORTMUX_USART3_gm, 1, 2, PORTMUX_USART3_NONE_gc},
          // I mean, they'd have to move it to the next register right?
        #else
          {PORTMUX_USART3_gm, 0, 2, PORTMUX_USART3_NONE_gc},
        #endif
        #if defined(USART4)
          {PORTMUX_USART4_gm, 1, 2, PORTMUX_USART4_NONE_gc},
          #if defined(USART5)
            {PORTMUX_USART5_gm, 1, 2, PORTMUX_USART5_NONE_gc},
          #endif
        #endif
      #endif
    #endif
  #endif
};



const usart_pinset usart2_pins[][5] PROGMEM = {
  {HWSERIAL0_MUX,              PIN_HWSERIAL1_TX,             PIN_HWSERIAL1_RX,             PIN_HWSERIAL1_XCK,            PIN_HWSERIAL1_XDIR},
  #ifdef HWSERIAL0_MUX_PINSWAP_1
    {HWSERIAL0_MUX_PINSWAP_1,    PIN_HWSERIAL1_TX_PINSWAP_1,   PIN_HWSERIAL1_RX_PINSWAP_1,   PIN_HWSERIAL1_XCK_PINSWAP_1,  PIN_HWSERIAL1_XDIR_PINSWAP_1},
  #endif
};

const usart_pinset usart2_pins[2] PROGMEM = {
  {PORTMUX_USART2_DEFAULT_gc, PIN_PF0,   PIN_PF1,   PIN_PF2,   PIN_PF3},
  {PORTMUX_USART2_ALT1_gc,    PIN_PF4,   PIN_PF4,   NOT_A_PIN, NOT_A_PIN},
};

const usart_pinset usart2_pins[2] PROGMEM = {
  {PORTMUX_USART2_DEFAULT_gc, PIN_PF0,   PIN_PF1,   PIN_PF2,   PIN_PF3},
  {PORTMUX_USART2_ALT1_gc,    PIN_PF4,   PIN_PF4,   NOT_A_PIN, NOT_A_PIN},
};

const usart_pinset usart2_pins[2] PROGMEM = {
  {PORTMUX_USART2_DEFAULT_gc, PIN_PF0,   PIN_PF1,   PIN_PF2,   PIN_PF3},
  {PORTMUX_USART2_ALT1_gc,    PIN_PF4,   PIN_PF4,   NOT_A_PIN, NOT_A_PIN},
};

const usart_pinset usart2_pins[2] PROGMEM = {
  {PORTMUX_USART2_DEFAULT_gc, PIN_PF0,   PIN_PF1,   PIN_PF2,   PIN_PF3},
  {PORTMUX_USART2_ALT1_gc,    PIN_PF4,   PIN_PF4,   NOT_A_PIN, NOT_A_PIN},
};

const usart_pinset usart2_pins[2] PROGMEM = {
  {PORTMUX_USART2_DEFAULT_gc, PIN_PF0,   PIN_PF1,   PIN_PF2,   PIN_PF3},
  {PORTMUX_USART2_ALT1_gc,    PIN_PF4,   PIN_PF4,   NOT_A_PIN, NOT_A_PIN},
};

void write_hwserial_mux(uint8_t port_num, uint8_t muxlevel);

bool hwserial_mux(uint8_t port_num, uint8_t  muxlevel, bool enact) {
  if (!(port_num < NUM_HWSERIAL_PORTS))           return false;
  if (muxlevel == MUX_NONE) {
    if (enact) write_hwserial_mux(port_num, pgm_read_byte_nearusart_ports[port_num][3]); // get the actual group code and pass to write function.
    return true;
  }
  if (muxlevel < usart_ports[port_num].mux_count) { // Serialn.swap(muxlevel) calls this with muxlevel or MUX_NONE
    if (enact) write_hwserial_mux(port_num,pgm_read_byte_near(usart_ports[port_num].pins[muxlevel][MUX_VALUE])); //again we need the group code, which is different from mux level (bitshifted)
    return true;
  }
  return false;
}

void write_hwserial_mux(uint8_t port_num, uint8_t mux_value) {
  volatile uint8_t* portmux = (volatile uint8_t *) usart_ports[port_num].muxreg;
  uint8_t temp   = *portmux;
  temp          &= ~usart_ports[port_num].mux_gm;
  temp          |= mux_value;
  *portmux       = temp;
}

uint8_t mux_from_pins(uint8_t port_num, uint8_t tx, uint8_t rx) {
  if (!(port_num < NUM_HWSERIAL_PORTS)) return NOT_A_PORT;
  if (tx == NOT_A_PIN && rx == NOT_A_PIN) {
    return usart_ports[port_num].mux_none;
  } else {
    for (uint8_t i = 0; i < usart_ports[port_num].mux_count; i++) {
      if ((tx == NOT_A_PIN || tx == pgm_read_byte_near(usart_ports[port_num].pins[i][PIN_TX])) && (rx == NOT_A_PIN || rx == pgm_read_byte_near(usart_ports[port_num].pins[i][PIN_RX]))) {
        return i;
      }
    }
    return NOT_A_MUX;
  }
}

uint8_t getPin(uint8_t port_num, MUX_HWSERIAL_t muxlevel, PIN_HWSERIAL_t pin) {
  if (!(port_num < NUM_HWSERIAL_PORTS)) return NOT_A_PORT;
  if (muxlevel < usart_ports[port_num].mux_count) return pgm_read_byte_near(usart_ports[port_num].pins[muxlevel][pin]);
  return NOT_A_PIN;
}
