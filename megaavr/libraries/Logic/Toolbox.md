# General Notes
* Remember that the whole Logic system must be turned off to make changes to any block. This may be problematic. Event channels, however, can be reconfigured on the fly! This provides a way to switch the behavior of Logic blocks without turning off the CCL peripheral while you do so.
* In this document we've often used the filter as just a delay. Remember that it *does* also filter!
* In all of the examples, we turn on the output pin for the logic blocks we are using. This is largely for debugging purposes (except on some silicon revisions of tinyAVR devices, where LINK input only works when the output pin of the logic block it is linked to is enabled - event output does work without enabling the output pin, however, so an event channel can be substituted, if one can be spared. See the silicon errata for your part to be sure that it's free of the LINK bug).
* Anything that works with the output ofg a logic block, when the logic block is the even one, it takes the output of the sequencer. When the sequencer is in use, only the output of the odd logic block before the sequencer is available, as the sequencer output replaces the even block output

# Secrets of the CCL Inputs
The CCLs have a bunch of inputs; these are listed under the INSELx bitfields of LUTnCTRLB and LUTnCTRLC. The descriptions in the table are extremely terse; one might assume that a feature which opened fundamentally new use cases to a part family would have at least a full sentence describing them; this does not appear to have been the case. As I see it, the following are ambiguous. Where I have determined the actual behaviour, this is noted.
* MASK - Clearly described, masked inputs always read 0.
* FEEDBACK - Feedback always takes the output of the sequencer (if it is in use) or the EVEN logic block.
* LINK - Gets the output of LUT+1, nothing confusing here.
* EVENT - Worth noting that while you can't modify the LUT without briefly disabling first the whole CCL, then the block in question, and then reenabling them both, you can freely modify the event system registers and change which event channel these are "tuned in" to.
* IO - This is async. How does this interact with the PORTx.PINnCTRL register? digital input disable bit? INVEN? INLVL?
* can - Anyone know of anything weird here?
* USARTn - "TX input source"???  My best guess is that this is the IRDA TX output mechanism that the USART chapter mentions, the counterpart to the RX input event. In actual IRDA this would be a stream of pulses, (ie, it would probably be used with one of the timer outputs) but you can also just use it to pipe the output to a different pin or peripheral. Probably more widely useful, this would also imply that you could move the TX pin to any pin capable of carrying output from the CCL or an event channel! It is hoped that an example of this can be added in the near future.
* TCA/TCD/TCB - these take the WO (waveform output) from the timers. Unlike the compare match event channels, which just give you a pulse, these give a high or low that matches the state that the pin would have!
  * Remap PWM onto CCL output pins when there is no suitable pin otherwise available.
  * Modulate one PWM signal with another at a different frequency (like you could do on some classic AVRs, with one specific channel on two timers on one pin).
  * switch the behavior of a CCL logic block or cascade based on time or - via clock-on-event - a certain number of events. For example, a signal from some peripheral could be used only until it had n positive transitions of an incoming clock signal, after which it was switched off (a complementary logic block could turn it on for a different pin, too), even if the constraints of the system preclude using interrupts for this due to interrupt response time (see note in [Pin Interrupts](../../extras/PinInterrupts.md)). This is used in some of my code to "split" WS2812 signals so that something outputting a signal for a string of 400 LEDs could get the first 200 sent to one string, and the second 200 send to a different string (so you could feed data to the middle of the string - where it's also more efficient to feed the power!)
  * Switch one pin between one of several duty cycles or pulse lengths *instantly* in response to an event (by generating each on it's own channel, then having one logic input switch between them)
* SPI - SPI inputs are either MOSI or SCK. And SCK only on IN2.  (note: On tinyAVR 0/1-series, the SPI inputs are given as SCK, MOSI, and MISO instead or MOSI, MOSI, SCK. MISO seems to have been dropped for all parts released after that, though). These inputs only work in master mode (at least on the DX and 2-series parts - 0/1-series parts may be different - the MISO input gives one reason to wonder, at least). I think the idea of the SPI inputs is that you can use MOSI and SCK to implement special methods for sending data using SPI - for example, manchester encoding it for special protocols. I have not done anything particularly exciting with this on paper - the inability to use an arbitrary clock speed/source for SPI precludes many of the obvious use cases here.
* ACn, ZCDn - clearly described, no further explanation needed.

## Pin I/O List
* Input from pin
  * 3 preferred inputs on the blocks "home" port: No added cost.
    * Px0
    * Px1
    * Px2
  * Pin inputs already connected to an event channel
  * Any pin on any port for which at least one event channel is available. Cost: 1 event channel
* Output on pin
  * Preferred outputs on the block's "home" port: No added cost
    * Px3
    * Px6 (alternate)
  * Event output on any port. Cost: 1 event channel if CCL output isn't already on event channel.
    * Py2
    * Py7 (alternate)


# Switches

## LUT as LATCH
In my opinion, this is one of the fundamental pieces of many of the "CCL Unleashed" type of applications. A "Latch" acts much like an RS-latch (the sequential logic option) - covering only simple cases (where there is no processing of either input through logic needed) - but uses only one logic block, instead of a pair of them. Since no part has enough logic blocks, and considering how quickly you can use them, anything that lets us use one fewer logic block is a blessing.
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
2. You can't reconfigure logic blocks without shutting down the CCL entirely, so you *DO* need to do things like this if you need it to continue operating without an interruption (either on this channel - or on some other logic block)


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

  Logic::start();                           // Start the CCL hardware
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
  Logic1.clocksource = clocksource::in2;    // Clock from input 2, the output of Logic0
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


# One more thing...
As I noted above, the SPI clock is too inflexible for many applications that the CCL SPI handling initially seemed suitable for. I think it still is, just less gracefully (two jumpered pins needed, but it should be possible to set a device as an SPI slave, wire outputs to SS and SCK, so you can assert SS and supply an arbitrary clock... at which point. the SPI modules is no longer an SPI module. You'd probably run in buffered mode because the whole idea is to give yourself more maneuvering room when you've got something with tight timing constraints to satisfy, and you've got a buffered shift register that signals with interrupt when it needs to be fed.  jumpered to SS and SCK so code on the device could assert SS, and provide an arbitrary clock signal to the SPI in slave made. If you put that into buffered mode and kept it fed properly, this would give you your arbitrary-SCK-SPI-port. And the components above could - if desired be used to get output in a different form. You would need to use pin inputs to get the values being output on the MISO line, of course, if you needed that... I think this can be used to create an interrupt driven WS2812 driver, or to take WS2812B datastream as input...
