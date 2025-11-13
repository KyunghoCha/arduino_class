int latch = 8;
int clock = 12;
int data = 11;

void setup() {
  pinMode(latch, OUTPUT);
  pinMode(clock, OUTPUT);
  pinMode(data, OUTPUT);
}

void loop() {
  for (int i = 0; i < 8; i++) {
    digitalWrite(latch, LOW);
    shiftOut(data, clock, MSBFIRST, 1 << i);
    digitalWrite(latch, HIGH);
    delay(500);
  }
}
