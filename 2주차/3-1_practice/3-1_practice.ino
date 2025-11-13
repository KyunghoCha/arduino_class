int count = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.print(count); Serial.print("\t");
  Serial.print(count, HEX); Serial.print ("\t");
  Serial.print(count, OCT); Serial.print("\t");
  Serial.print(count, BIN); Serial.print("\t");
  Serial.println("초");
  delay(1000);
  count++;
}