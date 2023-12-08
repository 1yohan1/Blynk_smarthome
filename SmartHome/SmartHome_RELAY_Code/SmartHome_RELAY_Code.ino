// 전구 및 스위치, RELAY

#include <WiFi.h>
#include <WiFiClient.h>
#include <time.h>
#include <BlynkSimpleEsp32.h>
#define BLYNK_PRINT Serial

int relay1Pin = 32;
int toggleSwitch1Pin = 14;
int relay2Pin = 33;
int toggleSwitch2Pin = 12;
int relay3Pin = 22;
int toggleSwitch3Pin = 13;

int relay1State = HIGH;
int lastToggle1State = HIGH;
int relay2State = HIGH;
int lastToggle2State = HIGH;
int relay3State = HIGH;
int lastToggle3State = HIGH;

#define VIRTUAL_PIN_RELAY_1_STATE V7
#define VIRTUAL_PIN_RELAY_2_STATE  V8
#define VIRTUAL_PIN_RELAY_3_STATE V9

char auth[] = ""; // TOKEN 입력
char ssid[] = ""; // WIFI 입력
char pass[] = ""; // PASSWORD 입력

void sendRelayState() {
  Blynk.virtualWrite(VIRTUAL_PIN_RELAY_1_STATE, relay1State);
  Blynk.virtualWrite(VIRTUAL_PIN_RELAY_2_STATE, relay2State);
  Blynk.virtualWrite(VIRTUAL_PIN_RELAY_3_STATE, relay3State);
}

void toggle1Relay(int newRelay1State) {
  relay1State = newRelay1State;
  digitalWrite(relay1Pin, relay1State);
}

void toggle2Relay(int newRelay2State) {
  relay2State = newRelay2State;
  digitalWrite(relay2Pin, relay2State);
}

void toggle3Relay() {
  relay3State = digitalRead(toggleSwitch3Pin);
  digitalWrite(relay3Pin, relay3State);
  Blynk.virtualWrite(VIRTUAL_PIN_RELAY_3_STATE, relay3State);

  if (relay3State == HIGH) {
    relay1State = HIGH;
    relay2State = HIGH;
    digitalWrite(relay1Pin, relay1State);
    digitalWrite(relay2Pin, relay2State);
    Blynk.virtualWrite(VIRTUAL_PIN_RELAY_1_STATE, relay1State);
    Blynk.virtualWrite(VIRTUAL_PIN_RELAY_2_STATE, relay2State);
  } else {
    relay1State = LOW;
    relay2State = LOW;
    digitalWrite(relay1Pin, relay1State);
    digitalWrite(relay2Pin, relay2State);
    Blynk.virtualWrite(VIRTUAL_PIN_RELAY_1_STATE, relay1State);
    Blynk.virtualWrite(VIRTUAL_PIN_RELAY_2_STATE, relay2State);
  }
}

BLYNK_WRITE(VIRTUAL_PIN_RELAY_1_STATE) {
  int newRelay1State = param.asInt();
  if (newRelay1State != relay1State) {
    toggle1Relay(newRelay1State);
  }
}

BLYNK_WRITE(VIRTUAL_PIN_RELAY_2_STATE) {
  int newRelay2State = param.asInt();
  if (newRelay2State != relay2State) {
    toggle2Relay(newRelay2State);
  }
}

BLYNK_WRITE(VIRTUAL_PIN_RELAY_3_STATE) {
  int toggle3State = param.asInt();
  if (toggle3State == HIGH) {
    Blynk.virtualWrite(VIRTUAL_PIN_RELAY_1_STATE, HIGH);
    Blynk.virtualWrite(VIRTUAL_PIN_RELAY_2_STATE, HIGH);
    Blynk.virtualWrite(VIRTUAL_PIN_RELAY_3_STATE, HIGH);
    relay1State = HIGH;
    relay2State = HIGH;
    relay3State = HIGH;
  } else {
    Blynk.virtualWrite(VIRTUAL_PIN_RELAY_1_STATE, LOW);
    Blynk.virtualWrite(VIRTUAL_PIN_RELAY_2_STATE, LOW);
    Blynk.virtualWrite(VIRTUAL_PIN_RELAY_3_STATE, LOW);
    relay1State = LOW;
    relay2State = LOW;
    relay3State = LOW;
  }
  digitalWrite(relay1Pin, relay1State);
  digitalWrite(relay2Pin, relay2State);
  digitalWrite(relay3Pin, relay3State);
}


void setup() {
  Serial.begin(115200);
  
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  pinMode(relay3Pin, OUTPUT);
  pinMode(toggleSwitch1Pin, INPUT_PULLUP);
  pinMode(toggleSwitch2Pin, INPUT_PULLUP);
  pinMode(toggleSwitch3Pin, INPUT_PULLUP);

  digitalWrite(relay1Pin, HIGH);
  digitalWrite(relay2Pin, HIGH);
  digitalWrite(relay3Pin, HIGH);  

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  timer.setInterval(1000L, sendRelayState);
}

void loop() {
  Blynk.run();
  timer.run();
}
