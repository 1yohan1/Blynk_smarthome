// 최종

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <time.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <MHZ19.h>
#define BLYNK_PRINT Serial

unsigned long previousBlynkUpdateTime = 0;
unsigned long motorStartTime = 0;
bool motorRunning = false;

#define DHT_PIN 0
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);
MHZ19 mhz(&Serial2);
#define GAS_PIN 36
#define fanPin 5 
#define motor1 25
#define motor2 26
#define MOTOR_DELAY_MICROS 2000000

#define TFT_CS    15
#define TFT_DC    2
#define TFT_MOSI  23
#define TFT_CLK   18
#define TFT_RST   4
#define TFT_MISO  19
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

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

#define VIRTUAL_PIN_TEMPERATURE  V0
#define VIRTUAL_PIN_HUMIDITY     V1
#define VIRTUAL_PIN_CO2          V2
#define VIRTUAL_PIN_GAS          V3
#define VIRTUAL_PIN_FAN_STATE    V4
#define VIRTUAL_PIN_MOTOR_CONTROL  V5
#define VIRTUAL_PIN_CURRENT_TIME V6
#define VIRTUAL_PIN_RELAY_1_STATE V7
#define VIRTUAL_PIN_RELAY_2_STATE  V8
#define VIRTUAL_PIN_RELAY_3_STATE V9

int fanControl = 0;
int temperature = 0;

BlynkTimer timer;

char auth[] = ""; // Token 입력
char ssid[] = ""; // WIFI 입력
char pass[] = ""; // PASSWORD 입력

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600 * 9; // GMT+9
const int daylightOffset_sec = 0;

enum FanState {
  AUTO_OFF,
  AUTO_ON,
  MANUAL_ON,
  MANUAL_OFF
};

FanState fanState = MANUAL_OFF;

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

void sendSensorData() {
  temperature = dht.readTemperature();
  int humidity = dht.readHumidity();  

  MHZ19_RESULT response = mhz.retrieveData();
  int co2Value = -1; 

  if (response == MHZ19_RESULT_OK) {
    co2Value = mhz.getCO2();
  }
  int gasValue = analogRead(GAS_PIN);

  Blynk.virtualWrite(VIRTUAL_PIN_TEMPERATURE, temperature);
  Blynk.virtualWrite(VIRTUAL_PIN_HUMIDITY, humidity);
  Blynk.virtualWrite(VIRTUAL_PIN_CO2, co2Value);
  Blynk.virtualWrite(VIRTUAL_PIN_GAS, gasValue);

  if (gasValue > 700){
    Blynk.logEvent("gas_alert", "Gas Leakage Detected");
  }
  
  switch(fanState) {
    case AUTO_OFF:
      if (temperature > 21) {
        digitalWrite(fanPin, HIGH);
        Blynk.virtualWrite(VIRTUAL_PIN_FAN_STATE, digitalRead(fanPin));
        fanState = AUTO_ON;
      }
      break;

    case AUTO_ON:
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
  
  String currentTime = getFormattedTime();
  displayValuesOnTFT(currentTime, temperature, humidity, co2Value, gasValue);
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
  Serial2.begin(9600);
  delay(10);
  
  ledcAttachPin(25, 1);
  ledcAttachPin(26, 2);
  ledcSetup(1, 12000, 8);
  ledcSetup(2, 12000, 8);

  dht.begin();
  tft.begin();
  
  tft.setRotation(1);
  pinMode(fanPin, OUTPUT);
  pinMode(GAS_PIN, INPUT);

  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  pinMode(relay3Pin, OUTPUT);
  pinMode(toggleSwitch1Pin, INPUT_PULLUP);
  pinMode(toggleSwitch2Pin, INPUT_PULLUP);
  pinMode(toggleSwitch3Pin, INPUT_PULLUP);

  digitalWrite(relay1Pin, HIGH);
  digitalWrite(relay2Pin, HIGH);
  digitalWrite(relay3Pin, HIGH);

  connectToWiFi();
  syncTime();
  
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  timer.setInterval(1000L, onTimer);
}

void onTimer() {
  sendRelayState();
  sendCurrentTimeToBlynk();
  sendSensorData();
}
void loop() {
  Blynk.run();
  timer.run();
  checkMotorStatus();
  unsigned long currentMillis = millis();


  int toggle1State = digitalRead(toggleSwitch1Pin);
  int toggle2State = digitalRead(toggleSwitch2Pin);
  int toggle3State = digitalRead(toggleSwitch3Pin);

  if (toggle1State != lastToggle1State) {
    toggle1Relay(toggle1State);
    delay(100);
  }

  if (toggle2State != lastToggle2State) {
    toggle2Relay(toggle2State);
    delay(100);
  }

  if (toggle3State != lastToggle3State) {
    toggle3Relay();
    delay(100);
  }

  lastToggle1State = toggle1State;
  lastToggle2State = toggle2State;
  lastToggle3State = toggle3State;
  
}
