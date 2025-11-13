const int SW_pin = 2;
const int X_pin = A0;
const int Y_pin = A1;
int redPin = 9;
int greenPin = 10;
int bluePin = 11;
int XPIN;
int YPIN;
int SWPIN;

void setup() {
  pinMode(SW_pin, INPUT);
  digitalWrite(SW_pin, HIGH);
  Serial.begin(9600);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}

void setColor(int red, int green, int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}

void loop() {
  int YPIN = analogRead(X_pin);
  int SWPIN = analogRead(SW_pin);
  Serial.print("스위치: ");
  Serial.print(digitalRead(SW_pin));
  Serial.print(" X-축:");
  Serial.print(digitalRead(X_pin));
  Serial.print(" ");
  Serial.print("Y-축:");
  Serial.print(digitalRead(Y_pin));
  if (YPIN == 0) { setColor(100, 80, 0); delay(100); }
  else setColor(0, 0, 0);
  if (YPIN == 1023) { setColor(255, 0, 255); delay(100); }
  else setColor(0, 0, 0);
  if (XPIN == 0) { setColor(0, 0, 255); delay(100); }
  else setColor(0, 0, 0);
  if (XPIN == 1023) { setColor(0, 255, 0); delay(100); }
  else setColor(0, 0, 0);
  if (SWPIN == LOW) { setColor(80, 20, 0); delay(100); }
  else setColor(0, 0, 0);
}