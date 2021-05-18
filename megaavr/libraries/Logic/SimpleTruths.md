# Simple Truths
These are useful values for the truth table that are handy to remember.

You can use bitwise logic to combine them, too!

## Static Logic
When there is no feedback, you end up with a very simple bit of logic - everything can be generated from these three primitives, and the bitwise operators!

### Basic Patterns
These will make output follow the given input or be the opposite of the given input, ignoring the state of any other inputs. Combine them using bitwise operators to get more complex logic...

```c++
#define LOGIC_IN0 (0xAA) // 0b10101010
#define LOGIC_IN1 (0xCC) // 0b11001100
#define LOGIC_IN2 (0xF0) // 0b11110000

#define INVERT_IN0 (~LOGIC_IN0) // 0b01010101
#define INVERT_IN1 (~LOGIC_IN1) // 0b00110011
#define INVERT_IN2 (~LOGIC_IN2) // 0b00001111

```

### Bitwise Math
Here is where it gets surprisingly easy... Say you want it to be LOW unless input 0 and 1 are both high - but input 2 should reverse that if it's high...
```c++
uint8_t truthtable = (LOGIC_IN0 & LOGIC_IN1); // We want 0 AND 1
uint8_t truthtable1 = truthtable ^ LOGIC_IN2; // and we want to reverse it when 2.  ((LOGIC_IN0 & LOGIC_IN1) ^  LOGIC_IN2)
// truthtable1 = 0x78 - if 2 is set, output HIGH unless both 0 and 1 are. If 2 is clear, OUTPUT LOW unless both 0 and 1 are HIGH.
uint8_t truthtable2 = truthtable | LOGIC_IN2; // (0 AND 1) OR 2                     ((LOGIC_IN0 & LOGIC_IN1) |  LOGIC_IN2)
// truthtable1 = 0xF8 - If 2 ias HIGH, output HIGH, otherwise, only output HIGH if both 0 and 1 are high.
uint8_t truthtable3 = truthtable & LOGIC_IN2; // (0 AND 1) AND 2                    (LOGIC_IN0  & LOGIC_IN1  &  LOGIC_IN2)
// truthtable1 = 0x80 - Output LOW unless all three inputs are HIGH.
uint8_t truthtable3 = truthtable & ~LOGIC_IN2; // (0 AND 1) AND 2                   (LOGIC_IN0  & LOGIC_IN1  & ~LOGIC_IN2)
// truthtable1 = 0x04 - output high only if 0 and 1, but not 2 are high

// And YOU CAN DO THIS IN ALL SITUATIONS!!
// Those three primitives, and the bitwise operators | (OR), & (AND), ~ (NOT), and ^ (XOR) can get you any logical combination.

uint8_t truthtable1 = (LOGIC_IN0 & LOGIC_IN1) ^ LOGIC_IN2; // and we want to reverse it when 2.  ((LOGIC_IN0 & LOGIC_IN1) ^ LOGIC_IN2)
// Equivilantly, we could say "I want 0 AND 1 AND NOT 2"
uint8_t truthtable4 = (LOGIC_IN0 & LOGIC_IN1) & (~LOGIC_IN2); // 3-input OR              (LOGIC_IN0  | LOGIC_IN1  | LOGIC_IN2)

```

### Masked inputs
When inputs are masked, a smaller number of bits in the truth table matter, effectively.
If we are following IN0 exactly, (0xAA) but the other two inputs are masked:
```c++
0b10101010   & ~(0b11001100)  & ~(0b11110000)
0b10101010   &   0b00110011   &   0b00001111
0b10101010   &   0b00000011   =   0b00000010 = 0x02
inverted:
0b01010101   &   0b00000011   =   0b00000001 = 0x01

0b11001100   & ~(0b10101010)  & ~(0b11110000)
0b11001100   &   0b01010101   &   0b00001111
0b11001100   &   0b00000101   =   0b00000100 = 0x04
inverted:
0b00110011   &   0b00000101   =   0b00000001 = 0x01

0b11110000   & ~(0b10101010)  & ~(0b11001100)
0b11110000   &   0b01010101   &   0b00110011
0b11110000   &   0b00010001   =   0b00010000 = 0x10
inverted:
0b00001111   &   0b00010001   =   0b00000001 = 0x01



```


## Dynamics
When there is feedback, the situation suddenly becomes a completely different animal... The situation of the oscillator has been covered in depth in that document,

### Latches
Suddenly - the logic above, while it can represent typical use cases, becomes quite a bit uglier - and less useful for understanding behavior
Take 0xD4 - Feedback on input 2, input 1 sets it as long as input 0 is low, and input 0 clears it if input 1 is low, and otherwise, it is unchanged...
Change happens when LOGIC_IN1 ^ LOGIC_IN0.

