// ================================

//   심박센서를 이용한 측정 테스트

// ================================

// ========== 예제 특징 ============

//  심박센서는 시리얼 모니터로 확인 합니다.

// ================================


// ======== 사용 라이브러리 =========
// MAX3010x : 1.1.2
// Adafruit_GFX : 1.12.1
// Adafruit_SSD1306 : 2.5.13
// =================================

// #include <Arduino.h>  // PlatformIO/VSCode 사용 시 권장

#include <Wire.h> // I2C 통신용 라이브러리
#include <Adafruit_GFX.h> // OLED 그래픽
#include <Adafruit_SSD1306.h> // OLED 드라이버
#include "MAX30105.h"
#include "heartRate.h"

#define LED1_PIN              25
#define LED2_PIN              26

#define BTN1_PIN              36
#define BTN2_PIN              39
#define BTN3_PIN              34
#define BTN4_PIN              35

#define HEART_RATE_PIN        32 // 심박센서 핀번호

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

const uint8_t RATE_SIZE = 4; // 평균화 샘플 수
uint8_t rates[RATE_SIZE];
uint8_t rateSpot = 0;
uint32_t lastBeat = 0; 

uint16_t beatsPerMinute;
uint8_t beatAvg;

// ======= OLED 초기화 =============

// Reset 핀이 없으므로 네번째 인자 -1로 초기화
// 꼭 wire1로 초기화 합시다. wire는 후에, 심박센서가 사용합니다. 
// wire로 초기화를 해야한다면, 추후 MAX30100 라이브러리를 수정해야 합니다.

Adafruit_SSD1306 display(OLED_SCREEN_WIDTH, OLED_SCREEN_HEIGHT, &Wire1, -1);  // 리셋 핀 -1 (없음)

// PulseOximeter 인스턴스 생성
MAX30105 particleSensor;



void setup() 
{

  Serial.begin(115200); //115200의 baudrate로 시리얼 연결
  Wire1.begin(SDA_OLED_PIN, SCL_OLED_PIN); //꼭 Wire1로 초기화 합시다.
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN,100000); // 센서 I2C 초기화

  ledBlinkInit(); //  LED Blink Init  
  btnInputInit(); // Button Input Init
  oledDisplayInit(); // OLED Play Init
  max30100TestInit(); // MAX30100 센서 연결 테스트
  heartRateInit(); // 심박측정 테스트

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

    // 간단하게 시리얼 모니터로 출력합니다.
    Serial.print("[");
    Serial.print(btn_1);
    Serial.print(btn_2);
    Serial.print(btn_3);
    Serial.print(btn_4);
    Serial.print("]");
    
    // 버튼은 풀업으로 연결되어있으므로, 누르면 LOW로 떨어집니다.

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
  // OLED 초기화
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR)) 
  {
    Serial.println(F("SSD1306 초기화 실패"));
    while (true);  // 무한 루프로 오동작 방지
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
    // 연결만 테스트하므로 실패 시 멈춰서 상태 유지
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

//
//////////////////////////////////////////////////////////////////////////////////////////
//


void heartRateInit()
{
#if USE_TEST_HEART_RATE
  
// Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) // I2C 포트, 400kHz 속도
  {
    Serial.println("MAX3010x was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //센서 Default Settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED

#endif
}

void heartRateTask()
{
#if USE_TEST_HEART_RATE

long irValue = particleSensor.getIR(); // 심박 RAW Data

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

  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);

  if (irValue < 50000)
    Serial.print(" No finger?");

  Serial.println();

  #endif
}


