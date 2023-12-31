# Blynk & Google Assistant Smart Home
- Blynk와 IFTTT 플랫폼을 활용하여 스마트 디바이스 및 응용 프로그램과 Google Assistant를 통한 음성 제어 관리하는 시스템을 구축.

## 프로젝트 목적
- 원격 제어를 통한 편리함
- 안전성(화재 예방)
- 에너지 절약

### 개발 보드
- ESP32 Dev Module

### 개발 툴
- Arduino IDE 1.18.19

### 플랫폼
- Blynk 2.0 & IFTTT



## 기능
#### DHT11(온습도), MH-Z19B(이산화탄소), MQ-2(가스) 센서로 Temperature, Humidity, Co2 ppm, Gas value 및 NTP 서버를 통한 시간 정보를 TFT-LCD 화면에 디스플레이 하도록 구현하고 Blynk Web dashboard 설정
![image](https://github.com/1yohan1/Blynk_smarthome/assets/149892068/c5ae57ed-592e-4208-9615-bca8660cabd9)

![image](https://github.com/1yohan1/Blynk_smarthome/assets/149892068/70f6c6a6-d975-4c2b-a43e-b16ada3d19f6)




#### Gas value가 700 이상일 시 가스 누출 경고 메세지 이메일 전송 및 알람 기능 추가.
``` C++
  if (gasValue > 700){
    Blynk.logEvent("gas_alert", "Gas Leakage Detected");
  }
```
![image](https://github.com/1yohan1/Blynk_smarthome/assets/149892068/88ac1d3c-5788-4549-9f92-fef8ba6be240)
![image](https://github.com/1yohan1/Blynk_smarthome/assets/149892068/290d38f1-ec9f-473b-ab94-058a0046c901)


#### Co2 ppm이 1500 이상일 시 환기 필요 메세지 이메일 전송 및 알람 기능 추가.
``` C++
  if (co2Value > 1500 ) {
    Blynk.logEvent("co2_alert", "Co2 is too high Let's ventilate");
  }
```

![image](https://github.com/1yohan1/Blynk_smarthome/assets/149892068/8d9bbe40-e5a4-491a-b4ac-208d5294bf97)
![image](https://github.com/1yohan1/Blynk_smarthome/assets/149892068/1f7ec37a-0910-47f5-b2dd-a621ee13eaa2)


#### Fan을 air conditioner 기능으로 사용.
- Temperature value 26'c 이상 자동 Fan 모듈 on
- 26'c 미만 자동 Fan 모듈 off
- 26'c 이상일 때 Blynk 서버에서 수동으로 Fan 모듈 off 기능 추가.
``` C++
enum FanState {
  AUTO_OFF,
  AUTO_ON,
  MANUAL_ON,
  MANUAL_OFF
};
  switch(fanState) {
    case AUTO_OFF:
      if (temperature > 25) {
        digitalWrite(fanPin, HIGH);
        Blynk.virtualWrite(VIRTUAL_PIN_FAN_STATE, digitalRead(fanPin));
        fanState = AUTO_ON;
      }
      break;

    case AUTO_ON:
      if (temperature <= 25) {
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
      if (fanControl == 0 && temperature > 25) {
        fanState = AUTO_ON;
      } else {
        fanState = AUTO_OFF;
      }
      break;
  }
```

#### DC 모터와 L298N 모터 드라이버를 통해 PWM(속도) 및 방향 제어 기능 추가. 블라인드 커튼을 구현.
  - 정방향, 역방향 및 속도값은 180으로 줬다.
``` C++
BLYNK_WRITE(VIRTUAL_PIN_MOTOR_CONTROL) {
  int val = param.asInt();
  
  if (val == 1) {
    ledcWrite(1, 0);
    ledcWrite(2, 180);
    motorStartTime = millis();  
    motorRunning = true;        
  } else {
    ledcWrite(1, 180);
    ledcWrite(2, 0);
    motorStartTime = millis();  
    motorRunning = true;        
  }
}
  
void setup(){
  ledcAttachPin(25, 1);
  ledcAttachPin(26, 2);
  ledcSetup(1, 12000, 8);
  ledcSetup(2, 12000, 8);
}
```

![image](https://github.com/1yohan1/Blynk_smarthome/assets/149892068/40922442-e14b-464e-a228-85ca1ca1d703)



#### 릴레이와 전구를 연결하여 Blynk 및 회로 스위치로 전구를 제어
- 1번 릴레이 = 거실 전구 제어
- 2번 릴레이 = 안방 전구 제어
- 3번 릴레이 = 거실 & 안방 전구 동시 제어

![image](https://github.com/1yohan1/Blynk_smarthome/assets/149892068/866bb528-b6c5-495b-baaf-768e34e8ff9e)




  ## IFTTT를 통한 음성인식 기능 추가
  ![image](https://github.com/1yohan1/Blynk_smarthome/assets/149892068/caa91496-c739-4192-afff-c5a87303daa7)

  #### Google Assistant 음성으로 제어

![image](https://github.com/1yohan1/Blynk_smarthome/assets/149892068/a327d61e-ec71-4fef-a9a4-a8acc2298972)

 
  - 에어컨 모듈 on/off
  - 블라인드 커튼 close/open
  - 릴레이 & 전구 on/off



## 향후 계획
### Temperature, Humidity, Co2 ppm, Gas value 정보를 Blynk의 가상핀 값으로 get해서 Google Assistant 음성으로 제공받기
  - IFTTT의 Then That은 현재 Google Assistant를 지원하지 않음
  - Google Actions로 사용자 지정 응답, 작업, 인텐트 등을 정의하여 Google Assistant를 통해 특정 명령을 실행할 때 수행되는 작업을 설정해야함

### 음성 한국 버전
- 현재 Google Assistant는 한국 버전을 제공하지 않는다.
- 다른 플랫폼을 사용하여 해결할 예정. () 

### TFT-LCD 문제
- 현재 Blynk 업데이트 주기를 1초로 설정해두었는데 TFT-LCD가 디스플레이되고 클리어되는 동안 모든 동작이 멈추게 되어 Blynk 업데이트가 2초로 주기가 강제 변경됨
- 
  ``` C++
  timer.setInterval(1000L, onTimer);
  ```
  
### TFT-LCD 해결방안
- Mutex, Semaphor를 통해 해결해보려함
``` C++
 TaskHandle_t tftTask;

void tftTaskFunction(void *pvParameters) {
  while (1) {
    portENTER_CRITICAL(&tftMutex);
    

    String displayStringCopy = tftDisplayString;
    tft.fillScreen(ILI9341_NAVY); 
    tft.drawRoundRect(0,0,319,239,10, ILI9341_WHITE); 
    tft.setCursor(0, 0);
    tft.setTextSize(3);
    tft.setTextColor(ILI9341_WHITE); 
    tft.print(displayStringCopy);
    
    portEXIT_CRITICAL(&tftMutex);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void setup() {
  // TFTLCD 작업을 별도의 코어에 고정
  xTaskCreatePinnedToCore(
    tftTaskFunction,   // 작업 함수
    "TFT Task",        // 작업 이름
    10000,             // 스택 크기 (조절 필요)
    NULL,              // 매개변수 (여기서는 사용하지 않음)
    1,                 // 우선순위
    &tftTask,          // 작업 핸들
    0                  // 코어 번호 (0 또는 1, 여기서는 0을 사용)
  );
}
 ```
TFT-LCD 클리어 도중 멈추는 현상이 발생

![image](https://github.com/1yohan1/Blynk_smarthome/assets/149892068/00deb2de-0cea-4ac5-ab48-bb7b4b7137dd)



- TFT-LCD 클리어 방식 모색 중
듀얼 채널 할당 후 바뀌기 전 화면, 바뀐 후 화면을 번갈아가며 디스플레이하여 블라인드 클리어 방식을 변경.
  
- 다른 LCD 모듈 사용, Liquid Crystal Display, Organic Light Emitting Diodes 등
### 자동화 서비스 도입
현재 자동화 시스템이 도입된 것은 에어컨 자동 On/Off기능밖에 없다.
추가로 신발장 아래 물체감지센서와 출입문 도어락 감지센서를 통해 집으로 들어오면 전구가 일괄적으로 켜지고, 집을 나가면 일괄적으로 꺼지는 기능을 구현할 예정.

또한, 09시는 블라인드가 자동으로 올라가고 21시는 자동으로 내려가는 기능을 구현할 예정
작품 시연을 위해 홀수분일 때 블라인드가 올라가고 짝수분일 때 블라인드가 내려가도록 구현할 예정정  
