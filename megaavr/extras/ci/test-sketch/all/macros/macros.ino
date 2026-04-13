/* test for macros from pins_arduino.h
*/

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
}

void loop() {
}
