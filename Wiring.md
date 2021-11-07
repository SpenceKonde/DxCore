### Minimal Wiring Requirements

These are the bare minimum requirements to bring the chip to life and program it using the Arduino IDE. The 0.1uf capacitors are *not* optional - you must use these or the parts will behave erratically, and may not work at all; these should be placed as close to the chip as possible.

* Vcc pin(s) connected to supply voltage (1.8-5v depending on part, clock speed, and fuse settings)
* Gnd pin(s) connected to Ground
* 0.1uF ceramic capacitor connected between each pair of Vcc and Gnd pins, as close to the part as practical.
* (DB and DD series only) 1uF ceramic capacitor between VDDIO2 and Gndd
* 470 ohm resistor from programmer pin 6 to UPDI pin (see: [Making a cheap UPDI programmer](megaavr/extras/MakeUPDIProgrammer.md) - this resistor is optional on the board if you have one on the programmer, but putting one on both is fine, and protects you in case you ever use one that doesn't have its own resistor).
* Unless the power supply is mounted on the same board, it is highly recommended to put a larger capacitor between power and ground on the board - the appropriate size and type will depend on the loading, voltage, stability of the power supply, and available space; typical values might range from 4.7 to 47uF if the board is not supplying any large loads. If it is, an even larger capacitor should be added - aluminum electrolytic caps are commonly used for this purpose. The absolute minimum "board level decoupling" (as in, located on the same board, in the ) capacitance is 1uF if the board may be connected directly to a power supply backed with large capacitors - with just the decoupling caps on each power pin and something like a laptop USB port as the power source, and very short wires and traces the maximum power supply slew rate could be exceeded, which could damage the chip.
