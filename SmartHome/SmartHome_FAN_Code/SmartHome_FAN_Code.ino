// 에어컨, FAN

#include <WiFi.h>
#include <WiFiClient.h>
#include <time.h>
#include <BlynkSimpleEsp32.h>
#define BLYNK_PRINT Serial
#define fanPin 5 

#define VIRTUAL_PIN_FAN_STATE    V4
int fanControl = 0;

BlynkTimer timer;

char auth[] = ""; // TOKEN 입력
char ssid[] = ""; // WIFI 입력
char pass[] = ""; // PASSWORD 입력

enum FanState {
  AUTO_OFF,
  AUTO_ON,
  MANUAL_ON,
  MANUAL_OFF
};

FanState fanState = MANUAL_OFF;

void sendSensorData() {
  switch(fanState) {
    case AUTO_OFF:
      if (temperature > 21) {
        digitalWrite(fanPin, HIGH);
        Blynk.virtualWrite(VIRTUAL_PIN_FAN_STATE, digitalRead(fanPin));
        fanState = AUTO_ON;
      }
      break;

    case AUTO_OFF:
      if (temperature <= 21) {
        digitalWrite(fanPin, LOW);
        Blynk.virtualWrite(VIRTUAL_PIN_FAN_STATE, digitalRead(fanPin));
        fanState = AUTO_OFF;
      }
      break;

    case MANUAL_ON:
      digitalWrite(fanPin, HIGH);
      break;

    case MANUAL_OFF:
      digitalWrite(fanPin, LOW);
      if (fanControl == 0 && temperature > 21) {
        fanState = AUTO_ON;
      } else {
        fanState = AUTO_OFF;
      }
      break;
  }  
}

BLYNK_WRITE(VIRTUAL_PIN_FAN_STATE) { 
  int manualFanControl = param.asInt(); 

  if(manualFanControl == 1) {
    fanState = MANUAL_ON;
    digitalWrite(fanPin, HIGH);
    Blynk.virtualWrite(VIRTUAL_PIN_FAN_STATE, digitalRead(fanPin));
  } else {
    fanState = MANUAL_OFF;
    if (temperature <= 21) {
      digitalWrite(fanPin, LOW);
      Blynk.virtualWrite(VIRTUAL_PIN_FAN_STATE, digitalRead(fanPin));
    }
  }
}

void setup(){
  Serial.begin(115200);
  pinMode(fanPin, OUTPUT);

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  timer.setInterval(1000L, sendSensorData);  
}

void loop() {
  Blynk.run();
  timer.run();
}
