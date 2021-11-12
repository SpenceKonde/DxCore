# USERSIG Library V2.1.0 - USERROW access for AVR DA, DB, and DD-series parts

**Written by:** _Spence Konde_
**Based on EEPROM.h by:** Christopher Andrews

## What is the USERSIG library

The USERSIG library provides an easy to use interface to interact with the so-called "user row" or "user signature" - this is a section of non-volatile memory like the EEPROM, except that it maintains it's values through a chip-erase... even if EESAVE is not set. This library is named USERSIG because USERROW is the name of a builtin - it included in the io headers (low level files containing register names and the like). That provides no facility for writing or doing other things that you probably want to do with the USERROW memory, though.

## Not quite like EEPROM
Unlike the earlier parts, the Dx-series parts have a major disadvantage in terms of USERROW functions: The erase granularity on tinyAVR and megaAVR is 1 byte, and it works just like EEPROM, as does the library. Unfortunately, here, the USERROW acts more like flash - while you can write it one byte at a time (also possible on with the flash on these parts), you can only erase it one page at a time. The page size, for all Dx parts thus far is 32 bytes (ie, the whole thing). It remains to be seen how

I was told that write endurance is similar to flash, not EEPROM - so around 10,000 cycles minimum. A naive algorithm might check each byte the user asked to write against what was stored in the USERROW, and if writing it without an erase wouldn't result in the requested value, buffer it in RAM, erase and rewrite it. In some circumstances this would be fine, but in others, the writes could be amplified by as much as a factor of 32 (imagine writing a 32-byte fresh set of values, every one different from what was there - 32 erase and write cycles (well, 29 on average, if they're all random numbers, since about 10% the time, one random 8-bit value can be written over another random value without having to erase), because each byte is written without accounting for the fact that all 32 values are being replaced). The 10,000 cycle endurance has just become 312 cycles, which is not acceptable.

