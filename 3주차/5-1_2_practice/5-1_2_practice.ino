int RGB_LED[] = { 6, 7, 8 };

void setup() {
  for (int i = 0; i < 3; i++)
    pinMode(RGB_LED[i], OUTPUT);
  Serial.begin(9600);
}

void set_led(char *title, int r_flag, int g_flag, int b_flag) {
  Serial.println(title);
  digitalWrite(RGB_LED[0], r_flag);
  digitalWrite(RGB_LED[1], g_flag);
  digitalWrite(RGB_LED[2], b_flag);
  delay(500);
}

void loop() {
  // set_led("빨간색 켜짐", HIGH, LOW, LOW);
  // set_led("녹색 켜짐", LOW, HIGH, LOW);
  // set_led("파란색 켜짐", LOW, LOW, HIGH);
  // set_led("노란색 켜짐", HIGH, HIGH, LOW);
  // set_led("자주색 켜짐", HIGH, LOW, HIGH);
  // set_led("청록색 켜짐", LOW, HIGH, HIGH);
  // set_led("흰색 켜짐", HIGH, HIGH, HIGH);
  // set_led("꺼짐", LOW, LOW, LOW);

  set_led("빨간색 켜짐", HIGH, LOW, LOW);
  set_led("노란색 켜짐", HIGH, HIGH, LOW);
  set_led("녹색 켜짐", LOW, HIGH, LOW);
  set_led("꺼짐", LOW, LOW, LOW);
}
