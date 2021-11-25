# UART implementation 1.4.0 implementation notes

## Constants no longer in variant files
We removed a bunch of constants that didn't vary from the variant files. For backwards compatibility we generate in UART_swap.h. There's no HWSERIALn_MUX_COUNT - it's named MUXCOUNT_USARTn and calculated in UARTswap.h

* HWSERIAL1
* HWSERIAL1_DRE_VECTOR
* HWSERIAL1_DRE_VECTOR_NUM
* HWSERIAL1_RXC_VECTOR
* HWSERIAL1_MUX_COUNT
We know what the names of the vectors are gonna be. So why #define them?
The vector numbers were only used for the buggy ISR elevating routine that caused race condition hangs!

Invalid mux options are not required to be mentioned nor bogus pins #defined for them in the variants.
We will start from the highest numbered mux. If it's defined, we check the next lowest until we get to 0. Whenever we don't see one defined, we #define a placeholder row in the table (don't worry, only 3 bytes each, we've saved more flash than that so far). I was also noted that every pin numbering puts TX and RX of every pin next to each other in the same order. So we rely on that explicitly now.

Valid mux options must define the mux mux code itself, plus the pins associated with it.
### Before
```c++
// USART 2
#define HWSERIAL2                       &USART2
#define HWSERIAL2_DRE_VECTOR            USART2_DRE_vect
#define HWSERIAL2_DRE_VECTOR_NUM        USART2_DRE_vect_num
#define HWSERIAL2_RXC_VECTOR            USART2_RXC_vect
#define HWSERIAL2_MUX                   PORTMUX_USART2_DEFAULT_gc
#define HWSERIAL2_MUX_PINSWAP_1         PORTMUX_USART2_ALT1_gc
#define HWSERIAL2_MUX_PINSWAP_NONE      PORTMUX_USART2_NONE_gc
#define PIN_HWSERIAL2_TX                PIN_PF0
#define PIN_HWSERIAL2_RX                PIN_PF1
#define PIN_HWSERIAL2_XCK               PIN_PF2
#define PIN_HWSERIAL2_XDIR              PIN_PF3
#define PIN_HWSERIAL2_TX_PINSWAP_1      PIN_PF4
#define PIN_HWSERIAL2_RX_PINSWAP_1      PIN_PF5
#define PIN_HWSERIAL2_XCK_PINSWAP_1     NOT_A_PIN
#define PIN_HWSERIAL2_XDIR_PINSWAP_1    NOT_A_PIN
```
### After
```c++
// USART 2
#define HWSERIAL2_MUX                   PORTMUX_USART2_DEFAULT_gc
#define HWSERIAL2_MUX_PINSWAP_1         PORTMUX_USART2_ALT1_gc
#define HWSERIAL2_MUX_PINSWAP_NONE      PORTMUX_USART2_NONE_gc
#define PIN_HWSERIAL2_TX                PIN_PF0
#define PIN_HWSERIAL2_RX                PIN_PF1
#define PIN_HWSERIAL2_XCK               PIN_PF2
#define PIN_HWSERIAL2_XDIR              PIN_PF3
#define PIN_HWSERIAL2_TX_PINSWAP_1      PIN_PF4
#define PIN_HWSERIAL2_RX_PINSWAP_1      PIN_PF5
#define PIN_HWSERIAL2_XCK_PINSWAP_1     NOT_A_PIN
#define PIN_HWSERIAL2_XDIR_PINSWAP_1    NOT_A_PIN
```
A tangle of convoluted #ifdefs serving little purpose was cut down and renoved from the 6-port-specific files. The code is simpler, and appears to not have broken anything dramatic so far - and now we will be able to make the DD's work.

Before gwe get into the minutae and pick apart some of the remaining issues... one more thing....

As many of you may be aware, the hardware USARTs are capable of functioning in some other modes, and some of the most intriguing to many uses have long been the OneWire (half duplex) mode, typically used in combination with the Open Drain mode. This mode can now be enabled automatically by calling Serial.begin(baud, SERIAL_8N1 | SERIAL_HALF_DUPLEX). because loopback would have you receiving your outgoing characters, we turn off the RX interrupt before sending anything, and, in the TXComplete interrupt, empty the incoming data registers to get rid of the data that we just sent and then re-enable it. It seemed to work. We also added support for the RS485 option - include SERIAL_RS485 with or without those other arguments, and it will ensure that XDIR is output, and drive it high 1 bit before the start of a signal and keep it that way until 1 bit after the end of the signal. This is meant for controlling a line driver chip. And if your line driver happens to be active low, never fear, just invert the pin using pinConfigure or manually writing the PINnCTRL register for it!