## Quick overview of the differences
In order to mitigate this, we have implemented a buffer, and "writes" which would require an erase are instead buffered. There are three new methods to manage this: one very important, one that is sometimes useful, and one that very rarely is (flush(), erase(), and pending()). The write() and update() methods are no longer different from each other and now have a return value that indicates whether that write was actually completed (if it didn't need an erase to write the value), or is in the buffer pending erase. Once there is one incomplete write pending, all writes go into the buffer until you call `USERSIG.flush()` to write it. Other methods of writing do not return that indication (they do whatever they do on the EEPROM library), and the only thing that will trigger an erase/write cycle other than calling `flush()` directly is using `put()` to store a value larger than 4 bytes (so most structs and objects, but not simple values) The rationale is that if you're writing a struct, you are probably already wrapping everything you intend to write into that.

There is also a new restriction: You are not allowed to write from an interrupt. This is not impossible, and would not be hard to support, but it comes at a flash and speed penalty that isn't negligible, and I don't think this is a common use case.

## USEROW Sizes

| AVR Device                          | USERROW size | Supported |
|-------------------------------------|--------------|-----------|
| tinyAVR 0/1/2-series                |          32b |        No |
| megaAVR 0-series (8k or 16k flash)  |          32b |        No |
| megaAVR 0-series (32k or 48k flash) |          64b |        No |
| DA, DB, DD-series (all flash sizes) |          32b |       Yes |
| EA-series (all flash sizes)         |          64b |   Not yet |

The tinyAVR and megaAVR devices are supported by the version of `USERROW.h` included with megaTinyCore.

Specifying an address beyond the size of the USEROW will wrap around to the beginning.

## Write Endurance
The USERROW write endurance is similar to the flash endurance according a well-placed source. There is no specification given in the datasheet.

## How to use it
The USERSIG library is included with megaTinyCore. To add its functionality to your sketch you'll need to reference the library header file. You do this by adding an include directive to the top of your sketch.

```Arduino
#include <USERSIG.h>

void setup(){

}

void loop(){

}

```

The library provides a global variable named `USERSIG`, you use this variable to access the library functions. The methods provided in the USERSIG class are listed below.

You can view all the examples [here](examples/).

## Library functions

### `USERSIG.read( address )`

This function allows you to read a single byte of data from the USERROW (if there are no pending writes) or the temporary buffer (if there are).
Its only parameter is an `int` which should be set to the address you wish to read (for the current parts this could have fit in a byte, but there was an announced part that had a much larger USERROW until they pulled down the product brief a few hours later, and that will need a larger address).

The function returns an `uint8_t` (byte) containing the value read.

### `USERSIG.write( address, value )`

The `write()` method allows you to write a single byte of data to the USERROW.
Two parameters are needed. The first is an `int` containing the address that is to be written, and the second is the data to be written, a `byte`.

When this is called, **if there is no data already in the buffer** to be written pending erasure, the value of currently stored in that byte of the USERROW will be checked, and if this value can be written to it without another erase, that will be done, and this function will return 1 (the number of bytes written). An erase is required whenever any bit needs to be changed from a 0 to a 1, but not to change a 1 to a 0 - that is to say, if performing a bitwise and between on the new value with the existing one would change it, an erase is required; the test is just `if ((new value) & (old value) == (new value))`.  Freshly erased USERROW memory reads 0xFF and hence can accept any value, and likewise you can always write 0 to an address without erasing. Otherwise it all depends on the values - you can write 5 to a location that was storing 15 or 21 without an erase, but not one that was storing 16.

If an erase would be required and the buffer doesn't contain anything waiting to be written, we load the current contents of the USERROW into a buffer store this value into that buffer, and this method returns a 0.

If the buffer already contains data, regardless of whether this byte could be written without an erase, we just store it to the buffer and return a 0.

A suggested method of writing a chunk of data to the USERROW is:
```c++
uint8_t written = 0;
written += USERSIG.write(0, MyFirstValue);
written += USERSIG.write(1, MySecondValue);
written += USERSIG.write(2, MyThirdValue);
written += USERSIG.write(3, MyLastValue);
if (written < 4) {
  // Now we have stuff in the buffer that won't be automatically saved!
}
```

**This function returns a 1 if the value was actually written to the USERROW. It returns a 0 otherwise.**

Calling this from within an interrupt will write nothing to the USERROW or the buffer, and return -16, an error code. As documented, we don't support writing to USERROW from interrupts.

If the new value is the same as the old value, and we would otherwise have written it to the USERROW, we skip the writing step and return a 1.

### `USERSIG.update( address, value )`

This is identical to the `write()` method, and exists only for compatibility. We will *never* write to the userrow when the value is the same as what's already there.

### `USERSIG.flush()`

The `USERSIG.flush()` method (named by analogy with `Serial.flush()`) must be called to actually write anything that was put into the buffer because an erase was required. This function will erase the USERROW, and then write the contents of the buffer (which contains the previous contents and all changes), and reset the status.

If there is nothing waiting to be written, an erase/write cycle will not be performed, and this will return 0. It is harmless to call this when nothing is waiting to be written, or when you're not sure if there's anything waiting to be written.

This returns the number of bytes written (number of bytes that are different from what is currently in the USERROW - this is done through a separate loop comparing each value in the buffer to the USERROW before the whole thing is erased and the buffer written back to it).

Thus, if you add the value returned by `USERSIG.flush()` to the sum of the values returned by `USERSIG.write()`, and have not called `USERSIG.put()` or written via subscript or through an `USRef` or `USPtr`, the result will be the number of bytes you have written in total:

```c++
/* MyData is a byte array with at least 20 values */
uint8_t written = 0;
for (byte i = 0; i < 20; i++) {
  written += USERSIG.write(i, MyData[i]);
}
written += USERSIG.flush();
// written == 20 now, so you know the whole thing is stored in non-volatile memory, and will still be there after a powercycle.
```

### `USERSIG.pending()`
This will simply return a 1 if there are one or more writes pending (we don't keep count of how many - flush() counts up changed bytes before it writes them). Note that you basically never need to call this - you can just call USERSIG.flush() when you're done writing stuff and aren't sure whether there are pending writes. This is there only for completeness.

### `USERSIG.erase()`
This will erase the USERROW without rewriting anything and clear the buffer.

### `USERSIG.get( address, object )`

This function will retrieve any object from the USERROW (if there are no pending writes) or the temporary buffer (if there are).
Two parameters are needed to call this function. The first is an `int` containing the address that is to be written, and the second is the object you would like to read.

This function returns a reference to the `object` passed in. It does not need to be used and is only returned for conveience.

### `USERSIG.put( address, object )`
This function will write any object to the USERROW.
Two parameters are needed to call this function. The first is an `int` containing the address that is to be written, and the second is the object you would like to write.

**Here is where it gets weird**
We can't return the number of bytes written like I would have liked to - that would be a rather awkward change to API, particularly since this is a template function. But if we always had put() write the buffer to the USERROW, that would make this vulnerable to write amplification when small datatypes are written, and it would become a method I would feel I needed to warn people about, rather than the go-to method of writing datatypes larger than a byte. My compromise was this: anything larger than 4 bytes will trigger an erase+write if it cannot be written (meaning none of the common primitive datatypes, but almost any `struct` or object). Anything smaller will not.

This function returns a reference to the `object` passed in. It does not need to be used and is only returned for conveience.

```c++
/* Like the above flush() example, MyData is an array of 20 bytes. */
USERSIG.write(0, 123);
USERSIG.write(1, 210);
Serial.println(USERSIG.pending()); // This will be 1 unless locations 0 and 1 can be written without an erase
USERSIG.put(2, MyInt);  // Write some 16-bit value -
Serial.println(USERSIG.pending()); // Still 1
USERSIG.put(4, MyData); // store MyData, the 20-element array. 20 is more than 4 bytes, so we write everything.
Serial.println(USERSIG.pending()); // Will now be 0 - put'ing the large object triggers the write.
```

This should be obvious, and is no different from the EEPROM library (but is much easier to run into here): Don't call `put()` with something larger than size of the USERROW (32b). It will wrap around, and only the last 32 bytes will be saved.

### **Subscript operator:** `USERSIG[address]`

This operator allows using the identifier `USERSIG` like an array.
USERSIG cells can be read _and_ **_written_** directly using this method.

This operator returns a reference to the USERSIG cell. When writing this way **THE BYTE WRITTEN COUNT IS NOT RECORDED** - you must call `USERSIG.flush()` once you've written all you are planning to at the moment.

### `USERSIG.length()`

This function returns a `byte` containing the number of bytes in the user signature space (32 on DA, DB, and DD-series parts.

## Advanced features

This library uses a component based approach to provide its functionality. This means you can also use these components to design a customized approach. Two background classes are available for use: `USRef` & `USPtr`.

**When using these to write, they do not return the number of bytes written** - that means that *you don't know if anything was actually written*. To make sure everything gets saved, you need to either call `USERSIG.flush()` after a series of writes, or call `USERSIG.erase()` before (and avoid writing the same address twice) or check `USERSIG.pending()` afterwards.

### `USRef` class

This object references a USERSIG cell.
Its purpose is to mimic a typical byte of RAM, however its storage is the USERSIG.
This class has an overhead of two bytes, similar to storing a pointer to a USERSIG cell.

```C++
USRef ref = USERSIG[ 10 ]; //Create a reference to 11th cell.

ref = 4; //write to USERSIG cell.

unsigned char val = ref; //Read referenced cell.
```

### `USPtr` class

This object is a bidirectional pointer to USERSIG cells represented by `USRef` objects.
Just like a normal pointer type, this type can be dereferenced and repositioned using
increment/decrement operators.

```C++
USPtr ptr = 10; //Create a pointer to 11th cell.

*ptr = 4; //dereference and write to USERSIG cell.

unsigned char val = *ptr; //dereference and read.

ptr++; //Move to next USERSIG cell.
```

#### `USERSIG.begin()`

This function returns an `USPtr` pointing to the first cell in the USERSIG.
This is useful for STL objects, custom iteration and C++11 style ranged for loops.

#### `USERSIG.end()`

This function returns an `USPtr` pointing at the location after the last USERSIG cell.
Used with `begin()` to provide custom iteration.

**Note:** The `USPtr` returned is invalid as it is out of range. In fact the hardware causes wrapping of the address (overflow) and `USERSIG.end()` actually references the first USERSIG cell.

## A general warning
This library and the USERROW functionality have not been as thoroughly vetted for gremlins as EEPROM. Unlike EEPROM, we don't know how many skeletons are in this ~closet~ memory section. Accordingly, before you go shipping a product that relies on this, be sure to test it under adverse scenarios if it is expected to perform reliably in them (for example, low voltage, power interruption during write, etc). Be particularly careful of things that are known to cause issues with EEPROM.
