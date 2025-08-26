// ================================

//   심박센서를 이용한 측정 테스트

// ================================

// ========== 예제 특징 ============

//  심박센서는 OLED로 확인합니다.

// ================================


// ======== 사용 라이브러리 =========
// MAX3010x : 1.1.2
// Adafruit_GFX : 1.12.1
// Adafruit_SSD1306 : 2.5.13
// =================================

// #include <Arduino.h>  // PlatformIO/VSCode 사용 시 권장

#include <Wire.h> 
#include <Adafruit_GFX.h> 
#include <Adafruit_SSD1306.h> 
#include "MAX30105.h"
#include "heartRate.h"

#define LED1_PIN              25
#define LED2_PIN              26

#define BTN1_PIN              36
#define BTN2_PIN              39
#define BTN3_PIN              34
#define BTN4_PIN              35

#define HEART_RATE_PIN        32 

//  - OLED I2C 핀 (Wire1) (보드에 따라 변경 가능)
//    예: ESP32에서 SDA=16, SCL=17 처럼 자유롭게 매핑 가능

#define SCL_OLED_PIN          22
#define SDA_OLED_PIN          21

//  -  MAX30100 (Wire)

#define I2C_SCL_PIN           17
#define I2C_SDA_PIN           16


// ============ Test ===============

// -사용 할 기능을 1로, 나머지는 0으로 둡니다.

#define USE_TEST_LED_BLINK     0 
#define USE_TEST_BTN_INPUT     0
#define USE_TEST_OLED_DISPLAY  1
#define USE_TEST_MAX30100      0
#define USE_TEST_HEART_RATE    1

#define INTERVAL_TIME_MS 500

#define OLED_SCREEN_WIDTH    128 
#define OLED_SCREEN_HEIGHT    64  
#define OLED_I2C_ADDR       0x3C

#define GRAPH_WIDTH          120
#define GRAPH_HEIGHT          32
#define GRAPH_START_X          4
#define GRAPH_START_Y         20

// ========== 변수 ================

const uint8_t RATE_SIZE = 4;
uint8_t rates[RATE_SIZE];
uint8_t rateSpot = 0;
uint32_t lastBeat = 0; 

uint16_t beatsPerMinute;
uint8_t beatAvg;

// ==== IR 값을 기준으로 그래프 스케일링 ========
int32_t irBuffer[120];          // IR 센서값을 저장하는 그래프 버퍼 (Raw Data)
int32_t bufferIndex = 0;        // 버퍼에서 현재 쓰고 있는 위치
int32_t currentMin = 999999;    // 현재 수집 중인 최소값
int32_t currentMax = 0;         // 현재 수집 중인 최대값  
int32_t displayMin = 999999;    // 화면 표시용 최소값 (스케일링용)
int32_t displayMax = 0;         // 화면 표시용 최대값 (스케일링용)
// ==== IR 값을 기준으로 그래프 스케일링 ========

// ======= OLED 초기화 =============
Adafruit_SSD1306 display(OLED_SCREEN_WIDTH, OLED_SCREEN_HEIGHT, &Wire1, -1);

// MAX3010x 인스턴스 생성
MAX30105 particleSensor;

void setup() 
{

  Serial.begin(115200);
  Wire1.begin(SDA_OLED_PIN, SCL_OLED_PIN);
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN,100000);

  ledBlinkInit();
  btnInputInit();
  oledDisplayInit();
  max30100TestInit();
  heartRateInit();

}

void loop() 
{
  
  ledBlinkTask();
  btnInputTask();
  heartRateTask();

}

// ============ Custom 함수 =================

//
//////////////////////////////////////////////////////////////////////////////////////////
//

void ledBlinkInit()
{
#if USE_TEST_LED_BLINK

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);

#endif
}

void ledBlinkTask()
{
#if USE_TEST_LED_BLINK

  static bool led_state = false;
  static uint32_t prev_time = millis(); // cpu 가동시간으로 부터 시간 측정
  if (millis() - prev_time >= INTERVAL_TIME_MS) // 500ms마다 led1,2 토글
  {
    prev_time += INTERVAL_TIME_MS;

    led_state = !led_state;
    digitalWrite(LED1_PIN, led_state ? HIGH : LOW);
    digitalWrite(LED2_PIN, led_state ? HIGH : LOW);
  }

#endif
}

//
//////////////////////////////////////////////////////////////////////////////////////////
//


void btnInputInit()
{
#if USE_TEST_BTN_INPUT

  pinMode(BTN1_PIN, INPUT_PULLUP);
  pinMode(BTN2_PIN, INPUT_PULLUP);
  pinMode(BTN3_PIN, INPUT_PULLUP);
  pinMode(BTN4_PIN, INPUT_PULLUP);

#endif
}

void btnInputTask()
{
#if USE_TEST_BTN_INPUT

  int btn_1 = 0;
  int btn_2 = 0;
  int btn_3 = 0;
  int btn_4 = 0;

  static uint32_t prev_time = millis();
  if (millis() - prev_time >= INTERVAL_TIME_MS)
  {
    prev_time += INTERVAL_TIME_MS;

    btn_1 = digitalRead(BTN1_PIN);
    btn_2 = digitalRead(BTN2_PIN);
    btn_3 = digitalRead(BTN3_PIN);
    btn_4 = digitalRead(BTN4_PIN);


    Serial.print("[");
    Serial.print(btn_1);
    Serial.print(btn_2);
    Serial.print(btn_3);
    Serial.print(btn_4);
    Serial.print("]");
    
 

    if (btn_1 == LOW) 
    {
      Serial.print("BTN1 Push,");
    } 
    else 
    {
      Serial.print("BTN1 Pull,");
    }

    if (btn_2 == LOW) 
    {
      Serial.print("BTN2 Push,");
    } 
    else 
    {
      Serial.print("BTN2 Pull,");
    }

    if (btn_3 == LOW) 
    {
      Serial.print("BTN3 Push,");
    } 
    else 
    {
      Serial.print("BTN3 Pull,");
    }

    if (btn_4 == LOW) 
    {
      Serial.print("BTN4 Push");
    } 
    else 
    {
      Serial.print("BTN4 Pull");
    }
    Serial.println(",");
  }
#endif
}

