/* Example 4: Quick bit of fun with split mode
 * https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/TakingOverTCA0.md
 *
 * A quick example of how cool split mode can be - You can get two different PWM frequencies out of the same timer.
 * Split mode only has one mode - both halves of the timer independently count down.
 * Here, we've made it even more interesting by using two frequencies almost identical to each other.... they will
 * "beat" against each other weith a frequency of 1.43 Hz (366 Hz / 256). You should be able to observe that with a
 * bicolor LED (and appropriate resistor) between the two pins. These have two LEDs with opposite polarity, typically
 * a red and a green, connected between two pins... the question is - what will it look like? How will it be different
 * from a single color LED? Make predictions and then test them. When I (Spence) did this, I was wrong.
 */

#if defined(MILLIS_USE_TIMERA0)
  #error "This sketch takes over TCA0, don't use for millis here."
#endif


void setup() {
  // We will be outputting PWM on PD2 amd PD3
  // No need to enable split mode - core has already done that for us.
  pinMode(PIN_PD2, OUTPUT); // PD2 - TCA0 WO2
  pinMode(PIN_PD3, OUTPUT); // PD3 - TCA0 WO3
  PORTMUX.TCAROUTEA = (PORTMUX.TCAROUTEA & ~(PORTMUX_TCA0_gm)) | PORTMUX_TCA0_PORTD_gc; // Variety! Also on all parts!
  TCA0.SPLIT.CTRLB = TCA_SPLIT_LCMP2EN_bm | TCA_SPLIT_HCMP0EN_bm; // PWM on WO2, WO3
  TCA0.SPLIT.LPER = 0xFF; // Count all the way down from 255 on WO0/WO1/WO2
  TCA0.SPLIT.HPER = 0xFE; // Count down from only 254 on WO3/WO4/WO5
  TCA0.SPLIT.LCMP2 = 128; // 50% duty cycle
  TCA0.SPLIT.HCMP0 = 127; // 50% duty cycle
  TCA0.SPLIT.CTRLA = TCA_SPLIT_CLKSEL_DIV256_gc | TCA_SPLIT_ENABLE_bm; // enable the timer with prescaler of 256 - slow it down so the phases shift more slowly, but not so slow it would flicker...
}

void loop() {
  // nothing to do here but enjoy your PWM.
}
