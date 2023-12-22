#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "UART0.h"

/////////////////////////////////////////////////////
//lcd 
#define COMMAND_4_BIT_MODE 0x28 //0010 1000
#define RS_PIN 0
#define RW_PIN 1
#define E_PIN 2
#define COMMAND_DISPLAY_ON_OFF_BIT 2 //0000 0010
#define COMMAND_CLEAR_DISPLAY 0x01 //0000 0001
/////////////////////////////////////////////////////
FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, UART0_receive, _FDEV_SETUP_READ);



// TWI(I2C) 통신 초기화 함수
void TWI_Init() {
	// TWI 클럭 주파수 설정 (SCL_freq = CPU_clk / (16 + 2 * TWBR * Prescaler))
	
	TWSR = (0<<TWPS0) | (0<<TWPS1);
	TWBR = 72;  // TWBR 값 설정 (주파수를 조절하여 원하는 클럭 주파수 설정)  100khz
	_delay_ms(10);  //setup bus free time
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

/////////////////////////////////////////////////////////
//lcd control source code
void LCD_init(void){
	
	_delay_ms(10);
	
	/* step1 : start */
	TWI_Write(0x00);  //초기화 목적으로 모두 low을 인가함.
	/* step2 : address send */
	again_write_address();
	
	TWI_Write(0x00); //R/W 핀으로 low를 출력하여 쓰기 전용으로 사용
	
	again_write_address();
	
	LCD_write_command(0x02); //4 bits mode instruction
	//again_write_address();
	LCD_write_command(COMMAND_4_BIT_MODE);  // 4 bits mode
	//again_write_address();
	//display on/off control
	//화면 on, 커서 off, 깜박임 off
	unsigned char command = 0x08 | (1 << COMMAND_DISPLAY_ON_OFF_BIT);
	LCD_write_command(command);
	//again_write_address();
	
	LCD_clear();  //화면 지움
	
	//entry mode set
	//출력 후 커서를 오른쪽으로 옮김. DDRAM의 주소가 증가하며 화면 이동은 없음.
	LCD_write_command(0x06);
	
	
}

void LCD_pulse_enable(void){
	TWI_Write(0x04);//0000 0010 (falling edge) high
	
	again_write_address();
	
	TWI_Write(0x00); //0000 0000 (falling edge) low
	
	again_write_address();
	
	_delay_ms(1);
}

void LCD_write_command(unsigned char command){
	
	TWI_Write(0x00); //명령어 실행 : RS = 0
	
	again_write_address();
	
	TWI_Write(command & 0xF0); //상위 4bits
	
	again_write_address();
	
	LCD_pulse_enable();
	
	TWI_Write( (command << 4) & 0xF0); //하위 4bits
	
	again_write_address();
	
	LCD_pulse_enable();
	
	
	_delay_ms(2);
}

void LCD_clear(void){
	LCD_write_command(COMMAND_CLEAR_DISPLAY);   //command = 0x01
	_delay_ms(2);
}

void LCD_write_data(unsigned char data){
	
	TWI_Write(0x01); //문자 출력에는 RS는 1
	
	again_write_address();
	
	TWI_Write(data & 0xF0);
	
	again_write_address();
	
	LCD_pulse_enable();
	
	TWI_Write( (data<<4) & 0xF0 );
	LCD_pulse_enable();
	
	_delay_ms(2);
}

void again_write_address(void){
	//데이터를 연속적으로 보내기 위해서는 주소를 다시 써야 함.////////
	TWI_Start();
	TWI_Write(0x40);  //slave address, write mode (0100 0000)
	/////////////////////////////////////////////////////////////
}


///////////////////////////////////////////////////////////////////////////////////


int main() {
	
	
	stdout = &OUTPUT;
	stdin = &INPUT;
	
	/*uart*/
	UART0_init();
	
	DDRA = 0xff;   //output(power 5v, backlight_lcd)
	DDRD = 0xff;   //output(SDA, SCL), twi
	DDRB = 0xff; //debugging
	
	PORTA = 0x03;  // 0000 0011   (power 5v, backlight_lcd)
	PORTD = 0xff;  // 1111 1111
	
	TWI_Init();
	TWI_Start();
	//PORTB = 0x01;
	TWI_Write(0x40);  //address, write mode (0100 0000)
	//PORTB = 0x02;
	
	
	//express lcd letter
	LCD_init();
	//PORTB = 0x04;
	
	_delay_ms(100);
	
	LCD_clear();
	
	//PORTB = 0x08;
	LCD_write_data('A');
	
	
	
	//PORTB = 0x10;
	
	while(1){
		
	}
		
	//
	//TWI_Stop();
	
	
	return 0;
}
