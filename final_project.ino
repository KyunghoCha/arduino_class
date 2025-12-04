#include <Arduino_FreeRTOS.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- OLED 설정 ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// FreeRTOS 틱 변환
#define MS2TICKS(ms) (ms / portTICK_PERIOD_MS)

// --- 핀 정의 ---
#define TRIG 4      // 초음파 TRIG
#define ECHO 5      // 초음파 ECHO
#define JSWITCH 6   // 조이스틱 버튼
#define BUZZER 7    // 부저
#define RGB_B 8     // RGB Blue
#define RGB_G 9     // RGB Green
#define RGB_R 10    // RGB Red
#define JX A1       // 조이스틱 X축
#define JY A0       // 조이스틱 Y축

// --- 데이터 구조체 ---
typedef struct {
  int x, y, btn;
} JoystickData;

typedef struct {
  float distance; // cm
} UltraData;

typedef struct {
  int r, g, b;
  bool blueToggle;
} RgbData;

// --- 전역 상태 변수 ---
volatile JoystickData joyData = { 0, 0, 0 };
volatile UltraData ultraData = { 0 };
volatile RgbData rgbData = { 0, 0, 0, false };

// 점 중심 좌표 및 속도 관련
#define VMAX 4.0                    // 조이스틱 이동 속도 (픽셀/프레임)
#define DISTANCE_SMOOTH_FACTOR 0.1  // 초음파 값 부드럽게 만드는 계수

volatile float currentCenterX = SCREEN_WIDTH / 2.0;
volatile float currentCenterY = SCREEN_HEIGHT / 2.0;
volatile float smoothedDistance = 0.0;

// 점/벌어짐 설정
#define MAX_GAP_PIX 50  // 점 간 최대 벌어짐 픽셀
#define DOT_RADIUS  2.0  // 점의 반지름

// 게임 상태
volatile bool gameOver = false;

// --- 장애물 설정 ---
#define WALL_WIDTH      5   // 장애물 벽의 폭
#define GAP_HEIGHT      15   // 장애물 구멍 하나의 높이
#define GAP_COUNT       2    // 구멍의 개수
#define WALL_SPEED      2.0  // 장애물 이동 속도 (픽셀/프레임)
#define MIN_GAP_SPACING 5    // 두 구멍 사이의 최소 간격 (픽셀)

volatile float wallX = SCREEN_WIDTH;
volatile int gapY1 = 0, gapY2 = 0; // 구멍의 상단 Y 좌표

// ----------------------------------------------------
// 유틸리티: 랜덤 구멍 생성 함수
// ----------------------------------------------------
void generateWallGaps() {
    // 구멍 2개와 구멍 사이의 최소 간격을 뺀 후, 구멍 1개의 높이를 뺀 나머지 공간에서 첫 구멍을 생성
    int maxRandomY1 = SCREEN_HEIGHT - (GAP_HEIGHT * GAP_COUNT) - MIN_GAP_SPACING;
    if (maxRandomY1 < 0) maxRandomY1 = 0;

    // 1. 첫 번째 구멍 위치 (gapY1) 생성
    gapY1 = random(0, maxRandomY1 + 1);

    // 2. 두 번째 구멍 위치 (gapY2) 생성
    
    // gapY1의 끝 지점 (gapY1 + GAP_HEIGHT)
    int gap1_end = gapY1 + GAP_HEIGHT;
    
    // 두 번째 구멍이 시작할 수 있는 최소 지점 (gap1_end + MIN_GAP_SPACING)
    int y2_start_min = gap1_end + MIN_GAP_SPACING;
    
    // 두 번째 구멍이 시작할 수 있는 최대 지점 (스크린 높이 - 구멍 높이)
    int y2_start_max = SCREEN_HEIGHT - GAP_HEIGHT;

    // 만약 최소 시작 지점이 최대 시작 지점보다 크다면, 랜덤하게 생성할 공간이 없음
    // 두 번째 구멍이 맨 아래에 생성되도록 강제, 예외 처리
    if (y2_start_min > y2_start_max) {
        // 이 경우는 거의 발생하지 않지만, 발생하면 첫 번째 구멍이 너무 길게 잡혔다는 의미
        // 여기서는 안전하게 최대값으로 설정합
        gapY2 = y2_start_max;
    } else {
        // 최소 시작 지점과 최대 시작 지점 사이에서 두 번째 구멍을 랜덤 생성
        gapY2 = random(y2_start_min, y2_start_max + 1);
    }
}

