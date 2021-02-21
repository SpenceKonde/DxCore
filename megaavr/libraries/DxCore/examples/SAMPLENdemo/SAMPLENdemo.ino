/*
ADC0.SAMPCTRL demo

Connect a 1 MEG resistor between PD1 and PD0.

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

void setup() {
  pinMode(PIN_PD1, OUTPUT);
  digitalWrite(PIN_PD1, 0);
  ADC0.SAMPCTRL = 0xFF;
  analogReadResolution(12);
  Serial.begin(1000000);
  delay(1000);
  analogRead(PIN_PD0);
}

void loop() {
  ADC0.SAMPCTRL++;
  digitalWrite(PIN_PD1, 1);
  Serial.print(analogRead(PIN_PD0));
  analogRead(PIN_PD0);
  analogRead(PIN_PD0);
  analogRead(PIN_PD0);
  analogRead(PIN_PD0);
  analogRead(PIN_PD0);
  analogRead(PIN_PD0);
  analogRead(PIN_PD0);
  Serial.print(",");
  digitalWrite(PIN_PD1, 0);
  Serial.println(analogRead(PIN_PD0));
  analogRead(PIN_PD0);
  analogRead(PIN_PD0);
  analogRead(PIN_PD0);
  analogRead(PIN_PD0);
  analogRead(PIN_PD0);
  analogRead(PIN_PD0);
  analogRead(PIN_PD0);
  if (ADC0.SAMPCTRL == 255) {
    while (1);
  }
}