The constants you pass to options are in the new USART_constants file.

Single bitfields  - not all have currently been supported
```text
SERIAL_PARITY_EVEN
SERIAL_PARITY_ODD
SERIAL_PARITY_NONE
SERIAL_STOP_BIT_1
SERIAL_STOP_BIT_2
SERIAL_DATA_5
SERIAL_DATA_6
SERIAL_DATA_7
SERIAL_DATA_8
SERIAL_MODE_ASYNC
```

These options combine a few of the above options together in order to specify a combination of data size, stop bit, and parity, like the cosntants that have been traditionally used.
You should always pass at least one of these as options (if you forget to, that looks the same as requesting 5N1 - except that these include a flag in the high byte of the options as well; the low byte is 0 but the flag is set, we know you asked for 5N1. IF it's not, you didn't specify any character size, and you probably wantted 8, not 5. )
```text
SERIAL_5N1
SERIAL_6N1
SERIAL_7N1
SERIAL_8N1
SERIAL_5N2
SERIAL_6N2
SERIAL_7N2
SERIAL_8N2
SERIAL_5E1
SERIAL_6E1
SERIAL_7E1
SERIAL_8E1
SERIAL_5E2
SERIAL_6E2
SERIAL_7E2
SERIAL_8E2
SERIAL_5O1
SERIAL_6O1
SERIAL_7O1
SERIAL_8O1
SERIAL_5O2
SERIAL_6O2
SERIAL_7O2
SERIAL_8O2
```

You can bitwise or them with these
```text
SERIAL_RS485
SERIAL_OPENDRAIN
SERIAL_LOOPBACK
SERIAL_TX_ONLY
SERIAL_RX_ONLY
SERIAL_EVENT_RX
SERIAL_HALF_DUPLEX (shorthand for open drain loopback)
```


I'm hoping for enlightenment regarding the Event Input mode tomorrow, and then the serial stuff will sorted out.





## Serial performance (or lack therof) and bloat
These re some thoughts from attempting to reimplement the ISRs in assembly.
Both large ISRs have the primary problem, that is, the isr calling a function and generating excessive prologue and epilogue.

These are some unedited musings

*an attempt to put the annoying part in assembler. can NOT considered to be reliable.
also, since the compiler has to push/pop r28/r29 it ends up as big as the C version
Is probably faster though, since every Y+x here is replaced by ADIW X, x; ST/LD X, y; SBIW X, x in C.
the assembly basically cuts 4 cycles on every memory access...
It would be better to implement the assembler interrupt routines in a separate .S file*

Comment on the problem itself: I remember reading mention of how the compiler treated the three pointer registers the same until the very end.... except of course, they're not the same, Y and Z can do displacement, X cannot (AVR didn't have enough opcodes ldd/std Z and ldd/std Y each take up 1/16th of possible opcodes (if you're wondering, the "_________ immediate" instructions that work on r16-r31 only each also take up 1/16th of the flash and there are like 6 of them or something; as do rjmp and rcall (in/out together do. That's the price you pay for having an instruction set simple enough that you can memorize it without too much difficulty). Z gets used first generally, and Y uusually has a pointer to the stack frame

```c++
  asm volatile (
    "MOVW R30, %0   \n\t"   // make sure the uartClass is actually in Z
    "LDD  R28, Z+12 \n\t"   // load USART module pointer
    "LDD  R29, Z+13 \n\t"   // same as above, high byte
    "LDI  R18, 0x40 \n\t"   // load USART_TXCIF_bm
    "STD  Y+4, R18  \n\t"   // and store to STATUS
    "STD  Y+2, %1   \n\t"   // store c
    "LDD  R18, Z+21 \n\t"   // load head
    "CPSE R18, %2   \n\t"   // compare with new tail
    "RJMP end       \n\t"
    "LDD  R18, Y+5  \n\t"   // load CTRLA
    "ANDI R18, 0xDF \n\t"   // &= (~USART_DREIE_bm)
    "STD  Y+5, R18  \n\t"   // store to CTRLA
    "end:           \n\t"
    "STD  Z+22, %2  \n\t"   // store new tail to uartClass._tx_buffer_tail
    :
    : "m" (uartClass), "r" (c), "r" (txTail)
    : "r18", "r28", "r29", "r30", "r31"
    );
```

