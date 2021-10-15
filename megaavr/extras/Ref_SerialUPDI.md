# SerialUPDI
SerialUPDI is a UPDI upload tool which uses a normal USB serial adapter with the most trivial of modifications directly as a UPDI programmer
It was written in 2020-2021 By Spence Konde and Quentin Bolsee based on the pymcuprog tool from Microchip, which is much, much, slower.

## Wiring the hardware
Briefly, you connect Rx and Tx with either a 4.7k resistor or a small (signal) schottky diode (band towards Tx). The RX like becomes the UPDI line. The diode is more tolerant of extra series resistance on the line, and doesn't require you to measure the value of the internal resistoer on the serial adapter, if there is one

### Required Hardware
There has been some debate and questions have been raised over whether the recommendations below are the best. More study is required.
1. A USB serial adapter These can be had for as low as $1 on ebay and aliexpress based on the CH340G, slightly more for CP210x or "FT232". Ideally, you want to dedicate a serial adapter to this purpose for ease of use, rather than having to connect and disconnect things every time you want to switch between progferamming with it and using as a serial adapter
1. a fast signal schottky diode (recommended) or a resistor (see below to figure out value)). Schottky, not silicon! Silicon are on the edge of working, when the whole point of using a schottky diode instead of a resistor is to move the system thoroughly into the working regime, instead of running on the edge between working and not working. The BAT-85 is a readily available through-hole part, though I prefer the SOT-23 package which can sit betwewen the pins on the bottom of the adapter, so it won't get cxaught on things.
1. 1 resistor, a few hundred ohms - 220 to 1k, even 2k is fine (may not be needed).
1. A few jumper wires. Plan to replace these periodically if you use it a lot. Cheap dupont terminals don't last.

### Serial adapter requirements
Almost any serial adapter can be used for pyupdi style programmer, as long as you take care to avoid these pitfalls:
1. The FTDI FT232, (both the genuine ones, and the fakes) are by default configured to use less CPU time by polling the adapter less often. For UPDI programming, the performance implications are severe: Even with the minimum latency timeout, it's a large portion of the time taken for a Dx-series part is spent in latency, not transferring data, and on the tinyAVR parts, latency dominates the write process. You can easily fix this (at least on windows - I'm not sure if the problem even happens on Linux): Open device manager, under Ports (COM and LPT), locate the FTDI adapter. Right click -> properties. Click the Port Settings tab, and then the Advanced button. In the middle of left hand side, there is an option called "Latency timer", likely set to 16ms. Set it to 1ms). Click OK enough times to leave the dialog. you'll see the adapter disappear and reappear as the change is applied. Configured properly, the FT232RL has spectacular performance. Configured improperly, the performance is downright abysmal.
2. Many serial adapters already have a resistor in series wuth TX! But not all of them do. Not having it is far more common on thr FT232RL based adapyer - and it all makes sense now, they're bot there to pritect the adaptwerm theye're soyou can can use a CH340G without a proper voltage switching mechanism (I've only seen a working board that switched votlages once, and it has other horrifying defects) I am working to get some of myt own design made, and they'll be way better. ! Typically between 1k and 2.2k; If using the recommended diode method, you need only add the diode, not the second resistor. If you aren't going with the diode method for some reason, use a resistor such that the total is 4.7k (you'll need to measure it with multimeter unless you can follow traces to the resistor and read out the code (the three number codees - first two numbers are the most significant digits, and the third is the number of zeros they are followed by - 222 is 2200 - 2.2k, 471 is 470, (which of course means that 101 means a 100 ohm resistor, and 100 means a 10 ohm one!)).  There are three approaches to measuring this; all wuith one end of multimeter clipped to Tx. Either
) look up the pinout of the serial adapter chip online and measure resistance to that pin,
b) check all the pins on the chip. Only one will have a value of a few k or less or
c) just go for the ends of the resistors, expecting one to have a 1-2.2k to Tx on one side, and 0 on the other.

