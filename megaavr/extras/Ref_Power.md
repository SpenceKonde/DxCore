# Power consumption figures for modern AVR parts
Harvested from the datasheets. Datasheets are subject to modification through the Datasheet Clarification Process. You can sign up for Microchip Product Change Notifications (PCNs) to get an email when this happens.

All values are taken from the largest flash version (for Dx/Ex) and the 16k versions and 32k versions (which are different) for the tinies.
Smaller flash versions of a part use ever so slightly less power. And yes, the tinys are power hogs compared to the Dx/Ex series.

## tinyAVR 0
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


## AVR DA

| Power Consumption   | min | typ | max | 125C| .  |  Conditions |
| --------------------|-----|-----|-----|-----|----|-------------|

| Power Consumption   | min | typ | max | .  |  Conditions |
| --------------------|-----|-----|-----|----|-------------|

## AVR DB
**WARNING** There is an erratum relevant to power consumption under certain conditions see [the Errata list](Errata.md).

| Power Consumption   | min | typ | max | 125C| .  |  Conditions |
| --------------------|-----|-----|-----|-----|----|-------------|
| IDD<sub>ACTIVE</sub>|  -  | 4.4 |   6 |   6 | mA | OSCHF = 24 MHz
|  -                  |  -  | 1.0 | 1.5 | 1.5 | mA | OSCHF = 4 MHz
|  -                  |  -  | 6.3 |  19 |  42 | μA | OSC32K = 32.768 kHz (note: Unsupported by DxCore)
|  -                  |  -  | 4.2 | 5.5 | 5.5 | mA | EXTCLK = 24 MHz
|  -                  |  -  | 0.85| 1.3 | 1.3 | mA | EXTCLK = 4 MHz
|  -                  |  -  | 9.0 |  -  |   - | μA | XOSC32K = 32.768 kHz, XOSC32KCTRLA.LPMODE = 1 (note: Unsupported by DxCore)
|  -                  |  -  | 7.5 |  -  |   - | μA | XOSC32K = 32.768 kHz, XOSC32KCTRLA.LPMODE = 1 (note: Unsupported by DxCore)
| IDD<sub>IDLE</sub>  |  -  | 2.2 |   3 |   3 | mA | OSCHF = 24 MHz
|  -                  |  -  | 590 | 850 | 850 | μA | OSCHF = 4 MHz
|  -                  |  -  | 3.5 |  19 |  40 | μA | OSC32K = 32.768 kHz (note: Unsupported by DxCore)
|  -                  |  -  | 1.9 | 2.5 | 2.5 | mA | EXTCLK = 24 MHz
|  -                  |  -  | 450 | 800 | 800 | μA | EXTCLK = 4 MHz
|  -                  |  -  | 7.5 |  -  |   - | μA | XOSC32K = 32.768 kHz XOSC32KCTRLA.LPMODE = 0 (note: Unsupported by DxCore)
|  -                  |  -  | 6.0 |  -  |   - | μA | XOSC32K = 32.768 kHz XOSC32KCTRLA.LPMODE = 1 (note: Unsupported by DxCore)
| IDD<sub>BASE</sub>  |  -  |0.65 |  9  |  32 | µA | Power-Down or Standby mode, all peripherals disabled VREGCTRL.PMODE = 0 (AUTO)
|  -                  |  -  | 170 | 250 | 250 | µA | Power-Down mode, all peripherals disabled VREGCTRL.PMODE = 1 (FULL)
|  -                  |  -  | 0.8 | 6.8 |  15 | µA | Power-Down mode VREGCTRL.PMODE = 0 (AUTO) VREGCTRL.HTLLEN = 1
| I<sub>RST</sub>     |  -  | 170 |  -  |   - | μA | RESET pulled to GND

## AVR DD
As a newer part, characterization is less thorough.

