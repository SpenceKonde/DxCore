# LUT as LATCH
In my opinion, this is one of the fundamental pieces of many of the "CCL Unleashed" type of applications. A "Latch" acts much like an RS-latch (the sequential logic option!) only using just one LUT

This example makes use of the Logic and Event libraries to keep code readable. If you're at the point where you're comfortable without them, you don't need me to tell you how to not use them.

```c++
#include <Logic.h>
#include <Event.h>

void setup() {=
  Event0.set_user(user::ccl0_event_a);    // Connect event_a of Logic0 to Event0
  Event1.set_user(user::ccl0_event_b);    // Connect event_b of Logic0 to Event1
  Logic0.enable=true;                     // Enable logic block 0
  Logic0.output=out::enable;              // Enable logic block 0 output pin
  Logic0.input0=in::event_a;              // Event channel - (we are only using soft_event)
  Logic0.input1=in::event_b;              // Event channel - (we are only using soft_event)
  Logic0.input2=in::feedback;             // Feedback (output from Logic0)
  Logic0.truth=(0b10110010);              // 0xB2 - it's usually easier to see as binary
  Logic0.init();
  Logic::start();
}

void loop() {
  Event0.soft_event();
  delay(1000);
  Event1.soft_event();
  delay(1000);
}
```
### Explanation
In this configuration the output holds whichever state it last had (via feedback) unless input 1 (SET) is HIGH while it's LOW, or input 2 (CLEAR) is HIGH while it is HIGH. In either of those cases it switches to the other state. What is so useful about this is that it allows you to turn pulse events into level events. Since many of the events that one might want to use to "change states" are just pulses, this is a very powerful tool.

Note that this only works on EVEN Logic units! It's not clear from the text of the datasheet (though the diagrams do show it - black and white, clear as crystal) as written above: feedback brings the output of the even Logic unit (or sequencer) back to both even and odd units. However, another event channel can be used instead to bring the output back (but only if one of the inputs to that Logic block isn't an event - each logic block only gets 2 event inputs).

### Application - Moar PWM
This is used in the TCDThirdPWM example sketch.

# Selector
A simple selection switch - that is, it switches between two inputs depending on the state of a third. This is most useful in combination with other uses of the Logic library, or other tools which make use of the event system.
```c++
#include <Logic.h>
#include <Event.h>

void setup() {
  pinMode(PIN_PA2,OUTPUT);                // set the CCL0 input pin 2 as OUTPUT.
  analogWrite(PIN_PC0,16);                // 1/16th duty cycle
  analogWrite(PIN_PC1,127);               // 50% duty cycle
                                          // In practice, you'd probably not have it outputting to the pins.
  Logic0.enable=true;                     // Enable logic block 0
  Logic0.output=out::enable;              // Enable logic block 0 output pin
  Logic0.input0=in::tca0;                 // TCA0 WO0 channel
  Logic0.input1=in::tca0;                 // TCA0 WO1 channel
  Logic0.input2=in::pin;                  // Pin input - note how we set it OUTPUT above
  Logic0.truth=(0b10110010);              // 0xB2 - it's usually easier to see as binary
  Logic0.init();
  Logic::start();
}

void loop() {
  delay(1000);
  digitalWrite(PIN_PA2,HIGH)
  delay(1000);
  digitalWrite(PIN_PA2,LOW)
}
```

### Explanation
The concept is straightforward; there are a few things in above that are noteworthy:
1. The "pin" input to logic doesn't set pinMode - thus, you can use an *OUTPUT* as your input to the logic block. Why would you do such a silly thing? Because it gives you a way to switch logic blocks between two states, whereas soft_event only provides a very brief pulse.


