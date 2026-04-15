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

  #if defined(PIN_PA2) && ( defined(__AVR_DD__) || defined(__AVR_EA__) || defined(__AVR_EB__) || defined(__AVR_DU__) )
    ct_assert( analogChannelToDigitalPin(22) == PIN_PA2 );
  #endif

  #if defined(PIN_PA4) && ( defined(__AVR_DD__) || defined(__AVR_EA__) || defined(__AVR_EB__) || defined(__AVR_DU__) )
    ct_assert( analogChannelToDigitalPin(24) == PIN_PA4 );
  #endif

  #if defined(PIN_PC1) && ( defined(__AVR_EA__) || defined(__AVR_EB__) )
    ct_assert( analogChannelToDigitalPin(29) == PIN_PC1 );
  #endif

  #if defined(PIN_PC1) && ( defined(__AVR_DD__) && !defined(MVIO_ENABLED) )
    ct_assert( analogChannelToDigitalPin(29) == PIN_PC1 );
  #endif

  #if defined(PIN_PD1)
    ct_assert( analogChannelToDigitalPin(1) == PIN_PD1 );
  #endif

  #if defined(PIN_PD4)
    ct_assert( analogChannelToDigitalPin(4) == PIN_PD4 );
  #endif

  #if defined(PIN_PC3) &&  defined(__AVR_DU__)
    ct_assert( analogChannelToDigitalPin(31) == PIN_PC3 );
  #endif

 #ifdef PIN_PE0
    ct_assert( analogChannelToDigitalPin(8) == PIN_PE0 );
  #endif

  #ifdef PIN_PF0
    ct_assert( analogChannelToDigitalPin(16) == PIN_PF0 );
  #endif

}

void loop() {
}
