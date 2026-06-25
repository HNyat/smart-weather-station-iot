/**
 * ============================================================
 * LoRa P2P Weather Station — SENSOR NODE Firmware
 * Board   : ESP8266
 * Sensors : DHT11 (D2) + BMP280 (D4, D3) + Rain Gauge (A0)
 * LoRa    : SX1278 (NSS:D8, DIO0:D1)
 * ============================================================ 
 */
#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

// Sử dụng ngoặc kép (Local Include) để lách lỗi đường dẫn thư viện
#include "Adafruit_BMP280.h"
#include "DHT.h"

// ============================================================
//  CẤU HÌNH HỆ THỐNG
// ============================================================
#define NODE_ID          0x0001
#define SLEEP_SEC        60           // Thời gian ngủ Deep Sleep (Giây)
#define MAX_TX_RETRY     3
#define ACK_TIMEOUT_MS   2500
#define TX_TIMEOUT_MS    2000
#define LORA_FREQ_HZ     433000000UL  // Tần số SX1278
#define LORA_SYNC_WORD   0xAB

// --- Pinout ---
#define LORA_NSS    D8   // GPIO15
#define LORA_DIO0   D1   // GPIO5
#define LORA_RST    -1   

#define I2C_SDA     D4   // GPIO2
#define I2C_SCL     D3   // GPIO0
#define BMP280_ADDR 0x76

#define DHT_PIN     D2   // GPIO4
#define DHT_TYPE    DHT11 

#define RAIN_PIN    A0   // Analog 0

// --- Ngưỡng cảnh báo ---
#define TEMP_ALARM_THRESHOLD   40.0f
#define RAIN_ALARM_THRESHOLD   500    
#define BATT_LOW_THRESHOLD_V   3.3f

// --- Thanh ghi SX1278 ---
#define REG_FIFO             0x00
#define REG_OP_MODE          0x01
#define REG_FR_MSB           0x06
#define REG_FR_MID           0x07
#define REG_FR_LSB           0x08
#define REG_PA_CONFIG        0x09
#define REG_FIFO_ADDR        0x0D
#define REG_FIFO_TX_BASE     0x0E
#define REG_FIFO_RX_BASE     0x0F
#define REG_FIFO_RX_CURR     0x10
#define REG_IRQ_FLAGS        0x12
#define REG_RX_NB_BYTES      0x13
#define REG_PKT_RSSI         0x1A
#define REG_MDM_CFG1         0x1D
#define REG_MDM_CFG2         0x1E
#define REG_MDM_CFG3         0x26
#define REG_SYNC_WORD        0x39
#define REG_VERSION          0x42

// IRQ & LoRa modes
#define IRQ_TX_DONE          0x08
#define IRQ_RX_DONE          0x40
#define IRQ_PAYLOAD_CRC_ERR  0x20
#define MODE_SLEEP    0x00
#define MODE_STDBY    0x01
#define MODE_TX       0x03
#define MODE_RX_SNGL  0x06
#define LORA_FLAG     0x80
#define ACK_MAGIC     0xAC

// ============================================================
//  BỘ NHỚ RTC
// ============================================================
#define RTC_MAGIC 0xDEADBEEF
typedef struct __attribute__((packed, aligned(4))) {
  uint32_t magic;
  uint16_t seq_num;
  uint8_t  fail_count;
  uint8_t  sf_level;
  int16_t  last_temp;
  uint16_t last_hum;
  uint16_t last_pres;
  uint16_t pad;
} RtcData_t;

RtcData_t rtcData;
bool      isFirstBoot = false;

// ============================================================
//  PAYLOAD (15 Bytes)
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

#define FLAG_FIRST_BOOT   (1 << 5)
#define FLAG_DATA_VALID   (1 << 4)
#define FLAG_BATT_LOW     (1 << 3)
#define FLAG_ALARM_TEMP   (1 << 1)
#define FLAG_ALARM_RAIN   (1 << 0)

// ============================================================
//  CẤU TRÚC SENSOR (Đã dời lên trên để fix lỗi Scope)
// ============================================================
struct SensorRaw {
  float temp;
  float humidity;
  float pressure;
  uint16_t rainAnalog;
  bool valid;
};

// ============================================================
//  GLOBAL OBJECTS
// ============================================================
Adafruit_BMP280 bmp;
DHT dht(DHT_PIN, DHT_TYPE);

// ============================================================
//  SPI THÔ (RAW SPI)
// ============================================================
inline void spiSelect()   { digitalWrite(LORA_NSS, LOW);  }
inline void spiDeselect() { digitalWrite(LORA_NSS, HIGH); }

