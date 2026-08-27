# The USB CDC bootloader for the AVR DU-series (usbcdcboot)

The DU-series has native USB, so unlike every other Dx/Ex part, it can be bootloaded without a serial adapter: select the **AVR DU-series (USB CDC Bootloader)** board,
plug the part's USB port into the computer, and upload. This document is the user-facing reference, the counterpart of [the Optiboot reference](Ref_Optiboot.md) for the classic serial bootloader.
Developer documentation (design, build instructions, provenance) lives with the sources in [`megaavr/bootloaders/usbcdcboot/`](../bootloaders/usbcdcboot/).

## What it is

A small (4 KB boot section) USB CDC-ACM bootloader speaking the same STK500v1 protocol as Optiboot, so avrdude uploads work the usual way.
It is a clean-room implementation written from the USB 2.0 specification and the DU datasheet (DS40002548A) - see `PROVENANCE.md` next to the sources.

While the bootloader is active it enumerates as a CDC serial port with VID/PID `0x1209:0x0001`; a running sketch enumerates as the application CDC with `0x1209:0x0002`.
(These are pid.codes test IDs, to be replaced for release.) The board definition registers both, so "Get Board Info" recognizes the device in either state.

## Burning it

"Burn Bootloader" with a UPDI programmer writes the bootloader hex and the fuses (including `BOOTSIZE = 8`, i.e. a 4 KB boot section).
After that, no programmer is needed for day-to-day work. As with Optiboot, uploading via "Upload Using Programmer" (UPDI) erases the chip,
bootloader included - re-burn it if you want USB uploads back.

Prebuilt hex files live in `megaavr/bootloaders/hex/` - one per flash size, with `_novreg` variants for boards that feed external 3.3 V into VUSB instead of using the internal regulator (this matches the "VUSB Power Source" tools menu).

## Entry conditions

On every reset the bootloader decides between staying resident (USB active, waiting for an upload) and jumping to the application:

* **1200 bps touch** - when the host opens the application's CDC port at 1200 baud and drops DTR (which is what avrdude does at the start of an upload),
the running sketch's USB stack writes a magic word, detaches from USB, and triggers a watchdog reset. The bootloader sees the magic word and stays. This is the normal, hands-free upload path.

* **Reset button** - an external reset (RESET pin, `EXTRF`) enters the bootloader. Useful when the sketch has crashed or its USB stack is not functional.
* **Empty application** - if the application reset vector reads as blank flash (`0xFFFF`), the bootloader stays, so a freshly bootloaded part is immediately uploadable.

Any other reset cause (power-on, brown-out, software reset, watchdog reset without the magic word) starts the application directly, so a deployed device does not sit in the bootloader after a power blip.

There is no timeout in the stay state: once entered, the bootloader waits until an upload arrives or the part is reset.

## LED

While (and only while) the bootloader is resident, it drives an indicator LED - by default PA7, active LOW, matching DxCore's Optiboot LED convention (`LED=A7`) on the 20/28/32-pin DU packages.
Since this is a USB bootloader, the LED never collides with a UART pin position. Builds for the 14-pin parts use a different pin (see the build scripts).

## Writing to the flash from the app

The last page of the boot section contains an app-callable SPM stub, and the last two bytes hold a bootloader version word - the same general scheme Optiboot uses,at different addresses.
The Flash library supports this out of the box when the board is the USB CDC Bootloader one (`USING_AVRDU_CDC_BOOTLOADER` is defined): `Flash.writeBytes()` and friends work from application code, with the boot section itself protected.

## Differences from Optiboot in practice

* No serial adapter, no autoreset circuit, no DTR capacitor: the USB cable is the whole story.
* Upload speed is not a menu option - USB CDC ignores the baud rate.
* The bootloader does not run on any UART, so all USARTs remain fully available to the sketch.
* Sketches start at 0x1000 (4 KB in) instead of 0x200; the board definition accounts for this automatically.
