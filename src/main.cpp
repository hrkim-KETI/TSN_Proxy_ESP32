#include <Arduino.h>
#include <WiFi.h>
#include <AsyncUDP.h>

// ==========================================
// 디버그 모드 설정 
// #define DEBUG 
// ==========================================

// 디버그용 매크로 함수 정의
#ifdef DEBUG
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
  #define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINTF(...)
#endif

// 1. PC 핫스팟 설정
const char* ssid = "keti-TSN";     
const char* password = "keti1234"; 

// 2. IP 설정
IPAddress local_IP(10, 42, 0, 11); 
// IPAddress local_IP(10, 42, 0, 12); 
// IPAddress local_IP(10, 42, 0, 13); 
// IPAddress local_IP(10, 42, 0, 14); 
IPAddress gateway(10, 42, 0, 1);
IPAddress subnet(255, 255, 255, 0);

AsyncUDP udp;
IPAddress remoteUdpIp;
uint16_t remoteUdpPort;
bool clientConnected = false;

void setup() {
    // 시리얼 초기화
    Serial.begin(115200);
    delay(2000);
    
    DEBUG_PRINTLN("\n--- [DevKit V1] Zonal Bridge Mode Start ---");

    // LAN9692 통신용 UART2 (RX:16, TX:17)
    Serial2.begin(115200, SERIAL_8N1, 16, 17);
    DEBUG_PRINTLN("[SYSTEM] UART2 Configured: RX=16, TX=17");

    WiFi.mode(WIFI_STA);
    if (!WiFi.config(local_IP, gateway, subnet)) {
        DEBUG_PRINTLN("[ERROR] Static IP Configuration Failed");
    }

    WiFi.begin(ssid, password);
    DEBUG_PRINT("[SYSTEM] Connecting to PC Hotspot");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        DEBUG_PRINT(".");
    }

    DEBUG_PRINTLN("\n[SYSTEM] WiFi Connected!");
    DEBUG_PRINT("[SYSTEM] ESP32 IP: ");
    DEBUG_PRINTLN(WiFi.localIP());

    if (udp.listen(5683)) {
        DEBUG_PRINTLN("[SYSTEM] UDP Listening on Port 5683...");

        udp.onPacket([](AsyncUDPPacket packet) {
            remoteUdpIp = packet.remoteIP();
            remoteUdpPort = packet.remotePort();
            clientConnected = true;

            // [UDP -> UART2] 데이터 포워딩 (실제 데이터 전송은 유지)
            DEBUG_PRINTF("[UDP->UART2] Forwarding %d bytes\n", packet.length());
            Serial2.write(packet.data(), packet.length());
        });
    }
}

void loop() {
    static uint8_t buf[2048];
    static size_t len = 0;
    static unsigned long lastReceiveTime = 0;

    // available한 바이트만 즉시 읽기 (대기 없음)
    while (Serial2.available() && len < sizeof(buf)) {
        buf[len++] = Serial2.read();
        lastReceiveTime = millis();
    }

    // 데이터가 있고, 5ms 동안 추가 데이터가 없으면 프레임 완료로 판단하여 전송
    if (len > 0 && (millis() - lastReceiveTime) >= 5) {
        DEBUG_PRINTF("[UART2->UDP] Sending %d bytes to PC\n", len);

        if (clientConnected) {
            udp.writeTo(buf, len, remoteUdpIp, remoteUdpPort);
        } else {
            udp.writeTo(buf, len, gateway, 5683);
        }

        len = 0;  // 버퍼 초기화
    }
}