/* test for macros from pins_arduino.h
*/
#define ct_assert(e) ((void)sizeof(char[1 - 2*!(e)]))

void setup() {
  volatile int i,p;

  for( p=0; p<64; p++) {
    i=digitalPinToAnalogInput(p);
    i+=analogInputToDigitalPin(p);
    i+=digitalOrAnalogPinToDigital(p);
    i+=portToPinZero(p);
    i+=digitalPinHasPWMTCB(p);
    i+=digitalPinHasPWM(p);
    i+=analogChannelToDigitalPin(p);
  }

  #if defined(USING_OPTIBOOT) && !defined(ASSUME_MVIO_FUSE)  // optiboot case is using non constant expressions!
    #define IS_MVIO_ENABLED() 0
  #endif

  #if !defined(IS_MVIO_ENABLED)
    #define IS_MVIO_ENABLED() 0
  #endif


  #if (PINS_COUNT == 12)
   ct_assert( analogChannelToDigitalPin(8) == NOT_A_PIN ); // 14 pin package
  #endif

  #if defined(PIN_PC0) && !(defined(__AVR_DA__) || defined(__AVR_DB__) || defined(__AVR_DU__)) && !IS_MVIO_ENABLED()
    #if PIN_A28 != NOT_A_PIN  // exlude 14/20 pin
      ct_assert( A28 == PIN_PC0 );
    #endif
  #endif

  #if defined(PIN_PC1) && !(defined(__AVR_DA__) || defined(__AVR_DB__) || defined(__AVR_DU__)) && !IS_MVIO_ENABLED()
      ct_assert( A29 == PIN_PC1 );
  #endif

  #if defined(PIN_PC3) && !(defined(__AVR_DA__) || defined(__AVR_DB__)) && !IS_MVIO_ENABLED()
    ct_assert(A31 == PIN_PC3);
    ct_assert( analogChannelToDigitalPin(31) == PIN_PC3 );
    ct_assert( digitalPinToAnalogInput(PIN_PC3) == 31 );
  #endif

  #if defined(PIN_PC0) && !(defined(__AVR_DA__) || defined(__AVR_DB__)) && !IS_MVIO_ENABLED()
    #if defined(__AVR_DU__)
      ct_assert( analogChannelToDigitalPin(28) == NOT_A_PIN );
      ct_assert( digitalPinToAnalogInput(PIN_PC0) == NOT_A_PIN );
    #else
      #if PIN_A28 != NOT_A_PIN  // exlude 14/20 pin
      ct_assert( analogChannelToDigitalPin(28) == PIN_PC0 );
      ct_assert( digitalPinToAnalogInput(PIN_PC0) == 28 );
      #endif
    #endif
  #endif

  #if defined(PIN_PC1) && !(defined(__AVR_DA__) || defined(__AVR_DB__)) && !IS_MVIO_ENABLED()
    ct_assert( analogChannelToDigitalPin(29) == PIN_PC1 );
    ct_assert( digitalPinToAnalogInput(PIN_PC1) == 29 );
  #endif

  #if defined(PIN_PC3) && !(defined(__AVR_DA__) || defined(__AVR_DB__)) && !IS_MVIO_ENABLED()
    ct_assert( analogChannelToDigitalPin(31) == PIN_PC3 );
    ct_assert( digitalPinToAnalogInput(PIN_PC3) == 31 );
  #endif

  #if defined(PIN_PA2) && ( defined(__AVR_DD__) || defined(__AVR_EA__) || defined(__AVR_EB__) || defined(__AVR_DU__) )
  ct_assert( analogChannelToDigitalPin(22) == PIN_PA2 );
  ct_assert( digitalPinToAnalogInput(PIN_PA2) == 22 );
  #endif

  #if defined(PIN_PA4) && ( defined(__AVR_DD__) || defined(__AVR_EA__) || defined(__AVR_EB__) || defined(__AVR_DU__) )
    ct_assert( analogChannelToDigitalPin(24) == PIN_PA4 );
    ct_assert( digitalPinToAnalogInput(PIN_PA4) == 24 );
  #endif

  #if defined(PIN_PD1)
    ct_assert( analogChannelToDigitalPin(1) == PIN_PD1 );
    ct_assert( digitalPinToAnalogInput(PIN_PD1) == 1 );
  #endif

  #if defined(PIN_PD4)
    ct_assert( analogChannelToDigitalPin(4) == PIN_PD4 );
    ct_assert( digitalPinToAnalogInput(PIN_PD4) == 4 );
  #endif

  #if defined(PIN_PC3) &&  defined(__AVR_DU__)
    ct_assert( analogChannelToDigitalPin(31) == PIN_PC3 );
    ct_assert( digitalPinToAnalogInput(PIN_PC3) == 31 );
  #endif

  #ifdef PIN_PE0
    ct_assert( analogChannelToDigitalPin(8) == PIN_PE0 );
    ct_assert( digitalPinToAnalogInput(PIN_PE0) == 8 );
  #endif

  #ifdef PIN_PE1
    ct_assert(analogChannelToDigitalPin(9) == PIN_PE1);
    ct_assert(digitalPinToAnalogInput(PIN_PE1) == 9 );
  #endif

  #ifdef PIN_PF0
    ct_assert( analogChannelToDigitalPin(16) == PIN_PF0 );
    ct_assert( digitalPinToAnalogInput(PIN_PF0) == 16 );
  #endif

}

void loop() {
}
