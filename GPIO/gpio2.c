/*
 * test_0630.cpp
 *
 * Created: 2023-06-30 오후 2:29:31
 * Author : admin
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>


int main(void){
	
	DDRA = 0xFF;  //output
	DDRB = 0x00;  //input
	//PORTB = 0xFF; //pull-up 
    /* Replace with your application code */
    while (1) {
		unsigned char data = PINB;
		//PINB 레지스터를 사용하여 입력받을 때, 플로팅상태를 방지하여야 한다. 만약 1비트만 읽고싶을 경우에는 비트마스크를 사용하면된다.!!
		if(data == 0x01){
			
			PORTA = 0x01;
			_delay_ms(500);
			PORTA = 0x00;
			_delay_ms(500);
		}
		
		else{
			PORTA = 0x01;
			_delay_ms(1000);
			PORTA = 0x00;
			_delay_ms(1000);
		}
		
    }
	
	
	return 0;
}

