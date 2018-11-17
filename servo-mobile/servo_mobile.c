                  /* Quick interactive demo running servo with Timer 1 */

// ------- Preamble -------- //
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "pinDefines.h"

#define PULSE_MIN         1000         /* experiment with these values */
#define PULSE_MAX         2000              /* to match your own servo */
#define PULSE_MID         1500


static inline void initTimer1Servo(void) {
                   /* Set up Timer1 (16bit) to give a pulse every 20ms */
                             /* Use Fast PWM mode, counter max in ICR1 */
  TCCR1A |= (1 << WGM11);
  TCCR1B |= (1 << WGM12) | (1 << WGM13);
  TCCR1B |= (1 << CS10);  /* /1 prescaling -- counting in microseconds */
  ICR1 = 20000;                                    /* TOP value = 20ms */
  TCCR1A |= (1 << COM1A1);              /* Direct output on PB1 / OC1A */
  DDRB |= (1 << PB1);                            /* set pin for output */
}

static inline void letsplay(void) {
  
  
  OCR1A = PULSE_MIN;			//Clockwise Max
  DDRB |= (1 << PB1);          /* re-enable output pin */
  
    _delay_ms(2000);           //time for rotation. This is necessary!
    while (TCNT1 < 4000) {;
    }                          /* delay until pulse part of cycle done */
    DDRB &= ~(1 << PB1);      /* disable output pin */
  _delay_ms(10000);
  
  
  OCR1A = PULSE_MAX; 			//Counterclockwise Max
  DDRB |= (1 << PB1);          /* re-enable output pin */
  
    _delay_ms(2000);			//time for rotation. This is necessary!
    while (TCNT1 < 4000) {;
    }                          /* delay until pulse part of cycle done */
    DDRB &= ~(1 << PB1);      /* disable output pin */
  _delay_ms(10000);

}

int main(void) {

  // -------- Inits --------- //
  OCR1A = PULSE_MID;            /* set it to middle position initially */
  initTimer1Servo();
  _delay_ms(1000); 

  // ------ Event loop ------ //
  while (1) {

    letsplay();

  }                                                  /* End event loop */
  return (0);                            /* This line is never reached */
}