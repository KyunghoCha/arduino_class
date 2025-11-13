int segmentLEDs[] = { 2, 3, 4, 5, 6, 7, 8, 9 };
int segmentLEDsNum = 7;

int digitForNum[9][7] = {
  { 0, 1, 1, 0, 0, 0, 0 }, // 1
  { 1, 1, 0, 1, 1, 0, 1 }, // 2
  { 1, 1, 1, 1, 0, 0, 1 }, // 3
  { 0, 1, 1, 0, 0, 1, 1 }, // 4
  { 1, 0, 1, 1, 0, 1, 1 }, // 5
  { 1, 0, 1, 1, 1, 1, 1 }, // 6
  { 1, 1, 1, 0, 0, 0, 0 }, // 7
  { 1, 1, 1, 1, 1, 1, 1 }, // 8
  { 1, 1, 1, 1, 0, 1, 1 }, // 9
};

void setup() {
  for (int i = 0; i < segmentLEDsNum; i++)
    pinMode(segmentLEDs[i], OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    char n = Serial.read();
    int num = n - '0' - 1;
    for (int i = 0; i < segmentLEDsNum; i++)
      digitalWrite(segmentLEDs[i], digitForNum[num][i]);

    for (int i = 0; i < 7; i++){
      int state = digitForNum[num][i];
      Serial.println(state);
      // ?
    }
  }
  delay(200);
}
