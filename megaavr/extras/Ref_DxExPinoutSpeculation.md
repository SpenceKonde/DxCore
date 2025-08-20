# Latest speculative PORTMUXes:

A mix of speculation, rationality, and the shadows of light cast by a burning candle`*` leads me to believe that, when the next series of AVR devices is released, if any of these peripherals are present, all mux options for which the essential pins are also present will be available. If a peripheral is present, but no set of pins with the essential pins are, if the associated pins are intrinsic to it's function, such that it is of no use without them (TWI, SPI, USART, time waveform output)

USARTROUTEA: 0b xxx11000 - 3 bits for USART1
USARTROUTEB  0b 55443322 - Only the first USART appears to get the embassador class streatment


##USART0
| USART0  | swap |  TX |  RX | XDIR | XCK |
|---------|------|-----|-----|------|-----|
| DEFAULT |    0 | PA0 | PA1 |  PA2 | PA3 |
| ALT1    |    1 | PA4 | PA5 |  PA6 | PA7 |
| ALT2    |    2 | PA2 | PA3 |   -  |  -  | First introduced in the low pincount parts
| ALT3    |    3 | PD4 | PD5 |  PD6 | PD7 |
| ALT4    |    4 | PC1 | PC2 |  PC3 |  -  |
| ALT5    |    5 | ??? | ??? |   -  |  -  | Apparently unused
| ALT6    |    6 | PF7 | PF6 |   -  |  -  | While the EB does use this
| None    |    7 |  -  |  -  |   -  |  -  |

Since this expansion, the register layout has changed from
USARTROUTEA: 3 3 2 2 1 1 0 0
USARTROUTEB: - - - - 5 5 4 4

to
USARTROUTEA: - - - 1 1 0 0 0
USARTROUTEB: - - - - - - 2 2

No part has been released with more than 3 USARTS to give evidence of their long term plans...

USARTROUTEA: - - 1 1 1 0 0 0
USARTROUTEB: - - 3 3 3 2 2 2
USARTROUTEC: - - 5 5 5 4 4 4
USARTROUTED: - - 7 7 7 6 6 6

USARTROUTEA: - - 1 1 1 0 0 0
USARTROUTEB: 5 5 4 4 3 3 2 2
USARTROUTEC: - - 7 7 7 6 6 6

