/*
 * test_0630.cpp
 *
 * Created: 2023-06-30 오후 2:29:31
 * Author : admin
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <time.h>
//하나의 포트에 입력과 출력 제어하기
int main(void){
	
	DDRA = 0x01;  
	PORTA = 0x00; //0v 
	
    /* Replace with your application code */
    while (1) {
		
		unsigned char data = PINA;
		data = (data & 0x02);
		
		if(data == 0x02){
			
			PORTA = 0x01;
			_delay_ms(500);
			PORTA = 0x00;
			_delay_ms(500);
		}
		
		else if(data==0){
			PORTA = 0x01;
			_delay_ms(1000);
			PORTA = 0x00;
			_delay_ms(1000);
		}
		
		
    }
	
	
	return 0;
}

