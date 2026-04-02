## AVR Dx / Ex Optiboot Bootloader Usage

The configure script generates a config_{MCU}.h file. It is called with arguments:
- ./configure  MCU  LED_PIN  #LED_toggles  UART_TX_PIN  BAUDRATE

To generate and flash the boot loader use:
- make {MCU}.hex will include the config file and generate a bootloader{MCU}.hex file
- make {MCU}.flash_d/ex will use avrdude (8.1) to flash the chip via updi

Programming requires avrdude 8.1 or later. Download and extract it to the location used in Makefile.
The configure script uses the header files that come with avr-gcc compiler. As ioavrxxYYzz.h appears to be generated, the comment lines of the USART PORTMUX are a reliable source for USART pin mapping.
The optiboot_dx.c relies on the azduino avr-gcc compiler, other compiler versions always require using the frame Pointer and won't compile.

To use the avrdude chip erase feature for AVR Dx/Ex add a line:
 chip_erase             = "1010.1100--1000.0000--0000.0000--0000.0000";
to the 'family common values' in avrdude.conf
