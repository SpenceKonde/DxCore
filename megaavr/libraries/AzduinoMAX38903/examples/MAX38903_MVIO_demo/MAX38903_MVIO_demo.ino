/*  MAX39803 selectable voltage regulator demo
 * This is designed for the Azduino Pro AVR128DB64 development board with the
 * selectable voltage regulator option. On these boards, with the option included
 * (it is extra cost, as the regulator costs more than the microcontroller. Typical of MAXIM semiconductor
 * MAXIM is short for "MAXIMum price").
 * The output of this regulator drives VDDIO2. The input voltage must not exceed 5.5v
 * (that's what you get when all the other specs on the part are insanely great, something had to give).
 * Osing the 5V regulator output to drive this may be achieved using the solder bridge jumpers, though the 5v out
 * option will be a bit low in that case.
 *
 * On these boards, enable and a 100k pulldown is connected to PE6, and PG6 and PG7 are SELA and SELB.
 * I assessed these to be the pins that were least valuable for other purposes, as they have few alternate functions.
 *
 * This also provides some demonstration of working with MVIO.
 *
 * Expected behavior:
 * 0.0V, 1.2V and 1.5V are too low for MVIO to work.
 * 1.8 through 4.0V should get results very close to 1800-4000mV
 * 5.0V should be close to 5000mv as long as the regulator is being fed between 5.2 and 5.5V.
 * Otherwise, it should be a couple hundred mV lower than the input voltage due to the (very low) dropout.
 *
 * (C) 2023 Spence Konde as part of DxCore. Open source, released under LGPL 2.1
 */

#include <AzduinoMAX38903.h>
uint8_t voltage_options[] = {REG_OFF, REG_1V2, REG_1V5, REG_1V8, REG_2V5, REG_3V0, REG_3V1, REG_3V3, REG_4V0, REG_5V0};
uint8_t BCDvoltage[] = {0x00, 0x12, 0x15, 0x18, 0x25, 0x30, 0x31, 0x33, 0x40, 0x50};
void setup() {
  analogReference(INTERNAL1V024); // lowest reference (will by half of reference voltage at most, because the internal facility for measuring VDDIO2 has a div-by-10 mechanism.)
  analogReadResolution(12); // maximum resolution
  // Serial.swap(1) // Uncomment if an HF crystal is used, as that prevents serial from using PA0 and PA1. In this case, be sure to set the solder bridge jumpers appropriately per documentation.
  Serial.begin(115200);
  Serial.print("MVIO status (should be 0) is: ");
  Serial.println(MVIO.STATUS);
  if (MVIO.STATUS) {
    Serial.println("ERROR: MVIO shows as working, but the regulator should be off, this indicates either MVIO disabled in fuses or that the board has a different source of power connected to VDDIO2 and/or the board does not have this regulator.");
    Serial.println("STOP: nothing to do here - this is not a working board with the required features and configuration, and the rest of the sketch cannot give meaningful results");
    while (1); // hang forever.
  }
}

void loop() {
  for (uint8_t i = 1; i < 10; i++) {
    Serial.println("Changing regulator voltage to: ");
    Serial.print(BCDvoltage[i] >> 4);
    Serial.write('.');
    Serial.println(BCDvoltage[i] & 0x0F);
    setMAX38903Voltage(voltage_options[i]);
    delay(100); //let everything settle - this is an eternity compared to expected rise time.
    if (MVIO.STATUS) {
      uint16_t adcreading = analogRead(ADC_VDDIO2DIV10);
      // this gives us 0-4095, and the voltage in millivolts is 10/4ths of the reading.
      adcreading *= 10; // multiply by 10
      adcreading >>= 1; // divide by 2;
      uint8_t temp = adcreading & 0x01;
      adcreading >>= 1; // divide by 2 again.
      adcreading += temp; //and round up if appropriate.
      Serial.print("MVIO OK, voltage on VDDIO2 is: ");
      Serial.print(adcreading);
      Serial.println(" mV");
    } else {
      Serial.println("MVIO not OK. This is expected with 1.2 and 1.5v, as MVIO must have 1.65v to function");
    }
    delay(5000); // to give you time to verify with a multimeter if you wish.
  }

  Serial.println("Turning off regulator.");
  setMAX38903Voltage(voltage_options[i]);
  delay(1000);
  if (!MVIO.STATUS) {
    Serial.println("MVIO not powered anymore");
  } else {
    Serial.println("MVIO is still powered, this is unexpected...")
  }
  delay(10000);
}
