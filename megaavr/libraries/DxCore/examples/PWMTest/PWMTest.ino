/******************************************************************************
 * PWM Test for AVR DA, AVR DB, AVR DD, and possibly AVR EA.
 *
 * This sketch will verify that the core is correctly outputting PWM on the
 * pins that the mux's are set to. It also measures the frequency of the PWM
 * (somewhat crudely, but it's easily good enough to see if you're on target)
 *
 * This is designed specifically to test initTCx(), analogWrite() and
 * turnOffPWM() (via digitalWrite); While the syntactical correctness is
 * tested every time you compile, whether the behavior is correct is another
 * matter. This has historically been the hardest non-exotic part of the
 * core to implement (analogWrite is not exotic, nor is the implementation -
 * it's implementing a ubiquitous feature (albeit with the bonus remapping
 * feature added on), as it's all written in C. Serial, on the other hand,
 * while probably harder in an absolute sense, was much less unpleasant. It
 * still implements a ubiquitous part of the API, but it does so using not
 * just assembly, but naked ISRs written in it, and constitutes a very
 * exotic bit of code.
 *
 * It should compile for all such parts. First, it outputs the tables of the
 * pins within each pinset of all timers except TCBs. 255 is NOT_A_PIN (at
 * least on this part, or if it is present, it's not available with current
 * compile options, like a timer output on PA0 when using a crystal).
 *
 * After this, we start with TCA0, look at the PORTMUX register to see which
 * pinset it is on, then at our pinset table to know what those pins are
 * and then we analogWrite() the pin to start PWM for 1 second. During that
 * time we poll the port input register and count the transitions. Repeat for
 * all channels that aren't "NOT_A_PIN".
 *
 * Then it does the same for TCA1 if any. When done with the two TCAs it
 * **calls takeOverTCA0 and takeOverTCA1 if there's a TCA1**, and then
 * sets the CTRLA on both timers back to what they were, so it serves as a
 * clock source for the TCBs.
 *
 * Then it tests the TCBs, skipping the one with millis on it.
 *
 * Finally, it tests the TCD, if any.
 *
 * In the future this will be expanded to also do the EB and DU.
 *
 * DU is just a few more preprocessor directives to not offer a PORTC, but the
 * EB will require adding two new timers. TCE will be added either before or
 * after TCA, since it will need to get the same takeover + restart for clock
 * thing as TCA, before we start testing TCBs.
 *
 * At the end, it will report the total number of tests that pass, fail, or
 * were skipped. Peripherals not present are not recorded as tests or skips,
 * same with pins not present when the peripheral is.
 * PA0/PA1 with a crystal don't count as attempts.
 * But a pin blocked off by millis is counted as an attempt.
 *
 *****************************************************************************/

/* Key Parameters - set these to match your wiring: */

// Adjust as needed
#define MYSERIAL Serial
// #define MYSERIALSWAP
// Override below logic:
#if !defined MYSERIALSWAP
  #if defined(__AVR_DD__) && defined(_AVR_PINCOUNT) && _AVR_PINCOUNT == 14 && MYSERIAL == SERIAL
    #define MYSERIALSWAP 3 // Too likely I'll be using a clock, and it's a pain to have to wire up VDDIO2 to use serial.
  #elif MYSERIAL == SERIAL && CLOCK_SOURCE != 0
    #define MYSERIALSWAP 1
  #elif (0 && (MYSERIAL == SERIAL && (_AVR_PINCOUNT == 64 && defined(__AVR_DB__) || defined(XTAL_PINS_HARDWIRED)))) // For arduino designs in testing.
    #define MYSERIALSWAP 1
  #else
    #define MYSERIALSWAP 0
  #endif
#endif

/* End of parameters/options section */
#include "FullSzeAVR_Timer.h"
#include <EEPROM.h>

