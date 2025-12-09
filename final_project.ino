#include <Arduino_FreeRTOS.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

// --- OLED 설정 ---
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// FreeRTOS 틱 변환
#define MS2TICKS(ms) ((ms) / portTICK_PERIOD_MS)

// --- 핀 정의 ---
#define TRIG    4
#define ECHO    5
#define JSWITCH 6
#define BUZZER  7
#define RGB_B   8
#define RGB_G   9
#define RGB_R   10
#define JX      A1
#define JY      A0

// --- HUD / 플레이 영역 ---
#define HUD_HEIGHT  8
#define PLAY_TOP    HUD_HEIGHT
#define PLAY_BOTTOM (SCREEN_HEIGHT - 1)
#define PLAY_HEIGHT (SCREEN_HEIGHT - HUD_HEIGHT)

// --- 데이터 구조체 ---
typedef struct {
  int x, y, btn;
} JoystickData;

typedef struct {
  float distance; // cm
} UltraData;

// --- 전역 상태 변수 ---
volatile JoystickData joyData = { 512, 512, 0 };
volatile UltraData ultraData  = { 0 };

// 점 중심 좌표 및 속도 관련
#define BASE_VMAX 4.0
#define DISTANCE_SMOOTH_FACTOR 0.15

volatile float currentCenterX = SCREEN_WIDTH  / 2.0;
volatile float currentCenterY = SCREEN_HEIGHT / 2.0;
volatile float smoothedDistance = 0.0;

// 점/벌어짐 설정
#define MAX_GAP_PIX 50
#define DOT_RADIUS  2.0

// 게임 상태
volatile bool gameOver = false;
volatile bool restartRequested = false;

// -----------------------------
// 점수 / EEPROM
// -----------------------------
volatile int score = 10;
uint16_t highScore = 0;
#define EEPROM_ADDR_HIGHSCORE 0

// -----------------------------
// 난이도
// -----------------------------
enum Difficulty : uint8_t { EASY=0, NORMAL=1, HARD=2 };
volatile Difficulty currentDifficulty = EASY;

// 난이도 전환 기준
#define SCORE_TO_NORMAL 10
#define SCORE_TO_HARD   30

// -----------------------------
// 장애물 설정
// -----------------------------
#define WALL_WIDTH      5
#define GAP_HEIGHT      15
#define GAP_COUNT       2
#define MIN_GAP_SPACING 5
#define BASE_WALL_SPEED 2.0

// ★ 동시에 여러 개 등장 방지: 무조건 1개만 사용
#define MAX_WALLS 1

// --- 이동하는 구멍(상하 이동) 옵션 ---
#define MOVING_WALL_CHANCE 20     // % 확률로 moving wall
#define MOVING_GAP_VEL     0.6f   // 구멍 상하 이동 속도

typedef struct {
  float x;

  // 기본 구멍 위치(플레이 영역 기준)
  int baseGapY1Local;
  int baseGapY2Local;

  // 실제 사용 위치
  int gapY1Local;
  int gapY2Local;

  // 상하 이동용
  float gapOffset;
  float gapVel;
  bool  moving;

  bool scored;
} Wall;

Wall walls[MAX_WALLS];

// -----------------------------
// 코인 설정
// -----------------------------
#define COIN_RADIUS 1.5
#define COIN_BONUS  1

typedef struct {
  float x;
  float y;
  bool active;
} Coin;

Coin coin = {0, 0, false};