The TX resistor is ubiquitous on CH340-based adapters. On FT232 adapters, it is not usually present. I suspect this difference is due to the fact that the CH340 can output true 5v TTL serial, while most others are 5v tolerant, but only go to 3.3v on their own.
3. Some serial adapters have a dedicated LED to indicate Rx. While some fancy chips have an I/O pin that drives the RX led (the FT232, for example), a cheap adapter with an RX LED may have just put an LED and resistor on the RX line (in fact that's what that above mentioned green boards with switch and micro USB port did).. The load from an LED on the UPDI line will overwhelm any signal from the target and prevent communication (a LED on TX is fine - the adapter has plenty of drive strength.)
4. Devices that do not output a 5v HIGH level sometimes will work at low speed but not higher speeds. A 10k pullup between the UPDI/RX line and 5v will usually sort this out.

I recommend the CH340 as the go-to serial adapter for several reasons:
* They are dirt cheap and readily available on ebay/aliexpress/amazon. 7/$5 shipped on aliexpress for the most basic design (the black kind with the stupid voltage jumper on the end. The wider ones with rounded corners and ENIG surface treatment are no better electrically, though the build quality is better (if you buy the black ones described above, it is recommended to solder the tabs ofg the USB connector in place - their wave soldering process fails to solder these most of the time), and are significantly bulkier. The better kind are what you find on ebay when you search for "CH340 6pin" - you want the black ones with the voltage switch. Until, that is, I bring my own design into production - it'll have voltage switching done *right* and a micro USB connector instead of a stuipid full sized USB-A, full-sized 1117 3.3v regulator, and a color-coded power light to indicate the current voltage setting, and a general absence of gross design flaws.
* The bare chips are dirt cheap and readily available - and also dead simple to design with. Anyone, even the clown who designed those green boards I've been badmouthing, can do it (it looked like someone's first board, frankly). Heck, a working CH340G serial adapter was the first board I designed, too (at least I didn't try to sell my first version!).
* They are - at most - a half second slower to write a full flash image with. For the price of one real FT232RL adapter, you could have half a dozen CH340's.
* They generally work at the maximum speed with just the addition of the diode

And because most of the alternatives have problems:
* The CP2102 doesn't support the top speed for Dx-series (345600 baud) - or, if you configure it to support that with the utility from Silicon Labs, you lose support for 460800 for tinyAVR.
* The PL2303 you have to downgrade the drivers for, because they're all counterfeit versions of an ancient version, and manufacturer of the chip is sick of the counterfeiters getting free driver support.
  * I only have rone PL2303 adapter handy, but as supplied, it had virtually no capacitance on Vcc - there was 0.5v ripple on a 3.3v line. It won't surprise you to hear rthat that caused problems for the thing connected to it.  Adding a capacitor fixed that (okay, fine, so what?) aren't better, and it is not clear why these are aeven being made anymote, because the build quality is worse than average, they are no cheaper, and have no advantages.
* The FT232 requires you to change the timeout setting to get acceptable performance.
  * Most FT232RL's are said to be counterfeit; the "FTDI-gate" driver debacle is far behind us, but this may matter to you for other reasons.
  * It is probably not coincidental that most FT232 adapters from China are also of noticeably poor build quality. You can see the chip not centered, the passives slightly crooked, the solder looks duller than it should, I've broken off a capacitor during normal handlingm and none of the solder joints inspirea confidence. They typically work fine, and some of them even have a 5-pin ISP header on them. (huh??? Well it's so you can progeram classic AVRs in bitbang mode....). But they really don't inspire confidence.
* As noted above, very few serial adapters output an actual 5v HIGH level though they are almost universally 5v tolerant (not damaged by 5v, but unable to output more than 3.3). These are sometimes flaky at higher speeds. CH340G does not have that issue - in fact, many "3.3v" CH340G's run at 5v, supply 3.3 to the Vcc pin and rely on the diodes to not harm thwe target. I can't say I'm a huge fan of this method... (it is totally possible to make them run 3.3 or 5v just fine)
  * If that describes youer adapter, and it staerts failing at the higher speeds, try a 10k resistor between the UPDI line and 5V,
* The HT42B534 has a variety of unusual attributes, some good, some bad.
  * It doesn't support arbitrary baud rates - no 345600 for Dx-series, or anything between 256k and 460.8k. This kind df sucks.
  * On the one hand, it uses standard windows CDC drivers; that's not a bad thing. (Not that I've ever found keeping serial drivers installed for other adapters to be a terrible burden.) Except when the standard drivers don't work very well. Setting it to a speed it doesn't like not only doesn't have the desired effect, but the attempt blocks rather than terminating gracefully. serialupdi has a fairly short timeout builtin and errors; many terminal programs will just hang until you restart them or unplug the serial adapter and plug it back in. Sometimes it stays behind in device manager after unplugging it, particularly after it hangs.
  * Modem control inputs are wrong: All show off until any one changes state, then all show opposite of actual value. Inverted behavior continues until next reset.
  * The chip (though not the available adapters) have a VDDIO pin to set output voltages, ie, a built-in level shifter. CH340 can only do 3.3V-5V, HT42 can do 1.8-5v. It would be a very nice feature if it was exposed in breakout boards and if these parts didn't have all the other quirks that they do...
  * They are almost CH340 cheap.
  * The latency is significantly lower than other serial adapters. Earlier during the spring 2021 optimization effort, after optimization for the "simpler case" of writing to a Dx-series (it is simpler in that there are larger pages, hence larger blocks of time spent, the speed of uploads was determined in large part by latency, rather than raw speed. This version performed significantly better on the HT42B534. Subsequent optimization allowed the speed of the other adapters to nearly equal that of the HT42B534, without the rest of it's quirks.
  * Unfortunately as a consequence of that, at higher baud rates it fails to program tinyAVR parts because it starts sending the next before a page write cycle is completed. while the other adapters have so much latency that they give the chip time for it's page write by default. This issue occurred with a number of other adapters as well and is fixed in SerialUPDI 1.2.0 (turbo mode for tinyAVR and megaAVR still doesn't work - this is expected, don't use turbo mode with an HT42 based serial adapter tiny and mega should use the 230400 version. Dx-series  has a special option just for the HT42 to maximize speed)


Now, if you find yourself with 2-4 serial adapters plugged in at all times, FTDI's offdering gets more attractive. They make an FT4232 with 4 serial ports from a single USB port. You do have to thange the latency timer for all 4 ports. The only breakout board I found was fairly pricey even from China (it doesn't look like the chips are being copied), and it would be better if it had some normal 6-pin headers, but it otherwise replaces a 4-port hub and 4 CH340's. Performance is identical to a FT232.

