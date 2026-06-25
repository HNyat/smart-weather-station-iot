#include <SPI.h>
#include "LoRa.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>   // ★ MỚI: gửi HTTP lên ThingSpeak
#include <WiFiClient.h>          // ★ MỚI
#include "weather_html.h"        // ★ MỚI: Lưu toàn bộ HTML/CSS/JS ở tab này

// =====================
// Chân kết nối LoRa
// =====================
#define SS    D8
#define RST   D0
#define DIO0  D1

// =====================
// Cấu hình WiFi Access Point (giữ nguyên — user cục bộ)
// =====================
const char* ssid = "WeatherStation";
const char* password = "12345678";
ESP8266WebServer server(80);

// =====================
// ★ MỚI: Cấu hình WiFi Station (để có Internet đẩy ThingSpeak)
// =====================
const char* staSsid     = "TEN_WIFI_NHA_BAN";      // ← ĐIỀN WiFi nhà bạn
const char* staPassword = "MAT_KHAU_WIFI_NHA_BAN"; // ← ĐIỀN mật khẩu

// =====================
// ★ MỚI: Cấu hình ThingSpeak
// =====================
const char* THINGSPEAK_API_KEY = "IWSWJL8BLKHHGSG2"; // ← ĐIỀN Write API Key
const char* THINGSPEAK_HOST    = "api.thingspeak.com";
unsigned long lastTSsend = 0;
const unsigned long TS_MIN_INTERVAL_MS = 16000; // Free tier: tối thiểu 15s/lần, dư 1s an toàn

// =====================
// Biến lưu trữ Dữ liệu thời tiết
// =====================
float temperature = 0;
float humidity = 0;
float pressure = 0;
int rain = 0;
uint8_t batteryPct = 0;   // ★ MỚI: lưu battery để đẩy lên ThingSpeak
int rssi = -72;           // ★ MỚI: lưu RSSI của sóng LoRa
uint8_t seqNum = 0;       // ★ MỚI: lưu Mã Gói (Sequence Number) nhận từ LoRa

// ============================================================
// KHAI BÁO CẤU TRÚC GÓI TIN NHỊ PHÂN (Phải khớp 100% với Trạm Phát)
// ============================================================
#pragma pack(push, 1)
typedef struct {
  uint16_t node_id;
  uint8_t  seq_num;
  uint8_t  flags;
  int16_t  temperature;
  uint16_t humidity;
  uint16_t pressure;
  uint16_t rain_analog;  
  uint8_t  battery_pct;  
  uint16_t crc16;
} WeatherPkt_t;
#pragma pack(pop)

// =====================
// Xử lý Web Root (Giao diện Dashboard Pro)
// =====================
void handleRoot() { 
  server.send_P(200, "text/html", htmlPage); 
}

void handleData() {
  server.sendHeader("Access-Control-Allow-Origin", "*"); // Cho phép CORS khi test Web từ laptop
  String json = "{";
  json += "\"temperature\":"; json += String(temperature,1);
  json += ",\"humidity\":"; json += String(humidity,1);
  json += ",\"pressure\":"; json += String(pressure,1);
  json += ",\"rain\":"; json += String(rain);
  json += ",\"battery_pct\":"; json += String(batteryPct); // Bổ sung thông tin pin gửi từ LoRa
  json += ",\"rssi\":"; json += String(rssi);               // ★ MỚI: gửi RSSI thực tế lên Dashboard Local
  json += ",\"seq_num\":"; json += String(seqNum);          // ★ MỚI: gửi SeqNum thực tế lên Dashboard Local
  json += "}";
  server.send(200,"application/json",json);
}