// -----------------------------
// 유틸
// -----------------------------
static inline float clampf(float v, float lo, float hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

Difficulty calcDifficultyFromScore(int s) {
  if (s >= SCORE_TO_HARD) return HARD;
  if (s >= SCORE_TO_NORMAL) return NORMAL;
  return EASY;
}

/*
  난이도 설계 변경
  - 벽 개수 증가 제거
  - 대신 "벽 재등장 간격(스폰 간격)"을 난이도에 따라 줄여 빈도 증가
*/
void getDifficultyParams(
  Difficulty d,
  float &wallMul,
  float &vmaxMul,
  int &coinChancePerThousand,
  int &spawnGapMin,
  int &spawnGapMax
) {
  switch (d) {
    case EASY:
      wallMul = 1.0;
      vmaxMul = 1.0;
      coinChancePerThousand = 6;   // 0.6%

      // 벽이 다시 나오는 "추가 오프셋" 범위(픽셀)
      spawnGapMin = 35;
      spawnGapMax = 55;
      break;

    case NORMAL:
      wallMul = 1.15;
      vmaxMul = 1.1;
      coinChancePerThousand = 9;  // 0.9%

      spawnGapMin = 25;
      spawnGapMax = 45;
      break;

    case HARD:
    default:
      wallMul = 1.3;              // 속도는 과하지 않게
      vmaxMul = 1.2;
      coinChancePerThousand = 12; // 1.2%

      // 빈도 체감 핵심
      spawnGapMin = 18;
      spawnGapMax = 35;
      break;
  }

  if (spawnGapMax < spawnGapMin) {
    spawnGapMax = spawnGapMin;
  }
}

// -----------------------------
// 벽 구멍 생성 (플레이 영역 기준)
// -----------------------------
void generateWallGaps(Wall &w) {
  int maxRandomY1 = PLAY_HEIGHT - (GAP_HEIGHT * GAP_COUNT) - MIN_GAP_SPACING;
  if (maxRandomY1 < 0) maxRandomY1 = 0;

  w.baseGapY1Local = random(0, maxRandomY1 + 1);

  int gap1_end = w.baseGapY1Local + GAP_HEIGHT;
  int y2_start_min = gap1_end + MIN_GAP_SPACING;
  int y2_start_max = PLAY_HEIGHT - GAP_HEIGHT;

  if (y2_start_min > y2_start_max) {
    w.baseGapY2Local = y2_start_max;
  } else {
    w.baseGapY2Local = random(y2_start_min, y2_start_max + 1);
  }

  // 초기 실제 위치
  w.gapOffset  = 0;
  w.gapY1Local = w.baseGapY1Local;
  w.gapY2Local = w.baseGapY2Local;
}

void respawnWall(Wall &w, float xStart) {
  w.x = xStart;
  generateWallGaps(w);
  w.scored = false;

  // 일정 확률로 상하 이동 벽
  w.moving = (random(0, 100) < MOVING_WALL_CHANCE);

  if (w.moving) {
    // 방향 랜덤
    w.gapVel = (random(0, 2) == 0) ? MOVING_GAP_VEL : -MOVING_GAP_VEL;
  } else {
    w.gapVel = 0;
  }
}

void initWalls() {
  // 항상 1개만 사용
  respawnWall(walls[0], SCREEN_WIDTH + 40);
}

// 점이 구멍 안에 완전히 들어왔는지
bool dotWithinGap(float dotY, int gapTopAbs) {
  return (dotY - DOT_RADIUS >= gapTopAbs) &&
         (dotY + DOT_RADIUS <= gapTopAbs + GAP_HEIGHT);
}

// -----------------------------
// 코인
// -----------------------------
void resetCoin() {
  coin.active = false;
  coin.x = 0;
  coin.y = 0;
}

void trySpawnCoin(int chancePerThousand) {
  if (coin.active) return;
  int r = random(0, 1000);
  if (r < chancePerThousand) {
    coin.active = true;
    coin.x = SCREEN_WIDTH - 1;

    float minY = PLAY_TOP + COIN_RADIUS + 1;
    float maxY = PLAY_BOTTOM - COIN_RADIUS - 1;
    if (maxY < minY) {
      coin.y = PLAY_TOP + PLAY_HEIGHT / 2.0;
    } else {
      coin.y = random((int)minY, (int)maxY + 1);
    }
  }
}

// -----------------------------
// 조이스틱 Task
// -----------------------------
void joyStick(void *p) {
  pinMode(JSWITCH, INPUT_PULLUP);

  bool lastBtn = false;

  for (;;) {
    joyData.x = analogRead(JX);
    joyData.y = analogRead(JY);
    joyData.btn = !digitalRead(JSWITCH);

    bool btnNow = joyData.btn;

    // 게임 오버 상태에서 버튼 눌림 엣지로 재시작 요청
    if (gameOver && btnNow && !lastBtn) {
      restartRequested = true;
    }

    lastBtn = btnNow;
    vTaskDelay(MS2TICKS(50));
  }
}

// -----------------------------
// 초음파 Task
// -----------------------------
void ultraSound(void *p) {
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  for (;;) {
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);

    long duration = pulseIn(ECHO, HIGH, 30000);
    float distance_cm = duration * 0.0343 / 2.0;

    if (distance_cm > 30.0) distance_cm = 30.0;
    if (distance_cm < 0.0)  distance_cm = 0.0;

    ultraData.distance = distance_cm;
    vTaskDelay(MS2TICKS(150));
  }
}

