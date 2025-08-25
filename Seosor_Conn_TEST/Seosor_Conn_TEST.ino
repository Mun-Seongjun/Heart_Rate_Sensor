
// ================================

//          센서 연결 테스트

// ================================

// ========== 예제 특징 ============

//  I2C로 연결된 SSD1306 OLED에 간단한 텍스트를 표시합니다.
//  I2C 핀을 보드의 원하는 핀으로 지정하여 사용합니다.

// ================================


// ======== 사용 라이브러리 =========
// // MAX30100_milan : 1.3.0
// (이 예제는 MAX30100 심박센서를
// 사용하지 않는 관계로 아래 라이브러리만 사용합니다.)
// Adafruit_GFX : 1.12.1
// Adafruit_SSD1306 : 2.5.13
// =================================

// #include <Arduino.h>  // PlatformIO/VSCode 사용 시 권장

#include <Wire.h> // I2C 통신용 라이브러리
#include <Adafruit_GFX.h> // OLED 그래픽
#include <Adafruit_SSD1306.h> // OLED 드라이버
#include "MAX30100.h" //센서 라이브러리


// ============ 장치 설정 ==========

#define LED1 25
#define LED2 26

#define BTN1   36
#define BTN2   39
#define BTN3   34
#define BTN4   35


//  - OLED I2C 핀 (Wire1) (보드에 따라 변경 가능)
//    예: ESP32에서 SDA=16, SCL=17 처럼 자유롭게 매핑 가능

#define SCL_OLED 22
#define SDA_OLED 21

//  -  MAX30100 (Wire)

#define I2C_SCL 17
#define I2C_SDA 16


// ============ Test ===============

// -사용 할 기능을 1로, 나머지는 0으로 둡니다.

#define TEST_LED_BLINK    0 
#define TEST_BTN_INPUT    0
#define TEST_OLED_DISPLAY 0
#define TEST_MAX30100     1

// ======== 논블로킹 주기 제어 ======

#define TEST_INTERVAL 500 // 주기를 만들기위해서 선언 (500ms)

// ======== OLED 설정 ==============

//  - 해상도와 I2C 주소 설정 (대부분 0x3C, 가끔 0x3D인 모듈도 있음)

#define SCREEN_WIDTH  128  // OLED 가로 해상도
#define SCREEN_HEIGHT 64  // OLED 세로 해상도
#define OLED_ADDR     0x3C

// ======= OLED 초기화 =============

// Reset 핀이 없으므로 네번째 인자 -1로 초기화
// 꼭 wire1로 초기화 합시다. wire는 후에, 심박센서가 사용합니다. 
// wire로 초기화를 해야한다면, 추후 MAX30100 라이브러리를 수정해야 합니다.

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, -1);  // 리셋 핀 -1 (없음)

// MAX30100 인스턴스 (기본 Wire 버스를 사용)
MAX30100 sensor;

void setup() 
{

Serial.begin(115200); //115200의 baudrate로 시리얼 연결
Wire1.begin(SDA_OLED, SCL_OLED); //꼭 Wire1로 초기화 합시다.
Wire.begin(I2C_SDA, I2C_SCL); // 센서 I2C 초기화

ledBlinkInit(); //  LED Blink Init  
btnInputInit(); // Button Input Init
oledDisplayInit(); // OLED Play Init
max30100TestInit(); // MAX30100 센서 연결 테스트

}

void loop() 
{
  
ledBlinkTask();
btnInputTask();

}

// ============ Custom 함수 =================

//
//////////////////////////////////////////////////////////////////////////////////////////
//

void ledBlinkInit()
{
#if TEST_LED_BLINK
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
#endif
}

void ledBlinkTask()
{
#if TEST_LED_BLINK
  static bool led_state = false;
  static uint32_t prev_time = millis(); // cpu 가동시간으로 부터 시간 측정
  if (millis() - prev_time >= TEST_INTERVAL) // 500ms마다 led1,2 토글
  {
    prev_time += TEST_INTERVAL;

    led_state = !led_state;
    digitalWrite(LED1, led_state ? HIGH : LOW);
    digitalWrite(LED2, led_state ? HIGH : LOW);
  }
#endif
}

//
//////////////////////////////////////////////////////////////////////////////////////////
//


void btnInputInit()
{
#if TEST_BTN_INPUT
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN3, INPUT_PULLUP);
  pinMode(BTN4, INPUT_PULLUP);
#endif
}

void btnInputTask()
{
#if TEST_BTN_INPUT

  int btn_1 = 0;
  int btn_2 = 0;
  int btn_3 = 0;
  int btn_4 = 0;

  static uint32_t prev_time = millis();
  if (millis() - prev_time >= TEST_INTERVAL)
  {
    prev_time += TEST_INTERVAL;

    btn_1 = digitalRead(BTN1);
    btn_2 = digitalRead(BTN2);
    btn_3 = digitalRead(BTN3);
    btn_4 = digitalRead(BTN4);

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
#if TEST_OLED_DISPLAY
  // OLED 초기화
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) 
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
#if TEST_MAX30100
  Serial.print("Initializing MAX30100.. ");
  if (!sensor.begin()) {
    Serial.print("FAILED: ");

    uint8_t partId = sensor.getPartId();
    if (partId == 0xFF) {
      Serial.println("I2C error (no response)");
    } else {
      Serial.print("wrong part ID 0x");
      Serial.print(partId, HEX);
      Serial.print(" (expected 0x");
      Serial.print(EXPECTED_PART_ID, HEX);
      Serial.println(")");
    }
    // 연결만 테스트하므로 실패 시 멈춰서 상태 유지
    while (1) { delay(10); }
  } else {
    Serial.println("Success");
  }
#endif
}

//
//////////////////////////////////////////////////////////////////////////////////////////
//