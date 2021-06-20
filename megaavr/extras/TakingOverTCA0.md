# Taking over TCA0 for your owm PWM

We have received many questions from users about how to take over one of the TCA modules to generate 16-bit PWM or change the output frequency.

The most common point of confusion is the fact that DxCore, out of the box, configures both TCA's for use in "Split Mode" - this allows it to generate 6 8-bit PWM signals. This provides 3 additional PWM pins (3 if TCB0 is needed for other purposes) - and since analogWrite() only supports 8-bit PWM anyway, when using the Arduino API functions, there is no loss of functionality imposed by this. But it must be disabled if you want to repurpose the timer.

Another issue is that there are Arduino API functions that reach out and poke the timers (analogWrite and digitalWrite, mostly). As of DxCore 1.3.2 there is now a simple solution to both in the form of the `takeOverTCA0()` and `takeOverTCA1()` functions. Calling either of these turns off the timer in question, marks it as "taken over" internally to disable PWM from analogWrite/digitalWrite (which turns off PWM channels), ands issues a hard reset command, resetting every timer register to it's power on default! It will now be in SINGLE mode, so access it using the `TCA0.SINGLE` struct (I find the two structs very awkward and verbose, but the difference isn;t just that they have different bits enabled; in SINGLE mode the compare and period registers are 16-bit and use that procedure with the temp register to read correct data, and if you were to access the individual bytes, you'd get different results if you did it in the "wrong" order, but only in SINGLE mode; in SPLIT mode they are independent 8-bit registers). You don't have to worry about that if you access using the appropriate struct for the mode.

### TCBs doing pwm are clocked from TCA0
TCBs only get prescale of /1 or 2/ - you want lower frequency for PWM. They can be clocked from the prescaled clock of a TCA though - and that's what we do. They all use TCA0. If you have both TCA's on your device, but only need to take over TCA0, you could change any TCB's you're using for PWM to use TCA1 as clock source - or just restart TCA0 at your final clock speed, if you know that will work for whatever the TCB PWM is driving . This is only used for PWM - TCBs use the /1 or /2 clock sources when used for Tone, Servo, and millis timekeeping. There are also the 2 channels of the TCD0 which can be used for PWM.

### Avoid using the TCA you are reconfiguring for millis
Reconfiguring a timer like this when it is used as the millis timer source will result in the derangement of timekeeping functionality. While this is less freqently an issue on DxCore vs megaTinyCore, since we have ample type B timers and default those for timekeeping, we do still provide the option to use the TCA(s) for timekeeping (maybe you need a large number of TCBs for some unusual use case). If you think this might be an issue in the future, or if you're writing code that will be used by the masses (whose capacity to use shared code under strange conditions never ceases to amaze me), you should trap that with a useful error with something like this:

```
#ifdef MILLIS_USE_TIMERA0
#error "This sketch takes over TCA0 - please use a different timer for millis"
#endif
```

# Examples
Now for the fun part - example code! What's so much fun about example code? Because you can steal it and re-use it, of course!

A note about the pin numbers - we use the PORT_Pxn notation to refer to pins; when I mention in the comments the pin number, that is an Arduino (logical) pin number, not a physical pin number (generally, this documentation does not refer to physical pin numbers except on the pinout charts).

Also, TCA0 can output PWM on pins 0-2 (0-5 for the large )


### Example 1: 16-bit PWM in single mode, dual slope with interrupt.
```
#if defined(MILLIS_USE_TIMERA0)
#error "This sketch takes over TCA0, don't use for millis here."
#endif

unsigned int DutyCycle = 0;
// picked more or less randomly, other than the fact that everything has it, so it makes a good example :-)
uint8_t OutputPin = PIN_PC0;


void setup() {
  pinMode(OutputPin, OUTPUT);
  takeOverTCA0(); // this replaces disabling and resettng the timer, required previously.

  PORTMUX.TCAROUTEA   = (PORTMUX.TCAROUTEA & ~(PORTMUX_TCA0_gm)) | PORTMUX_TCA0_PORTC_gc; // Set mux to PORTC

  TCA0.SINGLE.CTRLB   = (TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_WGMODE_DSBOTTOM_gc); // Dual slope PWM mode OVF interrupt at BOTTOM, PWM on WO0.
  TCA0.SINGLE.PER     = 0xFFFF;                                                 // Count all the way up to 0xFFFF.
  //  At 20MHz, this gives ~152Hz PWM with no prescaling.
  TCA0.SINGLE.CMP0    = DutyCycle;            // 0 - 65535
  TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;    // enable overflow interrupt
  TCA0.SINGLE.CTRLA   = TCA_SINGLE_ENABLE_bm; // enable the timer with no prescaler
}

void loop() { // Not even going to do anything in here
}

ISR(TCA0_OVF_vect) { // on overflow, we will increment TCA0.CMP0, this will happen after every full cycle - a little over 7 minutes.
  TCA0.SINGLE.CMP0 = DutyCycle++; // Because we are in Dual Slope Bottom mode, OVF fires at BOTTOM, at end, not TOP, in middle of the pulse.
  TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm; // Always remember to clear the interrupt flags, otherwise the interrupt will fire continually!
}
```


