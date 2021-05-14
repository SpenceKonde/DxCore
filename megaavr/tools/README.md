# Contents
The tools directory contains tooling written in the python language for uploading code. Currently, it is all related to the serial-updi (UPDI over a simple serial adapter)


# serial-updi
Arduino <--> pymcuprog Bridge
By Spence Konde and Quentin Bolsee
An Arduino IDE friendly launch script for pymcuprog, including improvements.


### Hardware for Serial UPDI

```
USB Serial Adapter
With internal 1-2k resistor on TX
This is the case in 90% of USB serial adapters.



This is the ideal case - the resistor is alreadt built into serial adaopter (less work) and it will just work with most configurtatioms. while still being safe.

--------------------                                 To Target device. If you make a 3-pin connector, use this pinout
                DTR|                                 ------------     so that it accidentally reversing it is safe.
    internal    Rx |--------------,------------------| UPDI           Switch Gnd and UPDI, if  connected backwards, you would apply reverse polarity power and burn it out
  Tx---/\/\/\---Tx |-------|<|---'          .--------| Gnd            Switch Gnd and Vcc, if connected backwards, UPDI would be hard-grounded while ground was connected
    resistor    Vcc|---------------------------------| Vcc            to positive voltage of idle serial line, hence, it would be below ground, e
  typ 1-2k      CTS|                     .`          ------------     With this scheme, if backwards, UPDI (idle high) would be connected to Vcc, and Vcc to UPDI. Either:
                Gnd|--------------------'                                 * Device is externally powered at similar voltage (if not similar, you'd probably be in trouble
--------------------                                                          no matter what). Target UPDI and adapter RX both held high, that's their idle state, no worries.
                                                                          * Device was to be powered by programmer, and is not a tinyAVR - in this case, the leakage current
                                                                              from RX does little to pouwert the part, and there is some current injection the UPDI pin
                                                                              but probably not very much, and modern AVRs are designed to withstand 15-20mA of that!
                                                                          * As above, but it's a tinyAVR = Reset pin does not have normal protection diodes because of the
                                                                              HV programming thing. No current flows, nothing happens.

                                                  Never design a cable pinout such that it can physically be plugged in backwards but will not survive that electronicallty


Also works great, convenient if still using jtag2updi without resistor built into it. Resistorss should sum to less than 4.7k, preferalby much less

--------------------                                   To Target device.
                DTR|                                 ------------         __________
    internal    Rx |--------------,------------------| UPDI----\/\/\/----|UPDI      |
  Tx---/\/\/\---Tx |-------|<|---'          .--------| Gnd       <2k     |          |
    resistor    Vcc|---------------------------------| Vcc               |__________|
  typ 1-2k      CTS|                     .`          ------------      series resistor between header and chip UPDI pin on target PCB; I use 470 ohm resistors,
                Gnd|--------------------'                                so I can use a programmer that doesn't have a resistor built in.
--------------------

Awkward wiring to work with
--------------------                                   To Target device.
                DTR|               ,----------------------------------.   __________
    internal    Rx |--------------/                  | UPDI----\/\/\/--*-|UPDI      |
  Tx---/\/\/\---Tx |-------|<|---'          .--------| Gnd       4.7k    |          |
    resistor    Vcc|---------------------------------| Vcc               |__________|
  typ 1-2k      CTS|                     .'          ------------
                Gnd|--------------------'
--------------------





No internal resistor      diode method works if there's no resistor on the adapter (or if you jumpere over it) for resistors up to several k
--------------------                                   To Target device.
                DTR|                                  -----------         __________
 No resistor?   Rx |--------------,------------------| UPDI----\/\/\/----|UPDI      |
  Are you sure? Tx |----|<|------`          .--------| Gnd         \     |          |
 This is rare!  Vcc|---------------------------------| Vcc          \    |__________|
                CTS|                     .`           -----------    \Resistor of around a few hundred or more.
                Gnd|--------------------'
--------------------

         No resistor on target OR adapter - include a resistor in one of the three places shown below, whichever is easier to wire in.
         A few hundred ohms, I'd default 470. ideally not much more than that so you can use it with a target board that does have a resistor in series with UPDI

No internal resistor
--------------------                                   To Target device.
                DTR|                                  -----------         __________
 No resistor?   Rx |---------------------,--/\/\-----| UPDI--------------|UPDI      |
  Are you sure? Tx |--/\/\---|<|----\/\/'        .---| Gnd               |          |
 This is rare!  Vcc|---------------------------------| Vcc               |__________|
                CTS|                          .`      ------------
                Gnd|-------------------------'
--------------------


Resistor-based schemes - these have a narrower window of conditions under which they work reliably. They are not recommended unless there is something keeping you from using a diode cofiguration.


The PyUPDI classic:

                         4.7k resistor
No internal resistor
--------------------                                   To Target device.
                DTR|                                  -----------         __________
 No resistor?   Rx |--------------,------------------| UPDI--------------|UPDI      |
  Are you sure? Tx |--/\/\/\/\---`          .--------| Gnd               |          |
 This is rare!  Vcc|---------------------------------| Vcc               |__________|
                CTS|                     .`           -----------
                Gnd|--------------------'
--------------------

Very much like the classic, except for the possiility of a resistor on the target. Must be 470 or under on the target.

Resistance should sum to 4.7k
--------------------                                   To Target device.
                DTR|                                  -----------         __________
    internal    Rx |--------------,------------------| UPDI----\/\/\/----|UPDI      |
  Tx---/\/\/\---Tx |--/\/\/\/\---`          .--------| Gnd     =< 470    |          |
    resistor    Vcc|---------------------------------| Vcc          \    |__________|
  typ 1-2k      CTS|                     .`           -----------    \If resistor present, not more than 470 ohms.
                Gnd|--------------------'
--------------------

If the resistor on the target is much more than 470 ohms, you're gonna want to bypass it. Alternately, it may be easier to replace it with a 0 ohm resistor or bridge it with a piece of wire or even just a blob of solder, and do the classic pyupdi.


The resistor (if any) in serial adapter, and the one you add should total 4.7k.
--------------------                                   To Target device.
                DTR|                       ,--------------------------.   __________
    internal    Rx |--------------,-------'          | UPDI----\/\/\/- *-|UPDI      |
  Tx---/\/\/\---Tx |--/\/\/\/\---`          .--------| Gnd       >470    |          |
    resistor    Vcc|---------------------------------| Vcc           \   |__________|
  typ 1-2k      CTS|                     .`           -----------     \resistor of more than around  470 ohms - must be bypassed, replaced, or shorted.
or no resistor  Gnd|--------------------'
--------------------

If there's no resistor in the serial adapter, you can do it without any extrta componentys, thought you've got 4 wires involved instead of 3:

--------------------                                   To Target device.
                DTR|              .-----------------------------------.   __________
 No resistor?   Rx |-------------'      ,------------| UPDI----\/\/\/--*-|UPDI      |
  Are you sure?-Tx |-------------------'     .-------| Gnd       4.7k    |          |
 This is rare!  Vcc|---------------------------------| Vcc               |__________|
 OR resistor    CTS|                      .'          ------------    Excessively - but conveniently - sized resistor.  These were (incorrectly) popularized for the first few years of UPDI for ar
  bypassed      Gnd|---------------------'
--------------------



```
