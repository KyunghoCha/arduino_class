#include <SoftwareSerial.h>

SoftwareSerial mySerial(8, 7);

char Incoming_value = 0;

void setup() {
  mySerial.begin(9600);
  Serial.begin(9600);
  pinMode(13, OUTPUT);
}

void loop() {
  if (mySerial.available() > 0) {
    Incoming_value = mySerial.read();
    Serial.println(Incoming_value);

    if (Incoming_value == '1')
      digitalWrite(13, HIGH);
    else if (Incoming_value == '0')
      digitalWrite(13, LOW);
  }
}