(assuming there is eventually a plan for an 8-UART chip that would replace the ATmega2560; that's the most portsi can imagine being useful.)



### USART1 mux options
| USART1  | swap |  TX |  RX | XDIR | XCK |
|---------|------|-----|-----|------|-----|
| DEFAULT |    0 | PC0 | PC1 |  PC2 | PC3 |
| ALT1    |    1 | PC4 | PC5 |  PC6 | PC7 |
| ALT2    |    2 | PD6 | PD7 |   -  |  -  | First introduced in the DD, as the sole mapping for USART1.
| None    |    3 |  -  |  -  |  -   |  -  |



### USART2 mux options
| USART2  | swap |  TX |  RX | XDIR | XCK |
|---------|------|-----|-----|------|-----|
| DEFAULT |    0 | PF0 | PF1 |  PF2 | PF3 |
| ALT1    |    1 | PF4 | PF5 |   -  |  -  |
| None    |    3 |  -  |  -  |   -  |  -  |


### USART3 mux options
| USART3  | swap |  TX |  RX | XDIR | XCK |
|---------|------|-----|-----|------|-----|
| DEFAULT |    0 | PB0 | PB1 |  PB2 | PB3 |
| ALT1    |    1 | PB4 | PB5 |  PB6 | PB7 |
| None    |    3 |  -  |  -  |   -  |  -  |

### USART4 mux options
| USART4  | swap |  TX |  RX | XDIR | XCK |
|---------|------|-----|-----|------|-----|
| DEFAULT |    0 | PE0 | PE1 |  PE2 | PE3 |
| ALT1    |    1 | PE4 | PE5 |  PE6 | PE7 |
| None    |    3 |  -  |  -  |   -  |  -  |

### USART5 mux options
| USART5  | swap |  TX |  RX | XDIR | XCK |
|---------|------|-----|-----|------|-----|
| DEFAULT |    0 | PG0 | PG1 |  PG2 | PG3 |
| ALT1    |    1 | PG4 | PG5 |  PG6 | PG7 |
| None    |    3 |  -  |  -  |   -  |  -  |

SPIROUTEA = 0bxxxx1100 AVR DA, AVR DB only
SPIROUTEA - 0bxx111000 all others

### SPI0 mux options
| SPI0    | swap | MOSI | MISO | SCK |  SS |
|---------|------|------|------|-----|-----|
| DEFAULT |    0 |  PA4 |  PA5 | PA6 | PA7 |
| ALT1    |    1 |  PE0 |  PE1 | PE2 | PE3 |
| ALT2    |    2 |  PG4 |  PG5 | PG6 | PG7 |
| ALT3    |    3 |  PA0 |  PA1 | PC0 | PC1 |
| ALT4    |    4 |  PD4 |  PD5 | PD6 | PD7 |
| ALT5    |    5 |  PC0 |  PC1 | PC2 | PC3 |
| ALT6    |    6 |  PC1 |  PC2 | PC3 | PF7 |
| None    |    7 |   -  |   -  |  -  |  -  |

### SPI1 mux options
| SPI1    | swap | MOSI | MISO | SCK |  SS |
|---------|------|------|------|-----|-----|
| DEFAULT |    0 |  PA4 |  PA5 | PA6 | PA7 |
| ALT1    |    1 |  PE0 |  PE1 | PE2 | PE3 |
| ALT2    |    2 |  PG4 |  PG5 | PG6 | PG7 |
| None    |    7 |   -  |   -  |  -  |  -  |


### TWI0 mux options
| Mapping | swap | Master or Slave   | Dual Mode Slave   |
|---------|------|-------------------|-------------------|
| DEFAULT |    0 | SDA/PA2 SCL/PA3   | SDA/PC2 SCL/PC3   |
| ALT1    |    1 | SDA/PA2 SCL/PA3   | SDA/PC6 SCL/PC7   |
| ALT2    |    2 | SDA/PC2 SCL/PC3   | SDA/PC6 SCL/PC7   |
| ALT3    |    3 | SDA/PA0 SCL/PA1   | SDA/PC2 SCL/PC3   |


### TWI1 mux options
| Mapping | swap | Master or Slave   | Dual Mode Slave   |
|---------|------|-------------------|-------------------|
| DEFAULT |    0 | SDA/PF2 SCL/PF3   | SDA/PB2 SCL/PB3   |
| ALT1    |    1 | SDA/PF2 SCL/PF3   | SDA/PB6 SCL/PB7   |
| ALT2    |    2 | SDA/PB2 SCL/PB3   | SDA/PB6 SCL/PB7   |

### TCA

| TCA0    | swap | WO0 | WO1 | WO2 | WO3 | WO4 | WO5 |
|---------|------|-----|-----|-----|-----|-----|-----|
| PORTA   |    0 | PA0 | PA1 | PA2 | PA3 | PA4 | PA5 |
| PORTB   |    1 | PB0 | PB1 | PB2 | PB3 | PB4 | PB5 |
| PORTC   |    2 | PC0 | PC1 | PC2 | PC3 | PC4 | PC5 |
| PORTD   |    3 | PD0 | PD1 | PD2 | PD3 | PD4 | PD5 |
| PORTE   |    4 | PE0 | PE1 | PE2 | PE3 | PE4 | PE5 |
| PORTF   |    5 | PF0 | PF1 | PF2 | PF3 | PF4 | PF5 |
| PORTG   |    6 | PG0 | PG1 | PG2 | PG3 | PG4 | PG5 |

| TCA1    | swap | WO0 | WO1 | WO2 | WO3 | WO4 | WO5 |
|---------|------|-----|-----|-----|-----|-----|-----|
| PORTB   |    0 | PB0 | PB1 | PB2 | PB3 | PB4 | PB5 |
| PORTC   |    1 | PC4 | PC5 | PC6 |  -  |  -  |  -  |
| PORTE   |    2 | PE4 | PE5 | PE6 |  -  |  -  |  -  |
| PORTG   |    3 | PG0 | PG1 | PG2 | PG3 | PG4 | PG5 |
| PORTA   |    4 | PA4 | PA5 | PA6 |  -  |  -  |  -  |
| PORTD   |    5 | PD4 | PD5 | PD6 |  -  |  -  |  -  |
| PORTF   | ~6~  | -   | -   | -   |  -  |  -  |  -  |

Port F likely left out because of PF6 being an input only pin.

## TCB

| TCBn | Default |  Alt  |
|------|---------|-------|
| TCB0 |    PA2  |   PF4 |
| TCB1 |    PA3  |   PF5 |
| TCB2 |    PC0  |   PB4 |
| TCB3 |    PB5  |   PC1 |
| TCB4 |    PG3  |   PC6 | Alt option is broken, I believe on all parts that have TCB4.




## TCD
| TCD0    | WOA | WOB | WOC | WOD |
|---------|-----|-----|-----|-----|
| DEFAULT | PA4 | PA5 | PA6 | PA7 | // Only non-broken option on most DA/DB
| ALT1    | PB4 | PB5 | PB6 | PB7 |
| ALT2    | PF0 | PF1 | PF2 | PF3 |
| ALT3    | PG4 | PG5 | PG6 | PG7 |
| ALT4    | PA4 | PA5 | PD4 | PD5 | // Added in DD.



### EVSYS and CCL mux options
Each port has 1 EVSYS output, which can be set to be located at Px2 (default) or Px7

Prior to the EA, ports had associated event channels that they had to be used with. These take the form of pairs of event channels which can use inputs from either of two ports.
Tiny0/1: See datasheet.
Tiny2: A,B | B,C | C,A
Dx:    A,B | C,D | E,F | G,- | -,- (only 48 and 64 pin DA/DB devices have had more than 6 event channels)

#### CCL

* For tinyAVR:
See pinout chart - Adhoc pin assignments to maximize pin access.

* For Dx, Ex:

Each CCL LUT has one home port, in which pins 0-2 can be directly used as input 0-2. Output is Px3 by default, or Px6 on alternate.

CCLs are paired with the following ports:
```
LUT0 - PORTA
LUT1 - PORTC
LUT2 - PORTD
LUT3 - PORTF - Note, no alternate option: PF6 has no output drivers.
LUT4 - PORTB
LUT5 - PORTG
```
There is no LUT for PORTE.

No parts have yet been released with more than 6 LUTs, or more than 7 ports.






`*` In ancient times, people believed that one skilled in such arts could view the future through a flame. Nostradamus makes frequent references to this method of divination, implying that it was a widely held belief - though according to scholars on his life and myth, in none of his writings does he ever claim to have used this (nor does he claim his prophecies to be original work), or any other method of divination other than astrology (at the time treated as a respectable sciencec), and he was said to be poor at that, having difficulty preparing star charts for clients correctly. (*Considering the track record of astrology, I'm not sure it mattered* "Considering that we're talking about him, it sure didn't make much difference for him.)
