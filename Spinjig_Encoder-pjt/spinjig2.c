/*
Golf Ball Spinning Jig using Encoder Project - 2016.11.13
*/

// ------- Preamble -------- //
#include <avr/io.h>                        /* Defines pins, ports, etc */
#include <util/delay.h>                     /* Functions to waste time */
#include <avr/power.h>
#include <avr/interrupt.h>
#include "pinDefines.h"
#include "USART.h"

#define CPT 500

volatile uint8_t direction = 0;
volatile uint16_t count = 0;
volatile float velocity_in_rpm = 0;

/*static inline void initTimer0(void) {

TCCR0A |= (1 << WGM00);				// Set timer/counter 0 waveform generation mode to fast pwm.
TCCR0A |= (1 << WGM01);

TCCR0A |= (1 << COM0A0);			// Set OC0A on compare match, clear OC0A at BOTTOM.
TCCR0A |= (1 << COM0A1);

TCCR0B |= (1 << CS00);				// Set timer/counter 0 clock source.(No prescaling)
TCCR0B &= ~(1 << CS01);
TCCR0B &= ~(1 << CS02);
}*/


static inline void initEncoder(void){    
DDRD |= ~(1 << PD3);				// encoder B input. 
DDRC |= ~(1 << PC0);				// encoder A input.
PORTD |= (1 << PD3);				// encoder B pullup resistor enabled.
PORTC |= (1 << PC0);				// encoder A pullup resistor enabled.
//EICRA |= (1 << ISC10);				// External interrupt control register A: Any logical change on INT1 generates an interrupt request.
EICRA |= (1 << ISC11)|(1 << ISC10);// External interrupt control register A: every rising edge on INT1 generates an interrupt request.
//EICRA |= (1 << ISC11);// External interrupt control register A: every falling edge on INT1 generates an interrupt request.
EIMSK |= (1 << INT1);				// Enable the external interrupt.
}


/*ISR(INT1_vect){
	// Determine direction and update encoder count from the logic levels of the encoder's A and B outputs.

    if(bit_is_set(PIND,PD3)){ 		

		if(bit_is_set(PINC,PC0)){ 		
			direction = 1;
			count ++;
		}
		else{
			direction = 0;
			count --;
		}
	}

    else{ 		

		if(bit_is_set(PINC,PC0)){ 		
			direction = 0;
			count --;
		}
		else{
			direction = 1;
			count ++;
		}
	}

}*/



ISR(INT1_vect){
	// Determine direction and update encoder count from the logic levels of the encoder's A and B outputs.

    if(bit_is_set(PIND,PD3)){ 		

		if(bit_is_set(PINC,PC0)){ 		
			direction = 1;
			count ++;
		}
		else{
			direction = 0;
			count --;
		}
	}

}


/*uint8_t debounce(void) {
	if (bit_is_set(PIND,PD3)) {
		_delay_us(5);
		if (bit_is_set(PIND,PD3)) {
			return (1);
		}
	}
	return (0);
} 


ISR(INT1_vect){
	// Determine direction and update encoder count from the logic levels of the encoder's A and B outputs.
    if (debounce()) {
		if(bit_is_set(PINC,PC0)){ 		
			direction = 1;
			count ++;
		}
		else{
			direction = 0;
			count --;
		}
    }
}*/


static inline void printVelocity(float number){
    //Converts a two_byte to a string fo decimal text, sends it
    uint16_t new_number = round(number);
    transmitByte('0' + (new_number/10000));
    transmitByte('0' + ((new_number/1000) % 10));
    transmitByte('0' + ((new_number/100) % 10));
    transmitByte('0' + ((new_number/10) % 10));
    transmitByte('0' + (new_number % 10));    
    printString("\r\n");
}


int main(void) {
  // -------- Inits --------- //
  clock_prescale_set(clock_div_1);                  /* CPU clock 8 MHz */

  initUSART();
  printString("\r\nDisplay DC Motor Speed\r\n\r\n");
  initEncoder(); 
		
 
  // ------ Event loop ------ //
  while (1) {

    count = 0;
    sei();
    _delay_ms(10);
    cli();
    velocity_in_rpm = ((float)count / CPT) * 100 * 60;
    printVelocity(velocity_in_rpm);
	_delay_ms(300);
	
  }	
 return (0);                            /* This line is never reached */
}
