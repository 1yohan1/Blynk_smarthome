# Blynk & IFTTT Smart Home
- Blynk와 IFTTT 플랫폼을 활용하여 스마트 디바이스 및 응용 프로그램과 Google Assistant를 통한 음성 제어 관리하는 시스템을 구축.

## 프로젝트 목적
- 원격 제어를 통한 편리함과 에너지 절약

### 개발 보드
- ESP32 Dev Module

### 개발 툴
- Arduino IDE 1.18.19

### 플랫폼
- Blynk 2.0 & IFTTT



## 기능
- DHT11(온습도), MH-Z19B(이산화탄소), MQ-2(가스) 센서로 Temperature, Humidity, Co2 ppm, Gas value 및 NTP 서버를 통한 시간 정보를 TFT-LCD 화면에 디스플레이 하도록 구현.
- Gas value가 700 이상일 시 가스 누출 경고 메세지 이메일 전송 및 알람 기능 추가.
``` C++
  if (gasValue > 700){
    Blynk.logEvent("gas_alert", "Gas Leakage Detected");
  }
```
![image](https://github.com/1yohan1/Blynk_smarthome/assets/149892068/88ac1d3c-5788-4549-9f92-fef8ba6be240)

- Co2 ppm이 1000 이상일 시 환기 필요 메세지 이메일 전송 및 알람 기능 추가.
``` C++
  if (co2Value > 1000 ) {
    Blynk.logEvent("co2_alert", "Co2 is too high Let's ventilate");
  }
```

- Fan을 air conditioner 기능으로 사용.
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
```

- DC 모터와 L298N 모터 드라이버를 통해 PWM(속도) 및 방향 제어 기능 추가. 블라인드 커튼을 구현.
  정방향, 역방향 및 속도값은 180으로 줬다.
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

- 릴레이와 전구를 연결하여 Blynk 및 회로 스위치로 전구를 제어


## 향후 계획
- IFTTT 프로토콜을 이용하여 음성인식 기능 통해 220v 전구 제어 및 에어컨 동작, 블라인드 커튼 등 음성으로 제어할 예정.

- 외부 전원 공급 이슈 해결.
