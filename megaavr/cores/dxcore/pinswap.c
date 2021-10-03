#include "Arduino.h"

/* Now for HARDWARE SERIAL which is the most important - also the trickiest; we need to totally rearchitect how the serial classes handle pins
 * Interestingly, only 0 and 1 have had any additional mappings declared!
 * The objective here is to GET HardwareSerial out of the business of handling pins so that the info we need lives in progmem
 * and can be used without shipping a whole bunch of constants into the class, to burn ram and make the constructor several lines long just to list the arguments....
 * We expect pins_arduino to have the following for each port and mux option :
 *
 * #define HWSERIAL0                       &USART0
 * #define HWSERIAL0_DRE_VECTOR            USART0_DRE_vect
 * #define HWSERIAL0_DRE_VECTOR_NUM        USART0_DRE_vect_num      <----- Was this only used for the serial interrupt elevation idiocy?
 * #define HWSERIAL0_RXC_VECTOR            USART0_RXC_vect
 * #define HWSERIAL0_MUX                   PORTMUX_USART0_DEFAULT_gc  <--- Mandatory
 * #define HWSERIAL0_MUX_PINSWAP_1         PORTMUX_USART0_ALT1_gc     <--- Mandatory if there are any mux options after this one
 * #define HWSERIAL0_MUX_PINSWAP_2         PORTMUX_USART0_ALT2_gc     <--- If neither this option nor further ones with pins exist, omit all references to it.
 * #define HWSERIAL0_MUX_PINSWAP_NONE      PORTMUX_USART0_NONE_gc     <--- Mandatory
 * #define PIN_HWSERIAL0_TX                PIN_PA0
 * #define PIN_HWSERIAL0_RX                PIN_PA1
 * #define PIN_HWSERIAL0_XCK               PIN_PA2
 * #define PIN_HWSERIAL0_XDIR              PIN_PA3
 * #define PIN_HWSERIAL0_TX_PINSWAP_1      NOT_A_PIN  <---- If any one of them is missing, or both RX and TX are
 * #define PIN_HWSERIAL0_RX_PINSWAP_1      NOT_A_PIN  <---- NOT_A_PIN, it's an invalid mux option, and won't be available
 * #define PIN_HWSERIAL0_XCK_PINSWAP_1     NOT_A_PIN  <---- invalid options may omit these, but must #define the mux option as noted above.
 * #define PIN_HWSERIAL0_XDIR_PINSWAP_1    NOT_A_PIN  <----
 * #define PIN_HWSERIAL0_TX_PINSWAP_2      PIN_PA4    <---- ALL FOUR must be present (even if NOT_A_PIN)
 * #define PIN_HWSERIAL0_RX_PINSWAP_2      PIN_PA5    <---- And one of TX or RX must be a pin, not NOT_A_PIN.
 * #define PIN_HWSERIAL0_XCK_PINSWAP_2     NOT_A_PIN  <---- Otherwise it will be treated as invalid.
 * #define PIN_HWSERIAL0_XDIR_PINSWAP_2    NOT_A_PIN  <----
 *
 * Thus far, with the exception of USART1, the rest of the serial ports have been entirely excluded
 * from USART0's DD-day mux party... and all he got to do was pick up crumbs and lick icecubes after USART0 snarfed the cake and drained the punchbowl.
 * what will be interesting is to see whether this trend continues (which existing data suggests) or whether that mention of
 * TWI1 wasn't the only error in the EA's multiplexing considerations table
 *
 * None of the code below is used.... YET.
 */



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
\




/* This heinous mes does two things:
 * 1. It builds a table containing the pins and portmux settings for all the options defined in pinx_arduino.h
 * 2. It counts up the mux options that exist - if they are defined but NOT_A_MUX (indicating gaps in table), then the row of the table is filled with NOT_A_PIN.
 *    This should be done if (and only if) subsequent options do have pins. For example, on DA/DB with 28/32 pins, USART1 has only 1 available mux option with pins
 *    HWSERIAL1_MUX_PINSWAP1 should not be defined, and the table created will have a single row.
 */
