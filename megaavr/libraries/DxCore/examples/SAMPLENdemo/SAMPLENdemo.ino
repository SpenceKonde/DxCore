/*
ADC0.SAMPCTRL demo

Connect a 1 MEG resistor between the two analog inputs. This sketch, because it use used as a "compile test" to approve new
versions determines valid analog input pins using the _VALID_ANALOG_PIN() macro. _VALID_ANALOG_PIN(0) checks analog input pins
starting from the one associated with channel 0 of the ADC, and returns the first one that exists, _VALID_ANALOG_PIN(1) returns
the second. Note that this is not intended for public consumption, and should not be relied on outside of CI-testing scenarios.

These pins work out to be:
| Part family   | FIRST_PIN | SECOND_PIN |
|---------------|-----------|------------|
| AVR DA+DB>48p | PIN_PD0   | PIN_PD1    |
| AVR DB/DD     | PIN_PD1   | PIN_PD2    |
| AVR DD <28pin | PIN_PD4   | PIN_PD5    |
| AVR EA        | PIN_PD0   | PIN_PD0    |


This makes PD0 a very high impedance input, controlled by PD1.
This sketch flips PD1 and then immediately takes some readings on PD0, printing the first one.
Format output is conducive to graphing in excel/etc (save as .csv)
One thing that is important to remember is that what matters is what the last voltage READ BY the ADC was.
For example, if you switch back and forth between reading a high impedance source, and a voltage very close to ground,
your numbers will err low with a low SAMPLEN, while if you were last measuring a high voltage, if would err high.
This of course means that if you are continually measuring the same high impedance, but slow changing voltage, you can
afford to use a shorter sample length than if you were switching between multiple analog voltages.

And finally, notice how it is slower when the change is from low to high vs high to low - which strikes me as
rather odd. Also that it goes all the way to 0 on a LOW, but not all the way to 4095 on a HIGH.


*/

#if defined(THIS_IS_CI) // This sketch is compiletested in many configurations and parts automatically. Within that context, these macros are available to find valid pins
const uint8_t FIRST_PIN = _VALID_ANALOG_PIN(0);
const uint8_t SECOND_PIN = _VALID_ANALOG_PIN(1);
#else
const uint8_t FIRST_PIN = A1;
const uint8_t SECOND_PIN = A2;
#endif

void setup() {
  pinMode(SECOND_PIN, OUTPUT);
  digitalWrite(SECOND_PIN, LOW);
  ADC0.SAMPCTRL = 0xFF;
  analogReadResolution(12);
  Serial.begin(115200);
  delay(1000);
  analogRead(FIRST_PIN);
}

void loop() {
  ADC0.SAMPCTRL++;
  Serial.print("Sampctrl=");
  Serial.println(ADC0.SAMPCTRL);
  Serial.flush();
  digitalWrite(SECOND_PIN, 1);
  Serial.print(analogRead(FIRST_PIN));
  Serial.print(' ');
  Serial.println(analogRead(FIRST_PIN));
  Serial.flush();
  digitalWrite(SECOND_PIN, 0);
  Serial.print(analogRead(FIRST_PIN));
  Serial.print(' ');
  Serial.println(analogRead(FIRST_PIN));
  Serial.flush();
  if (ADC0.SAMPCTRL == 255) {
    while (1);
  }
}
