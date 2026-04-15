## Low Power Sleep for AVR DxCore

AVR DxCore provides a low power 32 kHz crystal oscillator.
The chip can be powered down to 'Standby' consuming ~1uA.
In this mode, almost all peripherals are at rest including
timers. The time-keeping can be done by the RTC Clock, this
will be available from the Arduino millis() timer menu.

When the RTC is used as Arduino millis() timer, this code
uses the running RTC to implement a delay() equivalent that
sends the chip into low power Sleep_Standby_Mode during delay.
In this case, the code relies on the RTC interrupt routine
in DxCore/../cores/.../wiring.c which needs to store the
interrupt flags to __rtc_intflags.

If the RTC Timer is not used, the RTC XTAL oscillator is
initialized on the first call to rtc_sleep() and the RTC
interrput by this library.If a millis() timer is active
the millis() function is updated but the time returned is 
not correct to the ms.

The library provides:
- `RTC_Sleep.h`
- `void rtc_sleep(uint32_t ms)`
