/* UART_swap.h - Pin tables and ugly #ifdefs for efficient pinswapping.
 * and permit us to access all the options of the DD-series.
 * UART_constants.h has stuff that applies to UART in general.
 * This file has stuff that processes specific pinouts.
 *
 * Written by Spence Konde and MX682X in late 2021.
 *
 * This file is part of DxCore, which is free software (LGPL 2.1) based on the work
 * of countless members of the Arduino community.
 * See License.md for more information
 *
 * This is a mixture of stuff that was formerly spread out, and novel code to generate a _usart_pins variable to
 * hold required pin identifiers, mux values, and so on, as well as moving things that we bloody well know
 * out of variants. It's not like the DRE vector for hardware serial 1 is ever going to be something other than
 * USART1_DRE_vect or something, so why do we act like it could be?
 */

/* Hardware serial is the most important of the big three interfaces, and also the trickiest; we need to totally rearchitect how the serial classes handle pins
 * The objective here is to was to get HardwareSerial out of the business of handling pins so that the info we need lives in progmem
 * and can be used without shipping a whole bunch of constants into the class, which is a waste of ram and would involve a bloated class constructor several lines long.
 *
 * During this effort it became clear that we were defining many things in the variants that didn't vary. We've mostly done away with those.
 * #define NUM_HWSERIAL_PORTS              2                        <----- This will be filled in if not provided here, but it should be in the variant.
 * #define HWSERIAL0                       &USART0                  <----- This was unnecessary. Renumbering of USARTs is not supported (though we do have a way to change which one is aliased to Serial)
 * #define HWSERIAL0_DRE_VECTOR            USART0_DRE_vect          <----- Surely we know that USART0 will always have it's vector named USART0_DRE_vect?
 * #define HWSERIAL0_DRE_VECTOR_NUM        USART0_DRE_vect_num      <----- Was this only used for the serial interrupt elevation idiocy? (A: Yes)
 * #define HWSERIAL0_RXC_VECTOR            USART0_RXC_vect          <----- See 2 lines up.
 * #define HWSERIAL0_MUX                   PORTMUX_USART0_DEFAULT_gc  <--- Mandatory if this mux option is valid, must be omitted or NOT_A_MUX if it is not valid.
 * #define HWSERIAL0_MUX_PINSWAP_1         PORTMUX_USART0_ALT1_gc     <--- As above.
 * #define HWSERIAL0_MUX_PINSWAP_2         PORTMUX_USART0_ALT2_gc     <--- If this option does not exist, omit/
 * #define HWSERIAL0_MUX_PINSWAP_NONE      PORTMUX_USART0_NONE_gc     <--- Mandatory
 * #define PIN_HWSERIAL0_TX                PIN_PA0    <---- All mux options that exist must have a TX and RX pin specified
 * #define PIN_HWSERIAL0_RX                PIN_PA1    <---- If only one of the two is available, other should be defined as NOT_A_PIN
 * #define PIN_HWSERIAL0_XCK               PIN_PA2    <---- Preferably defined even if the pin isn't available (as NOT_A_MUX)
 * #define PIN_HWSERIAL0_XDIR              PIN_PA3
 * #define PIN_HWSERIAL0_TX_PINSWAP_1      NOT_A_PIN  <---- If either TX or RX pin is missing, or both RX and TX are NOT_A_PIN
 * #define PIN_HWSERIAL0_RX_PINSWAP_1      NOT_A_PIN  <---- it's an invalid mux option, and won't be available
 * #define PIN_HWSERIAL0_XCK_PINSWAP_1     NOT_A_PIN  <---- XCK and XDIR, on the other hand, aren't truly used by the core.
 * #define PIN_HWSERIAL0_XDIR_PINSWAP_1    NOT_A_PIN  <---- There is intense register pressure when we're doing Serial.begin(); the compiler is already creating bad code here. So
 * #define PIN_HWSERIAL0_TX_PINSWAP_2      PIN_PA4          wanted to avoid excess logic. On all current an announced pins, IF there is an XCK pin, it's the pin after RX, and
 * #define PIN_HWSERIAL0_RX_PINSWAP_2      PIN_PA5          if there is a XDIR pin, it's numbered 2 after RX. We don't do anything to avoid setting mode of that pin if we don't have the pin.
 * #define PIN_HWSERIAL0_XCK_PINSWAP_2     NOT_A_PIN        on the grounds that what the're attempting has no hope of working (they requested essential functionality on a pin that doesn't exist)
 * #define PIN_HWSERIAL0_XDIR_PINSWAP_2    NOT_A_PIN        hence nothing the core can do is correct, so we might as well be wrong and not waste flash than be wrong and waste flash.
 *
 * Thus far, with the exception of USART1, the rest of the serial ports have been entirely excluded from USART0's DD-day mux
 * party... and all he got to do was pick up crumbs and lick icecubes after USART0 snarfed the cake and drained the punchbowl.
 * what will be interesting is to see whether this trend continues (which existing data suggests) or whether that mention of
 * TWI1 wasn't the only error in the EA's multiplexing considerations table
 */



