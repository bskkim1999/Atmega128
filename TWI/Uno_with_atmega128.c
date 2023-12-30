#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "UART0.h"

FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, UART0_receive, _FDEV_SETUP_READ);


// TWI(I2C) 통신 초기화 함수
void TWI_Init() {
	// TWI 클럭 주파수 설정 (SCL_freq = CPU_clk / (16 + 2 * TWBR * Prescaler))
	
	TWSR = (0<<TWPS0) | (0<<TWPS1);
	TWBR = 72;  // TWBR 값 설정 (주파수를 조절하여 원하는 클럭 주파수 설정)  100khz
	_delay_ms(300);  //setup bus free time
}

// TWI(I2C) 통신 시작 함수
void TWI_Start() {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	
	while (!(TWCR & (1 << TWINT)));
}

// TWI(I2C) 통신 종료 함수
void TWI_Stop() {
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

// TWI(I2C) 통신으로 데이터 전송 함수
void TWI_Write(unsigned char data) {
	
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	
	while (!(TWCR & (1 << TWINT)));
}


///////////////////////////////////////////////////////////////////////////////////

//master : atmega128
int main() {
	
	
	stdout = &OUTPUT;
	stdin = &INPUT;
	
	/*uart*/
	//UART0_init();
	
	TWI_Init();
	//printf("3 \r\n");
	
	//printf("4 \r\n");
	
	///////////////////////////work/////////////////////////////
	while(1){
		TWI_Start();
		TWI_Write(0x10);  //address, write mode 0001 0000
		TWI_Write('1');
		_delay_ms(500);
		
		TWI_Start();
		TWI_Write(0x10);  //address, write mode 0001 0000
		TWI_Write('2');
		_delay_ms(500);
	}
	
	
	/////////////////////////////////////////////////////////////
	TWI_Stop();
	
	
	
	
	while (1) {
		
	}
	return 0;
}
