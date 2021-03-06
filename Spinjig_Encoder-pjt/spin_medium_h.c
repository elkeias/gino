/*
Golf ball spin measurement project 2016~2017
*/

// ------- Preamble -------- //
#include <avr/io.h>                        /* Defines pins, ports, etc */
#include <util/delay.h>                     /* Functions to waste time */
#include <avr/power.h>
#include <avr/interrupt.h>
#include "pinDefines.h"
#include "USART.h"

#define CPT 505

volatile uint16_t count = 0;
volatile float velocity = 0;
volatile uint16_t velocity_setpoint;

volatile int16_t integral_error;
volatile int16_t previous_error;

static inline void initTimer0(void) {

TCCR0A |= (1 << WGM00);				// Set timer/counter 0 waveform generation mode to fast pwm.
TCCR0A |= (1 << WGM01);

TCCR0A |= (1 << COM0A1);            // Clear OC0A on compare match, Set OC0A at BOTTOM.
TCCR0B |= (1 << CS00)|(1 << CS01);				// Set timer/counter 1/64 prescaler. PWM frequencty = 488Hz
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
    if (TCNT1 > 80) {
        count = TCNT1;
    // 48counts per half cycle, 96 counts per full cycle = (1s / (Max rpm: 10000rpm/(60s/min) * CPT) / 0.000000125s
    }
    TCNT1 = 0;
}


static inline void calVelocity(void){    
    TCNT1 = 0;
    sei();
    _delay_ms(10);
    cli();
    velocity = 60 / (count * 0.000000125 * CPT);   //velociy calculated in rpm
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


void printFloat(float number) {
  number = round(number * 100) / 100; /* round off to 2 decimal places */
  transmitByte('0' + number / 10);                       /* tens place */
  transmitByte('0' + number - 10 * floor(number / 10));        /* ones */
  transmitByte('.');
  transmitByte('0' + (number * 10) - floor(number) * 10);    /* tenths */
                                                   /* hundredths place */
  transmitByte('0' + (number * 100) - floor(number * 10) * 10);
  printString("\t");
}


static inline uint16_t getNumber16(void) {
  // Reads in characters, turns them into a number
  char ten_thousands = '0';
  char thousands = '0';
  char hundreds = '0';
  char tens = '0';
  char ones = '0';
  char thisChar = '0';

  do {
    ten_thousands = thousands;
    thousands = hundreds;                        /* shift numbers over */
    hundreds = tens;
    tens = ones;
    ones = thisChar;
    thisChar = receiveByte();                   /* get a new character */
    transmitByte(thisChar);                                    /* echo */
  } while (thisChar != '\r');

  transmitByte('\n');                                       /* newline */
  return (10000 * (ten_thousands - '0') + 1000 * (thousands - '0') + 100 * (hundreds - '0') +
          10 * (tens - '0') + ones - '0');
}


static inline uint8_t do_pid(float p, float i, float d) {     
     float p_error;
     float i_error;
     float d_error;
     
     float p_output;
     float i_output;
     float d_output;
     
     float output;     
     
     p_error = (float)velocity_setpoint - velocity;
     i_error = integral_error;
     d_error = p_error - previous_error;
     
     p_output = p * p_error;
     i_output = i * i_error;
     d_output = d * d_error;
     
     output = (p_output + i_output + d_output) / 80 ;  
     
     if (output > 255) {
        output = 255;
     }
     if (output < 0) {
        output = 0;
     }

//     printString("\r\n====output=====\r\n");
//     printVelocity(output);

//     printString("\r\n===p_error=====\r\n");
//     printVelocity(p_error);
         
     previous_error = p_error;
     integral_error += p_error;
     
     return (uint8_t)output;
}   


int main(void) {
  // -------- Inits --------- //
  clock_prescale_set(clock_div_1);                  /* CPU clock 8 MHz */

  float kp = 2;
  float ki = 0.05;
  float kd = 5;
  
  char input;
  
  velocity_setpoint = 50;
  
  initUSART();
  UCSR0A &= ~(1 << RXC0);
 
  initTimer0();
 
  initTimer1();
  initEncoder(); 
  
  DDRD |= (1 << PD6) | (1 << PD7);				// PWM & direction output. 
  PORTD |= (1 << PD7); 		// direction high
 
  // ------ Event loop ------ //
  while (1) {                     
    
    if (bit_is_set(UCSR0A, RXC0)) {

        input = UDR0;
        
        if (input == 'r') {
            printString("Riversing Direction...\r\n");
            
            while (OCR0A > 0) 
            {
                OCR0A--;
                _delay_ms(50);
                }  
            
            while (velocity > 200) {
                calVelocity();
            }
            
            PORTD ^= (1 << PD7);
            
            //if (velocity_setpoint > 500) {
                do 
               {
                   OCR0A++;
                   _delay_ms(100);
                   calVelocity();
                } while (velocity < velocity_setpoint);
            //}                                 
        }
/*        
        else if (input == 'p') {
            printString("Setting Kp parageters...\r\n");
            kp = (float)getNumber16() / 100;
        }
        else if (input == 'i') {
            printString("Setting Ki parageters...\r\n");
            ki = (float)getNumber16() / 100;
        }
        else if (input == 'd') {
            printString("Setting Kd parageters...\r\n");
            kd = (float)getNumber16() / 100;
        }
*/
        else if (input == 'm') {
             
            printString("\r\nEnter a desired rpm number for motor (200 to 10000):\r\n"); 
            velocity_setpoint = getNumber16();

            while (velocity < (velocity_setpoint)) {
                OCR0A++;
                _delay_ms(50);
                calVelocity();           
            }
            
        previous_error = 0;
        
        }
        else if (input == 's') {
            printString("Kp     Ki     Kd\r\n");
            printFloat(kp);
            printFloat(ki);
            printFloat(kd);
            printString("\r\n");
            _delay_ms(1500);
        }
        else {}        
    }
     
    calVelocity();           
    OCR0A = do_pid(kp,ki,kd);
    
//    printString("\r\n\r\n====OCR0A====\r\n");
//    printByte(OCR0A);
      
//    printVelocity(count);
//    printString("\r\n====velocity====\r\n\r\n");
    printVelocity(velocity);  
    
	_delay_ms(10);
	
    }	
 return (0);                            /* This line is never reached */
}
