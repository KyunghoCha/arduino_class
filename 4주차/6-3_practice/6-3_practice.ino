int redPin = 3;
int greenPin = 5;
int bluePin = 6;

int redControl = A0;
int greenControl = A1;
int blueControl = A2;

int redVal;
int greenVal;
int blueVal;

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  redVal = analogRead(redControl);
  greenVal = analogRead(greenControl);
  blueVal = analogRead(blueControl);

  redVal = map(redVal, 0, 1023, 0, 255);
  greenVal = map(greenVal, 0, 1023, 0, 255);
  blueVal = map(blueVal, 0, 1023, 0, 255);

  setColor(redVal, greenVal, blueVal);

  Serial.print(" - Red: ");
  Serial.println(redVal);
  Serial.print(" - Green: ");
  Serial.println(greenVal);
  Serial.print(" - Blue: ");
  Serial.println(blueVal);
}

void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(redPin, redVal);
  analogWrite(greenPin, greenVal);
  analogWrite(bluePin, blueVal);
}
