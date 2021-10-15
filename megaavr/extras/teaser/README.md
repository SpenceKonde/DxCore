# Teaser for the upcoming improvements to memory map output format
Because wehat we have is nearly usefulss the formatting is such garbage.

The objectives are - combine a wider column abbreviated names  to not haver the first column maje an unrecognizable mess out of the rest of it. Truncate the file paths. Nobody needs to know where avr-gcc object libraries were built, the source isn't gonna be there on your computer! And very few people will bother Type has too wide of a colun, there line number is mishabndled , and they tend to get destrotyed when imported to spteradsheets

It dates to an era of narrow 80 column terminals and everything having shoert names. Neither of thoser hold true today. Since we now pull in python anyway, we sh9uld to9rally be able to pass two file bames to it, such that if shit happened, it would just nor get rthe improvement (ie, do it after everything elser)

My currently plan is that one map file will be made normally. From that we will then try to make a human readable version and a .csv for spreadsheet tools. A failure then would just show errors but still get you the version of thile you geet now.

I think it should also be possibkle to fix the "useless address" issue in the assenmbly listings, Where it calculates relative offsets RAMand Registers to fill in as comments. Registers are described relative to the lengths of sections in a different address soace and the like. And it's not hard to do a better hjob there. Could also count indtructions in there and generate a little repor. I did two samples, on of just over 1k instructions another of just over 5k....  to the end of the programmory, many parts of flash are described in relative to EEPROM/FUSE/LOCK addresses. As I said, it's obviously wrong, and the worst offenders are easy to detect and reoplace with something more less seless. Even just showing there 16-bit address would be great, And these parts are so consistent that can hardly hekop but know where everything is.

```
 I wonder how the distributions vary by compiler version? by author? By ratio of C to C++?
 It would be very interedting to look at the differences between AVRe and AVRxt and be like "Okay, How mucch of a difference do these changes make? Did the LDS downgrade hurt, or is it jusst verty visible in a few places where we look at assembly, but not used much over whole programs?"

Certainly for AVRxt one sample I grabbed points to it being a very good deal for us.
  More than 1/3rd of instructions were either push, pop, or ldi in both damples!. ldi is no surprise, but push and pop were. 139 ldi, 139, push, and (alarmingly) only 136 pops....
  But push and pop are clumpy. ISRs and badly wwritten functions with unreasonable numbers of arguments tend to use them a lot.
  Tight, hand-optimized assembly - which is what humans are most involved in - doesn't make heavty use of them. They are, in effect, a weay to deal with the lack of hand optimization modt code faces.
  The next two were another huge surprise. I'm used to being annoyed at missed opportunities to std/ldd. But in fact, they were the 4th and 5th most used.
  They were followed by movw and sts, with population falling to half of those for anything outside of those top 7, which made up 63% of the sample
  Skips formed 1% of the sample. Branches 2.6%, only breq, brne, brcc abd brcs were used.
With another larger sample, we confirmed much of this puicture:
  This time it was ldi 14%, movw 10% pop 8% push 7.3% ldd at 6% and std at 5.5.
  The top 6 were unchanged - but then it shifted dramatically. This sample used rcall at about 4% of the time.
  The other one? Not evena single time. Looks like that must have dated to pre-mrelax.... meanwhile this one has not a single call.
  Is the compiler incapable of generating output that contains both rcall and call? Obviously rcall is preferable - but only when everything can be done by it, which is someetimes but not always the case.
  anyway, lds was #13 with 2.1% here.
  Skips formed 0.8% of the sample, and branches 4.1%.
  brne, breq brcc brcs formed the majority, bur there were 10 brge's and a single brpl

In neither case did the total number of different instructions ued exceed 60. Amazing that half of the instuctions never end upo gertting used.d
```


