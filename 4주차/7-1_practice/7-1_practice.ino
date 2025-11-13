#include <Keypad.h>

const int ROW_NUM = 4;
const int COLUMN_NUM = 4;
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte pin_rows[ROW_NUM] = { 8, 7, 6, 5 };
byte pint_column[COLUMN_NUM] = { 4, 3, 2, 1 };
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pint_column, ROW_NUM, COLUMN_NUM);

void setup() {
  Serial.begin(9600);
}

void loop() {
  char key = keypad.getKey();

  if (key) Serial.println(key);
}
