SleepLib feature goals
* Support all post-2016 parts (megaAVR 0-series, tinAVR 0/1/2, AVR DA/DB/DD/DU/EA)
On classic AVRs, sleep was all about SLEEP_MODE_POWERDOWN. There was a standby mode, sure,
but it has most of the disadvantages of power down, and used power for things it generally
didn't need to be doing and that the user didn't want to to be wasting power on. But things
have changed.

As always, it has the useless idle sleep mode that stops the CPU core but nothing else, and doesn't save enough power to notice.

Sleep Mode Powerdown does what it always did: Turns off everything except the lowspeed internal oscillator if enabled (WDT on classic AVRs, ULP on some others, or the RTC-PIT). It can wake the system only from the PIT interrupt, from a "change" or "LOW level" pin interrupt, and from a TWI slave address match interrupt (who'd have guessed?). New is the ability to wake on "RISING" or "FALLING" edges - however this is limited to only pins 2 and 6 within any port.
Standby mode does the same thing as power-down mode by default with the same power usage - with one big added feature:
**You can tell it to leave some peripherals on.**
There is no ADC noise reduction sleep mode now. If sleeping with the ADC interrupt enabled reduces noise, the documentation doesn't mention this.

## Run Standby peripherals
This is a (likely incomplete) list of things that can have some functionality enabled in standby mode.

|  Peripheral   | Register    | Bitmask                | Functionality |
|---------------|-------------|------------------------|------------------------------------------------------------------------------------------------|
| Analog Comp   | CTRLA       | AC_RUNSTDBY_bm         | AC remains fully operational while in standby. The peripheral clock is not needed.             |
| ADC           | OSCHFCTRLA  | ADC_RUNSTBY_bm         | Allows ADC to be triggered by events or freerunning mode and wake system with interrupts. `*`  |
| Clock Control | OSCHFCTRLA  | CLKCTRL_RUNSTBY_bm     | Internal HF oscillator is always on, eliminating the startup delay `**` `!`                    |
| Clock Control | PLLCTRLA    | CLKCTRL_RUNSTDBY_bm    | Since all the PLL can be used for is TCD0, and TCD0 can't run in standby, seems useless?       |
| Clock Control | OSC32KCTRLA | CLKCTRL_RUNSTDBY_bm    | Keeps the 32kHz internal oscillator running `**`                                               |
| Clock Control | XOSC32KCTRLA| CLKCTRL_RUNSTDBY_bm    | Keeps the 32kHz xtal oscillator running `**`                                                   |
| Clock Control | XOSCHFCTRLA | CLKCTRL_RUNSTDBY_bm    | External HF xtal is always on, eliminating the startup delay `**`                              |
| DAC           | CTRLA       | DAC_RUNSTDBY_bm        | DAC will continue outputting the current analog voltage in standby.                            |
|Real Time Clock| CTRLA       | RTC_RUNSTDBY_bm        | RTC keeps counting and can wake system with CMP or OVF interrupt. PIT will always run.         |
| Type A timer  | CTRLA       | TCA_SINGLE_RUNSTDBY_bm | Keeps the type A timer fully operational while in standby.                                     |
| Type A timer  | CTRLA       | TCA_SPLIT_RUNSTDBY_bm  | As above, but there are two of them like everything else TCA-related because of the two modes  |
| Type B timer  | CTRLA       | TCB_RUNSTDBY_bm        | Keeps the type B timer fully operational while in standby.                                     |
| USART         | CTRLB       | USART_SFDEN_bm         | Allows the USART to wake the part from standby if a character is received `*`                 |
| Zero Cross    | CTRLA       | ZCD_RUNSTDBY_bm        | ZCD remains fully operational while in standby. The peripheral clock is not needed.            |
| Opamp         | OP0CTRLA    | OPAMP_RUNSTBY_bm       | Keeps OPAMP0 fully operational while in standby. `!`                                           |
| Opamp         | OP1CTRLA    | OPAMP_RUNSTBY_bm       | Keeps OPAMP0 fully operational while in standby. `!`                                           |
| Opamp         | OP2CTRLA    | OPAMP_RUNSTBY_bm       | Keeps OPAMP0 fully operational while in standby. `!`                                           |
| Custoom Logic | CTRLA       | CCL_RUNSTDBY_bm        | Keeps clock source on so filter, edge detector, etc can keep working. Unclear what it does if not set |
`*` Considerable additional considerations are relevant, refer to datasheet.
`**` That is, it *stays on* even if nothing is requesting it, whereas if not set, it will only be started when requested, adding a potentially lengthy startup delay


## So, why use powerdown then?
My only guess is so that you don't have to go around turning off RUNSTBY for every peripheral if you use both standby and powerdown sleep? Or for familiarity? What is very clear is that standby is the star of the show on modern AVRs.


## SleepLib API proposal
When using SleepLib, you must only put the part into sleep mode using these functions, otherwise they may wake unexpectedly

### SleepWithMillis()
This resets the RTC to 0, sets RUNSTBY and enables the OVF interrupt, stops the millis timer and starts the RTC, then goes to sleep. When the RTC interrupt fires, a flag is set. Once it is woken, it will check for that flag; if that flag is set, it will add one overflow of ticks to the running millis total, clear the flag and return to sleep. If it is woken by some other source, then it will check the RTC count and stop the RTC, adjust millis, and restart the millis timer before returning to user code.

### SleepForTime(uint32_t ms)
This works largely as SleepWithMillis() however, you pass an amount of time, in milliseconds, that you want it to sleep for before waking up on its own. If that can be handled without an RTC overflow, the CMP interrupt is used, otherwise, we use OVF until the time remaining is less than one OVF.

### SleepClockSource(uint8_t clocksrc)
This configures the clock souece for the RTC used by SleepLib.
Acceptable options are INT32K, INT1K, XTAL32K, CLK32K.

### SleepingMillisResolution(uint8_t res)
This sets how often the device will wake if not otherwise awoken (the compare match wake source for SleepForTime is treated like any other interrupt)
The lower the resolution, the less power is used, but the larger the granularity of the millis tracked during sleep.

| res value | Resolution (32k source)  | Wakes every | Resolution (1k source)   | wakes every |
|-----------|--------------------------|-------------|--------------------------|-------------|
| 0x00      |    30 us                 | 2 sec       | 960us                    | 64 sec      |
| 0x01      |    60 us                 | 4 sec       | 2 ms                     | 128 sec     |
| 0x02      |   120 us                 | 8 sec       | 4 ms                     | 256 sec     |
| 0x03      |   240 us                 | 16  sec     | 8 ms                     | 512 sec     |
| 0x04      |   480 us                 | 32  sec     | 16 ms                    | 1024 sec    |
| 0x05      |   960 us                 | 64 sec      | 32 ms                    | 2048 sec    |
| 0x06      |     2 ms                 | 128 sec     | 64 ms                    | 1:06 hr     |
| 0x07      |     4 ms                 | 256 sec     | 128 ms                   | 2:12 hr     |
| 0x08      |     8 ms                 | 512 sec     | 256 ms                   | 4:24 hr     |
| 0x09      |    16 ms                 | 1024 sec    | 512 ms                   | 8:48 hr     |
| 0x0A      |    32 ms                 | 2048 sec    | 1 sec                    | 17:36 hr    |
| 0x0B      |    64 ms                 | 1:06 hr     | 2 sec                    | 1.5 days    |
| 0x0C      |   128 ms                 | 2:12 hr     | 4 sec                    | 3 days      |
| 0x0D      |   256 ms                 | 4:24 hr     | 8 sec                    | 6 days      |
| 0x0E      |   512 ms                 | 8:48 hr     | 16 sec                   | 12 days     |
| 0x0F      | 1.024 sec                | 17:36 hr    | 32 sec                   | 23 days     |