### Example 2: Variable frequency and duty cycle PWM
This generates PWM similar to the first example (though without the silly interrupt to change the duty cycle), but takes it a step further and into more practical territory with two functions to set the duty cycle and frequency. Calling those instead of this PWMDemo() function is all you'd need to make use of this. Somewhere I think I have the same functionality implemented for the classic AVR "Timer1" style 16-bit timers.

```
#if defined(MILLIS_USE_TIMERA0)
#error "This sketch takes over TCA0, don't use for millis here."
#endif

uint8_t OutputPin = PIN_PC0;

unsigned int Period=0xFFFF;

void setup() {
  pinMode(OutputPin, OUTPUT);
  PORTMUX.TCAROUTEA = (PORTMUX.TCAROUTEA & ~(PORTMUX_TCA0_gm)) | PORTMUX_TCA0_PORTC_gc;
  takeOverTCA0(); // this replaces disabling and resettng the timer, required previously.
  TCA0.SINGLE.CTRLB=(TCA_SINGLE_CMP0EN_bm|TCA_SINGLE_WGMODE_SINGLESLOPE_gc); //Single slope PWM mode, PWM on WO0
  TCA0.SINGLE.PER=Period; // Count all the way up to 0xFFFF
  // At 20MHz, this gives ~305Hz PWM
  TCA0.SINGLE.CMP0=0;
  TCA0.SINGLE.CTRLA=TCA_SINGLE_ENABLE_bm; //enable the timer with no prescaler
}

void loop() { // Not even going to do anything in here
  PWMDemo(150000);//150kHz
  PWMDemo(70000);//70kHz
  PWMDemo(15000);//15kHz
  PWMDemo(3000);//3kHz
  PWMDemo(120);//120Hz
  PWMDemo(35);//35Hz
  PWMDemo(13);//13Hz
}

void PWMDemo(unsigned long frequency){
  setFrequency(frequency);
  setDutyCycle(64); //~25%
  delay(4000);
  setDutyCycle(128); //~50%
  delay(4000);
  setDutyCycle(192); //~75%
  delay(4000);
}

void setDutyCycle(byte duty) {
  TCA0.SINGLE.CMP0=map(duty,0,255,0,Period);
}

void setFrequency(unsigned long freqInHz) {
  unsigned long tempperiod=(F_CPU/freqInHz);
  byte presc=0;
  while (tempperiod>65536 && presc<7) {
    presc++;
    tempperiod=tempperiod>>(presc>4?2:1);
  }
  Period=tempperiod;
  TCA0.SINGLE.CTRLA=(presc<<1)|TCA_SINGLE_ENABLE_bm;
  TCA0.SINGLE.PER=Period;
}
```

