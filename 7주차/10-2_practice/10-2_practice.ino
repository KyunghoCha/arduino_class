int Enable1 = 10;     // 38
int PWM1 = 9;
int DIR1 = 11;        // 37
int button_pin = 12;  // 14
boolean direction = true;

void setup() {
  pinMode(Enable1, OUTPUT);
  pinMode(PWM1, OUTPUT);
  pinMode(DIR1, OUTPUT);
  digitalWrite(Enable1, HIGH);
  digitalWrite(DIR1, direction);
  pinMode(button_pin, INPUT);
  Serial.begin(9600);
}

void loop() {
  int speed = analogRead(A0);
  speed = map(speed, 0, 1023, 0, 255);
  analogWrite(PWM1, speed);
  if (digitalRead(button_pin)) {
    direction = !direction;
    if (direction) {
      Serial.print("반시계 방향: ");
      Serial.println(speed);
    } else {
      Serial.print("시계 방향: ");
      Serial.println(speed);
    }
    digitalWrite(DIR1, direction);
    delay(2000);
  }
}
