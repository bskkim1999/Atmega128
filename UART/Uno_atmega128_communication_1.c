#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "UART0.h"

FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, UART0_receive, _FDEV_SETUP_READ);

void UART1_init(void)
{
	UBRR1H = 0x00; // 9600 보율로 설정
	UBRR1L = 207;
	UCSR1A |= _BV(U2X1); // 2배속 모드
	// 비동기, 8비트 데이터, 패리티 없음, 1비트 정지 비트 모드
	UCSR1C |= 0x06;
	UCSR1B |= _BV(RXEN1); // 송수신 가능
	UCSR1B |= _BV(TXEN1);
}

void UART1_transmit(unsigned char data)
{
	while( !(UCSR1A & (1 << UDRE1)) ); // 송신 가능 대기
	UDR1 = data; // 데이터 전송
}

int main(void){
	
	stdout = &OUTPUT;
	stdin = &INPUT;
	
	/*variable*/
	int data;  //2byte
	
	/*uart*/
	UART0_init();
	UART1_init();
	/*gpio*/
	
	
	/*timer&counter*/
	

	
	/* Replace with your application code */
	
	int dataToSend ;
	printf("input:");
	scanf("%d", &dataToSend);
	
	// 정수 값을 2바이트로 분할하여 전송
	unsigned char lowByte = dataToSend & 0xFF;
	unsigned char highByte = (dataToSend >> 8) & 0xFF;
	
	UART1_transmit(lowByte);
	_delay_ms(1);
	UART1_transmit(highByte);
	
	while (1) {
		
		
	}
	
	return 0;
}
