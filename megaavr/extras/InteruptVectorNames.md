### List of interrupt vector names

If there is a list of the names defined for the interrupt vectors is present somewhere in the datasheet, I was never able to find it. These are the possible names for interrupt vectors on the parts supported by megaTinyCore. Not all parts will have all interrupts listed below (interrupts associated with hardware not present on a chip won't exist there). An ISR is created with the ISR() macro.

**WARNING** If you misspell the name of a vector, you will get a compiler warning BUT NOT AN ERROR! Hence, you can upload the bad code... in this case the chip will freeze the instant the ISR you thought you assigned is called, as it jumps to BAD_ISR, which in turn jumps to the reset vector... but since the interrupt flag never gets cleared, as soon as interrupts are enabled, it will do it all over again, so the chip will be hung. Encountering this (and the annoying lack of a nice list anywhere outside of the io.h) was the impetus for creating this list.

|  DA-series           |  DB-series            |
|----------------------|-----------------------|
|  `NMI_vect`          |  `NMI_vect`           |
|  `BOD_VLM_vect`      |  `BOD_VLM_vect`       |
|  `RTC_CNT_vect`      |  `CLKCTRL_CFD_vect`   |
|  `RTC_PIT_vect`      |  `MVIO_MVIO_vect`     |
|  `CCL_CCL_vect`      |  `RTC_CNT_vect`       |
|  `PORTA_PORT_vect`   |  `RTC_PIT_vect`       |
|  `TCA0_LUNF_vect`    |  `CCL_CCL_vect`       |
|  `TCA0_OVF_vect`     |  `PORTA_PORT_vect`    |
|  `TCA0_HUNF_vect`    |  `TCA0_LUNF_vect`     |
|  `TCA0_CMP0_vect`    |  `TCA0_OVF_vect`      |
|  `TCA0_LCMP0_vect`   |  `TCA0_HUNF_vect`     |
|  `TCA0_CMP1_vect`    |  `TCA0_CMP0_vect`     |
|  `TCA0_LCMP1_vect`   |  `TCA0_LCMP0_vect`    |
|  `TCA0_CMP2_vect`    |  `TCA0_CMP1_vect`     |
|  `TCA0_LCMP2_vect`   |  `TCA0_LCMP1_vect`    |
|  `TCB0_INT_vect`     |  `TCA0_CMP2_vect`     |
|  `TCB1_INT_vect`     |  `TCA0_LCMP2_vect`    |
|  `TCD0_OVF_vect`     |  `TCB0_INT_vect`      |
|  `TCD0_TRIG_vect`    |  `TCB1_INT_vect`      |
|  `TWI0_TWIS_vect`    |  `TCD0_OVF_vect`      |
|  `TWI0_TWIM_vect`    |  `TCD0_TRIG_vect`     |
|  `SPI0_INT_vect`     |  `TWI0_TWIS_vect`     |
|  `USART0_RXC_vect`   |  `TWI0_TWIM_vect`     |
|  `USART0_DRE_vect`   |  `SPI0_INT_vect`      |
|  `USART0_TXC_vect`   |  `USART0_RXC_vect`    |
|  `PORTD_PORT_vect`   |  `USART0_DRE_vect`    |
|  `AC0_AC_vect`       |  `USART0_TXC_vect`    |
|  `ADC0_RESRDY_vect`  |  `PORTD_PORT_vect`    |
|  `ADC0_WCMP_vect`    |  `AC0_AC_vect`        |
|  `ZCD0_ZCD_vect`     |  `ADC0_RESRDY_vect`   |
|  `PTC_PTC_vect`      |  `ADC0_WCMP_vect`     |
|  `AC1_AC_vect`       |  `ZCD0_ZCD_vect`      |
|  `PORTC_PORT_vect`   |  `AC1_AC_vect`        |
|  `TCB2_INT_vect`     |  `PORTC_PORT_vect`    |
|  `USART1_RXC_vect`   |  `TCB2_INT_vect`      |
|  `USART1_DRE_vect`   |  `USART1_RXC_vect`    |
|  `USART1_TXC_vect`   |  `USART1_DRE_vect`    |
|  `PORTF_PORT_vect`   |  `USART1_TXC_vect`    |
|  `NVMCTRL_EE_vect`   |  `PORTF_PORT_vect`    |
|  `SPI1_INT_vect`     |  `NVMCTRL_EE_vect`    |
|  `USART2_RXC_vect`   |  `SPI1_INT_vect`      |
|  `USART2_DRE_vect`   |  `USART2_RXC_vect`    |
|  `USART2_TXC_vect`   |  `USART2_DRE_vect`    |
|  `AC2_AC_vect`       |  `USART2_TXC_vect`    |
|  `TCB3_INT_vect`     |  `AC2_AC_vect`        |
|  `TWI1_TWIS_vect`    |  `TWI1_TWIS_vect`     |
|  `TWI1_TWIM_vect`    |  `TWI1_TWIM_vect`     |
|  `PORTB_PORT_vect`   |  `TCB3_INT_vect`      |
|  `PORTE_PORT_vect`   |  `PORTB_PORT_vect`    |
|  `TCA1_LUNF_vect`    |  `PORTE_PORT_vect`    |
|  `TCA1_OVF_vect`     |  `TCA1_LUNF_vect`     |
|  `TCA1_HUNF_vect`    |  `TCA1_OVF_vect`      |
|  `TCA1_CMP0_vect`    |  `TCA1_HUNF_vect`     |
|  `TCA1_LCMP0_vect`   |  `TCA1_CMP0_vect`     |
|  `TCA1_CMP1_vect`    |  `TCA1_LCMP0_vect`    |
|  `TCA1_LCMP1_vect`   |  `TCA1_CMP1_vect`     |
|  `TCA1_CMP2_vect`    |  `TCA1_LCMP1_vect`    |
|  `TCA1_LCMP2_vect`   |  `TCA1_CMP2_vect`     |
|  `ZCD1_ZCD_vect`     |  `TCA1_LCMP2_vect`    |
|  `USART3_RXC_vect`   |  `ZCD1_ZCD_vect`      |
|  `USART3_DRE_vect`   |  `USART3_RXC_vect`    |
|  `USART3_TXC_vect`   |  `USART3_DRE_vect`    |
|  `USART4_RXC_vect`   |  `USART3_TXC_vect`    |
|  `USART4_DRE_vect`   |  `USART4_RXC_vect`    |
|  `USART4_TXC_vect`   |  `USART4_DRE_vect`    |
|  `PORTG_PORT_vect`   |  `USART4_TXC_vect`    |
|  `ZCD2_ZCD_vect`     |  `PORTG_PORT_vect`    |
|  `TCB4_INT_vect`     |  `ZCD2_ZCD_vect`      |
|  `USART5_RXC_vect`   |  `TCB4_INT_vect`      |
|  `USART5_DRE_vect`   |  `USART5_RXC_vect`    |

### Example

```
ISR(PORTA_PORT_vect) {
  //ISR code goes here
}
```
### Reminders
* ISRs should run FAST. Minimize the time that the code spends in the ISR. Never use polling loops, and avoid writing to serial. Most interrupts should just set a flag that is checked elsewhere, and do what must be done that instant (eg, reading from certain hardware registers). Anything that requires local variables slows down entry and exit to the ISR too, because each and every working register used needs to be saved on the stack, then restored at the end (this is 3 clock cycles per byte - 1 for the push, 2 for the pop, plus whatever you do with them). So again... try to do as little as possible. That's actually one of the big advantages of using a type B timer for millis, which we have enough timers to do by default in DxCore, unlike megaTinyCore: the math in millis is trivial, so ISR runs in ~ 50 clocks instead of... ~ 120 clocks...
* Read the datasheet, particularly relating to the relevant INTFLAGS register - some interrupt flags are automatically cleared when you take some action relating to the cause of the interrupt (ex: the ADC's result ready is cleared when you read the result), but most are not. Those must be cleared manually, and if these are not cleared, as soon as your ISR finishes, it will be called again, because you're not in an ISR, the interrupt is enabled, and the flag is set.
* Any global variable that an interrupt changes, if used outside the ISR, must be declared volatile - otherwise the compiler may optimize away access to it, resulting in code elsewhere not knowing that it was changed by the ISR.
* Any global variable read by the ISR and written to by code outside of the ISR larger than a byte must be written to with interrupts disabled - if the interrupt triggers in the middle of a write, the ISR would see a corrupted value.
