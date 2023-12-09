# Blynk & IFTTT Smart Home
- Blynk와 IFTTT 플랫폼을 활용하여 스마트 디바이스 및 응용 프로그램과 Google Assistant를 통한 음성 제어 관리하는 시스템을 구축.

## 프로젝트 목적
- 원격 제어를 통한 편리함과 에너지 절약
- Gas 누출 알람을 통한 화재 예방

### 개발 보드
- ESP32 Dev Module

### 개발 툴
- Arduino IDE 1.18.19

### 플랫폼
- Blynk 2.0 & IFTTT



## 기능
-  DHT11, MH-Z19B, MQ-2 센서로 Temperature, Humidity, Co2 ppm, Gas value 및 NTP 서버를 통한 시간 정보를 TFT-LCD 화면에 디스플레이 하도록 구현.
- Gas value가 700 이상일 시 가스 누출 경고 메세지 이메일 전송 및 알람 기능 추가.
- Co2 ppm이 1000 이상일 시 환기 필요 메세지 이메일 전송 및 알람 기능 추가.

-  Fan을 air conditioner 기능으로 사용.
-  Temperature value 26'c 이상 자동 Fan 모듈 on
-  26'c 미만 자동 Fan 모듈 off
-  26'c 이상일 때 Blynk 서버에서 수동으로 Fan 모듈 off 기능 추가.

-  DC 모터와 L298N 모터 드라이버를 통해 PWM(속도) 및 방향 제어 기능 추가. 블라인드 커튼을 구현. 

-  220v 전구 2개를 설치 후 릴레이로 전구를 제어하는 기능 추가.
래치 스위치 및 Blynk 가상 핀 스위치를 활용함. 전구 2개를 동시에 제어 가능한 래치 스위치 및 Blynk 가상 핀 스위치 하나를 추가.


## 향후 계획
- IFTTT 프로토콜을 이용하여 음성인식 기능 통해 220v 전구 제어 및 에어컨 동작, 블라인드 커튼 등 음성으로 제어할 예정.

- 외부 전원 공급 이슈 해결.
