/* device_timer_pins.h - a part of DxCore
 *
 * For each possible pin on a portmux option this defines a macro starting
 * with PIN_(name of associated _gc constant) as the pin that the peripheral
 * will use for that fuhction - if it is present - and NOT_A_PIN if it is not
 * present. Nothing is defined if a peripheral doesn't exist - you still have
 * to test if TCA1 exists, for example; this is logically coherent, because
 * even if a timer had no pins connected to it for PWM, you could STILL USE
 * IT'S WAVEFORM OUTPUT with the CCL in order to something, including PWM */

#ifndef Device_Timer_Pins_h
  #define Device_Timer_Pins_h







#if defined(__AVR_EA__) || defined(__AVR_SD__)
  #define _HAS_TCA1_PORTAD
#endif

#if defined(TCD0) && !(defined(__AVR_DA__)||defined(__AVR_DB__))
  #define _HAS_TCD0_PORTD
#endif


/* near universal */
#if !defined(__AVR_DU__)
  #if defined(TCA0)
    #if(!defined(MVIO) || _AVR_PINCOUNT > 28)
      #define PIN_TCA0_WO0_ALT2         PIN_PC0
    #else
      #define PIN_TCA0_WO0_ALT2         NOT_A_PIN
    #endif
    #define PIN_TCA0_WO1_ALT2           PIN_PC1
    #define PIN_TCA0_WO2_ALT2           PIN_PC2
  #endif
  #if defined(TCB2)
    #if(!defined(MVIO) || _AVR_PINCOUNT > 28)
      #define PIN_TCB2_WO_DEFAULT       PIN_PC0
    #else
      #define PIN_TCB2_WO_DEFAULT       NOT_A_PIN
    #endif
  #endif
  #if defined(TCB3)
    #define PIN_TCB3_WO_ALT             PIN_PC1
  #endif
  #if defined(TCE0)
    #define PIN_TCE0_WO1_PORTC          PIN_PC1
    #define PIN_TCE0_WO2_PORTC          PIN_PC2
    #define PIN_TCE0_WO3_PORTC2         PIN_PC1
    #define PIN_TCE0_WO4_PORTC2         PIN_PC2
  #endif
  #if defined(TCE0)
    #if(!defined(MVIO) || _AVR_PINCOUNT > 28)
      #define PIN_TCE0_WO2_PORTC2       PIN_PC0
      #define PIN_TCE0_WO0_PORTC        PIN_PC0
    #else
      #define PIN_TCE0_WO2_PORTC2       NOT_A_PIN
      #define PIN_TCE0_WO0_PORTC        NOT_A_PIN
    #endif
  #endif
#else // it is a DU, so we don't have to check peripherals DU doesn't have
  #if defined(TCA0)
    #define PIN_TCA0_WO0_ALT2           NOT_A_PIN
    #define PIN_TCA0_WO1_ALT2           NOT_A_PIN
    #define PIN_TCA0_WO2_ALT2           NOT_A_PIN
  #endif
  #if defined(TCB2)
    #define PIN_TCB2_WO_DEFAULT         NOT_A_PIN
  #endif
  #if defined(TCB3)
    #define PIN_TCB3_WO_ALT             NOT_A_PIN
  #endif
#endif

#if defined(TCF0)
  #if (((CLOCK_SOURCE & 0x03) == 0))
    #define PIN_TCF0_WO0_DEFAULT          PIN_PA0
  #else
    #define PIN_TCF0_WO0_DEFAULT          NOT_A_PIN
  #endif
  #if (((CLOCK_SOURCE & 0x03) == 1))
    #define PIN_TCF0_WO1_DEFAULT          NOT_A_PIN
  #else
    #define PIN_TCF0_WO1_DEFAULT          PIN_PA1
  #endif
#endif

#if defined(TCA0)
  #if (((CLOCK_SOURCE & 0x03) == 0)) // PA0 only available when using internal oscillator.
    #define PIN_TCA0_WO0_DEFAULT    PIN_PA0
  #else
    #define PIN_TCA0_WO0_DEFAULT    NOT_A_PIN
  #endif
  #if (((CLOCK_SOURCE & 0x03) == 1)) // crystal doesn't have PA1 available either
    #define PIN_TCA0_WO1_DEFAULT    NOT_A_PIN
  #else
    #define PIN_TCA0_WO1_DEFAULT    PIN_PA1
  #endif