| Power Consumption   | min | typ | max | 125C| .  |  Conditions |
| --------------------|-----|-----|-----|-----|----|-------------|
| IDD<sub>ACTIVE</sub>|  -  | 4.7 |  -  |  -  | mA | OSCHF = 24 MHz
|  -                  |  -  | 1.1 |  -  |  -  | mA | OSCHF = 4 MHz
|  -                  |  -  | 4.7 |  -  |  -  | μA | OSC32K = 32.768 kHz
|  -                  |  -  | 4.4 |  -  |  -  | mA | EXTCLK = 24 MHz
|  -                  |  -  | 910 |  -  |  -  | μA | EXTCLK = 4 MHz
|  -                  |  -  | 760 |  -  |  -  | μA | XTALHF = 4 MHz, XOSCHFCTRLA.FRQRANGE = 0x0
|  -                  |  -  | 3.32|  -  |  -  | mA | XTALHF = 24 MHz, XOSCHFCTRLA.FRQRANGE = 0x2
|  -                  |  -  | 8.2 |  -  |  -  | μA | XOSC32K = 32.768 kHz, XOSC32KCTRLA.LPMODE = 0
|  -                  |  -  | 6.3 |  -  |  -  | μA | XOSC32K = 32.768 kHz, XOSC32KCTRLA.LPMODE = 1 IDD<sub>IDLE</sub> Idle power consumption
| IDD<sub>IDLE</sub>  |  -  | 2.3 |  -  |  -  | mA | OSCHF = 24 MHz
|  -                  |  -  | 630 |  -  |  -  | μA | OSCHF = 4 MHz
|  -                  |  -  | 3.1 |  -  |  -  | μA | OSC32K = 32.768 kHz
|  -                  |  -  | 2.0 |  -  |  -  | mA | EXTCLK = 24 MHz
|  -                  |  -  | 480 |  -  |  -  | μA | EXTCLK = 4 MHz
|  -                  |  -  | 393 |  -  |  -  | μA | XTALHF = 4 MHz, XOSCHFCTRLA.FRQRANGE = 0x0
|  -                  |  -  | 1.37|  -  |  -  | mA | XTALHF = 24 MHz, XOSCHFCTRLA.FRQRANGE = 0x2
|  -                  |  -  | 6.8 |  -  |  -  | μA | XOSC32K = 32.768 kHz, XOSC32KCTRLA.LPMODE = 0
|  -                  |  -  | 4.8 |  -  |  -  | μA | XOSC32K = 32.768 kHz, XOSC32KCTRLA.LPMODE = 1
| IDD<sub>BASE</sub>  |  -  | 650 |  -  |  -  | nA | Power-Down or Standby mode, VREGCTRL.PMODE = 0 (AUTO)
|  -                  |  -  | 160 |  -  |  -  | μA | Power-Down mode, VREGCTRL.PMODE = 1 (FULL)
|  -                  |  -  | 0.9 |  -  |  -  | μA | Power-Down mode, VREGCTRL.PMODE = 0 (AUTO), VREGCTRL.HTLLEN = 1
| I<sub>RST</sub>     |  -  | 170 |  -  |  -  | μA | RESET line pulled low


## AVR EA
As a new product, not all of these have been characterized ~rigorously~.

At 3.0V unless otherwise noted.

| Power Consumption  | min | typ | max | 125C| .  |  Conditions |
| --------------------|-----|-----|-----|-----|----|-------------|
| IDD<sub>active</sub>|  -  |  12 |  -  |  -  | mA | OSCHF = 20 MHz, VDD = 5V |
|  -                  |  -  | 3.7 |  -  |  -  | mA | OSCHF = 10 MHz (OSCHF/2) |
|  -                  |  -  | 2.2 |  -  |  -  | mA | OSCHF = 5 MHz (OSCHF/4) |
|  -                  |  -  |  -  |  -  |  -  | uA | OSC32K = 32.768 kHz (very informative!) |
|  -                  |  -  |  10 |  -  |  -  | µA | XOSC32K = 32.768 kHz |
| IDD<sub>idle</sub>  |  -  | 3.5 |  -  |  -  | mA | OSCHF = 20 MHz, VDD = 5V |
|  -                  |  -  | 1.5 |  -  |  -  | mA | OSCHF = 10 MHz (OSCHF/2) |
|  -                  |  -  | 1.0 |  -  |  -  | mA | OSCHF = 5 MHz (OSCHF/4) |
|  -                  |  -  |  -  |  -  |  -  | uA | OSC32K = 32.768 kHz (very informative!)|
|  -                  |  -  | 3.3 |  -  |  -  | μA | XOSC32K = 32.768 kHz |
| IDD<sub>base</sub>  |  -  |0.08 |  -  |  -  | μA | Power-Down or Standby |