```c++
uint8_t truth= (LOGIC_IN2 | (LOGIC_IN1 & (LOGIC_IN1 ^ LOGIC_IN0))) & ~(LOGIC_IN0 & (LOGIC_IN1 ^ LOGIC_IN0))

  LOGIC_IN1 ^ LOGIC_IN0                = 0b 0110 0110 = 0x66
  LOGIC_IN0 & (LOGIC_IN1 ^ LOGIC_IN0)  = 0b 0010 0010 = 0x22
~(LOGIC_IN0 & (LOGIC_IN1 ^ LOGIC_IN0)) = 0b 1101 1101 = 0xDD

  LOGIC_IN1 ^ LOGIC_IN0                = 0b 0110 0110 = 0x66
  LOGIC_IN1 & (LOGIC_IN1 ^ LOGIC_IN0)  = 0b 0100 0100 = 0x44

0xF0 | 0x44 & 0xDD
       0xF4 & 0xDD = 0b 1111 0100 & 0b 1101 1101
```
We'll call the conditions which cause the output to go into its high state the Set Condition, and the one which causes it to go low to be the Clear Condition.

Is there anything we can say in general about latch truth tables?
Consider the above example. We had to say what it does if both set and reset input are high. We declared that there would be no change (and, whichever one stayed high longer would at that time have it's way, if it didn't already. That is viable. Declaring that set or clear would always have precedence is also viable. But if they set + clear would clear it if it was set, and set it if it was clear, that is no longer a latch - that's an oscillator. These sort of configurations will often oscillate at a frequency far higher than the CPU if run asynchronously,

```c++
Feedback |   Set   |   Clear | Both |  LUT |
  INPUT2 |  INPUT1 |  INPUT0 | STAY | 0xD4 |
  INPUT2 |  INPUT1 |  INPUT0 |  LOW | 0x54 |
  INPUT2 |  INPUT1 |  INPUT0 | HIGH | 0xDC |
  INPUT2 |  INPUT1 |  INPUT0 |  OPP | 0x5C |
  INPUT2 |  INPUT0 |  INPUT1 | STAY | 0xB2 |
  INPUT2 |  INPUT0 |  INPUT1 |  LOW | 0x32 |
  INPUT2 |  INPUT0 |  INPUT1 | HIGH | 0xBA |
  INPUT2 |  INPUT0 |  INPUT1 |  OPP | 0x3A |

  INPUT2 |  INPUT1 | ~INPUT0 | STAY | 0xE8 |
  INPUT2 |  INPUT1 | ~INPUT0 |  LOW | 0xA8 |
  INPUT2 |  INPUT1 | ~INPUT0 | HIGH | 0xEC |
  INPUT2 |  INPUT1 | ~INPUT0 |  OPP | 0xAC |
  INPUT2 |  INPUT0 | ~INPUT1 | STAY | 0xE8 |
  INPUT2 |  INPUT0 | ~INPUT1 |  LOW | 0xA8 |
  INPUT2 |  INPUT0 | ~INPUT1 | HIGH | 0xEC |
  INPUT2 |  INPUT0 | ~INPUT1 |  OPP | 0xAC |

  INPUT2 | ~INPUT1 |  INPUT0 | STAY | 0xD4 |
  INPUT2 | ~INPUT1 |  INPUT0 |  LOW | 0x54 |
  INPUT2 | ~INPUT1 |  INPUT0 | HIGH | 0xDC |
  INPUT2 | ~INPUT1 |  INPUT0 |  OPP | 0x5C |
  INPUT2 | ~INPUT0 |  INPUT1 | STAY | 0xB2 |
  INPUT2 | ~INPUT0 |  INPUT1 |  LOW | 0x32 |
  INPUT2 | ~INPUT0 |  INPUT1 | HIGH | 0xBA |
  INPUT2 | ~INPUT0 |  INPUT1 |  OPP | 0x3A |

  INPUT2 | ~INPUT1 | ~INPUT0 | STAY | 0xD4 |
  INPUT2 | ~INPUT1 | ~INPUT0 |  LOW | 0x54 |
  INPUT2 | ~INPUT1 | ~INPUT0 | HIGH | 0xDC |
  INPUT2 | ~INPUT1 | ~INPUT0 |  OPP | 0x5C |
  INPUT2 | ~INPUT0 | ~INPUT1 | STAY | 0xB2 |
  INPUT2 | ~INPUT0 | ~INPUT1 |  LOW | 0x32 |
  INPUT2 | ~INPUT0 | ~INPUT1 | HIGH | 0xBA |
  INPUT2 | ~INPUT0 | ~INPUT1 |  OPP | 0x3A |

```
