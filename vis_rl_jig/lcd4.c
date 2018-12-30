
#include <avr/io.h>
#include <util/delay.h>
#include <avr/power.h>
#include <avr/pgmspace.h>
#include "portpins.h"
#include "pinDefines.h"
#include "lcd.h"

   
//const uint16_t num_lens_p PROGMEM = 0;
//const uint16_t num_door_p PROGMEM = 0;

void printFloat(float number) {

  Lcd4_Write_Char(' ');                                  
  Lcd4_Write_Char('0' + floor(number / 10000));                                  /* ten thousands places */
  Lcd4_Write_Char('0' + floor(number / 1000) - 10 * floor(number / 10000));      /* thousands places */
  Lcd4_Write_Char('0' + floor(number / 100) - 10 * floor(number / 1000));        /* hundreds places */
  Lcd4_Write_Char('0' + floor(number / 10) - 10 * floor(number / 100));      /* tens places */
  Lcd4_Write_Char('0' + floor(number / 1) - 10 * floor(number / 10));      /* ones */

}

int main(void)
{
//   clock_prescale_set(clock_div_1);                  /* CPU clock 8 MHz */

   DDRC = 0xFF;
   DDRB = 0xFF;
   int i;
   float num_lens = 0;
   float num_door = 0;
   
   Lcd4_Init();
   
   Lcd4_Clear();
   Lcd4_Set_Cursor(1,1);
   Lcd4_Write_String("Visque-Smart Reliability Jig");
	   
   for(i=0;i<15;i++)
   {
       _delay_ms(500);
	   Lcd4_Shift_Left();
   }

/*	   
       for(i=0;i<15;i++)
	   {
		   _delay_ms(300);
		   Lcd4_Shift_Right();
	   }
*/	   
   
   while(1)
   {
	   	   
	   Lcd4_Clear();

	   Lcd4_Set_Cursor(1,1);
	   Lcd4_Write_String("DOOR:");
       printFloat(num_door);

       PORTB |= (1 << PB4);  // cylinder up
       _delay_ms(5000);
       PORTB &= ~(1 << PB4); // cylinder down
       _delay_ms(5000);
       num_door++;
	   	   
	   Lcd4_Set_Cursor(2,1);
	   Lcd4_Write_String("LENS:");
       printFloat(num_lens);	   
	   _delay_ms(1000);
   }
}
