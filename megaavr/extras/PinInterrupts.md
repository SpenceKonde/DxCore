# Pin interrupts
While the usual attachInterrupt() functionality is provided by megaTinyCore (and works on every pin, with every type of trigger), these will take slightly longer to run, and use more flash, than an equivalent interrupt implemented manually (due to the need to check for all 8 pins - while a manually implemented scheme would know that only the pins configured to generate interrupts need to be checked; that takes both time and flash space). Additionally, there are common use cases (for example, reading rotary encoders, particularly more than one) where each pin being handled separately prevents convenient shortcuts from being taken. For these reasons, it is often desirable or necessary to manually implement a pin interrupt.

## Manually implementing pin interrupts
The system for interrupts on megaavr parts is different from, and vastly more powerful than that of the classic AVR parts. Unlike classic AVR parts, there are no special interrupt pins (INT0, INT1, etc.) - instead, all pins can be configured to generate an interrupt on change, rising, falling or LOW level. While all pins on a port are handled by the same ISR (like classic AVR PCINT's), the pin that triggered the interrupt is recorded in the INTFLAGS register, making it easy to determine which pin triggered the interrupt.

### Enabling the interrupt
The pin interrupt control is handled by the PORTx.PINnCTRL register.
Bits 0-2 control interrupt and sense behavior:
000 = no interrupt, normal operation
001 = interrupt on change
010 = interrupt on rising
011 = interrupt on falling
100 = digital input buffer disabled entirely (equivalent of DIDR register on classic AVRs that have it)
101 = interrupt on LOW level

Bit 3 controls the pullup.

Bit 3 is set when pinMode() is used to set the pin to INPUT_PULLUP. When manually writing the PINnCTRL registers, be sure to either use bitwise operators to preserve this bit, or set it to the correct value.

### The ISR
Each port has one interrupt vector; their names are:

    PORTA_PORT_vect
    PORTB_PORT_vect
    PORTC_PORT_vect

When the interrupt condition occurs, the bit int PORTx.INTFLAGS corresponding to the interrupt will be set. If multiple pins in a port are used for interrupts corresponding to different things, you can use this to determine which pin triggered the interrupt. **YOU MUST CLEAR THIS BIT WITHIN THE ISR** - the interrupt will continue to be generated as long as the flag is set, so if you do not unset it, the ISR will run continuously after it was triggered once. To clear the bit, write a 1 to it; writing 255 to it will clear all of them.

### A basic example for the x16/x06

```cpp
unsigned long previousMillis;
byte ledState;
volatile byte interrupt1;
volatile byte interrupt2;
volatile byte interrupt3;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_PC2,INPUT_PULLUP); //PC2
  pinMode(PIN_PA1,INPUT_PULLUP); //PA1
  pinMode(PIN_PA2,INPUT_PULLUP); //PA2
  pinMode(PIN_PB0,INPUT_PULLUP); //PB0
  PORTC.PIN2CTRL=0b00001101; //PULLUPEN=1, ISC=5 trigger low level
  PORTA.PIN1CTRL=0b00001010; //PULLUPEN=1, ISC=2 trigger rising
  PORTA.PIN2CTRL|=0x02; //ISC=2 trigger rising - uses |= so current value of
  PORTB.PIN0CTRL=0b00001001; //PULLUPEN=1, ISC=1 trigger both
  Serial.begin(115200);
  delay(10);
  Serial.println("Startup");

}

void loop() {
  if (interrupt1){
    interrupt1=0;
    Serial.println("I1 fired");
  }
  if (interrupt2){
    interrupt2=0;
    Serial.println("I2 fired");
  }
  if (interrupt3){
    interrupt3=0;
    Serial.println("I3 fired");
  }
  //BlinkWithoutDelay, just so you can confirm that the sketch continues to run.
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    digitalWrite(LED_BUILTIN, ledState);
  }
}

ISR(PORTA_PORT_vect) {
  byte flags=PORTA.INTFLAGS;
  PORTA.INTFLAGS=flags; //clear flags
  if (flags&0x02) {
    interrupt1=1;
  }
  if (flags&0x04) {
    interrupt2=1;
  }
}

ISR(PORTB_PORT_vect) {
  PORTB.INTFLAGS=1; //we know only PB0 has an interrupt, so that's the only flag that could be set.
  interrupt3=1;
}

ISR(PORTC_PORT_vect) {
  _PROTECTED_WRITE(RSTCTRL.SWRR,1); //virtual reset
}
```

### Synchronous and Asynchronous pins
Certain pins (pin 2 and 6 in each port) are "fully asynchronous" - These pins have several special properties:
* They can be triggered by conditions which last less than one processor cycle.
* They can wake the system from sleep on change, rising, falling or level interrupt. Other pins can only wake on change or level interrupt.
* There is no "dead time" between successive interrupts (other pins have a 3 clock cycle "dead time" between successive interrupts)

In the example above, note that the interrupts on pin 14 and 15 (PA1 and PA2) are configured identically. However, if the part was put to sleep, only the one on pin PA2 would be able to wake the part, as they trigger on rising edge, and only PA2 is a fully asynchronous pin.

### Interrupt response time
The datasheet reports the interrupt response time as 6 clock cycles (1 to finish current instruction, 2 to save PC for return, and 3 to execute the jmp to the ISR). That is, like any vendor spec, technically accurate, but paints an overly optimistic picture, because you don't care about when execution reaches the start of the ISR(), you care when it starts executing the code you wrote in the ISR (right?). But before that happens, the prologue has to execute. The prologue is responsible for saving the state of the status register and working registers that your ISR modifies; so that they can be restored in the epilogue after the ISR is done and the function that was interrupted can continue and produce expected results. All ISRs written in C will push r0 and r1 onto the stack, load sreg into r0 and push that, and then clear r1 so it can act as `__zero_reg` as gcc requires. That's 5 more clocks (and was 8 on classic AVRs - push is single cycle now, just like st - push and pop are almost identical to st X+ and ld -X, only using the stack pointer instead of the X register). So you're up to a minimum of 11 cycles - and that only gets you one register, r0. For every byte of local storage you need (local variables, and I think potentially intermediate values, depending on complexity of the code) that's going to cost you a longer prologue. The serial transmit ISR in DxCore needs another 12 bytes, so the prologue takes 17 clocks, and it then calls (3 clocks) another routine, which pushes 2 more registers in it's prologue - so it's 27 clocks from when the tx data register is empty to when the ISR starts running! For every register pushed in the prologue, it needs to be popped back in epilogue at the end of the ISR; pop takes 2 clocks, and the reti instruction itself takes another 4. So at minimum, it's 12 clocks between when the last of the code you wrote in the isr finishes, and whatever it interrupted resumes.

In the above example of the serial TX interrupt, there are the 2 registers of the interior function, and it's ret, then 12 pops to restore the 12 pushes, then the 8 clocks of boiler plate regarding sreg and r0/r1, then reti: 8 + 24 + 8 + 4 = 44 clocks. The total overhead overhead is a whopping 71 clock cycles there! This is actually longer than the interrupt itself, which runs for around 35 clock cycles. At low F_CPU and high baud rate, the sum of these, 136, is *longer than it takes to transmit that character* (the point where this manifests is close to 57600 baud at 1 MHz - proportionally higher at faster clock speeds, and so rarely a problem if not running at unusually low system clock speed) - though there are significant complicating factors, and in some cases the buffer doesn't actually get used at all. It may be possible to improve this (unlike some parts of the Serial library, this has not been rewritten for DxCore) in a future update, but the code is rather intimidating. The RX interrupt is considerably faster and doesn't suffer from this problem (if it did, it would miss characters!)
