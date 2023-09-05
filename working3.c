//pid control
#define P_GAIN 4
#define	I_GAIN 0.001
#define D_GAIN 1.18
//#define TIME 0.001
//atmega system
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "UART0.h"

FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, UART0_receive, _FDEV_SETUP_READ);



volatile long external_pulses=0;
volatile long expected_pulses = 0;  //long = 4byte
volatile long count = 0;  //n번째

volatile int flag_cw = 0;
volatile int flag_ccw = 0;

int wheels=0;  //바퀴 수
volatile int flag=0;

//pid control variable
unsigned long target = 0;  //목표 펄스수
unsigned long current = 0;   //현재 펄스수
float pControl = 0;
float iControl = 0;
float dControl = 0;
float pidControl = 0;
float realError = 0;
float accError = 0;
float errorGap = 0;


///////////////////////////////////////////////////////////////////////////////////////////////
//encoder(PE4), encoder A
ISR(INT4_vect){
	
	count++;  //n번째
	
	
	//펄스수를 다 세었으면?
	if(  (external_pulses !=0)  && (external_pulses >= expected_pulses)) {
		EIMSK = 0x00;  //individual interrupt(decline)
		OCR1A = 0;  //dutycycle 0%, motor stop!
		flag = 1;
		
	}
	
	//펄스수를 세는 중이라면? 방향(cw or ccw) 결정한다.
	else{
		external_pulses ++;  //펄스 개수 셈.
		current++;  //현재 펄스 수를 센다. (pid)
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
/////////////////////////pid function/////////////////////////////////
void pControlSystem(void) {

	pControl = P_GAIN * realError;

	
}

void iControlSystem(void) {

	iControl = I_GAIN * accError ;

	
}

void dControlSystem(void) {

	dControl = D_GAIN * errorGap;

	
}

void pidControlSystem(void) {
	
	pidControl = pControl + iControl + dControl;
	
	pidControl = pidControl * 2.875;  //1바퀴, 360도
	//dutycycle을 조정함으로써 모터속도를 제어해야 한다. 최대 dutycycle = 30000 (75%), 최소 dutycycle = 2000(5%) 
	//dutycycle >= 75% 라고 계산결과(pid결과)가 나왔으면?
	if(pidControl >= 30000){
		OCR1A = 30000;
	}
	
	//pidControl < 30000 이라면?
	else {
		//dutycycle <= 5% 라면?
		if(pidControl <= 2000){
			OCR1A = 2000;  //dutycycle = 5%로 고정
		}
		// 5% < dutycycle < 75%
		//2000 < pidControl < 30000
		else{
			OCR1A = pidControl;  //decide pwm, control motor speed
		}
		
	}
	
	
	
}

void calculateErrors(void) {
	errorGap = target - current - realError;  //목표 값 - 현재 값 - 실시간 에러
	realError = target - current;	// 실시간 에러는 단순히 목표값 - 현재값을 의미합니다.
    accError += realError;  //누적 에러
    /*
    	누적 에러는 실시간 에러를 계속해서 더한 값입니다.
        I 제어를 위해 사용됩니다.
	*/
    

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
	DDRC = 0xff;  //output, check direction from led
	DDRD = 0x00;  //input, //encoder B, for using gpio
	DDRE = 0x00;  //input, //encoder A, for using external interrupt
	
	
	PORTA = 0xff;  ////for giving 5v (power)
	
	/*timer&counter*/
	
	
	while (1) {
		////////////////////////////////////초기화//////////////////////////
		external_pulses = 0;
		flag = 0;
		flag_cw = 0;
		flag_ccw = 0;
		
		
		
		//pid//
		current = 0; //(pid)
		pControl = 0;
		iControl = 0;
		dControl = 0;
		pidControl = 0;
		realError = 0;
		accError = 0;
		errorGap = 0;
		/////////////////////////////////////////////////////////////////////
		PORTC = 0x00; //all led are off
		
		
		//terminal//
		printf("input number of wheels : ");
		scanf("%d", &wheels);
		
		//wheels=1;
		expected_pulses = round(wheels * 360 * 5.59444);  //형변환(실수형으로), 소수 첫째자리에서 반올림
		target = expected_pulses;
		
		printf("expected : %ld \r\n", expected_pulses);
		
		if(wheels >= 0){
			INIT_INT4();
			InitializeTimer1();
			
			flag = 0;  
			OCR1A = 2000;   //this register decide dutycycle, dutycycle = 5%, alive motor, control speed
			
			
			//2번 카운트 될 때 마다 시계방향 또는 반시계방향을 결정함.
			//if - else if문 들어올 때 마다 초기화시키고, cw인지 ccw인지 확실하게 결정하기 위해서 "2이상" 이라는 조건을 덧붙임.
			while(1){
				//pid control
				calculateErrors();
				pControlSystem();
				iControlSystem();
				dControlSystem();
				
				pidControlSystem();
				
				//printf("OCR1A : %d      \r\n", OCR1A);
				//printf("pidControl : %f \r\n", pidControl);
				//펄스수를 다 세었으면 무한루프 탈출
				if(flag == 1){
					//초기화
					flag_cw = 0;
					flag_ccw = 0;
					break;
				}
				
				//펄스수를 세는 중임.
				else {
					
					//printf("pidControl : %f \r\n", pidControl);
					//펄스수를 세는 중이라면 방향 결정하여 출력.
					if( (flag_ccw % 2 == 0) && (flag_ccw >= 2) ){
						
						//printf("ccw \r\n");
						PORTC = 0x01;
						flag_cw = 0; //초기화
						flag_ccw = 0;  //초기화
					}
					
					else if( (flag_cw % 2 ==0)  && (flag_cw >= 2) ){
						
						//printf("cw \r\n");
						PORTC = 0x02;
						flag_cw = 0; //초기화
						flag_ccw = 0;  //초기화
					}
					//printf("OCR1A : %d \r\n", OCR1A);
					//printf("external pulses : %ld \r\n", external_pulses);
					//printf("expected_pulses : %ld \r\n", expected_pulses);
					//printf("count : %ld \r\n", count);
				}
				
				//check dutycycle
				//printf("OCR1A : %d \r\n", OCR1A);
				//printf("external_pulses : %ld \r\n", external_pulses);
				
			}
			
		}
		
		else{
			
			continue;
		}
		
		
	}
	
	printf("finish!");
	return 0;
}
