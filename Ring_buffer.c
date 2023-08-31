#define F_CPU 16000000UL
#define TX_IDLE 0
#define TX_BUSY 1
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

volatile uart_tx_buffer_type uart_tx_buffer;

//구조체 정의
typedef struct{
	
	unsigned char buffer[256];
	unsigned char inptr;
	unsigned char outptr;
	unsigned char state;
	
}uart_tx_buffer_type;

//데이터 전송
void uart_tx_char(unsigned char data){
	
	while( (UCSR0A & (1 << UDRE0))  == 0x00);
	
	UDR0 = data;
	
	
}


//변수 선언 및 초기화
void uart_tx_buffer_init(void){
	uart_tx_buffer.inptr = 0;
	uart_tx_buffer.outptr = 0;
	uart_tx_buffer.state = TX_IDLE;
}

//링버퍼에 데이터 쓰기
void uart_tx_buffer_write(unsigned char data){
	
	UCSR0B &= ~(1 << TXCIE0);
	
	if(uart_tx_buffer.inptr == uart_tx_buffer.outptr){
		
		if(uart_tx_buffer.state == TX_IDLE){
			
			uart_tx_char(data);
			uart_tx_buffer.state = TX_BUSY;
		}
		
		else{
			uart_tx_buffer.buffer[uart_tx_buffer.inptr++] = data;
		}
		
		
	}
	
	else{
		uart_tx_buffer.buffer[uart_tx_buffer.inptr++] = data;
	}
	
	UCSR0B |= (1 << TXCIE0); 
	
}

//데이터 송신 처리
ISR(USART0_TX_vect){
	
	if(uart_tx_buffer.inptr != uart_tx_buffer.outptr){
		UDR0 = uart_tx_buffer.buffer[uart_tx_buffer.outptr++];
	}
	
	else{
		uart_tx_buffer.state = TX_IDLE;
	}
	
}




int main(void){

    
	
    while (1){
    
    }
	
	return 0;
}

