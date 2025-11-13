#include <SoftwareSerial.h>

SoftwareSerial mySerial(8, 7);

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
}

byte rx_byte = 0;

void loop() {
  if (Serial.available()) {
    rx_byte = Serial.read();
    mySerial.write(rx_byte);
  }

  if (mySerial.available()) {
    rx_byte = mySerial.read();
    Serial.write(rx_byte);
  }
}
