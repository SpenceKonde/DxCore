# Event
A library for interfacing with the built-in Event system on the megaAVR-0 series MCUs.
Developed by [MCUdude](https://github.com/MCUdude/).

**From the datasheet:**
> The Event System (EVSYS) enables direct peripheral-to-peripheral signaling. It allows a change in one peripheral (the event generator) to trigger actions in other peripherals (the event users) through event channels, without using the CPU. It is designed to provide short and predictable response times between peripherals, allowing for autonomous peripheral control and interaction, and also for synchronized timing of actions in several peripheral modules. It is thus a powerful tool for reducing the complexity, size, and execution time of the software.

More information about the Event system and how it works can be found in the [Microchip Application Note AN2451](http://ww1.microchip.com/downloads/en/AppNotes/DS00002451B.pdf) and in the [megaAVR-0 family data sheet](http://ww1.microchip.com/downloads/en/DeviceDoc/megaAVR0-series-Family-Data-Sheet-DS40002015B.pdf).

### Level vs. Pulse events
There are two types of events - a "pulse" interrupt, which lasts for the duration of a single clock cycle (either `CLK_PER` or a relevant (slower) clock - for example, the USART XCK generator provides a pulse event which lasts one XCK period, whuich is far slower than CLK_PER), or a "level" interrupt which lasts for the duration of some condition. Often for a given even generator or user only one or the other makes sense. Less often, for some reason or another, you may need a level event, but all you have is a pulse event - or the other way around. A[CCL module (Logic.h)](../Logic/Readme.md) event between the two at the cost of the logic module and one event channel. In the case of timer WO (PWM) channels, the CCL already has level inputs. (1/21 - TODO: Describe, either here or in Logic library - it's the sort of thing that makes CCL so powerful.)

### Synchronization
The event system, under the hood, is asynchronous - it can react faster than the system clock (often a lot faster). Per the datasheet:

>Events can be either synchronous or asynchronous to the peripheral clock. Each Event System channel has two subchannels: one asynchronous and one synchronous.

>The asynchronous subchannel is identical to the event output from the generator. If the event generator generates a signal asynchronous to the peripheral clock, the signal on the asynchronous subchannel will be asynchronous. If the event generator generates a signal synchronous to the peripheral clock, the signal on the asynchronous subchannel
>will also be synchronous.

>The synchronous subchannel is identical to the event output from the generator, if the event generator generates a signal synchronous to the peripheral clock. If the event generator generates a signal asynchronous to the peripheral clock, this signal is first synchronized before being routed onto the synchronous subchannel. Depending on when it occurs, synchronization will delay the event by two to three clock cycles. The Event System automatically performs
>this synchronization if an asynchronous generator is selected for an event channel.

The fact that it is asynchronous usually doesn't matter - it's either "faster than anything else" (if the user is async, or "2-3 clock cycles behind", but it is one of the things one should keep in mind when using these features, because every so often it does.


### Some of these events seem kinda... weird?
At first glance, nore than half of the users and generators seem, at best, odd - and a good few of them might appear entirely useless. Most of the event system can only truly be understood when considering the full range of generators and users - particularly the CCL. One of the tragedies of a datasheet is that it - generally - lacks a "why". Behind every mysterious event, there is a use case that seems obscure to most people - but within some sub-field, it's common and essential. There are also times when something may seem surprising until you're more familiar with the event and logic systems in general.

### How do I read the levels of the event channels?
From your code? As far as I can tell, short of piping them to a pin and reading that, you don't (and no, I really don't understand why they couldn't have tied those synchronizers that connnect the internal async channels to the sync ones to the bits of a register - but I'm not Microchip engineeer.

## Event
Class for interfacing with the built-in Event system. Each event generator channel has its own object.
Use the predefined objects `Event0`, `Event1`, `Event2`, `Event3`, `Event4`, `Event5`, `Event6`, `Event7`, `Event8`, and `Event9`. Note that channels have different functionality, so make sure you use the right channel for the task.

In short:
* `genN::rtc_div8192`, `genN::rtc_div4096`, `genN::rtc_div2048` and `genN::rtc_div1024` are only available on odd numbered channels
* `genN::rtc_div512`, `genN::rtc_div256`, `genN::rtc_div128` and `genN::rtc_div64` are only available on even numbered channels
* Only parts with 48 or 64 pins have `Event8` and `Event9`
* PIN PA0..7 and PB0..7 can only be used as event generators on channel 0 and 1
* PIN PC0..7 and PD0..7 can only be used as event generators on channel 2 and 3
* PIN PE0..7 and PF0..7 can only be used as event generators on channel 4 and 5
* PIN PG0..7 can only be used as event generators on channel 6 and 7
* Channels 8 and 9 do not have any generators that take pins as inputs

## get_channel_number()
Function to get the current channel number. Useful if the channel object has been passed to a function as reference.

### Usage
``` c++
uint8_t this_channel = Event0.get_channel_number();  // In this case, get_channel_number will return 0
```

## get_user_channel()
Function to get what event channel a user is connected to. Returns -1 if not connected to any channel. Note that we use `user::` as prefix when we refer to event users. Also, note that we don't have to specify an object to determine what channel the user is connected to.
An event generator can have multiple event users, but an event user can only have one event generator.

### Usage
```c++
uint8_t connected_to = Event::get_user_channel(user::ccl0_event_a); // Returns the channel number ccl0_event_a is connected to
```


## set_generator()
Function to assign an event generator to a channel. Note that we use the prefix genN:: (where N is the channel number) when referring to generators unique to this particular channel. we use gen:: when referring to generators available on all generators.

### Usage
```c++
Event0.set_generator(gen::ccl0_out); // Use the output of logic block 0 (CCL0) as an event generator for Event0
Event2.set_generator(gen2::pin_pc0); // Use pin PC0 as an event generator for Event2
```

### Generator table
Below is a table with all possible generators for each channel. Many generators are associated with specific peripherals, and on low pin-count devices, attempting to use those will result in a compile error.

**The table in the original README applied to the megaAVR 0-series, not the Dx-series; the table for the Dx parts will be added at a later date.**


## get_generator()
Function to get the generator used for a particular channel.

### Usage
```c++
uint8_t generator_used = Event0.get_generator();
if(generator_used == gen::ccl0_out) {
  Serial.println("We're using gen::ccl0_out as generator");
}
```


## set_user()
Function to connect an event user to an event generator. Note that a generator can have multiple users.

### Usage
```c++
Event0.set_generator(gen0::pin_pa0); // Set pin PA0` as event generator for Event0
Event0.set_user(user::evoutc);       // Set EVOUTC (pin PC2) as event user
Event0.set_user(user::evoutd);       // Set evoutD (pin PD2) as event user
```

### Event User table
Below is a table with all of the event users for the AVR Dx-series parts.
#### Notes:
* The `evoutN_pin_pN7` is the same as `evoutN_pin_pN2` but where the pin is swapped from 2 to 7. This means that for instance, `evouta_pin_pa2` can't be used in combination with `evouta_pin_pa7`.
* Many of these refer to specific pins or peripherals - on smaller pin-count devices, some of these event users are not available; Attempting to set a user that doesn't exist will result in a compile error.
* There is no PF7 on either the DA or DB-series parts. There will be on the DD-series parts.


| Event users                                                              |
|--------------------------------------------------------------------------|
| `user::ccl0_event_a`                                                     |
| `user::ccl0_event_b`                                                     |
| `user::ccl1_event_a`                                                     |
| `user::ccl1_event_b`                                                     |
| `user::ccl2_event_a`                                                     |
| `user::ccl2_event_b`                                                     |
| `user::ccl3_event_a`                                                     |
| `user::ccl3_event_b`                                                     |
| `user::ccl4_event_a`                                                     |
| `user::ccl4_event_b`                                                     |
| `user::ccl5_event_a`                                                     |
| `user::ccl5_event_b`                                                     |
| `user::adc0_start`                                                       |
| `user::ptc_start`       **AVR DB only**                                  |
| `user::evouta_pin_pa2`                                                   |
| `user::evouta_pin_pa7`  *evouta_pin_pa2 with pin swapped*                |
| `user::evoutb_pin_pb2`                                                   |
| `user::evoutb_pin_pb7`  *evouta_pin_pb2 with pin swapped*                |
| `user::evoutc_pin_pc2`                                                   |
| `user::evoutc_pin_pc7`  *evouta_pin_pc2 with pin swapped*                |
| `user::evoutd_pin_pd2`                                                   |
| `user::evoutd_pin_pd7`  *evouta_pin_pd2 with pin swapped*                |
| `user::evoute_pin_pe2`                                                   |
| `user::evoute_pin_pe7`  *evouta_pin_pe2 with pin swapped*                |
| `user::evoutf_pin_pf2`                                                   |
| `user::evoutf_pin_pf7`  *evouta_pin_pf2 with pin swapped*                |
| `user::evoutg_pin_pg2`                                                   |
| `user::evoutg_pin_pg7`  *evouta_pin_pa2 with pin swapped*                |
| `user::usart0_irda`                                                      |
| `user::usart1_irda`                                                      |
| `user::usart2_irda`                                                      |
| `user::usart3_irda`                                                      |
| `user::usart4_irda`                                                      |
| `user::usart5_irda`                                                      |
| `user::tca0_cnta`                                                        |
| `user::tca0_cntb`                                                        |
| `user::tca1_cnta`                                                        |
| `user::tca1_cntb`                                                        |
| `user::tcb0_capt`                                                        |
| `user::tcb0_cnt`                                                         |
| `user::tcb1_capt`                                                        |
| `user::tcb1_cnt`                                                         |
| `user::tcb2_capt`                                                        |
| `user::tcb2_cnt`                                                         |
| `user::tcb3_capt`                                                        |
| `user::tcb3_cnt`                                                         |
| `user::tcb4_capt`                                                        |
| `user::tcb4_cnt`                                                         |
| `user::tcd0_in_a`                                                        |
| `user::tcd0_in_b`                                                        |
| `user::opamp0_enable`   **AVR DB only**                                  |
| `user::opamp0_disable`  **AVR DB only**                                  |
| `user::opamp0_dump`     **AVR DB only**                                  |
| `user::opamp0_drive`    **AVR DB only**                                  |
| `user::opamp1_enable`   **AVR DB only**                                  |
| `user::opamp1_disable`  **AVR DB only**                                  |
| `user::opamp1_dump`     **AVR DB only**                                  |
| `user::opamp1_drive`    **AVR DB only**                                  |
| `user::opamp2_enable`   **AVR DB only**                                  |
| `user::opamp2_disable`  **AVR DB only**                                  |
| `user::opamp2_dump`     **AVR DB only**                                  |
| `user::opamp2_drive`    **AVR DB only**                                  |


## clear_user()
Function to detach a user from a channel. Note that you don't need to know what channel to detach from, simply use `Event::clear_user()`.

### Usage
```c++
Event::clear_user(user::evouta); // Remove the user::evouta from whatever event channel it is connected to.
```

## soft_event()
Creates a single software event - users connected to that channel will react to it in the same way as they would to one caused by the generator the channel is connected to. Great if you have to force trigger something. Note that a software event only lasts a single system clock cycle, so it's rather fast! The software events will invert the channel, and so will trigger something regardless of whether it needs a the event channel to go high or low.

### Usage
```c++
Event0.soft_event(); // Create a single software event on Event0
```

## start()
Starts an event generator channel by writing the generator selected by `set_generator()` method to the `EVSYS.CHANNELn` register.

### Usage
```c++
Event0.start(); // Starts the Event0 generator channel
```

## stop()
Stops an event generator channel by clearing the `EVSYS.CHANNELn` register. The `EventN` object retains memory of what generator it was previously set to.=

### Usage
```c++
Event0.stop(); // Stops the Event0 generator channel
```
