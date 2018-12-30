
#include <avr/io.h>
#include <util/delay.h>
#include <avr/power.h>
#include "portpins.h"
#include "pinDefines.h"
#include "lcd.h"

int main(void)
{
   clock_prescale_set(clock_div_1);                  /* CPU clock 8 MHz */

   DDRC = 0xFF;
   DDRB = 0xFF;
   int i;
   Lcd4_Init();
   
   while(1)
   {
	   Lcd4_Set_Cursor(1,1);
	   Lcd4_Write_String("electroSome LCD Hello World");
	   for(i=0;i<15;i++)
	   {
		   _delay_ms(500);
		   Lcd4_Shift_Left();
	   }
	   for(i=0;i<15;i++)
	   {
		   _delay_ms(500);
		   Lcd4_Shift_Right();
	   }
	   Lcd4_Clear();
	   Lcd4_Set_Cursor(2,1);
	   Lcd4_Write_Char('e');
	   Lcd4_Write_Char('S');
	   _delay_ms(2000);
   }
}
