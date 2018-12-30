/*
VisqueSmart Lens Switch Test Project - 2018.03.09
*/

// ------- Preamble -------- //
#include <avr/io.h>                        /* Defines pins, ports, etc */
#include <util/delay.h>                     /* Functions to waste time */
#include <avr/power.h>
#include <avr/interrupt.h>
#include "portpins.h"
#include "pinDefines.h"

#define DIRECTION               PB4
#define MOTOR_CLOCK1            PD6
#define DEBOUNCE_TIME		100

#define MAX_DELAY    255               /* determines min startup speed */
#define MIN_DELAY     30		/* determines max cruise speed */
#define ACCELERATION     1		
#define LUI     20		


volatile uint16_t stepCounter= 0;
uint16_t stepsTaken = 0; 

static inline void initTimer0(void) {
  TCCR0A |= (1 << WGM01);                                  /* CTC mode */
  TCCR0A |= (1 << COM0A0);           /* Toggles pin each cycle through */
  //TCCR0B |= (1 << CS00) | (1 << CS01);               /* CPU clock / 64 */
  TCCR0B |= (1 << CS01);				/* CPU clock / 8 */
  //TCCR0B |= (1 << CS00);				/* CPU clock / 1 */
  OCR0A = MAX_DELAY; 
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


uint8_t debounce_1(void) {
	if (bit_is_clear(PINC,PC5)) {
		_delay_us(DEBOUNCE_TIME);
		if (bit_is_clear(PINC,PC5)) {
			return (1);
		}
	}
	return (0);
} 


int main(void) {
  // -------- Inits --------- //

  DDRD &= ~(1 << MOTOR_CLOCK1); 		//DISABLE CLOCK TO L297

  initTimer0();
		
  PORTC |= (1 << PC5);				//enable pull-up resister
  DDRB  |= (1 << PB4);           		/* enable output*/
  
//  uint8_t delay_2 = MIN_DELAY;

  uint8_t delay_2 = 50;


  sei();

  // ------ Event loop ------ //
  while (1) {
    		
 	PORTB |= (1 << DIRECTION);

    while (stepsTaken < LUI){    
	    delay_2 -= ACCELERATION;
	    takeSteps1(1, delay_2);
	    stepsTaken++;
    }
    
	takeSteps1(10000, MIN_DELAY); 
//    _delay_ms(2000); 	         

///*
    stepsTaken = 0;

    while (stepsTaken < LUI){    
	    delay_2 += ACCELERATION;
	    takeSteps1(1, delay_2);
	    stepsTaken++;
 	}
 	
 	_delay_ms(1000); 	         
    
 
    stepsTaken = 0;
    
    while (stepsTaken < LUI){    
	    delay_2 -= ACCELERATION;
	    takeSteps1(1, delay_2);
	    stepsTaken++;
 	}
//*/

	takeSteps1(10000, MIN_DELAY); 
//    _delay_ms(2000); 	         

///*
    stepsTaken = 0;

    while (stepsTaken < LUI){    
	    delay_2 += ACCELERATION;
	    takeSteps1(1, delay_2);
	    stepsTaken++;
 	}
 	
 	_delay_ms(1000); 	         

 	PORTB &= ~(1 << DIRECTION);

    stepsTaken = 0;


    while (stepsTaken < LUI){    
	    delay_2 -= ACCELERATION;
	    takeSteps1(1, delay_2);
	    stepsTaken++;
    }

	takeSteps1(10000, MIN_DELAY); 
//    _delay_ms(2000); 	         

///*
    stepsTaken = 0;

    while (stepsTaken < LUI){    
	    delay_2 += ACCELERATION;
	    takeSteps1(1, delay_2);
	    stepsTaken++;
 	}
 	
 	_delay_ms(1000); 	         
    
 
    stepsTaken = 0;

    
    while (stepsTaken < LUI){    
	    delay_2 -= ACCELERATION;
	    takeSteps1(1, delay_2);
	    stepsTaken++;
 	}
//*/

	takeSteps1(10000, MIN_DELAY); 

///*
    stepsTaken = 0;

    while (stepsTaken < LUI){    
	    delay_2 += ACCELERATION;
	    takeSteps1(1, delay_2);
	    stepsTaken++;
 	}

 	_delay_ms(1000); 	         
    
  } 	                                       /* End event loop */
  return (0);                            /* This line is never reached */
}