// ============================================================
// ★ MỚI: Đẩy dữ liệu lên ThingSpeak (field1-5), field6-8 do AI ghi
// ============================================================
void sendToThingSpeak() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[ThingSpeak] Bỏ qua: WiFi STA chưa có Internet");
    return;
  }
  if (millis() - lastTSsend < TS_MIN_INTERVAL_MS) {
    return; // Chưa đủ 15s từ lần gửi trước — bỏ qua, không log để khỏi rác Serial
  }

  WiFiClient client;
  HTTPClient http;

  String url = String("http://") + THINGSPEAK_HOST + "/update?api_key=" + THINGSPEAK_API_KEY;
  url += "&field1=" + String(temperature, 2);
  url += "&field2=" + String(humidity, 2);
  url += "&field3=" + String(pressure, 2);
  url += "&field4=" + String(rain);
  url += "&field5=" + String(batteryPct);

  http.begin(client, url);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String response = http.getString(); // entry_id nếu thành công, "0" nếu lỗi
    Serial.printf("[ThingSpeak] HTTP %d - entry_id=%s\n", httpCode, response.c_str());
  } else {
    Serial.printf("[ThingSpeak] Gửi thất bại: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
  lastTSsend = millis();
}

// =====================
// Setup Hệ thống
// =====================
void setup() {
  Serial.begin(115200);

  // ★ SỬA: chuyển sang AP_STA — vừa phát AP nội bộ, vừa kết nối Internet
  WiFi.mode(WIFI_AP_STA);

  // Phát WiFi AP (giữ nguyên — user cục bộ vẫn dùng được dù không có Internet)
  WiFi.softAP(ssid, password);
  Serial.println("\nAccess Point Started: WeatherStation");
  Serial.print("IP Web Dashboard: "); Serial.println(WiFi.softAPIP());

  // ★ MỚI: Kết nối STA vào WiFi nhà để có Internet
  Serial.printf("Đang kết nối STA tới '%s'", staSsid);
  WiFi.begin(staSsid, staPassword);
  unsigned long staStart = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - staStart < 15000) {
    delay(300);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nSTA kết nối OK — IP: " + WiFi.localIP().toString());
  } else {
    Serial.println("\nSTA timeout — chạy OFFLINE (vẫn AP + Web local bình thường)");
  }

  // Bật Web Server
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();

  // Bật LoRa
  LoRa.setPins(SS, RST, DIO0);
  if(!LoRa.begin(433E6)) {
    Serial.println("LoRa Init Failed");
    while(true);
  }
  LoRa.setSyncWord(0xAB); // Mật khẩu đồng bộ
  Serial.println("LoRa Ready - Đang chờ gói tin nhị phân...");
}

// =====================
// Vòng lặp chính (Loop)
// =====================
void loop() {
  server.handleClient(); // Duy trì Web Server

  int packetSize = LoRa.parsePacket();
  if (packetSize == sizeof(WeatherPkt_t)) { // Kiểm tra gói tin có đúng kích thước không
    WeatherPkt_t pkt;
    LoRa.readBytes((uint8_t*)&pkt, sizeof(pkt)); // Đọc toàn bộ cục Binary vào Struct

    // Giải mã: Phục hồi số thập phân (chia lại cho 100 và 10 như lúc Sensor Node nhân lên)
    temperature = pkt.temperature / 100.0f;
    humidity = pkt.humidity / 100.0f;
    pressure = (pkt.pressure / 10.0f) + 800.0f;
    rain = pkt.rain_analog;
    batteryPct = pkt.battery_pct;   // ★ MỚI
    rssi = LoRa.packetRssi();       // ★ MỚI: Đọc RSSI LoRa
    seqNum = pkt.seq_num;           // ★ MỚI: Đọc Mã gói (Seq) LoRa

    Serial.println("--- GÓI TIN MỚI ---");
    Serial.print("Nhiệt độ: "); Serial.print(temperature); Serial.println(" C");
    Serial.print("Độ ẩm: "); Serial.print(humidity); Serial.println(" %");
    Serial.print("Áp suất: "); Serial.print(pressure); Serial.println(" hPa");
    Serial.print("Mưa: "); Serial.println(rain);
    Serial.printf("RSSI: %d dBm | Seq: %d\n", rssi, seqNum);

    sendToThingSpeak();   // ★ MỚI: đẩy lên cloud ngay sau khi giải mã xong
  } else if (packetSize > 0) {
    Serial.println("Nhận được gói tin sai kích thước (Rác).");
  }
}
