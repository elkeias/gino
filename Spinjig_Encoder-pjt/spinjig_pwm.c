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
volatile uint16_t velocity = 0;

static inline void initTimer0(void) {

TCCR0A |= (1 << WGM00);				// Set timer/counter 0 waveform generation mode to fast pwm.
TCCR0A |= (1 << WGM01);

TCCR0A |= (1 << COM0A1);            // Clear OC0A on compare match, Set OC0A at BOTTOM.

TCCR0B |= (1 << CS00)|(1 << CS01);				// Set timer/counter 1/64 prescaler. PWM frequencty = 244Hz
}



static inline void initTimer1(void) {
  TCCR1B |= (1 << CS10);		     // No prescaling
}


static inline void initEncoder(void){    
DDRD |= ~(1 << PD3);				// encoder B input. 
DDRC |= ~(1 << PC0);				// encoder A input.
PORTD |= (1 << PD3);				// encoder B pullup resistor enabled.
PORTC |= (1 << PC0);				// encoder A pullup resistor enabled.
//EICRA |= (1 << ISC10);				// External interrupt control register A: Any logical change on INT1 generates an interrupt request.
EICRA |= (1 << ISC11)|(1 << ISC10);// External interrupt control register A: every rising edge on INT1 generates an interrupt request.
EIMSK |= (1 << INT1);				// Enable the external interrupt.
}


ISR(INT1_vect){
	// Determine direction and update encoder count from the logic levels of the encoder's A and B outputs.
    if (TCNT1 > 40){
        count = TCNT1;
    // XX count per cycle = (1s / (10000rpm/(60s/min) * 2 * CPT))
    // 93.7 count per cycle = (1s / (5000rpm/(60s/min) * CPT)) / 0.00000025s

    }
    TCNT1 = 0;
}


static inline void calVelocity(void){    
    TCNT1 = 0;
    sei();
    _delay_ms(10);
    cli();
    velocity = (float) 60 / (count * 0.00000025 * CPT);   //velociy calculated in rpm
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


static inline uint16_t getNumber16(void) {
  // Gets a PWM value from the serial port.
  // Reads in characters, turns them into a number
  char thousands = '0';
  char hundreds = '0';
  char tens = '0';
  char ones = '0';
  char thisChar = '0';

  do {
    thousands = hundreds;                        /* shift numbers over */
    hundreds = tens;
    tens = ones;
    ones = thisChar;
    thisChar = receiveByte();                   /* get a new character */
    transmitByte(thisChar);                                    /* echo */
  } while (thisChar != '\r');

  transmitByte('\n');                                       /* newline */
  return (1000 * (thousands - '0') + 100 * (hundreds - '0') +
          10 * (tens - '0') + ones - '0');
}


int main(void) {
  // -------- Inits --------- //
  clock_prescale_set(clock_div_2);                  /* CPU clock 4 MHz */

  uint16_t velocity_setpoint = 0;

  initUSART();
  UCSR0A &= ~(1 << RXC0);
 
  initTimer0();
  OCR0A = 0;
 
  initTimer1();
  initEncoder(); 
  
  DDRD |= (1 << PD6);				// PWM output. 
  		
 
  // ------ Event loop ------ //
  while (1) {
    
    if (bit_is_set(UCSR0A, RXC0)) {
        printString("\r\nEnter a desired rpm number for motor (200 to 5000):\r\n"); 
        velocity_setpoint = getNumber16();       
    }
            
    if (velocity < velocity_setpoint) {    
        while (velocity < velocity_setpoint) {
            OCR0A++;
            calVelocity();
            _delay_ms(30);
            }
    }
    else {    
        while (velocity > velocity_setpoint) {
            OCR0A--;
            calVelocity();
            _delay_ms(30);
            }
    }   

    printVelocity(velocity);    
//  printVelocity(count);
    printString("==========\r\n\r\n");

	_delay_ms(100);
	
  }	
 return (0);                            /* This line is never reached */
}
