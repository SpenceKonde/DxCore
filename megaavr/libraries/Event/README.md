# Event
A library for interfacing with the built-in Event system on the megaAVR-0 series MCUs, and ported to Dx-series and tinyAVR 0/1/2.
Developed by [MCUdude](https://github.com/MCUdude/).

**From the datasheet:**
> The Event System (EVSYS) enables direct peripheral-to-peripheral signaling. It allows a change in one peripheral (the event generator) to trigger actions in other peripherals (the event users) through event channels, without using the CPU. It is designed to provide short and predictable response times between peripherals, allowing for autonomous peripheral control and interaction, and also for synchronized timing of actions in several peripheral modules. It is thus a powerful tool for reducing the complexity, size, and execution time of the software.

## Overview
The event system allows routing of signals from event "generators" (outputs on various peripherals) through an event "channel", which one or more "event users" can be connected. When an event signal coming from the generator is is "active" or "high", the users connected to the same channel as the generator will perform certain specified actions depending on the peripheral. Generators are often the sorts of things that generate an interrupt if that is enabled - but some things can generate constant level events (such as following the state of a pin). The event users can do a wide variety of things. Opamps can be enabled and disabled entirely through events, the ACD can kick off a staged measurement. Type A and B timers can count them, and type B timers can measure their duration or time between them with input capture. USARTs can even use them as their RX input! This is nice and all - but what really makes the Event system reach it's potential is CCL (configurable custom logic) which can use events as inputs, in addiion to having access to several more internal sourcesof "event-like" signals - and being event generators in their own right.

More information about the Event system and how it works can be found in the [Microchip Application Note AN2451](http://ww1.microchip.com/downloads/en/AppNotes/DS00002451B.pdf) and in the [megaAVR-0 family data sheet](http://ww1.microchip.com/downloads/en/DeviceDoc/megaAVR0-series-Family-Data-Sheet-DS40002015B.pdf).

## Level vs. Pulse events
There are two types of events - a "pulse" interrupt, which lasts for the duration of a single clock cycle (either `CLK_PER` or a relevant (slower) clock - for example, the USART XCK generator provides a pulse event which lasts one XCK period, which is far slower than CLK_PER (the "peripheral; . while the TCD0-derived ones are) or a "level" interrupt which lasts for the duration of some condition. Often for a given generator or user only one or the other makes sense. Less often, for some reason or another, you may need a level event, but all you have is a pulse event - or the other way around. A [CCL module (Logic.h)](../Logic/README.md) event between the two at the cost of the logic module and one event channel. In the case of timer WO (PWM) channels, the CCL already has level inputs to match the pulse inputs that the event system can get on compare match. Many of the pulse interrupts are generated at the same moment that interrupt flags get set - except that where an interrupt bit sticks around until serviced, the pulse bit is present generally for only a single clock cycle (meaning they are only *usable* for triggering other event system things or peripherals).

## Synchronization
>Events can be either synchronous or asynchronous to the peripheral clock. Each Event System channel has two subchannels: one asynchronous and one synchronous.
>The asynchronous subchannel is identical to the event output from the generator. If the event generator generates a signal asynchronous to the peripheral clock, the signal on the asynchronous subchannel will be asynchronous. If the event generator generates a signal synchronous to the peripheral clock, the signal on the asynchronous subchannel
>will also be synchronous.
>The synchronous subchannel is identical to the event output from the generator, if the event generator generates a signal synchronous to the peripheral clock. If the event generator generates a signal asynchronous to the peripheral clock, this signal is first synchronized before being routed onto the synchronous subchannel. Depending on when the event occurs, synchronization will delay the it by two to three clock cycles. The Event System automatically perform this synchronization if an asynchronous generator is selected for an event channel.

The fact that it is asynchronous usually doesn't matter - it's either "faster than anything else" (if the user is async), or "2-3 clock cycles behind", but it is one of the things one should keep in mind when using these features, because every so often it does. As I understand it, an event coming from the TCD domain could be *faster* that a the main clock. I am not sure *which* TCD clock this means (is it the clock going in? clock after the sync prescaler? Or the clock after the count prescaler? Can the system actually miss events, or is there some mechanism to prevent this? )


### Generator table
Below is a table with all possible generators for each channel. Many generators are associated with specific peripherals, and on low pin-count devices, attempting to use peripherals not present on the selected part will result in a compile error.

|  All Pins                |  Event0                 |  Event1                 |  Event2                 |  Event3                 |  Event4                 |  Event5                 |  Event6                 |  Event7                 |  Event8                 |  Event9 |
|--------------------------|-------------------------|-------------------------|-------------------------|-------------------------|-------------------------|-------------------------|-------------------------|-------------------------|-------------------------|----------------------|
| `gen::disable`           | `gen0::disable`         | `gen1::disable`         | `gen2::disable`         | `gen3::disable`         | `gen4::disable`         | `gen5::disable`         | `gen6::disable`         | `gen7::disable`         | `gen8::disable`         | `gen9::disable`      |
| `gen::updi_synch`        | `gen0::rtc_div8192`     | `gen1::rtc_div512`      | `gen2::rtc_div8192`     | `gen3::rtc_div512`      | `gen4::rtc_div8192`     | `gen5::rtc_div512`      | `gen6::rtc_div8192`     | `gen7::rtc_div512`      | `gen8::rtc_div8192`     | `gen9::rtc_div512`   |
| `gen::rtc_ovf`           | `gen0::rtc_div4096`     | `gen1::rtc_div256`      | `gen2::rtc_div4096`     | `gen3::rtc_div256`      | `gen4::rtc_div4096`     | `gen5::rtc_div256`      | `gen6::rtc_div4096`     | `gen7::rtc_div256`      | `gen8::rtc_div4096`     | `gen9::rtc_div256`   |
| `gen::rtc_cmp`           | `gen0::rtc_div2048`     | `gen1::rtc_div128`      | `gen2::rtc_div2048`     | `gen3::rtc_div128`      | `gen4::rtc_div2048`     | `gen5::rtc_div128`      | `gen6::rtc_div2048`     | `gen7::rtc_div128`      | `gen8::rtc_div2048`     | `gen9::rtc_div128`   |
| `gen::ccl0_out`          | `gen0::rtc_div1024`     | `gen1::rtc_div64`       | `gen2::rtc_div1024`     | `gen3::rtc_div64`       | `gen4::rtc_div1024`     | `gen5::rtc_div64`       | `gen6::rtc_div1024`     | `gen7::rtc_div64`       | `gen8::rtc_div1024`     | `gen9::rtc_div64`    |
| `gen::ccl1_out`          | `gen0::pin_pa0`         | `gen1::pin_pa0`         | `gen2::pin_pc0`         | `gen3::pin_pc0`         | `gen4::pin_pe0`         | `gen5::pin_pe0`         | `gen6::pin_pg0`         | `gen7::pin_pg0`         |                         |                      |
| `gen::ccl2_out`          | `gen0::pin_pa1`         | `gen1::pin_pa1`         | `gen2::pin_pc1`         | `gen3::pin_pc1`         | `gen4::pin_pe1`         | `gen5::pin_pe1`         | `gen6::pin_pg1`         | `gen7::pin_pg1`         |                         |                      |
| `gen::ccl3_out`          | `gen0::pin_pa2`         | `gen1::pin_pa2`         | `gen2::pin_pc2`         | `gen3::pin_pc2`         | `gen4::pin_pe2`         | `gen5::pin_pe2`         | `gen6::pin_pg2`         | `gen7::pin_pg2`         |                         |                      |
| `gen::ac0_out`           | `gen0::pin_pa3`         | `gen1::pin_pa3`         | `gen2::pin_pc3`         | `gen3::pin_pc3`         | `gen4::pin_pe3`         | `gen5::pin_pe3`         | `gen6::pin_pg3`         | `gen7::pin_pg3`         |                         |                      |
| `gen::adc0_ready`        | `gen0::pin_pa4`         | `gen1::pin_pa4`         | `gen2::pin_pc4`         | `gen3::pin_pc4`         | `gen4::pin_pe4`         | `gen5::pin_pe4`         | `gen6::pin_pg4`         | `gen7::pin_pg4`         |                         |                      |
| `gen::usart0_xck`        | `gen0::pin_pa5`         | `gen1::pin_pa5`         | `gen2::pin_pc5`         | `gen3::pin_pc5`         | `gen4::pin_pe5`         | `gen5::pin_pe5`         | `gen6::pin_pg5`         | `gen7::pin_pg5`         |                         |                      |
| `gen::usart1_xck`        | `gen0::pin_pa6`         | `gen1::pin_pa6`         | `gen2::pin_pc6`         | `gen3::pin_pc6`         | `gen4::pin_pe6`         | `gen5::pin_pe6`         | `gen6::pin_pg6`         | `gen7::pin_pg6`         |                         |                      |
| `gen::usart2_xck`        | `gen0::pin_pa7`         | `gen1::pin_pa7`         | `gen2::pin_pc7`         | `gen3::pin_pc7`         | `gen4::pin_pe7`         | `gen5::pin_pe7`         | `gen6::pin_pg7`         | `gen7::pin_pg7`         |                         |                      |
| `gen::usart3_xck`        | `gen0::pin_pb0`         | `gen1::pin_pb0`         | `gen2::pin_pd0`         | `gen3::pin_pd0`         | `gen4::pin_pf0`         | `gen5::pin_pf0`         |                         |                         |                         |                      |
| `gen::spi0_sck`          | `gen0::pin_pb1`         | `gen1::pin_pb1`         | `gen2::pin_pd1`         | `gen3::pin_pd1`         | `gen4::pin_pf1`         | `gen5::pin_pf1`         |                         |                         |                         |                      |
| `gen::tca0_ovf_lunf`     | `gen0::pin_pb2`         | `gen1::pin_pb2`         | `gen2::pin_pd2`         | `gen3::pin_pd2`         | `gen4::pin_pf2`         | `gen5::pin_pf2`         |                         |                         |                         |                      |
| `gen::tca0_hunf`         | `gen0::pin_pb3`         | `gen1::pin_pb3`         | `gen2::pin_pd3`         | `gen3::pin_pd3`         | `gen4::pin_pf3`         | `gen5::pin_pf3`         |                         |                         |                         |                      |
| `gen::tca0_cmp0`         | `gen0::pin_pb4`         | `gen1::pin_pb4`         | `gen2::pin_pd4`         | `gen3::pin_pd4`         | `gen4::pin_pf4`         | `gen5::pin_pf4`         |                         |                         |                         |                      |
| `gen::tca0_cmp1`         | `gen0::pin_pb5`         | `gen1::pin_pb5`         | `gen2::pin_pd5`         | `gen3::pin_pd5`         | `gen4::pin_pf5`         | `gen5::pin_pf5`         |                         |                         |                         |                      |
| `gen::tca0_cmp2`         | `gen0::pin_pb6`         | `gen1::pin_pb6`         | `gen2::pin_pd6`         | `gen3::pin_pd6`         | `gen4::pin_pf6`         | `gen5::pin_pf6`         |                         |                         |                         |                      |
| `gen::tcb0_capt`         | `gen0::pin_pb7`         | `gen1::pin_pb7`         | `gen2::pin_pd7`         | `gen3::pin_pd7`         |                         |                         |                         |                         |                         |                      |
| `gen::tcb1_capt`         |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::tcb2_capt`         |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::tcb3_capt`         |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::ccl4_out`          |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::ccl5_out`          |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::ac1_out`           |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::ac2_out`           |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::zcd0_out`          |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::zcd1_out`          |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::zcd2_out`          |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::usart4_xck`        |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::usart5_xck`        |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::spi1_sck`          |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::tca1_ovf_lunf`     |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::tca1_hunf`         |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::tca1_cmp0`         |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::tca1_cmp1`         |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::tca1_cmp2`         |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::tcb4_capt`         |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::tcb0_ovf`          |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::tcb1_ovf`          |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::tcb2_ovf`          |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::tcb3_ovf`          |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::tcb4_ovf`          |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::tcd0_cmpbclr`      |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::tcd0_cmpaset`      |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::tcd0_cmpbset`      |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::tcd0_progev`       |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::mvio_ok`           |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::opamp0_ready`      |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::opamp1_ready`      |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::opamp2_ready`      |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |
| `gen::zcd3_out`          |                         |                         |                         |                         |                         |                         |                         |                         |                         |                         |

### Event User table
Below is a table with all of the event users for the AVR Dx-series parts.
#### Notes
* The `evoutN_pin_pN7` is the same as `evoutN_pin_pN2` but with the pin swapped using PORTMUX. This means that for instance, `evouta_pin_pa2` can't be used in combination with `evouta_pin_pa7`.
* Many of these refer to specific pins or peripherals - on smaller pin-count devices, some of these event users are not available; Attempting to set a user that doesn't exist will result in a compile error.
* There is no PF7 on either the DA or DB-series parts. There will be on the DD-series parts.
* The DB and DD-series parts do not have a Peripheral Touch Controller (PTC). Just as well we can't use it because they keep all the libraries proprietary and it was never usable.
* For backwards compatibilirty gen::tcbN is an alias ofd gen:tcbN_capt. The new name is preferred.


| Event users             |                                    |
|-------------------------|------------------------------------|
| `user::ccl0_event_a`    |                                    |
| `user::ccl0_event_b`    |                                    |
| `user::ccl1_event_a`    |                                    |
| `user::ccl1_event_b`    |                                    |
| `user::ccl2_event_a`    |                                    |
| `user::ccl2_event_b`    |                                    |
| `user::ccl3_event_a`    |                                    |
| `user::ccl3_event_b`    |                                    |
| `user::ccl4_event_a`    |                                    |
| `user::ccl4_event_b`    |                                    |
| `user::ccl5_event_a`    |                                    |
| `user::ccl5_event_b`    |                                    |
| `user::adc0_start`      |                                    |
| `user::ptc_start`       | **AVR DA only**                    |
| `user::evouta_pin_pa2`  |                                    |
| `user::evouta_pin_pa7`  | *evouta_pin_pa2 with pin swapped*  |
| `user::evoutb_pin_pb2`  |                                    |
| `user::evoutb_pin_pb7`  | *evouta_pin_pb2 with pin swapped*  |
| `user::evoutc_pin_pc2`  |                                    |
| `user::evoutc_pin_pc7`  | *evouta_pin_pc2 with pin swapped*  |
| `user::evoutd_pin_pd2`  |                                    |
| `user::evoutd_pin_pd7`  | *evouta_pin_pd2 with pin swapped*  |
| `user::evoute_pin_pe2`  |                                    |
| `user::evoute_pin_pe7`  | *evouta_pin_pe2 with pin swapped*  |
| `user::evoutf_pin_pf2`  |                                    |
| `user::evoutf_pin_pf7`  | *evouta_pin_pf2 with pin swapped*  |
| `user::evoutg_pin_pg2`  |                                    |
| `user::evoutg_pin_pg7`  | *evouta_pin_pa2 with pin swapped*  |
| `user::usart0_irda`     |                                    |
| `user::usart1_irda`     |                                    |
| `user::usart2_irda`     |                                    |
| `user::usart3_irda`     |                                    |
| `user::usart4_irda`     |                                    |
| `user::usart5_irda`     |                                    |
| `user::tca0_cnt_a`      | alias: tca0 (deprecated)           |
| `user::tca0_cnt_b`      |                                    |
| `user::tca1_cnt_a`      |                                    |
| `user::tca1_cnt_b`      |                                    |
| `user::tcb0_capt`       | alias: tcb0 (deprecated)           |
| `user::tcb0_cnt`        |                                    |
| `user::tcb1_capt`       | alias: tcb1 (deprecated)           |
| `user::tcb1_cnt`        |                                    |
| `user::tcb2_capt`       | alias: tcb2 (deprecated)           |
| `user::tcb2_cnt`        |                                    |
| `user::tcb3_capt`       | alias: tcb3 (deprecated)           |
| `user::tcb3_cnt`        |                                    |
| `user::tcb4_capt`       | alias: tcb4 (deprecated)           |
| `user::tcb4_cnt`        |                                    |
| `user::tcd0_in_a`       |                                    |
| `user::tcd0_in_b`       |                                    |
| `user::opamp0_enable`   | **AVR DB only**                    |
| `user::opamp0_disable`  | **AVR DB only**                    |
| `user::opamp0_dump`     | **AVR DB only**                    |
| `user::opamp0_drive`    | **AVR DB only**                    |
| `user::opamp1_enable`   | **AVR DB only**                    |
| `user::opamp1_disable`  | **AVR DB only**                    |
| `user::opamp1_dump`     | **AVR DB only**                    |
| `user::opamp1_drive`    | **AVR DB only**                    |
| `user::opamp2_enable`   | **AVR DB only**                    |
| `user::opamp2_disable`  | **AVR DB only**                    |
| `user::opamp2_dump`     | **AVR DB only**                    |
| `user::opamp2_drive`    | **AVR DB only**                    |

The DX-series and tinyAVR 2-series type B timers got a second event generator (tcbN_ovf) and user (tcbN_cnt), while the type A timers got a second event user, tcaN_cnt_b.

This broke existing code, because this changed the name of the other generator to distinguish the two, so what had been called "tcb0" was now "tcb0_capt" - We provide aliases for these older names in order to facilitate code portability, just like for generators.

### Generator table summary - in words
* `genN::rtc_div8192`, `genN::rtc_div4096`, `genN::rtc_div2048` and `genN::rtc_div1024` are only available on odd numbered channels
* `genN::rtc_div512`, `genN::rtc_div256`, `genN::rtc_div128` and `genN::rtc_div64` are only available on even numbered channels
* Only parts with 48 or 64 pins have `Event8` and `Event9`
* PIN PA0..7 and PB0..7 can only be used as event generators on channel 0 and 1
* PIN PC0..7 and PD0..7 can only be used as event generators on channel 2 and 3
* PIN PE0..7 and PF0..7 can only be used as event generators on channel 4 and 5
* PIN PG0..7 can only be used as event generators on channel 6 and 7
* Channels 8 and 9 do not have any generators that take pins as inputs
* Only the pin and RTC/PIT div event generators are limited to a subset of channels. All others are available on all channels

| Part Series | Event Channels | Chan/port | Layout   | 1port ch | pinless  |
|-------------|----------------|-----------|----------|----------|----------|
| AVR DB/DA   | 8 <48pin or 10 |         2 |AB-CD-EF-G|       2  |       2  |
| AVR DD      |   Always 6     |         2 |  A-CD-F  |    *2-4  |       0  |
| AVR EA      | Supposedly 6   |  Likely 2 | Unknown* |2-4 likely|       0  |
| megaAVR 0   | 6 < 48pin or 8 |         2 | AB-CD-EF |  2 or 0  |  0 or 2  |
| tinyAVR 2   |   Always 6     |         4 | AB-CA-BC |  2 or 0  |       0  |
| tinyAVR 1   | 2 sync 4 async |         2 |   Wacky  |       4  |1,2 or 4  |
| tinyAVR 0   | 1 sync 2 async |    1 or 2 |V. Wacky  |  1 or 0  |       0  |

`*` Likely AB-CD-EF

The tinyAVR 0/1-series layouts are really weird. See the documentation included with megaTinyCore for the table; 0-series don't have any RTC-PIT options, and 1-series has them all crowded onto one channel; both of them have two kinds of channels, and are generally a lot less flexible. The tinyAVR 2-series, on the other hand, is almost normal - except they have a better layout - (channels 0/1 get PA, PB, 2/3 get PC, PA and the last 2 get PB, PC. It's unclear how EA will be handled, Taken at face value the EA's product brief implies 6 channels in the standard layout, for 2 channels per port. 14-pin AVR-DD will have only 2 pins in PORTA (and with no portB, those 2 pins have channels 0 and 1 to themselves), and with no port E either, the 14 and 20pin parts will have only the two pins on PORTF, which are not going to be everyone's first choice for I/O (PF6 and PF7 are Reset and UPDI), so channels 4 and 5 are nearly pinless - assuming that what's in the headers pre-release is what they will ship. There is room for improvement, though it's really not bad.


## Okay, so how do I read the state of these event channels?
You don't? Even though each channel on the 2-series supposedly has a synchronizer on it, they for some reason didn't pipe those synchronized bits to a register that we could read from software, that would seem to be the natural thing to do. I'm not sure why, or whether there was a well-reasoned decision making process around this, or it was simply an oversight. An argument can certainly be made "what are you doing reading it from software?" (to which the obvious answer is "Because it's not working and I'm trying to figure out why").

The best you can do is pipe the outputs to a pin and read the pin....


## The Event class
Class for interfacing with the Event systemn (`EVSYS`). Each event channel has its own object.
Use the predefined objects `Event0`, `Event1`, `Event2`, `Event3`, `Event4`, `Event5`. These, alternately, are known as `EventSync0`, `EventSync1`, `EventAsync0`, `EventAsync1`, `EventAsync2`, `EventAsync3` on the tinyAVR 0-series and 1-series. Additionally, there is an `Event_empty` that is returned whenever you call a method that returns an Event reference, but it can't fulfil your request.  Note that not all generators are available on all channels; see the tables above. On 2-series, it's fairly simple, and, all else being equal,  the first channels are the most useful, so the standard rule of thumb of "whenever you use a generator that can go anywhere, use the highest number port" holds. With so many options for each pin, relative to the number of pins, that's unlikely to be a problem. The 0 and 1-series oparts are more complicated. Obviously,on 8 pin parts, the two channels that can do PORTA are the most valuable, and the 1 or 3 channels without them a better choice, when you have a choice. The sixth channel, present only on 1-series, has the RTC division options in place of pins. It is either critical to your application, or the least useful, with no middle ground.

### get_channel_number()
Method to get the current channel number. Useful if the channel object has been passed to a function as reference.

#### Usage
``` c++
uint8_t what_channel(Event& event_chan) {
  event_chan.get_channel_number();
}
```
### get_channel()
Static method. This returns the event object of that number. This is useful when you are programmatically choosing channels at runtime; It's very hard to get from the integer 3, for example, to the Event3 token you'd need to use to refer to it without this method, all you can do is have a big long if/elseif or switch/case structure to find the one you want. If the channel is not valid, you are instead given `Event_empty`

#### Usage
```c++

&Event MyEventChannel = Event::get_channel(MyUsedChannelCount);
if (MyEventChannel.get_channel_number() == 255) {
  // damn, out of channels.
  return;
} else {
  MyUsedChannelCount++;
}
```

### get_user_channel_number()
Method to get what event channel a user is connected to. Returns -1 if not connected to any channel. Note that we use `user::` as prefix when we refer to event users. This is a static method - you don't have to specify an object to determine what channel the user is connected to. An event channel, and hence an event generator, can have as many event users are you want - but an event user can only have one event generator. You cannot get a list or count of all users connected to a generator except by iterating over the list.

#### Usage
```c++
uint8_t connected_to = Event::get_user_channel_number(user::ccl0_event_a); // Returns the channel number ccl0_event_a is connected to
```

### get_user_channel()  **Changed in 1.2.0**
What used to be called this is now get_user_channel_number();
Method to get the channel a user is connected to; unlike get_user_channel_number() this returns an `&Event` - that is, a reference to the Event object.

#### Usage
```c++
&Event MyEventChannel = Event::get_user_channel(user::ccl0_event_a); // don't know what that channel is nor do care, as long I can point the AC at it instead.
MyEventChannel.set_generator(gen::ac0_out);

```

### set_generator()
Method to set the event generator of a channel. This is member method, and can only be called on an existing event object (contrast with the assigmn_generator(), which also finds a suitable channel) Note that we use the prefix genN:: (where N is the channel number) when referring to generators unique to this particular channel. we use gen:: when referring to generators available on all generators.

This can ALSO take as it's argument an arduino pin number. Assuming the pin exists and is a valid generator for that channel, this will set it as the input. The codepath for set_generator(pin) is more efficient than that for set_generator(genN::pin_pxn);

#### Usage
```c++
Event0.set_generator(gen::ccl0_out); // Use the output of logic block 0 (CCL0) as an event generator for Event0
Event2.set_generator(gen2::pin_pc0); // Use pin PC0 as an event generator for Event2
Event2.set_generator(PIN_PC0);       // Use pin PC0 as an event generator for Event2, more efficiently.
```

### get_generator()
Method to get the generator used for a particular channel.

#### Usage
```c++
uint8_t generator_used = Event0.get_generator();
if(generator_used == gen::ccl0_out) {
  Serial.println("We're using gen::ccl0_out as generator");
}
```

### assign_generator()
This is a static method it can be called without referencing a specific channel. You pass the name of any generator to it. The "Assign" methods perform basic event channel management. This method will check all applicable channels in the hopes of finding another channel that is already pointed at that generator - if it does, it will return a reference to that Event channel. Failing that, it will check every channel, looking for ones that are not connected to any other generators. Assuming it finds one, it will set it to use that generator and return a reference to that channel. If there are no channels pointing at that generator, nor any that are not connected to any generators, this will return Event_empty.

```c++
&Event MyEventChannel = Event::assign_generator(gen::ccl1_out);
if MyEventChannel.get_channel_number() != 255 {
  doStuffWithIt();
}
```
While it is legal to pass things like `gen1::pin_pa2` that is less efficient than just calling.....

### assign_generator_pin()
This static method works exactly the same as assign_generator - but it is meant to operate on pin numbers, not generator names. If assign_generator determines that that's what you passed it, it will call this, but it's obviously more efficient to call it directly. Passing invalid pins will also get you Empty_event.

### Usage
```c
&Event pinEvent = Event::assign_generator_pin(PIN_PC2);
if pinEvent.get_channel_number() != 255 {
  Serial.println("Oh laaaaame! No channels that can use the pin are available?!");
}
```

### set_user()
Method to connect an event user to an event generator. Note that a generator can have multiple users.

#### Usage
```c++
Event0.set_generator(gen0::pin_pa0); // Set pin PA0` as event generator for Event0
Event0.set_user(user::evoutc);       // Set EVOUTC (pin PC2) as event user
Event0.set_user(user::ccl0_event_a); // Also set CCL0 Event 0/Event A in event user (See Logic library)
```

### set_user_pin()
This is the analog of set_user above - the difference being that it takes a pin number as an argument. This must be a pin that supports event output - the number of the pin within the port must be 2 or 7, and you cannot use both pin 2 and pin 7 of the same port at the same time.

```c++
Event0.set_user_pin(PIN_PB2); //
```
Note: if you're totally out of event out pins, but not CCL blocks, you can set up a CCL to use the event channel as an input, and and use a CCL output pin; for example, if you set input 0 to be the event channel and masked the other inputs (and if this is all you're doing with it, that's what you'd do), you would want to set the truth table to 0x02.


### clear_user()
Method to detach a user from a channel. This is a static method - you don't need to know what channel to detach from, simply use `Event::clear_user()`.

#### Usage
```c++
Event::clear_user(user::evouta); // Remove the user::evouta from whatever event channel it is connected to.
```

### soft_event()
Creates a single software event - users connected to that channel will react to it in the same way as they would to one caused by the generator the channel is connected to. This can be used either to force something which could occur naturally (ex: for testing) - or the your use case may be predicated on pulses being entirely software generated in this way. Note that a software event only lasts a single system clock cycle - so it will not pass through a CCL's filter (for example) which could be useful, but is more likely to be undesirable. The software events will invert the channel, and so will trigger something regardless of whether it needs a the event channel to go high or low.

#### Usage
```c++
Event0.soft_event(); // Create a single software event on Event0
```

### long_soft_event()
soft_event() is only one system clock long. Often you need something a little longer. 2 clocks? 4 clocks maybe? Maybe it needs to get past the 'filter' on a CCL... Or you've got 2 things that need to happen a fraction of a microsecond apart; you've tried triggering one on the rising edge of a soft_event and the other on the falling edge, but it's just a hair too fast, and the other obvious solutions aren't viable due to the specifics of your scheme. The only way that a soft-event can last multiple system clocks is if the register is written every clock cycle. There is no time got a loop; only brute force (ie, a contiguous block of 'ST'ore instructions) will do the trick. Now in many ways this defeats the point of the event system; however, there are times when it is not entirely unreasonable to do this (as noted above), particularly if doing weird things with the CCLs, event system, and other peripherals.

The lengths that are available are 2, 4, 6, 10 and 16 (any number less than 4 will give 2 clock-long pulse, only 4 will give a 4 clock long one. Anything between 4 and 10 will give 6, exactly 10 will give 10, and anything larger will give 16). Those numbers were chosen arbitrarily to keep the size small, and give a selection that covered the reasonable use cases I could think of. If you need longer, you shouold definitely use a different approach.

#### Usage
```c++
Event0.long_soft_event(4); // will invert the state of the event channel for 4 system clock cycles (200ns at 20 MHz)
Event0.long_soft_event(10); // will invert the state of the event channel for 10 system clock cycles (500ns at 20 MHz)

```
Don't forget that this is an invert, not a "high" or "low". It should be entirely possible for the event that normally drives it to occur resulting in the state changing during that pulse, depending on it's configuration. Note also that the overhead of long_soft_event is typically several times the length of the pulse due to calculating the bitmask to write; it's longer with higher numbered channels. if that is a problem, whatever your use case is, it is not one of the ones this was intended for...


### start()
Starts an event generator channel by writing the generator selected by `set_generator()` method to the `EVSYS.CHANNELn` register.

#### Usage
```c++
Event0.start(); // Starts the Event0 generator channel
```

### stop()
Stops an event generator channel by clearing the `EVSYS.CHANNELn` register. The `EventN` object retains memory of what generator it was previously set to.

#### Usage
```c++
Event0.stop(); // Stops the Event0 generator channel
```

### **gen_from_peripheral()** and **user_from_peripheral()**
These two static methods allow you to pass a reference to a peripheral module, and get back the generator or user associated with it. In this context the "Peripheral Modules" are the structs containing the registers, defined in the io headers; for example `TCB0` or `USART1` or `CCL`.

This is most useful if you are writing portable (library) code that uses the Event library to interact with the event system. Say you made a library that lets users make one-shot pulses with timerB. You're not particularly interesting in getting your hands too dirty with the event system especially considering just how filthy the event system is on the 0/1-series. So you use the Event library to handle that part. You would of course need to know which timer to use - the natural way would be to ask the user to pass a reference or pointer. But then what? The fact that you've got the pointer to something which, as it happens, is TCB0 (which itself is annoying to determine from an unknown pointer)... though even KNOWING THAT, you're not able to use it with the event library, since it needs user::tcb0 (or user::tcb0_capt). As of the version distributed with megaTinyCore 2.5.0 and DxCore 1.4.0, there's a method for that. As the names imply, one gives generators, the other gives users. They take 2 arguments, the first being a pointer to a peripheral struct. The second, defaulting to 0, is the "type" of generator or user. Some peripherals have more than one event input or output. These are ordered in the same order as they are in the tables here and in the datasheet listings.

#### Usage
```c
// Here we see a typical use case - you get the generator, and immediately ask Event to assign a channel to it and give that to you. After getting it, you test to make sure it's not Event_empty, which indicates that either gen_from_peripheral failed, or assign_generator was out of event channels. Either way that's probably the user's fault, so you decide to return an error code.
uint8_t init(TCB_t* some_timer, /*and more arguments, most likely */) { // User could pass any TCB, and will expect your code to work!
  &Event_TCBnCapt = Event::assign_generator(Event::gen_from_peripheral(some_timer,0));
  if (_TCBnCapt.get_channel_number() == 255)
    return MY_ERROR_INVALID_TIMER_OR_NO_FREE_EVENT;
  doMoreCoolStuff();
}

```

Shown below, generators/user per instance  (second argument should be less than this; zero-indexed), and the number of instances (for reference)

| Peripheral |   tiny0  |   tiny1  |   tiny2  |   mega0  |  DA/DB/DB   |
|------------|----------|----------|----------|----------|-------------|
| TCAn       | 5 / 1 x1 | 5/1 x1   | 5 / 2 x1 | 5 / 1 x1 |  5 / 2 x1-2 |
| TCBn       |    ***   |    ***   | 2 / 2 x2 | 1/1 x3-4 |  2 / 2 x2-5 |
| CCL **     | 2 / 4    | 2 / 4    | 4 / 8    | 4 / 8    |4-6 / 8-12   |
| ACn        | 1 / 0 x1 | */0 x1-3 | 1 / 0 x1 | 1 / 0 x1 |  1 / 0 x1-3 |
| USARTn     | 0 / 1 x1 | 0/1 x1   | ! / 1 x2 | !/1 x3-4 |  ! / 1 x2-6 |

`*` - the tiny1 parts with 1 AC work normally. This is unfortuately not supported for tiny1 parts with the triple-AC configuration:
`**` - There is only one CCL peripheral, with multiple logic blocks. Each logic block has 1 event generator and 2 event users. If using the logic library, get the Logic instance number. The output generator is that number. The input is twice that number, and twice that number + 1.
`***` - This peripheral is not supported becausse the generator numbers are channel dependent.
`!` - These parts do have an option, but we didn't bother to implement it because it isn't particularly useful. But the Event RX mode combined with the TX input to the CCL permit arbitrary remapping of RX and very flexible remapping of TX.

And what they are:

| Peripheral |   TCAn   | TCBn |  CCL**  | ACn | USARTn  |
|------------|----------|------|---------|-----|---------|
| gen 0      | OVF/LUNF | CAPT | LUT0OUT | OUT |    !    |
| gen 1      |     HUNF | *OVF | LUT1OUT |     |         |
| gen 2      |     CMP0 |      | LUT2OUT |     |         |
| gen 3      |     CMP1 |      | LUT3OUT |     |         |
| gen 4      |     CMP2 |      |   ETC.  |     |         |
| user 0     |   EVACTA | CAPT | LUT0EVA |  -  | EVENTRX |
| user 1     | * EVACTB | *CNT | LUT0EVB |     |         |
| user 2     |          |      | LUT1EVA |     |         |
| user 2     |          |      | LUT1EVB |     |         |
| user 2     |          |      |   ETC.  |     |         |

`*` - indicates that this is not available on all parts
`**` - Since there's only one CCL, the pointer (or rather, its type) is just used to select which implementation is used. But this does mean that the CCL can have an insane number of options. But that's fine, because there are plenty of numbers between 0 and 255.
`!` - These parts do have an option, but we didn't bother to implement it because it isn't particularly useful. But the Event RX mode combined with the TX input to the CCL permit arbitrary remapping of RX and very flexible remapping of TX!


Asking for a generator that doesn't exist will return 0 (disabled); be sure to check for this in some way. Asking for a user that doesn't exist will return 255, which the library is smart enough not to accept. The most likely way this will happen is if you request with code written for TCA or TCB that needs one of the new features added with 2-series and Dx-series.
