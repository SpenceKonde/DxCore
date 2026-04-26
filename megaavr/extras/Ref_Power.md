# Power consumption figures for modern AVR parts
Harvested from the datasheets. Datasheets are subject to modification through the Datasheet Clarification Process. You can sign up for Microchip Product Change Notifications (PCNs) to get an email when this happens.

All values are taken from the largest flash version (for Dx/Ex) and the 16k versions and 32k versions (which are different) for the tinies.
Smaller flash versions of a part use ever so slightly less power. And yes, the tinys are power hogs compared to the Dx/Ex series.








## tinyAVR

### tinyAVR 0
Based on 1607 figures - Notice that tiny 1 and 2 have higher power consumption!

| Power Consumption   | typ | max | 125C| .  |  Conditions |
| --------------------|-----|-----|-----|----|-------------|
| IDD<sub>ACTIVE</sub>|  -  | 7.8 |  -  | mA | VDD = 5V, CLK_CPU = 20 MHz
|  -                 |  -  | 4.3 |  -  | mA | VDD = 5V, CLK_CPU = 10 MHz (div2)
|  -                 |  -  | 2.4 |  -  | mA | VDD = 3V, CLK_CPU = 10 MHz (div2)
|  -                 |  -  | 2.4 |  -  | mA | VDD = 5V, CLK_CPU = 5 MHz (div4)
|  -                 |  -  | 1.4 |  -  | mA | VDD = 3V, CLK_CPU = 5 MHz (div4)
|  -                 |  -  | 0.9 |  -  | mA | VDD = 2V, CLK_CPU = 5 MHz (div4)
|  -                 |  -  |  16 |  -  | µA | VDD = 5V, CLK_CPU = 32 KHz (OSCULP32K)
|  -                 |  -  | 9.2 |  -  | µA | VDD = 3V, CLK_CPU = 32 KHz (OSCULP32K)
|  -                 |  -  | 6.3 |  -  | µA | VDD = 2V, CLK_CPU = 32 KHz (OSCULP32K)
| IDD<sub>IDLE</sub>  |  -  | 2.1 |  -  | mA | VDD = 5V, CLK_CPU = 20 MHz
|  -                 |  -  | 1.3 |  -  | mA | VDD = 5V, CLK_CPU = 10 MHz (div2)
|  -                 |  -  | 0.7 |  -  | mA | VDD = 3V, CLK_CPU = 10 MHz (div2)
|  -                 |  -  | 0.9 |  -  | mA | VDD = 5V, CLK_CPU = 5 MHz (div4)
|  -                 |  -  | 0.5 |  -  | mA | VDD = 3V, CLK_CPU = 5 MHz (div4)
|  -                 |  -  | 0.3 |  -  | mA | VDD = 2V, CLK_CPU = 5 MHz (div4)
|  -                 |  -  | 4.3 |  -  | µA | VDD = 5V, CLK_CPU = 32 KHz (OSCULP32K)
|  -                 |  -  | 2.1 |  -  | µA | VDD = 3V, CLK_CPU = 32 KHz (OSCULP32K)
|  -                 |  -  | 1.4 |  -  | µA | VDD = 2V, CLK_CPU = 32 KHz (OSCULP32K)
| IDD<sub>STANDBY</sub>|0.69|  -  |  -  | µA | RTC running at 1.024 kHz from external XOSC32K (CL = 7.5 pF)
|  -                 |0.71 | 6.0 | 8.0 | µA | RTC running at 1.024 kHz from internal OSCULP32K
| IDD<sub>PWRDWN/STBY</sub>|0.1|5.0|7.0 | µA | All peripherals stopped
| I<sub>RST</sub>     | 100 |  -  |   - | μA | RESET pulled to GND


## tinyAVR 1