#ifndef UART_SWAP_H
#define UART_SWAP_H

#include "Arduino.h"

#if defined(USART0) || defined(USART1) || defined(USART2) || defined(USART3) || defined(USART4) || defined(USART5)

  /* Let's see what we can deduce from the headers - no need to crap up the variant files and make more busywork
   * The answer turns out to be "Everything except the actual pins"*/
  #if defined(PORTMUX_USARTROUTEA)
    #define HWSERIAL_MUX_REGISTER_BASE      &PORTMUX.USARTROUTEA
    #if defined(PORTMUX_USARTROUTEB)
      #define HWSERIAL_MUX_REG_COUNT 2
    #else
      #define HWSERIAL_MUX_REG_COUNT 1
    #endif
  #elif defined(PORTMUX_CTRLB)
    // This is a MUCH simpler case - swap is trivial on those.
    #define HWSERIAL_MUX_TINY
  #else
    #error "This part is unsupported, or there is a bug in the core or a problem with your toolchain."
  #endif

  /* Okay, now we know what the mux register is, and whether it's a tiny-like mux or a Dx-like one. */
  #define HAVE_HWSERIAL
  #if !defined(NUM_HWSERIAL_PORTS)
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
      #if !defined(HWSERIAL_MUX_TINY)
        #error "Only one USART, but this is a core for modern AVRs, and it's NOT a tinyAVR 0/1-series. Something is wrong!"
      #else
        #error "No USARTs detected, all supported parts have at least 2, something is wrong!"
      #endif
    #endif
  #endif
  /* If NUM_HWSERIAL_PORTS is not defined, we calculate it assuming there are no gaps. */

  /* Recreate all those old defines so they aren't spread over all the variants when they don't vary! */
  #if defined(USART0)
    #define HAVE_HWSERIAL0                  1
    #define HWSERIAL0_DRE_VECTOR            USART0_DRE_vect
    #define HWSERIAL0_DRE_VECTOR_NUM        USART0_DRE_vect_num
    #define HWSERIAL0_RXC_VECTOR            USART0_RXC_vect
    #define HWSERIAL0                       &USART0
  #endif
  #if defined(USART1)
    #define HAVE_HWSERIAL1                  1
    #define HWSERIAL1_DRE_VECTOR            USART1_DRE_vect
    #define HWSERIAL1_DRE_VECTOR_NUM        USART1_DRE_vect_num
    #define HWSERIAL1_RXC_VECTOR            USART1_RXC_vect
    #define HWSERIAL1                       &USART1
  #endif
  #if defined(USART2)
    #define HAVE_HWSERIAL2                  1
    #define HWSERIAL2_DRE_VECTOR            USART2_DRE_vect
    #define HWSERIAL2_DRE_VECTOR_NUM        USART2_DRE_vect_num
    #define HWSERIAL2_RXC_VECTOR            USART2_RXC_vect
    #define HWSERIAL2                       &USART2
  #endif
  #if defined(USART3)
    #define HAVE_HWSERIAL3                  1
    #define HWSERIAL3_DRE_VECTOR            USART3_DRE_vect
    #define HWSERIAL3_DRE_VECTOR_NUM        USART3_DRE_vect_num
    #define HWSERIAL3_RXC_VECTOR            USART3_RXC_vect
    #define HWSERIAL3                       &USART3
  #endif
  #if defined(USART4)
    #define HAVE_HWSERIAL4                  1
    #define HWSERIAL4_DRE_VECTOR            USART4_DRE_vect
    #define HWSERIAL4_DRE_VECTOR_NUM        USART4_DRE_vect_num
    #define HWSERIAL4_RXC_VECTOR            USART4_RXC_vect
    #define HWSERIAL4                       &USART4
  #endif
  #if defined(USART5)
    #define HAVE_HWSERIAL5                  1
    #define HWSERIAL5_DRE_VECTOR            USART5_DRE_vect
    #define HWSERIAL5_DRE_VECTOR_NUM        USART5_DRE_vect_num
    #define HWSERIAL5_RXC_VECTOR            USART5_RXC_vect
    #define HWSERIAL5                       &USART5
  #endif



