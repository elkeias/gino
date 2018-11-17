/*
G4 Golf ball spin measurement project 2016~2017
*/

// ------- Preamble -------- //
#include <avr/io.h>                        /* Defines pins, ports, etc */
#include <util/delay.h>                     /* Functions to waste time */
#include <avr/power.h>
#include <avr/interrupt.h>
#include "pinDefines.h"
#include "USART.h"

#define CPT 512

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



static inline void initTimer1(void) {
  TCCR1B |= (1 << CS10);		     // No prescaling
}


static inline void initEncoder(void){    
DDRD |= ~(1 << PD3);				// encoder B input. 
DDRC |= ~(1 << PC0);				// encoder A input.
PORTD |= (1 << PD3);				// encoder B pullup resistor enabled.
PORTC |= (1 << PC0);				// encoder A pullup resistor enabled.
EICRA |= (1 << ISC10);				// External interrupt control register A: Any logical change on INT1 generates an interrupt request.
//EICRA |= (1 << ISC11)|(1 << ISC10);// External interrupt control register A: every rising edge on INT1 generates an interrupt request.
//EICRA |= (1 << ISC11);// External interrupt control register A: every falling edge on INT1 generates an interrupt request.
EIMSK |= (1 << INT1);				// Enable the external interrupt.
}


ISR(INT1_vect){
	// Determine direction and update encoder count from the logic levels of the encoder's A and B outputs.
    if (TCNT1 > 38){
        count = TCNT1;
    // 47 count per half cycle = (1s / (5000rpm/(60s/min) * 2 * CPT) / 0.00000025s
    }
    TCNT1 = 0;
}



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
  clock_prescale_set(clock_div_2);                  /* CPU clock 8 MHz */

  initUSART();
  initTimer1();

  printString("\r\nDisplay DC Motor Speed\r\n\r\n");
  initEncoder(); 
		
 
  // ------ Event loop ------ //
  while (1) {

//    count = 0;
    TCNT1 = 0;
    sei();
    _delay_ms(10);
    cli();
    velocity_in_rpm = (float) 60 / (2* count * 0.00000025 * CPT);
    printVelocity(velocity_in_rpm);    
    printVelocity(count);
    printString("==========\r\n\r\n");
	_delay_ms(400);
	
  }	
 return (0);                            /* This line is never reached */
}
