/*
Lens Absorption Project - 2015.12.7 - 09:00
*/

// ------- Preamble -------- //
#include <avr/io.h>                        /* Defines pins, ports, etc */
#include <util/delay.h>                     /* Functions to waste time */
#include "pinDefines.h"

#define SOLENOID                  PB3
#define MOTOR                     PB4
#define DEBOUNCE_TIME		       100


uint8_t debounce_clear(void) {
	if (bit_is_clear(PINB,PB0)) {
		_delay_us(DEBOUNCE_TIME);
		if (bit_is_clear(PINB,PB0)) {
			return (1);
		}
	}
	return (0);
}

uint8_t debounce_set(void) {
	if (bit_is_set(PINB,PB0)) {
		_delay_us(DEBOUNCE_TIME);
		if (bit_is_set(PINB,PB0)) {
			return (1);
		}
	}
	return (0);
}

 
int main(void) {
  // -------- Inits --------- //

  uint8_t state = 0;

  PORTB |= (1 << PB0);				//enable pull-up resister 
  DDRB  |= (1 << PB3) | (1 << PB4);           		/* enable output*/
  
  // ------ Event loop ------ //
  
  while (1) {

  PORTB |= (1 << SOLENOID);
  PORTB |= (1 << MOTOR);

    if (debounce_clear() && (state == 0)) {
        PORTB &= ~(1 << MOTOR);        
        _delay_ms(5000);
        state = 1;
	    } 	                                       
	    
	if (debounce_set() && (state == 1)) {
        PORTB |= (1 << MOTOR);
        PORTB &= ~(1 << SOLENOID);
        
        _delay_ms(1000);
        state = 0;
	    } 	                                       
   }                                     /* End event loop */

  return (0);                            /* This line is never reached */
}