/* before the big mess below runs, we need to keep it from  mixbehaving if the user omits, for example. the mux settings
 * for a few muxes that aren't valid on their, when there's a higher numbered one that is valid.
 * check from the top down and make sure that there are no mux options defined atop a cloud without
 * a proper foundation.
 * From this, not only does the mess below generate the mux table, it also looks for MUX_DEFAULT_USARTn.
 * If it's not defined, it is defined as the first valid option for that USART. Override by defining that if needed
 */

#if defined(HWSERIAL0_MUX_PINSWAP_6)
  #if !defined(HWSERIAL0_MUX_PINSWAP_5)
    #define HWSERIAL0_MUX_PINSWAP_5 NOT_A_MUX
  #endif
#endif

#if defined(HWSERIAL0_MUX_PINSWAP_5)
  #if !defined(HWSERIAL0_MUX_PINSWAP_4)
    #define HWSERIAL0_MUX_PINSWAP_4 NOT_A_MUX
  #endif
#endif

#if defined(HWSERIAL0_MUX_PINSWAP_4)
  #if !defined(HWSERIAL0_MUX_PINSWAP_3)
    #define HWSERIAL0_MUX_PINSWAP_3 NOT_A_MUX
  #endif
#endif

#if defined(HWSERIAL0_MUX_PINSWAP_3)
  #if !defined(HWSERIAL0_MUX_PINSWAP_2)
    #define HWSERIAL0_MUX_PINSWAP_2 NOT_A_MUX
  #endif
#endif

#if defined(HWSERIAL0_MUX_PINSWAP_2)
  #if !defined(HWSERIAL0_MUX_PINSWAP_1)
    #define HWSERIAL0_MUX_PINSWAP_1 NOT_A_MUX
  #endif
#endif

#if defined(HWSERIAL0_MUX_PINSWAP_1)
  #if !defined(HWSERIAL0_MUX)
    #define HWSERIAL0_MUX NOT_A_MUX
  #endif
#endif

#if defined(HWSERIAL1_MUX_PINSWAP_3)
  #if !defined(HWSERIAL1_MUX_PINSWAP_2)
    #define HWSERIAL1_MUX_PINSWAP_2 NOT_A_MUX
  #endif
#endif

#if defined(HWSERIAL1_MUX_PINSWAP_2)
  #if !defined(HWSERIAL1_MUX_PINSWAP_1)
    #define HWSERIAL1_MUX_PINSWAP_1 NOT_A_MUX
  #endif
#endif

#if defined(HWSERIAL1_MUX_PINSWAP_1)
  #if !defined(HWSERIAL1_MUX)
    #define HWSERIAL1_MUX NOT_A_MUX
  #endif
#endif

#if defined(HWSERIAL2_MUX_PINSWAP_2)
  #if !defined(HWSERIAL2_MUX_PINSWAP_1)
    #define HWSERIAL2_MUX_PINSWAP_1 NOT_A_MUX
  #endif
#endif

#if defined(HWSERIAL2_MUX_PINSWAP_1)
  #if !defined(HWSERIAL2_MUX)
    #define HWSERIAL2_MUX NOT_A_MUX
  #endif
#endif

#if defined(HWSERIAL3_MUX_PINSWAP_2)
  #if !defined(HWSERIAL3_MUX_PINSWAP_1)
    #define HWSERIAL3_MUX_PINSWAP_1 NOT_A_MUX
  #endif
#endif

#if defined(HWSERIAL3_MUX_PINSWAP_1)
  #if !defined(HWSERIAL3_MUX)
    #define HWSERIAL3_MUX NOT_A_MUX
  #endif
#endif

#if defined(HWSERIAL4_MUX_PINSWAP_2)
  #if !defined(HWSERIAL4_MUX_PINSWAP_1)
    #define HWSERIAL4_MUX_PINSWAP_1 NOT_A_MUX
  #endif
#endif

#if defined(HWSERIAL4_MUX_PINSWAP_1)
  #if !defined(HWSERIAL4_MUX)
    #define HWSERIAL4_MUX NOT_A_MUX
  #endif
#endif

#if defined(HWSERIAL5_MUX_PINSWAP_2)
  #if !defined(HWSERIAL5_MUX_PINSWAP_1)
    #define HWSERIAL5_MUX_PINSWAP_1 NOT_A_MUX
  #endif
#endif

#if defined(HWSERIAL5_MUX_PINSWAP_1)
  #if !defined(HWSERIAL5_MUX)
    #define HWSERIAL5_MUX NOT_A_MUX
  #endif
