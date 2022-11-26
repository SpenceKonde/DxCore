# Optiboot_dx bootloader technical details

This directory contains the Optiboot small bootloader for AVR Dx-series
microcontrollers as distributed with DxCore.

Optiboot is more fully described here: [http://github.com/Optiboot/optiboot](http://github.com/Optiboot/optiboot)

## Credit where credit is due
Optiboot is the work of **Peter Knight** (aka Cathedrow), building on work of **Jason P** **Kyle**, **Spiff**, and **Ladyada**.

More recent maintenance and modifications are by **Bill Westfield** (aka WestfW).

Adaptation of Optiboot_x to Optiboot_dx by **Spence Konde** (aka DrAzzy, Azduino).

## Reporting issues
This is being maintained as part of DxCore, so issues be reported to:
[https://github.com/SpenceKonde/DxCore](https://github.com/SpenceKonde/DxCore)

## Changes made beyond the largescale adaptation to Dx-series parts
* To enable writes to flash from the app, the entry point is vastly different from Optiboot_x. On the Dx-series, only a single operation is protected and must run from the boot section: The ST or SPM which actually triggers the writing of data to the flash. Thus, everything else can be in the app, and we just need to have a an SPM Z+ instruction followed by a RET, These instructions have opcodes of 95F8 and 9508. Considering endianness, we want them to come out as `F895` `0895`. This is achieved by declaring a `((used))` constant at 0x0001FA (so it's immediately before the optiboot version), with the value `0x950895F8` - endianness reverses the order of the bytesm so it gives the needed F895 0895 sequence needed. The application then stages everything, culminating in a snippet of inline assembly which copies the word to write to r0 and r1, issues `call 0x01FA` (the assembler divides the number we give in half, as this instruction is word addressed; we must not do that division ourselves), and then clears r1 and returns to C. This is a scary approach, to be sure. However, it does work!
* This document has been converted into markdown for easier viewing on github.
* For some reason, the dummy app is supposedly required. This is *intensely annoying*, because avr-size includes it, and you thus get inaccurate numbers shown during the compile process. Since the binary size is of overwhelming importance when you're as close to the edge as we are here, we needed a better way. It seems that avr-size does not have a way to limit itself to arbitrary sections. To address this, the display mode of avr-size has been amended to use the -A option, showing the memory of all sections individually; this in turn means that there's a bunch of junk sections shown, but at least the three sections we care about are shown individually instead of being mashed together with the app. Sadly, windows lacks all the command line tools that linux has, so there isn't something we can easily pipe that output to which would turn
```text
section          size      addr
.data               0   8417280
.text             476         0
.application       42       512
.spmtarg            4       506
.version            2       510
.comment           17         0
.debug_aranges     48         0
.debug_info      2955         0
.debug_abbrev     724         0
.debug_line       937         0
.debug_frame      156         0
.debug_str       2004         0
.debug_loc        833         0
.debug_ranges     104         0
```
into what we'd really like:
```
.text             476         0
.spmtarg            4       506
.version            2       510
```
Or, better:
```text
Size: 476+4+2
```
Or even better:
```text
Size: 482
```
If anyone reading this knows a way to do that, it would make me very happy if you would share that technique with me.

## Known issues:
There are no known issues at this time (other than the fact that there is no EEPROM support That is because it does not fit. It might fit if we didn't need to buffer pages and could write data as it came in, but because we don't know we're getting a program page command until the fire hose of data has been turned on, we can't get rid of that so easily. It was a design decision to not lock in a 1024 byte bootloader section just to get EEPROM write capability; and the consequences are particularly serious on modern AVRs which cannot tolerate )

Available "slack" in the binaries is extremely limited.
* 128k Dx: 6-7 instruction words (12b)
* 64k Dx: 14 instruction words (14b)
* 64k DD: 14 instruction words (28b)
* 32k Dx: 18 instruction words (36b)
* 32k DD: 17 instruction words (34b)

LED Pins
* DA/DB, all: PA7
* 20+ pin DD: PA7
* 14-pin DD, Serial0: PD7
* 14-pin DD, Serial1: PD5

### Other resources

Arduino-specific issues are tracked as part of the Arduino project
at [http://github.com/arduino/Arduino](http://github.com/arduino/Arduino)


There is additional information in the wiki - however, the wiki does not reflect the changes specific to Optiboot DX
[https://github.com/Optiboot/optiboot/wiki](https://github.com/Optiboot/optiboot/wiki)


## To set up this environment in windows
This is how the files DxCore ships with are built. I can provide no guarantee that any other method will still work after what I've done to the makefile.
1. Download and extract the Arduino 1.0.6 .zip package.
  * Make sure it is not in a "protected" location. I normally put them in the root of C:.
  * Rename the folder so you remember what it's for; I have it in `C:\arduino-1.0.6-7.3.0compiler` (this location will be used in the examples below)
  * You may delete everything except the hardware and tools subdirectories
2. Download [the updated toolchain`https://spencekondetoolchains.s3.amazonaws.com/avr-gcc-7.3.0-atmel3.6.1-azduino6-i686-w64-mingw32.tar.bz2`](https://spencekondetoolchains.s3.amazonaws.com/avr-gcc-7.3.0-atmel3.6.1-azduino6-i686-w64-mingw32.tar.bz2)
3. Extract the toolchain, and locate the top level 'avr' folder inside.
4. Copy the AVR folder to the `C:\arduino-1.0.6-7.3.0compiler\hardware\tools` directory in the 1.0.6 install. Be careful not to put it inside the avr folder that is already there (if you make a mistake, delete the mess and go back to step 1 - trying to unpick the mess is hopeless, because there are like 4 nested folders named avr, 2 folders in different locations named bin, and several 'include' folders. Don't look at me, I didn't choose that directory structure!
5. Copy the `optiboot_dx` folder from `(core install location)/megaavr/bootloaders` to `C:\arduino-1.0.6-7.3.0compiler\hardware\arduino\bootloaders`.
6. You should now be able to open a command window in `C:\arduino-1.0.6-7.3.0compiler\hardware\arduino\bootloaders\optiboot_dx` and run `omake <target>`, or `build_all_dx.bat` to build all the binaries packaged with the core.


## Previous build instructions
These may or may not work. I am not versed in the ways of makefiles and am not competent to fix any issues that may be encountered. Thus, if you want to use one of these methods, you'll have to fix the makefile yourself (sorry - my brain uses 16-bit addressing. You'll notice I never talk about working with an ATmega2560 - that would require an extra bit, ya see? So figuring out how to wrangle a makefile on a 64-bit desktop computer is well beyond my ability). I will gladly accept pull requests that fix these things - as long as they do not break the method detailed above, as that would leave me unable to build DxCore bootloaders without toolchain wrangling, which as I said, is outside my zone of competence. If you find that these do in fact work as written, please let me know so I can update this file.

### Building optiboot for Arduino.

Production builds of optiboot for Arduino are done on a Mac in "unix mode"
using CrossPack-AVR-20100115.  CrossPack tracks WINAVR (for windows), which
is just a package of avr-gcc and related utilities, so similar builds should
work on Windows or Linux systems.

One of the Arduino-specific changes is modifications to the makefile to
allow building optiboot using only the tools installed as part of the
Arduino environment, or the Arduino source development tree.  All three
build procedures should yield identical binaries (.hex files) (although
this may change if compiler versions drift apart between CrossPack and
the Arduino IDE.)


### Building Optiboot in the Arduino IDE Install.

Work in the .../hardware/arduino/bootloaders/optiboot/ and use the
`omake <targets>` command, which just generates a command that uses
the arduino-included "make" utility with a command like:
    `make OS=windows ENV=arduino <targets>`
or  `make OS=macosx ENV=arduino <targets>`
On windows, this assumes you're using the windows command shell.  If
you're using a cygwin or mingw shell, or have one of those in your
path, the build will probably break due to slash vs backslash issues.
On a Mac, if you have the developer tools installed, you can use the
Apple-supplied version of make.
The makefile uses relative paths (`../../../tools/` and such) to find
the programs it needs, so you need to work in the existing optiboot
directory (or something created at the same "level") for it to work.

### Building Optiboot in the Arduino Source Development Install.

In this case, there is no special shell script, and you're assumed to
have "make" installed somewhere in your path.
Build the Arduino source ("ant build") to unpack the tools into the
expected directory.
Work in Arduino/hardware/arduino/bootloaders/optiboot and use
    `make OS=windows ENV=arduinodev <targets>`
or  `make OS=macosx ENV=arduinodev <targets>`


## Programming Chips Using the `_isp` Targets
This does not work on AVR Dx-series, and there are no plans to change this.

## Standard Targets
Being designed for DxCore, this provides 276 targets - there are 6 combinations of entry condition and timeout duration. There are two possible sizes for a DD-series part, and 7 combinations of . In the list below, brackets indicate an optional
parameter, while parenthesis indicates a mandatory one - one of the options must be used.
* `dx128_ser(0-5)_[alt_](extr, extr_8sec, all_8sec, poronly_8sec, extronly, swronly_8sec)`
* `dx64_ser(0-5)_[alt_](extr, extr_8sec, all_8sec, poronly_8sec, extronly, swronly_8sec)`
* `dx32_ser(0-4)_[alt_](extr, extr_8sec, all_8sec, poronly_8sec, extronly, swronly_8sec)`
* `dd64_ser(0-1)_[alt(1-4)_](extr, extr_8sec, all_8sec, poronly_8sec, extronly, swronly_8sec)`
* `dd32_ser(0-1)_[alt(1-4)_](extr, extr_8sec, all_8sec, poronly_8sec, extronly, swronly_8sec)`

### The number after ser is the USART number

* DA/DB: 0-5 for 64-pin parts, 0-4 for 48 pin parts, 0-2 for 28/32 pin ones
	* All files are built for the largest pincount available with that memory size; they are binary compatible - but obviously one that tries to use a port or mux option that doesn't exist will not work.
* DD: 0 or 1 for all parts.
	* All files are built for the AVR64DD32 or AVR32DD32. These are binary compatible with lower pincounts, but as above, if you specified a port and mux option that isn't available, it will not work, see below.

### The next optional argument specifies an alternate portmux option

* DA/DB: Either omit to use default pins, or alt_ to use the alternate pins. Be aware that alternate pin availability depends on the pincount.
	* ser4_alt will not work on 48-pin parts, ser1_alt works only on 48+ pin parts, and ser2_alt doesn't work on 28-pin parts.
	* ser0 (no alt) will not work if a crystal or external clock is connected. In the case of a crystal, it may be damaged. ser2 (no alt) will not work if a watch crystal is connected, and has an even higher risk of damaging the crystal.
* DD: omit to use the default pins, otherwise, ser0 can have alt1, alt2, alt3 or alt4. (PA4/5, PA2/3, PD4/PD5, PC1/PC2). ser1 can have only default pins, or alt2 (PD6, PD7).
	* ser0_alt1 and ser0_alt2 will not work on 14-pin parts. ser1 (no alt) will not work on 14 or 20-pin parts.
	* If a crystal of any sort is used, ser0 (no alt) will not work, and may damage the crystal connected to those pins.
	* There is no ser1_alt1 - that would put the serial on PC4 and PC5. None of the DD-series parts have those pins.

### The final option is the entry condition
* `extr` - Bootloader will run on reset pinreset and software reset, with a 1 second timeout. For uploads with autoreset.
* `extr_8sec` - Bootloader will run on reset pin reset and software reset, with an 8 second timeout. For uploads with manual reset.
* `all_8sec` - Bootloader will run on everything except a WDT reset, 8 second timeout.
* `poronly_8sec` - Bootloader will run only after a power on reset (POR), 8 second timeout. For manual power-cycle uploads when you're using reset as an input.
* `extronly` - Bootloader will run on a reset pin reset - but not a software reset - with a 1 second timeout. For uploads with autoreset.
* `swronly_8sec` - Bootloader will run only on a software reset, with 8-second timeout. Be aware that if your application does not perform a software reset, and does not smash the stack and crash, the bootloader will never run (since smashing the stack triggers a dirty reset, which in turn causes the bootloader to issue a software reset, that will run the bootloader).

Every permutation which corresponds to a valid serial port on some chip is available with all entry conditions

## The bootloader **clears the reset flags** and stashes it's value in GPIOR0 before jumping to the app
This is required in order to ensure that the entry conditions are honored, and to ensure that we don't try to run after a dirty reset without forcing a clean reset. With the reset flags cleared, a dirty reset is detected by a value of 0x00 in the reset flags.  In that case, we have no idea how the peripherals are configured, and since execution may have careened about the flash, whatever configuration the app had could be altered in arbitrary ways as well. Or it could have been caused by an interrupt with no ISR firing, in which case LVL0EX is still set and interrupts can't be enabled. Hence, the only action that can be expected to restore functionality is to issue a software reset.

Note that the same thing is done for non-optiboot configurations, where this step is instead done by the sketch in .init3 (ie, as soon as the stack pointer is initialized and r1 is cleared. The behavior is the same - reset flags cleared and stashed in GPIOR0.

## Making new entry conditions
This is very easy - you need only pass ENTRYCOND_REQUIRE=0xXX. This is compared to the reset flags.
The logic is simple:
1. If the the WDRF (WDT reset flag) is set, we always jump to the app in order to avoid an infinite loop: The bootloader exits by triggering a WDT reset, so a WDT reset means that either a WDT reset got us here, or the app triggered a WDT reset. If you want to start the bootloader from the app, trigger a Software Reset instead.
2. Otherwise, if the bitwise and of ENTRYCOND_REQUIRE and the reset flags is non-zero, we run the bootloader. Otherwise, we jump to the app.

## Origins of the different sizes on different parts
The parts with fully mapped flash write by byte, but have only a couple of words of additional space free versus 64k parts. This is somewhat surprising - you would think it would make much more of a difference than it does.

DD-series 32/16k parts lose 1 instruction word because they need to look up an additional byte of the part signature. AVR signatures have 0x1E as the first byte, and the second byte indicates the flash size, (0x9s for flash size in kb = 2<sup>s</sup> 0x90 is used for sub 1k as well. Note that the rare few parts for which the flash size is not a power of 2 are not always handled the same way; the single ATmega with 40k of flash is 0x95 (32k) while the ATmega4808/9 are 0x96 (64k). The ATmegaxxxRFR/RFR2 parts have 0xAs instead), and the final byte is assigned sequentially, but the 3 high bits are often used to indicate a new series or grouping of parts. 0x8x and 0x6x are used for ATAutomotive parts. Hence, the 64 and 128k parts must look up 1 byte only (the last one) and can simply ldi the other two, as can 32k DA/DB parts). Because the same DD binaries have to support two different flash sizes, the DD 32/16k parts need to look up the last two. That means an LDS instead of LDI, and LDS is a 2-byte instruction.

The majority of the extra flash required for the 128k bootloader is to handle the extended address command.
As of DxCore 1.5.0, we do not use SPM Z+ anymore on 128k parts - it caused verification failures when data was written to the last page in the first half of the flash, but no data was written to the second half of the flash, (despite the write being successful). This only cost 1 instruction word on the 128k parts - but with essentially little hope of fitting EEPROM write, the . It is still used on 64k parts. Though they have a RAMPZ register, the datasheet indicates that it is vestigal.
```text
RAMPZ is concatenated with the Z-register when reading
the (ELPM) program memory locations above the first 64 KB and writing the (SPM) program memory locations above
the first 128 KB of the program memory

...

These bits hold the MSB of the 24-bit address created by RAMPZ and the 16-bit Z-register. Only the number of bits
required to address the available data and program memory is implemented for each device. Unused bits will always
read as '0'.
```
Since the 64k parts have no program memory locations above the first 64k, RAMPZ should never have a non-zero value, so this should not be a problem.

It is plausible that with more aggressive optimization, it might be possible to make room for EEPROM writes on 64k and smaller flash. It would take a miracle to do that on 128k parts.

### Wastes of space
There are a few points at which the compiler generates terrible, terrible code.

```asm
After calling getch() toget the high byte of the length in r24
  mov r16, r24       // copy r24 (which is never reused) to r16,
  ldi r17, 0x00 ; 0  // load zero into r17 even though the next thing we do...
  mov r17, r16       // is copy r16 to r17...
  eor r16, r16       // and now we zero out r16....
  rcall .+180        // call getch() again
  or r16, r24        // this time we use an or, just for the hell of it.

This is done twice, and it wastes 3 instruction words, because it is equivalent to:
  mov r17, r24       // copy r24 to the place where we want it
  rcall .+180        // call getch() again
  mov r16, r24       // copy r24 to where we want the second byte.

```
Savings: 6 words, more than doubling the available space on the 128k parts.

```c++
  MYUART_TXPORT.OUT |= MYUART_TXPIN; // and "1" as per datasheet.
```
The datasheet does *not* say that!
```text
Initialization
Full Duplex Mode:
1. Set the baud rate (USARTn.BAUD).
2. Set the frame format and mode of operation (USARTn.CTRLC).
3. Configure the TXD pin as an output.
4. Enable the transmitter and the receiver (USARTn.CTRLB).
```
There is errata, but it is only relevant to half-duplex (ODME) mode.
Savings: 1 word

In real world situations, does it ever run into the WDT syncbusy situation? If not, ditching that would save 4 words.

Were there a way to know we had to erase a page beofre data was coiming down the wire and we can't take 10 ms to erase the page, ,
