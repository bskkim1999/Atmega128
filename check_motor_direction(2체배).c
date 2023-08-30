#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "UART0.h"

FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, UART0_receive, _FDEV_SETUP_READ);


volatile int count = 0;  //n번째
int flag_cw = 0;
int flag_ccw = 0;
//encoder(PE4), encoder A
ISR(INT4_vect){
	
	count++;  //n번째
	
	//홀수번째
	if(count % 2 == 1){
		//PIND가 0이라면?cw이다.
		if( (PIND & 0x01) == 0){
			flag_ccw++;
		}
		//PIND가 1이라면?ccw이다.
		else if( (PIND & 0x01) == 1){
			flag_cw++;
		}
	}
	
	
	//짝수번째
	else if(count % 2 == 0){
		//PIND가 1이라면?cw이다.
		if( (PIND & 0x01) == 1){
			flag_ccw++;
		}
		//PIND가 0이라면?ccw이다.
		else if( (PIND & 0x01) == 0){
			flag_cw++;
		}
		
	}
	
	
}

//interrupt (external), encoder A
void INIT_INT4(void){
	
	EIMSK = 0x10;   //external interrupt INT4(PE4)
	EICRB = 0x01;  //rising edge or falling edge
	sei();  //global interrupt(accept)
	
	
	
}

//pwm/ timercounter1, 파형출력핀 = OC1A, PB5
void InitializeTimer1(void){
	
	TCCR1A = 0x82;  //  fast pwm, output mode  1000 0010
	TCCR1B = 0x1a;  // pre-scaler : 8, 0001 1010
	
	ICR1 = 39999;  //compare-match interrupt, fpwm = 50hz(because of formula), this register decide top
	
	
	
}





int main(void){
	
	stdout = &OUTPUT;
	stdin = &INPUT;
	
	/*variable*/
	
	
	/*uart*/
	UART0_init();
	
	/*gpio*/
	DDRA = 0xff;  //output, //for giving 5v (power)
	DDRB = 0xff;  //output, produce pwm wave, dutycycle : 5% (rpm)
	DDRD = 0x00;  //input, //encoder B, for using gpio
	DDRE = 0X00;  //input, //encoder A, for using external interrupt
	
	
	PORTA = 0xff;  ////for giving 5v (power)
	
	/*timer&counter*/
	
	INIT_INT4();
	InitializeTimer1();
	
	
	OCR1A = 8000;   //this register decide dutycycle, dutycycle = 20%, alive motor
	
	while (1) {
		//2번 카운트 될 때 마다 시계방향 또는 반시계방향을 결정함.
		//if - else if문 들어올 때 마다 초기화시키고, cw인지 ccw인지 확실하게 결정하기 위해서 "2이상" 이라는 조건을 덧붙임.
		if( (flag_ccw % 2 == 0) && (flag_ccw >= 2) ){
			
			printf("ccw \r\n");
			flag_cw = 0; //초기화
			flag_ccw = 0;  //초기화
		}
		
		else if( (flag_cw % 2 ==0)  && (flag_cw >= 2) ){
			
			printf("cw \r\n");
			flag_cw = 0; //초기화
			flag_ccw = 0;  //초기화
		}
		
		
		//debugging//
		//printf("flag_cw : %d          ", flag_cw);
		//printf("flag_ccw : %d \r\n", flag_ccw);

		
	}
	
	printf("finish!");
	return 0;
}
