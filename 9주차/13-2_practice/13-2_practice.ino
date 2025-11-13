#include <SoftwareSerial.h>

SoftwareSerial mySerial(8, 9);

int LED = 13;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(LED, OUTPUT);
}

void loop() {
  if (mySerial.available()) {
    int com = mySerial.read();
    if (com == 'o') {
      digitalWrite(LED, LOW);
    } else if (com == 'x') {
      digitalWrite(LED, HIGH);
    }
  }
}