## Peripheral power consumption
At 3.0V unless otherwise noted, largest flash size

## AVR DA
| Power Consumption   | min | typ | max | .  |  Conditions |
| --------------------|-----|-----|-----|----|-------------|

## AVR DB
| Power Consumption   | min | typ | max | .  |  Conditions |
| --------------------|-----|-----|-----|----|-------------|
| IDD<sub>WDT</sub>   |  -  | 0.5 | 1.5 | μA | With 32.768 kHz Internal Oscillator
| IDD<sub>MVIO</sub>  |  -  | 0.5 |  -  | μA | .
| IDD<sub>VREF</sub>  |  -  | 175 | 320 | μA | ADC0REF enabled, VREF = 2.048V
|  -                  |  -  |  71 |  92 | μA | ACREF enabled, VREF = 2.048V
|  -                  |  -  |  40 |  62 | μA | DACREF enabled, VREF = 2.048V
| IDD<sub>BOD</sub>   |  -  |  17 |  27 | μA | Brown-out Detect (BOD) continuous
|  -                  |  -  | 1.6 |  12 | μA | Brown-out Detect (BOD) sampling @128, Hz, including IDD_OSC32K
|  -                  |  -  |0.95 |  12 | μA | Brown-out Detect (BOD) sampling @32, Hz, including IDD_OSC32K
| IDD<sub>TCA</sub>   |  -  |   6 |  -  | μA
| IDD<sub>TCB</sub>   |  -  | 3.6 |  -  | μA | CLK_PER = HFOSC/4 = 1 MHz
| IDD<sub>TCD</sub>   |  -  | 4.5 |  -  | μA
| IDD<sub>RTC</sub>   |  -  | 0.7 |  2  | μA | CLK_RTC = 32.768 kHz Internal, Oscillator
|  -                  |  -  | 3.9 |  -  | μA | RTC running at 1.024 kHz from, XOSC32K, XOSC32KCTRLA.LPMODE = 0
|  -                  |  -  | 2.1 |  -  | μA | RTC running at 1.024 kHz from, XOSC32K, XOSC32KCTRLA.LPMODE = 1
| IDD<sub>OSCHF</sub> |  -  | 160 |  -  | µA Internal Oscillator running at 4 MHz
| IDD<sub>XOSCHF</sub>|  -  | 360 |  -  | μA | 24 MHz XTAL, CL = 15 pF
| IDD<sub>OSC32K</sub>|  -  | 0.3 | 1.5 | μA | .
| IDD<sub>XOSC32K</sub>| -  | 2.5 |  -  | μA | XOSC32KCTRLA.LPMODE = 0, CL = 9 pF
|  -                  |  -  | 0.5 |  -  | μA | XOSC32KCTRLA.LPMODE = 1, CL = 9 pF
| IDD<sub>AC</sub>    |  -  |  70 | 130 | μA | CTRLA.POWER = 0x0
|  -                  |  -  |  17 |  35 | μA | CTRLA.POWER = 0x1
|  -                  |  -  |  12 |  33 | μA | CTRLA.POWER = 0x2
| IDD<sub>OPAMP</sub> |  -  | 1.0 |  -  | mA | One OPAMP in voltage follower, VCM = VDD/2, IRSEL = 0
|  -                  |  -  |0.75 |  -  | mA | One OPAMP in voltage follower, VCM = VDD/2, IRSEL = 1
| IDD<sub>DAC</sub>   |  -  | 120 | 180 | μA | DAC + DACOUT, VDACREF = VDD
|  -                  |  -  |   8 |  34 | μA | DAC, VDACREF = VDD/2
| IDD<sub>USART</sub> |  -  | 8.2 |  -  | μA | USART Enabled @9600 Baud
| IDD<sub>SPI</sub>   |  -  |   4 |  -  | μA | SPI Master @100 kHz
| IDD<sub>TWI</sub>   |  -  |   8 |  -  | μA | TWI Master @100 kHz
|  -                  |  -  |   6 |  -  | μA | TWI Slave @100 kHz
| IDD<sub>NVM_ER</sub>|  -  | 5.1 |  -  | mA | .
| IDD<sub>NVM_WR</sub>|  -  | 6.0 |  -  | mA | .
| IDD<sub>ZCD</sub>   |  -  |  10 |  -  | μA | Excluding sink/source currents