#if defined(TCA0)
void printTCA0Status() {
  uint16_t test = (uint16_t)&TCA0;
  volatile uint8_t *bp;
  bp = (volatile uint8_t *)test;
  for (uint8_t i = 4; i; i--) {
    if (i != 4) {
      MYSERIAL.println();
    }
    bp = MYSERIAL.printHex(bp, (uint8_t) 16, ':');
  }
  MYSERIAL.println();
}
#endif
#if defined(TCA1)
void printTCA1Status() {
  uint16_t test = (uint16_t)&TCA1;
  volatile uint8_t *bp;
  bp = (volatile uint8_t *)test;
  for (uint8_t i = 4; i; i--) {
    if (i != 4) {
      MYSERIAL.println();
    }
    bp = MYSERIAL.printHex(bp, (uint8_t) 16, ':');
  }
  MYSERIAL.println();
}
#endif
#if defined(TCF0)
void printTCF0Status() {
  uint16_t test = (uint16_t)&TCF0;
  volatile uint8_t *bp;
  bp = (volatile uint8_t *)test;
  for (uint8_t i = 4; i; i--) {
    if (i != 4) {
      MYSERIAL.println();
    }
    bp = MYSERIAL.printHex(bp, (uint8_t) 16, ':');
  }
  MYSERIAL.println();
}
#endif
#if defined(TCD0)
void printTCD0Status() {
  uint16_t test = (uint16_t)&TCD0;
  volatile uint8_t *bp;
  bp = (volatile uint8_t *)test;
  for (uint8_t i = 4; i; i--) {
    if (i != 4) {
      MYSERIAL.println();
    }
    bp = MYSERIAL.printHex(bp, (uint8_t) 16, ':');
  }
  MYSERIAL.println();
}
#endif
#if defined(TCE0)
void printTCE0Status() {
  uint16_t test = (uint16_t)&TCE0;
  volatile uint8_t *bp;
  bp = (volatile uint8_t *)test;
  for (uint8_t i = 4; i; i--) {
    if (i != 4) {
      MYSERIAL.println();
    }
    bp = MYSERIAL.printHex(bp, (uint8_t) 16, ':');
  }
  MYSERIAL.println();
}
#endif
#if defined(TCF0)
void printTCE0Status() {
  uint16_t test = (uint16_t)&TCF0;
  volatile uint8_t *bp;
  bp = (volatile uint8_t *)test;
  for (uint8_t i = 4; i; i--) {
    if (i != 4) {
      MYSERIAL.println();
    }
    bp = MYSERIAL.printHex(bp, (uint8_t) 16, ':');
  }
  MYSERIAL.println();
}
#endif
uint8_t SuccessCount = 0;
uint8_t AttemptCount = 0;
uint8_t SkipCount = 0;
uint8_t tcactrla = 0;

uint8_t CurrentPortmux = 255;
uint8_t CurrentTimer = tca0; // TCA0
static uint8_t CurrentChannel = 0;
uint8_t CurrentPin = NOT_A_PIN;
uint8_t CurrentTimerIndex = 0;
#if defined(__AVR_DB__) && defined(PORTG)
  #define MAXREG
#endif
void setup() {
  // put your setup code here, to run once:
  #if defined(MAXREG)
  VPORTE.OUT     &= ~(0x40);
  VPORTE.DIR     |= 0x40;
  VPORTG.OUT     |= 0x40;
  VPORTG.OUT     |= 0x80;
  VPORTG.DIR     |= 0x40;
  VPORTG.DIR     |= 0x80;
  delay(100);
  VPORTE.OUT     |= 0x40;
  #endif
  #if defined(MYSERIALSWAP)
  MYSERIAL.swap(MYSERIALSWAP);
  #endif
  PORTMUX.USARTROUTEA = 1;
  VPORTA.DIR |= 0x10;
  MYSERIAL.begin(115200);
  delay(100);
  MYSERIAL.println("PWM selftest");
  CurrentTimer = MyTimers[0];
  delay(1000);
  for (byte x = 0; x < 7; x++) {
    for (byte y = 0; y < 6; y++) {
      MYSERIAL.print(TCA0pinsets[6 * x + y]);
      MYSERIAL.print(", ");
    }
    MYSERIAL.println();
  }
  MYSERIAL.println();
  #ifdef TCA1
  for (byte x = 0; x < 6; x++) {
    for (byte y = 0; y < 6; y++) {
      MYSERIAL.print(TCA1pinsets[6 * x + y]);
      MYSERIAL.print(", ");
    }
    MYSERIAL.println();
  }
  MYSERIAL.println();
  #endif
  #ifdef TCD0
  for (byte x = 0; x < 5; x++) {
    for (byte y = 0; y < 4; y++) {
      MYSERIAL.print(TCD0pinsets[4 * x + y]);
      MYSERIAL.print(", ");
    }
    MYSERIAL.println();
  }
  MYSERIAL.println();
  #endif
  #ifdef TCE0
  for (byte x = 0; x < 5; x++) {
    for (byte y = 0; y < 8; y++) {
      MYSERIAL.print(TCDEpinsets[8 * x + y]);
      MYSERIAL.print(", ");
    }
    MYSERIAL.println();
  }
  MYSERIAL.println();
  #endif
  #ifdef TCF0
  for (byte x = 0; x < 8; x++) {
    for (byte y = 0; y < 2; y++) {
      MYSERIAL.print(TCDEpinsets[2 * x + y]);
      MYSERIAL.print(", ");
    }
    MYSERIAL.println();
  }
  MYSERIAL.println();
  #endif
  analogWrite(PIN_PD6, 128);
  delay(100);
  uint8_t dacpassed = 0;
  int16_t dacread = analogRead(PIN_PD6);
  dacread = analogRead(PIN_PD6);
  AttemptCount++;
  if ((dacread < 612) && (dacread > 412)) {
    dacpassed = 1;
    MYSERIAL.print("DAC Turn On OK");
  }
  MYSERIAL.println(dacread);
  delay(2000);
  pinMode(PIN_PD6, OUTPUT);
  digitalWrite(PIN_PD6, LOW);
  PORTD.PIN6CTRL = 0;
  dacread = analogRead(PIN_PD6); // throw away the first reading
  dacread = analogRead(PIN_PD6); // This reading will be ok.
  if ((dacread < 200) && dacpassed) {
    SuccessCount++;
    MYSERIAL.println("DAC Turn Off OK");
  } else {
    MYSERIAL.print("DAC not turned off, voltage reads as ");
    MYSERIAL.println(dacread);
  }
}


