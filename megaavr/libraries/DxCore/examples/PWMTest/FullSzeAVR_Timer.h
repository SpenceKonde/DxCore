/*  Welcome to IFDEF HELL!
 *   #if defined(ENTERED_HERE)
 *    abandon(allHope);
 *  #endif
 */

// *INDENT-OFF*
/* This whole thing could *NEVER* be made in a way that was readable to humans and
 *  which would pass asytle. astyle is a great idea, except that it's idea of what
 *  the right formatting is objectively awful when there are nested #if's. It's like
 *  if it didn't
 */

#define tca0 0x00
#define tca1 0x01
#define tcb0 0x10
#define tcb1 0x11
#define tcb2 0x12
#define tcb3 0x13
#define tcb4 0x14
#define tcd0 0x30 //in case there's a TCC ever.
#define tce0 0x40
#define tcf0 0x50




const PROGMEM_MAPPED uint8_t MyTimers[] = {
  #ifdef TCA0
    tca0,
  #endif
  #ifdef TCA1
    tca1,
  #endif
  #ifdef TCB0
    tcb0,
  #endif
  #ifdef TCB1
    tcb1,
  #endif
  #ifdef TCB2
    tcb2,
  #endif
  #ifdef TCB3
    tcb3,
  #endif
  #ifdef TCB4
    tcb4,
  #endif
  #ifdef TCD0
    tcd0,
  #endif
  #ifdef TCE0
    tce0,
  #endif
  #ifdef TCF0
    tcf0,
  #endif
  255
};




const PROGMEM_MAPPED uint8_t TCA0pinsets[] = {
  #if CLOCK_SOURCE == 0
    PIN_PA0, PIN_PA1,
  #else
    NOT_A_PIN, NOT_A_PIN,
  #endif
  #if defined(PIN_PA2) // 14-pin parts don't have these pins
    PIN_PA2,    PIN_PA3,    PIN_PA4,    PIN_PA5,
  #else
    NOT_A_PIN,  NOT_A_PIN,  NOT_A_PIN,  NOT_A_PIN,
  #endif
  #if defined(PORTB) //48-pin and 64 pin parts only have these
    PIN_PB0,    PIN_PB1,    PIN_PB2,    PIN_PB3,    PIN_PB4,    PIN_PB5,
  #else
    NOT_A_PIN,  NOT_A_PIN,  NOT_A_PIN,  NOT_A_PIN,  NOT_A_PIN,  NOT_A_PIN,
  #endif
  #if (defined(MVIO) && _AVR_PINCOUNT < 28) //14/20 pin parts with less than 28 pins don't have PC0
    NOT_A_PIN,
  #else
    PIN_PC0,
  #endif
  PIN_PC1, PIN_PC2, PIN_PC3,
  #if defined(PIN_PC4) // these don't exist until 48-pin parts
    PIN_PC4,    PIN_PC5,
  #else
    NOT_A_PIN,  NOT_A_PIN,
  #endif
  #if (defined(MVIO) && _AVR_PINCOUNT < 48) //no PD0 on MVIO parts with under 48 pins.
    NOT_A_PIN,
  #else
    PIN_PD0,
  #endif
  #if (_AVR_PINCOUNT >= 28) // and parts with 20 or 14 pins only have the second half of portd
    PIN_PD1,    PIN_PD2,    PIN_PD3,
  #else
    NOT_A_PIN,  NOT_A_PIN,  NOT_A_PIN,
  #endif
  PIN_PD4, PIN_PD5,
  #if defined(PIN_PE1)
    PIN_PE0,    PIN_PE1,    PIN_PE2,    PIN_PE3,
  #else
    NOT_A_PIN,  NOT_A_PIN,  NOT_A_PIN,  NOT_A_PIN,
  #endif
  #if defined(PIN_PE4)
    PIN_PE4,    PIN_PE5,
  #else
    NOT_A_PIN,  NOT_A_PIN,
  #endif
  #if defined(PIN_PF1) //PF0 and PF1 come as a mated pair
    PIN_PF0,    PIN_PF1,
  #else
    NOT_A_PIN,  NOT_A_PIN,
  #endif
  #if defined(PIN_PF2) // if it has PF2, it has the rest of the normal pins on PF.
    PIN_PF2,    PIN_PF3,    PIN_PF4,    PIN_PF5,
  #else
    NOT_A_PIN,  NOT_A_PIN,  NOT_A_PIN,  NOT_A_PIN,
  #endif
  #if defined(PORTG)
    PIN_PG0,    PIN_PG1,    PIN_PG2,    PIN_PG3,    PIN_PG4,    PIN_PG5,
  #else
    NOT_A_PIN,  NOT_A_PIN,  NOT_A_PIN,  NOT_A_PIN,  NOT_A_PIN,  NOT_A_PIN
  #endif
};
#if defined(TCA1)
const PROGMEM_MAPPED uint8_t TCA1pinsets[] = {
  PIN_PB0, PIN_PB1, PIN_PB2, PIN_PB3, PIN_PB4, PIN_PB5,
  PIN_PC4, PIN_PC5, PIN_PC6, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
  #if _AVR_PINCOUNT > 48
    #if !defined(MAXREG)
      PIN_PE4, PIN_PE5, PIN_PE6, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
      PIN_PG0, PIN_PG1, PIN_PG2, PIN_PG3, PIN_PG4, PIN_PG5,
    #else
      PIN_PE4, PIN_PE5, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
      PIN_PG0, PIN_PG1, PIN_PG2, PIN_PG3, PIN_PG4, PIN_PG5,
     #endif
  #else
    NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
    NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
  #endif
  #if defined(__AVR_EA__)
    PIN_PA4, PIN_PA5, PIN_PA6, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
    PIN_PD4, PIN_PD5, PIN_PD6, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
  #else
    NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
    NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
  #endif
};

