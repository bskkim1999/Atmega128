#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "UART0.h"

FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, UART0_receive, _FDEV_SETUP_READ);

volatile unsigned long timer0_millis = 0;
volatile int timer0_micros = 0;


ISR(TIMER0_OVF_vect){
	
	unsigned long m = timer0_millis;
	int f = timer0_micros;
	
	m+=1;
	f+=24;
	
	m+=(f/1000);
	f = f % 1000;
	
	timer0_millis = m;
	timer0_micros = f;
	
	//printf("%lu \r\n", m);    // uart의 오버런현상때문에 지연이 발생한다.
	
}

unsigned long millis(){
	
	unsigned long m;
	uint8_t oldSREG = SREG;
	
	cli();
	
	m = timer0_millis;
	
	SREG = oldSREG;
	
	return m;
	
	
}




void init_timer0(){
	TCCR0 = 1 << CS02;   //pre-scaler : 64
	TIMSK = 1 << TOIE0;  //overflow interrupt accept
	
	sei();
	
	
}


int main(void){
	
	stdout = &OUTPUT;
	stdin = &INPUT;
	
	/*variable*/
	uint8_t state = 0;
	unsigned long tp, tc;
	
	/*uart*/
	UART0_init();
	
	/*function*/
	init_timer0();
	
	/*gpio*/
	DDRB = 0x01;
	PORTB = 0x00;
	
	tp = millis();
	
	
	
	/* Replace with your application code */
	while (1) {
		tc = millis();
		
		if(tc - tp >1000){
			tp = tc;
			
			state = (state + 1) % 2;
			PORTB= state;
		}
	}
	
	return 0;
}
