int RGB_LED[] = { 9, 10, 11 };
int LOWER = 27;
int UPPER = 29;

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 3; i++)
    pinMode(RGB_LED[i], OUTPUT);
}

void loop() {
  int reading = analogRead(A1);
  float voltage = reading * 5.0 / 1024.0;
  Serial.print(voltage);
  Serial.print(" V : ");
  float temperatureC = voltage * 100;
  Serial.print(temperatureC);
  Serial.print(" C. ");
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  Serial.print(temperatureF);
  Serial.print(" F.");
  if (temperatureC < LOWER) {
    Serial.println("파란색 켬");
    digitalWrite(RGB_LED[0], LOW);
    digitalWrite(RGB_LED[1], LOW);
    digitalWrite(RGB_LED[2], HIGH);
  }
  else if (temperatureC > UPPER) {
    Serial.println("빨간색 켬");
    digitalWrite(RGB_LED[0], HIGH);
    digitalWrite(RGB_LED[1], LOW);
    digitalWrite(RGB_LED[2], LOW);
  }
  else {
    Serial.println("녹색 켬");
    digitalWrite(RGB_LED[0], LOW);
    digitalWrite(RGB_LED[1], HIGH);
    digitalWrite(RGB_LED[2], LOW);
  }
}