#endif


  /* This heinous mess does two things:
   * 1. It builds a table containing the pins and portmux settings for all the options defined in pins_arduino.h
   * 2. It counts up the mux options that exist - if they are defined but NOT_A_MUX (indicating gaps in table), then the row of the table is filled with NOT_A_PIN.
   *    This should be done if (and only if) subsequent options do have pins. For example, on DA/DB with 28/32 pins, USART1 has only 1 available mux option with pins
   *    HWSERIAL1_MUX_PINSWAP1 should not be defined, and the table created will have a single row.
   *
   *    The final row of the table contains the value used for "None" (not connected to any pins) option, the offset from PORTMUX.USARTROUTEA of the register that the mux bits are located at, and the group mask.
   */

  /* The king, USART0 is the most flexible of the hardware serial ports on newer devices, with an extra bit in the PORTMUX allowing up to 7 pinful mux options.
   * And on the DD-series, USART0 grabbed 4 options for a total of 6. */
  #define USART_PINS_WIDTH 3  // basically a sizeof(), but as define, needed for some functions
  #if defined(USART0)
    const uint8_t _usart0_pins[][USART_PINS_WIDTH] PROGMEM = {
      #if (defined(HWSERIAL0_MUX))
        #if (defined(PIN_HWSERIAL0_TX) && defined(PIN_HWSERIAL0_RX) && defined(PIN_HWSERIAL0_XCK) && defined(PIN_HWSERIAL0_XDIR) && ((PIN_HWSERIAL0_TX != NOT_A_PIN && CLOCK_SOURCE == 0) || (PIN_HWSERIAL0_RX != NOT_A_PIN && CLOCK_SOURCE != 1)))
          #if !defined(HWSERIAL0_MUX_DEFAULT)
            #define HWSERIAL0_MUX_DEFAULT (HWSERIAL0_MUX)
          #endif
          {HWSERIAL0_MUX, PIN_HWSERIAL0_TX, PIN_HWSERIAL0_XCK},
        #else
          {HWSERIAL0_MUX, NOT_A_PIN, NOT_A_PIN},
        #endif
        #if defined(HWSERIAL0_MUX_PINSWAP_1)
          #if (defined(PIN_HWSERIAL0_TX_PINSWAP_1) && defined(PIN_HWSERIAL0_RX_PINSWAP_1) && defined(PIN_HWSERIAL0_XCK_PINSWAP_1) && defined(PIN_HWSERIAL0_XDIR_PINSWAP_1) && (PIN_HWSERIAL0_TX_PINSWAP_1 != NOT_A_PIN || PIN_HWSERIAL0_RX_PINSWAP_1 != NOT_A_PIN))
          #if !defined(HWSERIAL0_MUX_DEFAULT)
            #define HWSERIAL0_MUX_DEFAULT (HWSERIAL0_MUX_PINSWAP_1)
          #endif
            {HWSERIAL0_MUX_PINSWAP_1, PIN_HWSERIAL0_TX_PINSWAP_1, PIN_HWSERIAL0_XCK_PINSWAP_1},
          #else
            {HWSERIAL0_MUX_PINSWAP_1, NOT_A_PIN, NOT_A_PIN},
          #endif
          #if defined(HWSERIAL0_MUX_PINSWAP_2)
            #if (defined(PIN_HWSERIAL0_TX_PINSWAP_2) && defined(PIN_HWSERIAL0_RX_PINSWAP_2) && defined(PIN_HWSERIAL0_XCK_PINSWAP_2) && defined(PIN_HWSERIAL0_XDIR_PINSWAP_2) && (PIN_HWSERIAL0_TX_PINSWAP_2 != NOT_A_PIN || PIN_HWSERIAL0_RX_PINSWAP_2 != NOT_A_PIN))
            #if !defined(HWSERIAL0_MUX_DEFAULT)
              #define HWSERIAL0_MUX_DEFAULT (HWSERIAL0_MUX_PINSWAP_2)
            #endif
              {HWSERIAL0_MUX_PINSWAP_2, PIN_HWSERIAL0_TX_PINSWAP_2, PIN_HWSERIAL0_XCK_PINSWAP_2},
            #else
              {HWSERIAL0_MUX_PINSWAP_2, NOT_A_PIN, NOT_A_PIN},
            #endif
            #if defined(HWSERIAL0_MUX_PINSWAP_3)
              #if (defined(PIN_HWSERIAL0_TX_PINSWAP_3) && defined(PIN_HWSERIAL0_RX_PINSWAP_3) && defined(PIN_HWSERIAL0_XCK_PINSWAP_3) && defined(PIN_HWSERIAL0_XDIR_PINSWAP_3) && (PIN_HWSERIAL0_TX_PINSWAP_3 != NOT_A_PIN || PIN_HWSERIAL0_RX_PINSWAP_3 != NOT_A_PIN))
              #if !defined(HWSERIAL0_MUX_DEFAULT)
                #define HWSERIAL0_MUX_DEFAULT (HWSERIAL0_MUX_PINSWAP_3)
              #endif
                {HWSERIAL0_MUX_PINSWAP_3, PIN_HWSERIAL0_TX_PINSWAP_3, PIN_HWSERIAL0_XCK_PINSWAP_3},
              #else
                {HWSERIAL0_MUX_PINSWAP_3, NOT_A_PIN, NOT_A_PIN},
              #endif
              #if defined(HWSERIAL0_MUX_PINSWAP_4)
                #if (defined(PIN_HWSERIAL0_TX_PINSWAP_4) && defined(PIN_HWSERIAL0_RX_PINSWAP_4) && defined(PIN_HWSERIAL0_XCK_PINSWAP_4) && defined(PIN_HWSERIAL0_XDIR_PINSWAP_4) && (PIN_HWSERIAL0_TX_PINSWAP_4 != NOT_A_PIN || PIN_HWSERIAL0_RX_PINSWAP_4 != NOT_A_PIN))
                #if !defined(HWSERIAL0_MUX_DEFAULT)
                  #define HWSERIAL0_MUX_DEFAULT (HWSERIAL0_MUX_PINSWAP_4)
                #endif
                  {HWSERIAL0_MUX_PINSWAP_4, PIN_HWSERIAL0_TX_PINSWAP_4, PIN_HWSERIAL0_XCK_PINSWAP_4},
                #else
                  {HWSERIAL0_MUX_PINSWAP_4, NOT_A_PIN, NOT_A_PIN},
                #endif
                #if defined(HWSERIAL0_MUX_PINSWAP_5)
                  #if (defined(PIN_HWSERIAL0_TX_PINSWAP_5) && defined(PIN_HWSERIAL0_RX_PINSWAP_5) && defined(PIN_HWSERIAL0_XCK_PINSWAP_5) && defined(PIN_HWSERIAL0_XDIR_PINSWAP_5) && (PIN_HWSERIAL0_TX_PINSWAP_5 != NOT_A_PIN || PIN_HWSERIAL0_RX_PINSWAP_5 != NOT_A_PIN))
                  #if !defined(HWSERIAL0_MUX_DEFAULT)
                    #define HWSERIAL0_MUX_DEFAULT (HWSERIAL0_MUX_PINSWAP_5)
                  #endif
                    {HWSERIAL0_MUX_PINSWAP_5, PIN_HWSERIAL0_TX_PINSWAP_5, PIN_HWSERIAL0_XCK_PINSWAP_5},
                  #else
                    {HWSERIAL0_MUX_PINSWAP_5, NOT_A_PIN, NOT_A_PIN},
                  #endif
                  #if defined(HWSERIAL0_MUX_PINSWAP_6)
                    #if (defined(PIN_HWSERIAL0_TX_PINSWAP_6) && defined(PIN_HWSERIAL0_RX_PINSWAP_6) && defined(PIN_HWSERIAL0_XCK_PINSWAP_6) && defined(PIN_HWSERIAL0_XDIR_PINSWAP_6) && (PIN_HWSERIAL0_TX_PINSWAP_6 != NOT_A_PIN || PIN_HWSERIAL0_RX_PINSWAP_6 != NOT_A_PIN))
                    #if !defined(HWSERIAL0_MUX_DEFAULT)
                      #define HWSERIAL0_MUX_DEFAULT (HWSERIAL0_MUX_PINSWAP_6)
                    #endif
                      {HWSERIAL0_MUX_PINSWAP_6, PIN_HWSERIAL0_TX_PINSWAP_6, PIN_HWSERIAL0_XCK_PINSWAP_6},
                    #else
                      {HWSERIAL0_MUX_PINSWAP_6, NOT_A_PIN, NOT_A_PIN},
                    #endif
                    #define MUXCOUNT_USART0 7
                  #else
                    #define MUXCOUNT_USART0 6
                  #endif
                #else
                  #define MUXCOUNT_USART0 5
                #endif
              #else
                #define MUXCOUNT_USART0 4
              #endif
            #else
              #define MUXCOUNT_USART0 3
            #endif
          #else
            #define MUXCOUNT_USART0 2
          #endif
        #else
          #define MUXCOUNT_USART0 1
        #endif
        {HWSERIAL0_MUX_PINSWAP_NONE, 0, PORTMUX_USART0_gm}
      #endif
    };
  #endif
  /* USART1 only has 2 bits to specify mux and this doesn't change on the DD, so it will only get at most 3 pinful mux options. The DD series has default and alt2, but no alt1 because that would go on PC4 and PC5 which are pins it doesn't have.
   * The DA/DB series only have default and alt1 for all USARTs at most, provided they have pins 4 and 5 on the port.*/
  #if defined(USART1)
    const uint8_t _usart1_pins[][USART_PINS_WIDTH] PROGMEM = {
      #if (defined(HWSERIAL1_MUX))
        #if (defined(PIN_HWSERIAL1_TX) && defined(PIN_HWSERIAL1_RX) && defined(PIN_HWSERIAL1_XCK) && defined(PIN_HWSERIAL1_XDIR) && (PIN_HWSERIAL1_TX != NOT_A_PIN || PIN_HWSERIAL1_TX != NOT_A_PIN))
          #if !defined(HWSERIAL1_MUX_DEFAULT)
            #define HWSERIAL1_MUX_DEFAULT (HWSERIAL1_MUX)
          #endif
          {HWSERIAL1_MUX, PIN_HWSERIAL1_TX, PIN_HWSERIAL1_XCK},
        #else
          {HWSERIAL1_MUX, NOT_A_PIN, NOT_A_PIN},
        #endif
        #if defined(HWSERIAL1_MUX_PINSWAP_1)
          #if (defined(PIN_HWSERIAL1_TX_PINSWAP_1) && defined(PIN_HWSERIAL1_RX_PINSWAP_1) && defined(PIN_HWSERIAL1_XCK_PINSWAP_1) && defined(PIN_HWSERIAL1_XDIR_PINSWAP_1) && (PIN_HWSERIAL1_TX_PINSWAP_1 != NOT_A_PIN || PIN_HWSERIAL1_RX_PINSWAP_1 != NOT_A_PIN))
          #if !defined(HWSERIAL1_MUX_DEFAULT)
            #define HWSERIAL1_MUX_DEFAULT (HWSERIAL1_MUX_PINSWAP_1)
          #endif
            {HWSERIAL1_MUX_PINSWAP_1, PIN_HWSERIAL1_TX_PINSWAP_1, PIN_HWSERIAL1_XCK_PINSWAP_1},
          #else
            {HWSERIAL1_MUX_PINSWAP_1, NOT_A_PIN, NOT_A_PIN},
          #endif
          #if defined(HWSERIAL1_MUX_PINSWAP2)
            #if (defined(PIN_HWSERIAL1_TX_PINSWAP_2) && defined(PIN_HWSERIAL1_RX_PINSWAP_2) && defined(PIN_HWSERIAL1_XCK_PINSWAP_2) && defined(PIN_HWSERIAL1_XDIR_PINSWAP_2) && (PIN_HWSERIAL1_TX_PINSWAP_2 != NOT_A_PIN || PIN_HWSERIAL1_RX_PINSWAP_2 != NOT_A_PIN))
          #if !defined(HWSERIAL1_MUX_DEFAULT)
            #define HWSERIAL1_MUX_DEFAULT (HWSERIAL1_MUX_PINSWAP_2)
          #endif
              {HWSERIAL1_MUX_PINSWAP_2, PIN_HWSERIAL1_TX_PINSWAP_2, PIN_HWSERIAL1_XCK_PINSWAP_2},
            #else
              {HWSERIAL1_MUX_PINSWAP_2, NOT_A_PIN, NOT_A_PIN},
            #endif
            #define MUXCOUNT_USART1 3
          #else
            #define MUXCOUNT_USART1 2
          #endif
        #else
          #define MUXCOUNT_USART1 1
        #endif
        {HWSERIAL1_MUX_PINSWAP_NONE, 0, PORTMUX_USART1_gm}
      #endif
    };
  #endif

  /* No current or announced product supports more than 2 options for USART 2, 3, 4 or 5.
   * It is also unclear what they will do on the first part released with a fourth USART and the expanded USART0 portmux options - One imagines that USART 3 would have nowhere to go USARTROUTEB with the other usarts having to move aside.
   * We'll wait to cross that bridge until Microchip has at least announced an intent to build it....
   */
  #if defined(USART2)
    const uint8_t _usart2_pins[][USART_PINS_WIDTH] PROGMEM = {
      #if (defined(HWSERIAL2_MUX))
        #if (defined(PIN_HWSERIAL2_TX) && defined(PIN_HWSERIAL2_RX) && defined(PIN_HWSERIAL2_XCK) && defined(PIN_HWSERIAL2_XDIR) && (PIN_HWSERIAL2_TX != NOT_A_PIN || PIN_HWSERIAL2_RX!= NOT_A_PIN))
          #if !defined(HWSERIAL2_MUX_DEFAULT)
            #define HWSERIAL2_MUX_DEFAULT (HWSERIAL2_MUX)
          #endif
          {HWSERIAL2_MUX, PIN_HWSERIAL2_TX, PIN_HWSERIAL2_XCK},
        #else
          {HWSERIAL2_MUX, NOT_A_PIN, NOT_A_PIN},
        #endif
        #if (defined(HWSERIAL2_MUX_PINSWAP_1))
          #if (defined(PIN_HWSERIAL2_TX_PINSWAP_1) && defined(PIN_HWSERIAL2_RX_PINSWAP_1) && defined(PIN_HWSERIAL2_XCK_PINSWAP_1) && defined(PIN_HWSERIAL2_XDIR_PINSWAP_1) && (PIN_HWSERIAL2_TX_PINSWAP_1 != NOT_A_PIN || PIN_HWSERIAL2_RX_PINSWAP_1 != NOT_A_PIN))
            #if !defined(HWSERIAL2_MUX_DEFAULT)
              #define HWSERIAL2_MUX_DEFAULT (HWSERIAL2_MUX_PINSWAP_1)
            #endif
            {HWSERIAL2_MUX_PINSWAP_1, PIN_HWSERIAL2_TX_PINSWAP_1, PIN_HWSERIAL2_XCK_PINSWAP_1},
          #else
            {HWSERIAL2_MUX_PINSWAP_1, NOT_A_PIN, NOT_A_PIN},
          #endif
          #define MUXCOUNT_USART2 2
        #else
          #define MUXCOUNT_USART2 1
        #endif
        {HWSERIAL2_MUX_PINSWAP_NONE, 0, PORTMUX_USART2_gm}
      #endif
    };
  #endif

  #if defined(USART3)
    const uint8_t _usart3_pins[][USART_PINS_WIDTH] PROGMEM = {
      #if (defined(HWSERIAL3_MUX))
        #if (defined(PIN_HWSERIAL3_TX) && defined(PIN_HWSERIAL3_RX) && defined(PIN_HWSERIAL3_XCK) && defined(PIN_HWSERIAL3_XDIR) && (PIN_HWSERIAL3_TX != NOT_A_PIN || PIN_HWSERIAL3_RX!= NOT_A_PIN))
          #if !defined(HWSERIAL3_MUX_DEFAULT)
            #define HWSERIAL3_MUX_DEFAULT (HWSERIAL3_MUX)
          #endif
          {HWSERIAL3_MUX, PIN_HWSERIAL3_TX, PIN_HWSERIAL3_XCK},
        #else
          {HWSERIAL3_MUX, NOT_A_PIN, NOT_A_PIN},
        #endif
        #if (defined(HWSERIAL3_MUX_PINSWAP_1))
          #if (defined(PIN_HWSERIAL3_TX_PINSWAP_1) && defined(PIN_HWSERIAL3_RX_PINSWAP_1) && defined(PIN_HWSERIAL3_XCK_PINSWAP_1) && defined(PIN_HWSERIAL3_XDIR_PINSWAP_1) && (PIN_HWSERIAL3_TX_PINSWAP_1 != NOT_A_PIN || PIN_HWSERIAL3_RX_PINSWAP_1!= NOT_A_PIN))
            #if !defined(HWSERIAL3_MUX_DEFAULT)
              #define HWSERIAL3_MUX_DEFAULT (HWSERIAL3_MUX_PINSWAP_1)
            #endif
            {HWSERIAL3_MUX_PINSWAP_1, PIN_HWSERIAL3_TX_PINSWAP_1, PIN_HWSERIAL3_XCK_PINSWAP_1},
          #else
            {HWSERIAL3_MUX_PINSWAP_1, NOT_A_PIN, NOT_A_PIN},
          #endif
          #define MUXCOUNT_USART3 2
        #else
          #define MUXCOUNT_USART3 1
        #endif /* This is the place where it gets murky, since EA-series might have the 4th USART on the 48 pin part...and has the expanded mux options... */
        {HWSERIAL3_MUX_PINSWAP_NONE, 0, PORTMUX_USART3_gm}
      #endif
    };
  #endif

  #if defined(USART4)
    const uint8_t _usart4_pins[][USART_PINS_WIDTH] PROGMEM = {
      #if (defined(HWSERIAL4_MUX))
        #if (defined(PIN_HWSERIAL4_TX) && defined(PIN_HWSERIAL4_RX) && defined(PIN_HWSERIAL4_XCK) && defined(PIN_HWSERIAL4_XDIR) && (PIN_HWSERIAL4_TX != NOT_A_PIN || PIN_HWSERIAL4_RX!= NOT_A_PIN))
          #if !defined(HWSERIAL4_MUX_DEFAULT)
            #define HWSERIAL4_MUX_DEFAULT (HWSERIAL4_MUX)
          #endif
          {HWSERIAL4_MUX, PIN_HWSERIAL4_TX, PIN_HWSERIAL4_XCK},
        #else
          {HWSERIAL4_MUX, NOT_A_PIN, NOT_A_PIN},
        #endif
        #if (defined(HWSERIAL4_MUX_PINSWAP_1))
          #if (defined(PIN_HWSERIAL4_TX_PINSWAP_1) && defined(PIN_HWSERIAL4_RX_PINSWAP_1) && defined(PIN_HWSERIAL4_XCK_PINSWAP_1) && defined(PIN_HWSERIAL4_XDIR_PINSWAP_1) && (PIN_HWSERIAL4_TX_PINSWAP_1 != NOT_A_PIN || PIN_HWSERIAL4_RX_PINSWAP_1!= NOT_A_PIN))
            #if !defined(HWSERIAL4_MUX_DEFAULT)
              #define HWSERIAL4_MUX_DEFAULT (HWSERIAL4_MUX_PINSWAP_1)
            #endif
            {HWSERIAL4_MUX_PINSWAP_1, PIN_HWSERIAL4_TX_PINSWAP_1, PIN_HWSERIAL4_XCK_PINSWAP_1},
          #else
            {HWSERIAL4_MUX_PINSWAP_1, NOT_A_PIN, NOT_A_PIN},
          #endif
          #define MUXCOUNT_USART4 2
        #else
          #define MUXCOUNT_USART4 1
        #endif
        {HWSERIAL4_MUX_PINSWAP_NONE, 1, PORTMUX_USART4_gm}
      #endif
    };
  #endif

  #if defined(USART5)
    const uint8_t _usart5_pins[][USART_PINS_WIDTH] PROGMEM = {
      #if (defined(HWSERIAL5_MUX))
        #if (defined(PIN_HWSERIAL5_TX) && defined(PIN_HWSERIAL5_RX) && defined(PIN_HWSERIAL5_XCK) && defined(PIN_HWSERIAL5_XDIR) && (PIN_HWSERIAL5_TX != NOT_A_PIN || PIN_HWSERIAL5_RTX!= NOT_A_PIN))
          #if !defined(HWSERIAL5_MUX_DEFAULT)
            #define HWSERIAL5_MUX_DEFAULT (HWSERIAL5_MUX)
          #endif
          {HWSERIAL5_MUX, PIN_HWSERIAL5_TX, PIN_HWSERIAL5_XCK},
        #else
          {HWSERIAL5_MUX, NOT_A_PIN, NOT_A_PIN},
        #endif
        #if (defined(HWSERIAL5_MUX_PINSWAP_1))
          #if (defined(PIN_HWSERIAL5_TX_PINSWAP_1) && defined(PIN_HWSERIAL5_RX_PINSWAP_1) && defined(PIN_HWSERIAL5_XCK_PINSWAP_1) && defined(PIN_HWSERIAL5_XDIR_PINSWAP_1) && (PIN_HWSERIAL5_TX_PINSWAP_1 != NOT_A_PIN || PIN_HWSERIAL5_RX_PINSWAP_1!= NOT_A_PIN))
            #if !defined(HWSERIAL5_MUX_DEFAULT)
              #define HWSERIAL5_MUX_DEFAULT (HWSERIAL5_MUX_PINSWAP_1)
            #endif
            {HWSERIAL5_MUX_PINSWAP_1, PIN_HWSERIAL5_TX_PINSWAP_1, PIN_HWSERIAL5_XCK_PINSWAP_1},
          #else
            {HWSERIAL5_MUX_PINSWAP_1, NOT_A_PIN, NOT_A_PIN},
          #endif
          #define MUXCOUNT_USART5 2
        #else
          #define MUXCOUNT_USART5 1
        #endif
        {HWSERIAL5_MUX_PINSWAP_NONE, 1, PORTMUX_USART5_gm}
      #endif
    };
    #endif
  #endif
#endif  // UART_SWAP_H