/* The king, USART0 is the most flexible of the hardware serial ports on newer devices, with an extra bit in the PORTMUX allowing up to 7 pinful mux options. */
#if defined(USART0)
  const uint8_t _usart0_pins[][5] PROGMEM = {
  #if (defined(HWSERIAL0_MUX))
    #if (defined(PIN_HWSERIAL0_TX) && defined(PIN_HWSERIAL0_RX) && defined(PIN_HWSERIAL0_XCK) && defined(PIN_HWSERIAL0_XDIR) && (PIN_HWSERIAL0_TX != NOT_A_PIN || PIN_HWSERIAL0_TX!= NOT_A_PIN))
      {HWSERIAL0_MUX, PIN_HWSERIAL0_TX, PIN_HWSERIAL0_RX, PIN_HWSERIAL0_XCK, PIN_HWSERIAL0_XDIR}
    #else
      {HWSERIAL0_MUX, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN}
    #endif
    #if defined(HWSERIAL0_MUX_PINSWAP1)
      #if (defined(PIN_HWSERIAL0_TX_PINSWAP1) && defined(PIN_HWSERIAL0_RX_PINSWAP1) && defined(PIN_HWSERIAL0_XCK_PINSWAP1) && defined(PIN_HWSERIAL0_XDIR_PINSWAP1) && (PIN_HWSERIAL0_TX_PINSWAP1 != NOT_A_PIN || PIN_HWSERIAL0_TX_PINSWAP1!= NOT_A_PIN))
        ,{HWSERIAL0_MUX_PINSWAP1, PIN_HWSERIAL0_TX_PINSWAP1, PIN_HWSERIAL0_RX_PINSWAP1, PIN_HWSERIAL0_XCK_PINSWAP1, PIN_HWSERIAL0_XDIR_PINSWAP1}
      #else
        ,{HWSERIAL0_MUX_PINSWAP1, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN}
      #endif
      #if defined(HWSERIAL0_MUX_PINSWAP2)
        #if (defined(PIN_HWSERIAL0_TX_PINSWAP2) && defined(PIN_HWSERIAL0_RX_PINSWAP2) && defined(PIN_HWSERIAL0_XCK_PINSWAP2) && defined(PIN_HWSERIAL0_XDIR_PINSWAP2) && (PIN_HWSERIAL0_TX_PINSWAP2 != NOT_A_PIN || PIN_HWSERIAL0_TX_PINSWAP2!= NOT_A_PIN))
          ,{HWSERIAL0_MUX_PINSWAP2, PIN_HWSERIAL0_TX_PINSWAP2, PIN_HWSERIAL0_RX_PINSWAP2, PIN_HWSERIAL0_XCK_PINSWAP2, PIN_HWSERIAL0_XDIR_PINSWAP2}
        #else
          ,{HWSERIAL0_MUX_PINSWAP2, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN}
        #endif
        #if defined(HWSERIAL0_MUX_PINSWAP3)
          #if (defined(PIN_HWSERIAL0_TX_PINSWAP3) && defined(PIN_HWSERIAL0_RX_PINSWAP3) && defined(PIN_HWSERIAL0_XCK_PINSWAP3) && defined(PIN_HWSERIAL0_XDIR_PINSWAP3) && (PIN_HWSERIAL0_TX_PINSWAP3 != NOT_A_PIN || PIN_HWSERIAL0_TX_PINSWAP3!= NOT_A_PIN))
            ,{HWSERIAL0_MUX_PINSWAP3, PIN_HWSERIAL0_TX_PINSWAP3, PIN_HWSERIAL0_RX_PINSWAP3, PIN_HWSERIAL0_XCK_PINSWAP3, PIN_HWSERIAL0_XDIR_PINSWAP3}
          #else
            ,{HWSERIAL0_MUX_PINSWAP3, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN}
          #endif
          #if defined(HWSERIAL0_MUX_PINSWAP4)
            #if (defined(PIN_HWSERIAL0_TX_PINSWAP4) && defined(PIN_HWSERIAL0_RX_PINSWAP4) && defined(PIN_HWSERIAL0_XCK_PINSWAP4) && defined(PIN_HWSERIAL0_XDIR_PINSWAP4) && (PIN_HWSERIAL0_TX_PINSWAP4 != NOT_A_PIN || PIN_HWSERIAL0_TX_PINSWAP4!= NOT_A_PIN))
              ,{HWSERIAL0_MUX_PINSWAP4, PIN_HWSERIAL0_TX_PINSWAP4, PIN_HWSERIAL0_RX_PINSWAP4, PIN_HWSERIAL0_XCK_PINSWAP4, PIN_HWSERIAL0_XDIR_PINSWAP4}
            #else
              ,{HWSERIAL0_MUX_PINSWAP4, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN}
            #endif
            #if defined(HWSERIAL0_MUX_PINSWAP5)
              #if (defined(PIN_HWSERIAL0_TX_PINSWAP5) && defined(PIN_HWSERIAL0_RX_PINSWAP5) && defined(PIN_HWSERIAL0_XCK_PINSWAP5) && defined(PIN_HWSERIAL0_XDIR_PINSWAP5) && (PIN_HWSERIAL0_TX_PINSWAP5 != NOT_A_PIN || PIN_HWSERIAL0_TX_PINSWAP5!= NOT_A_PIN))
                ,{HWSERIAL0_MUX_PINSWAP5, PIN_HWSERIAL0_TX_PINSWAP5, PIN_HWSERIAL0_RX_PINSWAP5, PIN_HWSERIAL0_XCK_PINSWAP5, PIN_HWSERIAL0_XDIR_PINSWAP5}
              #else
                ,{HWSERIAL0_MUX_PINSWAP5, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN}
              #endif
              #if defined(HWSERIAL0_MUX_PINSWAP6)
                #if (defined(PIN_HWSERIAL0_TX_PINSWAP6) && defined(PIN_HWSERIAL0_RX_PINSWAP6) && defined(PIN_HWSERIAL0_XCK_PINSWAP6) && defined(PIN_HWSERIAL0_XDIR_PINSWAP6) && (PIN_HWSERIAL0_TX_PINSWAP6 != NOT_A_PIN || PIN_HWSERIAL0_TX_PINSWAP6!= NOT_A_PIN))
                  ,{HWSERIAL0_MUX_PINSWAP6, PIN_HWSERIAL0_TX_PINSWAP6, PIN_HWSERIAL0_RX_PINSWAP6, PIN_HWSERIAL0_XCK_PINSWAP6, PIN_HWSERIAL0_XDIR_PINSWAP6}
                #else
                  ,{HWSERIAL0_MUX_PINSWAP1, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN}
                #endif
                };
                #define MUXCOUNT_USART0 7
              #else
                };
                #define MUXCOUNT_USART0 6
              #endif
            #else
              };
              #define MUXCOUNT_USART0 5
            #endif
          #else
            };
            #define MUXCOUNT_USART0 4
          #endif
        #else
          };
          #define MUXCOUNT_USART0 3
        #endif
      #else
        };
        #define MUXCOUNT_USART0 2
      #endif
    #else
      };
      #define MUXCOUNT_USART0 1
    #endif
  #endif