uint8_t spiRead(uint8_t reg) {
  spiSelect();
  SPI.transfer(reg & 0x7F);
  uint8_t val = SPI.transfer(0);
  spiDeselect();
  return val;
}

void spiWrite(uint8_t reg, uint8_t val) {
  spiSelect();
  SPI.transfer(reg | 0x80);
  SPI.transfer(val);
  spiDeselect();
}

void spiBurstWrite(uint8_t reg, const uint8_t* buf, size_t len) {
  spiSelect();
  SPI.transfer(reg | 0x80);
  for (size_t i = 0; i < len; i++) SPI.transfer(buf[i]);
  spiDeselect();
}

uint16_t crc16_ccitt(const uint8_t* data, uint8_t len) {
  uint16_t crc = 0xFFFF;
  for (uint8_t i = 0; i < len; i++) {
    crc ^= (uint16_t)data[i] << 8;
    for (uint8_t b = 0; b < 8; b++)
      crc = (crc & 0x8000) ? (crc << 1) ^ 0x1021 : (crc << 1);
  }
  return crc;
}

// ============================================================
//  PHẦN LÕI HỆ THỐNG
// ============================================================
void checkBootReason() {
  rst_info* ri = ESP.getResetInfoPtr();
  if (ri->reason == REASON_DEEP_SLEEP_AWAKE) {
    ESP.rtcUserMemoryRead(0, (uint32_t*)&rtcData, sizeof(rtcData));
    if (rtcData.magic == RTC_MAGIC) {
      isFirstBoot = false;
      return;
    }
  }
  memset(&rtcData, 0, sizeof(rtcData));
  rtcData.magic = RTC_MAGIC;
  rtcData.sf_level = 7;
  isFirstBoot = true;
}

bool initHardware() {
  pinMode(LORA_NSS, OUTPUT); 
  digitalWrite(LORA_NSS, HIGH);
  pinMode(LORA_DIO0, INPUT); 

  // Ép Bus I2C theo chân D4, D3
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(400000);

  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV8);

  dht.begin();
  if (!bmp.begin(BMP280_ADDR)) return false;

  bmp.setSampling(Adafruit_BMP280::MODE_FORCED,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);
  delay(200);
  return true;
}

SensorRaw readAllSensors() {
  SensorRaw d = {};
  
  // Đọc BMP280 & DHT11
  for (int attempt = 0; attempt < 3; attempt++) {
    if (bmp.takeForcedMeasurement()) {
      d.temp     = bmp.readTemperature();
      d.pressure = bmp.readPressure() / 100.0f;
      d.humidity = dht.readHumidity(); 
      
      if(!isnan(d.humidity)) {
         d.valid = true;
         break;
      }
    }
    delay(250);
  }

  if (!d.valid) {
    d.temp = rtcData.last_temp / 100.0f;
    d.humidity = rtcData.last_hum / 100.0f;
    d.pressure = (rtcData.last_pres / 10.0f) + 900.0f;
  }

  // Đọc Mưa (Analog A0) - Trung bình 16 mẫu khử nhiễu
  uint32_t a0Sum = 0;
  for (int i = 0; i < 16; i++) {
    a0Sum += analogRead(RAIN_PIN);
    delay(2);
  }
  d.rainAnalog = a0Sum / 16; 

  return d;
}

WeatherPkt_t buildPacket(const SensorRaw& s) {
  WeatherPkt_t pkt = {};
  pkt.node_id = NODE_ID;
  pkt.seq_num = (uint8_t)(rtcData.seq_num & 0xFF);

  if (isFirstBoot)                         pkt.flags |= FLAG_FIRST_BOOT;
  if (s.valid)                             pkt.flags |= FLAG_DATA_VALID;
  if (s.temp > TEMP_ALARM_THRESHOLD)       pkt.flags |= FLAG_ALARM_TEMP;
  if (s.rainAnalog < RAIN_ALARM_THRESHOLD) pkt.flags |= FLAG_ALARM_RAIN; 

  pkt.temperature = (int16_t)(s.temp * 100.0f);
  pkt.humidity    = (uint16_t)constrain(s.humidity * 100.0f, 0.0f, 10000.0f);
  pkt.pressure = (uint16_t)(constrain(s.pressure - 800.0f, 0.0f, 300.0f) * 10.0f);
  pkt.rain_analog = s.rainAnalog;
  pkt.battery_pct = 100; 

  pkt.crc16       = crc16_ccitt((const uint8_t*)&pkt, sizeof(pkt) - 2);
  return pkt;
}

