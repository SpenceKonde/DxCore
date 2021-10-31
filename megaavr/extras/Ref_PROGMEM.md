# PROGMEM sections


## 32k parts
There only one section and its' always mappoed. This is agood thing, Becaaaause....  as a modern AVR with less than 48k of flash, it's __AVR_ARCH__ 103. All const variables will be automatically left in flash and accessed transparently. No special considerations are needed, and you can ignorethe flmap settings (which do noithing here). The compiler (well linker) does it all for you!

## 64k parts
```
PROGMEM_MAPPED - Can be accessed with defauylt flashmap settings.
PROGMEM_SECTION0 - The lowhalf of theflash Stuff oput here will start *AFTER* your application.
PROGMEM_SECTION1 - this ismapped by default, ie, PROGMEM_MAPPED is an alias of this/
const PROGMEM - This covers ENTIRE flash.
```
Noitice PROGMEM itself requires const declaration. The others do not (I tried to makethem const, I couldn;t seem to make it work) Anyway.

You can furthermore change the maopped section like this:
```
NVMCTRL.CTRLB &= 0xEF; // sets the flash map to sectiuon zero
NVMCTRL.CTRLB |= 0x10; // sets the flash map to section one
```

Reading from PROGMEM_MAPPED is easy on the high page. You just read from it

There's a problem on the low page though; namely, the address is given relative the start of the flash not to the start of the section. So if it starts at address 0x1000 it will be mapped to 0x9000.
But the compiler still thinks it's address is 0x1000.

There's a simple if rather ugly construction to fix it with minimal overhead though:
```c
uint16_t PROGMEM_MAPPED bigarray1[] = { /*insert your gargantuan initializer here, Iwas testing this to make sure I understood it with a few 4096 element arrays of uint16_t's.*/ };
uint16_t PROGMEM_SECTION0 bigarray0[] = { /*insert your gargantuan initializer here, Iwas testing this to make sure I understood it with a few 4096 element arrays of uint16_t's.*/ };
uint16_t* bigarray0_mapped;yiu

void setup() {
  bigarray0_mapped = (uint16_t*)(((uint16_t)bigarray0)|0x8000);

  /* At this poiunt, bigarraty0_mapped[123] willrefer to element 123 of the array. Similar approaches with pointer magic can achieve the same thing for other types of variables. It's just slightly cleaner when they're tables.
}
```

Note also that `pgm_read_x_near()` WORK TOO


## 128k parts
Finally, on the 128k parts, there are 4 flash sections.
```
PROGMEM_MAPPED - Can be accessed with defauylt flashmap settings.
PROGMEM_SECTION0 - The low quarter of the flash. Stuff oput here will start *AFTER* your application (if there is room).
PROGMEM_SECTION1 - second quarter - TOGETHER WITH THE FIRST, YOU CAN ALSO USE pgm_read_x_near(), and you can put stuff here with PROGMEM!
PROGMEM_SECTION2 - Third quarter - the least convenient.
PROGMEM_SECTION3 - Final quarter - mapped byt default
```

You can furthermore change the mopped section like this:
```
NVMCTRL.CTRLB = (section << 4) | (NCMCRTRL.CTRLB & 0xCF); // sets the flash map to specified section.
```

and here's where it finally gets a little bit weird

* **PROGMEM_SECTION0** and **PROGMEM_SECTION1** can be accessed regardless where the mnapping is pointed at with the pgm_read_x_near() macros.

* **PROGMEM_SECTION1** and **PROGMEM_SECTION3** can be accessed directly, like normal variables, provided FLMAP isset to 1 or 3 (3 is default). The above trick of adding 0x8000 to them to get them into the right hlf of the address space isn't needed - they are already there.

* **PROGMEN_SECTION2** and **PROGMEN_SECTION0** can be accessed if the flashmap is pointed in the correct section BUT you do need to tuse somnething likethetrick I showed aobove to offset the addresses by 32768;

* **PROGMEM_SECTION2** and **PROGMEN_SECTION3** are out of range of normal lpm (pgm_read_byte_near). You need to jump thhrough the hoops described in any guide to progmem on larger AVRs to get a "farptr" THAT, in turn can be passed to pgm_read_nyte_far().

Nonne of these things are particularly hard, but reading far progmem with the pgmspace macros is annoying, and has a larger performance penalty.

While the actual instructiosnto read from the two methods don't take a different amount of time - ELPM and LPM are still 3-clock instructions, and LD is 2 plus 1 because it's flash (a fact that is buried in thei nstructionset manual), in practice, they are markedly slower because the macros in pgmspace arearen't super efficient, because they're little snippets of assembly which the compiler must treat as a black box, and they usually end up performing worse.

## Summaty
32k partswork like other 32k modern AVRsm and all the flash is automatically memory mapped and accessible.

On both 64 and 128k parts, you can access the top 32k of flash without changing anything, and put constantsthere there using the DxCore supplied `PROGMEM_MAPPED` qualifier. You dont declare them const but you can;t write to them normally.

on 128k parts, you can access the second section in the same way - you only need to change the flashmaop bits.

Sections 0, and 2 for the 128k partsm, accessing through mapping required changing the flashmao bits, AND using a bit oif pouinter arithmnartic to offset the address by 0x8000 (32768).

And sections 0 and 1 on any part can always be accessed with pgm_read_x_near macros without any particular difficulty!

So no matter what, there are always two ways to acecessw the program space, and both are pretty easy.

Butyou do need to put a little morethought into what you put where. Obviously, you startt from the high section, to get the mapped flash by default. After that if you know tyour application issmall even if the data it needs is huge, you can put itin the easier to work with section, or go the logically ocnistant route and work down from the enbd ioffkashm next moving intio section 2, then section1 ifthere is any space left. Also, it is verty common to just leave the flashmap at the startuop setting, and make that your luxury flash area, and if you need to, you can also put some stuff into progmem - and thiswoll work foremost people.

Finally, I would be remiss if I didn't mentioned the Flash.h libraryand the writing to flash from app option; the cleanest way is to limit writes to the last 32k of flash. Using that to save non-volatile data is a great approach. Not only that.... you couldeven use it to erase a bunch of flash, make sure that the NVMCTRL,CTRLA was set to LFWE mode, and if you're sert the app SPM permissions correctly, you can **simply assign values to that space in order to save data**!
