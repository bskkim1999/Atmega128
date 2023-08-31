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

//데이터 전송함수
void uart_tx_char(unsigned char data){
	
	while( (UCSR0A & (1 << UDRE0))  == 0x00);  //참 : 계속돈다(무한루프)   UDRE0 == 0, 거짓 : 반복문 탈출    UDRE0 == 1
	
	UDR0 = data;  //데이터 전송
	
	
}


//변수 선언 및 초기화함수
void uart_tx_buffer_init(void){
	uart_tx_buffer.inptr = 0;
	uart_tx_buffer.outptr = 0;
	uart_tx_buffer.state = TX_IDLE; //상태를 나타낸다.
}

//링버퍼에 데이터 쓰기함수
void uart_tx_buffer_write(unsigned char data){
	
	UCSR0B &= ~(1 << TXCIE0);    //송신완료 인터럽트 발생을 허용하지 않는다.

	//만약 입력포인터와 출력포인터가 같은 위치에 있다면?
	if(uart_tx_buffer.inptr == uart_tx_buffer.outptr){
		
		// 만약 상태가 IDLE라면?
		if(uart_tx_buffer.state == TX_IDLE){
			
			uart_tx_char(data);    //데이터를 전송한다.
			uart_tx_buffer.state = TX_BUSY;   //상태를 busy로 변경한다.
		}

		// 만약 상태가 IDLE가 아니라면? 즉 busy상태라면?
		else{
			uart_tx_buffer.buffer[uart_tx_buffer.inptr++] = data;    //버퍼에 데이터를 순차적으로 저장한다.
		}
		
		
	}
	
	//만약 입력포인터와 출력포인터가 같은 위치에 있지 않다면? 즉, 다른 위치에 있다면?
	else{
		uart_tx_buffer.buffer[uart_tx_buffer.inptr++] = data;      //버퍼에 데이터를 순차적으로 저장한다.
	}
	
	UCSR0B |= (1 << TXCIE0); //송신완료 인터럽트 발생을 허용한다.
	
}

//데이터 송신 처리 : 인터럽트 서비스 루틴
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

