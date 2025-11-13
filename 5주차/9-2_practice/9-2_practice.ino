int calibrationTime = 30;
unsigned long lowIn;
unsigned long pause = 5000;
boolean lockLow = true;
boolean takeLowTime = true;

int pirPin = 12;
int ledPin = 13;

void setup() {
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(pirPin, LOW);

  Serial.print("센서 보정 ");
  for (int i = 0; i < calibrationTime; i++) {
    Serial.print(".");
    delay(100);\
  }

  Serial.println(" 완료");
  Serial.println("센서 활성화");
  delay(50);
}

void loop() {
  if (digitalRead(pirPin) == HIGH) {
    digitalWrite(ledPin, HIGH);

    if (lockLow) {
      lockLow = false;
      Serial.print("--- ");
      Serial.print(millis() / 1000);
      Serial.println(" 초: 모션 감지!");
      delay(50);
    }

    takeLowTime = true; // 다시 LOW 감지를 허용
  }

  if (digitalRead(pirPin) == LOW) {
    digitalWrite(ledPin, LOW);

    if (takeLowTime) {
      lowIn = millis();     // LOW 신호가 처음 감지된 시간 기록
      takeLowTime = false;  // 한 번만 기록
    }

    if (!lockLow && (millis() - lowIn > pause)) {
      lockLow = true;
      Serial.print("모션 종료 ");
      Serial.print(millis() / 1000);
      Serial.println(" 초");
      delay(50);
    }
  }
}
