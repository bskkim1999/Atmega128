//slave : UNO
#include <Wire.h>

#define SLAVE_ADDRESS 0x08  // 슬레이브 주소 0000 1000
#define clockFrequency 100000

unsigned char receivedData;


void receiveData(int byteCount) {
    
    receivedData = Wire.read(); // 데이터 수신

    Serial.println("수신");
    if (receivedData == '1') {
      Serial.println("1");
     
      digitalWrite(2, HIGH);
      digitalWrite(3, LOW);
    }

    else if(receivedData == '2'){
      Serial.println("2");
      digitalWrite(2, LOW);
      digitalWrite(3, HIGH);
    }
    
}

void setup() {
  
  Wire.begin(SLAVE_ADDRESS); // 슬레이브 모드 시작
  Wire.onReceive(receiveData); // 데이터 수신 시 처리할 함수 등록
  

  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
 
  Serial.println("Ready to receive");
}

void loop() {
    
  
}