#endif
/* all available information indicates that USART1 will on new parts have 2 bits dedicated to it in portmux, hence having at most 3 pinful mux options.*/
#if defined(USART1)
  const uint8_t _usart1_pins[][5] PROGMEM = {
  #if (defined(HWSERIAL1_MUX))
    #if (defined(PIN_HWSERIAL1_TX) && defined(PIN_HWSERIAL1_RX) && defined(PIN_HWSERIAL1_XCK) && defined(PIN_HWSERIAL1_XDIR) && (PIN_HWSERIAL1_TX != NOT_A_PIN || PIN_HWSERIAL1_TX!= NOT_A_PIN))
      {HWSERIAL1_MUX, PIN_HWSERIAL1_TX, PIN_HWSERIAL1_RX, PIN_HWSERIAL1_XCK, PIN_HWSERIAL1_XDIR}
    #else
      {HWSERIAL1_MUX, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN}
    #endif
    #if defined(HWSERIAL1_MUX_PINSWAP1)
      #if (defined(PIN_HWSERIAL1_TX_PINSWAP1) && defined(PIN_HWSERIAL1_RX_PINSWAP1) && defined(PIN_HWSERIAL1_XCK_PINSWAP1) && defined(PIN_HWSERIAL1_XDIR_PINSWAP1) && (PIN_HWSERIAL1_TX_PINSWAP1 != NOT_A_PIN || PIN_HWSERIAL1_TX_PINSWAP1!= NOT_A_PIN))
        ,{HWSERIAL1_MUX_PINSWAP1, PIN_HWSERIAL1_TX_PINSWAP1, PIN_HWSERIAL1_RX_PINSWAP1, PIN_HWSERIAL1_XCK_PINSWAP1, PIN_HWSERIAL1_XDIR_PINSWAP1}
      #else
        ,{HWSERIAL1_MUX_PINSWAP1, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN}
      #endif
      #if defined(HWSERIAL1_MUX_PINSWAP2)
        #if (defined(PIN_HWSERIAL1_TX_PINSWAP2) && defined(PIN_HWSERIAL1_RX_PINSWAP2) && defined(PIN_HWSERIAL1_XCK_PINSWAP2) && defined(PIN_HWSERIAL1_XDIR_PINSWAP2) && (PIN_HWSERIAL1_TX_PINSWAP2 != NOT_A_PIN || PIN_HWSERIAL1_TX_PINSWAP2!= NOT_A_PIN))
          ,{HWSERIAL1_MUX_PINSWAP2, PIN_HWSERIAL1_TX_PINSWAP2, PIN_HWSERIAL1_RX_PINSWAP2, PIN_HWSERIAL1_XCK_PINSWAP2, PIN_HWSERIAL1_XDIR_PINSWAP2}
        #else
          ,{HWSERIAL1_MUX_PINSWAP2, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN}
        #endif
        };
        #define MUXCOUNT_USART1 3
      #else
        };
        #define MUXCOUNT_USART1 2
      #endif
    #else
      };
      #define MUXCOUNT_USART1 1
    #endif
  #endif
