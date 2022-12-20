# Link Time Optimization (LTO)

## What is Link Time Optimization?
Compilation of a C program happens over many, many steps, but at the broadest level, it can be split into five phases (though the first two are run independently on each "compilation unit"`*`). Only the ones named in **bold** below are ones where you are likely to ever encounter an error.
1. **Preprocessing:** This is when #defines are replaced with what you defined them to be, and when all #if/#else conditional code is included (or not). It is possible to get an error at this step with malformed preprocessor directives.
2. **Compiling:**, which generates a .o file from each .S, .c or .cpp file (including the sketch itself). Invalid C/CPP syntax here will generate an error at this step.
3. Create core.a: avr-gcc-ar is used to combine all of the .o files created by the compiler into a single core.a file. This allows Arduino-builder to skip recompiling the core every time you make a change to your code. Changing tool menu options, however, forces a full rebuild (as it should). I have never seen or heard of an error occurring at this stage.
4. **Linking:** All of those .o files (contained in the core.a) and the yoursketch.ino.cpp.o file created by the compiler are now examined by the linker (called by invoking avr-gcc with different parameters, most importantly `-Lm`). Up until now, the code being generated was largely free any sort of addresses (relative or absolute). All the bits which load or store from RAM or registers, or jump to different parts of the program are *all placeholders*. The linker's job is to take all these disparate location independent files, and stitch them together according to a skeleton called the crt (C runtime - Don't bother looking for it - it's not included in toolchain packages, and is challenging to dig up even if you have the gcc source sitting in front of you), by following the linker scripts (buried deep in the toolchain in a directory called ldscripts). In this process, it finally assigns each chunk of executable code or piece of data in PROGMEM to an absolute address within the flash. Now that it knows where everything is located, it can replace all those placeholders pointing to locations with the relative or absolute addresses. The result is a large file known as a .elf (it's a very bloated elf...)
5. Create hex file: Once the linker has successfully created the .elf file, all that remains is to extract the binary data from the .elf, which also contains a lot of other crap you don't need and can't fit in device memory even if you wanted to. avr-objdump generates a .bin file. Then avr-objdump is invoked a second time to convert the .bin to a .hex, and again to generate a .eep (if there is anything in the EEPROM section, which there likely is not). `**` I have never seen anything go wrong here. avr-objdump can be used with different arguments to get asm listings, and nm can be used to get a memory map (this is done when you do tools -> export compiled binary).

`*` A "compilation unit" in c or c++ is "any c or cpp file" either the sketch path, the .c/cpp files of any included library, and the .c/cpp files of the core itself. Those are each compiled separately (and .h files are not compiled at all. `#include <header.h>` will be replaced with the body of the file of that name by the preprocessor).

`**` Something about this phrasing seemed problematic.... "The linker generates an elf. avr-objdump is called to strip it and stuff the elf into a much smaller bin, which can then be hexed."

If you think about it for a moment, you may realize that the fact that the compiler is acting only on one compilation unit at a time limits it's ability to optimize - that's why link time optimization exists. For the following example, let's assume your sketch includes the line `#include <foo.h>` and the foo library provides at least a single function called foo() which takes an integer argument. The library consists of "foo.h" and "foo.cpp". Because your sketch includes the header, you can call foo from within the sketch, and there won't be an error at compile time (even if foo.cpp is missing!); foo.cpp will get compiled when you compile the sketch as well. Let's consider the case where foo() is a rather bulky function (maybe it wasn't written very well), which does very different things depending on it's argument Envision something like:
```c++
void foo(uint8_t myarg) {
  switch (myarg): {
    case 1:
    {
     simpleThing1();
     break;
    }
    /* <SNIP> thousands of lines of case statements to handle other arguments. I told you it was a poorly written library.
     * easier to understand. */
  }
}
```

For the sake of example, suppose you only ever call foo(1). If foo() was defined in your sketch, the compiler would notice that, and not include all the other cases in the generated binary.

But the compiler, when it compiles the foo.cpp compilation unit, *has no idea* that you only call it with one argument. Hence it generates a very large function. And the linker then includes that file and you end up with a gigantic binary (if it fits the flash, that is).