### 16k parts
| Power Consumption   | typ | max | .  |  Conditions |
| --------------------|-----|-----|----|-------------|
| IDD<sub>ACTIVE</sub>|10.2 |  -  | mA | CLK_CPU = 20 MHz (OSC20M) VDD = 5V |
|  -                  | 5.5 |  -  | mA | CLK_CPU = 10 MHz (OSC20M div2) VDD = 5V |
|  -                  | 3.1 |  -  | mA | CLK_CPU = 10 MHz (OSC20M div2) VDD = 3V |
|  -                  | 3.2 |  -  | mA | CLK_CPU = 5 MHz (OSC20M div4) VDD = 5V |
|  -                  | 1.8 |  -  | mA | CLK_CPU = 5 MHz (OSC20M div4) VDD = 3V |
|  -                  | 1.2 |  -  | mA | CLK_CPU = 5 MHz (OSC20M div4) VDD = 2V |
|  -                  |13.5 |  -  | µA | CLK_CPU = 32.768 kHz (OSCULP32K) VDD = 5V |
|  -                  | 7.5 |  -  | µA | CLK_CPU = 32.768 kHz (OSCULP32K) VDD = 3V |
|  -                  | 5.0 |  -  | µA | CLK_CPU = 32.768 kHz (OSCULP32K) VDD = 2V |
| IDD<sub>IDLE</sub>  | 4.3 | 6.3 | mA | CLK_CPU = 20 MHz (OSC20M), VDD = 5V |
|  -                  | 2.2 | 3.1 | mA | CLK_CPU = 10 MHz (OSC20M div2), VDD = 5V |
|  -                  | 1.1 | 1.9 | mA | CLK_CPU = 10 MHz (OSC20M div2), VDD = 3V |
|  -                  | 1.1 | 1.6 | mA | CLK_CPU = 5 MHz (OSC20M div4), VDD = 5V |
|  -                  | 0.6 | 0.9 | mA | CLK_CPU = 5 MHz (OSC20M div4), VDD = 3V |
|  -                  | 0.4 |  -  | mA | CLK_CPU = 5 MHz (OSC20M div4), VDD = 2V |
|  -                  | 8.2 |  20 | µA | CLK_CPU = 32.768 kHz (OSCULP32K) VDD = 5V |
|  -                  | 4.2 |  15 | µA | CLK_CPU = 32.768 kHz (OSCULP32K) VDD = 3V |
|  -                  | 2.6 |  -  | µA | CLK_CPU = 32.768 kHz (OSCULP32K) VDD = 2V |

| Power Consumption   | typ | 25C | 85C | 125C| .  |  Conditions |
| --------------------|-----|-----|-----|-----|----|-------------|
| IDD<sub>STBY</sub>  |0.69 |  -  |  -  |  -  | µA | RTC running at 1.024 kHz from external XOSC32K (CL = 7.5 pF) |
|  -                  |0.71 | 3.0 | 6.0 | 8.0 | µA | RTC running at 1.024 kHz from internal OSCULP32K
| IDD<sub>PWRDWN/STBY</sub>|0.1|2.0|5.0 | 7.0 | µA | All peripherals stopped
| I<sub>RST</sub>     | 100 |  -  |  -  |  -  | μA | RESET pulled to GND |

### 32k parts
| Power Consumption   | typ | max | .  |  Conditions |
| --------------------|-----|-----|----|-------------|
| IDD<sub>ACTIVE</sub>|  10 |  -  | mA | CLK_CPU = 20 MHz (OSC20M) VDD = 5V |
|  -                  | 5.3 |  -  | mA | CLK_CPU = 10 MHz (OSC20M div2) VDD = 5V |
|  -                  | 3.0 |  -  | mA | CLK_CPU = 10 MHz (OSC20M div2) VDD = 3V |
|  -                  | 3.0 |  -  | mA | CLK_CPU = 5 MHz (OSC20M div4) VDD = 5V |
|  -                  | 1.7 |  -  | mA | CLK_CPU = 5 MHz (OSC20M div4) VDD = 3V |
|  -                  | 1.1 |  -  | mA | CLK_CPU = 5 MHz (OSC20M div4) VDD = 2V |
|  -                  |19.2 |  -  | µA | CLK_CPU = 32.768 kHz (OSCULP32K) VDD = 5V |
|  -                  |10.9 |  -  | µA | CLK_CPU = 32.768 kHz (OSCULP32K) VDD = 3V |
|  -                  | 7.4 |  -  | µA | CLK_CPU = 32.768 kHz (OSCULP32K) VDD = 2V |
| IDD<sub>IDLE</sub>  | 2.9 | 6.3 | mA | CLK_CPU = 20 MHz (OSC20M), VDD = 5V |
|  -                  | 1.5 | 3.1 | mA | CLK_CPU = 10 MHz (OSC20M div2), VDD = 5V |
|  -                  |0.85 | 1.9 | mA | CLK_CPU = 10 MHz (OSC20M div2), VDD = 3V |
|  -                  |0.75 | 1.6 | mA | CLK_CPU = 5 MHz (OSC20M div4), VDD = 5V |
|  -                  | 0.4 | 1.2 | mA | CLK_CPU = 5 MHz (OSC20M div4), VDD = 3V |
|  -                  | 0.3 |  -  | mA | CLK_CPU = 5 MHz (OSC20M div4), VDD = 2V |
|  -                  | 5.7 |  20 | µA | CLK_CPU = 32.768 kHz (OSCULP32K) VDD = 5V |
|  -                  | 2.9 |  15 | µA | CLK_CPU = 32.768 kHz (OSCULP32K) VDD = 3V |
|  -                  | 1.9 |  -  | µA | CLK_CPU = 32.768 kHz (OSCULP32K) VDD = 2V |

