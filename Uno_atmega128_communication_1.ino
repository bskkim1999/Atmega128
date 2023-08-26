
//This file is arduinoUno code file.
void setup() {
    Serial.begin(9600);
    pinMode(2, OUTPUT);
}

void loop() {
    
    if (Serial.available() >= 2) { // 최소 2바이트 데이터 수신
        unsigned char lowByte = Serial.read(); //1byte
        unsigned char highByte = Serial.read(); //1byte
        
        int receivedValue = ((int)highByte << 8) | lowByte; // 재조합, 2byte
        Serial.println(receivedValue);

        if (receivedValue == 300) {
            digitalWrite(2, HIGH); // LED 켜기
            
        }
    }

    
}