Obviously, at the time that the linker runs, there is an opportunity to make considerable gains by removing code that is known to be unreachable. Without link-time optimization, the linker would always omit `foo()` from the binary if it saw no path from the code that called `foo()` at all. But you are calling foo(), so foo() got included. However DxCore and megaTinyCore compile with "link-time optimization" or LTO. In order for this to work, the compiler (at earlier steps) needs to be told to leave behind information to help the linker optimize (this has a sideffect of making the intermediate files non-human readable - though a file that gets automatically deleted fractions of a second after it's created not exactly readable to a human, regardless of what format it's in). Then when it gets to the linking step, it can see that "Oh, foo is only called with constand arguments, and the argument is always 1." and creates a copy of foo() (the symbol shows up in a memory map as `foo(int) [clone .constprop.x]` where x is some number), and then reoptimizes it to see if it can reduce the size of the binary. The savings from this step can range from nil (nothing that can be further optimized at the linking step is present in the code) to massive (arbitrarily large, in fact), but under typical conditions, 5-20% is normal. That's a REALLY BIG DEAL for embedded systems.

## Link time optimization also enables better error catching
We don't have exception handling, nor do most embedded systems have a means that they could use to report an error if it happened. There just isn't enough space in the flash for all that crap. This means that if code is syntactically valid, but does not produce the desired output, debugging is often a tedious process of inserting print statements and running it while connected to serial. (There's also hardware debugging, but it can only be done through Microchip's tools; I am not the one to ask about that, as I have not used it - I can't answer any questions, as literally the first thing I attempted to do with it was to make it detect the programmer (a Microchip SNAP) that I'd connected. I was never successful. Hence it is fair to say that I have failed at everything I have ever tried to do with Microchip's tools. Therefore, you'd be a madman to ask me for advice knowing that. And that was long enough ago that I don't remember the things I did that didn't work). If we could detect when the code we wrote couldn't possibly be correct, even though it was syntactically valid C, then we could generate an error, complete with a line number and some description of what we're trying to do that's wrong.
You may have noticed by now that we do this (in both mTC and DxC). This is done by relying on the constant folding functionality of the optimizer, as well as builtin expressions like like `__builtin_constant_p()` (which is true if the argument is a constant that will be subject to constant folding). This would make little difference without link time optimization. But with a combination of link-time optimization, and judicious usage `always_inline` wrappers around certain functions (example, shown below, for pinMode()):
```c++
inline __attribute__((always_inline)) void pinMode(uint8_t pin, uint8_t mode) {
  check_valid_digital_pin(pin);         /* generate compile error if a constant that is not a valid pin is used as the pin */
  check_valid_pin_mode(mode);           /* generate compile error if a constant that is not a valid pin mode is used as the mode */
  _pinMode(pin, mode);
}
// where:
inline __attribute__((always_inline)) void check_valid_pin_mode(uint8_t mode) {
  if (__builtin_constant_p(mode)) {
    if (mode != INPUT && mode != OUTPUT && mode != INPUT_PULLUP) {
      badArg("The mode passed to pinMode must be INPUT, OUTPUT, or INPUT_PULLUP (these have numeric values of 0, 1, or 2); it was given a constant that was not one of these.");
    }
  }
}

inline __attribute__((always_inline)) void check_valid_digital_pin(pin_size_t pin) {
  if (__builtin_constant_p(pin)) {
    if (pin >= NUM_TOTAL_PINS && pin != NOT_A_PIN) { // Exception made for NOT_A_PIN - code exists which relies on being able to pass this and have nothing happen.
      badArg("Digital pin is constant, but not a valid pin");
    }
  #if (CLOCK_SOURCE == 2)
    #if defined(MEGATINYCORE)
      if (pin == PIN_PA3) {
        badArg("Constant digital pin PIN_PA3 is used for the external osc, and is not available for other uses.");
      }
    #else
      if (pin == PIN_PA0) {
        badArg("Constant digital pin PIN_PA0 is used for the external osc, and is not available for other uses.");
      }
    #endif
  #elif CLOCK_SOURCE == 1
    if (pin < 2) {
      badArg("Pin PA0 and PA1 cannot be used for digital I/O because those are used for external crystal clock.");
    }
  #endif
  }
}
```

Notice how both check_* functions don't do anything with runtime observable consequences. `badArg()` simply generates an error at link time if it is found to be reachable. Since these functions don't do anything from the perspective of the hardware, the compiler will optimize them away entirely.

So a valid call to `pinMode()` becomes either a call to `_pinMode()` assuming the arguments are either constant, or are known only at runtime (compare, say, digitalWrite(PIN_PA7,LOW); with `volatile uint8_t pin = PIN_PA7; digitalWrite(pin,LOW);` you will notice that a) the second one is significantly larger because the compiler is not permitted to make assumptions about the value of pin, and b) the pin number you use will change the size of the binary. Of course it will! digitalWrite() turns off PWM. There are 3 kinds of timer that might be in use on a given pin, but if you are using a pin that cannot use any timer for PWM, ever (say, PIN_PC7 on a DA/DB), it can omit all the turning off of PWM, whereas if you use an unknown pin, it has to have the code to figure out what kinds of timers might be pointed at the pin, whether one of them is, and if so, how to turn the PWM off if it's on. This only works because of link time optimization, since (continuing that example) digitalWrite() is not in the same compilation unit as the main sketch, and that's why link time optimization is such a huge improvement.

