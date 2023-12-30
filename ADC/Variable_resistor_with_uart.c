/*
 * test_0630.cpp
 *
 * Created: 2023-06-30 오후 2:29:31
 * Author : admin
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "UART0.h"

FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, UART0_receive, _FDEV_SETUP_READ);

void ADC_init(unsigned char channel){
	ADMUX |= 0x00; //external AREF
	ADCSRA |= 0x07;  // 분주율
	ADCSRA |= (1<<ADEN); // enable ADC
	ADCSRA |= (1<<ADFR); // mode : free running

	ADMUX = ((ADMUX & 0xE0) | channel) ; 
	ADCSRA |= (1 << ADSC);  //start ad convert
}

int read_ADC(void){
	while(!(ADCSRA & (1 << ADIF)));
	
	return ADC;
}

int main(void){
	
	/*gpio control*/ 
	DDRA = 0xff;
	PORTA = 0xff;
	
	stdout = &OUTPUT;
	stdin = &INPUT;
	
	int read;
	
	UART0_init();
	ADC_init(0);
	
	while(1){
		
		read = read_ADC();
		
		printf("%d\r\n", read);
		//UART0_print_1_byte_number(read);
		//UART0_print_string("\r\n");
		
		_delay_ms(500);
		
		
		
	}
	
	
	return 0;	

		
}

