import os
import sys
import time
import math
import random
import requests
import datetime
import io

# Cấu hình UTF-8 cho stdout/stderr để tránh lỗi UnicodeEncodeError trên terminal Windows
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')
sys.stderr = io.TextIOWrapper(sys.stderr.buffer, encoding='utf-8')

# ThingSpeak Write Config
WRITE_API_KEY = "IWSWJL8BLKHHGSG2"

def generate_weather_metrics():
    """Tự động sinh các thông số thời tiết giả lập thực tế"""
    now = datetime.datetime.now()
    hour = now.hour + now.minute / 60.0
    
    # 1. Nhiệt độ: Đạt đỉnh vào lúc 14h chiều, thấp nhất lúc 4h sáng (biên độ dao động 25°C - 33°C)
    temp_base = 29.0
    temp_amplitude = 4.0
    # Hàm cosine đạt cực đại ở t=0, cực tiểu ở t=pi. Ta dịch pha sao cho đạt đỉnh lúc 14h.
    temp = temp_base + temp_amplitude * math.cos((hour - 14.0) * math.pi / 12.0)
    temp += random.uniform(-0.5, 0.5) # Thêm nhiễu nhẹ
    
    # 2. Độ ẩm: Tỷ lệ nghịch với nhiệt độ (biên độ dao động 60% - 85%)
    hum_base = 72.5
    hum_amplitude = 12.5
    hum = hum_base - hum_amplitude * math.cos((hour - 14.0) * math.pi / 12.0)
    hum += random.uniform(-1.0, 1.0) # Thêm nhiễu nhẹ
    hum = min(max(hum, 0.0), 100.0)
    
    # 3. Khí áp: Dao động hình sin chậm quanh mức trung bình 1010 hPa
    pres = 1010.0 + 2.0 * math.sin(hour * math.pi / 6.0) + random.uniform(-0.2, 0.2)
    
    # 4. Cảm biến Mưa: 1023 là khô ráo, giá trị càng thấp là mưa càng to
    # Giả lập xác suất có mưa: thỉnh thoảng có đợt mưa kéo dài vài phút
    # Dùng sóng sin tần số cực thấp để tạo chu kỳ khô/mưa dài
    rain_wave = math.sin(time.time() / 600.0) 
    if rain_wave > 0.7: # Chu kỳ có mưa
        rain = random.randint(200, 600) # Phát hiện mưa
    else:
        rain = random.randint(1010, 1023) # Khô ráo
        
    # 5. Dung lượng pin
    bat = random.randint(97, 100)
    
    return temp, hum, pres, rain, bat

def upload_to_thingspeak(temp, hum, pres, rain, bat):
    """Gửi các thông số giả lập lên ThingSpeak (Field 1 - 5)"""
    url = f"https://api.thingspeak.com/update?api_key={WRITE_API_KEY}"
    url += f"&field1={temp:.2f}"
    url += f"&field2={hum:.2f}"
    url += f"&field3={pres:.2f}"
    url += f"&field4={rain}"
    url += f"&field5={bat}"
    
    print(f"\n[{datetime.datetime.now().strftime('%H:%M:%S')}] Đang gửi dữ liệu đo giả lập lên ThingSpeak Cloud:")
    print(f"  - Nhiệt độ: {temp:.2f} °C")
    print(f"  - Độ ẩm: {hum:.2f} %")
    print(f"  - Khí áp: {pres:.2f} hPa")
    print(f"  - Cảm biến mưa (analog): {rain} ({'Có mưa' if rain < 700 else 'Khô ráo'})")
    print(f"  - Dung lượng pin: {bat} %")
    
    try:
        response = requests.get(url, timeout=10)
        result = response.text
        if result == "0":
            print("❌ Thất bại: ThingSpeak từ chối cập nhật (chờ 15s giữa mỗi lần gửi).")
        else:
            print(f"✅ Thành công! Entry ID trên ThingSpeak: {result}")
    except Exception as e:
        print(f"❌ Lỗi kết nối khi gửi dữ liệu lên ThingSpeak: {e}")

def main():
    print("====================================================")
    print("  TRẠM THỜI TIẾT IOT - BỘ GIẢ LẬP PHẦN CỨNG (SENDER) ")
    print("====================================================")
    print(f"Thiết lập gửi dữ liệu lên ThingSpeak Write API Key: {WRITE_API_KEY}")
    print("Chương trình sẽ tự động gửi gói tin mô phỏng sau mỗi 60 giây.")
    print("Nhấn Ctrl + C để dừng.")
    print("----------------------------------------------------")
    
    try:
        while True:
            temp, hum, pres, rain, bat = generate_weather_metrics()
            upload_to_thingspeak(temp, hum, pres, rain, bat)
            time.sleep(60)
    except KeyboardInterrupt:
        print("\nĐã dừng bộ giả lập phần cứng.")

if __name__ == "__main__":
    main()
