# **EEPROM Library V2.1.3** for Modern AVRs

**Written by:** *Christopher Andrews*.
**Ported and updated by:** *Spence Konde*.

## What is the EEPROM library?

The EEPROM library provides an easy to use interface to interact with the internal non-volatile storage found in AVR based Arduino boards. This version provides support for the tinyAVR 0/1/2-series, the megaAVR 0-series, and the AVR Dx-series, and is expected to work with the upcoming AVR Ex-series without modification.

It can be used exactly like the one included with the standard Arduino AVR core.

This is the documentation for the version included with with DxCore and megaTinyCore, and discusses several things applicable only to those platforms. Libraries that perform the same function and present the same API are included with virtually every Arduino core.

## When is EEPROM erased?
1. When a sketch manually erases some or all of it.
2. IF using a non-optiboot configuration, it can optionally be erased every time new code is uploaded. This is controlled by the EESAVE fuse bit. On AVR DA and DB parts, this is a \"safe\" fuse and is set on all uploads. On AVR DD and ATTiny parts, it is not considered a safe fuse, since it is on the same fusebyte that can disable non-HV UPDI programming; on those parts you must do \"burn bootloader\" to apply these changes. Note that according to recent datasheet clarifications, on a locked chip, EESAVE is will preserve the EEPROM - at least on those chips. In the incredibly unlikely event that this matters to you, check the relevant eratta and datasheet clarification document.

