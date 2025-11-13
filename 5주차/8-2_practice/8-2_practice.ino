int led = 3;
float BD_TH = 2.5;

void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);
}

void loop() {
  int reading = analogRead(A2);
  Serial.print("ADC: ");
  Serial.print(reading);
  float voltage = reading * 5.0 / 1024.0;
  Serial.print(", Voltage: ");
  Serial.print(voltage);
  if (voltage < BD_TH) {
    digitalWrite(led, HIGH);
    Serial.println("어두움->LED켬");
  }
  else {
    digitalWrite(led, LOW);
    Serial.println("밝음->LED 끔");
  }
  delay(500);
}
