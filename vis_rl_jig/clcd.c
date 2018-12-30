// ------- Preamble -------- //
#include <avr/io.h>                        /* Defines pins, ports, etc */
#include <util/delay.h>                     /* Functions to waste time */
#include <avr/power.h>
#include <avr/interrupt.h>
#include "pinDefines.h"

#include "clcd.h"

int main(void){
	char data1[] = "Hello, world!";
	char data2[] = "(*^-^) (/^^)/";

	clcd_init(BIT4_MODE);	// CLCD�� 4��Ʈ�� �ʱ�ȭ ��Ŵ
	while(TRUE){
		_delay_ms(500);	//��� ��ٷȴٰ�
		clcd_str(LINE1, data1);	//CLCD�� data1 ���ڿ��� ���
		_delay_ms(500);
		clcd_str(LINE2, data2);	//CLCD data2 ���ڿ��� ���
		_delay_ms(500);
		clcd_command(CMD_CLEAR_DISPLAY);	//ȭ���� �����ϰ� û��
	}
}
