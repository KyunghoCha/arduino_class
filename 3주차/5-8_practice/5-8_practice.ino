#include "LedControl.h"

LedControl lc = LedControl(12, 11, 10, 1);

void setup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 2);
  lc.clearDisplay(0);
}

void loop() {
  Serial.print("0fdsa");
  byte value[8] = {
    B10001000,
    B11111000,
    B10101000,
    B01110001,
    B00100001,
    B01111001,
    B01111101,
    B10111110
  };

  for (int i = 0; i < 8; i++){
    lc.setRow(0, i, value[i]);
    delay(500);
  }
  delay(500);
  lc.clearDisplay(0);
  delay(1000);
}
