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

  #if (CLOCK_SOURCE == 0) // PA0 only available when using internal oscillator.
    #define PIN_TCA0_WO0_DEFAULT    PIN_PA0
  #else
    #define PIN_TCA0_WO0_DEFAULT    NOT_A_PIN
  #endif

  #if (CLOCK_SOURCE == 1) // crystal doesn't have PA1 available either
    #define PIN_TCA0_WO0_DEFAULT    NOT_A_PIN
  #else
    #define PIN_TCA0_WO1_DEFAULT    PIN_PA1
  #endif

  #if ((ID_MASK_PINS & CORE_PART_ID) >= ID_14_PINS)
    #define PIN_TCA0_WO2_DEFAULT    PIN_PA2
    #define PIN_TCA0_WO3_DEFAULT    PIN_PA3
    #define PIN_TCA0_WO4_DEFAULT    PIN_PA4
    #define PIN_TCA0_WO5_DEFAULT    PIN_PA5

    #ifdef __AVR_DD__
      #define PIN_TCD0_WOA_ALT4     PIN_PA4
      #define PIN_TCD0_WOB_ALT4     PIN_PA5
    #endif
    #define PIN_TCD0_WOA_DEFAULT    PIN_PA4
    #define PIN_TCD0_WOB_DEFAULT    PIN_PA5
    #define PIN_TCD0_WOC_DEFAULT    PIN_PA6
    #define PIN_TCD0_WOD_DEFAULT    PIN_PA7
  #else
    // no PA2:7 for 14 pin
    #define PIN_TCA0_WO2_DEFAULT    NOT_A_PIN
    #define PIN_TCA0_WO3_DEFAULT    NOT_A_PIN
    #define PIN_TCA0_WO4_DEFAULT    NOT_A_PIN
    #define PIN_TCA0_WO5_DEFAULT    NOT_A_PIN

    #ifdef __AVR_DD__
      #define PIN_TCD0_WOA_ALT4     NOT_A_PIN
      #define PIN_TCD0_WOB_ALT4     NOT_A_PIN
    #endif
    #define PIN_TCD0_WOA_DEFAULT    NOT_A_PIN
    #define PIN_TCD0_WOB_DEFAULT    NOT_A_PIN
    #define PIN_TCD0_WOC_DEFAULT    NOT_A_PIN
    #define PIN_TCD0_WOD_DEFAULT    NOT_A_PIN
  #endif

  #if ((ID_MASK_PINS & CORE_PART_ID) >= ID_28_PINS)
    #define PIN_TCA0_WO0_ALT2       PIN_PC0
    #if (((ID_MASK_PINS & CORE_PART_ID) >= ID_48_PINS) || !((ID_AVR_DB | ID_AVR_DD) & CORE_PART_ID))
      // PD0 is present on all 48+ pin, and all 28+ pin parts that don't have MVIO, so not DB or DD.
      #define PIN_TCA0_WO0_ALT3     PIN_PD0
    #endif
    #define PIN_TCA0_WO1_ALT3       PIN_PD1
    #define PIN_TCA0_WO2_ALT3       PIN_PD2
    #define PIN_TCA0_WO3_ALT3       PIN_PD3

    #define PIN_TCA0_WO0_ALT5       PIN_PF0
    #define PIN_TCA0_WO1_ALT5       PIN_PF1

    #define PIN_TCD0_WOA_ALT2       PIN_PF0
    #define PIN_TCD0_WOB_ALT2       PIN_PF1
  #else
    // no lower port D PC0 for sub 28-pin
    #define PIN_TCA0_WO0_ALT2       NOT_A_PIN

    #define PIN_TCA0_WO0_ALT3       NOT_A_PIN
    #define PIN_TCA0_WO1_ALT3       NOT_A_PIN
    #define PIN_TCA0_WO2_ALT3       NOT_A_PIN
    #define PIN_TCA0_WO3_ALT3       NOT_A_PIN

    #define PIN_TCA0_WO0_ALT5       NOT_A_PIN
    #define PIN_TCA0_WO1_ALT5       NOT_A_PIN

    #define PIN_TCD0_WOA_ALT2       NOT_A_PIN
    #define PIN_TCD0_WOB_ALT2       NOT_A_PIN
  #endif

  // Everything except sub-28 pin has PC9, all have PC1/2/3
  #define PIN_TCA0_WO1_ALT2         PIN_PC1
  #define PIN_TCA0_WO2_ALT2         PIN_PC2
  #define PIN_TCA0_WO3_ALT2         PIN_PC3

  // Everything has top half of PORTD
  #define PIN_TCA0_WO4_ALT3         PIN_PD4
  #define PIN_TCA0_WO5_ALT3         PIN_PD5

  #ifdef __AVR_DD__
    #define PIN_TCD0_WOC_ALT4       PIN_PD4
    #define PIN_TCD0_WOD_ALT4       PIN_PD5
  #endif


  #if ((ID_MASK_PINS & CORE_PART_ID) >= ID_32_PINS)
    #define PIN_TCA0_WO2_ALT5       PIN_PF2
    #define PIN_TCA0_WO3_ALT5       PIN_PF3
    #define PIN_TCA0_WO4_ALT5       PIN_PF4
    #define PIN_TCA0_WO5_ALT5       PIN_PF5

    #define PIN_TCD0_WOC_ALT2       PIN_PF2
    #define PIN_TCD0_WOD_ALT2       PIN_PF3
  #else
    #define PIN_TCA0_WO2_ALT5       NOT_A_PIN
    #define PIN_TCA0_WO3_ALT5       NOT_A_PIN
    #define PIN_TCA0_WO4_ALT5       NOT_A_PIN
    #define PIN_TCA0_WO5_ALT5       NOT_A_PIN

    #define PIN_TCD0_WOC_ALT2       NOT_A_PIN
    #define PIN_TCD0_WOD_ALT2       NOT_A_PIN
  #endif



  #if ((ID_MASK_PINS & CORE_PART_ID) >= ID_48_PINS)
    // 48 pin parts have PB0:5
    #define PIN_TCA0_WO0_ALT1       PIN_PB0
    #define PIN_TCA0_WO1_ALT1       PIN_PB1
    #define PIN_TCA0_WO2_ALT1       PIN_PB2
    #define PIN_TCA0_WO3_ALT1       PIN_PB3
    #define PIN_TCA0_WO4_ALT1       PIN_PB4
    #define PIN_TCA0_WO5_ALT1       PIN_PB5

    #define PIN_TCA1_WO0_DEFAULT    PIN_PB0
    #define PIN_TCA1_WO1_DEFAULT    PIN_PB1
    #define PIN_TCA1_WO2_DEFAULT    PIN_PB2
    #define PIN_TCA1_WO3_DEFAULT    PIN_PB3
    #define PIN_TCA1_WO4_DEFAULT    PIN_PB4
    #define PIN_TCA1_WO5_DEFAULT    PIN_PB5

    #define PIN_TCD0_WOA_ALT1       PIN_PB4
    #define PIN_TCD0_WOB_ALT1       PIN_PB5

    // anything with 48+ pins has full PORTC
    #define PIN_TCA0_WO4_ALT2       PIN_PC4
    #define PIN_TCA0_WO5_ALT2       PIN_PC5

    #define PIN_TCA1_WO0_ALT1       PIN_PC4
    #define PIN_TCA1_WO1_ALT1       PIN_PC5
    #define PIN_TCA1_WO2_ALT1       PIN_PC6

    // At least half of PORTE
    #define PIN_TCA0_WO0_ALT4       PIN_PE0
    #define PIN_TCA0_WO1_ALT4       PIN_PE1
    #define PIN_TCA0_WO2_ALT4       PIN_PE2
    #define PIN_TCA0_WO3_ALT4       PIN_PE3
    #if ((ID_MASK_PINS & CORE_PART_ID) == ID_64_PINS)


      #define PIN_TCD0_WOC_ALT1     PIN_PB6
      #define PIN_TCD0_WOD_ALT1     PIN_PB7
      // Full port E for 64 pin
      #define PIN_TCA0_WO4_ALT4     PIN_PE4
      #define PIN_TCA0_WO5_ALT4     PIN_PE5

      #define PIN_TCA1_WO0_ALT2     PIN_PE4
      #define PIN_TCA1_WO1_ALT2     PIN_PE5
      #define PIN_TCA1_WO2_ALT2     PIN_PE6
      // Full port G for 64 pin
      #define PIN_TCA0_WO0_ALT6     PIN_PG0
      #define PIN_TCA0_WO1_ALT6     PIN_PG1
      #define PIN_TCA0_WO2_ALT6     PIN_PG2
      #define PIN_TCA0_WO3_ALT6     PIN_PG3
      #define PIN_TCA0_WO4_ALT6     PIN_PG4
      #define PIN_TCA0_WO5_ALT6     PIN_PG5

      #define PIN_TCA1_WO0_ALT3     PIN_PG0
      #define PIN_TCA1_WO1_ALT3     PIN_PG1
      #define PIN_TCA1_WO2_ALT3     PIN_PG2
      #define PIN_TCA1_WO3_ALT3     PIN_PG3
      #define PIN_TCA1_WO4_ALT3     PIN_PG4
      #define PIN_TCA1_WO5_ALT3     PIN_PG5

      #define PIN_TCD0_WOA_ALT3     PIN_PG4
      #define PIN_TCD0_WOB_ALT3     PIN_PG5
      #define PIN_TCD0_WOC_ALT3     PIN_PG6
      #define PIN_TCD0_WOD_ALT3     PIN_PG7

    #else
      #define PIN_TCD0_WOC_ALT1     NOT_A_PIN
      #define PIN_TCD0_WOD_ALT1     NOT_A_PIN
      // Half port E for 48-pin
      #define PIN_TCA0_WO4_ALT4     NOT_A_PIN
      #define PIN_TCA0_WO5_ALT4     NOT_A_PIN

      #define PIN_TCA1_WO0_ALT2     NOT_A_PIN
      #define PIN_TCA1_WO1_ALT2     NOT_A_PIN
      #define PIN_TCA1_WO2_ALT2     NOT_A_PIN

      // No port G
      #define PIN_TCA0_WO0_ALT6     NOT_A_PIN
      #define PIN_TCA0_WO1_ALT6     NOT_A_PIN
      #define PIN_TCA0_WO2_ALT6     NOT_A_PIN
      #define PIN_TCA0_WO3_ALT6     NOT_A_PIN
      #define PIN_TCA0_WO4_ALT6     NOT_A_PIN
      #define PIN_TCA0_WO5_ALT6     NOT_A_PIN

      #define PIN_TCA1_WO0_ALT3     NOT_A_PIN
      #define PIN_TCA1_WO1_ALT3     NOT_A_PIN
      #define PIN_TCA1_WO2_ALT3     NOT_A_PIN
      #define PIN_TCA1_WO3_ALT3     NOT_A_PIN
      #define PIN_TCA1_WO4_ALT3     NOT_A_PIN
      #define PIN_TCA1_WO5_ALT3     NOT_A_PIN

      #define PIN_TCD0_WOA_ALT3     NOT_A_PIN
      #define PIN_TCD0_WOB_ALT3     NOT_A_PIN
      #define PIN_TCD0_WOC_ALT3     NOT_A_PIN
      #define PIN_TCD0_WOD_ALT3     NOT_A_PIN
    #endif
  #else

    // Under 48 pin gets no port B
    #define PIN_TCA0_WO0_ALT1       NOT_A_PIN
    #define PIN_TCA0_WO1_ALT1       NOT_A_PIN
    #define PIN_TCA0_WO2_ALT1       NOT_A_PIN
    #define PIN_TCA0_WO3_ALT1       NOT_A_PIN
    #define PIN_TCA0_WO4_ALT1       NOT_A_PIN
    #define PIN_TCA0_WO5_ALT1       NOT_A_PIN

    #define PIN_TCD0_WOA_ALT1       NOT_A_PIN
    #define PIN_TCD0_WOB_ALT1       NOT_A_PIN
    #define PIN_TCD0_WOC_ALT1       NOT_A_PIN
    #define PIN_TCD0_WOD_ALT1       NOT_A_PIN

    // under 48 gets half port C
    #define PIN_TCA0_WO4_ALT2       NOT_A_PIN
    #define PIN_TCA0_WO5_ALT2       NOT_A_PIN

    // Under 48 pin gets no port E
    #define PIN_TCA0_WO0_ALT4       NOT_A_PIN
    #define PIN_TCA0_WO1_ALT4       NOT_A_PIN
    #define PIN_TCA0_WO2_ALT4       NOT_A_PIN
    #define PIN_TCA0_WO3_ALT4       NOT_A_PIN
    #define PIN_TCA0_WO4_ALT4       NOT_A_PIN
    #define PIN_TCA0_WO5_ALT4       NOT_A_PIN
    // Under 48 pin gets no port G
    #define PIN_TCA0_WO0_ALT6       NOT_A_PIN
    #define PIN_TCA0_WO1_ALT6       NOT_A_PIN
    #define PIN_TCA0_WO2_ALT6       NOT_A_PIN
    #define PIN_TCA0_WO3_ALT6       NOT_A_PIN
    #define PIN_TCA0_WO4_ALT6       NOT_A_PIN
    #define PIN_TCA0_WO5_ALT6       NOT_A_PIN

    #define PIN_TCD0_WOA_ALT3       NOT_A_PIN
    #define PIN_TCD0_WOB_ALT3       NOT_A_PIN
    #define PIN_TCD0_WOC_ALT3       NOT_A_PIN
    #define PIN_TCD0_WOD_ALT3       NOT_A_PIN

  #endif
#endif
