/*
Wedge Measuring Instrument Project - 2015.12.7 - 09:00
*/

이제 한글을 쓸 수 있다.

// ------- Preamble -------- //
#include <avr/io.h>                        /* Defines pins, ports, etc */
#include <util/delay.h>                     /* Functions to waste time */
#include <avr/power.h>
#include <avr/interrupt.h>
#include "pinDefines.h"

#define DIRECTION               PB4
#define MOTOR_CLOCK1            PD6
#define MOTOR_CLOCK2            PB3
#define DEBOUNCE_TIME		100
#define ONE_STEP		1
#define TURN			400

#define MAX_DELAY    255               /* determines min startup speed */
#define MIN_DELAY     35		/* determines max cruise speed */

volatile uint8_t stepCounter= 0;

static inline void initTimer0(void) {
  TCCR0A |= (1 << WGM01);                                  /* CTC mode */
  TCCR0A |= (1 << COM0A0);           /* Toggles pin each cycle through */
  //TCCR0B |= (1 << CS00) | (1 << CS01);               /* CPU clock / 64 */
  TCCR0B |= (1 << CS01);				/* CPU clock / 8 */
  OCR0A = MAX_DELAY; 
}

static inline void initTimer1(void) {
  TCCR1A |= (1 << WGM12);                             /* Fast PWM mode */
  TCCR1A |= (1 << WGM10);                        /* Fast PWM mode, pt.2 */
  TCCR1A |= (1 << COM1B1);                       /* output PWM to pin */
  TCCR1A |= (1 << COM1A1) | (1 << COM1B1);       /* output PWM to pin */
  //TCCR1B |= (1 << CS11) | (1 << CS10);		//Clock with /64 prescaler, PWM frequency 61Hz
  TCCR1B |= (1 << CS11);			//Clock with /8 prescaler, PWM frequency 488Hz
}

static inline void initTimer2(void) {
  TCCR2A |= (1 << WGM21);                                  /* CTC mode */
  TCCR2A |= (1 << COM2A0);           /* Toggles pin each cycle through */
  //TCCR2B |= (1 << CS20) | (1 << CS21);               /* CPU clock / 64 */
  TCCR2B |= (1 << CS21);				/* CPU clock / 8 */
  OCR2A = MAX_DELAY; 
}

void initPinChangeInterrupt(void) {

	PCICR |= (1 << PCIE1);			// enable Pin-change interrupts 1 (bank C)
	PCMSK1 |= (1 << PC5);			// enable specific interrupt for ourpin PC5

}

ISR(PCINT1_vect) {
  PORTB ^= (1 << DIRECTION);
  DDRD |= (1 << MOTOR_CLOCK1);
  OCR0A = 125;
  DDRB |= (1 << MOTOR_CLOCK2);
  OCR2A = 125;  			
  _delay_ms(500);
  PCIFR |= (1 << PCIF1);       			                    
}



void takeSteps1(uint16_t howManySteps, uint8_t delay) {
  OCR0A = delay;                  /* delay in counter compare register */

  TIMSK0 |= (1 << OCIE0A);             /* turn on interrupts, stepping */
  DDRD |= (1 << MOTOR_CLOCK1);	
  stepCounter = 0;            /* initialize to zero steps taken so far */
  while (!(stepCounter == howManySteps)) {
  }                                                            /* wait */
  DDRD &= ~(1 << MOTOR_CLOCK1); 
  TIMSK0 &= ~(1 << OCIE0A);                          /* turn back off */  
 }

ISR(TIMER0_COMPA_vect) {
  stepCounter++;                                   /* count step taken */
}

void takeSteps2(uint16_t howManySteps, uint8_t delay) {
  OCR2A = delay;                  /* delay in counter compare register */

  TIMSK2 |= (1 << OCIE2A);             /* turn on interrupts, stepping */
  DDRB |= (1 << MOTOR_CLOCK2);	
  stepCounter = 0;            /* initialize to zero steps taken so far */
  while (!(stepCounter == howManySteps)) {
  }                                                            /* wait */
  DDRB &= ~(1 << MOTOR_CLOCK2); 
  TIMSK2 &= ~(1 << OCIE2A);                          /* turn back off */  
 }

ISR(TIMER2_COMPA_vect) {
  stepCounter++;                                   /* count step taken */
}

uint8_t debounce_1(void) {
	if (bit_is_clear(PIND,PD0)) {
		_delay_us(DEBOUNCE_TIME);
		if (bit_is_clear(PIND,PD0)) {
			return (1);
		}
	}
	return (0);
} 

uint8_t debounce_2(void) {
	if (bit_is_clear(PIND,PD1)) {
		_delay_us(DEBOUNCE_TIME);
		if (bit_is_clear(PIND,PD1)) {
			return (1);
		}
	}
	return (0);
} 

uint8_t debounce_3(void) {
	if (bit_is_clear(PIND,PD2)) {
		_delay_us(DEBOUNCE_TIME);
		if (bit_is_clear(PIND,PD2)) {
			return (1);
		}
	}
	return (0);
} 

uint8_t debounce_4(void) {
	if (bit_is_clear(PIND,PD3)) {
		_delay_us(DEBOUNCE_TIME);
		if (bit_is_clear(PIND,PD3)) {
			return (1);
		}
	}
	return (0);
} 

uint8_t debounce_5(void) {
	if (bit_is_clear(PIND,PD4)) {
		_delay_us(DEBOUNCE_TIME);
		if (bit_is_clear(PIND,PD4)) {
			return (1);
		}
	}
	return (0);
} 