#endif
/* No product brief or other available information indicates plans for more mux options for USART2 */
#if defined(USART2)
  #if (defined(HWSERIAL2_MUX))
  const uint8_t _usart2_pins[][5] PROGMEM = {
    #if (defined(PIN_HWSERIAL2_TX) && defined(PIN_HWSERIAL2_RX) && defined(PIN_HWSERIAL2_XCK) && defined(PIN_HWSERIAL2_XDIR) && (PIN_HWSERIAL2_TX != NOT_A_PIN || PIN_HWSERIAL2_TX!= NOT_A_PIN))
      {HWSERIAL2_MUX, PIN_HWSERIAL2_TX, PIN_HWSERIAL2_RX, PIN_HWSERIAL2_XCK, PIN_HWSERIAL2_XDIR}
    #else
      {HWSERIAL2_MUX, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN}
    #endif
    #if (defined(HWSERIAL2_MUX_PINSWAP1))
      #if (defined(PIN_HWSERIAL2_TX_PINSWAP1) && defined(PIN_HWSERIAL2_RX_PINSWAP1) && defined(PIN_HWSERIAL2_XCK_PINSWAP1) && defined(PIN_HWSERIAL2_XDIR_PINSWAP1) && (PIN_HWSERIAL2_TX_PINSWAP1 != NOT_A_PIN || PIN_HWSERIAL2_TX_PINSWAP1!= NOT_A_PIN))
        ,{HWSERIAL2_MUX_PINSWAP1, PIN_HWSERIAL2_TX_PINSWAP1, PIN_HWSERIAL2_RX_PINSWAP1, PIN_HWSERIAL2_XCK_PINSWAP1, PIN_HWSERIAL2_XDIR_PINSWAP1}
      #else
        ,{HWSERIAL2_MUX_PINSWAP1, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN}
      #endif
      };
      #define MUXCOUNT_USART2 2
    #else
      };
      #define MUXCOUNT_USART2 1
    #endif
  #endif
#endif