Old one:
```


Symbols from C:\Users\Spence\AppData\Local\Temp\arduino_build_870305/dummy.ino.elf:

Name                  Value   Class        Type         Size     Line  Section

__heap_end          |00000000|   W  |            NOTYPE|        |     |*ABS*
__TEXT_REGION_ORIGIN__|00000000|   W  |            NOTYPE|        |     |*ABS*
__tmp_reg__         |00000000|   a  |            NOTYPE|        |     |*ABS*
__vector_default    |00000000|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:61
__vectors           |00000000|   T  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:61
__zero_reg__        |00000001|   a  |            NOTYPE|        |     |*ABS*
__FUSE_REGION_LENGTH__|00000010|   A  |            NOTYPE|        |     |*ABS*
__CCP__             |00000034|   a  |            NOTYPE|        |     |*ABS*
__RAMPZ__           |0000003b|   a  |            NOTYPE|        |     |*ABS*
__SP_L__            |0000003d|   a  |            NOTYPE|        |     |*ABS*
__SP_H__            |0000003e|   a  |            NOTYPE|        |     |*ABS*
__SREG__            |0000003f|   a  |            NOTYPE|        |     |*ABS*
__ctors_start       |00000100|   T  |            NOTYPE|        |     |.text
__trampolines_end   |00000100|   T  |            NOTYPE|        |     |.text
__trampolines_start |00000100|   T  |            NOTYPE|        |     |.text
__ctors_end         |00000102|   T  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:230
__dtors_end         |00000102|   T  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:230
__dtors_start       |00000102|   T  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:230
__init              |00000102|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:230
_pre_main()         |0000010e|   T  |              FUNC|00000014|     |.text  C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore/main.cpp:112
__do_copy_data      |00000122|   T  |            NOTYPE|0000001a|     |.text  /home/admin/build/toolchain-avr-special/gcc-build/avr/avrxmega4/libgcc/../../../../gcc/libgcc/config/avr/lib1funcs.S:2373
__do_clear_bss      |0000013c|   T  |            NOTYPE|00000010|     |.text  /home/admin/build/toolchain-avr-special/gcc-build/avr/avrxmega4/libgcc/../../../../gcc/libgcc/config/avr/lib1funcs.S:2441
.do_clear_bss_loop  |00000144|   t  |            NOTYPE|        |     |.text  /home/admin/build/toolchain-avr-special/gcc-build/avr/avrxmega4/libgcc/../../../../gcc/libgcc/config/avr/lib1funcs.S:2446
.do_clear_bss_start |00000146|   t  |            NOTYPE|        |     |.text  /home/admin/build/toolchain-avr-special/gcc-build/avr/avrxmega4/libgcc/../../../../gcc/libgcc/config/avr/lib1funcs.S:2448
__do_global_ctors   |0000014c|   T  |            NOTYPE|00000016|     |.text  /home/admin/build/toolchain-avr-special/gcc-build/avr/avrxmega4/libgcc/../../../../gcc/libgcc/config/avr/lib1funcs.S:2466
__bad_interrupt     |0000016a|   T  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_1          |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_10         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_11         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_12         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_13         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_14         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_15         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_16         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_17         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_18         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_2          |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_21         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_22         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_23         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_24         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_25         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_26         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_27         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_28         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_29         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_3          |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_31         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_32         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_33         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_34         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_35         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_36         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_37         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_38         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_39         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_4          |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_40         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_41         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_42         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_43         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_44         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_45         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_46         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_47         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_48         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_49         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_5          |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_50         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_51         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_52         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_53         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_54         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_55         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_56         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_57         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_58         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_59         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_6          |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_60         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_61         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_62         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_63         |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_7          |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_8          |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
__vector_9          |0000016a|   W  |            NOTYPE|        |     |.text  ../../../../crt1/gcrt1.S:209
SPIClass::endTransaction() [clone .constprop.39]|0000016c|   t  |              FUNC|0000015a|     |.text  C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\libraries\SPI\src/SPI.cpp:511
__EEPROM_REGION_LENGTH__|00000200|   A  |            NOTYPE|        |     |*ABS*
SPIClass::transfer(unsigned char) [clone .constprop.38]|000002c6|   t  |              FUNC|00000016|     |.text C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\libraries\SPI\src/SPI.cpp:545
SPIClass::transfer(void*, unsigned int) [clone .constprop.37]|000002dc|   t  |              FUNC|00000028|     |.text C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\libraries\SPI\src/SPI.cpp:568
SPIClass::beginTransaction(SPISettings) [clone .constprop.35]|00000304|   t  |              FUNC|00000184|     |.text C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\libraries\SPI\src/SPI.cpp:499
__LOCK_REGION_LENGTH__|00000400|   A  |            NOTYPE|        |     |*ABS*
__SIGNATURE_REGION_LENGTH__|00000400|   A  |            NOTYPE|        |     |*ABS*
__USER_SIGNATURE_REGION_LENGTH__|00000400|   A  |            NOTYPE|        |     |*ABS*
EthernetUDP::available()|00000488|   t  |              FUNC|00000008|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src/EthernetUdp.cpp:50
EthernetUDP::flush()|00000490|   t  |              FUNC|00000002|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src/EthernetUdp.cpp:179
UartClass::availableForWrite()|00000492|   t  |              FUNC|0000001e|     |.text  C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore/UART.cpp:337
UartClass::read()   |000004b0|   t  |              FUNC|00000028|     |.text  C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore/UART.cpp:321
UartClass::peek()   |000004d8|   t  |              FUNC|0000001c|     |.text  C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore/UART.cpp:313
UartClass::available()|000004f4|   t  |              FUNC|00000018|     |.text  C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore/UART.cpp:309
UartClass::end()    |0000050c|   t  |              FUNC|00000032|     |.text  C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore/UART.cpp:293
UartClass::_tx_data_empty_irq()|0000053e|   t  |              FUNC|00000056|     |.text C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore/UART.cpp:107
UartClass::_poll_tx_data_empty()|00000594|   t  |              FUNC|0000001e|     |.text  C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore/UART.cpp:135
UartClass::write(unsigned char)|000005b2|   t  |              FUNC|00000068|     |.text C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore/UART.cpp:374
UartClass::flush()  |0000061a|   t  |              FUNC|00000028|     |.text  C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore/UART.cpp:346
Print::write(unsigned char const*, unsigned int)|00000642|   t  |              FUNC|0000005a|     |.text  C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore\api/Print.cpp:30
UartClass::operator bool()|0000069c|   t  |              FUNC|00000004|     |.text  C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore/UART.h:200
micros              |000006a0|   t  |              FUNC|0000007e|     |.text  C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore/wiring.c:204
delay               |0000071e|   t  |              FUNC|00000060|     |.text  C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore/wiring.c:390
millis              |0000077e|   t  |              FUNC|00000018|     |.text  C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore/wiring.c:176
W5100Class::resetSS()|00000796|   t  |              FUNC|00000010|     |.text C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore/wiring.c:200
W5100Class::setSS() |000007a6|   t  |              FUNC|00000010|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src\utility/w5100.h:376
W5100Class::write(unsigned int, unsigned char const*, unsigned int)|000007b6|   t  |              FUNC|00000138|     |.text C:\arduino-1.8.13\libraries\Ethernet\src\utility/w5100.h:372
W5100Class::writeMR(unsigned char)|000008ee|   t  |              FUNC|00000024|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src\utility/w5100.cpp:380
W5100Class::RBASE(unsigned char)|00000912|   t  |              FUNC|0000001c|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src\utility/w5100.h:193
W5100Class::writeSn(unsigned char, unsigned int, unsigned char*, unsigned int)|0000092e|   t  |              FUNC|00000020|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src\utility/w5100.h:326
W5100Class::writeSnDPORT(unsigned char, unsigned int)|0000094e|   t  |              FUNC|00000028|     |.text C:\arduino-1.8.13\libraries\Ethernet\src\utility/w5100.h:241
W5100Class::writeSnPORT(unsigned char, unsigned int)|00000976|   t  |              FUNC|00000028|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src\utility/w5100.h:279
W5100Class::writeSn(unsigned char, unsigned int, unsigned char)|0000099e|   t  |              FUNC|00000038|     |.text C:\arduino-1.8.13\libraries\Ethernet\src\utility/w5100.h:276
EthernetUDP::write(unsigned char)|000009d6|   t  |              FUNC|0000002c|     |.text C:\arduino-1.8.13\libraries\Ethernet\src\utility/w5100.h:236
EthernetServer::write(unsigned char)|00000a02|   t  |              FUNC|0000002c|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src/EthernetUdp.cpp:89
EthernetClient::operator==(EthernetClient const&)|00000a2e|   t  |              FUNC|00000016|     |.text C:\arduino-1.8.13\libraries\Ethernet\src/EthernetServer.cpp:159
EthernetClient::write(unsigned char)|00000a44|   t  |              FUNC|0000002c|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src/EthernetClient.cpp:174
EthernetClient::setConnectionTimeout(unsigned int)|00000a70|   t  |              FUNC|00000008|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src/EthernetClient.cpp:81
EthernetClient::operator!=(EthernetClient const&)|00000a78|   t  |              FUNC|00000014|     |.text C:\arduino-1.8.13\libraries\Ethernet\src/Ethernet.h:241
EthernetClient::operator!=(bool)|00000a8c|   t  |              FUNC|00000004|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src/Ethernet.h:236
EthernetClient::operator==(bool)|00000a90|   t  |              FUNC|00000006|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src/Ethernet.h:236
EthernetClient::operator bool()|00000a96|   t  |              FUNC|00000010|     |.text C:\arduino-1.8.13\libraries\Ethernet\src/Ethernet.h:233
EthernetUDP::localPort()|00000aa6|   t  |              FUNC|00000008|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src/Ethernet.h:232
EthernetUDP::remotePort()|00000aae|   t  |              FUNC|00000008|     |.text C:\arduino-1.8.13\libraries\Ethernet\src/Ethernet.h:208
EthernetUDP::remoteIP()|00000ab6|   t  |              FUNC|00000020|     |.text C:\arduino-1.8.13\libraries\Ethernet\src/Ethernet.h:207
EthernetUDP::read(char*, unsigned int)|00000ad6|   t  |              FUNC|0000000e|     |.text  C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore\api/IPAddress.h:28
UartClass::begin(unsigned long)|00000ae4|   t  |              FUNC|00000012|     |.text C:\arduino-1.8.13\libraries\Ethernet\src/Ethernet.h:199
Print::flush()      |00000af6|   t  |              FUNC|00000002|     |.text  C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore/UART.h:170
Print::availableForWrite()|00000af8|   t  |              FUNC|00000006|     |.text  C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore/UART.h:170
pinMode             |00000afe|   t  |              FUNC|00000066|     |.text  C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore/wiring_digital.c:135
UartClass::begin(unsigned long, unsigned int)|00000b64|   t  |              FUNC|000000de|     |.text C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore/wiring_digital.c:149
SPISettings::SPISettings(unsigned long, unsigned char, unsigned char) [clone .constprop.33]|00000c42|   t  |              FUNC|0000000c|     |.text C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore/UART.cpp:291
EthernetUDP::beginPacket(IPAddress, unsigned int)|00000c4e|   t  |              FUNC|00000088|     |.text C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\libraries\SPI\src/SPI.h:225
Print::write(char const*) [clone .part.2]|00000cd6|   t  |              FUNC|0000001e|     |.text C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore\api/Print.h:47
Print::printNumber(unsigned long, unsigned char)|00000cf4|   t  |              FUNC|0000008a|     |.text  C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore\api/Print.h:49
Print::print(int, int) [clone .constprop.18]|00000d7e|   t  |              FUNC|00000066|     |.text  C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore\api/Print.cpp:245
IPAddress::printTo(Print&) const|00000de4|   t  |              FUNC|00000090|     |.text  C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore\api/Print.cpp:96
Print::println(char const*)|00000e74|   t  |              FUNC|00000024|     |.text C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore\api/IPAddress.cpp:115
W5100Class::read(unsigned int, unsigned char*, unsigned int)|00000e98|   t  |              FUNC|000000f8|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src\utility/w5100.cpp:391
EthernetClass::socketPeek(unsigned char)|00000f90|   t  |              FUNC|00000050|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src/socket.cpp:367
EthernetUDP::peek() |00000fe0|   t  |              FUNC|0000001c|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src/socket.cpp:374
EthernetClient::peek()|00000ffc|   t  |              FUNC|00000032|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src/EthernetUdp.cpp:174
W5100Class::read(unsigned int)|0000102e|   t  |              FUNC|00000020|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src/EthernetClient.cpp:113
W5100Class::readMR()|0000104e|   t  |              FUNC|00000006|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src\utility/w5100.h:162
W5100Class::softReset() [clone .part.0]|00001054|   t  |              FUNC|00000028|     |.text C:\arduino-1.8.13\libraries\Ethernet\src\utility/w5100.cpp:205
W5100Class::init() [clone .part.4]|0000107c|   t  |              FUNC|000001f8|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src\utility/w5100.cpp:90
EthernetClass::begin(unsigned char*, IPAddress) [clone .constprop.20]|00001274|   t  |              FUNC|00000156|     |.text C:\arduino-1.8.13\libraries\Ethernet\src\utility/w5100.h:155
W5100Class::readSn(unsigned char, unsigned int)|000013ca|   t  |              FUNC|0000001a|     |.text C:\arduino-1.8.13\libraries\Ethernet\src/Ethernet.cpp:64
W5100Class::execCmdSn(unsigned char, SockCMD)|000013e4|   t  |              FUNC|0000001c|     |.text C:\arduino-1.8.13\libraries\Ethernet\src\utility/w5100.h:232
EthernetClass::socketDisconnect(unsigned char)|00001400|   t  |              FUNC|00000034|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src\utility/w5100.h:273
EthernetClass::socketClose(unsigned char)|00001434|   t  |              FUNC|00000034|     |.text C:\arduino-1.8.13\libraries\Ethernet\src/socket.cpp:253
EthernetUDP::stop() |00001468|   t  |              FUNC|00000018|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src/socket.cpp:213
W5100Class::readSnSR(unsigned char)|00001480|   t  |              FUNC|00000006|     |.text C:\arduino-1.8.13\libraries\Ethernet\src/EthernetUdp.cpp:55
EthernetClass::socketStatus(unsigned char)|00001486|   t  |              FUNC|00000034|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src/EthernetUdp.cpp:56
EthernetServer::operator bool()|000014ba|   t  |              FUNC|00000056|     |.text C:\arduino-1.8.13\libraries\Ethernet\src/socket.cpp:202
EthernetClient::connected()|00001510|   t  |              FUNC|00000042|     |.text C:\arduino-1.8.13\libraries\Ethernet\src/EthernetServer.cpp:121
EthernetClient::stop()|00001552|   t  |              FUNC|00000066|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src/EthernetClient.cpp:161
EthernetUDP::endPacket()|000015b8|   t  |              FUNC|00000082|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src/EthernetClient.cpp:147
W5100Class::readSn(unsigned char, unsigned int, unsigned char*, unsigned int)|0000163a|   t  |              FUNC|00000020|     |.text C:\arduino-1.8.13\libraries\Ethernet\src/socket.cpp:528
EthernetClass::socketBeginMulticast(unsigned char, IPAddress, unsigned int) [clone .constprop.9]|0000165a|   t  |              FUNC|000001b6|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src\utility/w5100.h:226
EthernetUDP::beginMulticast(IPAddress, unsigned int)|00001810|   t  |              FUNC|0000007a|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src/socket.cpp:196
write_data(unsigned char, unsigned int, unsigned char const*, unsigned int)|0000188a|   t  |              FUNC|000000e8|     |.text C:\arduino-1.8.13\libraries\Ethernet\src/EthernetUdp.cpp:188
getSnTX_FSR(unsigned char)|00001972|   t  |              FUNC|00000074|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src/socket.cpp:415
EthernetClass::socketSendAvailable(unsigned char)|000019e6|   t  |              FUNC|00000052|     |.text C:\arduino-1.8.13\libraries\Ethernet\src/socket.cpp:395
EthernetClient::flush()|00001a38|   t  |              FUNC|00000026|     |.text C:\arduino-1.8.13\libraries\Ethernet\src/socket.cpp:481
EthernetClient::availableForWrite()|00001a5e|   t  |              FUNC|00000010|     |.text C:\arduino-1.8.13\libraries\Ethernet\src/EthernetClient.cpp:128
EthernetClass::socketSend(unsigned char, unsigned char const*, unsigned int)|00001a6e|   t  |              FUNC|000000e0|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src/EthernetClient.cpp:74
EthernetClient::write(unsigned char const*, unsigned int)|00001b4e|   t  |              FUNC|00000034|     |.text C:\arduino-1.8.13\libraries\Ethernet\src/socket.cpp:461
EthernetUDP::write(unsigned char const*, unsigned int)|00001b82|   t  |              FUNC|0000008c|     |.text  C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore\api/Print.h:39
getSnRX_RSR(unsigned char)|00001c0e|   t  |              FUNC|00000064|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src/EthernetUdp.cpp:97
EthernetClass::socketRecvAvailable(unsigned char)|00001c72|   t  |              FUNC|00000068|     |.text C:\arduino-1.8.13\libraries\Ethernet\src/socket.cpp:271
EthernetServer::available()|00001cda|   t  |              FUNC|00000108|     |.text C:\arduino-1.8.13\libraries\Ethernet\src/socket.cpp:357
EthernetServer::write(unsigned char const*, unsigned int)|00001de2|   t  |              FUNC|0000009c|     |.text C:\arduino-1.8.13\libraries\Ethernet\src/EthernetServer.cpp:66
EthernetClient::available()|00001e7e|   t  |              FUNC|00000010|     |.text C:\arduino-1.8.13\libraries\Ethernet\src/EthernetServer.cpp:179
EthernetClass::socketRecv(unsigned char, unsigned char*, int)|00001e8e|   t  |              FUNC|000001a2|     |.text C:\arduino-1.8.13\libraries\Ethernet\src/EthernetServer.cpp:179
EthernetUDP::read(unsigned char*, unsigned int)|00002030|   t  |              FUNC|0000003c|     |.text C:\arduino-1.8.13\libraries\Ethernet\src\utility/w5100.h:290
EthernetUDP::read() |0000206c|   t  |              FUNC|00000052|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src/EthernetUdp.cpp:151
EthernetClient::read()|000020be|   t  |              FUNC|00000032|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src/EthernetUdp.cpp:135
EthernetClient::read(unsigned char*, unsigned int)|000020f0|   t  |              FUNC|00000010|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src/EthernetClient.cpp:119
EthernetClass::socketBegin(unsigned char, unsigned int)|00002100|   t  |              FUNC|0000016c|     |.text C:\arduino-1.8.13\libraries\Ethernet\src/EthernetClient.cpp:121
EthernetUDP::begin(unsigned int)|0000226c|   t  |              FUNC|00000038|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src/socket.cpp:124
EthernetServer::begin()|000022a4|   t  |              FUNC|00000076|     |.text C:\arduino-1.8.13\libraries\Ethernet\src/EthernetUdp.cpp:41
EthernetClient::connect(IPAddress, unsigned int)|0000231a|   t  |              FUNC|00000120|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src/EthernetServer.cpp:33
EthernetClient::remotePort()|0000243a|   t  |              FUNC|0000005e|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src/EthernetClient.cpp:65
EthernetClient::localPort()|00002498|   t  |              FUNC|0000005e|     |.text C:\arduino-1.8.13\libraries\Ethernet\src\utility/w5100.h:279
EthernetUDP::parsePacket()|000024f6|   t  |              FUNC|000000ac|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src\utility/w5100.h:276
DNSClient::getHostByName(char const*, IPAddress&, unsigned int) [clone .constprop.10]|000025a2|   t  |              FUNC|00000480|     |.text C:\arduino-1.8.13\libraries\Ethernet\src/EthernetUdp.cpp:120
EthernetUDP::beginPacket(char const*, unsigned int)|00002a22|   t  |              FUNC|000000d8|     |.text C:\arduino-1.8.13\libraries\Ethernet\src/Dns.cpp:237
EthernetClient::connect(char const*, unsigned int)|00002afa|   t  |              FUNC|00000108|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src/EthernetUdp.cpp:71
EthernetClient::remoteIP()|00002c02|   t  |              FUNC|0000008e|     |.text  C:\arduino-1.8.13\libraries\Ethernet\src/EthernetClient.cpp:39
__vector_20         |00002c90|   T  |              FUNC|00000054|     |.text  C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore/UART0.cpp:49
__vector_19         |00002ce4|   T  |              FUNC|00000064|     |.text  C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore/UART0.cpp:40
__vector_30         |00002d48|   T  |              FUNC|00000050|     |.text  C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore/wiring.c:128
global constructors keyed to 65535_0_dummy.ino.cpp.o.4248|00002d98|   t  |              FUNC|00000158|     |.text C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore/wiring.c:141
main                |00002ef0|   T  |              FUNC|00000376|     |.text  C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore/main.cpp:27
__muluhisi3         |00003266|   T  |            NOTYPE|00000014|     |.text  /home/admin/build/toolchain-avr-special/gcc-build/avr/avrxmega4/libgcc/../../../../gcc/libgcc/config/avr/lib1funcs.S:626
__udivmodsi4        |0000327a|   T  |            NOTYPE|00000044|     |.text  /home/admin/build/toolchain-avr-special/gcc-build/avr/avrxmega4/libgcc/../../../../gcc/libgcc/config/avr/lib1funcs.S:1638
__udivmodsi4_loop   |00003286|   t  |            NOTYPE|        |     |.text  /home/admin/build/toolchain-avr-special/gcc-build/avr/avrxmega4/libgcc/../../../../gcc/libgcc/config/avr/lib1funcs.S:1646
__udivmodsi4_ep     |000032a0|   t  |            NOTYPE|        |     |.text  /home/admin/build/toolchain-avr-special/gcc-build/avr/avrxmega4/libgcc/../../../../gcc/libgcc/config/avr/lib1funcs.S:1660
__umulhisi3         |000032be|   T  |            NOTYPE|0000001e|     |.text  /home/admin/build/toolchain-avr-special/gcc-build/avr/avrxmega4/libgcc/../../../../gcc/libgcc/config/avr/lib1funcs.S:564
__ashrdi3           |000032dc|   T  |            NOTYPE|00000004|     |.text  /home/admin/build/toolchain-avr-special/gcc-build/avr/avrxmega4/libgcc/../../../../gcc/libgcc/config/avr/lib1funcs.S:3122
__lshrdi3           |000032e0|   T  |            NOTYPE|00000036|     |.text  /home/admin/build/toolchain-avr-special/gcc-build/avr/avrxmega4/libgcc/../../../../gcc/libgcc/config/avr/lib1funcs.S:3131
__cmpdi2_s8         |00003316|   T  |            NOTYPE|00000018|     |.text  /home/admin/build/toolchain-avr-special/gcc-build/avr/avrxmega4/libgcc/../../../../gcc/libgcc/config/avr/lib1funcs.S:2102
__tablejump2__      |0000332e|   T  |            NOTYPE|00000012|     |.text  /home/admin/build/toolchain-avr-special/gcc-build/avr/avrxmega4/libgcc/../../../../gcc/libgcc/config/avr/lib1funcs.S:2296
memset              |00003340|   T  |              FUNC|0000000e|     |.text
_exit               |0000334e|   T  |            NOTYPE|        |     |.text  /home/admin/build/toolchain-avr-special/gcc-build/avr/avrxmega4/libgcc/../../../../gcc/libgcc/config/avr/lib1funcs.S:2278
exit                |0000334e|   W  |            NOTYPE|        |     |.text  /home/admin/build/toolchain-avr-special/gcc-build/avr/avrxmega4/libgcc/../../../../gcc/libgcc/config/avr/lib1funcs.S:2278
__stop_program      |00003350|   t  |            NOTYPE|        |     |.text  /home/admin/build/toolchain-avr-special/gcc-build/avr/avrxmega4/libgcc/../../../../gcc/libgcc/config/avr/lib1funcs.S:2280
__data_load_start   |00003352|   A  |            NOTYPE|        |     |*ABS*
_etext              |00003352|   T  |            NOTYPE|        |     |.text
__data_load_end     |000035c2|   A  |            NOTYPE|        |     |*ABS*
__DATA_REGION_LENGTH__|00004000|   A  |            NOTYPE|        |     |*ABS*
__stack             |00007fff|   W  |            NOTYPE|        |     |*ABS*
__RODATA_PM_OFFSET__|00008000|   A  |            NOTYPE|        |     |*ABS*
__TEXT_REGION_LENGTH__|00020000|   A  |            NOTYPE|        |     |*ABS*
__DATA_REGION_ORIGIN__|00804000|   A  |            NOTYPE|        |     |*ABS*
__data_start        |00804000|   D  |            NOTYPE|        |     |.data
local_port          |00804000|   d  |            OBJECT|00000002|     |.data
mac                 |00804002|   d  |            OBJECT|00000006|     |.data  C:\Users\Spence\Documents\Arduino\dummy/dummy.ino:25
vtable for EthernetClient|0080400e|   d  |            OBJECT|0000002e|     |.data
vtable for EthernetUDP|0080403c|   d  |            OBJECT|0000002a|     |.data
digital_pin_to_bit_position|00804066|   d  |            OBJECT|00000037|     |.data C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\variants\64pin-standard/pins_arduino.h:465
digital_pin_to_port |0080409d|   d  |            OBJECT|00000037|     |.data  C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\variants\64pin-standard/pins_arduino.h:396
digital_pin_to_bit_mask|008040d4|   d  |            OBJECT|00000037|     |.data C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\variants\64pin-standard/pins_arduino.h:533
CSWTCH.77           |0080410b|   d  |            OBJECT|00000005|     |.data
vtable for UartClass|00804110|   d  |            OBJECT|0000001a|     |.data
vtable for IPAddress|0080412a|   d  |            OBJECT|00000006|     |.data
vtable for EthernetServer|00804130|   d  |            OBJECT|00000010|     |.data
__bss_start         |00804270|   B  |            NOTYPE|        |     |.bss
__data_end          |00804270|   D  |            NOTYPE|        |     |.data
_edata              |00804270|   D  |            NOTYPE|        |     |.data
W5100Class::ss_pin_mask|00804270|   b  |            OBJECT|00000001|     |.bss
W5100Class::ss_pin_offs|00804271|   b  |            OBJECT|00000001|     |.bss
W5100Class::CH_BASE_MSB|00804272|   b  |            OBJECT|00000001|     |.bss
timer_millis        |00804273|   b  |            OBJECT|00000004|     |.bss C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore/wiring.c:43
state               |00804277|   b  |            OBJECT|00000038|     |.bss
EthernetServer::server_port|008042af|   b  |            OBJECT|00000010|     |.bss
W5100Class::chip    |008042bf|   b  |            OBJECT|00000001|     |.bss
W5100Class::init()::initialized|008042c0|   b  |            OBJECT|00000001|     |.bss
INADDR_NONE         |008042c1|   b  |            OBJECT|00000006|     |.bss C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore\api/IPAddress.cpp:117
Serial0             |008042c7|   b  |            OBJECT|0000009b|     |.bss C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\cores\dxcore/UART0.cpp:58
EthernetClass::_dnsServerAddress|00804362|   b  |            OBJECT|00000006|     |.bss
SPI                 |00804368|   b  |            OBJECT|0000004a|     |.bss C:\Users\Spence\Documents\Arduino\hardware\DxCore\megaavr\libraries\SPI\src/SPI.cpp:577
DEFAULT_SPI_SETTINGS|008043b2|   b  |            OBJECT|00000002|     |.bss
server              |008043b4|   b  |            OBJECT|00000006|     |.bss C:\Users\Spence\Documents\Arduino\dummy/dummy.ino:33
ip                  |008043ba|   b  |            OBJECT|00000006|     |.bss C:\Users\Spence\Documents\Arduino\dummy/dummy.ino:28
__bss_end           |008043c0|   B  |            NOTYPE|        |     |.bss
_end                |008043c0|   N  |            NOTYPE|        |     |.comment
__eeprom_end        |00810000|   N  |            NOTYPE|        |     |.comment

```