## AVR DD
| Power Consumption   | min | typ | max | .  |  Conditions |
| --------------------|-----|-----|-----|----|-------------|
| IDD<sub>WDT</sub>   |  -  | 600 |  -  | nA | 32.768 kHz internal oscillator
| IDD<sub>MVIO</sub>  |  -  | 600 |  -  | nA | Dual supply configuration
| IDD<sub>VREF</sub>  |  -  | 160 |  -  | μA | ADC0REF enabled, VREF = 2.048V
|  -                  |  -  |  71 |  -  | μA | ACREF enabled, VREF = 2.048V
|  -                  |  -  |  40 |  -  | μA | DACREF enabled, VREF = 2.048V
| IDD<sub>BOD</sub>   |  -  |  17 |  -  | μA | Brown-out Detect (BOD) continuous
|  -                  |  -  | 800 |  -  | nA | Brown-out Detect (BOD) sampling @128 Hz, including IDD<sub>OSC32K</sub>
|  -                  |  -  | 600 |  -  | nA | Brown-out Detect (BOD) sampling @32 Hz, including IDD<sub>OSC32K</sub>
| IDD<sub>TCA</sub>   |  -  | 6.3 |  -  | μA | HFOSC = 4 MHz, CLK_PER = HFOSC/4 = 1 MHz
| IDD<sub>TCB</sub>   |  -  | 3.7 |  -  | μA | HFOSC = 4 MHz, CLK_PER = HFOSC/4 = 1 MHz
| IDD<sub>TCD</sub>   |  -  | 5.3 |  -  | μA | HFOSC = 4 MHz, CLK_PER = HFOSC/4 = 1 MHz
| IDD<sub>RTC</sub>   |  -  | 820 |  -  | nA | CLK_RTC = 32.768 kHz Internal Oscillator
| IDD<sub>OSCHF</sub> |  -  | 150 |  -  | μA | OSCHF at 4 MHz
| IDD<sub>XOSCHF</sub>|  -  | 350 |  -  | μA | 20 MHz XTAL, CL = 15 pF
| IDD<sub>OSC32K</sub>|  -  | 510 |  -  | nA | .
| IDD<sub>XOSC32K</sub>| -  | 2.4 |  -  | μA | XOSC32KCTRLA.LPMODE = 0, CL = 9 pF
|  -                  |  -  | 580 |  -  | nA | XOSC32KCTRLA.LPMODE = 1, CL = 9 pF
| IDD<sub>ADC</sub>   |  -  |  67 |  -  | nA | ADC - Nonconverting
|  -                  |  -  | 680 |  -  | μA | ADC @60 ksps(2)
|  -                  |  -  | 700 |  -  | μA | ADC @120 ksps(2)
| IDD<sub>AC</sub>    |  -  |  70 |  -  | μA | CTRLA.POWER = 0x0
|  -                  |  -  |  17 |  -  | μA | CTRLA.POWER = 0x1
|  -                  |  -  |  12 |  -  | μA | CTRLA.POWER = 0x2
| IDD<sub>DAC</sub>   |  -  | 120 |  -  | μA | DAC + DACOUT, VDACREF = VDD/2
|  -                  |  -  | 8.6 |  -  | μA | DAC, VDACREF = VDD/2
| IDD<sub>USART</sub> |  -  |  36 |  -  | μA | USART Enabled @9600 Baud
| IDD<sub>SPI</sub>   |  -  | 2.1 |  -  | μA | SPI Master @100 kHz
| IDD<sub>TWI</sub>   |  -  |  24 |  -  | μA | TWI Master @100 kHz
|  -                  |  -  |  17 |  -  | μA | TWI Slave @100 kHz
| IDD<sub>NVM_ER</sub>|  -  | 1.5 |  -  | mA | Flash Programming Erase |
| IDD<sub>NVM_WR</sub>|  -  |   3 |  -  | mA | Flash Programming Write |


