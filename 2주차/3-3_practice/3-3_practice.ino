void setup() {
  Serial.begin(9600);
}

void loop() {
  String stringOne = "대구가톨릭대학교";
  String stringTwo = "소프트웨어융합대학";
  int num1 = 12345;

  Serial.println();
  Serial.println(stringOne);
  Serial.println(stringOne + " " + stringTwo);

  Serial.println(String(num1));
  Serial.println(String(num1, BIN));
  Serial.println(String(num1, HEX));
  String stringThree = String(5.6789, 2);
  Serial.println(stringThree);

  Serial.println();
  Serial.println("21114073 차경호");

  while(true);
}