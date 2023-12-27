void custom_delay(int b){

	for (int x=0 ; x < b ; x ++){
		_delay_ms(1);
	}

}

void speaker_tone(int freq, int duration_ms){
	
	int temp, temp2;
	
	//Top
	temp = (16000000/freq-64)*0.015625;  //소수점은 버려진다. temp가 int형이기 때문이다. (정수형)
	
	//duty-cycle
	temp2 = temp/2;
	OCR1A=temp2; //50%, 10진수 = ?
	
	//Top
	ICR1= temp;
	
	
	custom_delay(duration_ms);
}

void speaker_tone_stop(void){
	
	ICR1 = 0;  //소리가 안나게 하기 위해서 top을 0으로 만들어버림.
	
	
}

void speaker_tone_init(void){
	DDRB = 0x20; //0010 0000
	
	//모드 : 10 bit fast pwm, non-inverting mode
	TCCR1B |= (1 << WGM13) |(1 << WGM12);
	TCCR1A |= (1 << WGM11) | (0 << WGM10) | (1<<COM1A1) | (0 << COM1A0);
	
	//pre-scaler
	TCCR1B |= (0<<CS12) | (1<<CS11) | (1<<CS10);  //pre-scaler : 64
	
	
}

//Test how to control speaker
int main(void){
	stdout = &OUTPUT;
	stdin = &INPUT;
	
	UART0_init();
	
	speaker_tone_init();
	speaker_tone(262, 500);  //도
	speaker_tone(294, 500);  //레
	speaker_tone(330, 500);  //미
	speaker_tone(349, 500);  //파
	speaker_tone(392, 500);  //솔
	speaker_tone_stop();
	
	while(1);
	
	return 0;
}
