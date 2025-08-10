# High Voltage programming on the AVR Dx and AVR Ex (and likely more future parts)

## Background
Almost since time immemorable, AVRs have featured a total of 2-3 methods of programming (the AVRrc parts had just one typically, but we don't talk about those parts here) `*`. 
	1. Some "normal" upload method, which works out of the box and doesn't require much work to interface with. This took the form of ISP on classic AVRs, and UPDI on modern ones. 
	2. (not found on the lowest end classic AVRs): Self programming, which means that the programming opperation was controlled by code executing on the microcontroller. Self programming, thus, usually refers to the use of a bootloader.
		* Classic TinyAVR parts often permitted self-programming, but did not have hardware bootloader support. That mattered little, as there are obvious techniques to implement a virtual bootloader by rewriting the reset vector.modern AVR, all parts, even the lowliest of tinyAVRs has Hardware Bootloader/SelfProgramming support, and we offer libraries to assist with carrying this out in some cases. 

	3. Some "hard" upload method.
		a. Classic AVR had HVPP (all parts except t85, t13, t84/841 and lower flash variants thereof) and HVSP (those aforementioned exceptions). 
			* HVSP required connections to all but 1 or 2 of the pins on the device. Reset was to be held at 12v during the opperation. On the 14-pin parts, since they'd be extra pins if they didn;t have some requirement, most of the extra pins have to be pulled to ground. 
			* HVPP requuired every pin on parts with up to 18 I/O pins (or something around there), and above that, it was generally two entire ports, plus reset and XTAL1. 
			* Both required +12v on RESET. Between this and needing a million connections, you can just forget about HVSP/HVPP in system. Rarely used in Arduino-land, except the occasional tiny85 or 84 "fuse doctor" which uses HV prog mode to rewrite the fuses with the default values 
			* There is nothing short of hardware damage that you can do to brick a classic AVR, assuming you remove it from the system without damage and connect it to an HV programmer. Reset the fusebits to default values, then proceed to reprogram via ISP like a normal person. The chip will be locked, but that just means you can't read their proprietary firmware; a chip becomes unlocked when the Chip Erase command is successfully executed (which can be done by either ISP or HV programing, and is done automatically at the start of each upload by the upload tool.
			 * Either way, these allow correction of a small number of specific cases where other programming techniques cannot be used; namely, anything involving incorrect values written to the high or low fuse byte has a very high risk of bricking the chip
			 	* High fuse includes RSTDISBL and SPIPROG, and ISP programming is lost if reset disable is turned on, or spi programming turned off.
			 	* Low fuse includes CLKSEL, a bitfield usually 4 bits wide, which must be set according to the clock source, according to a section near the end of the datasheet and the clock options section (near the beginning). Note that these 
		b. Modern AVR has HVUPDI (except where it has nothing)
			* HVUPDI requires nothing beyond a high voltage pulse on the appropriate pin, and hence does not preclude use in system, or allows for in system use with minimal rework (maybe removing a diode placed from reset to Vdd, or severing that trace  and reconnecting it later).
			* HVUPDI is used to reprogram after the UPDI pin is configured to do soemthing other than UPDI. The effect of activating HVUPDI mode switches the UPDI pin back to UPDI, allowing for programming. (The effect is cleared by POR only).

The 
				* The first is reconfiguring the UPDI pin into an I/O (where possible) or reset pin (tiny only), which is fixed by HVUPDI. 
				* The second is to set the BOD voltage to a voltage higher than the voltage that it can be powered at in it's current configuration (because it's connected to parts that won't tolerate the higher voltage), resulting in a part stuck in BOR until it can be poweerd from a sufficient voltage, which may requiring desoldering parts. In this case, the chip has no functionality until sufficient voltage is supplied at least long enough to fix the fuses. Note that only Ex-series and tinyAVR parts have high enough BOD options for this to be encountered with common low voltage parts. 





|                    |                                  |
|---------------|----------------------------------|--------------------------|
| Normal Upload      | ISP (SPI slave w/reset as CS) | UPDI (UART 8E2, w/ODME, LBME, and WFB = 1)) |
| Self Prog.         | Nearly always, sometimes w/HW support | Always with HW support | 
| Self-patching boot | Via virtual boot, possible       | Not possible, by design   |
| Flsdh sections     | App, sometimes bootloader        | Bootloader, App Code, App Data |
| Bootloader location| At the end of the flash          | At the start of the flash |
| Programming pins   | 1, Reset, or IO                  | 1 (UPDI/Reset/IO)
| cont..             | .                                | 2 (Reset/Input -AND- UPDI (m0, DA, DB) or UPDI/IO (all others))
| Reset as IO strength| >1k output impedance            | (tiny only) >1k output impedance |
| Irreversible brick | By physical damage only          | By physical damage only (all prior to EB-series) 
| cont..             | .                                | By design, 















`*` - "If you don't have anything nice to say...." leaves rather little to talk about, see. 
