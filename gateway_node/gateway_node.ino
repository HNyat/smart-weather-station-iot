#include <FS.h>
#include <LittleFS.h>
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
// Điều khiển Thiết bị ngoại vi (Relay)
// =====================
#define RELAY_PIN D2
int relayState = 0; // 0: OFF, 1: ON

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

// ============================================================
// ★ MỚI: Điều khiển Relay
// ============================================================
void handleControl() {
  server.sendHeader("Access-Control-Allow-Origin", "*"); // CORS
  if (server.hasArg("relay")) {
    String val = server.arg("relay");
    if (val == "1") {
      relayState = 1;
      digitalWrite(RELAY_PIN, HIGH);
      Serial.println("[Relay] BẬT máy bơm");
    } else if (val == "0") {
      relayState = 0;
      digitalWrite(RELAY_PIN, LOW);
      Serial.println("[Relay] TẮT máy bơm");
    }
  }
  String json = "{\"relay\":";
  json += String(relayState);
  json += "}";
  server.send(200, "application/json", json);
}

// ============================================================
// ★ MỚI: Ghi log offline khi không có kết nối WiFi
// ============================================================
void logOfflineData() {
  File file = LittleFS.open("/offline.csv", "a");
  if (!file) {
    Serial.println("[LittleFS] Không mở được file để lưu offline");
    return;
  }
  file.printf("%.2f,%.2f,%.2f,%d,%d\n", temperature, humidity, pressure, rain, batteryPct);
  file.close();
  Serial.println("[LittleFS] Đã ghi dữ liệu cảm biến vào offline.csv");
}

// ============================================================
// ★ MỚI: Tự động gửi bù dữ liệu offline khi có mạng trở lại (Non-blocking)
// ============================================================
unsigned long lastSyncTime = 0;
const unsigned long SYNC_INTERVAL = 16000; // Giãn cách 16s để tránh bị ThingSpeak chặn rate limit
bool isSyncing = false;

