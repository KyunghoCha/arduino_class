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
    B00000000,
    B01100110,
    B11111111,
    B11111111,
    B01111110,
    B00111100,
    B00011000,
    B00000000
  };

  for (int i = 0; i < 8; i++){
    lc.setRow(0, i, value[i]);
    delay(500);
  }
  delay(500);
  lc.clearDisplay(0);
  delay(1000);
}
