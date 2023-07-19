# ADC, resistor dividers, and the internal pullup
**Dirty trick** - sometimes it is desirable to have a means of measuring the approximate resistance of something. The most obvious example is when you are trying to determine parameters of a device that doesn't have the means to actively communicate, but which you otherwise control. For example, you have 5 different gadgets you can connect. The gadgets do not have an output built in - they just take power, ground, and a signal, and the output is the external behavior of the gadgets (which might be lights that glow based on the input, motors that spin at a speed controlled by the input, etc). Unfortunately your application requirements dictate that you must be able to connect any of the gadgets to the device, using as few additional pins as possible (ideally 1), and as few external parts as possible. Since you only need to know if you have a Gadget of type 0, 1, 2, 3, or 4, you might add a resistor divider between power and ground on the gadget. This uses 1 pin and 2 external components, but you can easily distinguish far more than 5 values. There are a couple of pitfalls here though - first, while the current lost to this divider would be trivial compared to a motor while running or a light while on, if you need low standby power consumption, you can't have a resistor divider sitting there draining power continuously. Also, the resistors should be close to the point where the connection to the microcontroller is made, so as to ensure that it's supply rail and ground are at the same voltage as those of the microcontroller. High currents and long wires would push values to the middle were the resistors located at the gadget end of the gadget<->MCU interconnect - which is okay, I guess, except that you need to add a pair of resistors to the end of a cable, and so there are going to be pieces of exposed conductor carrying the supply voltage right next to one tied to ground, requiring particular care in that end of the interconnect, which may not be practical (for example, if you're confined to using a common crimp type connector, those don't have a connector body that can shield your resistor pair, so you need to add not just insulation but armor around it to prevent a short during handling.

Anyone staring at this problem for long enough will realize that they can connect the resistor from ground to the added "identification" pin, at the MCU end of the interconnect. You still need to protect it from harm, but there's no exposed power rail on the other resistor for it to short to, so your countermeasures can go down a few tiers, and it won't drain power while off unless you leave the pullup on. Even in the case that the resistor were unprotected, the likely failure modes involve the resistor either coming disconnected entirely (infinite resistance, Vpin = Vdd), or shorting to it's other lead (approx. zero resistance, Vpin = Gnd). But those two conditions are both immediately apparent from the measurement - a value near the the upper limit means there's nothing connected, or if something is connected, it lacks a working resistor.

So you turn on the internal pullup, wait a few moments for the voltage to stabilize, and then read it with the ADC, and based on that you can determine what "bucket" the value falls into and that tells you what the gadget's "identification" resistor is, and hence what is connected. And you could even do it using a pin you were already using to detect if anything was connected at all (ex, if it formerly grounded that pin, and now we adapt it to ) The internal pullup is... oh.... hmmm.... "between 30k and 50k" says the datasheet "around 30k" say forum-goers without being challenged. That's quite a range - wouldn't the bins need to be awfully large to catch all values with such large limits? And what if the voltage drop across the pullup is nonlinear with the current through it? And what about the supply voltage?

## So can I do that?
In most cases, yes.
If you're willing to calibrate each specimen, this should work quite well

