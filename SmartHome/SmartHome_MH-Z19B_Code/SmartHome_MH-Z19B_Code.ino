// 이산화탄소, MH-Z19B

#include <WiFi.h>
#include <WiFiClient.h>
#include <time.h>
#include <BlynkSimpleEsp32.h>
#include <MHZ19.h>
#define BLYNK_PRINT Serial
MHZ19 mhz(&Serial2);

#define VIRTUAL_PIN_CO2          V2

BlynkTimer timer;

char auth[] = ""; // TOKEN 입력
char ssid[] = ""; // WIFI 입력
char pass[] = ""; // PASSWORD 입력

void sendSensorData() {
  MHZ19_RESULT response = mhz.retrieveData();
  int co2Value = -1; 

  if (response == MHZ19_RESULT_OK) {
    co2Value = mhz.getCO2();
  }

  if (co2Value > 1500){
    Blynk.logEvent("co2_alert", "Co2 is too high Let's ventilate");
  }
  
  Blynk.virtualWrite(VIRTUAL_PIN_CO2, co2Value);
}


void setup() {
  Serial.begin(115200);
  Serial2.begin(9600);

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  timer.setInterval(1000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
}
