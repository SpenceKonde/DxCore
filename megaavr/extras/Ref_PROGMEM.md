# PROGMEM sections
So, accessing the flash from the app is a little bit more complicated here than on other parts, since some parts of the flash is accessed

## 32k parts
There only one section and its' always mappoed. This is agood thing, Becaaaause....  as a modern AVR with less than 48k of flash, it's __AVR_ARCH__ 103. All const variables will be automatically left in flash and accessed transparently. No special considerations are needed, and you can ignorethe flmap settings (which do noithing here). The compiler (well linker) does it all for you!

## 64k parts
```text
PROGMEM_MAPPED - Can be accessed with defauylt flashmap settings.
PROGMEM_SECTION0 - The lowhalf of theflash Stuff oput here will start *AFTER* your application.
PROGMEM_SECTION1 - this ismapped by default, ie, PROGMEM_MAPPED is an alias of this/
const PROGMEM - This covers ENTIRE flash. Stuff you put here will start *BEFORE* your application, but after the vector table and trampolines sections.
```
Notice `PROGMEM` itself requires const declaration. The others do not (I tried to make them const, I couldn't seem to make it work).

You can furthermore change the maopped section like this:
```c++
NVMCTRL.CTRLB &= 0xEF; // sets the flash map to sectiuon zero
NVMCTRL.CTRLB |= 0x10; // sets the flash map to section one
```

Reading from PROGMEM_MAPPED is easy on the high page. You just read from it

There's a problem on the low page though; namely, the address is given relative the start of the flash not to the start of the section. So if it starts at address 0x1000 it will be mapped to 0x9000.
But the compiler still thinks it's address is 0x1000.

There's a simple if rather ugly construction to fix it with minimal overhead though:
```c++
uint16_t  PROGMEM_MAPPED   bigarray1[] = { /*insert your gargantuan initializer here, Iwas testing this to make sure I understood it with a few 4096 element arrays of uint16_t's.*/ };
uint16_t  PROGMEM_SECTION0 bigarray0[] = { /*insert your gargantuan initializer here, Iwas testing this to make sure I understood it with a few 4096 element arrays of uint16_t's.*/ };
uint16_t* bigarray0_mapped;

void setup() {
  bigarray0_mapped = (uint16_t*) (((uint16_t)bigarray0) | 0x8000);
  /* At this poiunt, bigarray1_mapped[123] willrefer to element 123 of the array. Similar approaches with pointer magic can achieve the same thing for other types of variables. It's just slightly cleaner when they're tables. To access  bigarray0, make sure NVMCTRL.CTRLB's FLMAP bits are set to 1 (default), and access normally. To access bigarray1. */
}
```

Note also that `pgm_read_x_near()` will work regardless of `FLMAP`.


## 128k parts
Finally, on the 128k parts, there are 4 flash sections.
```text
PROGMEM_MAPPED   - Can be accessed with default flashmap settings.
const PROGMEM    - The low half of flash. Stuff you put here will start *BEFORE* your application, but after the vector table and trampolines sections.
PROGMEM_SECTION0 - The low quarter of the flash. Stuff put here will start *AFTER* your application (if there is room).
PROGMEM_SECTION1 - second quarter - not mapped by default, accessible using the pgmspace macros.
PROGMEM_SECTION2 - Third quarter  - the least convenient.
PROGMEM_SECTION3 - Final quarter  - mapped by default.
```

You can furthermore change the mapped section like this:
```c++
NVMCTRL.CTRLB = (section << 4) | (NCMCRTRL.CTRLB & 0xCF); // sets the flash map to specified section.
```

And here's where it gets a little bit weird:

* **PROGMEM_SECTION0** and **PROGMEM_SECTION1** can be accessed regardless where the mnapping is pointed at with the pgm_read_x_near() macros.

* **PROGMEM_SECTION1** and **PROGMEM_SECTION3** can be accessed directly, like normal variables, provided FLMAP isset to 1 or 3 (3 is default). The above trick of adding 0x8000 to them to get them into the right hlf of the address space isn't needed - they are already there.

* **PROGMEM_SECTION2** and **PROGMEM_SECTION0** can be accessed if the flashmap is pointed in the correct section BUT you do need to use somnething like the trick I showed aobove to offset the addresses by 32768;

* **PROGMEM_SECTION2** and **PROGMEM_SECTION3** are out of range of normal lpm (pgm_read_byte_near). You need to jump through the hoops described in any guide to progmem on larger AVRs to get a "farptr" THAT, in turn can be passed to pgm_read_byte_far().



None of these things are particularly hard, but reading far progmem with the pgmspace macros is annoying, and has a larger performance penalty. While the actual instructios to read from the two methods don't take a different amount of time - ELPM and LPM are still 3-clock instructions, and LD is 2 plus 1 because it's flash (a fact that is buried in the instruction set manual, which refers us to a part of the datasheet with no mention of it, but "at a minimum 1 extra cycle is requires."), in practice, they are markedly slower because the macros in pgmspace arearen't super efficient, because they're little snippets of assembly which the compiler must treat as a black box, and they usually end up performing worse.

## Summary
32k, 16k, and 8k parts work like other 32k modern AVRs and all the flash is automatically memory mapped and accessible.

On both 64 and 128k parts, you can access the top 32k of flash without changing anything, and put constants there there using the DxCore supplied `PROGMEM_MAPPED` qualifier. You dont declare them const but you can't write to them normally.

On 128k parts, you can access the second section in the same way - you only need to change the flashmap bits.

Sections 0, and 2 for the 128k parts, accessing through mapping requires changing the flashmaP bits, AND using a bit of pointer arithmetic to offset the address by 0x8000 (32768).

And sections 0 and 1 on any part can always be accessed with pgm_read_x_near macros without any particular difficulty!

So no matter what, there are always two ways to access the program space, and both are pretty easy - but section 2 is the least desirable of them, since you either have to use ELPM - or offset the addresses and change the flashmap.

But you do need to put a bit thought into what you put where. Obviously, you probably want start from the high section - since access there is never slower, and always easier - to get the mapped flash by default - but only if your code will not be ported to devices that have less than 64k of flash.

If you need more than 32k of flash for constants, my recommendation is:
* If you need to write to flash from app, use the last block for that, always.
* For strings stored in flash, the F() macro is maximally convenient and portable.
* If you can't fit all you need to in the last block, use PROGMEM for stuff that won't fit - You can stuff nearly 64k of data in there on a 128k part, and it will push your code into the "least desirable" section 2.
* On 64k parts, you never need to change the FLMAP.
* On 32k parts, the PROGMEM method is usually most convenient.
* Mapped flash is better for things where you're sequentially accessing a bunch of bytes because `LPM __, Z+` isn't used everywhere it can be, while it's advantage is much smaller for randomly accessed data and larger datatypes.

## Don't forget Flash.h
Finally, I would be remiss if I didn't mention the Flash.h library and the option of writing to flash from the app. Using that to save non-volatile data is a great approach. Better still (on Dx-series only, not Ex), if the the app SPM permissions are set correctly you could even use it to erase a bunch of flash, make sure that the NVMCTRL.CTRLA was set to FLWR and then (remember, they're not set const) **simply assign values to erased locations declared PROGMEM_MAPPED there in order to save data**! (this does not work with the unlimited flash write option - that always requires using Flash.h to write - it also doesn't work with AVR128DA parts up to die rev. A8 for the first 512b of that last section).