bool loraInit() {
  if (spiRead(REG_VERSION) != 0x12) return false; 

  spiWrite(REG_OP_MODE, MODE_SLEEP);
  delay(5);
  spiWrite(REG_OP_MODE, LORA_FLAG | MODE_SLEEP);
  delay(10);

  uint64_t frf = ((uint64_t)LORA_FREQ_HZ << 19) / 32000000UL;
  spiWrite(REG_FR_MSB, (uint8_t)(frf >> 16));
  spiWrite(REG_FR_MID, (uint8_t)(frf >> 8));
  spiWrite(REG_FR_LSB, (uint8_t)(frf));

  spiWrite(REG_MDM_CFG1, 0x72);
  spiWrite(REG_MDM_CFG2, (rtcData.sf_level << 4) | 0x04);
  spiWrite(REG_MDM_CFG3, (rtcData.sf_level >= 11) ? 0x0C : 0x04);

  spiWrite(0x20, 0x00);
  spiWrite(0x21, 0x08);

  spiWrite(REG_PA_CONFIG, 0x8F); 
  spiWrite(REG_FIFO_TX_BASE, 0x00);
  spiWrite(REG_FIFO_RX_BASE, 0x00);
  spiWrite(REG_SYNC_WORD, LORA_SYNC_WORD);
  spiWrite(REG_OP_MODE, LORA_FLAG | MODE_STDBY);
  delay(5);
  return true;
}

int loraTxWaitAck(const WeatherPkt_t& pkt) {
  for (int attempt = 0; attempt < MAX_TX_RETRY; attempt++) {
    spiWrite(REG_OP_MODE, LORA_FLAG | MODE_STDBY);
    spiWrite(REG_FIFO_ADDR, 0x00);
    spiWrite(0x22, sizeof(WeatherPkt_t));
    spiBurstWrite(REG_FIFO, (const uint8_t*)&pkt, sizeof(pkt));
    spiWrite(REG_IRQ_FLAGS, 0xFF);
    
    spiWrite(REG_OP_MODE, LORA_FLAG | MODE_TX);
    
    unsigned long txStart = millis();
    while ((spiRead(REG_IRQ_FLAGS) & IRQ_TX_DONE) == 0) {
      if (millis() - txStart > TX_TIMEOUT_MS) break;
      yield();
    }
    spiWrite(REG_IRQ_FLAGS, IRQ_TX_DONE);
    
    return 1; // Hoàn thành đẩy gói tin lên đường truyền
  }
  return -999;
}

void saveStateAndSleep(bool txOK, const WeatherPkt_t& pkt) {
  if (txOK) {
    rtcData.seq_num++;
    rtcData.fail_count = 0;
    if (rtcData.sf_level > 7) rtcData.sf_level--;
  } else {
    rtcData.fail_count++;
    if (rtcData.fail_count >= 3 && rtcData.sf_level < 12) {
      rtcData.sf_level++;
      rtcData.fail_count = 0;
    }
  }

  if (pkt.flags & FLAG_DATA_VALID) {
    rtcData.last_temp = pkt.temperature;
    rtcData.last_hum = pkt.humidity;
    rtcData.last_pres = pkt.pressure;
  }
  ESP.rtcUserMemoryWrite(0, (uint32_t*)&rtcData, sizeof(rtcData));

  spiWrite(REG_OP_MODE, LORA_FLAG | MODE_SLEEP);
  delay(5);
  SPI.end();
  
  digitalWrite(LORA_NSS, HIGH); // Đã xoá Wire.end(); ở dòng này
  
  pinMode(I2C_SDA, INPUT);
  pinMode(I2C_SCL, INPUT);
  pinMode(DHT_PIN, INPUT);
  delay(10);
  
  // Mạch chìm vào Deep Sleep (Yêu cầu phải cắm dây D0 vào RST khi test thực địa)
  ESP.deepSleep((uint64_t)SLEEP_SEC * 1000000ULL, WAKE_RF_DISABLED);
}

void setup() {
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(1);
  checkBootReason();
  
  if (!initHardware()) {
    ESP.deepSleep((uint64_t)SLEEP_SEC * 1000000ULL, WAKE_RF_DISABLED);
    return;
  }

  SensorRaw sensors = readAllSensors();
  WeatherPkt_t pkt = buildPacket(sensors);

  if (!loraInit()) {
    ESP.deepSleep((uint64_t)SLEEP_SEC * 1000000ULL, WAKE_RF_DISABLED);
    return;
  }
  saveStateAndSleep((loraTxWaitAck(pkt) != -999), pkt);
}

void loop() {
  // Trạm phát dùng Deep Sleep nên sẽ không chạy tới hàm loop()
}