See also the [USERSIG](../USERSIG/README.md) library which writes to the rather similar memory section known as the USERROW (aka \"user signature space\"), which is only erased if manually erased or if a locked chip is erased to unlock it (that will always restore the flash, EEPROM, and USERROW to blank state to protect proprietary or confidential information from leaking). Note that there are significant differences in the USERSIG library on tinyAVR and AVR Dx-series parts due to underlying differences in the NVM controller; on tinyAVR the library presents an identical interface to EEPROM. On DxCore, it is necessary to call an additional function to commit the new data if an erase is required. See the linked readme file for more information.

## How to use it
The EEPROM library is included with all hardware packages for hardware with that functionality (which is almost universal). Depending on the hardware, however, the implementation may be very different. This version is designed for megaTinyCore and DxCore (The same write )

**Under no circumstances should you take this library out of the core and put it into your libraries folder** as doing that will break the EEPROM library for every part definition not supplied with this core.

```Arduino
#include <EEPROM.h>

void setup() {

}

void loop() {

}

```

The library provides a global variable named `EEPROM`, you use this variable to access the library functions (As of library version 2.1.4 we have eliminated unused variable warnings. The methods provided in the EEPROM class are listed below.


## EEPROM Sizes

| AVR Device                          | EEPROM size | Erase+Write time | Has USERROW | Has BOOTROW |
|-------------------------------------|-------------|------------------|-------------|-------------|
| tinyAVR 0/1-series 2k flash         |         64b |             4 ms |         32b |          No |
| tinyAVR 0/1/2-series 4-8k flash     |        128b |             4 ms |         32b |          No |
| tinyAVR 0/1/2-series 16-32k flash   |        256b |             4 ms |         32b |          No |
| megaAVR 0-series (all flash sizes)  |        256b |             4 ms |   32 or 64b |          No |
| DA, DB-series (all flash sizes)     |        512b |            11 ms |         64b |          No |
| DD-series (all flash sizes)         |        256b |            11 ms |         64b |          No |
| DU-series (all flash sizes)         |        256b |            11 ms |        512b |        256b |
| EA-series (all flash sizes)         |        512b |             4 ms |         64b |          No |
| EB-series (all flash sizes)         |        512b |             4 ms |         64b |         64b |

The USERROW and BOOTROW are two special sections of memory. The USERROW is the one that can be written via UPDI when the chip is locked as well as written by user code (see the [USERSIG](../USERSIG) library for more information on writing from user code, or the datasheet for more information on writing via UPDI to a locked chip.



Specifying an address beyond the size of the EEPROM will wrap around to the beginning. The addresses passed to EEPROM functions are a `uint8_t` (aka byte) on parts with up to 256b of flash and a `uint16_t` (word or unsigned int) on parts with more.


You can view all the examples [here](examples/); disclosure and disclaimer I did not write the examples, and have some doubts

## Warning: Using EEPROM right at startup
On the modern tinAVR devices (but not with any Dx-series parts) we have received at multiple reports from users of erratic failures to correctly interact with the EEPROM immediately upon startup. There is considerable evidence that the cause of the problem was a slow-rising power supply, coupled with the specific brownout detection configuration. This issue is still not entirely understood, but it is suspected that it ends up doing the write very close to it's minimum voltage, when the chip may be running out of spec because the chip had by that point switched to it's full clock speed (and BOD is forced on during NVMCTRL operations. Try to avoid writing to the EEPROM immediately upon startup - maybe pick a longer SUT (startup tme), or simply wait until later into execution to perform the write, etc. Many times the impacted individuals found that even a delay of a few milliseconds was sufficient to ensure that it worked (Issue #452). A more rigorous approach is to measure the voltage before writing and make sure it is within the intended operational range.


## Library functions

### `EEPROM.read(address)` [[*example*]](examples/eeprom_read/eeprom_read.ino)

This function allows you to read a single byte of data from the eeprom.
Its only parameter is an `int` which should be set to the address you wish to read.

The function returns an `unsigned char` containing the value read.

### `EEPROM.write(address, value)` [[*example*]](examples/eeprom_write/eeprom_write.ino)

The `write()` method allows you to write a single byte of data to the EEPROM.
Two parameters are needed. The first is an `int` containing the address that is to be written, and the second is a the data to be written (`unsigned char`).

This function does not return any value.

### `EEPROM.update(address, value)` [[*example*]](examples/eeprom_update/eeprom_update.ino)

This function is similar to `EEPROM.write()` however this method will only write data if the cell contents pointed to by `address` is different to `value`. This method can help prevent unnecessary wear on the EEPROM cells.

This function does not return any value.

### `EEPROM.get(address, object)` [[*example*]](examples/eeprom_get/eeprom_get.ino)

This function will retrieve any object from the EEPROM.
Two parameters are needed to call this function. The first is an `int` containing the address that is to be written, and the second is the object you would like to read.

This function returns a reference to the `object` passed in. It does not need to be used and is only returned for convenience.

### `EEPROM.put(address, object)` [[*example*]](examples/eeprom_put/eeprom_put.ino)

This function will write any object to the EEPROM.
Two parameters are needed to call this function. The first is an `int` containing the address that is to be written, and the second is the object you would like to write.

This function uses the *update* method to write its data, and therefore only rewrites changed cells.

This function returns a reference to the `object` passed in. It does not need to be used and is only returned for convenience.

### Subscript operator: `EEPROM[address]` [[*example*]](examples/eeprom_crc/eeprom_crc.ino)

This operator allows using the identifier `EEPROM` like an array.
EEPROM cells can be read *and* **written** directly using this method.

This operator returns a reference to the EEPROM cell.

```c++
unsigned char val;

// Read first EEPROM cell.
val = EEPROM[0];

// Write first EEPROM cell.
EEPROM[0] = val;

// Compare contents
if(val == EEPROM[0]) {
  // Do something...
}
```

### `EEPROM.length()`

This function returns an `unsigned int` containing the number of cells in the EEPROM.

---

## Advanced features

This library uses a component based approach to provide its functionality. This means you can also use these components to design a customized approach. Two background classes are available for use: `EERef` & `EEPtr`.

### `EERef` class

This object references an EEPROM cell.
Its purpose is to mimic a typical byte of RAM, however its storage is the EEPROM.
This class has an overhead of two bytes, similar to storing a pointer to an EEPROM cell.

```C++
EERef ref = EEPROM[10]; // Create a reference to 11th cell.

ref = 4; // write to EEPROM cell.

unsigned char val = ref; // Read referenced cell.
```

### `EEPtr` class

This object is a bidirectional pointer to EEPROM cells represented by `EERef` objects.
Just like a normal pointer type, this type can be dereferenced and repositioned using
increment/decrement operators.

```C++
EEPtr ptr = 10; // Create a pointer to 11th cell.

*ptr = 4; // dereference and write to EEPROM cell.

unsigned char val = *ptr; // dereference and read.

ptr++; // Move to next EEPROM cell.
```

### `EEPROM.begin()`

This function returns an `EEPtr` pointing to the first cell in the EEPROM.
This is useful for STL objects, custom iteration and C++11 style ranged for loops.

### `EEPROM.end()`

This function returns an `EEPtr` pointing at the location after the last EEPROM cell.
Used with `begin()` to provide custom iteration.

**Note:** The `EEPtr` returned is invalid as it is out of range (this is the standard behavior required by the aforementioned programming techniques. Note that on 256b EEPROM parts, the EEPtr returned cannot be distinguished in any way from a pointer to address 0 as the address is represented by an 8-bit value, which may cause code that uses it to behave in unexpected ways). I have not heard of any real-world code using this call.

### `EEPROM.getStatus()`
Returns a status indicating the state of the NVM controller, and, if an error was reported clears the error status.

Useful for debugging problems writing to EEPROM.

## Very advanced considerations
Because we have people using megaTinyCore and DxCore to write code that will be deployed to a production environment, these considerations had to be addressed.

### EEPROM endurance
The EEPROM write endurance is not infinite. The headline spec for all parts supported by this library is 100,000 writes (compared to 10,000 for the flash). For the DB-series a second figure is given, the number of \"erase/write cycles before refresh\" whatever the heck that means. It's spec'ed at 1 million minimum, typical 4 million. I'm not sure how to reconcile those two numbers and there is no description of how N<sub>D_REF</sub> differs from E<sub>D</sub>.

### Writing to EEPROM from within an interrupt (ISR) can make millis lose time
Writing to the EEPROM is very slow compared to basically anything else a microcontroller does, per the table above (the specifications are disturbingly vague for the Dx-series)
During an Interrupt Service Routine (ISR), like a function that is executed as a result of attachInterrupt(), or one defined with `ISR()`, all interrupts are disabled (unless one has been marked as priority level 1; the core does not ever do this by default). That includes the millis timekeeping interrupt... and leaving interrupts disabled for longer than (typically) 1ms will result in `millis()` and `micros()` losing time. This will never happen for a single isolated byte write, since the CPU is only halted if an attempt is made to write the EEPROM while another write is already in progress.

#### On DxCore and with the current version
This library verifies that there is no EEPROM write in progress, disables interrupts, and then writes to the EEPROM and restores SREG turning interrupts back on unless they were already disabled globally. Hence, there will never be any millis time lost when writing a single byte, nor when writing more than one byte at a time (ex, using put with a multibyte value) outside of an ISR. Put simply it now cannot happen if all EEPROM writes are made from a normal (non-interrupt, interrupts not disabled) context. If the main application is writing to the EEPROM and, an extremely poorly timed interrupt that *also* writes to the EEPROM is triggered within an extremely narrow window, this could result in losing up to 10ms (DxCore, if we trust the 11ms figure) or 3ms (megaTinyCore) (this window is around 3 clock cycles, in the middle of EEPROM.write() between when we check the NVMCTRL.STATUS, and when we disable interrupts).

When more than 1 byte is written from a single interrupt (regardless of whether the bytes are done as part of a larger value or not), it will always lose time - up to 11ms or 4ms per byte after the first, less 1-2 times the millis resolution (typically 1ms, see the detailed [timer usage documentation](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/Ref_Timers.md) for details).

Regardless of whether it was caused by an interrupt writing more than one byte, or an interrupt that writes one byte happening while the main code is attempting to write to the EEPROM, it will impact both millis() and micros(), but will never produce backwards time travel relative to values from before the interrupt fired. Immediately upon that interrupt and hence interrupts becoming available again, the millis timekeeping interrupt will fire and try to sort things out, and micros will jump ahead by the millis resolution (ie, by 1000 if millis resolution is 1ms, regardless of how long it was blocked from running while the EEPROM-writing interrupt was running), and timekeeping will proceed normally.

#### On megaTinyCore in earlier versions
The avr-libc runtime included with the toolchain provides an eeprom_write_byte() function, implemented in assembly. It does not disable interrupts. During a very brief (couple of clocks, maybe just 1) window between writing to the page buffer and starting the protected write sequence, if an interrupt fired and wrote to the EEPROM, it could malfunction badly and either corrupt a byte near the interrupt's write destination or fail to write at all depending on the addresses involved. Prior to library version 2.1.2 (megaTinyCore 1.3.4 and earlier) we used that implementation. We no longer do. It has been reimplemented more robustly and now has the same behavior as DxCore.


### EEPROM corruption due to low supply voltage
If the power supply voltage is insufficient (as in, below operating specifications) while writing to the EEPROM, corruption can result; In addition to the case discussed earlier, where a slow rising power supply and an early EEPROM write conspire to yield a corrupted value being written, (which users have encountered on tinyAVRs with the BOD off when writing the EEPROM immediately at power on) this includes the case where power is cut entirely while writing to the EEPROM in addition to transient brownouts, which are likely a less frequent cause of this sort of issue). This phenomenon is described in the \"Preventing Flash/EEPROM corruption\" section (9.3.3) of the datasheet. Enabling the Brown-Out Detect functionality (\"burn bootloader\" required after changing those settings to apply them) will prevent this by holding the part in reset when below a minimum supply voltage. Even better would be to use the VLM to detect when the operating voltage is perilously close to the BOD threshold.

Alternate methods of ensuring that there is not expected to be a power failure within the next 11 ms (DxCore) or 4ms (megaTinyCore) might include monitoring the supply voltage with the ADC. It is also important to practice good hardware design: particularly, you should have sufficient board-level decoupling capacitors to make sure the power can't drop out so quickly that a write might not finish. And if user action is the likely cause of the power loss, one could flash an LED while writing or similar.

#### For DxCore in particular
In extreme cases - if your application **must** save state in the event of unexpected power loss, your best hope might not using EEPROM, but FLASH! EEPROM.h uses the EEEWR mode, which while it offers excellent write endurance, is very slow, potentially as long as 11ms/byte. Flash, on the other hand, can write 2 bytes in just 70 us. If you were to keep a page of flash pre-erased and ready for a panic dump, you would be able to save far more via Flash.h and the VLM interrupt. You could then check for it on startup, and, if you found anything, copy it somewhere else and erase the page again to be ready in the event of the next crash. An example of this may be added approximately when I have a chance, likely not for a while.

## Debugging \"The EEPROM didn't write\"
* Don't wry to write to the EEPROM within the first few millisecondsafter power is applied.
  * On modern AVRs, it can definitely fail to write for some reason when you try to write to EEPROM immediately at startup; the exact reason is not clear, but it's the sort of bug that is much more likely rto manifest in testing than real-world use
    * Classic AVRs did this readilly enough and frequently enough (arduino was spared likely because they shippe dwith a bootloader, so by the time it reached the app the risk had passed) that the intermittent, debug nightmare class problem was widely understood - it happened when you tried to write to the flash, with an excessively short SUT selected and the BOD disabled when using a weak or slow rising power supply,or a suitable power supply with insufficient decoupling, the chip would reset during the write, and following the write **but the write would continue**; as the address register was reinitialized by the droop induced reset (or lost due to the voltage drop itself), so it would erase a different section of the eeprom than you were trying to erase.
  * That horror show is not possible on the modern AVRs, but failed writes still are possible. They force the BOD on at the minimum level on Dx and Ex parts when writing to the NVM.
* Make sure that you have all decoupling capacitors (0.1uF), and at minimum 1 uF of board-level decoupling. I usually use 4.7 uF on the board.
* Make sure you are operating the chip within spec.
* Do you have any ISRs writing to the NVM? (library is believed secure to everything except lost millis time during the write. )
* Does the NVM controller report any error conditions?
  * EEPROM.getStatus() will return the NVM controller status, and if there were any clearable error codes, they will be cleared; some error conditions will blockfuture attempts at NVMCTRL use if not properly cleared. If getStatus finds either of the command-code related errors, it will additionally set CTRLA to 0. If the attempt to write works

### `EEPROM.getStatus()` (new 8/25)
Returns a uint8_t;

On tinyAVR, this is simply NVMCTRL.STATUS; No bits are cleared, WERROR is read-only, but doesn't effect future attempts.
```text
0b000 = Neither flash nor eeprom is busy, and the last write, if any, was successful.
0bxx1 = Can't-happen. When flash is busy on these parts, CPU is halted, which makes it impossible to read the register when the flash is busy.
0bx10 = EEPROM in process of writing, no error recorded.
0b100 = Last NVM write (could be flash, EEPROM, or USERROW) failed. No further write attempt is in progress.
0x110 = Last NVM write failed, and another write has been started.
```

On Dx and Ex, the documentation indicates that the errors in STATUS need tobe cleared prior to further programming attempts, and that this can be done by writing 0 to them. Since all the other non-reserved bits are read-only, it should be safe to simply set the register to 0 to clear the bits (we don't calculate it explicitly), and return the value to the user so they can see what (if any) errors had been cleared.

The return values on Dx/Ex are:
```text
0b0000 0000 = Neither flash nor eeprom is busy. No relevant bits set.
0b0000 xxxx = No errors were recorded on the last attempted write.
0bxxxx xxx1 = EEPROM is currently being written. Starting other NVM writes will halt the CPU until it's completed the first operation.
0bxxxx xx1x = Flash is currently being written (Ex-only - Dx halts CPU during writes). Further NVM access attempts will halt the CPU until the outstanding write is true on these parts. Dx-series parts will never indicate this, as the CPU is always halted during write, and you can't read from a register while the CPU is halted.
0bxxxx x1xx = UROWWP is currently set, and the USERROW is write protected. You must make sure not to set this bit (which is reset to 0 during any reset) if you wish to write to the USERROW (User Signature Space).
0bxxxx 1xxx = EEWP is currently set, and the EEPROM is write protected. You must make sure not to set this bit (which is reset to 0 during any reset) if you wish to write to the EEPROM
0b0001 xxxx = A value that was not a valid command was written to the CTRLA register.
0b0010 xxxx = An attempt was made to write to a section that is write protected
0b0011 xxxx = A command collision has occurred due to failure to correctly handle the command resister (this library should not cause this to become set).
0b0100 xxxx = An attempt was made to write an address that is invalid for the address used to access it (using the Erash Flash command on addresses that aren't flash ())
0b0xxx xxxx = (not listed above) Unknown error that is not generated by known parts.
0b1xxx xxxx = Can't happen
```

In the event of errors, we clear the errors.

If the low error bit is set, (indicative of a problem with the command used), in order to return the NVMCTRL module to a usable state, this function will, in addition to clearing the error flags, reset CTRLA to 0/NOOP/None
#### Outputs, described differently

##### tinyAVR
```text
0b000 - Nothing going on here
0b001 - Writing EEPROM.
0b100 - Whatever the last thing you tried to write to NVM was (Flash, EEPROM, USERROW, BOOTROW), you or the library did "something" wrong.
0b101 - Last NVM write attempt failed. Will this one work? It will change to either 0b000 (success) or 0b100 Failure
0bx1x - Hold on, what chip am I actually working on?
Any higher bit set - You're definitely not working on a tiny, nor are you compiling for one...
```
##### Dx or Ex
You're writing the EEPROM if using this library, so bit 2 is of no interest, and bit 7 isn't used
```text
0b0000 0x00 - Everything looks totally normal. No NVM access ongoing.
0b0000 0x01 - As above, EEPROM write ongoing.
0b0000 0x10 - As above, flash write ongoing, and this is an EA or EB-series part.
0b0xxx 1xxx - EEPROM writes are not possible because somehow, bit 3 of NVMCTRL.CTRLB was written 1.
  Figure out how that is happening - narrow down when, eg try getStatus() at the start of setup.
  If that's still set - well, there are ways that libraries can run stuff before setup. You should be able to confirm  on a brand new empty sketch - that it won't be set, and then add back in libraries until you find the offender.
  The only way it could be set if all you do is setup serial and printEEPROM.getStatus(). If that still reports it set, and you're using a bootloader, recheckwith UPDI programming. This willwork as expected (as will our bootloader) set you're using a bootloder, and not the version we ship with, and should complain to the provider of that software.
0b0001 0x00 - An invalid command was passed. If a library was used to attempt the write, report bug to them.
0b0010 0xxx - Write Protect - an attempt was made to write to a writeprotected or invalid location.
0b0010 1xxx - (EB and later only) The expected result of attempting to write EEPROM when the EEWP bit is set (write protect error. EEWP is cleared on reset, so your sketch or bootloader set it. )
0b0011 0xxx - Command Collision - command was not cleared prior to attempting to set the new command as required. Write should now succeed (but something else involving the NVM is being done and leaving it in an inappropriate state. If this came from an eeprom write with this library, please report it posthaste.
0b0100 0xxx - (EB and later only) There is now a status code for "Wrong Section". This should only be encountered when developing code and writing your own asm to issue the programming instructions. See comment.
```

That last error, as far as I can tell, is issued when the user code attempts to write to the EEPROM or USERROW (which can only be written with ST (Store), not SPM (Store Program Memory)) using the SPM instruction. As I read the NVMCTRL documentation, on the EB, either ST or SPM can be used to write flash, but only ST can be used for writing EEPROM. If you wrote your own routine for flash or eeprom writes, you might encounter this when testing it. It should never be seen by normal people.

#### Usage
``` c++
// First, before we attempt the wonky write, get the status to see if it might be in a bad state,
Serial.print("Initial - expect 0x00 if no NVM use hasoccurred: ");
Serial.printHexln(EEPROM.getStatus());
EEPROM.write(/*your args here*/); //doesn't seem to be working
uint8_t temp = EEPROM.getStatus()
Serial.print("Post-write: Expect 0x02 - the byte write in process");
Serial.printHexln(temp);
// if the NVMCTRL_ERROR groupmask is defined, we know it has the new style error flags, which we're supposed toreset.
#if defined(NVMCTRL_ERROR_gm)
  if (temp & NVMCTRL_ERROR_gm) {
    Serial.println("A write error occurred!");
  } else {
    Serial.println("No write error occurred!");
  }
#else
  if (temp & NVMCTRL_WERROR_bm) {
    Serial.println("A write error occurred!");
  } else {
    Serial.println("No write error occurred!");
  }
#endif

// For goodmeasure:
  delay(20); //write easiuly done by now.
  Serial.print("Now we should get a 00 since it should be done: ");
  Serial.printHexln(EEPROM.getStatus());
```
