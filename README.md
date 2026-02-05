# TSN Proxy ESP32

ESP32 DevKit V1을 이용한 TSN(Time-Sensitive Networking) Proxy 프로젝트입니다. WiFi UDP와 UART2(LAN9692) 간의 브릿지 역할을 수행합니다.

## 프로젝트 개요

이 프로젝트는 ESP32 DevKit V1 보드를 사용하여 다음 기능을 제공합니다:
- WiFi를 통한 PC 핫스팟 연결
- UDP 통신 (포트 5683)
- UART2(RX:16, TX:17)를 통한 LAN9692 칩 통신
- UDP ↔ UART2 양방향 데이터 브릿지

## 하드웨어 요구사항

- ESP32 DevKit V1 보드
- USB 케이블 (Type-B 또는 Micro-USB)
- LAN9692 모듈 (UART2 연결)

## 소프트웨어 설치 (Windows)

### 1. Visual Studio Code 설치

1. [Visual Studio Code 공식 웹사이트](https://code.visualstudio.com/)에서 다운로드
2. 설치 파일을 실행하고 설치 진행
3. 설치 중 "Add to PATH" 옵션 선택 권장

### 2. PlatformIO IDE 확장 설치

1. VSCode 실행
2. 왼쪽 사이드바에서 **Extensions** (확장) 아이콘 클릭 (`Ctrl+Shift+X`)
3. 검색창에 **"PlatformIO IDE"** 입력
4. **PlatformIO IDE** 확장 찾아서 **Install** 클릭
5. 설치 완료 후 VSCode 재시작
6. 왼쪽 사이드바에 PlatformIO 아이콘이 나타나면 설치 완료

### 3. USB 드라이버 설치 (필요시)

ESP32 DevKit V1은 CP2102 또는 CH340 USB-UART 칩을 사용합니다:

- **CP2102**: [Silicon Labs 드라이버](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
- **CH340**: [CH340 드라이버](http://www.wch.cn/downloads/CH341SER_ZIP.html)

## 프로젝트 설정

### 1. 프로젝트 클론 또는 다운로드

```bash
git clone https://github.com/hrkim-KETI/TSN_Proxy_ESP32.git
cd TSN_Proxy_ESP32
```

### 2. VSCode에서 프로젝트 열기

1. VSCode 실행
2. **File > Open Folder** 선택
3. 클론한 프로젝트 폴더 선택

### 3. WiFi 설정 수정

`src/main.cpp` 파일에서 WiFi 설정을 환경에 맞게 수정:

```cpp
// WiFi 핫스팟 설정
const char* ssid = "your-wifi-ssid";     // 핫스팟 SSID
const char* password = "your-password";  // 핫스팟 비밀번호

// ESP32 고정 IP 설정 (필요시 변경)
IPAddress local_IP(10, 42, 0, 13);  // ESP32 IP 주소
IPAddress gateway(10, 42, 0, 1);     // 게이트웨이 (PC IP)
IPAddress subnet(255, 255, 255, 0);  // 서브넷 마스크
```

### 4. 디버그 모드 활성화 (선택사항)

디버그 메시지를 시리얼 모니터로 확인하려면 `src/main.cpp`에서:

```cpp
// 주석 해제
#define DEBUG
```

## 빌드 및 업로드

### 방법 1: PlatformIO 툴바 사용

1. VSCode 하단의 PlatformIO 툴바 확인
2. **Build** (체크마크 아이콘) 클릭 - 프로젝트 빌드
3. ESP32 보드를 USB로 PC에 연결
4. **Upload** (화살표 아이콘) 클릭 - 펌웨어 업로드
5. **Serial Monitor** (플러그 아이콘) 클릭 - 시리얼 모니터 열기

### 방법 2: 명령 팔레트 사용

1. `Ctrl+Shift+P`로 명령 팔레트 열기
2. 다음 명령어 실행:
   - **PlatformIO: Build** - 빌드
   - **PlatformIO: Upload** - 업로드
   - **PlatformIO: Serial Monitor** - 시리얼 모니터

### 방법 3: 터미널 명령어 사용

```bash
# 빌드
pio run

# 업로드
pio run --target upload

# 시리얼 모니터
pio device monitor

# 빌드 + 업로드 + 모니터
pio run --target upload && pio device monitor
```

## 시리얼 모니터 출력 예시

디버그 모드가 활성화된 경우:

```
--- [DevKit V1] Zonal Bridge Mode Start ---
[SYSTEM] UART2 Configured: RX=16, TX=17
[SYSTEM] Connecting to PC Hotspot.....
[SYSTEM] WiFi Connected!
[SYSTEM] ESP32 IP: 10.42.0.13
[SYSTEM] UDP Listening on Port 5683...
[UDP->UART2] Forwarding 128 bytes
[UART2->UDP] Sending 256 bytes to PC
```

## 핀 연결

| ESP32 핀 | 기능 | 연결 대상 |
|---------|------|-----------|
| GPIO 16 | UART2 RX | LAN9692 TX |
| GPIO 17 | UART2 TX | LAN9692 RX |
| GND | Ground | LAN9692 GND |

**참고**: Power는 각 디바이스에 별도로 인가

## 통신 프로토콜

- **UDP 포트**: 5683
- **UART 속도**: 115200 baud
- **시리얼 포맷**: 8N1 (8 data bits, No parity, 1 stop bit)

## 문제 해결

### 업로드 실패

1. **COM 포트 확인**:
   - Windows: 장치 관리자에서 포트(COM & LPT) 확인
   - USB 케이블이 데이터 전송 지원하는지 확인 (일부 케이블은 충전만 가능)

2. **부트 모드 진입**:
   - ESP32의 BOOT 버튼을 누른 채로 업로드 시도
   - 업로드 시작 메시지가 나오면 BOOT 버튼 해제

3. **드라이버 재설치**:
   - 장치 관리자에서 USB Serial 드라이버 제거 후 재설치

### WiFi 연결 실패

- SSID와 비밀번호가 정확한지 확인
- 핫스팟이 2.4GHz 대역인지 확인 (ESP32는 5GHz 미지원)
- 시리얼 모니터로 연결 상태 확인

### 시리얼 모니터 출력 없음

- 디버그 모드(`#define DEBUG`) 활성화 확인
- 보드레이트가 115200으로 설정되었는지 확인

## 기여

버그 리포트 및 기능 제안은 Issues 탭을 이용해주세요.