// ----------------------------------------------------
// 조이스틱 Task
// ----------------------------------------------------
void joyStick(void *p) {
  pinMode(JSWITCH, INPUT_PULLUP);
  for (;;) {
    joyData.x = analogRead(JX);
    joyData.y = analogRead(JY);
    joyData.btn = !digitalRead(JSWITCH);

    // 게임 오버 상태에서 버튼을 누르면 재시작
    if (gameOver && joyData.btn) {
        gameOver = false;
        currentCenterX = SCREEN_WIDTH/2.0;
        currentCenterY = SCREEN_HEIGHT/2.0;
        wallX = SCREEN_WIDTH;  // 장애물 초기화
        generateWallGaps();    // 새 장애물 구멍 생성
    }
    
    if (joyData.btn) {
      static bool lastBtnState = false;
      if (joyData.btn && !lastBtnState) {
        rgbData.blueToggle = !rgbData.blueToggle;
      }
      lastBtnState = joyData.btn;
    }

    vTaskDelay(MS2TICKS(50));
  }
}

// ----------------------------------------------------
// 초음파 Task (0~30cm 제한)
// ----------------------------------------------------
void ultraSound(void *p) {
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  for (;;) {
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);

    long duration = pulseIn(ECHO, HIGH);
    float distance_cm = duration * 0.0343 / 2.0;

    // 값 제한 (0.0cm ~ 30.0cm)
    if (distance_cm > 30.0) distance_cm = 30.0;
    if (distance_cm < 0.0) distance_cm = 0.0;

    ultraData.distance = distance_cm;

    vTaskDelay(MS2TICKS(150));
  }
}

