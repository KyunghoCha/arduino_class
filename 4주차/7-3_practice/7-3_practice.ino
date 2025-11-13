#include "LedControl.h"
#define joyX A0
#define joyY A1

int xMap, yMap, xValue, yValue;
LedControl lc = LedControl(12, 11, 10, 1);

void setup() {
  Serial.begin(115200);

  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
}

void loop() {
  xValue = analogRead(joyX);
  yValue = analogRead(joyY);
  xMap = map(xValue, 0, 1023, 0, 7);
  yMap = map(yValue, 0, 1023, 7, 0);
  lc.setLed(0, xMap, yMap, true);
  lc.clearDisplay(0);
}