#endif


#if defined(TCA0)
  #define PIN_TCA0_WO3_ALT2             PIN_PC3 // 0-2 handled above
  #define PIN_TCA0_WO4_ALT3             PIN_PD4
  #define PIN_TCA0_WO5_ALT3             PIN_PD5
#endif
#if defined(TCA1) && defined(_HAS_TCA1_PORTAD)
  #define PIN_TCA1_WO0_ALT5             PIN_PD4
  #define PIN_TCA1_WO1_ALT5             PIN_PD5
  #define PIN_TCA1_WO2_ALT5             PIN_PD6
#endif
#if defined(TCD0) && defined(_HAS_TCD0_PORTD)
  #define PIN_TCD0_WOC_ALT4             PIN_PD4
  #define PIN_TCD0_WOD_ALT4             PIN_PD5
#endif
#if defined(TCE0)
  #define PIN_TCE0_WO5_PORTC2           PIN_PC3
  #define PIN_TCE0_WO3_PORTC            PIN_PC3
  #define PIN_TCE0_WO4_PORTD            PIN_PD4
  #define PIN_TCE0_WO5_PORTD            PIN_PD5
  #define PIN_TCE0_WO6_PORTD            PIN_PD6
  #define PIN_TCE0_WO7_PORTD            PIN_PD7
#endif



#if _AVR_PINCOUNT >=20

  #if defined(TCA0)
    #define PIN_TCA0_WO2_DEFAULT        PIN_PA2
    #define PIN_TCA0_WO3_DEFAULT        PIN_PA3
    #define PIN_TCA0_WO4_DEFAULT        PIN_PA4
    #define PIN_TCA0_WO5_DEFAULT        PIN_PA5
  #endif
  #if defined(TCA1) && defined(_HAS_TCA1_PORTAD)
    #define PIN_TCA1_WO0_ALT4           PIN_PA4
    #define PIN_TCA1_WO1_ALT4           PIN_PA5
    #define PIN_TCA1_WO2_ALT4           PIN_PA6
  #endif
  #if defined(TCD0)
    #if defined(_HAS_TCD0_PORTD)
      #define PIN_TCD0_WOA_ALT4         PIN_PA4
      #define PIN_TCD0_WOB_ALT4         PIN_PA5
    #endif
    #define PIN_TCD0_WOA_DEFAULT        PIN_PA4
    #define PIN_TCD0_WOB_DEFAULT        PIN_PA5
    #define PIN_TCD0_WOC_DEFAULT        PIN_PA6
    #define PIN_TCD0_WOD_DEFAULT        PIN_PA7
  #endif
  #if defined(TCB0)
      #define PIN_TCB0_WO_DEFAULT       PIN_PA2
  #endif
  #if defined(TCB1)
      #define PIN_TCB1_WO_DEFAULT       PIN_PA3
  #endif
  #if defined(TCE0)
    #define PIN_TCE0_WO2_PORTA          PIN_PA2
    #define PIN_TCE0_WO3_PORTA          PIN_PA3
    #define PIN_TCE0_WO4_PORTA          PIN_PA4
    #define PIN_TCE0_WO5_PORTA          PIN_PA5
    #define PIN_TCE0_WO6_PORTA          PIN_PA6
    #define PIN_TCE0_WO7_PORTA          PIN_PA7
    #define PIN_TCE0_WO0_PORTA2         PIN_PA2
    #define PIN_TCE0_WO1_PORTA2         PIN_PA3
    #define PIN_TCE0_WO2_PORTA2         PIN_PA4
    #define PIN_TCE0_WO3_PORTA2         PIN_PA5
    #define PIN_TCE0_WO4_PORTA2         PIN_PA6
    #define PIN_TCE0_WO5_PORTA2         PIN_PA7
  #endif
  #if defined(TCF0)
      #define PIN_TCF0_WO0_ALT1         PIN_PA6
      #define PIN_TCF0_WO1_ALT1         PIN_PA7
  #endif
