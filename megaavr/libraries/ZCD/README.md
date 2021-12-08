# ZCD
A library for interfacing with the zero-cross detector peripheral in the AVR DA and DB series MCUs.
Developed by [MCUdude](https://github.com/MCUdude/).
Depending on the pin count of your target you have one, two or three zero-cross detectors at your disposal.
**External components are required for these to function** - these are described in great detail in the ZCD chapter of the datasheet.

ZCD instance | Input pin | Output pin(s) | Availability
-------------|-----------|---------------|----------------
ZCD0         | PD1       |    PA7 or PC7 | All AVR DA/DB
ZCD1         | PE3       |    PA7 or PC7 | 48/64-pin DA/DB
ZCD2         | PE7       |    PA7 or PC7 | 64-pin parts only
ZCD3         | PC2       |           PA7 | AVR DD-series

The selection of output pins may look meagre, especially considering that PA7 is shared with CLKOUT, the analog comparators and a variety of other compelling functions, and that PC7 is only present on parts with more than 48 pins - but remember that you can use these as event generators and CCL inputs, allowing you to effectively remap the output them to about half of the available I/O pins on a given device. Moreover, you will most likely wish to use them with the event system, rather than just getting a direct output, for example to restart a timer in order to do AC dimming.

`ZeroCross` is the class for interfacing with the built-in ZCD. On the DA and DB-series parts, use the predefined objects `zcd0`, `zcd1` or `zcd2`. On the DD-series parts, for reasons known only to Microchip (though it might have something to do with their newfound dedication to consistent pin numbering and the fact that none of the ZCD pins are present on the smaller DD-series parts), the single zero-crossing detector is ZCD3, so these parts will have a `zcd3` object defined instead.  Generally to use a ZCD, you would first set output and/or output_swap if you needed output on a pin, then call the `init()` method to configure the hardware and apply those settings, and finally `start()` to turn it on

## ZeroCross properties

### output
Property for setting the ZCD output pin. The output pin will toggle every time a zero-cross is detected.
Accepted values:
``` c++
out::disable; // No output pin
out::enable;  // Enable output pin
out::invert;  // Invert output pin
```

#### Usage
``` c++
zcd.output = out::enable; // Enable output pin
```

#### Default state
`zcd.output` defaults to `out::disable` if not specified in the user program.


### output_swap
Property to control swapping the physical output pin to its alternative position.  See the pinout diagrams in the DxCore README for detailed info.
Note that the initial silicon revisions of the DA and DB parts had a silicon bug - all the ZCDs were controlled by the first bit of the PORTMUX.ZCDROUTEA register, and the other two available bits did nothing. According to the Microchip silicon errata, no silicon revisions without this issue are yet available for the DA-series; however, the issue has largely spared the DB-series parts. Only the initial A4 release of the AVR128DB was impacted, and it was superseded almost immediately by the A5 die revision (the bugs in the A4 die revision were, apparently, so critical and functionality-breaking that a fix was needed immediately)

Accepted values:
```c++
out::no_swap;  // Use default pin position (PA7)
out::pin_swap; // Use alternative position (PC7)
```

#### Usage
```c++
zcd.output_swap = out::no_swap; // No pin swap for output
```

#### Default state
`zcd.output_swap` defaults to `out::no_swap` if not specified in the user program.

## ZeroCross methods

### init()
Method for initializing the ZCD hardware, and apply the settings configured by the variables described above. Call this before the `start()` method.
#### Usage
```c++
zcd.init(); // Initialize ZCD
```

### start()
Call this method to enable the zero-crossing detector.
#### Usage
```c++
zcd.start(); // Start zcd
```


### stop()
Disables the ZCD

#### Usage
```c++
zcd.stop(); // Stop ZCD
```


## attachInterrupt()
The attachInterrupt method allows enabling of the ZCD interrupt and the designation of a user-supplied function to call. The interrupt will trigger when a zero-cross is detected as a rising, falling or change state.
Valid arguments for the second parameters are `RISING`, `FALLING` and `CHANGE`.

### Usage
```c++
zcd.attachInterrupt(interruptFunction, RISING); // Run interruptFunction when a rising zero-cross is detected

void interruptFunction() {
  digitalWrite(myLedPin, CHANGE);
}
```


## detachInterrupt()
Method for disabling ZCD interrupt.

### Usage
```c++
zcd.detachInterrupt(); // Disable interrupt
```

## have_separate_mux()
Static method to test whether the the ZCDROUTEA register which controls the pin swapping for the ZCD outputs, is functional. It relies on testing the part ID and die revision.

### Usage
```c++
if (ZeroCross::have_separate_mux()) {
  Serial.println("The ZCD portmux works on this part");
} else {
  Serial.println("All ZCDs are controlled by the ZCD0 bit...")
}
```