// ----------------------------------------------------
// OLED Task (게임의 메인 루프)
// ----------------------------------------------------
void oledTask(void *p) {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  
  currentCenterX = SCREEN_WIDTH/2.0;
  currentCenterY = SCREEN_HEIGHT/2.0;

  for (;;) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    if (!gameOver) {
        // 1. 초음파 선형 보간 (Smoothing) 적용
        smoothedDistance += (ultraData.distance - smoothedDistance) * DISTANCE_SMOOTH_FACTOR;
        
        // 2. 초음파 거리로 점 벌어짐 (gapPix) 계산
        float gapPix = map(smoothedDistance, 0, 30, 0, MAX_GAP_PIX);
        
        // 3. 조이스틱 이동 속도 계산 및 데드존 설정
        float vx = map(joyData.x, 0, 1023, -VMAX, VMAX);
        float vy = map(joyData.y, 0, 1023, -VMAX, VMAX); 
        
        if (abs(joyData.x - 512) < 50) vx = 0;
        if (abs(joyData.y - 512) < 50) vy = 0;
        
        // 4. 중심 좌표에 조이스틱 이동 적용 (누적)
        currentCenterX += vx;
        currentCenterY += vy;
        
        // 5. 화면 경계 제한 (Boundary Check)
        if(currentCenterX < DOT_RADIUS) currentCenterX = DOT_RADIUS;
        if(currentCenterX > SCREEN_WIDTH-1-DOT_RADIUS) currentCenterX = SCREEN_WIDTH-1-DOT_RADIUS;
        
        if (currentCenterY - gapPix/2.0 < DOT_RADIUS) {
            currentCenterY = DOT_RADIUS + gapPix/2.0;
        }
        if (currentCenterY + gapPix/2.0 > SCREEN_HEIGHT-1 - DOT_RADIUS) {
            currentCenterY = SCREEN_HEIGHT-1 - DOT_RADIUS - gapPix/2.0;
        }

        // 6. 두 점 좌표 계산
        float y1 = currentCenterY - gapPix/2.0; 
        float y2 = currentCenterY + gapPix/2.0;
    
        // 7. 장애물 이동 및 생성
        wallX -= WALL_SPEED;

        if (wallX < -WALL_WIDTH) {
            wallX = SCREEN_WIDTH;
            generateWallGaps();
        }

        // --------------------------------------------------
        // 8. 충돌 감지 (Collision Check)
        // --------------------------------------------------
        
        // A. X축 충돌 조건: 점의 X 범위가 벽의 X 범위와 겹침
        if (currentCenterX + DOT_RADIUS >= wallX && currentCenterX - DOT_RADIUS <= wallX + WALL_WIDTH) {
            bool hitWall = false;
            
            // Y축 벽 경계 계산
            // 벽1: 0 ~ gapY1
            // 벽2: gapY1 + GAP_HEIGHT ~ gapY2
            // 벽3: gapY2 + GAP_HEIGHT ~ SCREEN_HEIGHT
            
            // y_top: 점의 상단 Y 경계 (y1 - DOT_RADIUS)
            // y_bottom: 점의 하단 Y 경계 (y2 + DOT_RADIUS)
            
            // 점 1 (y1) 충돌 판정
            // 점 1이 벽 1에 닿았거나 (상단)
            if (y1 - DOT_RADIUS < gapY1) hitWall = true;
            // 점 1이 벽 2에 닿았거나 (중앙)
            else if (y1 + DOT_RADIUS > gapY1 + GAP_HEIGHT && y1 - DOT_RADIUS < gapY2) hitWall = true;
            // 점 1이 벽 3에 닿았을 때 (하단) - y1은 위쪽 점이라서 벽 3에 닿기 어려움 (예외적)
            //else if (y1 + DOT_RADIUS > gapY2 + GAP_HEIGHT) hitWall = true; 
            
            // 점 2 (y2) 충돌 판정
            // 점 2가 벽 1에 닿았거나 (상단) - y2는 아래쪽 점이라서 벽 1에 닿기 어려움 (예외적)
            // if (y2 - DOT_RADIUS < gapY1) hitWall = true;
            // 점 2가 벽 2에 닿았거나 (중앙)
            if (y2 - DOT_RADIUS < gapY2 && y2 + DOT_RADIUS > gapY1 + GAP_HEIGHT) hitWall = true;
            // 점 2가 벽 3에 닿았을 때 (하단)
            else if (y2 + DOT_RADIUS > gapY2 + GAP_HEIGHT) hitWall = true; 
            
            
            // 최종 판정: 두 점 중 하나라도 벽에 닿았으면 충돌
            if (hitWall) {
                gameOver = true;
            }
        }
        
        // 9. 점 그리기
        display.fillCircle((int)currentCenterX, (int)y1, (int)DOT_RADIUS, SSD1306_WHITE);
        display.fillCircle((int)currentCenterX, (int)y2, (int)DOT_RADIUS, SSD1306_WHITE);

        // 10. 장애물 그리기
        display.fillRect((int)wallX, 0, WALL_WIDTH, gapY1, SSD1306_WHITE);
        display.fillRect((int)wallX, gapY1 + GAP_HEIGHT, WALL_WIDTH, gapY2 - (gapY1 + GAP_HEIGHT), SSD1306_WHITE);
        display.fillRect((int)wallX, gapY2 + GAP_HEIGHT, WALL_WIDTH, SCREEN_HEIGHT - (gapY2 + GAP_HEIGHT), SSD1306_WHITE);

        // 11. 초음파 세로 바 그래프 표시
        int barWidth = 4;
        int barX = SCREEN_WIDTH - barWidth;
        int barHeight = map((int)(smoothedDistance * 10), 0, 300, 0, SCREEN_HEIGHT);
        
        display.fillRect(barX, SCREEN_HEIGHT - barHeight, barWidth, barHeight, SSD1306_WHITE);
        display.drawRect(barX, 0, barWidth, SCREEN_HEIGHT, SSD1306_WHITE);

    } else {
      // --- 게임 오버 로직 ---
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

// ----------------------------------------------------
// RGB Task (게임 오버 시 변화 추가)
// ----------------------------------------------------
void rgbTask(void *p) {
  pinMode(RGB_R, OUTPUT);
  pinMode(RGB_G, OUTPUT);
  pinMode(RGB_B, OUTPUT);

  for (;;) {
    if (gameOver) {
        // 게임 오버 시 LED 빨간색 깜박임
        analogWrite(RGB_R, 255);
        analogWrite(RGB_G, 0);
        analogWrite(RGB_B, 0);
        vTaskDelay(MS2TICKS(250));
        analogWrite(RGB_R, 0);
        vTaskDelay(MS2TICKS(250));
    } else {
        rgbData.r = map(joyData.x, 0, 1023, 0, 255);
        rgbData.g = map(joyData.y, 0, 1023, 0, 255);
        rgbData.b = rgbData.blueToggle ? 255 : 0;

        analogWrite(RGB_R, rgbData.r);
        analogWrite(RGB_G, rgbData.g);
        analogWrite(RGB_B, rgbData.b);
        vTaskDelay(MS2TICKS(50));
    }
  }
}

// ----------------------------------------------------
// Buzzer Task (게임 오버 시 멈춤 추가)
// ----------------------------------------------------
void buzzerTask(void *p) {
  pinMode(BUZZER, OUTPUT);

  for (;;) {
    if (joyData.btn && !gameOver) { // 게임 오버 상태가 아닐 때만 소리
      tone(BUZZER, 1000, 10);
    }
    if (gameOver) {
        noTone(BUZZER); // 게임 오버 시 소리 끔
    }
    vTaskDelay(MS2TICKS(50));
  }
}

// ----------------------------------------------------
// Setup
// ----------------------------------------------------
void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A5)); // 랜덤 시드 초기화

  // 첫 장애물 구멍 생성
  generateWallGaps(); 

  // Task 생성
  xTaskCreate(joyStick,   "joy",    128, NULL, 3, NULL);
  xTaskCreate(ultraSound, "ultra",  128, NULL, 3, NULL);
  xTaskCreate(rgbTask,    "rgb",    128, NULL, 1, NULL);
  xTaskCreate(buzzerTask, "buzzer", 128, NULL, 1, NULL);
  xTaskCreate(oledTask,   "oled",   256, NULL, 2, NULL);
}

void loop() {
  // FreeRTOS를 사용하므로 loop() 함수는 비워 둠
}