## The dark side of LTO
There are two cases where (given invalid code), LTO makes turning the code into valid code more challenging. These are:
1. You are writing inline assembly and there is some sort of syntax error. An ephermeral .S file is created, the assembler looks at it, sees that it's not valid, and prints the error and line number. Then arduino builder immediately deletes that file. Now you have been told the line number of the error *in an intermediate file that doesn't exist anymore*. The file is also *not human readable* even if it hadn't been deleted because of the extra machine-readable annotations that were added to enable LTO. This is straightforward to deal with with using a few console commands to generate the .S file without LTO and not immediately delete it. See [Inline Assembly Syntax Errors](https://github.com/SpenceKonde/AVR-Guidance/blob/master/LowLevel/SyntaxError_InlineASM.md)
2. You are getting an error you don't expect, don't understand, and can't figure out by inspecting the related functions, and you don't even know the line or file that it occurred in: the error says the line was `.text+0` and the file was listed is a file which does not define the function that the error is occurring in (for example, it might complain of multiple definitions of `bar` saying that the error is in `sketchname.ino.cpp .text+0`, and that the previous definition was at `sketchname.ino.cpp .text+0`. Often these can be understood by indepth study of the function generating the error, and where it's located and how it could be getting treated as defined more than once. When you can't there is a last-ditch maneuver that can be used to try to get additional information.

## Disabling LTO
**Warning: Disabling LTO breaks a lot of stuff, because of how we take advantage of constant folding.** Because of this, **uploading of sketches is disabled** when LTO is disabled.

To disable LTO first close all copies of the IDE. Navigate to the folder that the core is installed to. In the unlikely event that you manually installed the core this is just (sketchbook location)/hardware/(DxCore or megaTinyCore). Inside that folder, open the `megaavr` folder and skip the next paragraph.

This is inside the Arduino15 folder (on Windows, for example, it would be `C:\Users\Username\AppData\Arduino15` - note that AppData is hidden. Other platforms have it located in other places, according to where that platform places user-writable files. In a portable installation, this location is always (install location)/portable. Inside that folder is a folder named 'packages' which contains a folder for every hardware package you've installed using board manager. Open the one for the core you're using. It will have a folder named after the currently installed version. Open that folder.

Either way, you'll see a file called platform.txt. Move it somewhere not under Arduino15, or rename it to something else.

If you have a manual installation, copy [extras/platform_without_LTO_manual.txt](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/platform_with_LTO_manual.txt) in it's place. If it's a board manager installation (likely), you need to download [github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/platform_without_LTO.txt](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/platform_without_LTO.txt). Either way, rename it to platform.txt.

Start the IDE. You will find that the sketch will compile to a markedly larger binary (the one I was testing while writing this went from 2.5k to 4.7k - the difference is larger (as a percent) for small sketches)... and that the error will now actually have line numbers. You will also notice SWARMS of warnings about the fact that LTO is off. This is intentional, as it ensures that there's no way you could forget about having disabled LTO. Now, armed with that additional information, you should be able to track down the cause of the problem, and fix it.

Eventually you'll get to a point where you either get an error indicating that the binary was too big, a "Relocation truncated to fit" or "is not in region .text" error (these happen when the sketch is too big, and as a result, the link time optimizer is unable to generate any output) if the binary is so much bigger with LTO that it doesn't fit anymore, it will compile successfully. If you out of habit tried to upload, you'd get to that point and get receive an error tellign you that LTO is disabled and to go turn it back on.
Once you get to that point, it's time to turn LTO back on. Exit all IDE windows, and now you can go get rid of that new platform.txt, and move or rename the old one back platform.txt (in case you lost it, a clean copy can be found in extras as [as platform_with_LTO_manual.txt](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/platform_with_LTO_manual.txt) in extras for manual installations, or at [in the repo for the board manager installation as platform_with_LTO](https://github.com/SpenceKonde/DxCore/blob/master/megaavr/extras/platform_with_LTO.txt).

Note: there are two copies of each of those files. The ones ending in `_manual` are for manual installations, the others are for board manager installations. They are identical except that board manager doesn't have a megaavr folder inside the core's folder