uint8_t debounce_6(void) {
	if (bit_is_clear(PIND,PD7)) {
		_delay_us(DEBOUNCE_TIME);
		if (bit_is_clear(PIND,PD7)) {
			return (1);
		}
	}
	return (0);
}

uint8_t readADC(uint8_t channel) {
  ADMUX = (0xf0 & ADMUX) | channel;
  ADCSRA |= (1 << ADSC);
  loop_until_bit_is_clear(ADCSRA, ADSC);
  return (ADCH);
}


int main(void) {
  // -------- Inits --------- //
  //clock_prescale_set(clock_div_1);                  /* CPU clock 8 MHz */

  initTimer0();
  initTimer1();
  initTimer2();
		
  PORTC |= (1 << PC5);				//enable pull-up resister
  PORTD |= (1 << PD0)|(1 << PD1)|(1 << PD2)|(1 << PD3)|(1 << PD4)|(1 << PD7);	//initialize pullup resistor on the input pin 
  DDRB  |= (1 << PB1) | (1 << PB2) | (1 << PB4);           		/* enable output*/


 //Top Collimator Up to Prevent Crushing with Limit Switch
  PORTB |= ~(1 << DIRECTION);	
  DDRD |= (1 << MOTOR_CLOCK1);
  OCR0A = 125;  			
  _delay_ms(1500);
  DDRD |= ~(1 << MOTOR_CLOCK1);

 //Measuring Lens Up to Prevent Crushing with Limit Switch
  PORTB |= (1 << DIRECTION);	
  DDRB |= (1 << MOTOR_CLOCK2);
  OCR2A = 125;  			
  _delay_ms(500);
  DDRB |= ~(1 << MOTOR_CLOCK2);



  sei();
  initPinChangeInterrupt();

  uint8_t action1 = 0b00000000;
  uint8_t action2 = 0b00000000;

  // Set up ADC
  ADMUX |= (1 << REFS0);                  /* reference voltage on AVCC */
  ADCSRA |= (1 << ADPS1) | (1 << ADPS0);     /* ADC clock prescaler /8 */
  ADMUX |= (1 << ADLAR);			//Left-adjust result, return only 8bits  
  ADCSRA |= (1 << ADEN);                                 /* enable ADC */



  // ------ Event loop ------ //
  while (1) {

	uint8_t speed=MAX_DELAY;

	OCR1A = readADC(PC0);
	OCR1B = readADC(PC1);

  	DDRD &= ~(1 << MOTOR_CLOCK1); 		//DISABLE CLOCK TO L297
  	DDRB &= ~(1 << MOTOR_CLOCK2); 		//DISABLE CLOCK TO L297

	if (debounce_3()) {		
		action1 ^= (1 << 0);
		_delay_ms(1000);
		}
	
 
	if (debounce_6()) {		
		action2 ^= (1 << 0);
		_delay_ms(1000);
	}

	switch(action1)	
	{

	case 0:		
		
		

		if (debounce_2()) {				//Top Collimator UP
			PORTB &= ~(1 << DIRECTION);
			DDRD |= (1 << MOTOR_CLOCK1);	
			while(bit_is_clear(PIND,PD1)) {	

				if (speed > 35) {
				OCR0A = speed;	
				speed--;
				_delay_ms(15);
				}							
			}
		}
		
		if (debounce_1()) {				//Top Collimator Down
  			PORTB |= (1 << DIRECTION);
			DDRD |= (1 << MOTOR_CLOCK1);	
			while(bit_is_clear(PIND,PD0)) {	

				if (speed > 35) {
				OCR0A = speed;	
				speed--;
				_delay_ms(5);
				}							
			}
		}

		break;

	case 1:		

		if (debounce_1()) {
			
			while(bit_is_clear(PIND,PD0)) {	
  				PORTB |= (1 << DIRECTION);
				takeSteps1(1, MAX_DELAY);
 				_delay_ms(100);
			}
		}

		if (debounce_2()) {

			while(bit_is_clear(PIND,PD1)) {	
				PORTB &= ~(1 << DIRECTION);
				takeSteps1(1, MAX_DELAY);
 				_delay_ms(100);
			}
		}
		
		break;
	}
	


	switch(action2)	
	{

	case 0:		
		
		if (debounce_4()) {				//Measuring Lens UP
  			PORTB |= (1 << DIRECTION);
			DDRB |= (1 << MOTOR_CLOCK2);	
			while(bit_is_clear(PIND,PD3)) {	

				if (speed > 35) {
				OCR2A = speed;	
				speed--;
				_delay_ms(10);
				}							
			}

		}

		if (debounce_5()) {				//Measuring Lens DOWN
			PORTB &= ~(1 << DIRECTION);
			DDRB |= (1 << MOTOR_CLOCK2);	
			while(bit_is_clear(PIND,PD4)) {	

				if (speed > 35) {
				OCR2A = speed;	
				speed--;
				_delay_ms(5);
				}							
			}
		
}
		
		break;

	case 1:		

		if (debounce_4()) {
			
			while(bit_is_clear(PIND,PD3)) {	
  				PORTB |= (1 << DIRECTION);
				takeSteps2(1, MAX_DELAY);
 				_delay_ms(100);
			}
		}

		if (debounce_5()) {

			while(bit_is_clear(PIND,PD4)) {	
				PORTB &= ~(1 << DIRECTION);
				takeSteps2(1, MAX_DELAY);
 				_delay_ms(100);
			}
		}
	break;
	}
    } 	                                       /* End event loop */


  return (0);                            /* This line is never reached */
}
