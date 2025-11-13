int latch = 8;
int clock = 12;
int data = 11;

void setup() {
  pinMode(latch, OUTPUT);
  pinMode(clock, OUTPUT);
  pinMode(data, OUTPUT);
}

void loop() {
  for (int i = 0; i < 256; i++) {
    digitalWrite(latch, LOW);
    shiftOut(data, clock, MSBFIRST, B10100110);
    digitalWrite(latch, HIGH);
    delay(500);
  }
}