//
//////////////////////////////////////////////////////////////////////////////////////////
//

void oledDisplayInit()
{
#if USE_TEST_OLED_DISPLAY

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR)) 
  {
    Serial.println(F("SSD1306 초기화 실패"));
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);              
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(1, 1);            
  display.println(F("OLED Test!"));
  display.display();

#endif
}

//
//////////////////////////////////////////////////////////////////////////////////////////
//

void max30100TestInit()
{
#if USE_TEST_MAX30100

  Serial.print("Initializing MAX3010x.. ");
  if (!sensor.begin()) 
  {

    Serial.print("FAILED: ");

    uint8_t partId = sensor.getPartId();
    if (partId == 0xFF) 
    {
      Serial.println("I2C error (no response)");
    } 
    else 
    {
      Serial.print("wrong part ID 0x");
      Serial.print(partId, HEX);
      Serial.print(" (expected 0x");
      Serial.print(EXPECTED_PART_ID, HEX);
      Serial.println(")");
    }

    while (1) 
    { 
      delay(10); 
    }
  } 
  else 
  {
    Serial.println("Success");
  }

#endif
}

//
//////////////////////////////////////////////////////////////////////////////////////////
//


void heartRateInit()
{
#if USE_TEST_HEART_RATE
  
// Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST))
  {
    Serial.println("MAX3010x was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); 
  particleSensor.setPulseAmplitudeRed(0x0A); 
  particleSensor.setPulseAmplitudeGreen(0);

#endif
}

void heartRateTask()
{
#if USE_TEST_HEART_RATE

// ======== 1단계: 센서 데이터 읽기 ========
  long irValue = particleSensor.getIR();

// ======== 2단계: 그래프용 데이터 저장 ========
  irBuffer[bufferIndex] = irValue;

// 최대/최소값 업데이트 (자동 스케일링용)
  if (irValue > currentMax)
  {
    currentMax = irValue;
  } 
  if (irValue < currentMin && irValue > 10000) 
  {
    currentMin = irValue;
  }

// 버퍼 인덱스를 다음으로 이동 (0~119 순환)
  bufferIndex = (bufferIndex + 1) % GRAPH_WIDTH;
// 한 바퀴 돌면 스케일 업데이트
  if (bufferIndex == 0) {
    displayMin = currentMin;
    displayMax = currentMax;
    currentMin = 999999;
    currentMax = 0;
  }
// ======== 3단계: 심박 감지 ========
  if (checkForBeat(irValue) == true) // 라이브러리 내 Peak 검출 알고리즘
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; // Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      // Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

// ======== 4단계: OLED에 결과 표시 ========
#if USE_TEST_OLED_DISPLAY
  static uint32_t prevDisp = 0;
  if (millis() - prevDisp >= 100) { // 100ms마다 화면 업데이트
    prevDisp = millis();

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.print("BPM:");
    display.print((int)beatsPerMinute);
    display.print("  Avg:");
    display.print(beatAvg);

    display.setCursor(0, 8);
    display.print("IR:");
    display.print(irValue/1000);
    display.print("k");

  
    drawHeartRateGraph();

    display.setCursor(0, 56);
    if (irValue < 50000) {
      display.print("NO FINGER");
    } else {
      display.print("Signal: OK");
    }
    
    display.display();
  }
#endif
  static unsigned long lastSerial = 0;
  if (millis() - lastSerial > 300) {
    Serial.print("IR=");
    Serial.print(irValue);
    Serial.print(", BPM=");
    Serial.print(beatsPerMinute);
    Serial.print(", Avg BPM=");
    Serial.print(beatAvg);

    if (irValue < 50000)
      Serial.print(" No finger?");

    Serial.println();
    lastSerial = millis();
  }

#endif
}

void drawHeartRateGraph() {
  // 그래프 테두리
  display.drawRect(GRAPH_START_X-1, GRAPH_START_Y-1, GRAPH_WIDTH+2, GRAPH_HEIGHT+2, SSD1306_WHITE);
  
  // 스케일링 범위가 유효한 경우에만 그래프 그리기
  if (displayMax > displayMin && displayMax > 10000) {
    
    for (int i = 1; i < GRAPH_WIDTH; i++) {
      int currentIndex = (bufferIndex + i - 1) % GRAPH_WIDTH;
      int nextIndex = (bufferIndex + i) % GRAPH_WIDTH;
      
      // 자동 스케일링으로 Y 좌표 계산
      int y1 = map(irBuffer[currentIndex], displayMin, displayMax, 
                   GRAPH_START_Y + GRAPH_HEIGHT - 1, GRAPH_START_Y);
      int y2 = map(irBuffer[nextIndex], displayMin, displayMax, 
                   GRAPH_START_Y + GRAPH_HEIGHT - 1, GRAPH_START_Y);
      
      // 범위 제한
      y1 = constrain(y1, GRAPH_START_Y, GRAPH_START_Y + GRAPH_HEIGHT - 1);
      y2 = constrain(y2, GRAPH_START_Y, GRAPH_START_Y + GRAPH_HEIGHT - 1);
      
      // 선 그리기
      display.drawLine(GRAPH_START_X + i - 1, y1, GRAPH_START_X + i, y2, SSD1306_WHITE);
    }
  }
  
  
}