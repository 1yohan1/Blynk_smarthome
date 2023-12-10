// 현재시간 및 TFT LCD 디스플레이

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <time.h>
#include <BlynkSimpleEsp32.h>
#define BLYNK_PRINT Serial

unsigned long previousBlynkUpdateTime = 0;

#define TFT_CS    15
#define TFT_DC    2
#define TFT_MOSI  23
#define TFT_CLK   18
#define TFT_RST   4
#define TFT_MISO  19
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

#define VIRTUAL_PIN_CURRENT_TIME V6

BlynkTimer timer;

char auth[] = ""; // TOKEN 입력
char ssid[] = ""; // WIFI 입력
char pass[] = "0"; // PASSWORD 입력

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600 * 9; // GMT+9
const int daylightOffset_sec = 0;

void connectToWiFi() {
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void syncTime() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  time_t now = time(nullptr);
  while (now < 24 * 3600) {
    delay(100);
    now = time(nullptr);
  }
}

String getFormattedTime() {
  struct tm *timeinfo;
  time_t now = time(nullptr);
  timeinfo = localtime(&now);

  char buffer[30];
  strftime(buffer, 30, "%a %b %d %H:%M:%S / %Y", timeinfo);
  return String(buffer);
}

void displayValuesOnTFT(String currentTime, int temperature, int humidity, int co2Value, int gasValue) {
  
  tft.fillScreen(ILI9341_NAVY); 
  tft.drawRoundRect(0,0,319,239,10, ILI9341_WHITE); 
  tft.setCursor(0, 0);
  tft.setTextSize(3);
  tft.setTextColor(ILI9341_WHITE); 
  tft.println("Current Time: ");
  
  tft.setTextSize(2);
  tft.println(currentTime);
  tft.println();
  
  tft.setTextSize(3);
  tft.setTextColor(ILI9341_YELLOW); 
  tft.println("Temperature: " + String(temperature) + " C");
  tft.println();
  tft.println("Humidity: " + String(humidity) + " %");
  tft.println();
  tft.setTextSize(3);
  tft.setTextColor(ILI9341_RED); 
  tft.println("CO2 Level: " + String(co2Value));
  tft.println();
  tft.setTextSize(3);
  tft.setTextColor(ILI9341_GREEN); 
  tft.println("Gas Value: " + String(gasValue));
}

void sendCurrentTimeToBlynk() {
  time_t now = time(nullptr);
  struct tm *timeinfo;
  time(&now);
  timeinfo = localtime(&now);
  
  char strftime_buf[64];
  strftime(strftime_buf, sizeof(strftime_buf), "%c", timeinfo);

  Blynk.virtualWrite(VIRTUAL_PIN_CURRENT_TIME, "Current Time: ", strftime_buf);
}

void setup() {
  Serial.begin(115200);
  
  tft.begin();
  tft.setRotation(1);
  
  connectToWiFi();
  syncTime();  

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  timer.setInterval(1000L, CurrentTimeToBlynk);
}

void loop() {
  Blynk.run();
  timer.run();
}
