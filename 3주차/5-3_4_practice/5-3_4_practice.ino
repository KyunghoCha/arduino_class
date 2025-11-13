int segmentLEDs[] = { 2, 3, 4, 5, 6, 7, 8, 9 };
int segmentLEDsNum = 8;

int digitForNum[9][7] = {
  { 0, 1, 1, 0, 0, 0, 0, 0 }, // 0
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
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < segmentLEDsNum; j++)
      digitalWrite(segmentLEDs[j], digitForNum[i][j]);
    delay(500);
  }
}
