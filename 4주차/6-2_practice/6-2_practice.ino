int analog = A0;
int led1 = 2;
int led2 = 3;
int led3 = 4;

void setup() {
  Serial.begin(9600);
  pinMode(analog, INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
}

void loop() {
  int valBefore = analogRead(analog);
  int val = map(valBefore, 0, 1023, 1, 3);
  Serial.print("값: ");
  Serial.println(val);
  if (val == 1) {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
  }
  if (val == 2) {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, LOW);
  }
  if (val == 3) {
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, HIGH);
  }
}