### Example 3: High speed 8-bit PWM
A user requested (#152) high speed PWM. They wanted split mode disabled, and PWM frequency higher than 62KHz. This is indeed possible - though do note that the maximum frequency of PWM possible with a full 8 bits of resolution is 78.125 kHz when running at 20 MHz (20000000/256); at 24, it's 93.75 kHz, and overclocked to 32 MHz, 125 kHz. The next higher frequency for which perfect 8-bit resolution is possible is half of those frequencies. Higher fequencies require lower resolution (see above example for one approach, which can also be used for intermediate frequencies) though if the frequency is constant, varying your input between 0 and the period instead of using map() is desirable, as map may not be smooth. As a further aside, if 78.125kHz is suitable, there is no need to disable split mode. It strikes me now, as I adapt this example for the Dx-series parts, that 62 KHz is almost exactly the maximum possible for 8-bit PWM at 16 MHz system clock. I'm pretty sure there's a connection!

Do note that if pushing the PWM frequency is your aim, you can go considerably higher by using the Type D timer - it is rated for a TCD clock of up to 48 MHz.... (and I was able to generate PWM from it without anomalies with it clocked at 128 MHz (32 MHz system clock multiplied by 4) - these parts have a ton of headroom on frequency at room temp and under non-adverse conditions)

```
#if defined(MILLIS_USE_TIMERA0)
#error "This sketch takes over TCA0, don't use for millis here."
#endif


void setup() {
  // We will be outputting PWM on PA2
  pinMode(PIN_PA2, OUTPUT);
  PORTMUX.TCAROUTEA = (PORTMUX.TCAROUTEA & ~(PORTMUX_TCA0_gm)) | PORTMUX_TCA0_PORTA_gc;
  takeOverTCA0();

  TCA0.SINGLE.CTRLB = (TCA_SINGLE_CMP2EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc); //Single slope PWM mode, PWM on WO0
  TCA0.SINGLE.PER = 0x00FF; // Count all the way up to 0x00FF (255) - 8-bit PWM
  // At 20MHz, this gives ~78.125kHz PWM
  TCA0.SINGLE.CMP2 = 0;
  TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm; //enable the timer with no prescaler
}

void loop() { //Lets generate some output just to prove it works
  static byte pass = 0;
  static unsigned int duty = 255;
  TCA0.SINGLE.CMP2 = duty-- ; //step down the duty cycle each iteration through loop;
  delay(100);  //so we can see the duty cycle changing over time on the scope/with an LED
  if (!duty) {
    if (pass == 0) {
      // After the first pass, lets go up to 100kHz
      pass = 1;
      duty = 199;
      TCA0.SINGLE.PER = 199;
    } else if (pass==1) {
      //and now the requested 62 kHz (actually 62.11kHz)
      pass=2;
      duty = 322;
      TCA0.SINGLE.PER = 322;
    } else { // and back to the beginning.
      pass = 0;
      duty = 255;
      TCA0.SINGLE.PER = 255;
    }
  }
}
```

### Example 4: Quick bit of fun with split mode
A quick example of how cool split mode can be - You can get two different PWM frequencies out of the same timer. Split mode only has one mode - both halves of the timer independently count down.

Here, we've made it even more interesting by using two frequencies almost identical to each other.... they will "beat" against each other weith a frequency of 1.43 Hz (366 Hz / 256). You should be able to observe that with a bicolor LED (and appropriate resistor) between the two pins. These have two LEDs with opposite polarity, typically a red and a green, connected between two pins... the question is - what will it look like? How will it be different from a single color LED? Make predictions and then test them. When I (Spence) did this, my prediction was wrong.

```
#if defined(MILLIS_USE_TIMERA0)
#error "This sketch takes over TCA0, don't use for millis here."
#endif


void setup() {
  // We will be outputting PWM on PD2 amd PD3
  // No need to enable split mode - core has already done that for us.
  pinMode(PIN_PD2, OUTPUT); //PD2 - TCA0 WO2
  pinMode(PIN_PD3, OUTPUT); //PD3 - TCA0 WO3
  PORTMUX.TCAROUTEA = (PORTMUX.TCAROUTEA & ~(PORTMUX_TCA0_gm)) | PORTMUX_TCA0_PORTD_gc; // Variety! Also on all parts!
  TCA0.SPLIT.CTRLB = TCA_SPLIT_LCMP2EN_bm | TCA_SPLIT_HCMP0EN_bm; //PWM on WO2, WO3
  TCA0.SPLIT.LPER = 0xFF; // Count all the way down from 255 on WO0/WO1/WO2
  TCA0.SPLIT.HPER = 0xFE; // Count down from only 254 on WO3/WO4/WO5
  TCA0.SPLIT.LCMP2 = 128; // 50% duty cycle
  TCA0.SPLIT.HCMP0 = 127; // 50% duty cycle
  TCA0.SPLIT.CTRLA = TCA_SPLIT_CLKSEL_DIV256_gc | TCA_SPLIT_ENABLE_bm; //enable the timer with prescaler of 256 - slow it down so the phases shift more slowly, but not so slow it would flicker...
}

void loop() {
  //nothing to do here but enjoy your PWM.
}
```
