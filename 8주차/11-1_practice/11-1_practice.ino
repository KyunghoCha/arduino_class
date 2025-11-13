#include <LiquidCrystal.h>

LiquidCrystal LCD(2, 3, 4, 5, 6, 7);

void setup() {
  LCD.begin(0x3F, 16, 2);
  LCD.print("Hello Ardunio!");
  LCD.setCursor(6, 1);
  LCD.print("LCD 1602");
}

void loop() {
  LCD.setCursor(0, 1);
  LCD.print(millis()/1000);
}