#else
  #if defined(TCA0)
    #define PIN_TCA0_WO2_DEFAULT        NOT_A_PIN
    #define PIN_TCA0_WO3_DEFAULT        NOT_A_PIN
    #define PIN_TCA0_WO4_DEFAULT        NOT_A_PIN
    #define PIN_TCA0_WO5_DEFAULT        NOT_A_PIN
  #endif
  #if defined(TCA1) && defined(_HAS_TCA1_PORTAD)
    #define PIN_TCA1_WO0_ALT4           NOT_A_PIN
    #define PIN_TCA1_WO1_ALT4           NOT_A_PIN
    #define PIN_TCA1_WO2_ALT4           NOT_A_PIN
  #endif
  #if defined(TCD0)
    #if defined(_HAS_TCD0_PORTD)
      #define PIN_TCD0_WOA_ALT4         NOT_A_PIN
      #define PIN_TCD0_WOB_ALT4         NOT_A_PIN
    #endif
    #define PIN_TCD0_WOA_DEFAULT        NOT_A_PIN
    #define PIN_TCD0_WOB_DEFAULT        NOT_A_PIN
    #define PIN_TCD0_WOC_DEFAULT        NOT_A_PIN
    #define PIN_TCD0_WOD_DEFAULT        NOT_A_PIN
  #endif
  #if defined(TCB0)
      #define PIN_TCB0_WO_DEFAULT       NOT_A_PIN
  #endif
  #if defined(TCB1)
      #define PIN_TCB1_WO_DEFAULT       NOT_A_PIN
  #endif
  #if defined(TCE0)
    #define PIN_TCE0_WO2_PORTA          NOT_A_PIN
    #define PIN_TCE0_WO3_PORTA          NOT_A_PIN
    #define PIN_TCE0_WO4_PORTA          NOT_A_PIN
    #define PIN_TCE0_WO5_PORTA          NOT_A_PIN
    #define PIN_TCE0_WO6_PORTA          NOT_A_PIN
    #define PIN_TCE0_WO7_PORTA          NOT_A_PIN
    #define PIN_TCE0_WO0_PORTA2         NOT_A_PIN
    #define PIN_TCE0_WO1_PORTA2         NOT_A_PIN
    #define PIN_TCE0_WO2_PORTA2         NOT_A_PIN
    #define PIN_TCE0_WO3_PORTA2         NOT_A_PIN
    #define PIN_TCE0_WO4_PORTA2         NOT_A_PIN
    #define PIN_TCE0_WO5_PORTA2         NOT_A_PIN
  #endif
  #if defined(TCF0)
      #define PIN_TCF0_WO0_ALT1         NOT_A_PIN
      #define PIN_TCF0_WO1_ALT1         NOT_A_PIN
  #endif
#endif



#if _AVR_PINCOUNT >= 28
  #if defined(TCA0)
    #if !defined(MVIO) || _AVR_PINCOUNT > 32
      #define PIN_TCA0_WO0_ALT3         PIN_PD0
    #else
      #define PIN_TCA0_WO0_ALT3         NOT_A_PIN
    #endif
    #define PIN_TCA0_WO1_ALT3           PIN_PD1
    #define PIN_TCA0_WO2_ALT3           PIN_PD2
    #define PIN_TCA0_WO3_ALT3           PIN_PD3
    #define PIN_TCA0_WO0_ALT5           PIN_PF0
    #define PIN_TCA0_WO1_ALT5           PIN_PF1
  #endif
  #if defined(TCD0)
    #define PIN_TCD0_WOA_ALT2           PIN_PF0
    #define PIN_TCD0_WOB_ALT2           PIN_PF1
  #endif
  #if defined(TCE0)
    #if !defined(MVIO) || _AVR_PINCOUNT > 32
      #define PIN_TCE0_WO0_PORTD        PIN_PD0
    #else
      #define PIN_TCE0_WO0_PORTD        NOT_A_PIN
    #endif
    #define PIN_TCE0_WO1_PORTD          PIN_PD1
    #define PIN_TCE0_WO2_PORTD          PIN_PD2
    #define PIN_TCE0_WO3_PORTD          PIN_PD3
    #define PIN_TCE0_WO0_PORTF          PIN_PF0
    #define PIN_TCE0_WO1_PORTF          PIN_PF1
  #endif