New One:

```

Memory Map for dummy.ino generated on:                                            Using avr-gcc version
F_CPU: 24000000         CLOCK_SOURCE: Internal        Part Name: avr128db64       Using Optiboot:          Reset Pin is:
Millis Timer: TCB2      Printf: Default               DxCore Version:             SPM from app:


Name of Symbol - Slightly abbreviated better readability                        |  Value |Class|    Type |   Size |  Sect |  Source File | Line
--------------------------------------------------------------------------------|--------|-----|---------|--------|-------|--------------|------
__heap_end                                                                      | 000000 |  W  |  NOTYPE |        | *ABS* |
__tmp_reg__                                                                     | 000000 |  a  |  NOTYPE |        | *ABS* |
__vector_default                                                                | 000000 |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 61
__vectors                                                                       | 000000 |  T  |  NOTYPE |        | .text | crt1/gcrt1.S | 61
__zero_reg__                                                                    | 000001 |  a  |  NOTYPE |        | *ABS* | |
__CCP ̲ _                                                                        | 000034 |  a  |  NOTYPE |        | *ABS* | |
__RAMPZ__                                                                       | 00003b |  a  |  NOTYPE |        | *ABS* | |
__SP_L__                                                                        | 00003d |  a  |  NOTYPE |        | *ABS* | |
__SP_H__                                                                        | 00003e |  a  |  NOTYPE |        | *ABS* | |
__SREG ̲ _                                                                       | 00003f |  a  |  NOTYPE |        | *ABS* | |
__ctors_start                                                                   | 000100 |  T  |  NOTYPE |        | .text | |
__trampolines_end                                                               | 000100 |  T  |  NOTYPE |        | .text | |
__trampolines_start                                                             | 000100 |  T  |  NOTYPE |        | .text | |
__ctors_end                                                                     | 000102 |  T  |  NOTYPE |        | .text | crt1/gcrt1.S | 230
__dtors_end                                                                     | 000102 |  T  |  NOTYPE |        | .text | crt1/gcrt1.S | 230
__dtors_start                                                                   | 000102 |  T  |  NOTYPE |        | .text | crt1/gcrt1.S | 230
__init                                                                          | 000102 |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 230
pre_main()                                                                      | 00010e |  T  |    FUNC | 000014 | .text | cores/dxcore/main.cpp | 112
__do_copy_data                                                                  | 000122 |  T  |  NOTYPE | 00001a | .text | config/avr/lib1funcs.S | 2373
__do_clear_bss                                                                  | 00013c |  T  |  NOTYPE | 000010 | .text | config/avr/lib1funcs.S | 2441
.do_clear_bss_loop                                                              | 000144 |  t  |  NOTYPE |        | .text | config/avr/lib1funcs.S | 2446
.do_clear_bss_start                                                             | 000146 |  t  |  NOTYPE |        | .text | config/avr/lib1funcs.S | 2448
__do_global_ctors                                                               | 00014c |  T  |  NOTYPE | 000016 | .text | config/avr/lib1funcs.S | 2466
__bad_interrupt                                                                 | 00016a |  T  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_1                                                                      | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_10                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_11                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_12                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_13                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_14                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_15                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_16                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_17                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_18                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_2                                                                      | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_21                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_22                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_23                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_24                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_25                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_26                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_27                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_28                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_29                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_3                                                                      | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_31                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_32                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_33                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_34                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_35                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_36                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_37                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_38                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_39                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_4                                                                      | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_40                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_41                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_42                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_43                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_44                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_45                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_46                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_47                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_48                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_49                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_5                                                                      | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_50                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_51                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_52                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_53                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_54                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_55                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_56                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_57                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_58                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_59                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_6                                                                      | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_60                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_61                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_62                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_63                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_7                                                                      | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_8                                                                      | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_9                                                                      | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
SPIClass::endTransaction() [cnst.39]                                            | 00016c |  t  |    FUNC | 00015a | .text | SPI/src/SPI.cpp | 511
SPIClass::transfer(uint8_t) [cnst.38]                                           | 0002c6 |  t  |    FUNC | 000016 | .text | SPI/src/SPI.cpp | 545
SPIClass::transfer(void*, uint16_t) [cnst.37]                                   | 0002dc |  t  |    FUNC | 000028 | .text | SPI/src/SPI.cpp | 568
SPIClass::beginTransaction(SPISettings) [cnst.35]                               | 000304 |  t  |    FUNC | 000184 | .text | SPI/src/SPI.cpp | 499
EthernetUDP::available()                                                        | 000488 |  t  |    FUNC | 000008 | .text | Ethernet/src/EthernetUdp.cpp | 50
EthernetUDP::flush()                                                            | 000490 |  t  |    FUNC | 000002 | .text | Ethernet/src/EthernetUdp.cpp | 179
UartClass::availableForWrite()                                                  | 000492 |  t  |    FUNC | 00001e | .text | cores/dxcore/UART.cpp | 337
UartClass::read()                                                               | 0004b0 |  t  |    FUNC | 000028 | .text | cores/dxcore/UART.cpp | 321
UartClass::peek()                                                               | 0004d8 |  t  |    FUNC | 00001c | .text | cores/dxcore/UART.cpp | 313
UartClass::available()                                                          | 0004f4 |  t  |    FUNC | 000018 | .text | cores/dxcore/UART.cpp | 309
UartClass::end()                                                                | 00050c |  t  |    FUNC | 000032 | .text | cores/dxcore/UART.cpp | 293
UartClass::_tx_data_empty_irq()                                                 | 00053e |  t  |    FUNC | 000056 | .text | cores/dxcore/UART.cpp | 107
UartClass::_poll_tx_data_empty()                                                | 000594 |  t  |    FUNC | 00001e | .text | cores/dxcore/UART.cpp | 135
UartClass::write(uint8_t)                                                       | 0005b2 |  t  |    FUNC | 000068 | .text | cores/dxcore/UART.cpp | 374
UartClass::flush()                                                              | 00061a |  t  |    FUNC | 000028 | .text | cores/dxcore/UART.cpp | 346
Print::write(uint8_t const*, uint16_t)                                          | 000642 |  t  |    FUNC | 00005a | .text | dxcore/api/Print.cpp | 30
UartClass::operator bool()                                                      | 00069c |  t  |    FUNC | 000004 | .text | cores/dxcore/UART.h | 200
micros                                                                          | 0006a0 |  t  |    FUNC | 00007e | .text | cores/dxcore/wiring.c | 204
delay                                                                           | 00071e |  t  |    FUNC | 000060 | .text | cores/dxcore/wiring.c | 390
millis                                                                          | 00077e |  t  |    FUNC | 000018 | .text | cores/dxcore/wiring.c | 176
W5100Class::resetSS()                                                           | 000796 |  t  |    FUNC | 000010 | .text | cores/dxcore/wiring.c | 200
W5100Class::setSS()                                                             | 0007a6 |  t  |    FUNC | 000010 | .text | src/utility/w5100.h | 376
W5100Class::write(uint16_t, uint8_t const*, uint16_t)                           | 0007b6 |  t  |    FUNC | 000138 | .text | src/utility/w5100.h | 372
W5100Class::writeMR(uint8_t)                                                    | 0008ee |  t  |    FUNC | 000024 | .text | src/utility/w5100.cpp | 380
W5100Class::RBASE(uint8_t)                                                      | 000912 |  t  |    FUNC | 00001c | .text | src/utility/w5100.h | 193
W5100Class::writeSn(uint8_t, uint16_t, uint8_t*, uint16_t)                      | 00092e |  t  |    FUNC | 000020 | .text | src/utility/w5100.h | 326
W5100Class::writeSnDPORT(uint8_t, uint16_t)                                     | 00094e |  t  |    FUNC | 000028 | .text | src/utility/w5100.h | 241
W5100Class::writeSnPORT(uint8_t, uint16_t)                                      | 000976 |  t  |    FUNC | 000028 | .text | src/utility/w5100.h | 279
W5100Class::writeSn(uint8_t, uint16_t, uint8_t)                                 | 00099e |  t  |    FUNC | 000038 | .text | src/utility/w5100.h | 276
EthernetUDP::write(uint8_t)                                                     | 0009d6 |  t  |    FUNC | 00002c | .text | src/utility/w5100.h | 236
EthernetServer::write(uint8_t)                                                  | 000a02 |  t  |    FUNC | 00002c | .text | Ethernet/src/EthernetUdp.cpp | 89
EthernetClient::operator==(EthernetClient const&)                               | 000a2e |  t  |    FUNC | 000016 | .text | Ethernet/src/EthernetServer.cpp | 159
EthernetClient::write(uint8_t)                                                  | 000a44 |  t  |    FUNC | 00002c | .text | Ethernet/src/EthernetClient.cpp | 174
EthernetClient::setConnectionTimeout(uint16_t)                                  | 000a70 |  t  |    FUNC | 000008 | .text | Ethernet/src/EthernetClient.cpp | 81
EthernetClient::operator!=(EthernetClient const&)                               | 000a78 |  t  |    FUNC | 000014 | .text | Ethernet/src/Ethernet.h | 241
EthernetClient::operator!=(bool)                                                | 000a8c |  t  |    FUNC | 000004 | .text | Ethernet/src/Ethernet.h | 236
EthernetClient::operator==(bool)                                                | 000a90 |  t  |    FUNC | 000006 | .text | Ethernet/src/Ethernet.h | 236
EthernetClient::operator bool()                                                 | 000a96 |  t  |    FUNC | 000010 | .text | Ethernet/src/Ethernet.h | 233
EthernetUDP::localPort()                                                        | 000aa6 |  t  |    FUNC | 000008 | .text | Ethernet/src/Ethernet.h | 232
EthernetUDP::remotePort()                                                       | 000aae |  t  |    FUNC | 000008 | .text | Ethernet/src/Ethernet.h | 208
EthernetUDP::remoteIP()                                                         | 000ab6 |  t  |    FUNC | 000020 | .text | Ethernet/src/Ethernet.h | 207
EthernetUDP::read(char*, uint16_t)                                              | 000ad6 |  t  |    FUNC | 00000e | .text | dxcore/api/IPAddress.h | 28
UartClass::begin(uint32_t)                                                      | 000ae4 |  t  |    FUNC | 000012 | .text | Ethernet/src/Ethernet.h | 199
Print::flush()                                                                  | 000af6 |  t  |    FUNC | 000002 | .text | cores/dxcore/UART.h | 170
Print::availableForWrite()                                                      | 000af8 |  t  |    FUNC | 000006 | .text | cores/dxcore/UART.h | 170
pinMode                                                                         | 000afe |  t  |    FUNC | 000066 | .text | cores/dxcore/wiring_digital.c | 135
UartClass::begin(uint32_t, uint16_t)                                            | 000b64 |  t  |    FUNC | 0000de | .text | cores/dxcore/wiring_digital.c | 149
SPISettings::SPISettings(uint32_t, uint8_t, uint8_t) [cnst.33]                  | 000c42 |  t  |    FUNC | 00000c | .text | cores/dxcore/UART.cpp | 291
EthernetUDP::beginPacket(IPAddress, uint16_t)                                   | 000c4e |  t  |    FUNC | 000088 | .text | SPI/src/SPI.h | 225
Print::write(char const*) [clone .part.2]                                       | 000cd6 |  t  |    FUNC | 00001e | .text | dxcore/api/Print.h | 47
Print::printNumber(uint32_t, uint8_t)                                           | 000cf4 |  t  |    FUNC | 00008a | .text | dxcore/api/Print.h | 49
Print::print(int, int) [cnst.18]                                                | 000d7e |  t  |    FUNC | 000066 | .text | dxcore/api/Print.cpp | 245
IPAddress::printTo(Print&) const                                                | 000de4 |  t  |    FUNC | 000090 | .text | dxcore/api/Print.cpp | 96
Print::println(char const*)                                                     | 000e74 |  t  |    FUNC | 000024 | .text | dxcore/api/IPAddress.cpp | 115
W5100Class::read(uint16_t, uint8_t*, uint16_t)                                  | 000e98 |  t  |    FUNC | 0000f8 | .text | src/utility/w5100.cpp | 391
EthernetClass::socketPeek(uint8_t)                                              | 000f90 |  t  |    FUNC | 000050 | .text | Ethernet/src/socket.cpp | 367
EthernetUDP::peek()                                                             | 000fe0 |  t  |    FUNC | 00001c | .text | Ethernet/src/socket.cpp | 374
EthernetClient::peek()                                                          | 000ffc |  t  |    FUNC | 000032 | .text | Ethernet/src/EthernetUdp.cpp | 174
W5100Class::read(uint16_t)                                                      | 00102e |  t  |    FUNC | 000020 | .text | Ethernet/src/EthernetClient.cpp | 113
W5100Class::readMR()                                                            | 00104e |  t  |    FUNC | 000006 | .text | src/utility/w5100.h | 162
W5100Class::softReset() [clone .part.0]                                         | 001054 |  t  |    FUNC | 000028 | .text | src/utility/w5100.cpp | 205
W5100Class::init() [clone .part.4]                                              | 00107c |  t  |    FUNC | 0001f8 | .text | src/utility/w5100.cpp | 90
EthernetClass::begin(uint8_t*, IPAddress) [cnst.20]                             | 001274 |  t  |    FUNC | 000156 | .text | src/utility/w5100.h | 155
W5100Class::readSn(uint8_t, uint16_t)                                           | 0013ca |  t  |    FUNC | 00001a | .text | Ethernet/src/Ethernet.cpp | 64
W5100Class::execCmdSn(uint8_t, SockCMD)                                         | 0013e4 |  t  |    FUNC | 00001c | .text | src/utility/w5100.h | 232
EthernetClass::socketDisconnect(uint8_t)                                        | 001400 |  t  |    FUNC | 000034 | .text | src/utility/w5100.h | 273
EthernetClass::socketClose(uint8_t)                                             | 001434 |  t  |    FUNC | 000034 | .text | Ethernet/src/socket.cpp | 253
EthernetUDP::stop()                                                             | 001468 |  t  |    FUNC | 000018 | .text | Ethernet/src/socket.cpp | 213
W5100Class::readSnSR(uint8_t)                                                   | 001480 |  t  |    FUNC | 000006 | .text | Ethernet/src/EthernetUdp.cpp | 55
EthernetClass::socketStatus(uint8_t)                                            | 001486 |  t  |    FUNC | 000034 | .text | Ethernet/src/EthernetUdp.cpp | 56
EthernetServer::operator bool()                                                 | 0014ba |  t  |    FUNC | 000056 | .text | Ethernet/src/socket.cpp | 202
EthernetClient::connected()                                                     | 001510 |  t  |    FUNC | 000042 | .text | Ethernet/src/EthernetServer.cpp | 121
EthernetClient::stop()                                                          | 001552 |  t  |    FUNC | 000066 | .text | Ethernet/src/EthernetClient.cpp | 161
EthernetUDP::endPacket()                                                        | 0015b8 |  t  |    FUNC | 000082 | .text | Ethernet/src/EthernetClient.cpp | 147
W5100Class::readSn(uint8_t, uint16_t, uint8_t*, uint16_t)                       | 00163a |  t  |    FUNC | 000020 | .text | Ethernet/src/socket.cpp | 528
EthernetClass::socketBeginMulticast(uint8_t, IPAddress, uint16_t) [cnst.9]      | 00165a |  t  |    FUNC | 0001b6 | .text | src/utility/w5100.h | 226
EthernetUDP::beginMulticast(IPAddress, uint16_t)                                | 001810 |  t  |    FUNC | 00007a | .text | Ethernet/src/socket.cpp | 196
write_data(uint8_t, uint16_t, uint8_t const*, uint16_t)                         | 00188a |  t  |    FUNC | 0000e8 | .text | Ethernet/src/EthernetUdp.cpp | 188
getSnTX_FSR(uint8_t)                                                            | 001972 |  t  |    FUNC | 000074 | .text | Ethernet/src/socket.cpp | 415
EthernetClass::socketSendAvailable(uint8_t)                                     | 0019e6 |  t  |    FUNC | 000052 | .text | Ethernet/src/socket.cpp | 395
EthernetClient::flush()                                                         | 001a38 |  t  |    FUNC | 000026 | .text | Ethernet/src/socket.cpp | 481
EthernetClient::availableForWrite()                                             | 001a5e |  t  |    FUNC | 000010 | .text | Ethernet/src/EthernetClient.cpp | 128
EthernetClass::socketSend(uint8_t, uint8_t const*, uint16_t)                    | 001a6e |  t  |    FUNC | 0000e0 | .text | Ethernet/src/EthernetClient.cpp | 74
EthernetClient::write(uint8_t const*, uint16_t)                                 | 001b4e |  t  |    FUNC | 000034 | .text | Ethernet/src/socket.cpp | 461
EthernetUDP::write(uint8_t const*, uint16_t)                                    | 001b82 |  t  |    FUNC | 00008c | .text | dxcore/api/Print.h | 39
getSnRX_RSR(uint8_t)                                                            | 001c0e |  t  |    FUNC | 000064 | .text | Ethernet/src/EthernetUdp.cpp | 97
EthernetClass::socketRecvAvailable(uint8_t)                                     | 001c72 |  t  |    FUNC | 000068 | .text | Ethernet/src/socket.cpp | 271
EthernetServer::available()                                                     | 001cda |  t  |    FUNC | 000108 | .text | Ethernet/src/socket.cpp | 357
EthernetServer::write(uint8_t const*, uint16_t)                                 | 001de2 |  t  |    FUNC | 00009c | .text | Ethernet/src/EthernetServer.cpp | 66
EthernetClient::available()                                                     | 001e7e |  t  |    FUNC | 000010 | .text | Ethernet/src/EthernetServer.cpp | 179
EthernetClass::socketRecv(uint8_t, uint8_t*, int)                               | 001e8e |  t  |    FUNC | 0001a2 | .text | Ethernet/src/EthernetServer.cpp | 179
EthernetUDP::read(uint8_t*, uint16_t)                                           | 002030 |  t  |    FUNC | 00003c | .text | src/utility/w5100.h | 290
EthernetUDP::read()                                                             | 00206c |  t  |    FUNC | 000052 | .text | Ethernet/src/EthernetUdp.cpp | 151
EthernetClient::read()                                                          | 0020be |  t  |    FUNC | 000032 | .text | Ethernet/src/EthernetUdp.cpp | 135
EthernetClient::read(uint8_t*, uint16_t)                                        | 0020f0 |  t  |    FUNC | 000010 | .text | Ethernet/src/EthernetClient.cpp | 119
EthernetClass::socketBegin(uint8_t, uint16_t)                                   | 002100 |  t  |    FUNC | 00016c | .text | Ethernet/src/EthernetClient.cpp | 121
EthernetUDP::begin(uint16_t)                                                    | 00226c |  t  |    FUNC | 000038 | .text | Ethernet/src/socket.cpp | 124
EthernetServer::begin()                                                         | 0022a4 |  t  |    FUNC | 000076 | .text | Ethernet/src/EthernetUdp.cpp | 41
EthernetClient::connect(IPAddress, uint16_t)                                    | 00231a |  t  |    FUNC | 000120 | .text | Ethernet/src/EthernetServer.cpp | 33
EthernetClient::remotePort()                                                    | 00243a |  t  |    FUNC | 00005e | .text | Ethernet/src/EthernetClient.cpp | 65
EthernetClient::localPort()                                                     | 002498 |  t  |    FUNC | 00005e | .text | src/utility/w5100.h | 279
EthernetUDP::parsePacket()                                                      | 0024f6 |  t  |    FUNC | 0000ac | .text | src/utility/w5100.h | 276
DNSClient::getHostByName(char const*, IPAddress&, uint16_t) [cnst.10]           | 0025a2 |  t  |    FUNC | 000480 | .text | Ethernet/src/EthernetUdp.cpp | 120
EthernetUDP::beginPacket(char const*, uint16_t)                                 | 002a22 |  t  |    FUNC | 0000d8 | .text | Ethernet/src/Dns.cpp | 237
EthernetClient::connect(char const*, uint16_t)                                  | 002afa |  t  |    FUNC | 000108 | .text | Ethernet/src/EthernetUdp.cpp | 71
EthernetClient::remoteIP()                                                      | 002c02 |  t  |    FUNC | 00008e | .text | Ethernet/src/EthernetClient.cpp | 39
_ vector_20                                                                     | 002c90 |  T  |    FUNC | 000054 | .text | cores/dxcore/UART0.cpp | 49
__vector_19                                                                     | 002ce4 |  T  |    FUNC | 000064 | .text | cores/dxcore/UART0.cpp | 40
__vector_30                                                                     | 002d48 |  T  |    FUNC | 000050 | .text | cores/dxcore/wiring.c | 128
global constructors keyed to 65535_0_dummy.ino.cpp.o.4248                       | 002d98 |  t  |    FUNC | 000158 | .text | cores/dxcore/wiring.c | 141
main                                                                            | 002ef0 |  T  |    FUNC | 000376 | .text | cores/dxcore/main.cpp | 27
__muluhisi3                                                                     | 003266 |  T  |  NOTYPE | 000014 | .text | config/avr/lib1funcs.S | 626
__udivmodsi4                                                                    | 00327a |  T  |  NOTYPE | 000044 | .text | config/avr/lib1funcs.S | 1638
__udivmodsi4_loop                                                               | 003286 |  t  |  NOTYPE |        | .text | config/avr/lib1funcs.S | 1646
__udivmodsi4_ep                                                                 | 0032a0 |  t  |  NOTYPE |        | .text | config/avr/lib1funcs.S | 1660
__umulhisi3                                                                     | 0032be |  T  |  NOTYPE | 00001e | .text | config/avr/lib1funcs.S | 564
__ashrdi3                                                                       | 0032dc |  T  |  NOTYPE | 000004 | .text | config/avr/lib1funcs.S | 3122
__lshrdi3                                                                       | 0032e0 |  T  |  NOTYPE | 000036 | .text | config/avr/lib1funcs.S | 3131
__cmpdi2_s8                                                                     | 003316 |  T  |  NOTYPE | 000018 | .text | config/avr/lib1funcs.S | 2102
__tablejump2__                                                                  | 00332e |  T  |  NOTYPE | 000012 | .text | config/avr/lib1funcs.S | 2296
memset                                                                          | 003340 |  T  |    FUNC | 00000e | .text |                        |
_exit                                                                           | 00334e |  T  |  NOTYPE |        | .text | config/avr/lib1funcs.S | 2278
exit                                                                            | 00334e |  W  |  NOTYPE |        | .text | config/avr/lib1funcs.S | 2278
__stop_program                                                                  | 003350 |  t  |  NOTYPE |        | .text | config/avr/lib1funcs.S | 2280
__data_load_start                                                               | 003352 |  A  |  NOTYPE |        | *ABS* |                        |
_etext                                                                          | 003352 |  T  |  NOTYPE |        | .text |                        |
__data_load_end                                                                 | 0035c2 |  A  |  NOTYPE |        | *ABS* |                        |
__stack                                                                         | 007fff |  W  |  NOTYPE |        | *ABS* |                        |
__RODATA_PM_OFFSET__                                                            | 008000 |  A  |  NOTYPE |        | *ABS* |                        |
__data_start                                                                    | 804000 |  D  |  NOTYPE |        | .data |                        |
local_port                                                                      | 804000 |  d  |  OBJECT | 000002 | .data |                        |
mac                                                                             | 804002 |  d  |  OBJECT | 000006 | .data | Arduino/dummy/dummy.ino | 25
vtable for EthernetClient                                                       | 80400e |  d  |  OBJECT | 00002e | .data |                        |
vtable for EthernetUDP                                                          | 80403c |  d  |  OBJECT | 00002a | .data |                        |
digital_pin_to_bit_position                                                     | 804066 |  d  |  OBJECT | 000037 | .data | variants/64pin-standard/pins_arduino.h | 465
digital_pin_to_port                                                             | 80409d |  d  |  OBJECT | 000037 | .data | variants/64pin-standard/pins_arduino.h | 396
digital_pin_to_bit_mask                                                         | 8040d4 |  d  |  OBJECT | 000037 | .data | variants/64pin-standard/pins_arduino.h | 533
CSWTCH.77                                                                       | 80410b |  d  |  OBJECT | 000005 | .data |                        |
vtable for UartClass                                                            | 804110 |  d  |  OBJECT | 00001a | .data |                        |
vtable for IPAddress                                                            | 80412a |  d  |  OBJECT | 000006 | .data |                        |
vtable for EthernetServer                                                       | 804130 |  d  |  OBJECT | 000010 | .data |                        |
__bss_start                                                                     | 804270 |  B  |  NOTYPE |        | .bss  |                        |
__data_end                                                                      | 804270 |  D  |  NOTYPE |        | .data |                        |
_edata                                                                          | 804270 |  D  |  NOTYPE |        | .data |                        |
W5100Class::ss_pin_mask                                                         | 804270 |  b  |  OBJECT | 000001 | .bss  |                        |
W5100Class::ss_pin_offs                                                         | 804271 |  b  |  OBJECT | 000001 | .bss  |                        |
W5100Class::CH_BASE_MSB                                                         | 804272 |  b  |  OBJECT | 000001 | .bss  |                        |
timer_millis                                                                    | 804273 |  b  |  OBJECT | 000004 | .bss  | cores/dxcore/wiring.c  | 43
state                                                                           | 804277 |  b  |  OBJECT | 000038 | .bss  |                        |
EthernetServer::server_port                                                     | 8042af |  b  |  OBJECT | 000010 | .bss  |                        |
W5100Class::chip                                                                | 8042bf |  b  |  OBJECT | 000001 | .bss  |                        |
W5100Class::init()::initialized                                                 | 8042c0 |  b  |  OBJECT | 000001 | .bss  |                        |
INADDR_NONE                                                                     | 8042c1 |  b  |  OBJECT | 000006 | .bss  | dxcore/api/IPAddress.cpp | 117
Serial0                                                                         | 8042c7 |  b  |  OBJECT | 00009b | .bss  | cores/dxcore/UART0.cpp | 58
EthernetClass::_dnsServerAddress                                                | 804362 |  b  |  OBJECT | 000006 | .bss  |                        |
SPI                                                                             | 804368 |  b  |  OBJECT | 00004a | .bss  | SPI/src/SPI.cpp | 577
DEFAULT_SPI_SETTINGS                                                            | 8043b2 |  b  |  OBJECT | 000002 | .bss  |                        |
server                                                                          | 8043b4 |  b  |  OBJECT | 000006 | .bss  | Arduino/dummy/dummy.ino | 33
ip                                                                              | 8043ba |  b  |  OBJECT | 000006 | .bss  | Arduino/dummy/dummy.ino | 28
__bss_end                                                                       | 8043c0 |  B  |  NOTYPE |        | .bss  |                        |
_end                                                                            | 8043c0 |  N  |  NOTYPE |        | .comment
__eeprom_end                                                                    | 810000 |  N  |  NOTYPE |        | .comment


```