Tests were conducted exactly as such a sketch would, except that to expedite things, I wired up 9 resistors from 22k to 470k that I planned to use, 1 per I/O pin not used by serial on an ATtiny1624, and 5V, 3.3V, and courtesy of a bad connection, 2.3v were tested.
I found:
Resistance decreases slightly as supply voltage increases; over the 2.7v interval tested, it was found to have an average (of the values of Rpullup calculated from the ADC measurements and known external resistance) of 33146 ohms on a particular specimen at 3.3v, ranging to 33862 at 2.3 and 32182 at 5 volts, that is around a 5% change over most of the operating voltage range. Not bad - and it's nearly linear with voltage.
How about with current? Within that range, the measurements varied by half of the tolerance of the  +/- 5% resistors (which these days are usually better than 5%), 2.5% which is a lot better than 30-50k
Scrambling the connections and remeasuring yielded results inconsistent with concern over differing pin offset voltages. The estimated resistance was within except with the highest resistance (where, since it's near the edge of the scale, small changes make a large difference). Hence most of that 5% range could be explained solely by resistor-to-resistor variation.

No temperature testing was done as I lack appropriate facilities. I'm not going to cook my board in a (somehow moisture sealed) double boiler to get 100C or run tests under boiling computer spray duster (using it as freeze spray, while effective, is highly environmentally irresponsible. Tetrafluoroethane is a much stronger greenhouse gas than CO2 - if you boil off a little over a pound of it, over the next 20 years, it's as if you'd burned an extra ton of coal just to test the ADC at -25C (that was the closest to the -40 spec I could come up with a way of reaching that's "hardware store", though one could then put a vacuum pump on it to lower it's boiling point further to keep it closer to -40. That sounds crude, but that's actually a pretty common method used in scientific research. Any of those experiments you might read that are performed with "millikelvin" temperatures probably have one stage of the cooling that involves pumped helium (they generally have many stages of cooling, each one able to get to a lower temperature than the last, as long as it can dump it's waste heat at a lower temperature, and is less efficient and more expensive. You're getting to the end iof the line when liquid helium at atmospheric pressure isn't cold enough, and you're pumping it down (and recycling the precious helium). But what I just described with computer spray duster being used to chill a circuit board? Yeah it's as crude as it sounds. That stuff is also frequently flammable (more so than the labeling implies), and I don't see how any products of the combustion could be not-highly-noxious (though they'd probably lower the global warming potential significantly;. So I'll take the datasheet's word, which is that the difference is typically around +/- 18%

## So how to do it right?
We know that Rpu is a function of T and V, the individual specimen.

We also probably know what voltage the device is intended to run at, and what temperature too.

If you are going to to do this, I suggest storing in the userrow.

V could be adequately represented with a number from (1.8v) 0 to 255 (6.5V, the abs. max on the chip) - yielding around 18 mV per LSB, and the -40-125C of an extended temp range part is also easily represented with a byte.
Thus, a 16-bit number representing the pullup strength in ohms can be used as a 1 point approximation. If you also measured voltage and  temperature with the on-chip facilities, you could then use a simple linearized model to account for temperature and voltage differences if you needed to - I would suggest that the calibration step adjust the resistance  for normalized voltage = 0 and normalized temperature = 0 (1.8v, -40 C), as you will likely only perform that once and then load different code, and doing that makes using it easier, since you only need to perform two multiplications with 16-bit integers

Rpu = Rpu<sub>cal</sub> - k1 (V-V<sub>cal</sub>) + k2 (T - T<sub>cal</sub>)

From that, we can see that:

Rpu = Rpu<sub>cal</sub> + k1 V<sub>cal</sub> - k2 T<sub>cal</sub> + k2 T - k1 V

Let's define T as the chip's temperature as measured by the on-chip temperature sensor, normalized to the chip's operating temperature range by subtracting the minimum operating temperature (-40).

```c++
// Rpu = Rpucal + k1*Vcal - k2 * Tcal + k2 * T - k1 * V
// Hence, to make the result easy to use, we recommend
typedef struct RPUcal_t {
  uint16_t base; // Rpucal + (k1*Vcal)/32 - k2 * Tcal
  uint8_t k1;
  uint8_t k2;
} RPUcal_t
RPUcal_t * rpucal;
rpucal = (RPUcal_t *)(USERROW_EHD - 16); // Store it in the user row, before any oscillator tuning.
uint16_t Rpu;
if (rpucal -> k1 == 0xFF || rpucal -> k2 == 0xFF) { // have only done a single-point cal.
  Rpu = rpucal -> base
else {
  // Since k1 would end up being very small, something like 6.7, we can multiply it by 32 to preserve some of the accuracy and then rightshift the product 5 places.
  uint16_t Rpu=rpucal -> base + ((rpucal -> k1 * getVoltageAsByte()) >> (5)) + rpucal -> k2 * getTempAsByte(); // writing these left as an exercise for the reader.
}

```
If you can rely on the operating conditions being similar, you could calibrate it at those approximate conditions, store it in the user row, and then assume it's close enough (This depends on how accurately you need to know it, and how much voltage and temperature will vary), or instead of measuring at a second point, you can use estimates for k1 and k2: k1 = 215, k2 = 75, measured on a 1624. With my chip, the base value would be around 30100 with those calibration values, and thus the predicted pullup strength is:
```c++
Rpu = 30100 + ((215 * getVoltageAsByte()) >> 5) + 76 * getTemperatureAsByte();
```

An even more accurate way to calculate the pullup strength would be to recall that there was negligible variation between pins - if you can spare a microcontroller pin, a reference resistor could be used, and the calibration performed "live" at the actual conditions.

Through these methods, it is possible to use the internal pullup as one side of a resistor divider for this sort of purpose, provided that you can ensure that the temperature and voltage variations are either small or account for them (and/or the precision required low enough) that simply measuring it at approximately those conditions will give you a suitable calibration value. As long as the number of buckets you need to sort values into remains small, it runs on a regulated power supply, and the device will operate at comfortable human living temperatures, only a single point calibration should be needed. It may in fact be found as a larger sample of chips are tested that even this is unnecessary; maybe Microchip has really good control over their process, and while their datasheets give them great leeway, they actually repeatedly hit the bullseye. My initial testing seems to suggest that that may be the case, at least within part families - Every 2-series I tested has been within a kOhm or two!
