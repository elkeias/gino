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

	clcd_init(BIT4_MODE);	// CLCD를 4비트로 초기화 시킴
	while(TRUE){
		_delay_ms(500);	//잠깐 기다렸다가
		clcd_str(LINE1, data1);	//CLCD에 data1 문자열을 출력
		_delay_ms(500);
		clcd_str(LINE2, data2);	//CLCD data2 문자열을 출력
		_delay_ms(500);
		clcd_command(CMD_CLEAR_DISPLAY);	//화면을 깨끗하게 청소
	}
}
