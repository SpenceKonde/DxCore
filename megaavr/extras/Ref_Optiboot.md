# Optiboot for DxCore
Optiboot existed long before these parts had even been conceived of, as an answer to the bloated bootloader used on the early Arduino boards. A key improvement was that Optiboot cut the STK500 protocol down to the bare minimum needed for communication with avrdude (It might be described as "read and write flash on command, otherwise, smile and nod" Any command that we don't know how to answer, we just respond with an acknowledgement and hope or fake numbers and hope it's okay with that). It was subsequenrly ported to every classic AVR imaginable. And then to the modern AVRs (they are very different in how they write - but the STK protocol didn't change, and implementing that is more work. )

## Bootloader support
This can be installed using a UPDI programmer by selecting the desired part, and using the Tools -> Burn Bootloader option. Afterwards, uploading can be performed via serial using the appropriate pins. If you are using a crystal, (hopefully obviously) you can't use USART0's default pin mapping. Note that even with a crystal, the bootloader still runs from the internal oscillator, so you will always be able to reprogram, even if the crystal isn't oscillating because of improper loading capacitors or defective crystal, or your trying to overclock and the chip you have can't take that speed.
The bootloader will be lost if new code is updated via UPDI programming - otherwise, it was guaranteed to crash immediately.

Once the part is bootloaded, sketches can be uploaded by connecting a serial adapter to those pins (including the usual DTR-autoreset circuit), and clicking upload.

## Bootloader considerations
Should you use a bootloader?
1. If the board is for production and you don't intend for users to be updating the code, no. Use UPDI programming
2. For development, if you don't have an HV programmer, unless you're working with tinyAVR 2-series with 20 or 24 pins, it is not recommended for the reasons described above. If you are, it is personal preference.
3. If you need to store data in flash from the app - you don't have a choice; you must use optiboot (see below).

Assuming you *are* working with a board that supports autoreset...
1. Using a single port and bootloader is an "easier" setup for people to get working - it's only dependency is avrdude, versus SerialUPDI which needs to pull in a whole python package or jtag2updi which needs another processor. So this is a good choice for a product that end users will update in the field. It's also much more like classic AVR devices with which everyone is familiar.
2. There are many ways to get normal serial to work over longer distances (as in longer wires). These do not apply to SerialUPDI, so if it's at the end of long wires, you might do better with Optiboot.
3. If the board is designed with the expectation that it will be used Optiboot (it has autoreset, and the documentation recommends using optiboot, you'll likely be happoer using that.

## Upload speed
It's actually not bad - it beats jtag2updi, easily (since that needs to send each page to the programmer, and then the programmer has to send that to the target), and is highly competitive with serialUPDI, more so than the Dx-series is. The more complicated write process means more USB latency delays on SerialUPDI than on the Dx-series, which works in Optiboot's favor. However, SerialUPDI is able to win by sheer brute force - Optiboot runs at 115200 baud, but SerialUPDI can be run as fast as 460800 baud!

I've been asked about supporting the no-verify option. I have decided against providing that option - there is absolutely no error checking whatsoever at any point in the upload process other than verify.

## Sketch clock speed
**IMPORTANT**
When you "burn bootloader", the base oscillator frequency is set according to the selected clock speed. The actual operating speed while running the sketch is when the sketch is compiled. You can bootload with a 16-MHz derived speed, change the menu to 20, and then upload something, and it will run at the wrong speed, and you will see nothing but gibberish on the serial monitor. If you initially set it to 16/8/4/1MHz, you may use any of those options when you upload your sketch and it will run at that speed; if you initially set it to 20/10/5MHz, you may use any of those options. If you wish to change between 16/8/4/1MHz and 20/10/5MHz, you must burn bootloader again - failure to do so will result in it running at the wrong speed, and all timing will be wrong. When uploading via UPDI, we set the OSCCFG fuse every time. That's not possible when uploading through the bootloader though. And since the bootloader runs at the same baud rate, you aren't notified by an upload failure (like would usually happen on classic AVRs.)

There is a "workaround" for this sketch dependence on the fuse value, though. The tinyAVR parts have an extremely flexible oscillator. For the 1-series, the "16 MHz" one can be tuned to speeds between (typically) 10 MHz and over 25, while the "20 MHz" one can often hit 32, though none of the ones I've tried it on were stable at 32. The 2-series parts can go from approx. 12 up to around 30 on the "16 MHz" oscillator, and something like 15 through the mid 30's on the 2-series (they're often stable as high as 32, unlike 1-series, too). Since 16 and 20 are in both of those ranges, if you "tune" the internal oscillator, and then select a "tuned" speed when uploading, you can upload through optiboot and get 16 or 20 MHz as desired, regardless of what speed you selected when bootloading it. See the [tuning guide](https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/Ref_Tuning.md).

## Upload using Programmer is not supported
Optiboot boards will not work if a sketch is uploaded via UPDI. Only two schemes are supported by this core for getting a compiled binary onto the chip:
* Use UPDI to upload the sketch (this is the non-optiboot configuration). You must have a board definition without (Optiboot) in the name selected.
* Use UPDI for bootloading only. Upload sketches with serial only. You must have a board definition with (Optiboot) in the name selected.

I am hoping to find a way to output an error more gracefully, but in 2.5.0, the solution results in the top of the error being something like this (of course, this line isn't shown for a Dx-series part, but I think the idea is clear).
In white text:
```text
ERROR: Upload using programmer is not supported on optiboot boards""C:\arduino-1.8.13\hardware\tools\avr/bin/avrdude -CC:\Users\Spence\Documents\Arduino\hardware\megaTinyCore\megaavr/avrdude.conf -v -pattiny3216 -cjtag2updi -PCOM11 -Ufuse2:w:0x02:m -Ufuse6:w:0x04:m -Ufuse8:w:0x02:m -Uflash:w:C:\Users\Spence\AppData\Local\Temp\arduino_build_10043/sketch_dec10a.ino.hex:i
```
In orange "error text":

```text
java.io.IOException: Cannot run program "ERROR: Upload using programmer is not supported on optiboot boards\"\"C:\arduino-1.8.13\hardware\tools\avr/bin/avrdude": CreateProcess error=2, The system cannot find the file specified
```
I hope this is clear enough.

## Upload speed
It's actually not bad...

Programmer      |   Read    |  Write    | Notes                                    |
----------------|-----------|-----------|------------------------------------------|
jtag2updi       |  6.6 kb/s |  5.9 kb/s | Running on 16 MHz Nano                   |
Curiosity Nano  |  5.9 kb/s |  3.3 kb/s | Via avrdude - which is likely (hopefully!) not ideal  |
Optiboot Dx     | 10.6 kb/s |  6.9 kb/s | 115200 baud as supplied by DxCore        |
SerialUPDI      |  8.8 kb/s |  8.8 kb/s | 115200 baud, any adapter                 |
SerialUPDI      | 16.3 kb/s | 16.4 kb/s | 230400 baud, any adapter                 |
SerialUPDI      | 23.7 kb/s | 24.3 kb/s | 345600 baud, FT232RL                     |
SerialUPDI      | 32.7 kb/s | 24.5 kb/s | 460800 baud, HT42B534 optimizations      |

Note that jtag2updi is fully baud rate limited - it has to take a page at 115200 baud, the maximum supported by AVRdude, and then stop receiving more data, and focus on the software half-duplex serial port it uses to communicate over UPDI.

Those numbers were take for a full 128k upload to a DA part. The 10.6k read is running at the nearly the speed of the wire (for SerialUPDI, with the wonky low latency HT42B534 adapters, you can hit 9.1, which is the wire speed for UPDI at 115200, but as you can see, the normal adapters were only a whisker behind). The "wire speed" is different because UPDI has 2 stop bits instead of 1, and a parity bit, so it's 12 bit/byte instead of 10.  There is a clear path to improving Optiboot write performance which I calculate will bring the speed up to about 9.5 kb/s write, and should work at 230400 baud were it not for the fact that AVRdude on some platforms is capped at 115200

Bearing that in mind, the planned enhancement would bring the write speed up to about the same as the read speed, but because of the cap imposed by AVRdude, that's going to be around 10kb/s, meaning 26-30 seconds to write + verify all told for maximum size sketch (it could be half that without the avrdude speed limit).

I've been asked about supporting the no-verify option. Since there is literally no check on whether an upload succeeded other the presence or absence of gross protocol errors and the verification step (with either Optiboot or SerialUPDI, I don't think I can endorse this option. The same is true on all other AVRs, and *I think it is foolish and irresponsible that the Aruino team offers that option to users at all* - at least on AVR. Maybe other platforms with other upload protocols have CRC checks and othher such basic safeguards. But AVR provides no such assurance.

## Entry Conditions
Depending on your needs, you may want or need different configurations for when the bootloader should run and when it should just jump straight to the application. It determines this with the reset flags, which it clears, and then stashes in GPR.GPR0 right before jumping to app. It doesn't clear them until it jumps to app, so you do get to see what kind of reset was involved. We supply two versions, the "1-second autoreset" and the "8-second non-autoreset" version.

| Version  | UPDI reset (UPDIRF) | Software Reset (SWRF) | WDT reset (WDRF) | External (EXTRF) | Brownout (BORF) | Power On Reset (PORF)| None        |
|----------|---------------------|-----------------------|------------------|------------------|-----------------|----------------------|-------------|
| 1-second | Yes                 | Yes                   | Never            | Yes              | No              | No                   | Reset again |
| 8-second | Yes                 | Yes                   | Never            | Yes              | No              | Yes                  | Reset again |

As can be seen, since software can trigger either a WDT reset or a SW reset, it is possible to choose to reset into the bootloader or reset bypassing the bootloader in that way.

The bootloader will wait for 1 or 8 seconds for an upload if it resets and the entry conditions are met:
* If a configuration with a reset pin was selected, the `_rst` version will be installed. It will run after a hardware (reset pin), software reset (see above), or UPDI-reset (ie, immediately after bootloading) only.
* Otherwise, it will run after any of those, as well as power on reset.
* A brownout which causes the BOD to trigger, but where the voltage on the power rails does not fall below approx. 1.25v (which would trigger the "POR rearm") is NOT an entry condition. Yes, this could be from someone attempting to power cycle it to start the bootloader and underestimating how long their capacitors would hold a charge for, which is a time when we would want to enter the bootloader - however, it could also indicate a flaky power supply or weak batteries, in which case we probably don't want to enter the bootloader.
* A software reset is always an entry condition. If you need a reset from software that doesn't run the bootloader, trigger a watchdog reset instead (covered in same section of this readme as software resets).
* A watchdog reset is never an entry condition. The WDT is how Optiboot resets when it's either timed out (actually, the thing that times out *is* the watchdog timer) or finished uploading (at which point it sets timeout to minimum and waits for reset).

These reset flags are stashed in GPR.GPR0 - if you use the GPR's be sure to read any information you need from this before writing over it. We then clear the reset flag register, so that next reset, we know what caused it and can take the appropriate actions.
The WDRF flag will ALWAYS be set when the bootloader has run, and the entry condition was met: it will set up the WDT for the specified timeout and wait for recogtnizable characters to arrive. If they do not, the WDT reset ensures that the chip is reinitialized, and it can jump to the app with a clean slate. If the WDRF is set, the reset must have come from the WDT - otherwise it would have been cleared previously - and that is not an entry condition. It is intended that one of the two methods of software reset will trigger the bootloader, and the other (WDT reset) will not, allowing user code to reset cleanly without having to wait through the bootloader timeout.
Provided that WDRF is not set we will always run after a UPDI, software, or reset pin reset. Brownouts also won't trigger it (though if accompanied by PORF, the 8-second one will, as that is a natural symptom of a slow rising power supply). The 8-second one does run on power on (so you can run the bootloader by power-cycling it if you can't reset it).

Finally, if exeecution ever arrives without any reset cause, we will fire a software reset, at which point it will meet the SWRF entry condition and wait the specified time for an upload. We do an additional reset here because this is generally an error condition - either your code jumped off into empty flash amd wrapped around, or you triggered and interrupt with no ISR. A lot of C's "undefined behavior" will result in this sort of "dirty" reset, where it starts executing from 0x0000 - *but hasn't actually been reset* and all the peripherals are set up however your application set them up before it crashed. Both the bootloader and the core assume that peripherals are in their power on state when the code starts running.
## Bootloader size
There is a critical difference between the classic AVRs and the modern ones, and this one I think is one of the few places they may have made a poor decision. On classic AVRs, the bootloader section was at the very end of the flash. When it was enabled, the chip would jump to there on reset instead of starting at 0x0000, the bootloader would do it's thing, and then get to the application code by jumping to 0x0000. That way, there was no need to know whether it was a bootloader or non-bootloader configuration at compile time. Now, the bootloader is at the start of the flash, and jumps to the end of it's section. Hence, the compiler needs to know to offset everything by 512 bytes (or more for other theoretical bootloaders), and now you cannot use optiboot to upload a binary compiled for non-optiboot mode, nor the other way around. You cannot translate a hex file compiled for optiboot to non-optiboot or vise-versa.

### Differences in binaries (technical)
When the bootloader is enabled the interrupt vectors are located at the start of the application section, 512 bytes in (like megaAVR 0-series and tinyAVR 0/1-series, and unlike classic AVRs, the bootloader section is at the beginning of the flash). Options to set the desired USART that the bootloader will run on are available for all serial ports, with either the normal or alternate pin locations. USART0 with default pins is the default option here, and these are the ones that are connected to the 6-pin serial header on the DA-series breakout boards that I sell. An option is provided when burning bootloader to choose an 8 second delay after reset - use this if you will be manually pressing reset.

One's first thought on hearing about the binaries being different might be whether they really are different enough that you have to recompile - couldn't you just say, offset the non-bootloader one 512b, maybe with a simple python script? Unfortunately lots of instructions use absolute addresses. Even if you caught all the jmp and call instructions, that wouldn't solve the issue of every reference to a flash address, every jump table, and so on. Export compiled binary does generate files with different names for optiboot and non-optiboot, so it's easy to export both versions.
There are also a couple of small differences between what is generated for bootloader and non-bootloader boards. With a bootloader, we don't need to put in a guard function for handling of empty reset cause (see the [reset guide](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Reset.md), since Optiboot handles that. Writing to flash also has very different requirements depending on the presence or absence of a bootloader, and in the latter case, how much flash you want to be able to write. With Optiboot it is simple: The SPM instruction must be located in the boot section, that's the first page of flash, and at the end, just before the version numbers, I place two additional instructions - a SPM Z+ (write word to flash and increment Z pointer) and RET (return). So in this case, you set up r0 and r1 with inline assembly, and then instead of calling SPM yourself (since you're in the application section), you can just call that address - it will be in the bootloader section, so it will be allowed to write to the application,and then it will immediately return. (In product where untrusted code would be involved, don't worry - there's a bit you can set in the NVMCTRL registers that turns off the ability to read from or execute anything from the boot section; it can only be set from the bootloader, and only cleared by a total reset. So a bootloader that set that bit before jumping to the app could never be used to write to flash from the app - all it would see is NOOP instructions there if it tried). Making it work without a bootloader is trickier because the you still have to define a boot section (otherwise it treasts all of the flash as boot flash, which you can't write except over UPDI), but have it actually act as part of the app, setting the vectors to point to the start, not 512b in - and for unresdtricted writes, we also have to provide an SPM routine to call and force the compiler to put it in the right place (despite the compiler's vehament objections). So in that case, 3 different implementations of the core of that library are used depending on whether Optiboot is used.

## Be sure you are using the latest bootloader version
Between the initial release and 1.3.0, substantial improvements were made to the bootloader. It has not been altered since ~and we forsee no further changes for DA and DB~ There is, potentially, one major improvement left. Currently, we buffer a page, and then write it between commands. But we don't need to do that. We can write it as it comes in! The only restraint is that we need to have the word written before the next word arrives, and I was too cautious initially. We have a 2 byte "buffer" consisting of the RXDATA register and the double buffer byte behind it, plus the shift register. At 115200 baud, we have about 90 us per byte, and it takes 70 us to write two bytes - so we have plenty of time, we could even double the speed and still do it, and the time and flash consuming page buffer could be eliminated and replaced with on-the-fly writes. This would free up some space (probably not enough for EEPROM writes though), and improve performance dramatically for large uploads.

## Future Devices
The DD-series is essentially a cut-down DB-series, without the OPAMPs. NVM programming woks he same way, and Optiboot versions will be available not long after release of the hardware.

The EA-series is more tinyAVR-like and that will be the codebase we use to developt Optiboot-EA (or Ex) from. Until the datasheet is released, it is not clear how different the process of writing will be on account of the RWW - I don't think we can get much if any performnacnce boost from it, so it will just be a hindrance if anything.
