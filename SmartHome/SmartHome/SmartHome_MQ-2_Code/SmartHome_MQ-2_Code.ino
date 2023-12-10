// 가스, MQ-2

#include <WiFi.h>
#include <WiFiClient.h>
#include <time.h>
#include <BlynkSimpleEsp32.h>
#define BLYNK_PRINT Serial
#define GAS_PIN 36

#define VIRTUAL_PIN_GAS          V3

BlynkTimer timer;

char auth[] = ""; // TOKEN 입력
char ssid[] = ""; // WIFI 입력
char pass[] = ""; // PASSWORD 입력

void sendSensorData() {
  
  int gasValue = analogRead(GAS_PIN);
  Blynk.virtualWrite(VIRTUAL_PIN_GAS, gasValue);
  
  if (gasValue > 700){
    Blynk.logEvent("gas_alert", "Gas Leakage Detected");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(fanPin, OUTPUT);
  
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  timer.setInterval(1000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
}