// -----------------------------
// RGB Task
// -----------------------------
void rgbTask(void *p) {
  pinMode(RGB_R, OUTPUT);
  pinMode(RGB_G, OUTPUT);
  pinMode(RGB_B, OUTPUT);

  for (;;) {
    if (gameOver) {
      // 게임 오버 시 빨간색 깜빡
      analogWrite(RGB_R, 255);
      analogWrite(RGB_G, 0);
      analogWrite(RGB_B, 0);
      vTaskDelay(MS2TICKS(250));
      analogWrite(RGB_R, 0);
      analogWrite(RGB_G, 0);
      analogWrite(RGB_B, 0);
      vTaskDelay(MS2TICKS(250));
      continue;
    }

    Difficulty d = currentDifficulty;

    if (d == EASY) {
      analogWrite(RGB_R, 0);
      analogWrite(RGB_G, 0);
      analogWrite(RGB_B, 255);   // 파랑
    } else if (d == NORMAL) {
      analogWrite(RGB_R, 255);
      analogWrite(RGB_G, 255);
      analogWrite(RGB_B, 0);     // 노랑
    } else {
      analogWrite(RGB_R, 255);
      analogWrite(RGB_G, 0);
      analogWrite(RGB_B, 0);     // 빨강
    }

    vTaskDelay(MS2TICKS(50));
  }
}

// -----------------------------
// 부저 Task
// -----------------------------
void buzzerTask(void *p) {
  pinMode(BUZZER, OUTPUT);

  for (;;) {
    if (!gameOver && joyData.btn) {
      tone(BUZZER, 1000, 10);
    } else if (gameOver) {
      noTone(BUZZER);
    }
    vTaskDelay(MS2TICKS(50));
  }
}

// -----------------------------
// 게임 리셋
// -----------------------------
void resetGameState() {
  gameOver = false;
  restartRequested = false;

  score = 0;
  smoothedDistance = 0.0;

  currentCenterX = SCREEN_WIDTH / 2.0;
  currentCenterY = PLAY_TOP + PLAY_HEIGHT / 2.0;

  currentDifficulty = EASY;

  initWalls();
  resetCoin();
}

