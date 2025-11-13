int count = 0;

void setup() {
  Serial.begin(9600);

  Serial.println();
  Serial.println("21114073 차경호");
}

void loop() {
  Serial.print("The number is ");
  Serial.println(count);
  count++;
  delay(500);
}
