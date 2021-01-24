#if defined(MILLIS_USE_TIMERA0)||defined(__AVR_ATtinyxy2__)
#error "This sketch takes over TCA0, don't use for millis here.  Pin mappings on 8-pin parts are different"
#endif

unsigned int DutyCycle=0;

void setup() {
  // We will be outputting PWM on PB0
  pinMode(PIN_PB0, OUTPUT); //PB0 - TCA0 WO0, pin7 on 14-pin parts
  TCA0.SPLIT.CTRLA=0; //disable TCA0 and set divider to 1
  TCA0.SPLIT.CTRLESET=TCA_SPLIT_CMD_RESET_gc|0x03; //set CMD to RESET, and enable on both pins.
  TCA0.SPLIT.CTRLD=0; //Split mode now off, CMPn = 0, CNT = 0, PER = 255
  TCA0.SINGLE.CTRLB=(TCA_SINGLE_CMP0EN_bm|TCA_SINGLE_WGMODE_DSBOTTOM_gc); //Dual slope PWM mode OVF interrupt at BOTTOM, PWM on WO0
  TCA0.SINGLE.PER=0xFFFF; // Count all the way up to 0xFFFF
  // At 20MHz, this gives ~152Hz PWM
  TCA0.SINGLE.CMP0=DutyCycle;
  TCA0.SINGLE.INTCTRL=TCA_SINGLE_OVF_bm; //enable overflow interrupt
  TCA0.SINGLE.CTRLA=TCA_SINGLE_ENABLE_bm; //enable the timer with no prescaler
}

void loop() { // Not even going to do anything in here
}

ISR(TCA0_OVF_vect) { //on overflow, we will increment TCA0.CMP0, this will happen after every full cycle - a little over 7 minutes.
  TCA0.SINGLE.CMP0=DutyCycle++; // Because we are in Dual Slope Bottom mode, OVF fires at BOTTOM, at end, not TOP, in middle of the pulse.
  TCA0.SINGLE.INTFLAGS=TCA_SINGLE_OVF_bm; //Always remember to clear the interrupt flags, otherwise the interrupt will fire continually!
}
