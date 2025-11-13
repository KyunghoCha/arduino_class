#include "LedControl.h"

LedControl lc = LedControl(12, 11, 10, 1);

unsigned long delaytime = 1000;

byte hf[8] = {
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10100101,
  B10011001,
  B01000010,
  B00111100
};

byte nf[8] = {
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10111101,
  B10000001,
  B01000010,
  B00111100
};

byte sf[8] = {
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10011001,
  B10100101,
  B01000010,
  B00111100
};

void display_face(byte *face) {
  for (int i = 0; i < 8; i++)
    lc.setRow(0, i, face[i]);
  delay(delaytime);
}

void setup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 2);
  lc.clearDisplay(0);
}

void loop() {
  display_face(hf);
  display_face(nf);
  display_face(sf);
}
