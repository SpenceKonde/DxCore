# Azduino MAX38903 library

We sell an AVR128DB64 board with the MAX38903 regulator on board to provide VDDIO2 at a series of selectable voltages. While far less flexible than - say - using the output of the on-chip opamp driven by the DAC, this is a real LDO regulator that can supply up to an amp of current. To phrase it another way, you can power non-trivial devices from it without issues. The way this clever regulator works to get as many options as it does from just 2 select pins is by using the floating pin as a third state - hence yieldng 9 option instead of a mere 4. Dropout can be as low as 100mV while supplying an amp to the load. The two select lines and the enable pin can be manipulated to rapidly switch the output voltage in a process taking just a couple of microseconds. This is one hell of a regulator. The one thing it doesn't do is support input voltages above 5.5. For more information on how to make these connections with the on-board solder bridge jumpers on the Azduino AVR128DB64, see our power supply guide in the manually.

Typically it will be powered either from external 5V directly, or it could run from a 5v output from the main regulator. If using typical 5.2-5.3V USB power, you'll be able to get the full range of voltages out of this.

Note that the AVR and the regulator must be running at approximately the same voltage; that is why we do not supply such configurations.

## Supported Voltages
| Voltage | Name    | Notes                                          |
|---------|---------|------------------------------------------------|
| 5.0     | REG_5V0 | Requires >= 5.1v Vin                           |
| 4.0     | REG_4V0 |                                                |
| 3.3     | REG_3V3 |                                                |
| 3.1     | REG_3V1 |                                                |
| 3.0     | REG_3V0 |                                                |
| 2.5     | REG_2V5 |                                                |
| 1.8     | REG_1V8 |                                                |
| 1.5     | REG_1V5 | Will not work as VDDIO2 - MVIO requires 1.65v. |
| 1.2     | REG_1V2 | Will not work as VDDIO2 - MVIO requires 1.65v. |
| N/A     | REG_OFF | Turn off the regulator output.                 |

## API
`setMAX38903Voltage()` - Call with any of the REG_xVx constants in the table above to set the voltage.

And that's it.

## Pins used
I attempted to pick the least useful pins on the chip as the control lines - and certainly to avoid any uniquely useful pins.
* **PE6** is used for the enable pin; We do not support PWM on that pin, Port E doesn't get a CCL LUT, so it's not using up a precious LUT output option, and it has no other particularly notable alternate functions. It is externally pulled down with a 100k resistor to ensure that the regulator does not turn on before the AVR is able to tell it what voltage to run at.
* **PG6** and **PG7** are used for the select inputs. Note that once the voltage is set, these pins can be used for other purposes. Just dont leave an alternate function on them when you call `setMAX38903Voltage()` (this library does not attempt to clear out altermate functions - largely because even just turnin off PWM would make the process of changing the voltage take literally about 25 times longer). With TCD0 PWM broken except on PORTA, the only alternate functions these pins can have are as alternate outputs for EVSYS and a CCL block.
