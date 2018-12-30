
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "portpins.h"
#include "pinDefines.h"
#include "lcd.h"

#define DEBOUNCE_TIME		100

float num_lens = 0;   


void printFloat(float number) {

  Lcd4_Write_Char(' ');                                  
  Lcd4_Write_Char('0' + floor(number / 10000));                                  /* ten thousands places */
  Lcd4_Write_Char('0' + floor(number / 1000) - 10 * floor(number / 10000));      /* thousands places */
  Lcd4_Write_Char('0' + floor(number / 100) - 10 * floor(number / 1000));        /* hundreds places */
  Lcd4_Write_Char('0' + floor(number / 10) - 10 * floor(number / 100));      /* tens places */
  Lcd4_Write_Char('0' + floor(number / 1) - 10 * floor(number / 10));      /* ones */

}


uint8_t debounce_1(void) {
	if (bit_is_clear(PIND,PD2)) {
		_delay_ms(DEBOUNCE_TIME);
		if (bit_is_clear(PIND,PD2)) {
			return (1);
		}
	}
	return (0);
} 


void initInterrupt0(void) {

	EIMSK |= (1 << INT0);			// trigger when limit switch changes
	EICRA |= (1 << ISC01);			// The falling edge of INT0 generates an interrupt request
    sei();
}

ISR(INT0_vect) {
    if (debounce_1()) {		
        num_lens++;
	}
}


int main(void)
{

  initInterrupt0();

   DDRC = 0xFF;
   DDRB = 0xFF;
   PORTD |= (1 << PD2);     // initialize pullup resistor on the input pin 
   
   int i;
   float num_lens_2;   
   
   
   Lcd4_Init();
   Lcd4_Clear();
   Lcd4_Set_Cursor(1,1);
   Lcd4_Write_String("Visque-Smart Reliability Jig");
	   
   for(i=0;i<15;i++)
   {
       _delay_ms(250);
	   Lcd4_Shift_Left();
   }
   
   while(1)
   {
	   	   
	   Lcd4_Clear();

	   Lcd4_Set_Cursor(1,1);
	   Lcd4_Write_String("LENS:");
       num_lens_2 = num_lens / 2;
       printFloat(num_lens_2);

	   _delay_ms(1000);
   }
}
