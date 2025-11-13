int pirPin = 2;
int ledPin = 13;
int buzPin = 3;

void setup() {
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buzPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int val = digitalRead(pirPin);
  
  if (val == HIGH) {
    Serial.print("모션 감지! ");
    Serial.println(val);

    tone(buzPin, 1200);
    digitalWrite(ledPin, HIGH);
    delay(150);
    digitalWrite(ledPin, LOW);
    noTone(buzPin);
    delay(150);
  } else {
    Serial.print("모션 감지 없음! ");
    Serial.println(val);

    digitalWrite(ledPin, LOW);
    noTone(buzPin);
    delay(150);
  }
}