#else
  #if defined(TCA0)
    #define PIN_TCA0_WO0_ALT3           NOT_A_PIN
    #define PIN_TCA0_WO1_ALT3           NOT_A_PIN
    #define PIN_TCA0_WO2_ALT3           NOT_A_PIN
    #define PIN_TCA0_WO3_ALT3           NOT_A_PIN
    #define PIN_TCA0_WO0_ALT5           NOT_A_PIN
    #define PIN_TCA0_WO1_ALT5           NOT_A_PIN
  #endif
  #if defined(TCD0)
    #define PIN_TCD0_WOA_ALT2           NOT_A_PIN
    #define PIN_TCD0_WOB_ALT2           NOT_A_PIN
  #endif
  #if defined(TCE0)
    #if !defined(MVIO)
      #define PIN_TCE0_WO0_PORTD        NOT_A_PIN
    #endif
    #define PIN_TCE0_WO1_PORTD          NOT_A_PIN
    #define PIN_TCE0_WO2_PORTD          NOT_A_PIN
    #define PIN_TCE0_WO3_PORTD          NOT_A_PIN
    #define PIN_TCE0_WO0_PORTF          NOT_A_PIN
    #define PIN_TCE0_WO1_PORTF          NOT_A_PIN
  #endif
#endif


#if _AVR_PINCOUNT >= 32
  #if defined(TCA0)
    #define PIN_TCA0_WO2_ALT5           PIN_PF2
    #define PIN_TCA0_WO3_ALT5           PIN_PF3
    #define PIN_TCA0_WO4_ALT5           PIN_PF4
    #define PIN_TCA0_WO5_ALT5           PIN_PF5
  #endif
  #if defined(TCB0)
    #define PIN_TCB0_WO_ALT             PIN_PF4
  #endif
  #if defined(TCB1)
    #define PIN_TCB1_WO_ALT             PIN_PF5
  #endif
  #if defined(TCD0)
    #define PIN_TCD0_WOC_ALT2           PIN_PF2
    #define PIN_TCD0_WOD_ALT2           PIN_PF3
  #endif
  #if defined(TCE0)
    #define PIN_TCE0_WO2_PORTF          PIN_PF2
    #define PIN_TCE0_WO3_PORTF          PIN_PF3
    #define PIN_TCE0_WO4_PORTF          PIN_PF4
    #define PIN_TCE0_WO5_PORTF          PIN_PF5
  #endif
  #if defined(TCF0)
    #define PIN_TCF0_WO0_ALT2           PIN_PF4
    #define PIN_TCF0_WO1_ALT2           PIN_PF5
  #endif
#else
  #if defined(TCA0)
    #define PIN_TCA0_WO2_ALT5           NOT_A_PIN
    #define PIN_TCA0_WO3_ALT5           NOT_A_PIN
    #define PIN_TCA0_WO4_ALT5           NOT_A_PIN
    #define PIN_TCA0_WO5_ALT5           NOT_A_PIN
  #endif
  #if defined(TCB0)
    #define PIN_TCB0_WO_ALT             NOT_A_PIN
  #endif
  #if defined(TCB1)
    #define PIN_TCB1_WO_ALT             NOT_A_PIN
  #endif
  #if defined(TCD0)
    #define PIN_TCD0_WOC_ALT2           NOT_A_PIN
    #define PIN_TCD0_WOD_ALT2           NOT_A_PIN
  #endif
  #if defined(TCE0)
    #define PIN_TCE0_WO2_PORTF          NOT_A_PIN
    #define PIN_TCE0_WO3_PORTF          NOT_A_PIN
    #define PIN_TCE0_WO4_PORTF          NOT_A_PIN
    #define PIN_TCE0_WO5_PORTF          NOT_A_PIN
  #endif
  #if defined(TCF0)
    #define PIN_TCF0_WO0_ALT2           NOT_A_PIN
    #define PIN_TCF0_WO1_ALT2           NOT_A_PIN
  #endif
#endif