| Power Consumption   | typ | 25C | 85C | 125C| .  |  Conditions |
| --------------------|-----|-----|-----|-----|----|-------------|
| IDD<sub>STBY</sub>  |0.69 |  -  |  -  |  -  | µA | RTC running at 1.024 kHz from external XOSC32K (CL = 7.5 pF) |
|  -                  |0.71 | 3.0 | 6.0 | 8.0 | µA | RTC running at 1.024 kHz from internal OSCULP32K
| IDD<sub>PWRDWN/STBY</sub>|0.1|2.0|5.0 | 7.0 | µA | All peripherals stopped
| I<sub>RST</sub>     | 100 |  -  |  -  |  -  | μA | RESET pulled to GND |


## tinyAVR 2

### 16k parts
| Power Consumption   | min | typ | max | .  |  Conditions |
| --------------------|-----|-----|-----|----|-------------|
| IDD<sub>ACTIVE</sub>|  -  |10.2 |  -  | mA |CLK_CPU = 20 MHz (OSC20M) VDD = 5V |
|  -                  |  -  | 5.5 |  -  | mA |CLK_CPU = 10 MHz (OSC20M div2) VDD = 5V |
|  -                  |  -  | 3.1 |  -  | mA |CLK_CPU = 10 MHz (OSC20M div2) VDD = 3V |
|  -                  |  -  | 3.2 |  -  | mA |CLK_CPU = 5 MHz (OSC20M div4) VDD = 5V |
|  -                  |  -  | 1.8 |  -  | mA |CLK_CPU = 5 MHz (OSC20M div4) VDD = 3V |
|  -                  |  -  | 1.2 |  -  | mA |CLK_CPU = 5 MHz (OSC20M div4) VDD = 2V |
|  -                  |  -  |13.5 |  -  | µA |CLK_CPU = 32.768 kHz (OSCULP32K) VDD = 5V |
|  -                  |  -  | 7.5 |  -  | µA |CLK_CPU = 32.768 kHz (OSCULP32K) VDD = 3V |
|  -                  |  -  | 5.0 |  -  | µA |CLK_CPU = 32.768 kHz (OSCULP32K) VDD = 2V |
| IDD<sub>IDLE</sub>  |  -  | 4.3 |  -  | mA |CLK_CPU = 20 MHz (OSC20M), VDD = 5V |
|  -                  |  -  | 2.5 |  -  | mA |CLK_CPU = 10 MHz (OSC20M div2), VDD = 5V |
|  -                  |  -  | 1.4 |  -  | mA |CLK_CPU = 10 MHz (OSC20M div2), VDD = 3V |
|  -                  |  -  | 1.6 |  -  | mA |CLK_CPU = 5 MHz (OSC20M div4), VDD = 5V |
|  -                  |  -  | 0.9 |  -  | mA |CLK_CPU = 5 MHz (OSC20M div4), VDD = 3V |
|  -                  |  -  | 0.6 |  -  | mA |CLK_CPU = 5 MHz (OSC20M div4), VDD = 2V |
|  -                  |  -  | 8.2 |  -  | µA |CLK_CPU = 32.768 kHz (OSCULP32K) VDD = 5V |
|  -                  |  -  | 4.2 |  -  | µA |CLK_CPU = 32.768 kHz (OSCULP32K) VDD = 3V |
|  -                  |  -  | 2.6 |  -  | µA |CLK_CPU = 32.768 kHz (OSCULP32K) VDD = 2V |
| IDD<sub>STBY</sub>  |  -  |0.69 |  -  | µA |RTC running at 1.024 kHz from external XOSC32K (CL = 7.5 pF) |
|  -                  |  -  | 1.0 |  -  | µA |RTC running at 1.024 kHz from internal OSCULP32K, T = 25°C |
|  -                  |  -  |  -  |  -  | µA |RTC running at 1.024 kHz from internal OSCULP32K, T = 85°C |
|  -                  |  -  |  -  |  -  | µA |RTC running at 1.024 kHz from internal OSCULP32K, T = 125°C |
| IDD<sub>PWRDWN/STBY</sub>|-|0.1 |  -  | µA |RTC running at 1.024 kHz from internal OSCULP32K, T = 25°C |
|  -                  |  -  |  -  |  -  | µA |RTC running at 1.024 kHz from internal OSCULP32K, T = 85°C |
|  -                  |  -  |  -  |  -  | µA |RTC running at 1.024 kHz from internal OSCULP32K, T = 125°C |
| I<sub>RST</sub>     | 100 |  -  |   - | μA | RESET pulled to GND

