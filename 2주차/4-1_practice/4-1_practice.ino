int red1 = 10;
int yellow1 = 9;
int green1 = 8;

int red2 = 13;
int yellow2 = 12;
int green2 = 11;

void setup() {
  pinMode(red1, OUTPUT);
  pinMode(yellow1, OUTPUT);
  pinMode(green1, OUTPUT);
  
  pinMode(red2, OUTPUT);
  pinMode(yellow2, OUTPUT);
  pinMode(green2, OUTPUT);
}

void loop() {
  changeLights();
  delay(1500);
}

void changeLights() {
  // 노란 신호를 둘 다 켠다.
  digitalWrite(green1, LOW);
  digitalWrite(yellow1, HIGH);
  digitalWrite(yellow2, HIGH);
  delay(500);

  // 노란 신호등 둘 다 끄고, 빨간 신호등과 건너 편에
  // 녹색 신호등을 켠다. digitalWrite(yellow1, LOW); digitalWrite(red1, HIGH);
  digitalWrite(yellow2, LOW);
  digitalWrite(red2, LOW);
  digitalWrite(green2, HIGH);
  delay(500);

  // 노란 신호등 둘 다 다시 켠다.
  digitalWrite(yellow1, HIGH);
  digitalWrite(yellow2, HIGH);
  digitalWrite(green2, LOW);
  delay(300);

  // 노란 신호등 둘 다 끄고, 빨간색과 건너 편에 녹색 신호등을 켠다.
  digitalWrite(green1, HIGH);
  digitalWrite(yellow1, LOW);
  digitalWrite(red1, LOW);
  digitalWrite(yellow2, LOW);
  digitalWrite(red2, HIGH);
  delay(500);
}