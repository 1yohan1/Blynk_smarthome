// 온습도 센서, DHT11

#include <WiFi.h>
#include <WiFiClient.h>
#include <time.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#define BLYNK_PRINT Serial
#define DHT_PIN 0
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

#define VIRTUAL_PIN_TEMPERATURE  V0
#define VIRTUAL_PIN_HUMIDITY     V1

int temperature = 0;
BlynkTimer timer;

char auth[] = ""; // TOKEN 입력
char ssid[] = ""; // WIFI 입력
char pass[] = ""; // PASSWORD 입력

void sendSensorData() {
  temperature = dht.readTemperature();
  int humidity = dht.readHumidity();

  Blynk.virtualWrite(VIRTUAL_PIN_TEMPERATURE, temperature);
  Blynk.virtualWrite(VIRTUAL_PIN_HUMIDITY, humidity);
}


void setup() {
  Serial.begin(115200);
  dht.begin();

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  timer.setInterval(1000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
}
