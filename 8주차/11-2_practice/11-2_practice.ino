#include <LiquidCrystal.h>

LiquidCrystal LCD(2, 3, 4, 5, 6, 7);

byte heart[8] = {
   0b00000,
   0b01010,
   0b11111,
   0b11111,
   0b11111,
   0b01110,
   0b00100,
   0b00000
};

byte smiley[8] = {
   0b00000,
   0b00000,
   0b01010,
   0b00000,
   0b00000,
   0b10001,
   0b01110,
   0b00000
};

byte armsDown[8] = {
   0b00100,
   0b01010,
   0b00100,
   0b00100,
   0b01110,
   0b10101,
   0b00100,
   0b01010
};

byte armsUp[8] = {
   0b00100,
   0b01010,
   0b00100,
   0b10101,
   0b01110,
   0b00100,
   0b00100,
   0b01010
};

void setup() {
  LCD.begin(0x3F, 16, 2);
  
  LCD.createChar(0, heart);
  LCD.createChar(1, smiley);
  LCD.createChar(2, armsDown);
  LCD.createChar(3, armsUp);
  LCD.setCursor(0, 0);
  LCD.print("I ");
  LCD.write(byte(0));
  LCD.print(" Arduino! ");
  LCD.write(byte(1));
}

void loop() {
  LCD.setCursor(4, 1);
  LCD.write(2);
  delay(500);
  LCD.setCursor(4, 1);
  LCD.write(3);
  delay(500);
}