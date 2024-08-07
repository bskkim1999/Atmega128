#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <util/atomic.h>
#include "UART0.h"

//lcd
#define PORT_DATA PORTB
#define PORT_CONTROL PORTC
#define DDR_DATA DDRB
#define DDR_CONTROL DDRC

#define RS_PIN 0
#define RW_PIN 1
#define E_PIN 2

#define COMMAND_CLEAR_DISPLAY 0x01 //0000 0001
#define COMMAND_8_BIT_MODE 0x30  //0011 0000
#define COMMAND_4_BIT_MODE 0x28 //0010 1000

#define COMMAND_DISPLAY_ON_OFF_BIT 2 //0000 0010
#define COMMAND_CURSOR_ON_OFF_BIT 1 //0000 0001
#define COMMAND_BLINK_ON_OFF_BIT 0  //0000 0000

FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, UART0_receive, _FDEV_SETUP_READ);


//lcd 8bit mode or 4bit mode
int mode=4;
int count = 65;

void LCD_pulse_enable(void){
	PORT_CONTROL |= (1 << E_PIN);  //PORTC = PORTC | 0000 0100
	_delay_ms(1);
	PORT_CONTROL &= ~(1 << E_PIN); //PORTC = ~(PORTC & 0000 0100)
	_delay_ms(1);
}

void LCD_write_data(unsigned char data){
	PORT_CONTROL |= (1 << RS_PIN); //PORTC = PORTC | 0000 0001
	
	if(mode==8){
		PORT_DATA = data;  //PORTD = data
		LCD_pulse_enable();
	}
	else{
		PORT_DATA = data & 0xf0;
		LCD_pulse_enable();
		
		PORT_DATA = (data<<4) & 0xf0;
		LCD_pulse_enable();
	}
	_delay_ms(2);
}

void LCD_write_command(unsigned char command){
	PORT_CONTROL &= ~(1 << RS_PIN);  //PORTC = ~(PORTC & 0000 0001)
	if(mode==8){
		PORT_DATA = command; //PORTD = command
		LCD_pulse_enable();
	}
	else{
		PORT_DATA = command & 0xf0;
		LCD_pulse_enable();
		
		PORT_DATA = (command << 4) & 0xf0;
		LCD_pulse_enable();

	}
	_delay_ms(2);
}

void LCD_clear(void){
	LCD_write_command(COMMAND_CLEAR_DISPLAY);   //command = 0x01
	_delay_ms(2);
}

void LCD_init(void){
	_delay_ms(50);
	
	if(mode==8){
		DDR_DATA = 0xff;  //DDRD = 0xff  output
	}
	else{
		DDR_DATA = 0xf0;
	}
	PORT_DATA = 0x00; //PORTD = 0x00  //low
	DDR_CONTROL |= (1 << RS_PIN) | (1 << RW_PIN) | (1 << E_PIN);  //DDRC = DDRC | 0000 0001 | 0000 0010 | 0000 0100
	
	PORT_CONTROL &= ~(1 << RW_PIN); //PORTC = ~(PORTC & 0000 0001)   //write mode
	
	if(mode==8){
		LCD_write_command(COMMAND_8_BIT_MODE);  //command = 0011 0000   //8bit mode
	}
	else{
		LCD_write_command(0x02);  // command = 0000 0010
		LCD_write_command(COMMAND_4_BIT_MODE);  //command = 0010 1000  //4bit mode
	}
	
	unsigned char command = 0x08 | (1 << COMMAND_DISPLAY_ON_OFF_BIT); //command = 0000 1000 | 0000 0100 = 0000 1100
	LCD_write_command(command);  //command = command
	
	LCD_clear();
	
	LCD_write_command(0x06);  //command = 0000 0110
	
}

void LCD_goto_XY(unsigned char row, unsigned char col){
	unsigned char command;
	if(row == 0){
		command = 128 + col;
	}
	else if(row == 1){
		command = 148 + col;
	}
	
	LCD_write_command(command);
	
}

void LCD_write_string(char *string){
	unsigned char i;
	for(i=0 ; string[i] != '\0' ; i++){
		LCD_write_data(string[i]);
	}
}




int main(void){
	
	stdout = &OUTPUT;
	stdin = &INPUT;
	
	/*variable*/
	
	
	/*uart*/
	//UART0_init();
	
	/*gpio*/
	DDRA = 0xff; //output
	PORTA = 0x01; // 0000 0001   //5v
	
	
	/*timer&counter*/
	
	LCD_init();
	LCD_clear();
	_delay_ms(1000);
	
	
	//application character code
	LCD_goto_XY(0,0);
	
	for(int i=1 ; i<=26 ; i++){
		LCD_write_data(count);
		count++;
	}
	
	
	/* Replace with your application code */
	while (1) {
		
		
	}
	
	
	//printf("finish!");
	return 0;
}