#if defined(USART3)
  #if (defined(HWSERIAL3_MUX))
  const uint8_t _usart3_pins[][5] PROGMEM = {
    #if (defined(PIN_HWSERIAL3_TX) && defined(PIN_HWSERIAL3_RX) && defined(PIN_HWSERIAL3_XCK) && defined(PIN_HWSERIAL3_XDIR) && (PIN_HWSERIAL3_TX != NOT_A_PIN || PIN_HWSERIAL3_TX!= NOT_A_PIN))
      {HWSERIAL3_MUX, PIN_HWSERIAL3_TX, PIN_HWSERIAL3_RX, PIN_HWSERIAL3_XCK, PIN_HWSERIAL3_XDIR}
    #else
      {HWSERIAL3_MUX, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN}
    #endif
    #if (defined(HWSERIAL3_MUX_PINSWAP1))
      #if (defined(PIN_HWSERIAL3_TX_PINSWAP1) && defined(PIN_HWSERIAL3_RX_PINSWAP1) && defined(PIN_HWSERIAL3_XCK_PINSWAP1) && defined(PIN_HWSERIAL3_XDIR_PINSWAP1) && (PIN_HWSERIAL3_TX_PINSWAP1 != NOT_A_PIN || PIN_HWSERIAL3_TX_PINSWAP1!= NOT_A_PIN))
        ,{HWSERIAL3_MUX_PINSWAP1, PIN_HWSERIAL3_TX_PINSWAP1, PIN_HWSERIAL3_RX_PINSWAP1, PIN_HWSERIAL3_XCK_PINSWAP1, PIN_HWSERIAL3_XDIR_PINSWAP1}
      #else
        ,{HWSERIAL3_MUX_PINSWAP1, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN}
      #endif
      };
      #define MUXCOUNT_USART3 2
    #else
      };
      #define MUXCOUNT_USART3 1
    #endif
  #endif
#endif
#if defined(USART4)
  #if (defined(HWSERIAL4_MUX))
  const uint8_t _usart4_pins[][5] PROGMEM = {
    #if (defined(PIN_HWSERIAL4_TX) && defined(PIN_HWSERIAL4_RX) && defined(PIN_HWSERIAL4_XCK) && defined(PIN_HWSERIAL4_XDIR) && (PIN_HWSERIAL4_TX != NOT_A_PIN || PIN_HWSERIAL4_TX!= NOT_A_PIN))
      {HWSERIAL4_MUX, PIN_HWSERIAL4_TX, PIN_HWSERIAL4_RX, PIN_HWSERIAL4_XCK, PIN_HWSERIAL4_XDIR}
    #else
      {HWSERIAL4_MUX, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN}
    #endif
    #if (defined(HWSERIAL4_MUX_PINSWAP1))
      #if (defined(PIN_HWSERIAL4_TX_PINSWAP1) && defined(PIN_HWSERIAL4_RX_PINSWAP1) && defined(PIN_HWSERIAL4_XCK_PINSWAP1) && defined(PIN_HWSERIAL4_XDIR_PINSWAP1) && (PIN_HWSERIAL4_TX_PINSWAP1 != NOT_A_PIN || PIN_HWSERIAL4_TX_PINSWAP1!= NOT_A_PIN))
        ,{HWSERIAL4_MUX_PINSWAP1, PIN_HWSERIAL4_TX_PINSWAP1, PIN_HWSERIAL4_RX_PINSWAP1, PIN_HWSERIAL4_XCK_PINSWAP1, PIN_HWSERIAL4_XDIR_PINSWAP1}
      #else
        ,{HWSERIAL4_MUX_PINSWAP1, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN}
      #endif
      };
      #define MUXCOUNT_USART4 2
    #else
      };
      #define MUXCOUNT_USART4 1
    #endif
  #endif
#endif
#if defined(USART5)
  #if (defined(HWSERIAL5_MUX))
  const uint8_t _usart5_pins[][5] PROGMEM = {
    #if (defined(PIN_HWSERIAL5_TX) && defined(PIN_HWSERIAL5_RX) && defined(PIN_HWSERIAL5_XCK) && defined(PIN_HWSERIAL5_XDIR) && (PIN_HWSERIAL5_TX != NOT_A_PIN || PIN_HWSERIAL5_TX!= NOT_A_PIN))
      {HWSERIAL5_MUX, PIN_HWSERIAL5_TX, PIN_HWSERIAL5_RX, PIN_HWSERIAL5_XCK, PIN_HWSERIAL5_XDIR}
    #else
      {HWSERIAL5_MUX, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN}
    #endif
    #if (defined(HWSERIAL5_MUX_PINSWAP1))
      #if (defined(PIN_HWSERIAL5_TX_PINSWAP1) && defined(PIN_HWSERIAL5_RX_PINSWAP1) && defined(PIN_HWSERIAL5_XCK_PINSWAP1) && defined(PIN_HWSERIAL5_XDIR_PINSWAP1) && (PIN_HWSERIAL5_TX_PINSWAP1 != NOT_A_PIN || PIN_HWSERIAL5_TX_PINSWAP1!= NOT_A_PIN))
        ,{HWSERIAL5_MUX_PINSWAP1, PIN_HWSERIAL5_TX_PINSWAP1, PIN_HWSERIAL5_RX_PINSWAP1, PIN_HWSERIAL5_XCK_PINSWAP1, PIN_HWSERIAL5_XDIR_PINSWAP1}
      #else
        ,{HWSERIAL5_MUX_PINSWAP1, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN}
      #endif
      };
      #define MUXCOUNT_USART5 2
    #else
      };
      #define MUXCOUNT_USART5 1
    #endif
  #endif