### A note on breakout boards
Some tinyAVR and other UPDI-using-part breakout boards have an on-board resistor. Sometimes this is a 4.7k one. That is NOT appropriate. I was part of the problem for a while. I think the original mistake came from people conflating the pyupdi resistor with a generally appropriate series resistor. When I started megaTinyCore, my early collaborator was making hardware with a 4.7k resistor; I assumed he was doing it right. This doesn't work with serial UPDI. It will work with dedicated programmers like jtag2updi, as long as they don't have their own resistor, or that resistoe is an appropriate value. . Suffice to say, for a time it was a very common belief. I use 470 ohms now, but I can't find fault with a design over it not having the resistor. Expecting the programmer to be able to provide series resistance is not an unfair expectation,

### Connections:
* Vcc, Gnd of serial adapter to Vcc, Gnd of target
* Add either a resistor or schottky diode between Tx and Rx (in the case of serial adapters without their own TX series resistor, an external one is needed. See the charts below.
* Many adapters have a built-in 1k, 1.5k, or 2.2k resistor in series with Tx.
  * If yours does, you can simply install a diode from Rx to Tx, (band towards Tx). My top pick here is the BAT54C; it's in a tiny SOT-23 package (it's 2 diodes both weith the "band" towards the pin that's alone on one side) Why? Because, assuming your serial adapter has the pins on 0.1" header, and TX and RX are next to each other (both extremely common) the the diode fits right in between them. and with no lead that could later fatigue and break the result is less likely to be damaged by rough handling. Then if I want to more obviously mark it as a UPDI programmer, I might cut off the Tx, DTR and CTS pins. Serial adapters go for under for a buck a piece on ebay, so it's worth making dedicated UPDI programming ones.
  * If your adapter does *not* have a resistor in series with Tx, put a resistor (470-2.2k) in series with your diode - this is to protect both sides in the event of a total loss of synchronizatiopn where the target and programmer drive the pin in opposite directions, as well as damage if the adapter is plugged in backwards to an externally powered board (ie, with board Vcc connected to programmer serial lines - part of the protocol to deal with a target that's nonresponsive involves a very long "double break", though I think even unprotected, most adapters would survive a lot of double-breaks).
  * If you don't have any small schottky diodes on hand you can instead use a resistor - you want a total  of 4.7k resistance, counting your resistor and the one on the adapter, between Tx and Rx.
* Rx of adapter to UPDI pin of target. A small resistor (under 1k - like the 470 ohm one we generally use) in series with this on the target board is fine.


```
USB Serial Adapter
With internal 1-2k resistor on TX
This is the case in 90% of USB serial adapters.


Ideal:
internal resistor in adapter: 2.2k >= Ra

--------------------                                 To Target device
                DTR|                                  __________________
    internal    Rx |--------------,------------------| UPDI---\/\/---------->
  Tx---/\/\/\---Tx |-------|<|---'          .--------| Gnd    470 ohm (100 ~ 1k)
    resistor    Vcc|---------------------------------| Vcc
  typ 1-2k      CTS|                     .`          |__________________
                Gnd|--------------------'             If you make a 3-pin connector, use this pinout
--------------------

or

--------------------                                 To Target device
                DTR|                                  __________________
    internal    Rx |--------------,------------------| UPDI----------------->
  Tx---/\/\/\---Tx |-------|<|---'          .--------| Gnd
    resistor    Vcc|---------------------------------| Vcc
  typ 1-2k      CTS|                     .`          |__________________
                Gnd|--------------------'
--------------------



Also works great, convenient if still using jtag2updi without resistor built into it. Resistorss should sum to less than 4.7k, preferalby much less

--------------------                                   To Target device
                DTR|                                  __________________
    internal    Rx |--------------,------------------| UPDI----\/\/\/---->
  Tx---/\/\/\---Tx |-------|<|---'          .--------| Gnd     < 2.2k
    resistor    Vcc|---------------------------------| Vcc
  typ 1-2k      CTS|                     .`          |__________________
                Gnd|--------------------'           Series resistor between header and chip UPDI pin on target PCB
--------------------                                I use 470 ohm resistors.

Yes internal resistor on adapter
Yes resistor on target: several k or more:
This will often by 4.7k: it must be bypassed, replaced with a smaller one or shorted out.


--------------------                                   To Target device
                DTR|               ,----------------------------------.
    internal    Rx |--------------/                  | UPDI----\/\/\/--*-
  Tx---/\/\/\---Tx |-------|<|---'          .--------| Gnd       4.7k
    resistor    Vcc|---------------------------------| Vcc
  typ 1-2k      CTS|                     .'          |__________________
                Gnd|--------------------'
--------------------



No internal resistor on adapter.
Yes resistor on target >= 100 ohms and not more than a few k.

--------------------                                   To Target device
                DTR|                                  __________________
 No resistor?   Rx |--------------,------------------| UPDI----\/\/\/------>
  Are you sure? Tx |----|<|------`          .--------| Gnd     > 100
 This is rare!  Vcc|---------------------------------| Vcc     < 2.2k
                CTS|                     .`          |__________________    Resistor of around a few hundred to a few k
                Gnd|--------------------'
--------------------


No resistor on target OR adapter:

Include A resistor in ONE of the three places shown below, whichever is easier to wire in,
the first and second positions, with nothing between Rx and target UPDI, are slightly preferable

Value of resistor should be a few hundred ohms, I'd default 470.

No internal resistor
--------------------                                   To Target device
                DTR|                                  __________________
 No resistor    Rx |---------------------,--/\/\-----| UPDI---------------->
                Tx |--/\/\---|<|----\/\/'        .---| Gnd
 This is rare!  Vcc|---------------------------------| Vcc
                CTS|                          .`     |__________________
                Gnd|-------------------------'
--------------------


Resistor-based schemes - these have a narrower window of parameters under which they work reliably.
If you look at the UPDI line on a 'scope while it is malfunctioning, you will see that sometimes
the voltage is not going all the way down to ground when one side tries to assert it.

They are not recommended unless there is something keeping you from using a diode configuration.


The PyUPDI classic:

                         4.7k resistor
No internal resistor
--------------------                                   To Target device
                DTR|                                  __________________
 No resistor?   Rx |--------------,------------------| UPDI---------------->
  Are you sure? Tx |--/\/\/\/\---`          .--------| Gnd
 This is rare!  Vcc|---------------------------------| Vcc
                CTS|                     .`          |__________________
                Gnd|--------------------'
--------------------

Very much like the classic, except for the possiility of a resistor on the target. Must be 470 or under on the target.

Resistance should sum to 4.7k
--------------------                                   To Target device
                DTR|                                  __________________
    internal    Rx |--------------,------------------| UPDI----\/\/\/------>
  Tx---/\/\/\---Tx |--/\/\/\/\---`          .--------| Gnd     =< 470
    resistor    Vcc|---------------------------------| Vcc         \If resistor present, not more than 470 ohms.
  typ 1-2k      CTS|                     .`          |__________________
                Gnd|--------------------'
--------------------


If the resistor on the target is much more than 470 ohms, you're going to want to bypass it. Alternately,
it may be easier to replace it with a 0 ohm resistor or bridge it with a piece of wire or even
just a blob of solder, and do the classic pyupdi. Note that using a diode will work with resistances oin the target
that are too much for it to work using a resistor.


The resistor (if any) in serial adapter, and the one you add should total 4.7k.
--------------------                                   To Target device
                DTR|                       ,--------------------------.
    internal    Rx |--------------,-------'          | UPDI----\/\/\/- *--->
  Tx---/\/\/\---Tx |--/\/\/\/\---`          .--------| Gnd       >470
    resistor    Vcc|---------------------------------| Vcc
  typ 1-2k      CTS|                     .`          |__________________     resistor of more than around  470 ohms - must be bypassed, replaced, or shorted.
or no resistor  Gnd|--------------------'
--------------------

If there's no resistor in the serial adapter and the target happens to have a 4.7k resistor, you can do it without
any extrta components, though you've got 4 wires involved instead of 3:

--------------------                                   To Target device
                DTR|              .-----------------------------------.
 No resistor?   Rx |-------------'      ,------------| UPDI----\/\/\/--*---->
  Are you sure?-Tx |-------------------'     .-------| Gnd       4.7k
 This is rare!  Vcc|---------------------------------| Vcc
 OR resistor    CTS|                      .'         |__________________
  bypassed      Gnd|---------------------'       Excessively - but conveniently - sized resistor.
--------------------                        These were (incorrectly) popularized for the first few years of UPDI

```

Why do we want a little bit of resistance (a few hundred) when doing the diode method? The classical response is "In case target and programmer are completely out of sync and try to drive the line in opposite direcctions" Note that on tinyAVR this isn't as much of a concern, as the output drivers on the UPDI/reset pin are so weak that it seems unlikely that they are capable of exceeding the maximum current per pin specification. Besides that, though, there's another reasonn: So that when (not if) you plug the connector in backward, but the target has external power too, there isn't a risk of the TX pin of adapter being damaged from trying to drive the positive supply rail low;

### Jumper reliability
Don't expect frequently used dupont connectores to work relivbably for long, unless you bought the good terminals (Amphenol is the current manufacturer, having bought Berg which bought DuPont's connector division eons ago), you want the highest spreing force version, since you are using with a 3-pin connector (they spec the middle spring force for 10-40 pin connectors). Expect to pay 30-40 cents each, and if you put them side by side with the cheap ones (which are clones of the Harwin knockoff), there is no chance of mistaking the two.  I think having 2 holes in a 5p housing empty may make it worse - I'm not really sure why this happens so fast with UPDI cables, but it seems to. I had similar problems with jtag2upd, but it seems rto happen more with UPDI than other protocols and I don't know why. I have experiments in progewss aimed at finding a way to make temporary connections to standard pin header that does not become unreliable over time. Dupont line bought pre-crimped that is in the form of ribbon cable is trash. Try to minimize your use of it. The kind that has individual wires is better.

## Software

Choose a Serial-UPDI option from the Tools -> Programmer menu of a core which supports this upload method (megaTinyCore, DxCore, and soon, MegaCoreX). The tinyAVR core offers 57600 baud, 230400 baud and 460800 baud options. DxCore offers 57600, 230400, 345600 and a "special" 460800 w/chunking for use with the HT42B534 serial adapter, which can't do 345600 baud (and any other adapter runs far more slowly with this option) and select the Serial Port from the Tools -> Port menu.

Note that this does not give you serial monitor - you need to connect a serial adapter the normal way for that (I suggest using two, along with an external serial terminal application). This technique works with those $1 CH340 serial adapters from ebay, aliexpress, etc. Did you accidentally buy some that didn't have a DTR pin broken out, and so weren't very useful with the $2 Pro Minis you hoped to use them with? They're perfect for this. Although the CH340 parts have the lowest performance, the difference between parts is now quite small (this was not the case prior to the optimization leading up to the 2.3.2 release, before which the speeds ranged from 125 bytes per second to just over 1k.

## Technical details

### UPDI protocol
The UPDI protocol uses a single wire for bidirectional communication with the target chip. To prevent collisions the target chip never initiates a transfer - the programmer sends a command and then waits for the expected response, if any. If it doesn't get a response, wheb it was expecting one, something went weong. SerialUPDI will print a (generally thoroughly useless) error, which no worse than other UPDI programming tools (or programmers in general). The data itself is sent as though the data line was a USART in one-wire mode, with a special autobaud enabled. The autobaud behavior, and there fact that it talks to sonme supervisor instead of the normal running chip, is the main thing differentiating it from a normal serial port set for 8-bits, 1 start bit, 2 stop bits, and parity enabled (ie, the data frames are 12 bit periosds long. Theoretical maximum bytes per second is just shy of (BAUD RAT)/12 - At the minimum, it will always wait 2 bit periods between when the programmer finishes sending and when it starets transmitting. The weird thing is that when idle, autobaud works with just the sync character. Break will reset evetything, but isn't requured except to begin communication. That signals the start of a transaction and initiates and sets the baud rate to be used. There are at least 4 special modes available, each of which requires sending a 16-byte KEY: NVM programming mode, Chip Erase (this will also unlock locked chips after erasing), and Write User Row (write data to the user row, this can be - and generally is only - used on a locked chip and is rare outside of peroduction). Finally there is the Debugging mode, which is not described in the datasheet, though the KEY is known (and could easily be sniffed if it wasn't). There could be any number of other keys that are not known outside of Microchip. We know they do a factoey calibration. I imagine that it would be done uaing a different key (and likely with a bit that can be set to truly disable permanently - in some chips security measures are provided by what they call an e-fuse (not to be confused with the configuration fuses we have here throuhgh the origin of the name is the same) which is just a thin, weak interconnect within the chip that it can causer to "fuse" (like the circuit protection kind of fuse") rendering a manufacturer only configuration mode unusable aftwerward). Th

In NVM programming mode, you can read the flash as well as the "data space" (the 64k address block the chip sees during normal operation. Addressing is 24-bit, high bit is set to access flash, and you write by manipulating the NVMCTRL registers and writing to the flash as though you were a bootloader, except that ypou can also write fuses, execute chip erase command, and of courese weite to the bootloader section of the chip, and whenever connected via UPDI you have the powrt to assert or release thwe UPDI reset state, which must be done in a specific pattern to ensure that the WDT isn't running, as it would reest the chip and you would be having a bad time. Note: Yes! This means that a programmer could be used to do paged erase and werite! Serial UPDI doesn't have this feature. I don't know of any that do. It's not hard to implement and this may be changed in a future release. My thoughts on this are at the bottom of the document.

### Upload and verify performance

    BAUD    |    FT232RL    kb/s   |     CP2102    kb/s |     CH340    kb/s  |    HT42B534    kb/s  |
------------|----------------------|--------------------|--------------------|----------------------|
115200      |     8.7 W /    8.8 R |     8.7 W /  8.8 R |  8.4 W /  8.5 R    |     9.0 W /  9.1 R   |
230400      |    16.6.W /   16.4 R |    16.3 W / 16.5 R | 14.6 W / 16.6 R    |    17.7 W / 17.9 R   |
345600*     |    24.3 W /   23.4 R |    23.2 W / 23.0 R | 22.5 W / 22.1 R    |        UNSUPPORTED   |
460800**    |             N/A      |                N/A |             N/A    |     24.7W / 32.7 R   |
** HT42B534 was run using a 32-byte block size, running with finite block size resulted in successful transfers for other parts, though the threshold block size varied - but a massive decrease in overall speed, similar to 115200 baud., as one will outrun the NVM controller writing at 460800 baud - I just had to see how it compared to the FT232RL. Both of them are running right up at the limit of the chip's ability to write data to the flash - and the FT232RL doesn't need any special measures taken and works with the tinyAVR parts too. On the other hand, the HT42B534 leads the pack at the (new as of 1.3.6) default of 230400 baud, and is dirt cheap (CH340-level prices).

For comparison, on the Dx-series parts (which are easier to use as test subjects since they have more flash, so uploads take longer and are easier to time. These numbers were taken using a 128k test image, which is an optimal situation.

Programmer      |  Read    | Write    | Notes                                    |
----------------|----------|----------|------------------------------------------|
jtag2updi       | 6.6 kb/s | 5.9 kb/s | Running on 16 MHz Nano                   |
Curiosity Nano  | 5.9 kb/s | 3.3 kb/s | Via avrdude - which is likely (hopefully!) not ideal  |
Optiboot Dx     |10.6 kb/s | 6.9 kb/s | 115200 baud as supplied by DxCore        |

Because of the smaller page sizes and the more timeconsuming rigmarole surrounsing that, ATtiny parts are slower to program; the smaller the pages, the slower it is - but the time for programming the entire flash is still less for smaller parts because there is less data to write, because net programming speed (byres/second after overheaad goes down more slowly than the flash sizes. The two write numbers are for parts with 64 byte and 128 byte pages, respectively.

    BAUD      |     FT232RL    kb/s |     CP2102* kb/s   |     -   CH340    kb/s  |    HT42B534    kb/s       |
--------------|---------------------|--------------------|------------------------|---------------------------|
115200        | 4.6, 6.2 W /  8.8 R | 4.3,6.2 W / 8.8 R  | 3.6, 5.2 W /  8.5 R    | 3.6,7.2 W / 9.1 R         |
230400        | 7.5,10.0 W / 16.5 R | 7.1,9.6 W / 16.4 R | 4.9, 7.8 W / 15.6 R    |    Errors out             |
345600*       | 9.1,13.6 W / 23.4 R |8.5,13.2 W / 23.1 R | 5.6, 9.5 W / 22.0 R    |    UNSUPPORTED            |
460800        |10.4,15.6 W / 28.2 R |        Not tested  | 6.0,10.4 w / 26.8 R    |    Errors out             |
* The CP2102 does not, by default, support any speeds between 256kbaud and 460800 baud - but a free configuration utility from Silicon Labs enables customization of the baud rates in each range of requested speeds (though unfortunately, you can't define those ranges). I reconfigured mine for 345600 baud for development of with the Dx-series parts, which don't work at 460800, and did not bother to set it back to factory settings just to fill in the table; I would expect to see approximately 10kb/s and 15kb/s write speeds and around 26kb/s read speed. T

In this case, S<sub>prog(64)</sub> = 2/3 S<sub>prog(128)</sub>, so t<sub>prog(16k)</sub> = 3/4 * t<sub>prog(32k)</sub>)  (t<sub>prog(16k)</sub> = (16/32) * t<sub>prog(32k)</sub>)/(S<sub>prog(64)</sub>/S<sub>prog(128)</sub>) = t<sub>prog(32k)</sub>) * (3/2)(1/2).)

No modern AVRs exist with page sizes of less than 64 bytes. Only 64b, 128b, and 512b exist. While many expected a page size of 256b for the DD-series - the most recent ATpacks show 512 (then again, they have other stuff copy-pasted from the DA/DB too still).

| Part Series         |  Flash range |  Page size | Writes by | Max. Speed R, W | Full Write | 230400 |
|---------------------|--------------|------------|-----------|-----------------|------------|--------|
| tinyAVR 0/1/2       | 2-16k/   32k | 64b / 128b |     Pages | 28k/s, 10-16k/s |    <  2.2s |  <  5s |
| megaAVR 0           | 8-16k/32-48k | 64b / 128b |     Pages | 28k/s, 10-16k/s |    <  3.3s |  <  8s |
| AVR Dx-series       |      32-128k |       521b |     Words | 23k/s,    24k/s |    < 11.0s |  < 16s |
| Dx-series w/HT42    |      32-128k |       512b |     Words | 32k/s,    24k/s |    <  9.4s |  < 15s |
| AVR Ex-series*      | 8-16k/32-64k | 64b / 128b |     Pages | 28k/s, 10-16k/s |    <  4.4s |  < 10s |

Full write assumes using the fastest programming option to write the entire memory of the largest on the list with the FT232 except where noted. 230400 column is same, except at that baud rate.

**Numbers for the EA-series are speculative!** It is assumed that it will behave mostly like tiny/mega. Page sizes for unreleased parts pulled from the headers (for the DD-series) and pycmnuprog repo (for the EA-series). Only product briefs are available for those series, and IO headers are only out for the AVE64DD parts. One interesting thing to note about theser is that while everyone has been talking like the DD is closer to release, the official pymcuprog repo tells a different stoery: It had no DD device spec at a time when it had not only 64k EA-series, but also it's smaller siblings. They have been releasing the largest oer second largest flash version, then filling in below that, then doing there largest on modern AVRs. That implies that the EA may be further along than we realize, and the DD may be hitting some unexpected snags. In any event, I'll be happy if they take long enough that when the product is released, they've actually fixed all the known bugs instead of just catalogued them, and tested it carefully enough that it will take me more than an hour after progeramming one trip over multiple silicon bugs (DA-series I tripped over 3 in that time).


**Note** - those numbers were taken with SerialUPDI 1.1.0, versions of the core with later versions (megaTinyCore 2.4.0 and DxCore 1.4.0) may have different performance. megaTinyCore will enable a write delay except in TURBO mode on these parts to improve compatibility with different adapters, at the expense of significantly slower write operations. Not used in turbo mode, so turbo still works just as fast.  TURBO mode does not use this delay, and instead assumes the adaptyer's USB latency will do the trick (as the shortest delay that can be added by time.sleep() in python is several times longer than we need, added on top of the USB latency! For CH340G and FT232RL on Windows, it is. Thankfully, it appears we narrowly dodged a need for a more painful compatibility measure impacting all operations on everything.


#### Observations on Speed
The following is based on the numbers above, test data not included, as well as examination of oscilloscope trace. Note that the 460800 baud data for the Dx-series is not directly comparable as it uses blocksize to artificially slow the writes to prevent overrunning the buffer on the target (which is barely a buffer) which the chip empties at a constant rate.

Approximately, write time can be modeled as:
T = ceil(size / pagesize) * C + size / (baud in bits/sec / 12 bits/byte)
where C in turn can be modeled as:
C = (c * n) + d/(baud / 12)

12 bits to 1 byte because the UPDI protocol uses 1 parity bit and 2 stop bits.

The same relationship holds - with a different constant - for read and write with the size of blocks read substituting for page size.

`d` is the overhead in bytes involved in setting up each page; it depends on the series of the part in question.

`c` is a charachteristic time depending on the serial adapter - the USB Latency. This is not actually a single consistent, constant value; potential values of it appear to be quantized and certain USB latency pauses within the upload process are of different durations.

`n` in a the number of separate USB transactions required to set up each page write, a function of the procedure used by the programming tool. Most of the optimization involved reducing this number; it is 2 for reads and Dx-series writes. For tinyAVR and megaAVR it is 4 when writing.

Monitoring the transactions on a scope, one can pauses with small exchanges p
(small overhead)->(latency) repeated n times per page, followed by a write lasting a period of time proportional to (pagesize + d)

Within the region of interest, baud is large relative to d, so C is nearly constant

At low baud rates, `T` tends towards a inverse relationship with baud.
At high baud rates, `T` tends toward a function of size, NVM version, and serial adapter only. Overcoming this limit requires reducing the number of USB transactions per page written, or block read.

T<sub>read</sub> is the same on all parts; an unimplemented change could push it a bit lower by bringing N all the way down to 1 from 2. The practical impact, however, is very small; after the end of the performance enhancement push, T<sub>read</sub>, reading in 512b blocks is dominated by the second term.

For the Dx-series parts as shown in the above table, with the full suite of optimization, T<sub>read</sub> = T<sub>write</sub> (the same change is likely possible here; the improvement is maybe 10%)

For tinyAVR, T<sub>read</sub> is the same as Dx, but T<sub>write</sub> is far worse - not only is pagesize smaller but `n` 4 instead of 2 so `C` is twice as high. At 460800 baud, the second term is about a third the magnitude of the first for write (and the tinies will accept writes at 691.2 kbaud). The important thing to keep in mind is that these chips still program in seconds - and the page write is being committed during the time that one of these pauses is ongoing.

## Thoughts on paged erase
There are 4 "erasure modes" imaginable, (by me at least, and assuming the user desires a practical working chip - sute, we could randomly erase pages, then write the flash over  it and see what happened, but that's beyond the scope of fuerher consideration),  plus one subset of an erasure mode that can be more readily implemented and is of relevance in known use cases.  Ny thoughts, where present, on the prospcts are in italics after each line.
1. Chip Erase alwayss - simple, easy implement, and analopgous to ICSP programming in the classdic AVRs. That's wehat we do now. It is around 20ms counting overhead, and lattency. It's fast and essy.
2. Erase as you go - reading the file you've been asked to write, every time you get to something in a different page than you were previously weriting to, take a time out and erase that page. This is basically what the bootloader does, except that it would have the added disadvantage of the latency overhead. change nvm command to NOOP, change it to FLPER, `ST ptr`, `ST *pter`, wait for the page erase then set nvm command back to NOOP, then to FLWR and proceed. That would be about 17ms delay per page. This would add like 4.5 seconds to the programming process for full 128k part. (versus the 2.6 it adds for a bootloader) to a programming process that may be taking only 11 seconds including verification: a hefty price. *I don't really like the idea, I didn't work like hell to cut every unnecessary round trip to the chip and make this faster so I could throw away 7 latencty periods aand a 10ms page erase per page... On the other hand, some people seem to want this, and it's relatively easy to implement. It will never be a default mode, and I'm not particularly interested in it.*
3. Supply a hex file. Programming tool would calculate the fastest procedure that would clear all there pages needed, and no pages before the start of it (to spare the bootloader). Severaol variants of this are below *I ain't gonna write a hex file analyzer for this, but if I ever implement 4c, and you want to write an analyzer function that will output a map like I described, I'd be happy to use it*
  a. Make no guarantees beyond that.  *okay, doing this isn't hard, start with address of the page with first data erase with largest block starting with that page, go to address aftwer the last byte that was erased, if that ias in the hex file repeat until either file or flash runs out"*
  b. As a, except that pages after the hex file would be left intact *Would need to analyze the file to find the last page weitten and first page written, then go use same algorithm above except that we would erase that largest block that doesn't breach end of the region*
  c. As b, except that that any page not written according to the hex file would ber left intact. Same net effect as 2, except with speed penalty on the order of under 0.25 seconds for any non-perverse file.  *requires full analysis, and basically ends up as 4*
  d. As c, and if a page was partially written, the old data there would be preserved, meaning it would have to read the last pager, modify that temporary data *Dreaaaaam on! The best you'll get from me, ever, is a way to tell it not to erase pages. And if you then tell it to write to those unerased pages result is bitwise AND. If you want read-modify-write, then read, modify, and write yourselfyo can do that now...just read the flash to a hex file, and mwrge it with the new one giving new file precedce - you can do that NOW, too.*
4. Erase pages according to an "erasure map" and those pages would be erased. Variants
  a. It's a 256-bit binary value, (or however many pages the part has). But it would need some encoding to make suree it was printable, and could be used in a command line. You can get about 6 bits per character before things get dicey. So 4 bytes to represent 3 of data if willing to decode it (43 bytes total), or 64 total if we just used hex ansd got 4 bits/byte. *That's a mighty long argument to have to supply. But okay, chunk it 4 bytes at a time to process it.... Would be only slightly less annoying that 3c and awkward for the user*
  b. Argument contains erasure list, ordered by address. _ means do not erase it and go to next address, 0-5 are 2^n page page erase, skipping all pages between current address and next valid beginning of a block of that size. End of line means no further erasures. `_01234554315554321` would skip page 0 to spare an existing bootloader, then erase everything except the last two pages and a 6 page "hole" before the midpoint (maybe it's gots some data stored there because it's readable with lpm instwead of elpm or mapped flash). `_012345555555` would skip first page etc. *I like something like this - but it deals badly with poorly positioned holes - it could need to be up to 256 bytes long if only erasing the first and last page, for example*
  c. As b, except intreoduce additional characters: A, B, C, D, E for which would be analogous with 1, 2, 3, 4, 5, except they would mean "skip that size block, **starting from the next block of that size if current address isn't, including everything from here to there**" like the numbers, only skipping instead of erasing, so `_E555555` would skip the first 64 pages and erase the rest, and both `_D5555555`and `_5555555` are the same. The advantage is for cases with large holes, Imagine application that occupied entire low flash, and has user data in the rest that we want to persist - except that final 4 pages need to be erased because that region is where you store data with firmware-version-specific semantics. `5555EEEDCB2` instead of `5555_________________________________________________________________________________________________________________________2`. You can still contrive perverse cases, like say, `_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0_0` which would erase every odd numbered page. *I think this is the way to go if I ever implement something more sophisticated than erase-as-we-go . I don't care if the user experience for erasing patterns that are not part of plausible flash write is lousy, the algorithm doesn't care if a user supplies a perversely long command unless it's so long that the OS objects to passing it, they aren't actually trying to erase or probream anythingm, they're just trying to find corner cases that don't work.*
5. Fractional Erase - Erase the first n blocks of 32 pages. Nothing more complicated supported. This is on the list for one reason only, and that is that if you want the data written to the flash with Flash.h without a bootloader to be preserved, this is the simplest performant solution. *if people start complaining this is an option. I haven't had anyone ask about this, though....*

No matter how this is done though, the verify process needs to be significantly adapted so it doesn't care about the rest of the flash.

### Brain teaser inspired by above
Writing over flash twice will result in bitwise and of the two. What's interesting about this is that you can even look at the start and it doesn't immediately look scary, particularly if neither was compiled with -mrelax, though close examination of the vectors will reveal that all is not well.  You know that on a part with >8k flash without -mrelex evey other word in the vector table is suopposed to be `0x940C` (`0C94` in the order it's stored in flash - AVR is little endian), worth -mrelax it will almnost always be alternating `0xC___` (`__C_` in the hex file) and `0x0000` and on smaller flash parts, they use 1-word vectors and so there's no scattering of NOOPs, just a bunch of  `0xC___` in a row at the start before program starts. You can see how AND would leave most of that intact. But you could still see that the rest od the values were differenbt. What would be the give-away within the vector table?

It tuerns out there's a lot of order there, and the result will be basically guaranteed to be obviously invalid. Other bitwise operatoers are also obvious. The rest of the file has far more entropy, and it won't be as obvious

Suopose someone has 2 hex files. Assume they both fill the flash. This infidual gives you one file. The file is either one of the two hex files, or it the result of combineing the files using some bitwise operatoer (except foer the interrupt vectors at the start). The original progerams are both very useful to you, and you've wired up hardware to his specifications. He swears that the hex file he gave you is one of the real program and works just great, and if it doesn't work for you, you must not have wired it right. What operation would he need to use to make the fake most believable?

The hex file doesn't work. You suspect he gave you the mangled file instead of a real one (he was clever wenough to not alter the vector table, if he did that). What would you look for to prove that he gave you a sham file?