New one **almost** pastes straight into markdown, too - except that the double underscores have a meaning in markdown.


Memory Map for dummy.ino generated on:                                            Using avr-gcc version
F_CPU: 24000000         CLOCK_SOURCE: Internal        Part Name: avr128db64       Using Optiboot:          Reset Pin is:
Millis Timer: TCB2      Printf: Default               DxCore Version:             SPM from app:


Name of Symbol - Slightly abbreviated better readability                        |  Value |Class|    Type |   Size |  Sect |  Source File | Line
--------------------------------------------------------------------------------|--------|-----|---------|--------|-------|--------------|------
__heap_end                                                                      | 000000 |  W  |  NOTYPE |        | *ABS* |
__tmp_reg__                                                                     | 000000 |  a  |  NOTYPE |        | *ABS* |
__vector_default                                                                | 000000 |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 61
__vectors                                                                       | 000000 |  T  |  NOTYPE |        | .text | crt1/gcrt1.S | 61
__zero_reg__                                                                    | 000001 |  a  |  NOTYPE |        | *ABS* | |
__CCP ̲ _                                                                        | 000034 |  a  |  NOTYPE |        | *ABS* | |
__RAMPZ__                                                                       | 00003b |  a  |  NOTYPE |        | *ABS* | |
__SP_L__                                                                        | 00003d |  a  |  NOTYPE |        | *ABS* | |
__SP_H__                                                                        | 00003e |  a  |  NOTYPE |        | *ABS* | |
__SREG ̲ _                                                                       | 00003f |  a  |  NOTYPE |        | *ABS* | |
__ctors_start                                                                   | 000100 |  T  |  NOTYPE |        | .text | |
__trampolines_end                                                               | 000100 |  T  |  NOTYPE |        | .text | |
__trampolines_start                                                             | 000100 |  T  |  NOTYPE |        | .text | |
__ctors_end                                                                     | 000102 |  T  |  NOTYPE |        | .text | crt1/gcrt1.S | 230
__dtors_end                                                                     | 000102 |  T  |  NOTYPE |        | .text | crt1/gcrt1.S | 230
__dtors_start                                                                   | 000102 |  T  |  NOTYPE |        | .text | crt1/gcrt1.S | 230
__init                                                                          | 000102 |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 230
pre_main()                                                                      | 00010e |  T  |    FUNC | 000014 | .text | cores/dxcore/main.cpp | 112
__do_copy_data                                                                  | 000122 |  T  |  NOTYPE | 00001a | .text | config/avr/lib1funcs.S | 2373
__do_clear_bss                                                                  | 00013c |  T  |  NOTYPE | 000010 | .text | config/avr/lib1funcs.S | 2441
.do_clear_bss_loop                                                              | 000144 |  t  |  NOTYPE |        | .text | config/avr/lib1funcs.S | 2446
.do_clear_bss_start                                                             | 000146 |  t  |  NOTYPE |        | .text | config/avr/lib1funcs.S | 2448
__do_global_ctors                                                               | 00014c |  T  |  NOTYPE | 000016 | .text | config/avr/lib1funcs.S | 2466
__bad_interrupt                                                                 | 00016a |  T  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_1                                                                      | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_10                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_11                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_12                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_13                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_14                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_15                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_16                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_17                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_18                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_2                                                                      | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_21                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_22                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_23                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_24                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_25                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_26                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_27                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_28                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_29                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_3                                                                      | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_31                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_32                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_33                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_34                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_35                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_36                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_37                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_38                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_39                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_4                                                                      | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_40                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_41                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_42                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_43                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_44                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_45                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_46                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_47                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_48                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_49                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_5                                                                      | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_50                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_51                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_52                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_53                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_54                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_55                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_56                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_57                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_58                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_59                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_6                                                                      | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_60                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_61                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_62                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_63                                                                     | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_7                                                                      | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_8                                                                      | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
__vector_9                                                                      | 00016a |  W  |  NOTYPE |        | .text | crt1/gcrt1.S | 209
SPIClass::endTransaction() [cnst.39]                                            | 00016c |  t  |    FUNC | 00015a | .text | SPI/src/SPI.cpp | 511
SPIClass::transfer(uint8_t) [cnst.38]                                           | 0002c6 |  t  |    FUNC | 000016 | .text | SPI/src/SPI.cpp | 545
SPIClass::transfer(void*, uint16_t) [cnst.37]                                   | 0002dc |  t  |    FUNC | 000028 | .text | SPI/src/SPI.cpp | 568
SPIClass::beginTransaction(SPISettings) [cnst.35]                               | 000304 |  t  |    FUNC | 000184 | .text | SPI/src/SPI.cpp | 499
EthernetUDP::available()                                                        | 000488 |  t  |    FUNC | 000008 | .text | Ethernet/src/EthernetUdp.cpp | 50
EthernetUDP::flush()                                                            | 000490 |  t  |    FUNC | 000002 | .text | Ethernet/src/EthernetUdp.cpp | 179
UartClass::availableForWrite()                                                  | 000492 |  t  |    FUNC | 00001e | .text | cores/dxcore/UART.cpp | 337
UartClass::read()                                                               | 0004b0 |  t  |    FUNC | 000028 | .text | cores/dxcore/UART.cpp | 321
UartClass::peek()                                                               | 0004d8 |  t  |    FUNC | 00001c | .text | cores/dxcore/UART.cpp | 313
UartClass::available()                                                          | 0004f4 |  t  |    FUNC | 000018 | .text | cores/dxcore/UART.cpp | 309
UartClass::end()                                                                | 00050c |  t  |    FUNC | 000032 | .text | cores/dxcore/UART.cpp | 293
UartClass::_tx_data_empty_irq()                                                 | 00053e |  t  |    FUNC | 000056 | .text | cores/dxcore/UART.cpp | 107
UartClass::_poll_tx_data_empty()                                                | 000594 |  t  |    FUNC | 00001e | .text | cores/dxcore/UART.cpp | 135
UartClass::write(uint8_t)                                                       | 0005b2 |  t  |    FUNC | 000068 | .text | cores/dxcore/UART.cpp | 374
UartClass::flush()                                                              | 00061a |  t  |    FUNC | 000028 | .text | cores/dxcore/UART.cpp | 346
Print::write(uint8_t const*, uint16_t)                                          | 000642 |  t  |    FUNC | 00005a | .text | dxcore/api/Print.cpp | 30
UartClass::operator bool()                                                      | 00069c |  t  |    FUNC | 000004 | .text | cores/dxcore/UART.h | 200
micros                                                                          | 0006a0 |  t  |    FUNC | 00007e | .text | cores/dxcore/wiring.c | 204
delay                                                                           | 00071e |  t  |    FUNC | 000060 | .text | cores/dxcore/wiring.c | 390
millis                                                                          | 00077e |  t  |    FUNC | 000018 | .text | cores/dxcore/wiring.c | 176
W5100Class::resetSS()                                                           | 000796 |  t  |    FUNC | 000010 | .text | cores/dxcore/wiring.c | 200
W5100Class::setSS()                                                             | 0007a6 |  t  |    FUNC | 000010 | .text | src/utility/w5100.h | 376
W5100Class::write(uint16_t, uint8_t const*, uint16_t)                           | 0007b6 |  t  |    FUNC | 000138 | .text | src/utility/w5100.h | 372
W5100Class::writeMR(uint8_t)                                                    | 0008ee |  t  |    FUNC | 000024 | .text | src/utility/w5100.cpp | 380
W5100Class::RBASE(uint8_t)                                                      | 000912 |  t  |    FUNC | 00001c | .text | src/utility/w5100.h | 193
W5100Class::writeSn(uint8_t, uint16_t, uint8_t*, uint16_t)                      | 00092e |  t  |    FUNC | 000020 | .text | src/utility/w5100.h | 326
W5100Class::writeSnDPORT(uint8_t, uint16_t)                                     | 00094e |  t  |    FUNC | 000028 | .text | src/utility/w5100.h | 241
W5100Class::writeSnPORT(uint8_t, uint16_t)                                      | 000976 |  t  |    FUNC | 000028 | .text | src/utility/w5100.h | 279
W5100Class::writeSn(uint8_t, uint16_t, uint8_t)                                 | 00099e |  t  |    FUNC | 000038 | .text | src/utility/w5100.h | 276
EthernetUDP::write(uint8_t)                                                     | 0009d6 |  t  |    FUNC | 00002c | .text | src/utility/w5100.h | 236
EthernetServer::write(uint8_t)                                                  | 000a02 |  t  |    FUNC | 00002c | .text | Ethernet/src/EthernetUdp.cpp | 89
EthernetClient::operator==(EthernetClient const&)                               | 000a2e |  t  |    FUNC | 000016 | .text | Ethernet/src/EthernetServer.cpp | 159
EthernetClient::write(uint8_t)                                                  | 000a44 |  t  |    FUNC | 00002c | .text | Ethernet/src/EthernetClient.cpp | 174
EthernetClient::setConnectionTimeout(uint16_t)                                  | 000a70 |  t  |    FUNC | 000008 | .text | Ethernet/src/EthernetClient.cpp | 81
EthernetClient::operator!=(EthernetClient const&)                               | 000a78 |  t  |    FUNC | 000014 | .text | Ethernet/src/Ethernet.h | 241
EthernetClient::operator!=(bool)                                                | 000a8c |  t  |    FUNC | 000004 | .text | Ethernet/src/Ethernet.h | 236
EthernetClient::operator==(bool)                                                | 000a90 |  t  |    FUNC | 000006 | .text | Ethernet/src/Ethernet.h | 236
EthernetClient::operator bool()                                                 | 000a96 |  t  |    FUNC | 000010 | .text | Ethernet/src/Ethernet.h | 233
EthernetUDP::localPort()                                                        | 000aa6 |  t  |    FUNC | 000008 | .text | Ethernet/src/Ethernet.h | 232
EthernetUDP::remotePort()                                                       | 000aae |  t  |    FUNC | 000008 | .text | Ethernet/src/Ethernet.h | 208
EthernetUDP::remoteIP()                                                         | 000ab6 |  t  |    FUNC | 000020 | .text | Ethernet/src/Ethernet.h | 207
EthernetUDP::read(char*, uint16_t)                                              | 000ad6 |  t  |    FUNC | 00000e | .text | dxcore/api/IPAddress.h | 28
UartClass::begin(uint32_t)                                                      | 000ae4 |  t  |    FUNC | 000012 | .text | Ethernet/src/Ethernet.h | 199
Print::flush()                                                                  | 000af6 |  t  |    FUNC | 000002 | .text | cores/dxcore/UART.h | 170
Print::availableForWrite()                                                      | 000af8 |  t  |    FUNC | 000006 | .text | cores/dxcore/UART.h | 170
pinMode                                                                         | 000afe |  t  |    FUNC | 000066 | .text | cores/dxcore/wiring_digital.c | 135
UartClass::begin(uint32_t, uint16_t)                                            | 000b64 |  t  |    FUNC | 0000de | .text | cores/dxcore/wiring_digital.c | 149
SPISettings::SPISettings(uint32_t, uint8_t, uint8_t) [cnst.33]                  | 000c42 |  t  |    FUNC | 00000c | .text | cores/dxcore/UART.cpp | 291
EthernetUDP::beginPacket(IPAddress, uint16_t)                                   | 000c4e |  t  |    FUNC | 000088 | .text | SPI/src/SPI.h | 225
Print::write(char const*) [clone .part.2]                                       | 000cd6 |  t  |    FUNC | 00001e | .text | dxcore/api/Print.h | 47
Print::printNumber(uint32_t, uint8_t)                                           | 000cf4 |  t  |    FUNC | 00008a | .text | dxcore/api/Print.h | 49
Print::print(int, int) [cnst.18]                                                | 000d7e |  t  |    FUNC | 000066 | .text | dxcore/api/Print.cpp | 245
IPAddress::printTo(Print&) const                                                | 000de4 |  t  |    FUNC | 000090 | .text | dxcore/api/Print.cpp | 96
Print::println(char const*)                                                     | 000e74 |  t  |    FUNC | 000024 | .text | dxcore/api/IPAddress.cpp | 115
W5100Class::read(uint16_t, uint8_t*, uint16_t)                                  | 000e98 |  t  |    FUNC | 0000f8 | .text | src/utility/w5100.cpp | 391
EthernetClass::socketPeek(uint8_t)                                              | 000f90 |  t  |    FUNC | 000050 | .text | Ethernet/src/socket.cpp | 367
EthernetUDP::peek()                                                             | 000fe0 |  t  |    FUNC | 00001c | .text | Ethernet/src/socket.cpp | 374
EthernetClient::peek()                                                          | 000ffc |  t  |    FUNC | 000032 | .text | Ethernet/src/EthernetUdp.cpp | 174
W5100Class::read(uint16_t)                                                      | 00102e |  t  |    FUNC | 000020 | .text | Ethernet/src/EthernetClient.cpp | 113
W5100Class::readMR()                                                            | 00104e |  t  |    FUNC | 000006 | .text | src/utility/w5100.h | 162
W5100Class::softReset() [clone .part.0]                                         | 001054 |  t  |    FUNC | 000028 | .text | src/utility/w5100.cpp | 205
W5100Class::init() [clone .part.4]                                              | 00107c |  t  |    FUNC | 0001f8 | .text | src/utility/w5100.cpp | 90
EthernetClass::begin(uint8_t*, IPAddress) [cnst.20]                             | 001274 |  t  |    FUNC | 000156 | .text | src/utility/w5100.h | 155
W5100Class::readSn(uint8_t, uint16_t)                                           | 0013ca |  t  |    FUNC | 00001a | .text | Ethernet/src/Ethernet.cpp | 64
W5100Class::execCmdSn(uint8_t, SockCMD)                                         | 0013e4 |  t  |    FUNC | 00001c | .text | src/utility/w5100.h | 232
EthernetClass::socketDisconnect(uint8_t)                                        | 001400 |  t  |    FUNC | 000034 | .text | src/utility/w5100.h | 273
EthernetClass::socketClose(uint8_t)                                             | 001434 |  t  |    FUNC | 000034 | .text | Ethernet/src/socket.cpp | 253
EthernetUDP::stop()                                                             | 001468 |  t  |    FUNC | 000018 | .text | Ethernet/src/socket.cpp | 213
W5100Class::readSnSR(uint8_t)                                                   | 001480 |  t  |    FUNC | 000006 | .text | Ethernet/src/EthernetUdp.cpp | 55
EthernetClass::socketStatus(uint8_t)                                            | 001486 |  t  |    FUNC | 000034 | .text | Ethernet/src/EthernetUdp.cpp | 56
EthernetServer::operator bool()                                                 | 0014ba |  t  |    FUNC | 000056 | .text | Ethernet/src/socket.cpp | 202
EthernetClient::connected()                                                     | 001510 |  t  |    FUNC | 000042 | .text | Ethernet/src/EthernetServer.cpp | 121
EthernetClient::stop()                                                          | 001552 |  t  |    FUNC | 000066 | .text | Ethernet/src/EthernetClient.cpp | 161
EthernetUDP::endPacket()                                                        | 0015b8 |  t  |    FUNC | 000082 | .text | Ethernet/src/EthernetClient.cpp | 147
W5100Class::readSn(uint8_t, uint16_t, uint8_t*, uint16_t)                       | 00163a |  t  |    FUNC | 000020 | .text | Ethernet/src/socket.cpp | 528
EthernetClass::socketBeginMulticast(uint8_t, IPAddress, uint16_t) [cnst.9]      | 00165a |  t  |    FUNC | 0001b6 | .text | src/utility/w5100.h | 226
EthernetUDP::beginMulticast(IPAddress, uint16_t)                                | 001810 |  t  |    FUNC | 00007a | .text | Ethernet/src/socket.cpp | 196
write_data(uint8_t, uint16_t, uint8_t const*, uint16_t)                         | 00188a |  t  |    FUNC | 0000e8 | .text | Ethernet/src/EthernetUdp.cpp | 188
getSnTX_FSR(uint8_t)                                                            | 001972 |  t  |    FUNC | 000074 | .text | Ethernet/src/socket.cpp | 415
EthernetClass::socketSendAvailable(uint8_t)                                     | 0019e6 |  t  |    FUNC | 000052 | .text | Ethernet/src/socket.cpp | 395
EthernetClient::flush()                                                         | 001a38 |  t  |    FUNC | 000026 | .text | Ethernet/src/socket.cpp | 481
EthernetClient::availableForWrite()                                             | 001a5e |  t  |    FUNC | 000010 | .text | Ethernet/src/EthernetClient.cpp | 128
EthernetClass::socketSend(uint8_t, uint8_t const*, uint16_t)                    | 001a6e |  t  |    FUNC | 0000e0 | .text | Ethernet/src/EthernetClient.cpp | 74
EthernetClient::write(uint8_t const*, uint16_t)                                 | 001b4e |  t  |    FUNC | 000034 | .text | Ethernet/src/socket.cpp | 461
EthernetUDP::write(uint8_t const*, uint16_t)                                    | 001b82 |  t  |    FUNC | 00008c | .text | dxcore/api/Print.h | 39
getSnRX_RSR(uint8_t)                                                            | 001c0e |  t  |    FUNC | 000064 | .text | Ethernet/src/EthernetUdp.cpp | 97
EthernetClass::socketRecvAvailable(uint8_t)                                     | 001c72 |  t  |    FUNC | 000068 | .text | Ethernet/src/socket.cpp | 271
EthernetServer::available()                                                     | 001cda |  t  |    FUNC | 000108 | .text | Ethernet/src/socket.cpp | 357
EthernetServer::write(uint8_t const*, uint16_t)                                 | 001de2 |  t  |    FUNC | 00009c | .text | Ethernet/src/EthernetServer.cpp | 66
EthernetClient::available()                                                     | 001e7e |  t  |    FUNC | 000010 | .text | Ethernet/src/EthernetServer.cpp | 179
EthernetClass::socketRecv(uint8_t, uint8_t*, int)                               | 001e8e |  t  |    FUNC | 0001a2 | .text | Ethernet/src/EthernetServer.cpp | 179
EthernetUDP::read(uint8_t*, uint16_t)                                           | 002030 |  t  |    FUNC | 00003c | .text | src/utility/w5100.h | 290
EthernetUDP::read()                                                             | 00206c |  t  |    FUNC | 000052 | .text | Ethernet/src/EthernetUdp.cpp | 151
EthernetClient::read()                                                          | 0020be |  t  |    FUNC | 000032 | .text | Ethernet/src/EthernetUdp.cpp | 135
EthernetClient::read(uint8_t*, uint16_t)                                        | 0020f0 |  t  |    FUNC | 000010 | .text | Ethernet/src/EthernetClient.cpp | 119
EthernetClass::socketBegin(uint8_t, uint16_t)                                   | 002100 |  t  |    FUNC | 00016c | .text | Ethernet/src/EthernetClient.cpp | 121
EthernetUDP::begin(uint16_t)                                                    | 00226c |  t  |    FUNC | 000038 | .text | Ethernet/src/socket.cpp | 124
EthernetServer::begin()                                                         | 0022a4 |  t  |    FUNC | 000076 | .text | Ethernet/src/EthernetUdp.cpp | 41
EthernetClient::connect(IPAddress, uint16_t)                                    | 00231a |  t  |    FUNC | 000120 | .text | Ethernet/src/EthernetServer.cpp | 33
EthernetClient::remotePort()                                                    | 00243a |  t  |    FUNC | 00005e | .text | Ethernet/src/EthernetClient.cpp | 65
EthernetClient::localPort()                                                     | 002498 |  t  |    FUNC | 00005e | .text | src/utility/w5100.h | 279
EthernetUDP::parsePacket()                                                      | 0024f6 |  t  |    FUNC | 0000ac | .text | src/utility/w5100.h | 276
DNSClient::getHostByName(char const*, IPAddress&, uint16_t) [cnst.10]           | 0025a2 |  t  |    FUNC | 000480 | .text | Ethernet/src/EthernetUdp.cpp | 120
EthernetUDP::beginPacket(char const*, uint16_t)                                 | 002a22 |  t  |    FUNC | 0000d8 | .text | Ethernet/src/Dns.cpp | 237
EthernetClient::connect(char const*, uint16_t)                                  | 002afa |  t  |    FUNC | 000108 | .text | Ethernet/src/EthernetUdp.cpp | 71
EthernetClient::remoteIP()                                                      | 002c02 |  t  |    FUNC | 00008e | .text | Ethernet/src/EthernetClient.cpp | 39
_ vector_20                                                                     | 002c90 |  T  |    FUNC | 000054 | .text | cores/dxcore/UART0.cpp | 49
__vector_19                                                                     | 002ce4 |  T  |    FUNC | 000064 | .text | cores/dxcore/UART0.cpp | 40
__vector_30                                                                     | 002d48 |  T  |    FUNC | 000050 | .text | cores/dxcore/wiring.c | 128
global constructors keyed to 65535_0_dummy.ino.cpp.o.4248                       | 002d98 |  t  |    FUNC | 000158 | .text | cores/dxcore/wiring.c | 141
main                                                                            | 002ef0 |  T  |    FUNC | 000376 | .text | cores/dxcore/main.cpp | 27
__muluhisi3                                                                     | 003266 |  T  |  NOTYPE | 000014 | .text | config/avr/lib1funcs.S | 626
__udivmodsi4                                                                    | 00327a |  T  |  NOTYPE | 000044 | .text | config/avr/lib1funcs.S | 1638
__udivmodsi4_loop                                                               | 003286 |  t  |  NOTYPE |        | .text | config/avr/lib1funcs.S | 1646
__udivmodsi4_ep                                                                 | 0032a0 |  t  |  NOTYPE |        | .text | config/avr/lib1funcs.S | 1660
__umulhisi3                                                                     | 0032be |  T  |  NOTYPE | 00001e | .text | config/avr/lib1funcs.S | 564
__ashrdi3                                                                       | 0032dc |  T  |  NOTYPE | 000004 | .text | config/avr/lib1funcs.S | 3122
__lshrdi3                                                                       | 0032e0 |  T  |  NOTYPE | 000036 | .text | config/avr/lib1funcs.S | 3131
__cmpdi2_s8                                                                     | 003316 |  T  |  NOTYPE | 000018 | .text | config/avr/lib1funcs.S | 2102
__tablejump2__                                                                  | 00332e |  T  |  NOTYPE | 000012 | .text | config/avr/lib1funcs.S | 2296
memset                                                                          | 003340 |  T  |    FUNC | 00000e | .text |                        |
_exit                                                                           | 00334e |  T  |  NOTYPE |        | .text | config/avr/lib1funcs.S | 2278
exit                                                                            | 00334e |  W  |  NOTYPE |        | .text | config/avr/lib1funcs.S | 2278
__stop_program                                                                  | 003350 |  t  |  NOTYPE |        | .text | config/avr/lib1funcs.S | 2280
__data_load_start                                                               | 003352 |  A  |  NOTYPE |        | *ABS* |                        |
_etext                                                                          | 003352 |  T  |  NOTYPE |        | .text |                        |
__data_load_end                                                                 | 0035c2 |  A  |  NOTYPE |        | *ABS* |                        |
__stack                                                                         | 007fff |  W  |  NOTYPE |        | *ABS* |                        |
__RODATA_PM_OFFSET__                                                            | 008000 |  A  |  NOTYPE |        | *ABS* |                        |
__data_start                                                                    | 804000 |  D  |  NOTYPE |        | .data |                        |
local_port                                                                      | 804000 |  d  |  OBJECT | 000002 | .data |                        |
mac                                                                             | 804002 |  d  |  OBJECT | 000006 | .data | Arduino/dummy/dummy.ino | 25
vtable for EthernetClient                                                       | 80400e |  d  |  OBJECT | 00002e | .data |                        |
vtable for EthernetUDP                                                          | 80403c |  d  |  OBJECT | 00002a | .data |                        |
digital_pin_to_bit_position                                                     | 804066 |  d  |  OBJECT | 000037 | .data | variants/64pin-standard/pins_arduino.h | 465
digital_pin_to_port                                                             | 80409d |  d  |  OBJECT | 000037 | .data | variants/64pin-standard/pins_arduino.h | 396
digital_pin_to_bit_mask                                                         | 8040d4 |  d  |  OBJECT | 000037 | .data | variants/64pin-standard/pins_arduino.h | 533
CSWTCH.77                                                                       | 80410b |  d  |  OBJECT | 000005 | .data |                        |
vtable for UartClass                                                            | 804110 |  d  |  OBJECT | 00001a | .data |                        |
vtable for IPAddress                                                            | 80412a |  d  |  OBJECT | 000006 | .data |                        |
vtable for EthernetServer                                                       | 804130 |  d  |  OBJECT | 000010 | .data |                        |
__bss_start                                                                     | 804270 |  B  |  NOTYPE |        | .bss  |                        |
__data_end                                                                      | 804270 |  D  |  NOTYPE |        | .data |                        |
_edata                                                                          | 804270 |  D  |  NOTYPE |        | .data |                        |
W5100Class::ss_pin_mask                                                         | 804270 |  b  |  OBJECT | 000001 | .bss  |                        |
W5100Class::ss_pin_offs                                                         | 804271 |  b  |  OBJECT | 000001 | .bss  |                        |
W5100Class::CH_BASE_MSB                                                         | 804272 |  b  |  OBJECT | 000001 | .bss  |                        |
timer_millis                                                                    | 804273 |  b  |  OBJECT | 000004 | .bss  | cores/dxcore/wiring.c  | 43
state                                                                           | 804277 |  b  |  OBJECT | 000038 | .bss  |                        |
EthernetServer::server_port                                                     | 8042af |  b  |  OBJECT | 000010 | .bss  |                        |
W5100Class::chip                                                                | 8042bf |  b  |  OBJECT | 000001 | .bss  |                        |
W5100Class::init()::initialized                                                 | 8042c0 |  b  |  OBJECT | 000001 | .bss  |                        |
INADDR_NONE                                                                     | 8042c1 |  b  |  OBJECT | 000006 | .bss  | dxcore/api/IPAddress.cpp | 117
Serial0                                                                         | 8042c7 |  b  |  OBJECT | 00009b | .bss  | cores/dxcore/UART0.cpp | 58
EthernetClass::_dnsServerAddress                                                | 804362 |  b  |  OBJECT | 000006 | .bss  |                        |
SPI                                                                             | 804368 |  b  |  OBJECT | 00004a | .bss  | SPI/src/SPI.cpp | 577
DEFAULT_SPI_SETTINGS                                                            | 8043b2 |  b  |  OBJECT | 000002 | .bss  |                        |
server                                                                          | 8043b4 |  b  |  OBJECT | 000006 | .bss  | Arduino/dummy/dummy.ino | 33
ip                                                                              | 8043ba |  b  |  OBJECT | 000006 | .bss  | Arduino/dummy/dummy.ino | 28
__bss_end                                                                       | 8043c0 |  B  |  NOTYPE |        | .bss  |                        |
_end                                                                            | 8043c0 |  N  |  NOTYPE |        | .comment
__eeprom_end                                                                    | 810000 |  N  |  NOTYPE |        | .comment