### 16k parts
| Power Consumption   | min | typ | max | .  |  Conditions |
| --------------------|-----|-----|-----|----|-------------|
| IDD<sub>ACTIVE</sub>|  -  |10.2 |  -  | mA |CLK_CPU = 20 MHz (OSC20M) VDD = 5V |
|  -                  |  -  | 5.5 |  -  | mA |CLK_CPU = 10 MHz (OSC20M div2) VDD = 5V |
|  -                  |  -  | 3.1 |  -  | mA |CLK_CPU = 10 MHz (OSC20M div2) VDD = 3V |
|  -                  |  -  | 3.2 |  -  | mA |CLK_CPU = 5 MHz (OSC20M div4) VDD = 5V |
|  -                  |  -  | 1.8 |  -  | mA |CLK_CPU = 5 MHz (OSC20M div4) VDD = 3V |
|  -                  |  -  | 1.2 |  -  | mA |CLK_CPU = 5 MHz (OSC20M div4) VDD = 2V |
|  -                  |  -  |13.5 |  -  | µA |CLK_CPU = 32.768 kHz (OSCULP32K) VDD = 5V |
|  -                  |  -  | 7.5 |  -  | µA |CLK_CPU = 32.768 kHz (OSCULP32K) VDD = 3V |
|  -                  |  -  | 5.0 |  -  | µA |CLK_CPU = 32.768 kHz (OSCULP32K) VDD = 2V |
| IDD<sub>IDLE</sub>  |  -  | 4.3 |  -  | mA |CLK_CPU = 20 MHz (OSC20M), VDD = 5V |
|  -                  |  -  | 2.5 |  -  | mA |CLK_CPU = 10 MHz (OSC20M div2), VDD = 5V |
|  -                  |  -  | 1.4 |  -  | mA |CLK_CPU = 10 MHz (OSC20M div2), VDD = 3V |
|  -                  |  -  | 1.6 |  -  | mA |CLK_CPU = 5 MHz (OSC20M div4), VDD = 5V |
|  -                  |  -  | 0.9 |  -  | mA |CLK_CPU = 5 MHz (OSC20M div4), VDD = 3V |
|  -                  |  -  | 0.6 |  -  | mA |CLK_CPU = 5 MHz (OSC20M div4), VDD = 2V |
|  -                  |  -  | 8.2 |  -  | µA |CLK_CPU = 32.768 kHz (OSCULP32K) VDD = 5V |
|  -                  |  -  | 4.2 |  -  | µA |CLK_CPU = 32.768 kHz (OSCULP32K) VDD = 3V |
|  -                  |  -  | 2.6 |  -  | µA |CLK_CPU = 32.768 kHz (OSCULP32K) VDD = 2V |
| IDD<sub>STBY</sub>  |  -  |0.69 |  -  | µA |RTC running at 1.024 kHz from external XOSC32K (CL = 7.5 pF) |
|  -                  |  -  | 1.0 |  -  | µA |RTC running at 1.024 kHz from internal OSCULP32K, T = 25°C |
|  -                  |  -  |  -  |  -  | µA |RTC running at 1.024 kHz from internal OSCULP32K, T = 85°C |
|  -                  |  -  |  -  |  -  | µA |RTC running at 1.024 kHz from internal OSCULP32K, T = 125°C |
| IDD<sub>PWRDWN/STBY</sub>|-|0.1 |  -  | µA |RTC running at 1.024 kHz from internal OSCULP32K, T = 25°C |
|  -                  |  -  |  -  |  -  | µA |RTC running at 1.024 kHz from internal OSCULP32K, T = 85°C |
|  -                  |  -  |  -  |  -  | µA |RTC running at 1.024 kHz from internal OSCULP32K, T = 125°C |
| I<sub>RST</sub>     | 100 |  -  |   - | μA | RESET pulled to GND