void showDone() {
  MYSERIAL.println("The test has completed");
  MYSERIAL.print("Pins tested: ");
  MYSERIAL.println(AttemptCount);
  MYSERIAL.print("Pass: ");
  MYSERIAL.println(SuccessCount);
  MYSERIAL.print("Fail: ");
  MYSERIAL.println(AttemptCount - SuccessCount - SkipCount);
  MYSERIAL.print("Skipped: ");
  MYSERIAL.println(SkipCount);
  if (SuccessCount + SkipCount < AttemptCount) {
    MYSERIAL.print("Test FAILED!");
    MYSERIAL.print(AttemptCount - SuccessCount - SkipCount);
    MYSERIAL.print('/');
    MYSERIAL.print(AttemptCount - SkipCount);
    MYSERIAL.println(" failed!");
  } else {
    MYSERIAL.println("Test PASSED!");
  }

  MYSERIAL.println();
}
void loop() {
  // put your main code here, to run repeatedly:
  uint8_t timertype = CurrentTimer & 0xF0;
  _swap(timertype);
  uint8_t timernum = CurrentTimer & 0x0F;
  uint8_t currentpin = NOT_A_PIN;
  switch (timertype) {
    case 0: {
        //TCA
        if (timernum == 0) {
          CurrentPortmux = ((PORTMUX.TCAROUTEA & 0x07));
        } else {
          CurrentPortmux = ((PORTMUX.TCAROUTEA >> 3));
        }
        MYSERIAL.print("Testing TCA");
        MYSERIAL.print(timernum);
        MYSERIAL.print(" mux number ");
        MYSERIAL.print(CurrentPortmux);
        MYSERIAL.print(" channel ");
        MYSERIAL.println(CurrentChannel);
        if (timernum == 0) {
          currentpin = TCA0pinsets[6 * CurrentPortmux + CurrentChannel];
        }
        #if defined(TCA1)
        else {
          currentpin = TCA1pinsets[6 * CurrentPortmux + CurrentChannel];
        }
        #endif
        MYSERIAL.print("Pin is ");
        MYSERIAL.println(currentpin);
        break;
      }
    case 1: {
        //TCB
        if (MILLIS_TIMER & 0x20) {
          if ((MILLIS_TIMER & 0x07) == timernum) {
            MYSERIAL.print("Millis is using TCB");
            SkipCount++;
            AttemptCount++;
            currentpin = 254;
            break;
          }
        }
        MYSERIAL.println();
        MYSERIAL.print("Testing TCB");
        MYSERIAL.print(timernum);
        CurrentPortmux = (PORTMUX.TCBROUTEA & (1 << timernum)) ? 1 : 0;
        currentpin = TCBpinsets[(timernum << 1) + CurrentPortmux];
        if (CurrentPortmux) {
          MYSERIAL.print(" AltPin ");
        } else {
          MYSERIAL.print(" DefaultPin ");
        }
        MYSERIAL.println(currentpin);
        break;
      }
    case 3: {
        #if defined(TCD0)
        MYSERIAL.print("Testing TCD0 mux number ");
        MYSERIAL.println(PORTMUX.TCDROUTEA);
        currentpin = TCD0pinsets[(4 * PORTMUX.TCDROUTEA) + CurrentChannel];
        MYSERIAL.print("Pin is ");
        MYSERIAL.println(currentpin);
        #else
        CurrentChannel = 127;
        currentpin = 255;
        #endif
        break;
      }
    case 255: {
        MYSERIAL.println("Done!");
        while (1);
      }
    default: {
        MYSERIAL.println("Timer not present on this part");
      }
  }
  if (currentpin == NOT_A_PIN || currentpin == 254) {
    MYSERIAL.println("The current configuration does not permit PWM from this timer channel.");
  } else {
    uint16_t tglcount = 0;
    PORT_t *thisport = portToPortStruct(digitalPinToPort(currentpin));
    uint8_t bit_mask = digitalPinToBitMask(currentpin);
    if (bit_mask == NOT_A_PIN) {
      MYSERIAL.println("Pin does not exist on this part");
    } else {
      uint8_t laststate = ((thisport->IN) & bit_mask) ? 1 : 0;
      MYSERIAL.flush();
      AttemptCount++;
      analogWrite(currentpin, 129);
      uint32_t startmillis = millis();
      while (millis() - startmillis < 1000) {
        if ((((thisport->IN) & bit_mask) ? 1 : 0) != laststate) {
          tglcount++;
          laststate = !laststate;
        }
      }
      //MYSERIAL.printHexln((uint8_t*)(uint16_t)&TCD0,32,' ');
      //MYSERIAL.printHexln(GPIOR0);
      MYSERIAL.println(digitalPinToPort(currentpin));
      MYSERIAL.print("One second togglecount: ");
      MYSERIAL.println(tglcount);
      MYSERIAL.print("Freq = ");
      MYSERIAL.print(tglcount >> 1);
      MYSERIAL.println(" Hz.");
      digitalWrite(currentpin, LOW);
      if (tglcount > 900 && tglcount < 4000) {
        tglcount = 0;
        laststate = 0;
        while (millis() - startmillis < 1000) {
          if ((((thisport->IN) & bit_mask) ? 1 : 0) != laststate) {
            tglcount++;
            laststate = !laststate;
          }
        }
        if (tglcount < 2) {
          SuccessCount++;
        } else {
          MYSERIAL.print("Uhoh, saw additional pulses: ");
          MYSERIAL.println(tglcount);
        }
      }
    }
  }
  MYSERIAL.println("--------------------");
  switch (timertype) {
    case 0: {
        //TCA
        if (CurrentChannel >= 5) {
          MYSERIAL.println();
          CurrentChannel = 0;
          CurrentTimerIndex++;
          #if !defined(TCA1)
          if (CurrentTimerIndex == 1)
          #else
          if (CurrentTimerIndex == 2)
          #endif
          {
            tcactrla = TCA0.SPLIT.CTRLA;
            takeOverTCA0();
            TCA0.SINGLE.CTRLA = tcactrla;
            #if defined(TCA1)
            takeOverTCA1();
            TCA1.SINGLE.CTRLA = tcactrla;
            #endif
          }
        } else {
          CurrentChannel++;
        }
        break;
      }
    case 1: {
        //TCB
        // Just go to the next one.
        CurrentTimerIndex++;
        break;
      }
    case 3: {
        //TCD
        #if defined(TCD0)
        if (CurrentChannel >= 3) {
          CurrentChannel = 0;
          CurrentTimerIndex++;
        } else {
          CurrentChannel++;
        }
        #else
        CurrentChannel = 0;
        CurrentTimerIndex++;
        #endif
        break;
      }
    case 255:
    default: {
        showDone();
        while (1);
      }
  }

  CurrentTimer = MyTimers[CurrentTimerIndex];
  if (CurrentTimer == 255) {
    showDone();
    while (1);
  }
}
