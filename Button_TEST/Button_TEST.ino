
// ================================

//          버튼 입력 예제

// ================================

// ========== 예제 특징 ============

//  버튼의 누름 여부를 
//  시리얼 모니터에 주기적으로 출력합니다.

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
#define BTN1   36
#define BTN2   39
#define BTN3   34
#define BTN4   35

// ============ Test ===============

// 사용 할 기능을 1로 나머지는 0으로 둡니다.

#define TEST_LED_BLINK    0 
#define TEST_BTN_INPUT    1


// ======== 논블로킹 주기 제어 ======

#define TEST_INTERVAL 500 // 주기를 만들기위해서 선언 (500ms)


void setup() 
{

Serial.begin(115200); //115200의 baudrate로 시리얼 연결

ledBlinkInit(); //  LED Blink Init  
btnInputInit(); // Button Input Init

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