#if _AVR_PINCOUNT >= 48
  #if defined(TCA0)
    #define PIN_TCA0_WO0_ALT1           PIN_PB0
    #define PIN_TCA0_WO1_ALT1           PIN_PB1
    #define PIN_TCA0_WO2_ALT1           PIN_PB2
    #define PIN_TCA0_WO3_ALT1           PIN_PB3
    #define PIN_TCA0_WO4_ALT1           PIN_PB4
    #define PIN_TCA0_WO5_ALT1           PIN_PB5
    #define PIN_TCA0_WO4_ALT2           PIN_PC4
    #define PIN_TCA0_WO5_ALT2           PIN_PC5
    #define PIN_TCA0_WO0_ALT4           PIN_PE0
    #define PIN_TCA0_WO1_ALT4           PIN_PE1
    #define PIN_TCA0_WO2_ALT4           PIN_PE2
    #define PIN_TCA0_WO3_ALT4           PIN_PE3
  #endif
  #if defined(TCA1)
    #define PIN_TCA1_WO0_ALT1           PIN_PC4
    #define PIN_TCA1_WO1_ALT1           PIN_PC5
    #define PIN_TCA1_WO2_ALT1           PIN_PC6
    #define PIN_TCA1_WO0_DEFAULT        PIN_PB0
    #define PIN_TCA1_WO1_DEFAULT        PIN_PB1
    #define PIN_TCA1_WO2_DEFAULT        PIN_PB2
    #define PIN_TCA1_WO3_DEFAULT        PIN_PB3
    #define PIN_TCA1_WO4_DEFAULT        PIN_PB4
    #define PIN_TCA1_WO5_DEFAULT        PIN_PB5
  #endif
  #if defined(TCB2)
    #define PIN_TCB2_WO_ALT             PIN_PB4
  #endif
  #if defined(TCB3)
    #define PIN_TCB3_WO_DEFAULT         PIN_PB5
  #endif
  #if defined(TCD0)
    #define PIN_TCD0_WOA_ALT1           PIN_PB4
    #define PIN_TCD0_WOB_ALT1           PIN_PB5
  #endif
  #if defined(TCE0)
    #define PIN_TCE0_WO0_PORTB          PIN_PB0
    #define PIN_TCE0_WO1_PORTB          PIN_PB1
    #define PIN_TCE0_WO2_PORTB          PIN_PB2
    #define PIN_TCE0_WO3_PORTB          PIN_PB3
    #define PIN_TCE0_WO4_PORTB          PIN_PB4
    #define PIN_TCE0_WO5_PORTB          PIN_PB5
    #define PIN_TCE0_WO4_PORTC          PIN_PC4
    #define PIN_TCE0_WO5_PORTC          PIN_PC5
    #define PIN_TCE0_WO6_PORTC          PIN_PC6
    #define PIN_TCE0_WO7_PORTC          PIN_PC7
    #define PIN_TCE0_WO0_PORTE          PIN_PE0
    #define PIN_TCE0_WO1_PORTE          PIN_PE1
    #define PIN_TCE0_WO2_PORTE          PIN_PE2
    #define PIN_TCE0_WO3_PORTE          PIN_PE3
  #endif
