# PROGMEM sections
So, accessing the flash from the app is a little bit more complicated here than on other parts, since some parts of the flash can accessed trivially like they were ram, and other parts cannot be. Except on megaAVR 0 (which can have up to 48k mapped), parts with 32k or less are defined with `__AVR_ARCH__ == 103`, indicating all flash is memory mapped. This table sums up the currently defined `__AVR_ARCH__` levels and on which parts they can be found. This macro is defined at a very low level and comes from the device-spec files.

As of 1.6.8 this is now handled a little more rigorously, but should be backwards compatible if you were using the API correctly.

## 8k, 16k, 32k parts
There is only one section and it's always mapped. This is awesome, Becaaaause....  as a modern AVR with less than 48k of flash, it's `__AVR_ARCH__ == 103`. That is recognized by the linker, and all you need to do is declare a variable `const` will be automatically left in flash and accessed transparently. No special considerations are needed, and you can ignore the flmap settings (which have no effect. The compiler (well, technically it's the linker that does it) does it all for you!

That's handy, because the flash is approaching tinyAVR territory in some families, so you don't have flash to spare for such fripperies as changing the flash mapping and carefully steping around the hazards inherrent in that. The EA and EB will both be available down to 8k, maxing out at 64 and 32k respectively.

## Parts with more flash, it gets a bit more complicated
Because you see, only 32k can be mapped at a time. But we don't just need to think about which 32k segment to put it in. We need to think about how we put it there, and how we're getting it out, and whether we need to change the mapping dynamically while the program is running or can take oine value and stick with it.

### Flash mapping menu
There is now a flash mapping menu. **It is ignored if all the flash is mapped**. For a part where the largest-flash device has N FLMAP options, there are N + 1 options. Unless the final option is chosen, the FLMAPLOCK bit is set very early in initialization, along with the chosen FLMAP.
* First option (default): Use the last FLMAP option (3, 96-128k on 128k devices, 1 on 64k devices)
* Options 2 through N - use the specified FLMAP section number instead.
  * DANGER: Only odd numbered sections will come out of the declaration with the correct address. Even numbered sections need 0x8000 added to the address.
* Final option - Do not touch NVMCTRL.CTRLB at all, meaning FLMAP is NOT locked.
  * No PROGMEM_MAPPED macro is defined - when the FLMAP is not locked, there is no guardrail to prevent you from changing FLMAP; if variables are being accessed like they were in RAM, you don't know that the compiler won't reorder things or otherwise end up trying to use those variables like they were in RAM.
  * because great caution is needed, in this case. we do not give you PROGMEM_MAPPED because the mappedc section is no longer a constant.
  * If you choose this option, significant hazards arise, detailed below.
  * Only use this option if you have considered and ruled out other approaches
* When specifying a locked FLMAP configuration, it is possible to specify a number higher than the maximum FLMAP section if you're using a part with less flash than the maximum.
  * The section used will be (selected FLMAP) % (number of FLMAP sections) - so sections 2 and 3 on a 64k device will be treated as 0 and 1.
  * The FLMAP lock bit is cleared by a reset - (like the rest of NVMCTRLB - which consists of the 2 FLMAP bits, the FLMAP lock, and 3 other access control bits )


#### That last menu option, unlocked FLMAP
This is a dangerous option (though if you never write FLMAP, it works - this was what we were doing before 1.6.7, only with PROGMEM_MAPPED defined too, and only in passing was it mentioned that you have to not change FLMAP or everything would go to hell). It was judged that this . If this option is chosen, the FLMAP bits are left at default (pointing to highest section) and FLMAPLOCK is not set. the PROGMEM_SECTIONn macros are defined, but there is no PROGMEM_MAPPED macro. This means that if code is written to blindly assume that there is a PROGMEM_MAPPED, and that it can count on that to be accessible, the compile will fail (since no PROGMEM_MAPPED), which is the correct behavior: If FLMAPLOCK is not set, you cannot depend on any particular section of the flash being mapped unless you have made sure that it is. **With this option selected, it is your responsibility to make sure that you never give the compiler latitude to dereference a pointer to mapped flash when the FLMAP bit does not select that section of the flash!**

This should be used only by advanced users. It is suggested to review the generated assembly listings to ensure that the compiler has not done anything that isn't above board here; writing to the FLMAP bits is unlikely to be rendered as anything other than `sts 0x1001, r__` so it's fairly easy to searj for and you would then need to make sure they they were located where you intended relative to any load instructions that load them. If, like most people, you don't feel comfortable reading the assembly listings or know what to look for, do not use this option. This is on

At a minimum, be sure to consider the following.
* **If you do not change FLMAP at all, you should not select this option!**
* If mapped flash is accessed from within an ISR, that ISR must save the current FLMAP setting, change it to the one it needs, and then restore it. The method shown below is based on how the compiler saves and restores SREG and similar (see examples below).
* You must make sure that the compiler isn't caching things in registers when the FLMAP bits are changed - you need a (performance impacting) memory barrier to force it to invalidate the cache. (see examples).
* You must ensure (through whatever means is appropriate for your application) that you never try to access a variable in a progmem section like a variable unless you know that you have FLMAP set correctly.
* You can always use the facilities provided by pgmspace.h (`read_byte_near()` et. al.) to read the flash (though if it's not in the near (lower 64k) progmem space you have to use farptrs and read_datatype_far()). Consider whether it would be possible to use that to access data that you can't fit in the mapped flash, thus allowing you to use a preferred - FLMAP locked - option
  * The "hot" data that is accessed most often from the most places benefits more from being in mapped flash, so if your constant data splits naturally into a "hot" (accessed often and from many places) and "cold" (accessed from one or only a small number of places) portion, and the "hot" data all fits into 32k, use a locked flash map option, put the hot data into that section, and put the cold data in PROGMEM and access with read_byte_near().
  * This will work with up to 64k of constants in PROGMEM, and 32k of constants in the mapped flash on a 128k part - there would still be a 32k section of "cursed ground" that isn't mapped and can't be read with the read_datatype_near() macros (which are actually just wrappers around a trivial bit of inline asm that executes lpm - or, for read_datatype_far(), a write to RAMPZ and a wrapper around elpm, and required working with with farptr's, which are bad juju, hence why we call that read_x_far() only flash "cursed ground". With presently available parts you should not need to set foot on cursed ground: You have 32k mapped, and 64k near, so only 32k of cursed ground. As you fill up the PROGMEM, your executable code will be pushed deeper into the flash, so that when 64k (well slightly less because there are interrupt vectors and maybe a bootloader) of PROGMEM constants are allocated, you end up with the executable code - which doesn't mind - residing in the cursed ground.

```c
ISR(MYPERIPHERAL_INTNAME_vect) {
  __asm__ __volatile__ ("lds r0, 0x1001     \n\t   push r0 \n\t"); //first line of the body of ISR.
  NVMCTRL.CTRLB = NVMCTRLB & 0x8F | (flmapsection number << 4);
  /* do your thing */
  __asm__ __volatile__ ("pop r0    \n\t  sts 0x1001, r0    \n\t"); //last line of the body of the ISR.
}
/* Memory barrier techniques */
// enabling or disabling interrupts generates a memory barrier:

__asm__ __volatile__(":::memory"); //prevent reordering of instructions meant to happen before the nvmctrl write to a time after it
NVMCTRL.CTRLB = NVMCTRLB & 0x8F | (flmapsection number << 4); // this is an SFR, which from software perspective is a volatile uint8_t;
__asm__ __volatile__(":::memory"); //prevent reordering of subsequent instructions so it can't move a read that's supposed to happen after this is changed to the pre-change section.
```

## 64k vs 128k - a big difference
Both 64k and 128k parts have the FLMAP menu. However, that doubling of the number of sections has major ramifications.

## 64k parts
```text
PROGMEM_MAPPED - Unless the unlocked flash map option is chosen, this will point to the mapped flash.
PROGMEM_SECTION0 - The low half of the flash. Stuff put here will be located *AFTER* your application. We do NOT recommend this option, because it provides no guarantee or ready means of verifying whether it is there.
PROGMEM_SECTION1 - this is mapped by default, ie, PROGMEM_MAPPED is an alias of this.
PROGMEM - This covers ENTIRE flash. Stuff you put here will be located *BEFORE* your application, but after the vector table and trampolines sections.
```
Notice `PROGMEM` itself requires const declaration.... the other sections don't enforce this, but should always still be declared const!

Reading from PROGMEM_MAPPED is easy on the high page. You just read from it

There's a problem on the low page though; namely, the address is given relative the start of the flash not to the start of the section. So if it starts at address 0x1000 it will be mapped to 0x9000.
But the compiler still thinks its address is 0x1000.

There's a simple if rather ugly construction to fix it with minimal overhead though:
```c++
uint16_t  PROGMEM_MAPPED   bigarray1[] = { /*insert your gargantuan initializer here, I was testing this to make sure I understood it with a few 4096 element arrays of uint16_t's.*/ };
uint16_t  PROGMEM_SECTION0 bigarray0[] = { /*insert your gargantuan initializer here, I was testing this to make sure I understood it with a few 4096 element arrays of uint16_t's.*/ };
uint16_t* bigarray0_mapped;

void setup() {
  bigarray0_mapped = (uint16_t*) (((uint16_t)bigarray0) | 0x8000);
  /* At this poiunt, bigarray1_mapped[123] willrefer to element 123 of the array. Similar approaches with pointer magic can achieve the same thing for other types of variables. It's just slightly cleaner when they're tables. To access  bigarray0, make sure NVMCTRL.CTRLB's FLMAP bits are set to 1 (default), and access normally. To access bigarray1. */
}
```

Note also that `pgm_read_x_near()` will work regardless of `FLMAP`.

We will look into whether it possible to automatically fix the pointers.

## 128k parts
Finally, on the 128k parts, there are 4 flash sections.
```text
PROGMEM_MAPPED   - Unless the FLMAP is unlocked, this is pointed to one of the flash sections.
PROGMEM_SECTION0 - The low quarter of the flash. Stuff put here will start *AFTER* your application (if there is room).
PROGMEM_SECTION1 - second quarter - not mapped by default, accessible using the pgmspace macros.
PROGMEM_SECTION2 - Third quarter  - the least convenient.
PROGMEM_SECTION3 - Final quarter  - mapped by default.
const PROGMEM    - The low half of flash. Stuff you put here will be located *BEFORE* your application, but after the vector table and trampolines sections.
```

And here's where it gets a little bit weird:

* **PROGMEM_SECTION0** and **PROGMEM_SECTION1** can be accessed regardless where the mapping is pointed at with the pgm_read_x_near() macros.

* **PROGMEM_SECTION1** and **PROGMEM_SECTION3** can be accessed directly, like normal variables, provided FLMAP isset to 1 or 3 (3 is default). The above trick of adding 0x8000 to them to get them into the right half of the address space isn't needed - they are already there.

* **PROGMEM_SECTION2** and **PROGMEM_SECTION0** can be accessed if the flashmap is pointed in the correct section BUT you do need to use somnething like the trick I showed aobove to offset the addresses by 0x8000 (32768);

* **PROGMEM_SECTION2** and **PROGMEM_SECTION3** are out of range of normal lpm (pgm_read_byte_near). You need to jump through the hoops described in any guide to progmem on larger AVRs to get a "farptr", and THAT, in turn can be passed to pgm_read_byte_far().

| Section | pgm read near | direct access | offset access | pgm read far |
|---------|---------------|---------------|---------------|--------------|
| FLMAP 0 | Yes           | No            | Inadvisable`*`| Yes `**`     |
| FLMAP 1 | Yes           | Yes `*`       | Not needed    | Yes `**`     |
| FLMAP 2 | No            | No            | Yes           | Yes          |
| FLMAP 3 | No            | Yes           | Not needed    | Yes          |

`*` - These are risky choices depending on the size of the code because unlike PROGMEM, PROGMEM_SECTIONn goes AFTER the code in that section. You however don't know what the linker has done until you look at the flashmap output and verify that the stuff you told it to put in there actually wound up here.
`**` - far pgmspace reads are slower/

None of these things are particularly hard, but reading far progmem with the pgmspace macros is annoying, and has a larger performance penalty. While the actual instructions to read from the two methods don't take a different amount of time - ELPM and LPM are still 3-clock instructions, and LD is 2 plus 1 because it's flash (a fact that is buried in the instruction set manual, which refers us to a part of the datasheet with no mention of it, but "at a minimum 1 extra cycle is requiren."), in practice, they can perform anywhere from nearly as well to markedly worse - the macros in pgmspace are little snippets of assembly which the compiler more or less treats as a black box, and they usually end up performing worse - eg, it won't use postincrement when you're iterating over an array and using pgm read functions to read, but it will if you use

## For forward compatibility, don't use the last 512b of flash
In the case of an application which does not change the flashmap it is suggested to not make use of the very last page (512b) of flash. Future versions of the core may use the last page of flash for some constant tables to improve performance and shrink memory footprint.

## Summary
If you're reading the summary, and have not studied the above document carefully, do not choose the unlocked flmap options from the tools menu. Using that option safely is hard.

32k, 16k, and 8k parts work like other 32k modern AVRs and all the flash is automatically memory mapped and accessible. You never need to use PROGMEM or the F() macro unless you are dealing with a library that requires input to something be of type flash_string_helper. In this case, I advocate complaining to the author of the library and informing him that there are parts were F(), which is still needed to use their library is slower and wastes a small amount of flash compared to not using those, and suggest that they overload the function with a version theat takes a normal c string.

On both 64 and 128k parts, you can access the top 32k of flash without changing anything, and put constants there there using the DxCore supplied `PROGMEM_MAPPED` qualifier.

On 128k parts, you can access the second section in the same way/

Sections 0, and 2 for the 128k parts, accessing through mapping requires changing the flashmaP bits, AND using a bit of pointer arithmetic to offset the address by 0x8000 (32768).

And sections 0 and 1 on any part can always be accessed with pgm_read_x_near macros without any particular difficulty!

So no matter what, there are always two ways to access the program space, and both are pretty easy - but section 2 is the least desirable of them, since you either have to use ELPM - or offset the addresses and change the flashmap.

But you do need to put a bit thought into what you put where. Obviously, you probably want start from the high section - since access there is never slower, and always easier - to get the mapped flash by default - but only if your code will not be ported to devices that have less than 64k of flash.

## Overall suggestions
If you need more than 32k of flash for constants.
* If you need to write to flash from app, always use the end of theflash for that.
* For strings stored in flash, the F() macro is maximally convenient and portable - though it's slightly less performant.
* If you can't fit all you need to into one section, use PROGMEM for stuff that won't fit - You can stuff nearly 64k of data in there on a 128k part, and it will push your code into the "least desirable" section 2.
* Section 0 is a very dangerous section to use because your code will push stuff out of there and you may not realize until you read gibberish.
* On 64k parts, you never need to change the FLMAP.
* On 32k parts just declare things constant
* Mapped flash is better for things where you're sequentially accessing a bunch of bytes because `LPM __, Z+` isn't used everywhere it can be, while its advantage is much smaller for randomly accessed data and larger datatypes.

## Don't forget Flash.h
Finally, I would be remiss if I didn't mention the Flash.h library and the option of writing to flash from the app. Using that to save non-volatile data is a great approach. Better still, if the the app SPM permissions are set correctly you could even use it to erase a bunch of flash, make sure that the NVMCTRL.CTRLA was set to FLWR and then (remember, you aren't forced to declare them const!) **simply assign values to erased locations declared PROGMEM_MAPPED there in order to save data**! (this does not work with the unlimited flash write option - that always requires using Flash.h to write - it also doesn't work with AVR128DA parts up to die rev. A8 on the first page of the flash). (do note that using Flash.h to write about twice as fast, thopugh.)
