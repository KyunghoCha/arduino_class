void setup() {
  String my_str = "This is my string";
  Serial.begin(9600);

  Serial.println();
  Serial.println(my_str);

  my_str.toUpperCase();
  Serial.println(my_str);

  my_str = "My new string";
  Serial.println(my_str);

  my_str.replace("string", "Arduino sketch");
  Serial.println(my_str);

  int in = my_str.indexOf("in");
  Serial.println(in);

  Serial.print("스트링 길이: ");
  Serial.println(my_str.length());

  Serial.println();
  Serial.println("21114073 차경호");
}

void loop() {
  // put your main code here, to run repeatedly:

}
