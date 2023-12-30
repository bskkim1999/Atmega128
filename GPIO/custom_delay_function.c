/*
 * test_0630.cpp
 *
 * Created: 2023-06-30 오후 2:29:31
 * Author : admin
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>




void custom_delay(int b){

	for (int x=0 ; x < b ; x ++){
		_delay_ms(1);
	}


}
int main(void){
	
	DDRA = 0xff;
    /* Replace with your application code */
    while (1) {
		
		for(int i=0 ; i<100 ; i++){
			PORTA = 0x00;
			custom_delay(i);
			PORTA = 0xff;
			custom_delay(i);

		}
		
		
    }
}