## AVR EA
| As a new product, not all of these have been characterized ~rigorously~.

| Power Consumption  | min | typ | max | 125C| .  |  Conditions |
| --------------------|-----|-----|-----|-----|----|-------------|
| IDD<sub>WDT</sub>   |  -  | 270 |  -  |  -  | nA | 32.768 kHz internal oscillator |
| IDD<sub>VREF</sub>  |  -  |  -  |  -  |  -  | µA | (Empty row. Very informative!) |
|  -                  |  -  |  -  |  -  |  -  | µA | (Empty row. Very informative!) |
|  -                  |  -  |  -  |  -  |  -  | µA | (Empty row. Very informative!) |
| IDD<sub>BOD</sub>   |  -  |  14 |  -  |  -  | μA | Brown-out Detect (BOD) continuous |
|  -                  |  -  |0.60 |  -  |  -  | μA | Brown-out Detect (BOD) sampling @ 128 Hz, including IDD<sub>OSC</sub>32K |
|  -                  |  -  |0.42 |  -  |  -  | μA | Brown-out Detect (BOD) sampling @ 32 Hz, including IDD<sub>OSC</sub>32K |
| IDD<sub>TCA</sub>   |  -  |  38 |  -  |  -  | μA | Device in Idle sleep mode |
| IDD<sub>TCB</sub>   |  -  |  19 |  -  |  -  | μA | Device in Idle sleep mode |
| IDD<sub>RTC</sub>   |  -  | 0.6 | -   |  -  | uA | 32.768 kHz internal oscillator (OSC32K) |
| IDD<sub>OSC32k</sub>|  -  | 270 |  -  |  -  | nA | . |
| IDD<sub>XOSC32k</sub>| -  |  -  |  -  |  -  | µA | (w/out conditions for the speed specified of course this isn't known!) |
| IDD<sub>OSCHF</sub> |  -  |  -  |  -  |  -  | µA | (w/out conditions for the speed specified of course this isn't known!) |
| IDD<sub>XOSCHF</sub>|  -  |  -  |  -  |  -  | µA | (w/out conditions for the speed specified of course this isn't known!) |
| IDD<sub>ADC</sub>   |  -  | 270 |  -  |  -  | μA | CLK_ADC = 1 MHz, device in Idle sleep mode |
| IDD<sub>AC</sub>    |  -  | 160 |  -  |  -  | μA | CTRLA.POWER = 0x0, device in Idle sleep mode |
|  -                  |  -  |  -  |  -  |  -  | μA | CTRLA.POWER = 0x1, device in Idle sleep mode |
|  -                  |  -  | 110 |  -  |  -  | μA | CTRLA.POWER = 0x2, device in Idle sleep mode |
|  -                  |  -  |  -  |  -  |  -  | μA | CTRLA.POWER = 0x3, (This line is not present in datasheet) |
| IDD<sub>DAC</sub>   |  -  | 104 |  -  |  -  | μA | VDACREF = 1.024 V, device in Idle sleep mode |
|  -                  |  -  | 125 |  -  |  -  | μA | VDACREF = 4.096 V, device in Idle sleep mode |
|  -                  |  -  |  98 |  -  |  -  | µA | VDACREF = VDD, device in Idle sleep mode |
| IDD<sub>USART</sub> |  -  |  -  |  -  |  -  | µA | USART Enabled @9600 Baud  (Very informative)|
| IDD<sub>SPI</sub>   |  -  |   7 |  -  |  -  | μA | SPI Master @100 kHz, device in Idle sleep mode |
| IDD<sub>TWI</sub>   |  -  |  27 |  -  |  -  | μA | TWI Master @100 kHz, device in Idle sleep mode |
|  -                  |  -  |  21 |  -  |  -  | μA | TWI Slave @100 kHz, device in Idle sleep mode |
| IDD<sub>NVM_ER</sub>|  -  | 1.5 |  -  |  -  | mA | Flash Programming Erase |
| IDD<sub>NVM_WR</sub>|  -  | 3.0 |  -  |  -  | mA | Flash Programming Write |