#else
  #if defined(TCA0)
    #define PIN_TCA0_WO0_ALT1           NOT_A_PIN
    #define PIN_TCA0_WO1_ALT1           NOT_A_PIN
    #define PIN_TCA0_WO2_ALT1           NOT_A_PIN
    #define PIN_TCA0_WO3_ALT1           NOT_A_PIN
    #define PIN_TCA0_WO4_ALT1           NOT_A_PIN
    #define PIN_TCA0_WO5_ALT1           NOT_A_PIN
    #define PIN_TCA0_WO4_ALT2           NOT_A_PIN
    #define PIN_TCA0_WO5_ALT2           NOT_A_PIN
    #define PIN_TCA0_WO0_ALT4           NOT_A_PIN
    #define PIN_TCA0_WO1_ALT4           NOT_A_PIN
    #define PIN_TCA0_WO2_ALT4           NOT_A_PIN
    #define PIN_TCA0_WO3_ALT4           NOT_A_PIN
  #endif
  #if defined(TCA1)
    #define PIN_TCA1_WO0_ALT1           NOT_A_PIN
    #define PIN_TCA1_WO1_ALT1           NOT_A_PIN
    #define PIN_TCA1_WO2_ALT1           NOT_A_PIN
    #define PIN_TCA1_WO0_DEFAULT        NOT_A_PIN
    #define PIN_TCA1_WO1_DEFAULT        NOT_A_PIN
    #define PIN_TCA1_WO2_DEFAULT        NOT_A_PIN
    #define PIN_TCA1_WO3_DEFAULT        NOT_A_PIN
    #define PIN_TCA1_WO4_DEFAULT        NOT_A_PIN
    #define PIN_TCA1_WO5_DEFAULT        NOT_A_PIN
  #endif
  #if defined(TCB2)
    #define PIN_TCB2_WO_ALT             NOT_A_PIN
  #endif
  #if defined(TCB3)
    #define PIN_TCB3_WO_DEFAULT         NOT_A_PIN
  #endif
  #if defined(TCD0)
    #define PIN_TCD0_WOA_ALT1           NOT_A_PIN
    #define PIN_TCD0_WOB_ALT1           NOT_A_PIN
  #endif
  #if defined(TCE0)
    #define PIN_TCE0_WO0_PORTB          NOT_A_PIN
    #define PIN_TCE0_WO1_PORTB          NOT_A_PIN
    #define PIN_TCE0_WO2_PORTB          NOT_A_PIN
    #define PIN_TCE0_WO3_PORTB          NOT_A_PIN
    #define PIN_TCE0_WO4_PORTB          NOT_A_PIN
    #define PIN_TCE0_WO5_PORTB          NOT_A_PIN
    #define PIN_TCE0_WO4_PORTC          NOT_A_PIN
    #define PIN_TCE0_WO5_PORTC          NOT_A_PIN
    #define PIN_TCE0_WO6_PORTC          NOT_A_PIN
    #define PIN_TCE0_WO7_PORTC          NOT_A_PIN
    #define PIN_TCE0_WO0_PORTE          NOT_A_PIN
    #define PIN_TCE0_WO1_PORTE          NOT_A_PIN
    #define PIN_TCE0_WO2_PORTE          NOT_A_PIN
    #define PIN_TCE0_WO3_PORTE          NOT_A_PIN
  #endif
#endif





#if _AVR_PINCOUNT == 64
  #if defined(TCA0)
    #define PIN_TCA0_WO4_ALT4           PIN_PE4
    #define PIN_TCA0_WO5_ALT4           PIN_PE5
    #define PIN_TCA0_WO0_ALT6           PIN_PG0
    #define PIN_TCA0_WO1_ALT6           PIN_PG1
    #define PIN_TCA0_WO2_ALT6           PIN_PG2
    #define PIN_TCA0_WO3_ALT6           PIN_PG3
    #define PIN_TCA0_WO4_ALT6           PIN_PG4
    #define PIN_TCA0_WO5_ALT6           PIN_PG5
  #endif
  #if defined(TCA1)
    #define PIN_TCA1_WO0_ALT2           PIN_PE4
    #define PIN_TCA1_WO1_ALT2           PIN_PE5
    #define PIN_TCA1_WO2_ALT2           PIN_PE6
    #define PIN_TCA1_WO0_ALT3           PIN_PG0
    #define PIN_TCA1_WO1_ALT3           PIN_PG1
    #define PIN_TCA1_WO2_ALT3           PIN_PG2
    #define PIN_TCA1_WO3_ALT3           PIN_PG3
    #define PIN_TCA1_WO4_ALT3           PIN_PG4
    #define PIN_TCA1_WO5_ALT3           PIN_PG5
  #endif
  #if defined(TCB4)
    #define PIN_TCB4_WO_DEFAULT         PIN_PG3
  #endif
  #if defined(TCD0)
    #define PIN_TCD0_WOC_ALT1           PIN_PB6
    #define PIN_TCD0_WOD_ALT1           PIN_PB7
    #define PIN_TCD0_WOA_ALT3           PIN_PG4
    #define PIN_TCD0_WOB_ALT3           PIN_PG5
    #define PIN_TCD0_WOC_ALT3           PIN_PG6
    #define PIN_TCD0_WOD_ALT3           PIN_PG7
  #endif
  #if defined(TCE0)
    #define PIN_TCE0_WO6_PORTB          PIN_PB6
    #define PIN_TCE0_WO7_PORTB          PIN_PB7
    #define PIN_TCE0_WO4_PORTE          PIN_PE4
    #define PIN_TCE0_WO5_PORTE          PIN_PE5
    #define PIN_TCE0_WO6_PORTE          PIN_PE6
    #define PIN_TCE0_WO7_PORTE          PIN_PE7
    #define PIN_TCE0_WO0_PORTG          PIN_PG0
    #define PIN_TCE0_WO1_PORTG          PIN_PG1
    #define PIN_TCE0_WO2_PORTG          PIN_PG2
    #define PIN_TCE0_WO3_PORTG          PIN_PG3
    #define PIN_TCE0_WO4_PORTG          PIN_PG4
    #define PIN_TCE0_WO5_PORTG          PIN_PG5
    #define PIN_TCE0_WO6_PORTG          PIN_PG6
    #define PIN_TCE0_WO7_PORTG          PIN_PG7
  #endif
