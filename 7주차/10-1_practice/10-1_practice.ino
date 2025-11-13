#include <Servo.h>

Servo myServo;
int servoPin = 11;

void setup() {
  myServo.attach(servoPin);
}

void loop() {
  int value = analogRead(A0);
  int angle = map(value, 0, 1023, 0, 180);
  myServo.write(angle);
}