#endif




const uint8_t _usart_ports[][4] PROGMEM = {
  {PORTMUX_USART0_gm, 0, MUXCOUNT_USART0, HWSERIAL0_MUX_PINSWAP_NONE},
  #if defined(USART1)
    {PORTMUX_USART1_gm, 0, MUXCOUNT_USART1, HWSERIAL1_MUX_PINSWAP_NONE},
    #if defined(USART2)
      {PORTMUX_USART2_gm, 0, MUXCOUNT_USART2, HWSERIAL2_MUX_PINSWAP_NONE},
      #if defined(USART3)
        #if (PORTMUX_USART0_gm < 8)
          {PORTMUX_USART3_gm, 1, MUXCOUNT_USART3, HWSERIAL3_MUX_PINSWAP_NONE},
          // if the groupmask is small, it must be on the next register.
        #else
          {PORTMUX_USART3_gm, 0, MUXCOUNT_USART3, HWSERIAL3_MUX_PINSWAP_NONE},
        #endif
        #if defined(USART4)
          {PORTMUX_USART4_gm, 1, MUXCOUNT_USART4, HWSERIAL4_MUX_PINSWAP_NONE},
          #if defined(USART5)
            {PORTMUX_USART5_gm, 1, MUXCOUNT_USART5, HWSERIAL5_MUX_PINSWAP_NONE},
          #endif
        #endif
      #endif
    #endif
  #endif
};


void _usart_mux_set(uint8_t port_num, uint8_t mux_code) {
  volatile uint8_t* portmux = (pgm_read_byte_near(&_usart_ports[port_num][1]) == 1) ? ((volatile uint8_t *) &PORTMUX_USARTROUTEA) : ((volatile uint8_t *) &PORTMUX_USARTROUTEA);
  uint8_t temp   = *portmux;
  temp          &= ~pgm_read_byte_near(&_usart_ports[port_num][1]);
  temp          |= mux_code;
  *portmux       = temp;
}

uint8_t _usart_swap(uint8_t portinfo[][5], uint8_t port_num, uint8_t swap_num) {
  uint8_t mux_val = 0;
  if (swap_num == 255) { // swap of -1 (255) -> enable with no pins connected.
    mux_val = pgm_read_byte_near(&_usart_ports[port_num][3]);
    return 255;
  } else {
    uint8_t mux_count=pgm_read_byte_near(&_usart_ports[port_num][2]);
    if (swap_num < mux_count) {
      mux_val = pgm_read_byte_near(&portinfo[swap_num][0]);
    } else { // Invalid swap requested.
      swap_num = 0;
      mux_val = pgm_read_byte_near(&portinfo[0][0]);
    }
  }
  _usart_mux_set(port_num, mux_val);
  return swap_num;
}

uint8_t _usart_pins(uint8_t portinfo[][5], uint8_t port_num, uint8_t tx, uint8_t rx) {
  if (tx == NOT_A_PIN && rx == NOT_A_PIN) {
    return _usart_ports[port_num][3];
  } else {
    for (uint8_t i = 0; i < _usart_ports[port_num][2]; i++) {
      if (tx == pgm_read_byte_near(&portinfo[i][1]) && (rx == pgm_read_byte_near(&portinfo[i][2]))) {
        return pgm_read_byte_near(&portinfo[i][0]);
      }
    }
    return NOT_A_MUX;
  }
}
