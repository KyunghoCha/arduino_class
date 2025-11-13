int Rvalue = 254, Gvalue = 1, Bvalue = 127;
int Rdirection = -1, Gdirection = 1, Bdirection = -1;

int Red = 9, Green = 10, Blue = 11;

void setup() {
}

void loop() {
  analogWrite(Red, Rvalue);
  analogWrite(Green, Gvalue);
  analogWrite(Blue, Bvalue);

  Rvalue = Rvalue + Rdirection;
  Gvalue = Gvalue + Gdirection;
  Bvalue = Bvalue + Bdirection;

  if (Rvalue >= 255 || Rvalue <= 0) Rdirection = Rdirection * -1;
  if (Gvalue >= 255 || Gvalue <= 0) Gdirection = Gdirection * -1;
  if (Bvalue >= 255 || Bvalue <= 0) Bdirection = Bdirection * -1;
  delay(10);
}
