/* Example 3: High speed 8-bit PWM
 * https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/TakingOverTCA0.md
 *
 * A user of megaTinyCore requested (#152) high speed PWM. They wanted split mode disabled, and PWM frequency higher
 * than 62KHz. This is indeed possible - though do note that the maximum frequency of PWM possible with a full 8 bits
 * of resolution is 78.125 kHz when running at 20 MHz (20000000/256); at 24, it's 93.75 kHz, and overclocked to 32 MHz,
 * 125 kHz. The next highest  frequency for which perfect 8-bit resolution is possible is half of those frequencies.
 * Higher fequencies require lower resolution (see above example for one approach, which can also be used for
 * intermediate frequencies) - though if the frequency is constant, varying your input between 0 and the period instead
 * of using map() is desirable, as map may not be smooth. As a further aside, if 78.125kHz is suitable, there is no
 * need to disable split mode (ynless other features were required, like event inputs or buffering (which might well
 * be what the original requester wanted single mode for)
 * It strikes me now, as I adapt this example for the Dx-series parts, that 62 KHz is almost exactly the maximum
 * possible for 8-bit PWM at 16 MHz system clock. I'm pretty sure there's a connection!
 *
 * Do note that if pushing the PWM frequency is your aim, you can go considerably higher by using the Type D timer.
 * It is rated for a TCD clock of up to 48 MHz.... (and I was able to generate PWM from it without anomalies with
 * it clocked at 128 MHz (32 MHz system clock multiplied by 4, using the 4x multiplier setting that was in the initial
 * io headers, but was pulled from the datasheet before release, and the headers shortly after) - these parts have a
 * ton of headroom on frequency at room temp and under non-adverse conditions)
 */

#if defined(MILLIS_USE_TIMERA0)
  #error "This sketch takes over TCA0, don't use for millis here."
#endif


void setup() {
  // We will be outputting PWM on PA2
  pinMode(PIN_PA2, OUTPUT);
  takeOverTCA0();

  PORTMUX.TCAROUTEA = (PORTMUX.TCAROUTEA & ~(PORTMUX_TCA0_gm)) | PORTMUX_TCA0_PORTA_gc;
  TCA0.SINGLE.CTRLB = (TCA_SINGLE_CMP2EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc); // Single slope PWM mode, PWM on WO2
  TCA0.SINGLE.PER = 0x00FF; // Count all the way up to 0x00FF (255) - 8-bit PWM
  // At 20MHz, this gives ~78.125kHz PWM
  TCA0.SINGLE.CMP2 = 0;
  TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm; // enable the timer with no prescaler
}

void loop() { // Lets generate some output just to prove it works
  static byte pass = 0;
  static unsigned int duty = 255;
  TCA0.SINGLE.CMP2 = duty--; // step down the duty cycle each iteration through loop;
  delay(100);  // so we can see the duty cycle changing over time on the scope/with an LED
  if (!duty) {
    if (pass == 0) {
      // After the first pass, lets go up to 100kHz
      pass = 1;
      duty = 199;
      TCA0.SINGLE.PER = 199;
    } else if (pass == 1) {
      // and now the requested 62 kHz (actually 62.11kHz)
      pass = 2;
      duty = 322;
      TCA0.SINGLE.PER = 322;
    } else { // and back to the beginning.
      pass = 0;
      duty = 255;
      TCA0.SINGLE.PER = 255;
    }
  }
}