#endif
const PROGMEM_MAPPED uint8_t TCBpinsets[] = {
  #if defined(PIN_PA2)
    PIN_PA2,
  #else
    NOT_A_PIN,
  #endif
  #if defined(PIN_PF4)
    PIN_PF4,
  #else
    NOT_A_PIN,
  #endif
  #if defined(PIN_PA3)
    PIN_PA3,
  #else
    NOT_A_PIN,
  #endif
  #if defined(PIN_PF5)
    PIN_PF5,
  #else
    NOT_A_PIN,
  #endif
  #if defined(PIN_PC0)
    PIN_PC0,
  #else
    NOT_A_PIN,
  #endif
  #if defined(PIN_PB4)
    PIN_PB4,
  #else
    NOT_A_PIN,
  #endif
  #if defined(PIN_PB5)
    PIN_PB5,
  #else
    NOT_A_PIN,
  #endif
  #if defined(PIN_PC1)
    PIN_PC1,
  #else
    NOT_A_PIN,
  #endif
  #if defined(PIN_PG3)
    PIN_PG3,
  #else
    NOT_A_PIN,
  #endif
  #if defined(PIN_PC6)
    PIN_PC6,
  #else
    NOT_A_PIN,
  #endif
};

const PROGMEM_MAPPED uint8_t TCD0pinsets[] = {
  #if defined(PIN_PA4)
    PIN_PA4, PIN_PA5, PIN_PA6, PIN_PA7,
  #else
    NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
  #endif
  #if !(defined(__AVR_DA__) || defined(__AVR_DB__))
    #if defined(PIN_PB4)
      PIN_PB4, PIN_PB5,
    #else
      NOT_A_PIN, NOT_A_PIN,
    #endif
    #if defined(PIN_PB6)
      PIN_PB6, PIN_PB7,
    #else
      NOT_A_PIN, NOT_A_PIN,
    #endif
    #if defined(PIN_PF1)
      PIN_PF0, PIN_PF1,
    #else
      NOT_A_PIN, NOT_A_PIN,
    #endif
    #if defined(PIN_PF2)
      PIN_PF2, PIN_PF3,
    #else
      NOT_A_PIN, NOT_A_PIN,
    #endif
    #if defined(PORTG)
      PIN_PG4, PIN_PG5, PIN_PG6, PIN_PG7,
    #else
      NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
    #endif
    #if defined(__AVR_DD__)
      #if _AVR_PINCOUNT > 14
        PIN_PA4, PIN_PA5, PIN_PD4, PIN_PD5,
      #else
        NOT_A_PIN, NOT_A_PIN, PIN_PD4, PIN_PD5,
      #endif
    #else
      NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN
    #endif
  #else
    NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
    NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
    NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
    NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN
  #endif
};
/* TODO:
// This is a 128b table. The majority of it is "reserved" mappings. Obvious assumptions were made.
const PROGMEM_MAPPED uint8_t TCE0pinsets[] = {
  #if (CLOCK_SOURCE == 0)
    PIN_PA0, PIN_PA1,
  #else
    NOT_A_PIN, NOT_A_PIN,
  #endif
  #if defined(PIN_PA2)
    PIN_PA2, PIN_PA3, PIN_PA4, PIN_PA5, PIN_PA6, PIN_PA7,
  #else
    NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
  #endif
  #if defined (PORTB)
    PIN_PB0, PIN_PB1, PIN_PB2, PIN_PB3, PIN_PB4, PIN_PB5,
    #if defined(PIN_PB6)
      PIN_PB6, PIN_PB7,
    #else
      NOT_A_PIN, NOT_A_PIN,
    #endif
  #else
    NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
  #endif
  #if defined(PORTC)
    #if defined(MVIO) && _AVR_PINCOUNT < 28
      NOT_A_PIN,
    #else
      PIN_PC0,
    #endif
    PIN_PC1, PIN_PC2, PIN_PC3,
    #if defined(PIN_PC4)
      PIN_PC4, PIN_PC5, PIN_PC6, PIN_PC7,
    #else
      NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
    #endif
  #endif
  #if defined(PORTD)
    #if defined(MVIO) && _AVR_PINCOUNT < 48
      NOT_A_PIN,
    #else
      PIN_PD0,
    #endif
    #if _AVR_PINCOUNT > 20
      PIN_PD1, PIN_PD2, PIN_PD3,
    #else
      NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
    #endif
    PIN_PD4, PIN_PD5, PIN_PD6, PIN_PD7,
  #else
    NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
  #endif
  #if defined(PORTE)
    PIN_PE0, PIN_PE1, PIN_PE2, PIN_PE3,
    #if defined(PIN_PE4)
      PIN_PE4, PIN_PE5, PIN_PE6, PIN_PE7,
    #else
      NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
  #else
    NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
  #endif
  #if defined(PORTF)
    #if !defined(PIN_PF1)
      NOT_A_PIN, NOT_A_PIN,
    #else
      PIN_PF0, PIN_PF1,
    #endif
    #if !defined(PIN_PF2)
      NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
    #else
      PIN_PF2, PIN_PF3, PIN_PF4, PIN_PF5,
    #endif
    NOT_A_PIN, NOT_A_PIN,
  #else
    NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
  #endif
  #if defined(PORTG)
    PIN_PG0, PIN_PG1, PIN_PG2, PIN_PG3, PIN_PG4, PIN_PG5, PIN_PG6, PIN_PG7,
  #else
    NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
  #endif
  NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
  // C2
  #if (CLOCK_SOURCE == 0)
    PIN_PA0, PIN_PA1,
  #else
    NOT_A_PIN, NOT_A_PIN,
  #endif
      #if defined(MVIO) && _AVR_PINCOUNT < 28
      NOT_A_PIN,
    #else
      PIN_PC0,
    #endif
  PIN_PC1, PIN_PC2, PIN_PC3, NOT_A_PIN, NOT_A_PIN,
  // A2
  #if defined(PIN_PA2)
    PIN_PA2, PIN_PA3, PIN_PA4, PIN_PA5, PIN_PA6, PIN_PA7, NOT_A_PIN, NOT_A_PIN,
  #else
    NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
  #endif
  NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
  NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
  NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
  NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
  NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
  NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN, NOT_A_PIN,
};

const PROGMEM_MAPPED uint8_t TCF0pinsets[] = {
  #if (CLOCK_SOURCE == 0)
    PIN_PA0, PIN_PA1,
  #else
    NOT_A_PIN, NOT_A_PIN,
  #endif
  #if defined(PIN_PA6)
    PIN_PA6, PIN_PA7,
  #else
    NOT_A_PIN, NOT_A_PIN,
  #endif
  #if defined(PIN_PF4)
    PIN_PF4, PIN_PF5,
  #else
    NOT_A_PIN, NOT_A_PIN,
  #endif
  NOT_A_PIN, NOT_A_PIN,
  NOT_A_PIN, NOT_A_PIN,
  NOT_A_PIN, NOT_A_PIN,
  NOT_A_PIN, NOT_A_PIN,
  NOT_A_PIN, NOT_A_PIN
};
*/