// -----------------------------
// OLED Task (메인 루프)
// -----------------------------
void oledTask(void *p) {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  currentCenterX = SCREEN_WIDTH / 2.0;
  currentCenterY = PLAY_TOP + PLAY_HEIGHT / 2.0;

  for (;;) {
    if (restartRequested) {
      resetGameState();
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    if (!gameOver) {
      // 1) 난이도 파라미터 계산
      currentDifficulty = calcDifficultyFromScore(score);

      float wallMul, vmaxMul;
      int coinChance;
      int spawnGapMin, spawnGapMax;
      getDifficultyParams(currentDifficulty, wallMul, vmaxMul, coinChance, spawnGapMin, spawnGapMax);

      float wallSpeed = BASE_WALL_SPEED * wallMul;
      float vmax = BASE_VMAX * vmaxMul;

      // 2) 초음파 스무딩 및 점 간격
      smoothedDistance += (ultraData.distance - smoothedDistance) * DISTANCE_SMOOTH_FACTOR;
      smoothedDistance = clampf(smoothedDistance, 0.0, 30.0);

      float gapPix = (smoothedDistance / 30.0) * MAX_GAP_PIX;
      gapPix = clampf(gapPix, 0.0, (float)MAX_GAP_PIX);

      // 3) 조이스틱 속도(float)
      float vx = ((joyData.x - 512) / 512.0) * vmax;
      float vy = ((joyData.y - 512) / 512.0) * vmax;

      if (abs(joyData.x - 512) < 50) vx = 0;
      if (abs(joyData.y - 512) < 50) vy = 0;

      // 4) 중심 이동
      currentCenterX += vx;
      currentCenterY += vy;

      // 5) 경계(플레이 영역)
      float minX = DOT_RADIUS;
      float maxX = SCREEN_WIDTH - 1 - DOT_RADIUS;

      float minY = PLAY_TOP + DOT_RADIUS + gapPix / 2.0;
      float maxY = PLAY_BOTTOM - DOT_RADIUS - gapPix / 2.0;

      currentCenterX = clampf(currentCenterX, minX, maxX);
      currentCenterY = clampf(currentCenterY, minY, maxY);

      // 6) 두 점 좌표
      float y1 = currentCenterY - gapPix / 2.0;
      float y2 = currentCenterY + gapPix / 2.0;

      // 7) 벽 이동/리젠 (+ 구멍 상하 이동)
      //    항상 walls[0]만 사용
      Wall &w = walls[0];
      w.x -= wallSpeed;

      // --- 구멍 상하 이동 업데이트 ---
      if (w.moving) {
        float minOff = -w.baseGapY1Local;
        float maxOff = PLAY_HEIGHT - (w.baseGapY2Local + GAP_HEIGHT);

        if (maxOff < minOff) {
          minOff = 0;
          maxOff = 0;
        }

        w.gapOffset += w.gapVel;

        if (w.gapOffset < minOff) {
          w.gapOffset = minOff;
          w.gapVel = -w.gapVel;
        } else if (w.gapOffset > maxOff) {
          w.gapOffset = maxOff;
          w.gapVel = -w.gapVel;
        }

        int offI = (int)round(w.gapOffset);
        w.gapY1Local = w.baseGapY1Local + offI;
        w.gapY2Local = w.baseGapY2Local + offI;
      }

      if (w.x < -WALL_WIDTH) {
        // 난이도에 따른 빈도 증가
        int offset = random(spawnGapMin, spawnGapMax + 1);
        respawnWall(w, SCREEN_WIDTH + offset);
      }

      // 8) 벽 통과 점수
      float wallRight = w.x + WALL_WIDTH;
      if (!w.scored && wallRight < (currentCenterX - DOT_RADIUS)) {
        score += 1;
        w.scored = true;
      }

      // 9) 코인 스폰/이동/충돌
      trySpawnCoin(coinChance);

      if (coin.active) {
        float coinSpeed = wallSpeed * 0.9;
        coin.x -= coinSpeed;

        if (coin.x < -COIN_RADIUS) {
          resetCoin();
        } else {
          float dx = coin.x - currentCenterX;
          float dy1 = coin.y - y1;
          float dy2 = coin.y - y2;

          float rSum = DOT_RADIUS + COIN_RADIUS;
          float r2 = rSum * rSum;

          if ((dx*dx + dy1*dy1) <= r2 || (dx*dx + dy2*dy2) <= r2) {
            score += COIN_BONUS;
            resetCoin();
          }
        }
      }

      // 10) 충돌 감지
      float wallLeft  = w.x;
      float wallRight2 = w.x + WALL_WIDTH;

      float dotLeft  = currentCenterX - DOT_RADIUS;
      float dotRight = currentCenterX + DOT_RADIUS;

      if (dotRight >= wallLeft && dotLeft <= wallRight2) {
        int gap1Abs = PLAY_TOP + w.gapY1Local;
        int gap2Abs = PLAY_TOP + w.gapY2Local;

        bool dot1Safe = dotWithinGap(y1, gap1Abs) || dotWithinGap(y1, gap2Abs);
        bool dot2Safe = dotWithinGap(y2, gap1Abs) || dotWithinGap(y2, gap2Abs);

        if (!dot1Safe || !dot2Safe) {
          gameOver = true;
        }
      }

      // 11) HUD - 현재 점수
      display.setCursor(0, 0);
      display.print("SCORE:");
      display.print(score);

      // 12) 점 그리기
      display.fillCircle((int)currentCenterX, (int)y1, (int)DOT_RADIUS, SSD1306_WHITE);
      display.fillCircle((int)currentCenterX, (int)y2, (int)DOT_RADIUS, SSD1306_WHITE);

      // 13) 벽 그리기
      {
        int x = (int)w.x;
        int gap1Abs = PLAY_TOP + w.gapY1Local;
        int gap2Abs = PLAY_TOP + w.gapY2Local;

        int h1 = gap1Abs - PLAY_TOP;
        if (h1 > 0) display.fillRect(x, PLAY_TOP, WALL_WIDTH, h1, SSD1306_WHITE);

        int seg2Top = gap1Abs + GAP_HEIGHT;
        int h2 = gap2Abs - seg2Top;
        if (h2 > 0) display.fillRect(x, seg2Top, WALL_WIDTH, h2, SSD1306_WHITE);

        int seg3Top = gap2Abs + GAP_HEIGHT;
        int h3 = (PLAY_BOTTOM + 1) - seg3Top;
        if (h3 > 0) display.fillRect(x, seg3Top, WALL_WIDTH, h3, SSD1306_WHITE);
      }

      // 14) 코인 그리기
      if (coin.active) {
        display.fillCircle((int)coin.x, (int)coin.y, (int)COIN_RADIUS, SSD1306_WHITE);
      }

      // 15) 초음파 세로 바(우측, 플레이 영역)
      int barWidth = 4;
      int barX = SCREEN_WIDTH - barWidth;
      int barHeight = (int)((smoothedDistance / 30.0) * PLAY_HEIGHT);

      display.fillRect(barX, PLAY_BOTTOM - barHeight + 1, barWidth, barHeight, SSD1306_WHITE);
      display.drawRect(barX, PLAY_TOP, barWidth, PLAY_HEIGHT, SSD1306_WHITE);

    } else {
      // --- 게임 오버 처리 + 최고점수 저장 ---
      if (score > highScore) {
        highScore = (uint16_t)score;
        EEPROM.put(EEPROM_ADDR_HIGHSCORE, highScore);
      }

      // HUD - BEST만 추가
      display.setCursor(0, 0);
      display.print("BEST:");
      display.print(highScore);

      display.setTextSize(2);
      display.setCursor(10, 20);
      display.print("GAME OVER");
      display.setTextSize(1);
      display.setCursor(10, 45);
      display.print("PRESS SWITCH TO");
      display.setCursor(10, 55);
      display.print("RESTART");
    }

    display.display();
    vTaskDelay(MS2TICKS(50));
  }
}

// -----------------------------
// Setup
// -----------------------------
void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A5));

  EEPROM.get(EEPROM_ADDR_HIGHSCORE, highScore);
  if (highScore > 10000) highScore = 0;

  initWalls();
  resetCoin();

  xTaskCreate(joyStick,   "joy",    128, NULL, 3, NULL);
  xTaskCreate(ultraSound, "ultra",  128, NULL, 3, NULL);
  xTaskCreate(rgbTask,    "rgb",    128, NULL, 1, NULL);
  xTaskCreate(buzzerTask, "buzzer", 128, NULL, 1, NULL);
  xTaskCreate(oledTask,   "oled",   256, NULL, 2, NULL);
}

void loop() {
  // FreeRTOS 사용
}
