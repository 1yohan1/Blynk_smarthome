// 블라인드 커튼, MOTOR

#include <WiFi.h>
#include <WiFiClient.h>
#include <time.h>
#include <BlynkSimpleEsp32.h>
#define BLYNK_PRINT Serial

unsigned long motorStartTime = 0;
bool motorRunning = false;
#define motor1 25
#define motor2 26
#define MOTOR_DELAY_MICROS 2000000

#define VIRTUAL_PIN_MOTOR_CONTROL  V5

BlynkTimer timer;

char auth[] = ""; // TOKEN 입력
char ssid[] = ""; // WIFI 입력
char pass[] = ""; // PASSWORD 입력

void checkMotorStatus() {
  if (motorRunning) {
    unsigned long currentTime = micros();
    if (currentTime - motorStartTime >= MOTOR_DELAY_MICROS) {
      // 모터 동작 시간이 지났으므로 모터를 정지시킴
      ledcWrite(1, 0);
      ledcWrite(2, 0);
      motorRunning = false;  // 모터 동작 중 플래그 해제
    }
  }
}

BLYNK_WRITE(VIRTUAL_PIN_MOTOR_CONTROL) {
  int val = param.asInt();
  
  if (val == 1) {
    ledcWrite(1, 0);
    ledcWrite(2, 180);
    motorStartTime = millis();  // 모터 시작 시간 기록
    motorRunning = true;        // 모터 동작 중 플래그 설정
  } else {
    ledcWrite(1, 180);
    ledcWrite(2, 0);
    motorStartTime = millis();  // 모터 시작 시간 기록
    motorRunning = true;        // 모터 동작 중 플래그 설정
  }
}

void setup() {
  Serial.begin(115200);
  
  ledcAttachPin(25, 1);
  ledcAttachPin(26, 2);
  ledcSetup(1, 12000, 8);
  ledcSetup(2, 12000, 8);
  
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
}

void loop() {
  Blynk.run();
  timer.run();
  checkMotorStatus();
}
