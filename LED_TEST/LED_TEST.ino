
// ================================

//            LED 예제

// ================================

// ======== 사용 라이브러리 =========
// (이 예제는 LED 토글만 사용하므로 아래 헤더는 생략 가능합니다)
// // MAX30100_milan : 1.3.0
// // Adafruit_GFX : 1.12.1
// // Adafruit_SSD1306 : 2.5.13
// =================================

// #include <Arduino.h>  // PlatformIO/VSCode 사용 시 권장


// ============ 장치 설정 ==========

#define LED1 25
#define LED2 26

// ============ Test ===============

#define TEST_LED_BLINK    1 // 사용 할 기능을 1로 바꿉니다.

// ======== 논블로킹 주기 제어 ======

#define TEST_INTERVAL 500 // 주기를 만들기위해서 선언 (500ms)


void setup() 
{

Serial.begin(115200); //115200의 baudrate로 시리얼 연결
ledBlinkInit(); //  LED Blink Init  

}

void loop() 
{
  
ledBlinkTask();

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
