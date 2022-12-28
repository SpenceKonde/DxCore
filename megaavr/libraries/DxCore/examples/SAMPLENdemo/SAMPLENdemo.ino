/*
ADC0.SAMPCTRL demo

Connect a 1 MEG resistor between the two analog inputs. This sketch, because it is used as a "compile test" to approve new
versions, has to choose pins that always exist on parts being tested. These pins turn out to be PD4-7, and we use the first two.
| Part family   | FIRST_PIN | SECOND_PIN |
|---------------|-----------|------------|
| AVR DA+DB>48p | PIN_PD4   | PIN_PD5    |
| AVR DB/DD     | PIN_PD4   | PIN_PD5    |
| AVR DD <28pin | PIN_PD4   | PIN_PD5    |
| AVR EA        | PIN_PD4   | PIN_PD5    |


This makes PD4 a very high impedance input, controlled by PD5.
This sketch flips PD5 and then immediately takes some readings on PD4, printing the first one.
Format output is conducive to graphing in excel/etc (save as .csv)
One thing that is important to remember is that what matters is what the last voltage READ BY the ADC was.
For example, if you switch back and forth between reading a high impedance source, and a voltage very close to ground,
your numbers will err low with a low SAMPLEN, while if you were last measuring a high voltage, if would err high.
This of course means that if you are continually measuring the same high impedance, but slow changing voltage, you can
afford to use a shorter sample length than if you were switching between multiple analog voltages.

And finally, notice how it is slower when the change is from low to high vs high to low - which strikes me as
rather odd. Also that it goes all the way to 0 on a LOW, but not all the way to 4095 on a HIGH.


*/
#define FIRST_PIN PIN_PD4
#define SECOND_PIN PIN_PD5

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
