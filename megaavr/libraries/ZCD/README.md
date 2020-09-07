# ZCD
A library for interfacing with the zero-cross detector peripheral in the AVR DA and DB series MCUs.  
Developed by [MCUdude](https://github.com/MCUdude/).  
Depending on the pin count of your target you have one, two ir three zero-cross detectors at your disposal.
**Read the AVR DA or DB datasheet to calculate the needed external circuity needed to get the ZCD hardware to trigger when it should!**

## ZeroCross
Class for interfacing with the built-in ZCD. use the predefined objects `zcd`, `zcd1` or `zcd2`.


### output
Variable for setting the ZCD output pin. The output pin will toggle every time a zero-cross is detected.
Accepted values:
``` c++
out::disable; // No output pin
out::enable;  // Enable output pin
out::invert;  // Invert output pin
```

##### Usage
``` c++
zcd.output = out::enable; // Enable output pin
```

##### Default state
`zcd.output` defaults to `out::disable` if not specified in the user program.


### output_swap
Variable for pin swapping the physical output pin to its alternative position. See the pinout diagrams in the DxCore README for detailed info.  
Accepted values:
```c++
out::no_swap;  // Use default pin position (PA7)
out::pin_swap; // Use alternative position (PC6)
```

##### Usage
```c++
zcd.output_swap = out::no_swap; // No pin swap for output
```

##### Default state
`zcd.output_swap` defaults to `out::no_swap` if not specified in the user program.


## init()
Method for initializing the ZCD hardware.

##### Usage
```c++
zcd.init(); // Initialize ZCD
```

## start()
Method for starting the ZCD.
##### Usage
```c++
zcd.start(); // Start zcd
```


## stop()
Method for stopping the ZCD.

##### Usage
```c++
zcd.stop(); // Stop ZCD
```


## attachInterrupt()
Method for enabling ZCD interrupt. The interrupt will trigger when a zero-cross is detected as a rising, falling or change state.
Valid arguments for the second parameters are `RISING`, `FALLING` and `CHANGE`.

##### Usage
```c++
zcd.attachInterrupt(interruptFunction, RISING); // Run interruptFunction when a rising zero-cross is detected

void interruptFunction()
{
  digitalWrite(myLedPin, CHANGE);
}
```


## detachInterrupt()
Method for disabling ZCD interrupt.

##### Usage
```c++
zcd.detachInterrupt(); // Disable interrupt
```
