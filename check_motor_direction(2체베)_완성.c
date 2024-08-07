#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "UART0.h"

FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, UART0_receive, _FDEV_SETUP_READ);

volatile int external_pulses=0;
volatile int expected_pulses = 0;
volatile int count = 0;  //n번째

volatile int flag_cw = 0;
volatile int flag_ccw = 0;


int angle=0;
volatile int flag=0;
//encoder(PE4), encoder A
ISR(INT4_vect){
	
	count++;  //n번째
	
	
	//펄스수를 다 세었으면?
	if(  (external_pulses !=0)  && (external_pulses >= expected_pulses)) {
		cli();  //global interrupt(decline)
		OCR1A = 0;  //dutycycle 0%, motor stop!
		flag = 1;
		
	}
	
	//펄스수를 세는 중이라면? 방향(cw or ccw) 결정한다.
	else{
		external_pulses ++;  //펄스 개수 셈.
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
	
	
	while (1) {
		//초기화//
		external_pulses = 0;
		flag = 0;
		flag_cw = 0;
		flag_ccw = 0;
		
		INIT_INT4();
		InitializeTimer1();
		
		
		printf("input angle : ");
		scanf("%d", &angle);
		
		expected_pulses = round(angle * 5.59444);  //형변환(실수형으로), 소수 첫째자리에서 반올림
		
		if(angle >= 0){
			//external_pulses = 0;
			//flag = 0;  // 왜 이곳에 flag를 초기화 시켜야 하는가? 의문이다. 여기에 초기화를 시키지 않고 상단에서 초기화를 시키면 flag = 1이 됨.
			OCR1A = 8000;   //this register decide dutycycle, dutycycle = 20%, alive motor
			
			//printf("check1!   ");
			//printf("flag_1 : %d      \r\n", flag);
			//2번 카운트 될 때 마다 시계방향 또는 반시계방향을 결정함.
			//if - else if문 들어올 때 마다 초기화시키고, cw인지 ccw인지 확실하게 결정하기 위해서 "2이상" 이라는 조건을 덧붙임.
			while(1){
				//printf("check2!   \r\n");
				//printf("flag_cw : %d          ", flag_cw);
				//printf("flag_ccw : %d \r\n", flag_ccw);
				//printf("external_pulses : %d \r\n", external_pulses);
				//printf("expected_pulses : %d   ", expected_pulses);
				//printf("flag_2 : %d      ", flag);
				//펄스수를 다 세었으면 무한루프 탈출
				if(flag == 1){
					//초기화
					flag_cw = 0;
					flag_ccw = 0;
					break;
				}
				
				//펄스수를 세는 중임.
				else {
					//printf("flag_cw : %d          ", flag_cw);
					//printf("flag_ccw : %d \r\n", flag_ccw);
					
					//펄스수를 세는 중이라면 방향 결정하여 출력.
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
					
					
				}
				
				
			}
			
		}
		
		else{
			//printf("check3!   ");
			continue;
		}
		
		
		//debugging//
		//printf("flag_cw : %d          ", flag_cw);
		//printf("flag_ccw : %d \r\n", flag_ccw);
		//printf("check4!   ");
		
	}
	
	printf("finish!");
	return 0;
}
