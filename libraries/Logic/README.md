# Logic
A library for interfacing with the CCL (Configurable Custom Logic) peripherals on modern AVR MCUs.
Developed by MCUdude for use with [MegaCoreX](https://github.com/MCUdude/MegaCoreX), adapted to tinyAVR 0/1-series parts by [Tadashi G. Takaoka](https://github.com/tgtakaoka), and to DA-series by [Spence Konde](https://github.com/SpenceKonde)
Correction of several issues on the tinyAVR parts and adaptation of examples by [Spence Konde](https://github.com/SpenceKonde).
All of the examples assume the use of megaTinyCore, MegaCoreX or DxCore as appropriate (code is generally directly portable between megaAVR 0-series, tinyAVR 2-series and AVR Dx-series parts, minor modifications may be required for porting to or from tinyAVR 0/1-series parts). This document is focused on the Dx-series parts. For information on this library relating to the megaAVR or tinyAVR parts, see the version of this document included with MegaCoreX or megaTinyCore.
More information about CCL can be found in the [Microchip Application Note TB3218](http://ww1.microchip.com/downloads/en/AppNotes/TB3218-Getting-Started-with-CCL-90003218A.pdf) and in the [megaAVR-0 family data sheet](http://ww1.microchip.com/downloads/en/DeviceDoc/megaAVR0-series-Family-Data-Sheet-DS40002015B.pdf) and the datasheet for the part in question.



## Logic
Class for interfacing with the built-in logic block (sometimes referred to as `LUT`s, from the "LookUp Table" - though it is curious use of language, that is what Microchip refers to them as). Use the predefined objects `Logic0`, `Logic1`, `Logic2`, `Logic3`, `Logic4` and `Logic5`. The logic blocks are paired, each pair sharing a single sequencer and `feedback` channel. Additionally, each logic block is associated with a specific port, having it's input on pins 0 through 2, and it's output on pin 3 or 6 (note that these patterns are different on tinyAVR - the alternate output pin being on a different port). In order: `PORTA`, `PORTC`, `PORTD`, `PORTF`, `PORTB`, and `PORTG`. `Logic4` and `Logic5` are only available on 48 and 64 pin AVR Dx-series devices.

These objects expose all configuration options as member variables as documented below, as well as member methods to set the applicable registers.

### enable
Variable for enabling or disabling a logic block.
Accepted values:
```c++
true;  // Enable the current logic block
false; // Disable the current logic block
```

##### Usage
```c++
Logic0.enable = true; // Enable logic block 0
```

##### Default state
`Logic0.enable` defaults to `false` if not specified in the user program.


### input0..input2
Variable for setting what mode input 0..2 on a logic block should have.

Accepted values for non-tinyAVR parts:

``` c++
in::masked;           // Pin not in use
in::unused;           // Pin not in use
in::disable;          // Pin not in use
in::feedback;         // Connect output of sequencer (if used) or even logic block (n or n-1) to this input
in::link;             // Connect output of logic block n+1 to this input, or block 0 for the last block.
in::event_0;          // Connect input to event a
in::event_a;          // Connect input to event a (preferred)
in::event_1;          // Connect input to event b
in::event_b;          // Connect input to event b (preferred)
in::pin;              // Connect input to CCL IN0, IN1, IN2 for input 0, 1, 2, do not change pinMode
in::input_pullup;     // Connect input to CCL IN0, IN1, IN2 for input 0, 1, 2, set input, pullup on
in::input;            // Connect input to CCL IN0, IN1, IN2 for input 0, 1, 2, set input, pullup off
in::input_no_pullup;  // Connect input to CCL IN0, IN1, IN2 for input 0, 1, 2, set input, pullup off
in::ac;               // Connect input to the output of the internal analog comparator (input 0,1,2 from AC0,1,2)
in::zcd;              // Connect input to the output of the zero crossing detector (input 0,1,2 from ZCD0,1,2) - Dx-series only
in::uart;             // Connect input to UART TX. Input 0 connects to UART0 TX, input 1 to UART1 TX, and input 2 to UART2 TX
in::spi;              // Connect input to SPI. Input 0 and 1 connects to MOSI, and input 2 connects to SCK
in::tca0;             // Connect input to TCA0. Input 0 connects to WO0, input 1 to WO1 and input2 to WO2 (preferred)
in::tca;              // Connect input to TCA0. Input 0 connects to WO0, input 1 to WO1 and input2 to WO2
in::tca1;             // Connect input to TCA1. Input 0 connects to WO0, input 1 to WO1 and input2 to WO2 - DA/DB-series only
in::tcb;              // Connect input to TCB. Input 0 connects to TCB0 W0, input 1 to TCB1 WO, and input 2 to TCB2 WO.
in::tcd;              // Connect input to TCD0. Input 0 connects to WOA, input 1 to WOB and input2 to WOC - Dx-series only
```

Note:
* The 48-pin and 64-pin DA-series parts have 6 LUTs. No pins for CCL5 are available in the 48-pin package.
* On 28-pin parts, IN1 and IN2 inputs for Logic3 are not available. If pin input is needed, use a different block, or use the event system to route pin inputs to them.
* When SPI is used as an input source, inputs 0 and 1 connect to MOSI, there is no input for MISO - these are intended for applications like modulating a squarewave carrier with data, like the remote control for a TV. Note also that the order is different from the ATtiny parts.
* If input on the highest-number Logic block is set to link, it will use the output of Logic0.
* On 28-pin and 32-pin Dx-series parts, link input for the highest-number logic block does not work, at least in the initial silicon releases. See the applicable Errata sheet.
* If you need to link input to logic block other than the n+1 block, you can use the event system for that.
* Not all Dx-series parts have 3 analog comparators or zero-crossing detectors. The datasheet does not specify what happens if, for example, input1 is set to ac on a part with a single analog comparator - it's not clear if it uses the output of AC0 (or ZCD0), or is simply logic 0.
* Timer/Counter input sources are associated with a WO (Waveform Output) channel - they are logic 1 (true) when the PWM output on that channel is `HIGH` (See the datasheet I/O multiplexed signals chart to associate WO channels with pins)
* See the version of this file distributed with megaTinyCore for information on the corresponding options on those parts.

##### Usage
``` c++
Logic0.input0 = in::link;         // Connect output from block 1 to input 0 of block 0
Logic0.input1 = in::input;        // Connect the input 1 from block 0 to its GPIO
Logic0.input2 = in::input_pullup; // Connect the input 2 from block 0 to its GPIO, with pullup on
```

##### Default state
`LogicN.inputN` defaults to `in::unused` if not specified in the user program.


### output
Variable for changing the logic block output pin behavior. Note that the output of the logic block still can be used internally if the output pin is disabled.
Accepted values:
```c++
out::disable; // Disable the output GPIO pin. Useful when triggering an interrupt instead.
out::enable;  // Enable the output GPIO pin
```

##### Usage
```c++
Logic0.output = out::disable; // Disable the output GPIO pin.
```

##### Default state
`LogicN.output` defaults to `out::disable` if not specified in the user program.


### output_swap
Variable for pin swapping the physical output pin to its alternative position. See the pinout diagrams in the [Core this is part of](../../../README.md) for more info.
Accepted values:
```c++
out::no_swap;  // Use default pin position, pin 3 on the port
out::pin_swap; // Use alternative position, pin 6 on the port
```

##### Usage
```c++
Logic0.output_swap = out::no_swap; // No pin swap for output of block0
```

##### Default state
`LogicN.output_swap` defaults to `out::no_swap` if not specified in the user program.


### filter
Variable to control whether the output passes through a synchronizer or filter. Useful when multiple logic blocks are connected internally to prevent race conditions and glitches that could arise due to the asynchronous nature of CCL clocking. Alternately, the delay itself may be desirable, or it can be combined with a configuration which would oscillate asynchronously to instead output a prescaled clock, which could, in turn, be used with "clock on event" to provide a type B timer with a prescaled clock. Either filter or synchronizer is required for edge detector, below.
Accepted values:
```c++
filter::disable;      // No filter used, asynchronous output.
filter::synchronizer; // Connect synchronizer to output; delays output by 2 clock cycles.
filter::synch;        // Syntactic sugar for synchronizer
filter::sync;         // Syntactic sugar for synchronizer
filter::filter;       // Connect filter to output; delays output by 4 clock cycles, only passes output that is stable for >2 clock cycles.
```

##### Usage
```c++
Logic0.filter = filter::filter; // Enable filter on output of block 0
```

See also [Prescaling Clocks with CCLs](https://github.com/SpenceKonde/AVR-Guidance/blob/master/CCL_EVSYS_hacks/CCL_prescaling.md)

##### Default state
`LogicN.filter` defaults to `filter::disable` if not specified in the user program.


### clocksource
Variable to set the clock source for the logic block; this is used for the synchronizer and filter only (otherwise, the logic blocks are asynchronous - and shockingly fast. You can rig them up so that they oscillate, and with the most direct approaches). Note that 32kHz-derived and unprescaled clock options are not available on 0-series and 1-series parts; keep this in mind if backwards portability is important. If sequential logic is used, it is clocked from the clock source used by the even-numbered logic block, if it uses a clock.
Accepted values:
```c++
clocksource::clk_per;      // Clock from the peripheral clock (ie, system clock)
clocksource::in2;          // Clock from the selected input2; it is treated as a 0 in the truth table.
clocksource::oschf;        // Clock from the **unprescaled** internal HF oscillator. Same as clk_per if system clock not prescaled.
clocksource::osc32k;       // Clock from the internal 32.768 kHz oscillator
clocksource::osc1l;        // Clock from the internal 32.768 kHz oscillator prescaled by 32
```


##### Usage
```c++
Logic2.clocksource = clocksource::oschf; // Set block 2 to use unprescaled high frequency internal oscillator.
```

##### Default state
`LogicN.clocksource` defaults to `clocksource::clk_per` if not specified in the user program.



### edgedetect
Variable for controlling use of the edge detector. The edge detector can be used to generate a pulse when detecting a rising edge on its input. To detect a falling edge, the TRUTH table should be programmed to provide inverted output. In order to avoid unpredictable behavior, a valid filter option must be enabled. Note that this is likely only of use when the output is being used for sequential logic or as the input to another logic block; it looks particularly useful on the odd LUT input to a J-K flip-flop sequential logic unit.

```c++
edgedetect::disable;      // No edge detection used
edgedetect::enable;       // Edge detection used
```

### sequencer
Variable for connecting a "sequencer" to the logic block output - these are latches or flip-flops which remember a state. There is 1 sequencer per 2 CCLs, each controls one of the two inputs to a flip flop or latch; this option is ignored for the odd-numbered logic blocks. Flip-flops are clocked from the same clock source as the even logic block, latches are asynchronous.

Accepted values:
```c++
sequencer::disable;      // No sequencer connected
sequencer::d_flip_flop;  // D flip flop sequencer connected
sequencer::jk_flip_flop; // JK flip flop sequencer connected
sequencer::d_latch;      // Gated D latch sequencer connected - note that on most tinyAVR and megaAVR parts, this doesn't work. See the Errata.
sequencer::rs_latch;     // RS latch sequencer connected
```

##### Usage
```c++
Logic0.sequencer = sequencer::disable; // Disable sequencer
```

##### Default state
`LogicN.sequencer` defaults to `sequencer::disable` if not specified in the user program.


### truth
Variable to hold the 8-bit truth table value.
Accepted values between 0x00 and 0xFF.

##### Usage
```c++
Logic0.truth = 0xF0;
```

##### Default state
`LogicN.truth` defaults to `0x00` if not specified in the user program.



## init()
Method for initializing a logic block; the settings you have previously configured will be applied and pins configured as requested at this time only.

##### Usage
```c++
Logic0.init(); // Initialize block 0
Logic1.init(); // Initialize block 1
```


## start()
Method for starting the CCL hardware after desired blocks have been initialized using `LogicN.init()`.

##### Usage
```c++
Logic::start(); // Start CCL hardware
```

## stop()
Method for stopping the CCL hardware, for example to reconfigure the logic blocks.

##### Usage
```c++
Logic::stop(); // Stop CCL
```

## Reconfiguring
There are TWO levels of "enable protection" on the CCL hardware. According to the Silicon Errata, only one of these is intended. As always, it's anyone's guess when or if this issue will be corrected in a future silicon rev, and if so, on which parts (it would appear that Microchip only became aware of the issue after the Dx-series parts were released - although it impacts all presently available parts, it is only listed in errata updated since mid-2020). The intended enable-protection is that a given logic block cannot be reconfigured while enabled. This is handled by `init()` - you can write your new setting to a logic block, call `LogicN.init()` and it will briefly disable the logic block, make the changes, and reenable it.

The unintended layer is that no logic block can be reconfigured without also disabling the whole CCL system. Changes can be freely made to the `Logic` classes, however, only the `init()` method will apply those changes, and you must call `Logic::stop()` before calling them, and `Logic::start()` afterwards. If/when parts become available where this is not necessary, this step may be omitted, and this library may be amended to provide a way to check.

##### Example
```c++


// logic blocks 0, 1 configured, initialized, and in use
Logic1.truth=0x55;      // new truth table
Logic1.input2=tca0;     // and different input 2
Logic3.enabled=true;    // enable another LUT
Logic3.input0=in::link; // Use link from LUT0
Logic3.input1=in::ac;   // and the analog comparator
Logic3.input2=in::pin;  // and the LUT3 IN2 pin
Logic3.truth=0x79;      // truth table for LUT3

Logic3.attachInterrupt(RISING,interruptFunction);

// Interrupt now attached - but - Logic3 not enabled, and logic1 is using old settings

Logic::stop();  // have to turn off Logic0 too, even though I might not want to
Logic1.init();  // apply changes to logic block 1
Logic3.init();  // apply settings to logic block 3 for the first time
Logic::start(); // reenable


```

## attachInterrupt()
Method for enabling interrupts for a specific block.
Valid arguments for the third parameters are `RISING`, `FALLING` and `CHANGE`.
This method isn't available on tinyAVR 0/1-series, as these parts cannot generate an interrupt from the CCL blocks.

##### Usage
```c++
Logic0.attachInterrupt(blinkLED, RISING); // Runthe blinkLED function when the putput goes high

void blinkLED()
{
  digitalWrite(myLedPin, CHANGE);
}
```



## detachInterrupt()
Method for disabling interrupts for a specific block.
This method isn't available on tinyAVR 0/1-series.

##### Usage
```c++
Logic0.detachInterrupt(); // Disable interrupts for block 0
```
