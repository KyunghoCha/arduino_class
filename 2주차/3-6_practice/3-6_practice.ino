void setup() {
  Serial.begin(9600);

  String str = "Welcome to APPSKIT!";

  Serial.println();

  str.toUpperCase();
  Serial.println(str);
  
  str.toLowerCase();
  Serial.println(str);

  Serial.println();
  Serial.println("21114073 차경호");
}

void loop() {
  // put your main code here, to run repeatedly:

}
