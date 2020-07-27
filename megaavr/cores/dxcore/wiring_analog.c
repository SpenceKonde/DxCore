/*
  wiring_analog.c - analog input and output
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  Modified 28 September 2010 by Mark Sproul
*/

#include "wiring_private.h"
#include "pins_arduino.h"
#include "Arduino.h"

void analogReference(uint8_t mode)
{
	if (mode < 7 && mode !=4) {
		VREF.ADC0REF=mode;
	}
}
void DACReference(uint8_t mode)
{
	if (mode < 7 && mode !=4) {
		VREF.DAC0REF=mode;
	}
}

int analogRead(uint8_t pin)
{
	if (pin!=ADC_DAC0 && pin != ADC_TEMPERATURE)
	{
		pin = digitalPinToAnalogInput(pin);
		if(pin == NOT_A_PIN) return -1;
	}
#if defined(ADC0)
	/* Reference should be already set up */
	/* Select channel */
	ADC0.MUXPOS = (pin << ADC_MUXPOS_gp);

	/* Start conversion */
	ADC0.COMMAND = ADC_STCONV_bm;

	/* Wait for result ready */
	while(!(ADC0.INTFLAGS & ADC_RESRDY_bm));

	/* Combine two bytes */
	return ADC0.RES;

#else	/* No ADC, return 0 */
	return 0;
#endif

}

// Right now, PWM output only works on the pins with
// hardware support.  These are defined in the appropriate
// pins_*.c file.  For the rest of the pins, we default
// to digital output.
void analogWrite(uint8_t pin, int val)
{
	uint8_t bit_pos  = digitalPinToBitPosition(pin);
	if(bit_pos == NOT_A_PIN) return;
	// We need to make sure the PWM output is enabled for those pins
	// that support it, as we turn it off when digitally reading or
	// writing with them.  Also, make sure the pin is in output mode
	// for consistently with Wiring, which doesn't require a pinMode
	// call for the analog output pins.
	pinMode(pin, OUTPUT);

	//if(val < 1){	/* if zero or negative drive digital low */

	//	digitalWrite(pin, LOW);

	//} else if(val > 255){	/* if max or greater drive digital high */

	//	digitalWrite(pin, HIGH);

	//} else {	/* handle pwm to generate analog value */
	/* Get timer */
	uint8_t digital_pin_timer =  digitalPinToTimer(pin);
	uint8_t* timer_cmp_out;

	TCB_t *timer_B;
	//TCA_t *timer_A;
	/* Find out Port and Pin to correctly handle port mux, and timer. */
	switch (digital_pin_timer) { //use only low nybble which defines which timer it is

		case TIMERA0:
			if(val <= 0){	/* if zero or negative drive digital low */
				digitalWrite(pin, LOW);
			} else if(val >= 255){	/* if max or greater drive digital high */
				digitalWrite(pin, HIGH);
			} else {

				/* Calculate correct compare buffer register */
				if (bit_pos>2) {
					bit_pos-=3;
					timer_cmp_out = ((uint8_t*) (&TCA0.SPLIT.HCMP0)) + (bit_pos<<1);
					(*timer_cmp_out) = (val);
					TCA0.SPLIT.CTRLB |= (1 << (TCA_SPLIT_HCMP0EN_bp + bit_pos));
				} else {
					timer_cmp_out = ((uint8_t*) (&TCA0.SPLIT.LCMP0)) + (bit_pos<<1);
					(*timer_cmp_out) = (val);
					TCA0.SPLIT.CTRLB |= (1 << (TCA_SPLIT_LCMP0EN_bp + bit_pos));
				}
			}
			break;
		#ifdef TCA1
		case TIMERA1:
			if(val <= 0){	/* if zero or negative drive digital low */
				digitalWrite(pin, LOW);
			} else if(val >= 255){	/* if max or greater drive digital high */
				digitalWrite(pin, HIGH);
			} else {

				/* Calculate correct compare buffer register */
				if (bit_pos>2) {
					bit_pos-=3;
					timer_cmp_out = ((uint8_t*) (&TCA1.SPLIT.HCMP0)) + (bit_pos<<1);
					(*timer_cmp_out) = (val);
					TCA1.SPLIT.CTRLB |= (1 << (TCA_SPLIT_HCMP0EN_bp + bit_pos));
				} else {
					timer_cmp_out = ((uint8_t*) (&TCA1.SPLIT.LCMP0)) + (bit_pos<<1);
					(*timer_cmp_out) = (val);
					TCA1.SPLIT.CTRLB |= (1 << (TCA_SPLIT_LCMP0EN_bp + bit_pos));
				}
			}
			break;
			#endif
      case TIMERB0:
      case TIMERB1:
      case TIMERB2:
      case TIMERB3:
      case TIMERB4:

        /* Get pointer to timer, TIMERB0 order definition in Arduino.h*/
        //assert (((TIMERB0 - TIMERB3) == 2));
        timer_B = ((TCB_t *)&TCB0 + (digital_pin_timer - TIMERB0));

        /* set duty cycle */
        timer_B->CCMPH = val;

        /* Enable Timer Output */
        timer_B->CTRLB |= (TCB_CCMPEN_bm);

        break;

        /* If non timer pin, or unknown timer definition. */
        /* do a digital write */

		#if defined(DAC0)
		case DACOUT:
			#ifdef DAC0_DATAH
				DAC0.DATAH=val;
			#else
				DAC0.DATA=val;
			#endif
			DAC0.CTRLA=0x41; //OUTEN=1, ENABLE=1
			break;
		#endif
	    #if (defined(TCD0) && defined(USE_TIMERD0_PWM))
		case TIMERD0:
			if(val < 1){	/* if zero or negative drive digital low */
				digitalWrite(pin, LOW);
			} else if(val > 254){	/* if max or greater drive digital high */
				digitalWrite(pin, HIGH);
			} else {
			    if (bit_pos) {
			    	TCD0.CMPBSET=(255-val)<<1;
			    } else {
			    	TCD0.CMPASET=(255-val)<<1;
			    }
				if (!(TCD0.FAULTCTRL & (1<<(6+bit_pos)))) { //bitpos will be 0 or 1 for TIMERD pins
					//if not active, we need to activate it, which produces a glitch in the PWM
					TCD0.CTRLA=TIMERD0_PRESCALER;//stop the timer
					while(!(TCD0.STATUS&0x01)) {;} // wait until it's actually stopped
					uint8_t sreg=SREG;
					cli();
					_PROTECTED_WRITE(TCD0.FAULTCTRL,TCD0.FAULTCTRL|(1<<(6+bit_pos)));
					SREG=sreg;
					TCD0.CTRLA=TIMERD0_PRESCALER|1; //reenable it
				} else {
					while(!(TCD0.STATUS&0x02)) {;} //if previous sync in progress, wait for it to finish.
					TCD0.CTRLE=0x02; //Synchronize
				}
			}
			break;

		#endif

		/* If non timer pin, or unknown timer definition.	*/
		/* do a digital write	*/
		case NOT_ON_TIMER:
		default:
			if (val < 128) {
				digitalWrite(pin, LOW);
			} else {
				digitalWrite(pin, HIGH);
			}
			break;
	}
}
