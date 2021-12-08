#include <Event.h>
#include <Logic.h>

void setup() {
  initThirdPWM();
  Serial.begin(115200);
}



void loop() {
  static uint16_t i = 1 ;
  // show all three TCD0 PWM channels
  // If you have a 'scope, you can see that they are also each active for a different "part" of the cycle!
  if (i < 508) {
    analogWrite(PIN_PA6, i >> 1);
    analogWrite(PIN_PA7, 255 - (i >> 1));
    if (i < 255) {
      PA3DutyCycle(i);
    } else {
      PA3DutyCycle(510 - i);
    }
  } else {
    i = 1;
  }
  delay(200);
  i++;
}
void initThirdPWM() {

  TCD0.CTRLA &= ~TCD_ENABLE_bm;
  TCD0.CTRLA = (TCD_CLKSEL_OSCHF_gc | TCD_CNTPRES_DIV4_gc | TCD_SYNCPRES_DIV8_gc);
  // switch from /1 sync prescale (for maximum speed synchronization) to /8
  // and from /32 prescale on count to /4 (same overall frequency).
  TCD0.DLYCTRL = 0x3E;
  // Set delay prescale to /8 - giving us 8x8 = 64 prescale. But we only have 1-255 counts on the delay counter, while in default
  // DxCore/megaTinyCore, TCD0 is usually configured to count to 510, or higher (to get the target "basic PWM" frequency
  // of not more than 1.5 kHz, and not less than 500 Hz (er, okay, 490), a range chosen so not preclude
  // PWMing MOSFETs directly, but otherwise be as fast as possible.
  // With the DLYVAL, we can only go down 50% duty cycle....
  // UNLESS we also llower the TOP, which is now supported!
  // and doesn't break analogWrite() on DxCore (it still does on megaTinyCore. I can't justify it there.
  // however see note below...
  TCD0.CMPBCLR = 254;



  TCD0.DLYVAL = 0x80; // 50% duty cycle
  while (!(TCD0.STATUS & 0x01));
  TCD0.CTRLA |= 1;
  // event channel 0
  Event0.set_generator(gen::tcd0_cmpbclr);
  Event0.set_user(user::ccl0_event_a);
  Event0.start();
  Event1.set_generator(gen::tcd0_progev);
  Event1.set_user(user::ccl0_event_b);
  Event1.start();
  Logic0.enable = true;
  Logic0.output = out::enable;
  Logic0.input0 = in::event_a;
  // Logic0.input0 = in::masked;
  Logic0.input1 = in::event_b;
  Logic0.input2 = in::feedback;
  Logic0.truth = (0xB2);
  // low nybble is when input 2 is low. Value 2, 0b0010, means the output pin is driven low
  // and only switches if input 0 goes high, and then we will output high until,
  // er the high nybble, 0xB, 9b1011: it when input 1 goes high, it turns off again
  // . when input 0 goes high,
  // Hence it's high between TCD0_CMPBCLR and TCD_PROGEV (delayed eventt, triggered  from CMPBCLR)
  Logic0.init();
  Logic::start();
}

void PA3DutyCycle(uint8_t duty) {
  // You can handle the half duty issue here, if it is relevant at the chip and clock speed you are using
  // with the magic of the invert function!
  // if (duty > 0x80) {
  //   PORTA.PIN3CTRL |= PORT_INVEN_bm;
  //
  // }
  // duty = (duty << 1)+ 1;
  TCD0.DLYVAL = duty;
  while (!(TCD0.STATUS & TCD_CMDRDY_bm)); // wait for any sync weirdness, because TCD is wacky
  // then tell it to sync at end of cycle (better than sync, though i'm not sure how it interacts here)
  TCD0.CTRLE = TCD_SYNCEOC_bm;
}
