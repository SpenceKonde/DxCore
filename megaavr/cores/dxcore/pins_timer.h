#ifndef pins_timer_H
  #ifdef MEGATINYCORE
    // TODO
  #else
    #ifdef TCA0
      /* Port A - all six channels */
      #ifdef PIN_PA0
        #define PIN_TCA0_WO0_PORTA PIN_PA0
      #else
        #define PIN_TCA0_WO0_PORTA NOT_A_PIN
      #endif
      #ifdef PIN_PA1
        #define PIN_TCA0_WO1_PORTA PIN_PA1
      #else
        #define PIN_TCA0_WO1_PORTA NOT_A_PIN
      #endif
      #ifdef PIN_PA2
        #define PIN_TCA0_WO2_PORTA PIN_PA2
      #else
        #define PIN_TCA0_WO2_PORTA NOT_A_PIN
      #endif
      #ifdef PIN_PA3
        #define PIN_TCA0_WO3_PORTA PIN_PA3
      #else
        #define PIN_TCA0_WO3_PORTA NOT_A_PIN
      #endif
      #ifdef PIN_PA4
        #define PIN_TCA0_WO4_PORTA PIN_PA4
      #else
        #define PIN_TCA0_WO4_PORTA NOT_A_PIN
      #endif
      #ifdef PIN_PA5
        #define PIN_TCA0_WO5_PORTA PIN_PA5
      #else
        #define PIN_TCA0_WO5_PORTA NOT_A_PIN
      #endif

      /* Port B - all six channels */
      #ifdef PIN_PB0
        #define PIN_TCA0_WO0_PORTB PIN_PB0
      #else
        #define PIN_TCA0_WO0_PORTB NOT_A_PIN
      #endif
      #ifdef PIN_PB1
        #define PIN_TCA0_WO1_PORTB PIN_PB1
      #else
        #define PIN_TCA0_WO1_PORTB NOT_A_PIN
      #endif
      #ifdef PIN_PB2
        #define PIN_TCA0_WO2_PORTB PIN_PB2
      #else
        #define PIN_TCA0_WO2_PORTB NOT_A_PIN
      #endif
      #ifdef PIN_PB3
        #define PIN_TCA0_WO3_PORTB PIN_PB3
      #else
        #define PIN_TCA0_WO3_PORTB NOT_A_PIN
      #endif
      #ifdef PIN_PB4
        #define PIN_TCA0_WO4_PORTB PIN_PB4
      #else
        #define PIN_TCA0_WO4_PORTB NOT_A_PIN
      #endif
      #ifdef PIN_PB5
        #define PIN_TCA0_WO5_PORTB PIN_PB5
      #else
        #define PIN_TCA0_WO5_PORTB NOT_A_PIN
      #endif

      /* Port C - all six channels */
      #ifdef PIN_PC0
        #define PIN_TCA0_WO0_PORTC PIN_PC0
      #else
        #define PIN_TCA0_WO0_PORTC NOT_A_PIN
      #endif
      #ifdef PIN_PC1
        #define PIN_TCA0_WO1_PORTC PIN_PC1
      #else
        #define PIN_TCA0_WO1_PORTC NOT_A_PIN
      #endif
      #ifdef PIN_PC2
        #define PIN_TCA0_WO2_PORTC PIN_PC2
      #else
        #define PIN_TCA0_WO2_PORTC NOT_A_PIN
      #endif
      #ifdef PIN_PC3
        #define PIN_TCA0_WO3_PORTC PIN_PC3
      #else
        #define PIN_TCA0_WO3_PORTC NOT_A_PIN
      #endif
      #ifdef PIN_PC4
        #define PIN_TCA0_WO4_PORTC PIN_PC4
      #else
        #define PIN_TCA0_WO4_PORTC NOT_A_PIN
      #endif
      #ifdef PIN_PC5
        #define PIN_TCA0_WO5_PORTC PIN_PC5
      #else
        #define PIN_TCA0_WO5_PORTC NOT_A_PIN
      #endif

      /* Port D - all six channels */
      #ifdef PIN_PD0
        #define PIN_TCA0_WO0_PORTD PIN_PD0
      #else
        #define PIN_TCA0_WO0_PORTD NOT_A_PIN
      #endif
      #ifdef PIN_PD1
        #define PIN_TCA0_WO1_PORTD PIN_PD1
      #else
        #define PIN_TCA0_WO1_PORTD NOT_A_PIN
      #endif
      #ifdef PIN_PD2
        #define PIN_TCA0_WO2_PORTD PIN_PD2
      #else
        #define PIN_TCA0_WO2_PORTD NOT_A_PIN
      #endif
      #ifdef PIN_PD3
        #define PIN_TCA0_WO3_PORTD PIN_PD3
      #else
        #define PIN_TCA0_WO3_PORTD NOT_A_PIN
      #endif
      #ifdef PIN_PD4
        #define PIN_TCA0_WO4_PORTD PIN_PD4
      #else
        #define PIN_TCA0_WO4_PORTD NOT_A_PIN
      #endif
      #ifdef PIN_PD5
        #define PIN_TCA0_WO5_PORTD PIN_PD5
      #else
        #define PIN_TCA0_WO5_PORTD NOT_A_PIN
      #endif

      /* Port E - all six channels */
      #ifdef PIN_PE0
        #define PIN_TCA0_WO0_PORTE PIN_PE0
      #else
        #define PIN_TCA0_WO0_PORTE NOT_A_PIN
      #endif
      #ifdef PIN_PE1
        #define PIN_TCA0_WO1_PORTE PIN_PE1
      #else
        #define PIN_TCA0_WO1_PORTE NOT_A_PIN
      #endif
      #ifdef PIN_PE2
        #define PIN_TCA0_WO2_PORTE PIN_PE2
      #else
        #define PIN_TCA0_WO2_PORTE NOT_A_PIN
      #endif
      #ifdef PIN_PE3
        #define PIN_TCA0_WO3_PORTE PIN_PE3
      #else
        #define PIN_TCA0_WO3_PORTE NOT_A_PIN
      #endif
      #ifdef PIN_PE4
        #define PIN_TCA0_WO4_PORTE PIN_PE4
      #else
        #define PIN_TCA0_WO4_PORTE NOT_A_PIN
      #endif
      #ifdef PIN_PE5
        #define PIN_TCA0_WO5_PORTE PIN_PE5
      #else
        #define PIN_TCA0_WO5_PORTE NOT_A_PIN
      #endif

      /* Port F - all six channels */
      #ifdef PIN_PF0
        #define PIN_TCA0_WO0_PORTF PIN_PF0
      #else
        #define PIN_TCA0_WO0_PORTF NOT_A_PIN
      #endif
      #ifdef PIN_PF1
        #define PIN_TCA0_WO1_PORTF PIN_PF1
      #else
        #define PIN_TCA0_WO1_PORTF NOT_A_PIN
      #endif
      #ifdef PIN_PF2
        #define PIN_TCA0_WO2_PORTF PIN_PF2
      #else
        #define PIN_TCA0_WO2_PORTF NOT_A_PIN
      #endif
      #ifdef PIN_PF3
        #define PIN_TCA0_WO3_PORTF PIN_PF3
      #else
        #define PIN_TCA0_WO3_PORTF NOT_A_PIN
      #endif
      #ifdef PIN_PF4
        #define PIN_TCA0_WO4_PORTF PIN_PF4
      #else
        #define PIN_TCA0_WO4_PORTF NOT_A_PIN
      #endif
      #ifdef PIN_PF5
        #define PIN_TCA0_WO5_PORTF PIN_PF5
      #else
        #define PIN_TCA0_WO5_PORTF NOT_A_PIN
      #endif

      /* Port G - all six channels */
      #ifdef PIN_PG0
        #define PIN_TCA0_WO0_PORTG PIN_PG0
      #else
        #define PIN_TCA0_WO0_PORTG NOT_A_PIN
      #endif
      #ifdef PIN_PG1
        #define PIN_TCA0_WO1_PORTG PIN_PG1
      #else
        #define PIN_TCA0_WO1_PORTG NOT_A_PIN
      #endif
      #ifdef PIN_PG2
        #define PIN_TCA0_WO2_PORTG PIN_PG2
      #else
        #define PIN_TCA0_WO2_PORTG NOT_A_PIN
      #endif
      #ifdef PIN_PG3
        #define PIN_TCA0_WO3_PORTG PIN_PG3
      #else
        #define PIN_TCA0_WO3_PORTG NOT_A_PIN
      #endif
      #ifdef PIN_PG4
        #define PIN_TCA0_WO4_PORTG PIN_PG4
      #else
        #define PIN_TCA0_WO4_PORTG NOT_A_PIN
      #endif
      #ifdef PIN_PG5
        #define PIN_TCA0_WO5_PORTG PIN_PG5
      #else
        #define PIN_TCA0_WO5_PORTG NOT_A_PIN
      #endif

      /* Initial pins at startup - these can ofc be changed */
      #if PINS_TCA0 == PORTMUX_TCA0_PORTA_gc
        #define PIN_TCA0_WO0_INIT PIN_TCA0_WO0_PORTA
        #define PIN_TCA0_WO1_INIT PIN_TCA0_WO1_PORTA
        #define PIN_TCA0_WO2_INIT PIN_TCA0_WO2_PORTA
        #define PIN_TCA0_WO3_INIT PIN_TCA0_WO3_PORTA
        #define PIN_TCA0_WO4_INIT PIN_TCA0_WO4_PORTA
        #define PIN_TCA0_WO5_INIT PIN_TCA0_WO5_PORTA
      #elif PINS_TCA0 == PORTMUX_TCA0_PORTB_gc
        #define PIN_TCA0_WO0_INIT PIN_TCA0_WO0_PORTB
        #define PIN_TCA0_WO1_INIT PIN_TCA0_WO1_PORTB
        #define PIN_TCA0_WO2_INIT PIN_TCA0_WO2_PORTB
        #define PIN_TCA0_WO3_INIT PIN_TCA0_WO3_PORTB
        #define PIN_TCA0_WO4_INIT PIN_TCA0_WO4_PORTB
        #define PIN_TCA0_WO5_INIT PIN_TCA0_WO5_PORTB
      #elif PINS_TCA0 == PORTMUX_TCA0_PORTC_gc
        #define PIN_TCA0_WO0_INIT PIN_TCA0_WO0_PORTC
        #define PIN_TCA0_WO1_INIT PIN_TCA0_WO1_PORTC
        #define PIN_TCA0_WO2_INIT PIN_TCA0_WO2_PORTC
        #define PIN_TCA0_WO3_INIT PIN_TCA0_WO3_PORTC
        #define PIN_TCA0_WO4_INIT PIN_TCA0_WO4_PORTC
        #define PIN_TCA0_WO5_INIT PIN_TCA0_WO5_PORTC
      #elif PINS_TCA0 == PORTMUX_TCA0_PORTD_gc
        #define PIN_TCA0_WO0_INIT PIN_TCA0_WO0_PORTD
        #define PIN_TCA0_WO1_INIT PIN_TCA0_WO1_PORTD
        #define PIN_TCA0_WO2_INIT PIN_TCA0_WO2_PORTD
        #define PIN_TCA0_WO3_INIT PIN_TCA0_WO3_PORTD
        #define PIN_TCA0_WO4_INIT PIN_TCA0_WO4_PORTD
        #define PIN_TCA0_WO5_INIT PIN_TCA0_WO5_PORTD
      #elif PINS_TCA0 == PORTMUX_TCA0_PORTE_gc
        #define PIN_TCA0_WO0_INIT PIN_TCA0_WO0_PORTE
        #define PIN_TCA0_WO1_INIT PIN_TCA0_WO1_PORTE
        #define PIN_TCA0_WO2_INIT PIN_TCA0_WO2_PORTE
        #define PIN_TCA0_WO3_INIT PIN_TCA0_WO3_PORTE
        #define PIN_TCA0_WO4_INIT PIN_TCA0_WO4_PORTE
        #define PIN_TCA0_WO5_INIT PIN_TCA0_WO5_PORTE
      #elif PINS_TCA0 == PORTMUX_TCA0_PORTF_gc
        #define PIN_TCA0_WO0_INIT PIN_TCA0_WO0_PORTF
        #define PIN_TCA0_WO1_INIT PIN_TCA0_WO1_PORTF
        #define PIN_TCA0_WO2_INIT PIN_TCA0_WO2_PORTF
        #define PIN_TCA0_WO3_INIT PIN_TCA0_WO3_PORTF
        #define PIN_TCA0_WO4_INIT PIN_TCA0_WO4_PORTF
        #define PIN_TCA0_WO5_INIT PIN_TCA0_WO5_PORTF
      #elif PINS_TCA0 == PORTMUX_TCA0_PORTG_gc
        #define PIN_TCA0_WO0_INIT PIN_TCA0_WO0_PORTG
        #define PIN_TCA0_WO1_INIT PIN_TCA0_WO1_PORTG
        #define PIN_TCA0_WO2_INIT PIN_TCA0_WO2_PORTG
        #define PIN_TCA0_WO3_INIT PIN_TCA0_WO3_PORTG
        #define PIN_TCA0_WO4_INIT PIN_TCA0_WO4_PORTG
        #define PIN_TCA0_WO5_INIT PIN_TCA0_WO5_PORTG
      #else /* Must be PORTA then... */
        #define PIN_TCA0_WO0_INIT PIN_TCA0_WO0_PORTA
        #define PIN_TCA0_WO1_INIT PIN_TCA0_WO1_PORTA
        #define PIN_TCA0_WO2_INIT PIN_TCA0_WO2_PORTA
        #define PIN_TCA0_WO3_INIT PIN_TCA0_WO3_PORTA
        #define PIN_TCA0_WO4_INIT PIN_TCA0_WO4_PORTA
        #define PIN_TCA0_WO5_INIT PIN_TCA0_WO5_PORTA
      #endif
    #endif

    #ifdef TCA1
      /* PORT B - all six channels */
      #ifdef PIN_PB0
        #define PIN_TCA1_WO0_PORTB PIN_PB0
      #else
        #define PIN_TCA1_WO0_PORTB NOT_A_PIN
      #endif
      #ifdef PIN_PB1
        #define PIN_TCA1_WO1_PORTB PIN_PB1
      #else
        #define PIN_TCA1_WO1_PORTB NOT_A_PIN
      #endif
      #ifdef PIN_PB2
        #define PIN_TCA1_WO2_PORTB PIN_PB2
      #else
        #define PIN_TCA1_WO2_PORTB NOT_A_PIN
      #endif
      #ifdef PIN_PB3
        #define PIN_TCA1_WO3_PORTB PIN_PB3
      #else
        #define PIN_TCA1_WO3_PORTB NOT_A_PIN
      #endif
      #ifdef PIN_PB4
        #define PIN_TCA1_WO4_PORTB PIN_PB4
      #else
        #define PIN_TCA1_WO4_PORTB NOT_A_PIN
      #endif
      #ifdef PIN_PB5
        #define PIN_TCA1_WO5_PORTB PIN_PB5
      #else
        #define PIN_TCA1_WO5_PORTB NOT_A_PIN
      #endif

      /* PORT C - just three channels */
      #ifdef PIN_PC4
        #define PIN_TCA1_WO0_PORTC PIN_PC4
      #else
        #define PIN_TCA1_WO0_PORTC NOT_A_PIN
      #endif
      #ifdef PIN_PC5
        #define PIN_TCA1_WO1_PORTC PIN_PC5
      #else
        #define PIN_TCA1_WO1_PORTC NOT_A_PIN
      #endif
      #ifdef PIN_PC6
        #define PIN_TCA1_WO2_PORTC PIN_PC6
      #else
        #define PIN_TCA1_WO2_PORTC NOT_A_PIN
      #endif

      /* PORT E - just three channels */
      #ifdef PIN_PE4
        #define PIN_TCA1_WO0_PORTE PIN_PE4
      #else
        #define PIN_TCA1_WO0_PORTE NOT_A_PIN
      #endif
      #ifdef PIN_PE5
        #define PIN_TCA1_WO1_PORTE PIN_PE5
      #else
        #define PIN_TCA1_WO1_PORTE NOT_A_PIN
      #endif
      #ifdef PIN_PE6
        #define PIN_TCA1_WO2_PORTE PIN_PE6
      #else
        #define PIN_TCA1_WO2_PORTE NOT_A_PIN
      #endif

      /* PORT G - all six channels */
      #ifdef PIN_PG0
        #define PIN_TCA1_WO0_PORTG PIN_PG0
      #else
        #define PIN_TCA1_WO0_PORTG NOT_A_PIN
      #endif
      #ifdef PIN_PG1
        #define PIN_TCA1_WO1_PORTG PIN_PG1
      #else
        #define PIN_TCA1_WO1_PORTG NOT_A_PIN
      #endif
      #ifdef PIN_PG2
        #define PIN_TCA1_WO2_PORTG PIN_PG2
      #else
        #define PIN_TCA1_WO2_PORTG NOT_A_PIN
      #endif
      #ifdef PIN_PG3
        #define PIN_TCA1_WO3_PORTG PIN_PG3
      #else
        #define PIN_TCA1_WO3_PORTG NOT_A_PIN
      #endif
      #ifdef PIN_PG4
        #define PIN_TCA1_WO4_PORTG PIN_PG4
      #else
        #define PIN_TCA1_WO4_PORTG NOT_A_PIN
      #endif
      #ifdef PIN_PG5
        #define PIN_TCA1_WO5_PORTG PIN_PG5
      #else
        #define PIN_TCA1_WO5_PORTG NOT_A_PIN
      #endif
      /* Initial pins at startup - these can ofc be changed */
      #if PINS_TCA1 == PORTMUX_TCA1_PORTB_gc
        #define PIN_TCA1_WO0_INIT PIN_TCA1_WO0_PORTB
        #define PIN_TCA1_WO1_INIT PIN_TCA1_WO1_PORTB
        #define PIN_TCA1_WO2_INIT PIN_TCA1_WO2_PORTB
        #define PIN_TCA1_WO3_INIT PIN_TCA1_WO3_PORTB
        #define PIN_TCA1_WO4_INIT PIN_TCA1_WO4_PORTB
        #define PIN_TCA1_WO5_INIT PIN_TCA1_WO5_PORTB
      #elif PINS_TCA1 == PORTMUX_TCA1_PORTC_gc
        #define PIN_TCA1_WO0_INIT PIN_TCA1_WO0_PORTC
        #define PIN_TCA1_WO1_INIT PIN_TCA1_WO1_PORTC
        #define PIN_TCA1_WO2_INIT PIN_TCA1_WO2_PORTC
      #elif PINS_TCA1 == PORTMUX_TCA1_PORTE_gc
        #define PIN_TCA1_WO0_INIT PIN_TCA1_WO0_PORTE
        #define PIN_TCA1_WO1_INIT PIN_TCA1_WO1_PORTE
        #define PIN_TCA1_WO2_INIT PIN_TCA1_WO2_PORTE
      #elif PINS_TCA1 == PORTMUX_TCA1_PORTG_gc
        #define PIN_TCA1_WO0_INIT PIN_TCA1_WO0_PORTG
        #define PIN_TCA1_WO1_INIT PIN_TCA1_WO1_PORTG
        #define PIN_TCA1_WO2_INIT PIN_TCA1_WO2_PORTG
        #define PIN_TCA1_WO3_INIT PIN_TCA1_WO3_PORTG
        #define PIN_TCA1_WO4_INIT PIN_TCA1_WO4_PORTG
        #define PIN_TCA1_WO5_INIT PIN_TCA1_WO5_PORTG
      #endif
    #endif

    #ifdef TCD0
      #ifdef PIN_PA4
        #define PIN_TCD0_WOA_DEFAULT PIN_PA4
      #else
        #define PIN_TCD0_WOA_DEFAULT NOT_A_PIN
      #endif
      #ifdef PIN_PA5
        #define PIN_TCD0_WOB_DEFAULT PIN_PA5
      #else
        #define PIN_TCD0_WOB_DEFAULT NOT_A_PIN
      #endif
      #ifdef PIN_PA6
        #define PIN_TCD0_WOC_DEFAULT PIN_PA6
      #else
        #define PIN_TCD0_WOC_DEFAULT NOT_A_PIN
      #endif
      #ifdef PIN_PA7
        #define PIN_TCD0_WOD_DEFAULT PIN_PA7
      #else
        #define PIN_TCD0_WOD_DEFAULT NOT_A_PIN
      #endif

      /* Port B - currently bugged and not usable */
      #ifdef PIN_PB4
        #define PIN_TCD0_WOA_ALT1 PIN_PB4
      #else
        #define PIN_TCD0_WOA_ALT1 NOT_A_PIN
      #endif
      #ifdef PIN_PB5
        #define PIN_TCD0_WOB_ALT1 PIN_PB5
      #else
        #define PIN_TCD0_WOB_ALT1 NOT_A_PIN
      #endif
      #ifdef PIN_PB6
        #define PIN_TCD0_WOC_ALT1 PIN_PB6
      #else
        #define PIN_TCD0_WOC_ALT1 NOT_A_PIN
      #endif
      #ifdef PIN_PB7
        #define PIN_TCD0_WOD_ALT1 PIN_PB7
      #else
        #define PIN_TCD0_WOD_ALT1 NOT_A_PIN
      #endif

      /* Port F - currently bugged and not usable */
      #ifdef PIN_PF0
        #define PIN_TCD0_WOA_ALT2 PIN_PF0
      #else
        #define PIN_TCD0_WOA_ALT2 NOT_A_PIN
      #endif
      #ifdef PIN_PF1
        #define PIN_TCD0_WOB_ALT2 PIN_PF1
      #else
        #define PIN_TCD0_WOB_ALT2 NOT_A_PIN
      #endif
      #ifdef PIN_PF2
        #define PIN_TCD0_WOC_ALT2 PIN_PF2
      #else
        #define PIN_TCD0_WOC_ALT2 NOT_A_PIN
      #endif
      #ifdef PIN_PF3
        #define PIN_TCD0_WOD_ALT2 PIN_PF3
      #else
        #define PIN_TCD0_WOD_ALT2 NOT_A_PIN
      #endif

      /* Port G - currently bugged and not usable */
      #ifdef PIN_PG4
        #define PIN_TCD0_WOA_ALT3 PIN_PG4
      #else
        #define PIN_TCD0_WOA_ALT3 NOT_A_PIN
      #endif
      #ifdef PIN_PG5
        #define PIN_TCD0_WOB_ALT3 PIN_PG5
      #else
        #define PIN_TCD0_WOB_ALT3 NOT_A_PIN
      #endif
      #ifdef PIN_PG6
        #define PIN_TCD0_WOC_ALT3 PIN_PG6
      #else
        #define PIN_TCD0_WOC_ALT3 NOT_A_PIN
      #endif
      #ifdef PIN_PG7
        #define PIN_TCD0_WOD_ALT3 PIN_PG7
      #else
        #define PIN_TCD0_WOD_ALT3 NOT_A_PIN
      #endif

      #if TCD0_PINS == PORTMUX_TCD0_ALT1_gc
        #define PIN_TCD0_WOA_INIT PIN_TCD0_WOA_ALT1
        #define PIN_TCD0_WOB_INIT PIN_TCD0_WOB_ALT1
        #define PIN_TCD0_WOC_INIT PIN_TCD0_WOC_ALT1
        #define PIN_TCD0_WOD_INIT PIN_TCD0_WOD_ALT1
      #elif TCD0_PINS == PORTMUX_TCD0_ALT2_gc
        #define PIN_TCD0_WOA_INIT PIN_TCD0_WOA_ALT2
        #define PIN_TCD0_WOB_INIT PIN_TCD0_WOB_ALT2
        #define PIN_TCD0_WOC_INIT PIN_TCD0_WOC_ALT2
        #define PIN_TCD0_WOD_INIT PIN_TCD0_WOD_ALT2
      #elif TCD0_PINS == PORTMUX_TCD0_ALT3_gc
        #define PIN_TCD0_WOA_INIT PIN_TCD0_WOA_ALT3
        #define PIN_TCD0_WOB_INIT PIN_TCD0_WOB_ALT3
        #define PIN_TCD0_WOC_INIT PIN_TCD0_WOC_ALT3
        #define PIN_TCD0_WOD_INIT PIN_TCD0_WOD_ALT3
      #else
        #define PIN_TCD0_WOA_INIT PIN_TCD0_WOA_DEFAULT
        #define PIN_TCD0_WOB_INIT PIN_TCD0_WOB_DEFAULT
        #define PIN_TCD0_WOC_INIT PIN_TCD0_WOC_DEFAULT
        #define PIN_TCD0_WOD_INIT PIN_TCD0_WOD_DEFAULT
      #endif
    #endif // end of TCD
  #endif // end of DxCore side
#endif