But that doesn't exactly solve the problem - a big part of the problem is that the actual ISR calls a function.
That is, the problem isn't just this:
```c++
_tx_data_empty_irq():
 48a: cf 93         push  r28
 48c: df 93         push  r29
 48e: fc 01         movw  r30, r24
 490: a4 85         ldd r26, Z+12 ; 0x0c
 492: b5 85         ldd r27, Z+13 ; 0x0d
 494: 96 89         ldd r25, Z+22 ; 0x16
 496: ef 01         movw  r28, r30
 498: c9 0f         add r28, r25
 49a: d1 1d         adc r29, r1
 49c: c9 5a         subi  r28, 0xA9 ; 169
 49e: df 4f         sbci  r29, 0xFF ; 255
 4a0: 88 81         ld  r24, Y
 4a2: 20 e4         ldi r18, 0x40 ; 64
 4a4: 14 96         adiw  r26, 0x04 ; 4    // Yeesh, I see why this makes you want to reimplement it.
 4a6: 2c 93         st  X, r18             // I remember reading that the way the X register is understood by the compiler
 4a8: 14 97         sbiw  r26, 0x04 ; 4    // and uses all three pointer registers like thy could displace, and the
 4aa: 12 96         adiw  r26, 0x02 ; 2    // virtual ldd/std instructions then get transmuted into this crap.
 4ac: 8c 93         st  X, r24             // I understand, compilers are hard, but like... ytou'd think that they could
 4ae: 12 97         sbiw  r26, 0x02 ; 2    // combine the slow word add/sub instructions from two adjacent STD X virtual instructions wouldn't you?
 4b0: 9f 5f         subi  r25, 0xFF ; 255
 4b2: 9f 73         andi  r25, 0x3F ; 63
 4b4: 85 89         ldd r24, Z+21 ; 0x15
 4b6: 89 13         cpse  r24, r25
 4b8: 06 c0         rjmp  .+12      ; 0x4c6 <UartClass::_tx_data_empty_irq(UartClass&)+0x3c>
 4ba: 15 96         adiw  r26, 0x05 ; 5
 4bc: 8c 91         ld  r24, X
 4be: 15 97         sbiw  r26, 0x05 ; 5
 4c0: 8f 7d         andi  r24, 0xDF ; 223
 4c2: 15 96         adiw  r26, 0x05 ; 5
 4c4: 8c 93         st  X, r24
 4c6: 96 8b         std Z+22, r25 ; 0x16
 4c8: df 91         pop r29
 4ca: cf 91         pop r28
 4cc: 08 95         ret
```
It's also that it gets called by this:
```c++
00000882 <__vector_22>:
__vector_22():
 882: 1f 92         push  r1
 884: 0f 92         push  r0
 886: 0f b6         in  r0, 0x3f  ; 63
 888: 0f 92         push  r0
 88a: 11 24         eor r1, r1
 88c: 0b b6         in  r0, 0x3b  ; 59
 88e: 0f 92         push  r0
 890: 2f 93         push  r18
 892: 3f 93         push  r19
 894: 4f 93         push  r20
 896: 5f 93         push  r21
 898: 6f 93         push  r22
 89a: 7f 93         push  r23
 89c: 8f 93         push  r24
 89e: 9f 93         push  r25
 8a0: af 93         push  r26
 8a2: bf 93         push  r27
 8a4: ef 93         push  r30
 8a6: ff 93         push  r31
 8a8: 8c ec         ldi r24, 0xCC ; 204
 8aa: 90 e4         ldi r25, 0x40 ; 64
 8ac: ee dd         rcall .-1060    ; 0x48a <UartClass::_tx_data_empty_irq(UartClass&)>
 8ae: ff 91         pop r31
 8b0: ef 91         pop r30
 8b2: bf 91         pop r27
 8b4: af 91         pop r26
 8b6: 9f 91         pop r25
 8b8: 8f 91         pop r24
 8ba: 7f 91         pop r23
 8bc: 6f 91         pop r22
 8be: 5f 91         pop r21
 8c0: 4f 91         pop r20
 8c2: 3f 91         pop r19
 8c4: 2f 91         pop r18
 8c6: 0f 90         pop r0
 8c8: 0b be         out 0x3b, r0  ; 59
 8ca: 0f 90         pop r0
 8cc: 0f be         out 0x3f, r0  ; 63
 8ce: 0f 90         pop r0
 8d0: 1f 90         pop r1
 8d2: 18 95         reti
```