void handleAutoSync() {
  if (WiFi.status() != WL_CONNECTED) {
    isSyncing = false;
    return;
  }
  
  if (millis() - lastSyncTime < SYNC_INTERVAL) {
    return;
  }

  if (!LittleFS.exists("/offline.csv")) {
    isSyncing = false;
    return;
  }

  File file = LittleFS.open("/offline.csv", "r");
  if (!file) {
    isSyncing = false;
    return;
  }

  if (!file.available()) {
    file.close();
    LittleFS.remove("/offline.csv");
    Serial.println("[Sync] File offline trống, đã xóa.");
    isSyncing = false;
    return;
  }

  isSyncing = true;
  
  // Đọc dòng đầu tiên
  String line = file.readStringUntil('\n');
  line.trim();

  // Kiểm tra dữ liệu dòng
  if (line.length() == 0) {
    file.close();
    // Tạo file tạm để chép phần còn lại
    File tempFile = LittleFS.open("/offline_temp.csv", "w");
    while (file.available()) {
      String remainingLine = file.readStringUntil('\n');
      tempFile.println(remainingLine);
    }
    file.close();
    tempFile.close();
    LittleFS.remove("/offline.csv");
    LittleFS.rename("/offline_temp.csv", "/offline.csv");
    lastSyncTime = millis();
    return;
  }

  // Sao chép các dòng còn lại vào file tạm
  bool hasMore = file.available() > 0;
  File tempFile;
  if (hasMore) {
    tempFile = LittleFS.open("/offline_temp.csv", "w");
  }

  while (file.available()) {
    String remainingLine = file.readStringUntil('\n');
    if (tempFile) {
      tempFile.println(remainingLine);
    }
  }

  file.close();
  if (tempFile) {
    tempFile.close();
  }

  // Thay thế file offline bằng file tạm chứa các phần còn lại
  LittleFS.remove("/offline.csv");
  if (hasMore) {
    LittleFS.rename("/offline_temp.csv", "/offline.csv");
  }

  // Tách dữ liệu: temp,hum,pres,rain,batteryPct
  int comma1 = line.indexOf(',');
  int comma2 = line.indexOf(',', comma1 + 1);
  int comma3 = line.indexOf(',', comma2 + 1);
  int comma4 = line.indexOf(',', comma3 + 1);

  if (comma1 == -1 || comma2 == -1 || comma3 == -1 || comma4 == -1) {
    Serial.printf("[Sync] Định dạng dòng lỗi: %s. Đã bỏ qua.\n", line.c_str());
    lastSyncTime = millis();
    return;
  }

  float offline_temp = line.substring(0, comma1).toFloat();
  float offline_hum = line.substring(comma1 + 1, comma2).toFloat();
  float offline_pres = line.substring(comma2 + 1, comma3).toFloat();
  int offline_rain = line.substring(comma3 + 1, comma4).toInt();
  int offline_bat = line.substring(comma4 + 1).toInt();

  Serial.printf("[Sync] Đang gửi bù: Temp=%.2f, Hum=%.2f, Pres=%.2f, Rain=%d, Bat=%d%%\n", 
                offline_temp, offline_hum, offline_pres, offline_rain, offline_bat);

  WiFiClient client;
  HTTPClient http;

  String url = String("http://") + THINGSPEAK_HOST + "/update?api_key=" + THINGSPEAK_API_KEY;
  url += "&field1=" + String(offline_temp, 2);
  url += "&field2=" + String(offline_hum, 2);
  url += "&field3=" + String(offline_pres, 2);
  url += "&field4=" + String(offline_rain);
  url += "&field5=" + String(offline_bat);

  http.begin(client, url);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String response = http.getString();
    Serial.printf("[Sync] Gửi bù thành công: HTTP %d - entry_id=%s\n", httpCode, response.c_str());
  } else {
    Serial.printf("[Sync] Gửi bù thất bại: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();

  lastSyncTime = millis();
}

// =====================
// Setup Hệ thống
// =====================
void setup() {
  Serial.begin(115200);

  // Cấu hình chân Relay
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Mặc định tắt máy bơm

  // Bật LittleFS
  if(!LittleFS.begin()) {
    Serial.println("LittleFS Mount Failed");
  } else {
    Serial.println("LittleFS Mounted Successfully");
  }

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
  server.on("/control", handleControl); // ★ MỚI
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

  // Tự động kiểm tra và gửi bù dữ liệu ngoại tuyến khi có mạng
  handleAutoSync();

  int packetSize = LoRa.parsePacket();
  if (packetSize == sizeof(WeatherPkt_t)) { // Kiểm tra gói tin có đúng kích thước không
    WeatherPkt_t pkt;
    LoRa.readBytes((uint8_t*)&pkt, sizeof(pkt)); // Đọc toàn bộ cục Binary vào Struct

    // Giải mã: Phục hồi số thập phân (chia lại cho 100 và 10 như lúc Sensor Node nhân lên)
    temperature = pkt.temperature / 100.0f;
    humidity = pkt.humidity / 100.0f;
    pressure = (pkt.pressure / 10.0f) + 800.0f;
    rain = pkt.rain_analog;
    batteryPct = pkt.battery_pct;   
    rssi = LoRa.packetRssi();       
    seqNum = pkt.seq_num;           

    Serial.println("--- GÓI TIN MỚI ---");
    Serial.print("Nhiệt độ: "); Serial.print(temperature); Serial.println(" C");
    Serial.print("Độ ẩm: "); Serial.print(humidity); Serial.println(" %");
    Serial.print("Áp suất: "); Serial.print(pressure); Serial.println(" hPa");
    Serial.print("Mưa: "); Serial.println(rain);
    Serial.printf("RSSI: %d dBm | Seq: %d\n", rssi, seqNum);

    // Gửi lên ThingSpeak nếu có WiFi và không bận gửi bù, ngược lại thì lưu offline
    if (WiFi.status() == WL_CONNECTED && !isSyncing) {
      sendToThingSpeak();   
    } else {
      logOfflineData();
    }
  } else if (packetSize > 0) {
    Serial.println("Nhận được gói tin sai kích thước (Rác).");
  }
}
