/*
 * test_0630.cpp
 *
 * Created: 2023-06-30 오후 2:29:31
 * Author : admin
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#define F_CPU 16000000UL
#include <util/delay.h>
unsigned char led=0x01, ledB=0x01;

int main(void){
	
	DDRE = 0xff;
	DDRB = 0xff;
	PORTE = ledB;
	TCCR0 = 0x1f;
	OCR0 = 0xaa;
	TCNT0 = 0x00;
	
	while(1);
	
	return 0;
    
}