Notice that the function doesn't use r19, r20, r21, r22 or r23. Or r0, for that matter. 0x3b is RAMPZ. Sometimes it likes to save and restore that one too. I dunno on what criteria either, since I swear i've seen interrupts not save and restore it *shrug* but it does even on 128k parts
That doesn't matter - they're call used they get saved and restored.
And r1 is always saved, cleared and restored unless the ISR is naked - which *does* make sense, at least.
To bring joy to people on small-flash parts, there's one of those bloated things for both ISRs on each serial port. Whatyou eant to do is implement it in full assembly with  single routine that handkes all cases reasonably efficientlky,

  I think I have a solution though - though it's a little scary how I enter it. It relies on two dubious things which I've done elsewhere, though not extensively.
They are:
Jumps and calls from anywhere to anywhere seem to work, regardless of what file the label is in, I thought there were restrictions, but it doesn;t seem like there are. M Flash library is able to call EntryPointSPM by name successfully (I thought I'd have to figure out a way to get it's address, but nope, worked fine... o_o.  to wrrite toall but the first5bit512b of flas yiu need to be executing from the first 512b (BOOTSIZE=1, if there is no biotlkioader involved, ) has to be located in .trampolines because I needed to find a section that would always be in the first 512b because I needed the fake bootloader section to be as small as possible to maximize the flexibility of the no-restrictions flash from app mode. But putting it in init didn't do that if the user had variables declared PROGMEM. I dunno what is supposed to go there, but whatever it is, it's more important to the linker to put those things at low addresses than progmem, but progmem is more important to keep at low address than .initn. (That rjmp is there so that if/when execution stumbles onto this, it'll jump over it instead of trying to write r0 and r1 to whatever location in flash the z is pointing at and then return to wherever the top two bytes of the stack point to, probably 0000 leading to a bootloop at startup)

```c++
  void __spm_entrypoint (void) __attribute__ ((naked)) __attribute__((used)) __attribute__ ((section (".trampolines")));
  void __spm_entrypoint (void)
  {
    __asm__ __volatile__(
              "rjmp .+4"                "\n\t" // Jump over this if we got here the wrong way
             "EntryPointSPM:"           "\n\t" // this is the label we call
              "spm z+"                  "\n\t" // write r0, r1 to location pointed to by r30,r31, with post increment
              "ret"::);                        // and then return.
  }
```
The part which is **definitely** sketchy is ducking out of assembly to grab a global variable address in a constraint and immediately returning asm, while `((naked))` and unable to actually do anything safely in c; it seems to work - all that we need the compiler to do is generate 2 LDI's, we just don't know what they are a priori. I do it to grab the function table pointer in my split-apart attachInterrupt kludge in the same way (refer to the github version not the released version, as I looked at it today and fixed some problems, and because I'd forgotten I could jump to labels no matter where they were I was working around that instead of just jumping. )

So here my thinking:
  Declare the ISRs that exist in multiplicate and need to all run one function with
  NAKED_ISR(). Each one goes something like:
```c++
  ISR(HWSERIAL0_DRE_VECTOR, ISR_NAKED) {
    asm volatile (
      "push r30"      "\n\t" // push the z reg onto stack within this
      "push r31"      "\n\t" // block of assenbly, gotta make room...
      :                      // then drop back into C, sort of...
      :                      // but all you do there is grab
    );                       // the address of the object by passing
    asm volatile (           // it to inline assembly in the z
      "jmp HWSerialDREISR"   //
      :                      //
      : "z" (Serial0)        //
    );
  }
```

So then, your **actual** isr, is a function declared (naked) (used)
and all it has in it is yet another block of inline assembly
and you should find the class instance address in Z....

```c++
  asm volatile (
   "HWSerialDREISR:"  "\n\t"    //
    "push r18"        "\n\t"    //
    "in   r18, 0x3F"  "\n\t"    // save sreg
    "push r18"        "\n\t"    // (also need to save clear and restore r1 if using that or mul insn)
    "push r19"        "\n\t"    // tail goes here
    "push r20"        "\n\t"    // and the character can go here
    "push r28"        "\n\t"    //
    "push r29"        "\n\t"    //
    "ldd r19, Z+22"   "\n\t"    // okay, "tail to r19"? Okay, tail to r19...
    //.. somehow find character we want to send ....
    // and put it into r20, then pick up from your code
    "ldd  r28, Z+12"  "\n\t"    // load USART module pointer
    "ldd  r29, Z+13"  "\n\t"    // same as above, high byte
    "ldi  r18, 0x40"  "\n\t"    // load USART_TXCIF_bm
    "std  Y+4, r18"   "\n\t"    // and store to STATUS
    "std  Y+2, r20"   "\n\t"    // store c
    "inc r19"         "\n\t"    // increment tail - unless done above
    "andi r19, 63"    "\n\t"    // 64b buffer wraparound - continue your code
    "ldd  r18, Z+21"  "\n\t"    // load head
    "cpse r18, %2"    "\n\t"    // compare with new tail
    "rjmp DREend"     "\n\t"    // Maybe better not name labels so generically, since they seem to have global scope O_o
    "ldd  r18, Y+5"   "\n\t"    // load CTRLA
    "andi r18, 0xDF"  "\n\t"    // &= (~USART_DREIE_bm)
    "std  Y+5, r18"   "\n\t"    // store to CTRLA
   "DREend:"          "\n\t"    //
    "std  Z+22, %2"   "\n\t"    // store new tail to uartClass._tx_buffer_tail
    "pop r29"         "\n\t"    // now, we're naked so we have to clean up
    "pop r28"         "\n\t"    // pop everything pushed in reverse order
    "pop r20"         "\n\t"    //
    "pop r19"         "\n\t"    //
    "pop r18"         "\n\t"    //
    "out 0x3F"        "\n\t"    // restore the SREG
    "pop r18"         "\n\t"    //
    "pop r31"         "\n\t"    // Now clean up after the naked isr
    "pop r30"         "\n\t"    // so the rest of the app won't know
    "reti"            "\n"      // and execute a reti....
  );
```

## Serial Event
1.4.0 pulls serial even entirely.
The code removed is shown here:

```c++
  #if defined(ENABLE_SERIAL_EVENT)
    #if defined(HWSERIAL0)
      void serialEvent() __attribute__((weak));
      bool Serial0_available() __attribute__((weak));
    #endif

    #if defined(HWSERIAL1)
      void serialEvent1() __attribute__((weak));
      bool Serial1_available() __attribute__((weak));
    #endif

    #if defined(HWSERIAL2)
      void serialEvent2() __attribute__((weak));
      bool Serial2_available() __attribute__((weak));
    #endif

    #if defined(HWSERIAL3)
      void serialEvent3() __attribute__((weak));
      bool Serial3_available() __attribute__((weak));
    #endif

    #if defined(HWSERIAL4)
      void serialEvent4() __attribute__((weak));
      bool Serial4_available() __attribute__((weak));
    #endif

    #if defined(HWSERIAL5)
      void serialEvent5() __attribute__((weak));
      bool Serial5_available() __attribute__((weak));
    #endif

    void serialEventRun(void) {
    #if defined(HWSERIAL0)
      if (Serial0_available && serialEvent && Serial0_available()) serialEvent();
    #endif
    #if defined(HWSERIAL1)
      if (Serial1_available && serialEvent1 && Serial1_available()) serialEvent1();
    #endif
    #if defined(HWSERIAL2)
      if (Serial2_available && serialEvent2 && Serial2_available()) serialEvent2();
    #endif
    #if defined(HWSERIAL3)
      if (Serial3_available && serialEvent3 && Serial3_available()) serialEvent3();
    #endif
    #if defined(HWSERIAL4)
      if (Serial4_available && serialEvent4 && Serial4_available()) serialEvent4();
    #endif
    #if defined(HWSERIAL5)
      if (Serial5_available && serialEvent5 && Serial5_available()) serialEvent5();
    #endif
    }
  #endif
```

That also meant we could pull ourt those weird available functions, which were to trick the compiler into not including unused serial events
```c++
  bool Serial3_available() {
    return Serial3.available();
  }
```
