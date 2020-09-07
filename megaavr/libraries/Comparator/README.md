# Comparator
A library for interfacing with the analog comparator peripheral in the AVR DA and DB series MCUs.  
Developed by [MCUdude](https://github.com/MCUdude/).  
The AVR DA/DB has three comparators where four positive and three negative pins are available for use. An alternative for the negative pin is to use an internally generated reference voltage instead.


## Comparator
Class for interfacing with the built-in comparator. use the predefined objects `Comparator`, `Comparator1` or `Comparator2`.


### input_p
Variable for setting what input pin the positive input of the comparator should be connected to 
Accepted values:
``` c++
in_p::in0; // Use positive input pin 0 as input
in_p::in1; // Use positive input pin 1 as input
in_p::in2; // Use positive input pin 2 as input
in_p::in3; // Use positive input pin 3 as input
```

##### Usage
``` c++
Comparator.input_p = in_p::in0;  // Connect positive input pin 0 to the positive pin of the comparator
```

##### Default state
`Comparator.input_p` defaults to `in_p::in0` if not specified in the user program.


### input_n
Variable for setting what input pin the negative input of the comparator should be connected to 
Accepted values:
``` c++
in_n::in0;    // Use positive input pin 0 as input
in_n::in1;    // Use positive input pin 1 as input
in_n::in2;    // Use positive input pin 2 as input
in_p::dacref; // Use DACREF as input
```

##### Usage
``` c++
Comparator.input_n = in_n::in0;  // Connect negative input pin 0 to the negative pin of the comparator
```

##### Default state
`Comparator.input_n` defaults to `in_n::in0` if not specified in the user program.


### reference
Variable for setting what reference voltage the DACREF should use. This voltage is internally generated. 
Accepted values:
``` c++
ref::disable;    // Do not use any reference
ref::vref_1v024; // 1.024V internal voltage
ref::vref_2v048; // 2.048V internal voltage
ref::vref_2v500; // 2.5V internal voltage
ref::vref_2v5;
ref::vref_4v096; // 4.096V internal voltage
ref::vref_vdd;   // VDD as reference
ref::vref_vrefa; // External reference from the VREFA pin
```

##### Usage
``` c++
Comparator.reference = ref::vref_4v096;  // Use the internal 4.096V reference for the DACREF
```

##### Default state
`Comparator.reference` defaults to `ref::disable` if not specified in the user program.


### dacref
Variable for setting the DACREF value. The DACREF voltage is the voltage that the comparator uses as it's reference.  
This is the formula for the DACREF output voltage:  
  
<img src="http://latex.codecogs.com/svg.latex?V_{DACREF} = \frac{Comparator.dacref}{256} * Comparator.reference" border="0"/>

##### Usage
``` c++
Comparator.dacref = 127;  // Divide the reference voltage by two
```

##### Default state
`Comparator.dacref` defaults to `255` if not specified in the user program.


### hysteresis
Variable for setting the comparator input hysteresis. Useful for eliminating "bouncing".
Accepted values:
``` c++
hyst::disable; // No hysteresis
hyst::small;   // 10mV hysteresis (±5mV)
hyst::medium;  // 25mV hysteresis (±12.5mV)
hyst::large;   // 50mV hysteresis (±25mV)
```

##### Usage
``` c++
Comparator.hysteresis = hyst::large;  // Use 50V hysteresis
```

##### Default state
`Comparator.hysteresis` defaults to `hyst::disable` if not specified in the user program.


### output
Variable for setting the comparator output pin (PA7).
Accepted values:
``` c++
out::disable; // No output pin
out::enable;  // Enable output pin (PA7)
out::invert;  // Invert output pin (PA7)
```

##### Usage
``` c++
Comparator.output = out::enable; // Enable output pin (PA7)
```

##### Default state
`Comparator.output` defaults to `out::disable` if not specified in the user program.


### output_swap
Variable for pin swapping the physical output pin to its alternative position. See the pinout diagrams in the DxCore README for detailed info.  
Accepted values:
```c++
out::no_swap;  // Use default pin position
out::pin_swap; // Use alternative position
```

##### Usage
```c++
Comparator.output_swap = out::no_swap; // No pin swap for output
```

##### Default state
`Comparator.output_swap` defaults to `out::no_swap` if not specified in the user program.


### output_initval
Initial state the comparator output pin has when initialized.
Accepted values:
```c++
out::init_low;  // Output pin low after initialization
out::init_high; // Output pin high after initialization
```

##### Usage
```c++
Comparator.output_initval = out::init_high;
```

##### Default state
`Comparator.output_initval` defaults to `out::init_low` if not specified in the user program.


## init()
Method for initializing the comparator.

##### Usage
```c++
Comparator.init(); // Initialize comparator
```

## start()
Method for starting the analog comparator.
##### Usage
```c++
Comparator.start(); // Start comparator
```


## stop()
Method for stopping the analog comparator.

##### Usage
```c++
Comparator.stop(); // Stop comparator
```


## attachInterrupt()
Method for enabling analog comparator interrupt. The interrupt will trigger when the the comparator output changes.
Valid arguments for the third parameters are `RISING`, `FALLING` and `CHANGE`.

##### Usage
```c++
Comparator.attachInterrupt(blinkLED, RISING); // Run the blinkLED function when the comparator output goes high

void blinkLED()
{
  digitalWrite(myLedPin, CHANGE);
}
```


## detachInterrupt()
Method for disabling analog comparator interrupt.

##### Usage
```c++
Comparator.detachInterrupt(); // Disable interrupt
```
