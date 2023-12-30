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
#define RS_LOW_EN_high 0x04
#define RS_LOW_EN_low 0x00
#define RS_HIGH_EN_high 0x05
#define RS_HIGH_EN_low 0x01
#define Backlight 0x08
#define COMMAND_DISPLAY_ON_OFF_BIT 2 //0000 0010
#define COMMAND_CLEAR_DISPLAY 0x01 //0000 0001
/////////////////////////////////////////////////////
FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, UART0_receive, _FDEV_SETUP_READ);


int status[20]={0};
// TWI(I2C) 통신 초기화 함수
void TWI_Init() {
	// TWI 클럭 주파수 설정 (SCL_freq = CPU_clk / (16 + 2 * TWBR * Prescaler))
	
	DDRD = 0x03; // PD0(SCL), PD1(SDA)
	
	TWSR = (0<<TWPS0) | (0<<TWPS1); //SCL 주파수의 분주비 조절
	TWBR = 72;  // TWBR 값 설정 (주파수를 조절하여 원하는 클럭 주파수 설정)  100khz
	_delay_ms(50);  //setup bus free time
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
	
	_delay_ms(50);
	
	/*step1 : 3의 값을 3번 줌.*/
	LCD_write_command_4bit(0x30); _delay_ms(5); // 0011 0000
	status[0] = TWSR;
	
	LCD_write_command_4bit(0x30); _delay_us(100); // 0011 0000	
	status[1] = TWSR;
	
	LCD_write_command_4bit(0x30); _delay_us(100); // 0011 0000
	status[2] = TWSR;
	
	LCD_write_command_4bit(0x20); _delay_us(100); // 0011 0000
	status[3] = TWSR;
	
	/*step2 : Function set*/ //4비트, 2행, 5*8 폰트
	LCD_write_command_8bit(0x28); _delay_us(50);
	status[4] = TWSR;
	
	/*step3 : display on / off control */
	LCD_write_command_8bit(0x08); _delay_us(50);
	status[5] = TWSR;
	
	/*step4 : clean screen*/
	LCD_clear();  //화면 지움
	status[6] = TWSR;
	
	/*step5 : entry mode set */
	//출력 후 커서를 오른쪽으로 옮김. DDRAM의 주소가 증가하며 화면 이동은 없음.
	LCD_write_command_8bit(0x06);
	status[7] = TWSR;
	/*step6 : display on, cursor & cursor blink off*/
	LCD_write_command_8bit(0x0c); 
	status[8] = TWSR;
	
}


void LCD_write_command_4bit(unsigned char command){
	// 상위 4비트만 twi 전송함.
	int lcd_buf[2]={0};
		
	lcd_buf[0] = (command & 0xF0) | RS_LOW_EN_high | Backlight ;  //상위 4비트 
	lcd_buf[1] = ( command & 0xF0) | RS_LOW_EN_low | Backlight; 
	
	TWI_Write(lcd_buf[0]);  // task1
	again_write_address();
	
	TWI_Write(lcd_buf[1]);  // task2
	again_write_address();
	
	_delay_ms(2);
}

void LCD_write_command_8bit(unsigned char command){
	// 상위 4비트와 하위 4비트를 twi 전송함.
	int lcd_buf[4]={0};
	
	lcd_buf[0] = (command & 0xF0) | RS_LOW_EN_high | Backlight;  //상위 4비트
	lcd_buf[1] = ( command & 0xF0) | RS_LOW_EN_low | Backlight;
	lcd_buf[2] = ( (command << 4)  & 0xF0) | RS_LOW_EN_high | Backlight;  //하위 4비트
	lcd_buf[3] = ( (command << 4) & 0xF0) | RS_LOW_EN_low | Backlight;
	
	TWI_Write(lcd_buf[0]);  // task1
	again_write_address();
	
	TWI_Write(lcd_buf[1]);  // task2
	again_write_address();
	
	TWI_Write(lcd_buf[2]);  // task2
	again_write_address();
	
	TWI_Write(lcd_buf[3]);  // task2
	again_write_address();
	
	_delay_ms(2);
}

void LCD_clear(void){
	LCD_write_command_8bit(COMMAND_CLEAR_DISPLAY);   //command = 0x01
	_delay_ms(5);
}

void LCD_write_data(unsigned char data){
	
	// 상위 4비트와 하위 4비트를 twi 전송함.
	int lcd_buf[4]={0};
	
	lcd_buf[0] = (data & 0xF0) | RS_HIGH_EN_high | Backlight;  //상위 4비트
	lcd_buf[1] = ( data & 0xF0) | RS_HIGH_EN_low | Backlight;
	lcd_buf[2] = ( (data << 4)  & 0xF0) | RS_HIGH_EN_high | Backlight;  //하위 4비트
	lcd_buf[3] = ( (data << 4) & 0xF0) | RS_HIGH_EN_low | Backlight;
	
	TWI_Write(lcd_buf[0]);  // task1
	again_write_address();
	
	TWI_Write(lcd_buf[1]);  // task2
	again_write_address();
	
	TWI_Write(lcd_buf[2]);  // task2
	again_write_address();
	
	TWI_Write(lcd_buf[3]);  // task2
	again_write_address();
	
	
	
	
}

void again_write_address(void){
	//데이터를 연속적으로 보내기 위해서는 주소를 다시 써야 함.////////
	TWI_Start();
	TWI_Write(0x4E);  //slave address, write mode (0100 1110)
	_delay_ms(1); 
	/////////////////////////////////////////////////////////////
}

void lcd_goto_XY(uint8_t row, uint8_t col)
{
	uint8_t address = (0x40 * row) + col;
	uint8_t command = 0x80 | address;
	
	LCD_write_command_8bit(command);
}