# Toggle
One input signal. When that is pulsed, if the output is high, it will go low, if it is low, it will go high - either way, it will then stay there until the input is pulsed again. In contrast to the "latch" which is trivial to implement - this one is not so simple, requiring two logic blocks and their "sequencer", because - unless steps are taken to prevent it - the first time you switch it, the thing will start oscillating (and won't stop).

```
Pending...
```


# Oscillators
These are various logic blocks and pairs that oscillate, dividing the system clock.
## Synchronized Oscillator, Single
### For even Logic block

```c++
#include <Logic.h>
#include <Event.h>

void setup() {
  Logic0.enable = true;                     // Enable logic block 0
  Logic0.input0 = in::feedback;             // Feedback (output from Logic0)
  Logic0.input1 = in::masked;               // masked
  Logic0.input2 = in::masked;               // masked
  Logic0.output = out::enable;              // Enable logic block 0 output pin
  Logic0.filter = filter::filter or sync;   // synchronizer delays each edge 2 clocks, filter by 4.
  Logic0.truth = 0x01;                      // Set truth table: Invert, HIGH if input0 LOW
  Logic0.init();                            // Initialize logic block 0
}

```
### For Odd Logic block

```c++
#include <Logic.h>
#include <Event.h>

void setup() {

  Logic1.enable = true;                     // Enable logic block 1
  Logic1.input0 = in::event_a;              // Event channel, carrying the output of Logic1
  Logic1.input1 = in::masked;               // masked
  Logic1.input2 = in::masked;               // masked
  Logic1.output = out::enable;              // enable logic block 1 output pin
  Logic1.filter = filter::filter or sync;   // synchronizer delays each edge 2 clocks, filter by 4.
  Logic1.truth = 0x01;                      // Set truth table: Invert, HIGH if input0 LOW
  Logic1.init();                            // Initialize logic block 1

  Event0.set_generator(gen::ccl1_out);      // Use output of Logic1
  Event0.set_user(user::ccl1_event_a);      // Connect Event0 (carrying Logic1 output) to Logic1 event A input
  Event0.start();                           // Enable Event0

  Logic::start();                           // Start the CCL hardware
}

```
### Explanation
Either way, these try to take on the reverse of whatever state they are in - but because their output is delayed by the synchronizer or filter, we get a prescaled clock out instead: Logic0 sees that it's output is HIGH, it's Truth table says it should go LOW. For the next two (four) clock cycles while that new LOW works it's way through the synchronizer, Logic0 has no reason to change. Immediately after the LOW arrives, it outputs HIGH in response. Each time there is that two (or four) cycle delay; 2 or 4 clocks each for rising and falling gives 4 or 8 clocks per cycle of the output.

## Oscillator, double (adding) - Logic x 2, Event x 0

```c++
#include <Logic.h>
#include <Event.h>

void setup() {

  Logic0.enable = true;                     // Enable logic block 0
  Logic0.input0 = in::link;                 // output from Logic1
  Logic0.input1 = in::masked;               // masked
  Logic0.input2 = in::masked;               // masked
  Logic0.output = out::enable;              // Enable logic block 0 output pin
  Logic0.filter = filter::filter or sync;   // synchronizer delays each edge 2 clocks, filter by 4.
  Logic0.truth = 0x01;                      // Set truth table: Invert, HIGH if input0 LOW
  Logic0.init();                            // Initialize logic block 0

  Logic1.enable = true;                     // Enable logic block 1
  Logic1.input0 = in::feedback;             // feedback (from logic 0)
  Logic1.input1 = in::masked;               // masked
  Logic1.input2 = in::masked;               // masked
  Logic1.output = out::enable;              // enable logic block 1 output pin
  Logic1.filter = filter::filter or sync;   // synchronizer delays each edge 2 clocks, filter by 4.
  Logic1.truth = 0x02;                      // Set truth table: Copy, HIGH if input0 HIGH
  Logic1.init();                            // Initialize logic block 1

  Logic::start();                           // Start the CCL hardware
}

```
### Explanation
Two logic blocks with either (F)ilter or (S)ynchronizer.

Even LUT uses output Odd LUT n+1 via `link`, delays 2 or 4 cycles, and outtputs the opposite polarity.

Odd LUT uses output from Even LUT via `feedback`, delays 2 or 4 cycles, and forwards with same polarity.

Even LUT Output is a clock signal with frequency 1/8 (S + S), 1/12 (F + S) or 1/16th (F + F) of the frequency of the of the `clocksource` used.


## Oscillator, double (multiplying) - Logic x 2, Event x 1

```c++
#include <Logic.h>
#include <Event.h>

void setup() {

  Logic0.enable = true;                     // Enable logic block 0
  Logic0.input0 = in::feedback;             // feedback (from Logic0)
  Logic0.input1 = in::masked;               // masked
  Logic0.input2 = in::masked;               // masked
  Logic0.output = out::enable;              // Enable logic block 0 output pin
  Logic0.filter = filter::filter or sync;   // Synchronizer delays each edge 2 clocks, filter by 4.
  Logic0.clocksource = clocksource::clk_per;// Clock from clk_per
  Logic0.truth = 0x01;                      // Set truth table: Invert, HIGH if input0 LOW
  Logic0.init();                            // Initialize logic block 0

  Logic1.enable = true;                     // Enable logic block 1
  Logic1.input0 = in::event_a;              // Event A carrying feedback from Logic1
  Logic1.input1 = in::masked;               // masked
  Logic1.input2 = in::feedback;             // Feedback (from Logic0)
  Logic1.output = out::enable;              // enable logic block 1 output pin
  Logic1.filter = filter::filter or sync;   // synchronizer delays each edge 2 clocks, filter by 4.
  Logic0.clocksource = clocksource::in2;    // Clock from input 2, the output of Logic0
  Logic1.truth = 0x02;                      // Set truth table: Copy, HIGH if input0 HIGH
  Logic1.init();                            // Initialize logic block 1

  Event0.set_generator(gen::ccl1_out);      // Use output of Logic1
  Event0.set_user(user::ccl1_event_a);      // Connect Event0 (carrying Logic1 output) to Logic1 event A input
  Event0.start();                           // Enable Event0

  Logic::start();                           // Start the CCL hardware
}

```
### Explanation
Two logic blocks with either (F)ilter or (S)ynchronizer - but here one, which is clocking the other, rather than replacing the other's feedback input, so they multiply.

Both are set to use their own output (via feedback for an even one, and an event channel for an odd one), and delay 2 or 4 cycles via a synchronizer or filter before switching their output. Which assignment of roles to the even or odd LUT is most efficient depends on the specifics of your use case. Note that these can go in either direction.

The output from the "second" logic block (the one clocked from the "first") is a clock signal with frequency 1/16 (S x S), 1/32 (F x S) or 1/64th (F x F) of the frequency of the of the `clocksource` used.


## Oscillator closing remarks
* Assuming your "output" needs to be an event channel, which it probably does, if you can arrange things so that the output is on a logic block that already needs an event channel for feedback (because it's odd) or to get signal to other blocks (third member of an addition chain), you can save an event channel. Of course, that assumes that there aren't other reasons you need to use a separate event channel for the output.
* A logic block that is used as a clock source for others, if it uses event channel feedback (ie, an odd-numbered block), it does not need to be adjacent to the other block(s). The subsequent step can just connect to the same logic channel.
* If you are using more than 3 logic blocks as oscillators, you are probably approaching your problem the wrong way.
* If you are using more than one multiplication stage, you are probably approaching your problem the wrong way.


# General Notes
* Remember that the whole Logic system must be turned off to make changes to any block. This may be problematic. Event channels, however, can be reconfigured on the fly! This provides a way to switch the behavior of Logic blocks without turning off the CCL peripheral while you do so.
* In this document we've often used the filter as just a delay. Remember that it *does* also filter!
* In all of the examples, we turn on the output pin for the logic blocks we are using. This is largely for debugging purposes (except on some silicon revisions of tinyAVR devices, where LINK input only works when the output pin of the logic block it is linked to is enabled - event output does work without enabling the output pin, however, so an event channel can be substituted, if one can be spared. See the silicon errata for your part to be sure).

# One more thing...
I keep having the feeling that it should be possible to set a device as an SPI slave. Then have it generate a square wave period 1.25 us from the "third" type D pwm channel directed (using external connections) at it's own SCK pin. Feed MISO to logic. MISO state controls a Selector (see above) to choose whether an output is TCD WOA or TCD WOB. Those each start HIGH, the "1"  goes low 3/5ths of the way into the cycle, the "0" 1/5th. This goes to the *actual* output pin. And naturally, you would also be driving your own SS pin low through a jumper while doing this... Your ISR would throw a byte in the SPI data buffer, increment the index, and return (simple!). The result being that you would end up generating "morse code" type output, the sort of thing you normally have to bitbang... without bitbanging. Now what is this data that has to go out at 1.25 us per bit, no faster, but which we have to push out in such volume that we are willing to do all these crazy contortions to do it without CPU involvement? Well, it would be coming from a uint8_t array named something like "pixels"... - this would be a neopixel controller.

I have yet to find a reason why this shouldn't work (finding that the SPI feeds to the CCL were master only was a bummer, but it's not like you can't use event channels). The whole thing is *weird* - but with WS2812 LEDs, you are shackled to the 30us per LED per frame data rate. On every step of that programming you need to be thinking about your need to get that data pushed out. You want to do garage-door-opener style remote control? Okayyy, but you can't transmit or receive while writing data to the LEDs.

Meanwhile, at 32 MHz, which the Dx-series can do no problem, you almost feel guilty using a bitbanging routine, because your "hand tuned assembly" is spending literally 80% of it's time in delay instructions, and calling it "tuning" feels a bit generous when what you did was select your nop, rjmp .+0, and ret instructions so there was just one batch of them and you had only to rcall different places within it for the three different delay durations you can need... all to save a single-digit number of words on a 128k flash chip)....
