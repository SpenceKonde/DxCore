// We are returning to the conventional name and there is no longer a bloat inducing class layer between

#ifndef UART_NAME_HACK_H
  #define USART_NAME_HACK_H
  #include <HardwareSerial.h>
  #define UartClass HardwareSerial
#endif
