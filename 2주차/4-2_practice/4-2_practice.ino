// DigitalReadSerial
// 핀 2의 디지털 입력을 읽고, 그 결과를 시리얼 모니터에 출력한다.

int pushButton=2;

void setup() {
  Serial.begin(9600);
  pinMode(pushButton, INPUT);
}

void loop() {
  // 입력 핀을 읽는다:
  int buttonState = digitalRead(pushButton);
  // 버튼의 상태를 출력한다.
  Serial.println(buttonState);
  delay(10);
}