#else
  #if defined(TCA0)
    #define PIN_TCA0_WO4_ALT4           NOT_A_PIN
    #define PIN_TCA0_WO5_ALT4           NOT_A_PIN
    #define PIN_TCA0_WO0_ALT6           NOT_A_PIN
    #define PIN_TCA0_WO1_ALT6           NOT_A_PIN
    #define PIN_TCA0_WO2_ALT6           NOT_A_PIN
    #define PIN_TCA0_WO3_ALT6           NOT_A_PIN
    #define PIN_TCA0_WO4_ALT6           NOT_A_PIN
    #define PIN_TCA0_WO5_ALT6           NOT_A_PIN
  #endif
  #if defined(TCA1)
    #define PIN_TCA1_WO0_ALT2           NOT_A_PIN
    #define PIN_TCA1_WO1_ALT2           NOT_A_PIN
    #define PIN_TCA1_WO2_ALT2           NOT_A_PIN
    #define PIN_TCA1_WO0_ALT3           NOT_A_PIN
    #define PIN_TCA1_WO1_ALT3           NOT_A_PIN
    #define PIN_TCA1_WO2_ALT3           NOT_A_PIN
    #define PIN_TCA1_WO3_ALT3           NOT_A_PIN
    #define PIN_TCA1_WO4_ALT3           NOT_A_PIN
    #define PIN_TCA1_WO5_ALT3           NOT_A_PIN
  #endif
  #if defined(TCB4)
    #define PIN_TCB4_WO_DEFAULT         NOT_A_PIN
  #endif
  #if defined(TCD0)
    #define PIN_TCD0_WOC_ALT1           NOT_A_PIN
    #define PIN_TCD0_WOD_ALT1           NOT_A_PIN
    #define PIN_TCD0_WOA_ALT3           NOT_A_PIN
    #define PIN_TCD0_WOB_ALT3           NOT_A_PIN
    #define PIN_TCD0_WOC_ALT3           NOT_A_PIN
    #define PIN_TCD0_WOD_ALT3           NOT_A_PIN
  #endif
  #if defined(TCE0)
    #define PIN_TCE0_WO6_PORTB          NOT_A_PIN
    #define PIN_TCE0_WO7_PORTB          NOT_A_PIN
    #define PIN_TCE0_WO4_PORTE          NOT_A_PIN
    #define PIN_TCE0_WO5_PORTE          NOT_A_PIN
    #define PIN_TCE0_WO6_PORTE          NOT_A_PIN
    #define PIN_TCE0_WO7_PORTE          NOT_A_PIN
    #define PIN_TCE0_WO0_PORTG          NOT_A_PIN
    #define PIN_TCE0_WO1_PORTG          NOT_A_PIN
    #define PIN_TCE0_WO2_PORTG          NOT_A_PIN
    #define PIN_TCE0_WO3_PORTG          NOT_A_PIN
    #define PIN_TCE0_WO4_PORTG          NOT_A_PIN
    #define PIN_TCE0_WO5_PORTG          NOT_A_PIN
    #define PIN_TCE0_WO6_PORTG          NOT_A_PIN
    #define PIN_TCE0_WO7_PORTG          NOT_A_PIN
  #endif
#endif






#endif
