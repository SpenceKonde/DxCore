// configure: avr64dd28 PA6 6 PA0 115200 /
#define  __CONFIG_H__
#define  __AVR_Dx__

#define	 WDTTIME          1

#define  F_OSC             (24000000L)
#define  LED_PORT          VPORTA
#define  LED               (1<<6)
#define  LED_START_FLASHES (6>>1)


#define  BAUD_RATE         (115200L)
#define  MYUART            (USART0)
#define  MYPMUX_REG    	   PORTMUX.USARTROUTEA
#define  MYUART_PMUX_VAL   (0x00<<0)
#define  MYUART_RXPINCTRL  PORTA.PIN1CTRL
#define  MYUART_TXPORT     VPORTA
#define  MYUART_TXPIN      (1<<0)
