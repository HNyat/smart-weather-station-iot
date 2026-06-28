import os
import sys
import io
# Cấu hình UTF-8 cho stdout/stderr để tránh lỗi UnicodeEncodeError trên terminal Windows
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')
sys.stderr = io.TextIOWrapper(sys.stderr.buffer, encoding='utf-8')

import requests
import datetime
import pandas as pd
import numpy as np
import joblib
import sqlite3


# Configuration defaults
CHANNEL_ID = "3413241"
READ_API_KEY = "VISWQT7BABDVZOFS"
WRITE_API_KEY = "IWSWJL8BLKHHGSG2"

# Paths to models relative to project root
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.dirname(SCRIPT_DIR)

MODEL_DIR = os.path.join(PROJECT_ROOT, "models")
DATA_DIR = os.path.join(PROJECT_ROOT, "data")

MODEL_TEMP_PATH = os.path.join(MODEL_DIR, "temp_model.joblib")
MODEL_HUM_PATH = os.path.join(MODEL_DIR, "hum_model.joblib")
MODEL_STATUS_PATH = os.path.join(MODEL_DIR, "status_model.joblib")
FEATURE_COLS_PATH = os.path.join(MODEL_DIR, "feature_cols.joblib")

def check_models_exist():
    """Checks if all required model files are trained and saved"""
    required = [MODEL_TEMP_PATH, MODEL_HUM_PATH, MODEL_STATUS_PATH, FEATURE_COLS_PATH]
    missing = [path for path in required if not os.path.exists(path)]
    if missing:
        print("LỖI: Thiếu các file mô hình sau:")
        for m in missing:
            print(f"  - {m}")
        print("Vui lòng chạy kịch bản huấn luyện trước: python train.py")
        sys.exit(1)

def fetch_latest_feeds(channel_id, read_key):
    """Fetches the last 300 feeds from ThingSpeak to compute lag values"""
    url = f"https://api.thingspeak.com/channels/{channel_id}/feeds.json?results=300"
    if read_key:
        url += f"&api_key={read_key}"
        
    print(f"Đang đọc dữ liệu mới nhất từ ThingSpeak (Channel ID: {channel_id})...")
    response = requests.get(url, timeout=10)
    response.raise_for_status()
    data = response.json()
    
    feeds = data["feeds"]
    if len(feeds) < 3:
        print("LỖI: Kênh ThingSpeak chưa đủ dữ liệu (cần tối thiểu 3 bản ghi cảm biến).")
        sys.exit(1)
        
    return feeds

def detect_and_sanitize_anomalies(df):
    """
    Phát hiện và xử lý các giá trị bất thường của cảm biến (nhiễu vật lý hoặc sốc đột ngột)
    bằng cách chuyển chúng thành NaN và thực hiện điền khuyết (Forward Fill).
    """
    df_clean = df.copy()
    
    # Đảm bảo các cột cảm biến có kiểu số thực để tránh lỗi so sánh hoặc kiểu Object
    for col in ["temperature", "humidity", "pressure", "rain_analog", "battery"]:
        df_clean[col] = pd.to_numeric(df_clean[col], errors='coerce')
    
    # 1. Kiểm tra ngưỡng vật lý
    temp_out_of_range = (df_clean["temperature"] < 0) | (df_clean["temperature"] > 55)
    hum_out_of_range = (df_clean["humidity"] < 0) | (df_clean["humidity"] > 100)
    pres_out_of_range = (df_clean["pressure"] < 950) | (df_clean["pressure"] > 1050)
    bat_out_of_range = (df_clean["battery"] < 0) | (df_clean["battery"] > 100)
    
    if temp_out_of_range.any():
        print(f"CẢNH BÁO AI: Phát hiện {temp_out_of_range.sum()} giá trị nhiệt độ ngoài ngưỡng vật lý [0-55°C]. Sẽ tự động làm sạch.")
        df_clean.loc[temp_out_of_range, "temperature"] = np.nan
        
    if hum_out_of_range.any():
        print(f"CẢNH BÁO AI: Phát hiện {hum_out_of_range.sum()} giá trị độ ẩm ngoài ngưỡng vật lý [0-100%]. Sẽ tự động làm sạch.")
        df_clean.loc[hum_out_of_range, "humidity"] = np.nan
        
    if pres_out_of_range.any():
        print(f"CẢNH BÁO AI: Phát hiện {pres_out_of_range.sum()} giá trị khí áp ngoài ngưỡng vật lý [950-1050 hPa]. Sẽ tự động làm sạch.")
        df_clean.loc[pres_out_of_range, "pressure"] = np.nan

    if bat_out_of_range.any():
        print(f"CẢNH BÁO AI: Phát hiện {bat_out_of_range.sum()} giá trị pin ngoài ngưỡng vật lý [0-100%]. Sẽ tự động làm sạch.")
        df_clean.loc[bat_out_of_range, "battery"] = np.nan
        
    # 2. Kiểm tra biến động đột biến (Spike Detection) so với dòng trước đó
    if len(df_clean) > 1:
        temp_filled = df_clean["temperature"].ffill()
        temp_diff = temp_filled.diff().abs()
        temp_spikes = temp_diff > 12.0
        if temp_spikes.any():
            print(f"CẢNH BÁO AI: Phát hiện {temp_spikes.sum()} điểm biến động nhiệt độ đột biến (>12°C). Sẽ tự động làm sạch.")
            df_clean.loc[temp_spikes, "temperature"] = np.nan
            
        hum_filled = df_clean["humidity"].ffill()
        hum_diff = hum_filled.diff().abs()
        hum_spikes = hum_diff > 35.0
        if hum_spikes.any():
            print(f"CẢNH BÁO AI: Phát hiện {hum_spikes.sum()} điểm biến động độ ẩm đột biến (>35%). Sẽ tự động làm sạch.")
            df_clean.loc[hum_spikes, "humidity"] = np.nan
            
    # 3. Điền khuyết lại các giá trị bị gán NaN bằng forward fill/backward fill
    df_clean = df_clean.ffill().bfill()
    return df_clean

def calculate_heat_index(temp, hum):
    """
    Tính toán chỉ số nhiệt oi bức (Heat Index) dựa trên nhiệt độ và độ ẩm thực tế.
    Với các giá trị nhiệt độ thấp hơn 27°C, chỉ số nhiệt tương đương với nhiệt độ thực tế.
    """
    hi = 0.5 * (temp + 61.0 + ((temp - 68.0) * 1.2) + (hum * 0.094))
    return temp if temp < 27.0 else hi

def calculate_dew_point(temp, hum):
    """
    Tính toán điểm sương (Dew Point) theo công thức Magnus-Tetens.
    """
    a = 17.27
    b = 237.7
    hum_clipped = np.clip(hum, 0.01, 100.0)
    alpha = ((a * temp) / (b + temp)) + np.log(hum_clipped / 100.0)
    return (b * alpha) / (a - alpha)


def preprocess_and_predict(feeds, feature_cols, model_temp, model_hum, model_status):
    """
    Parses current feeds, builds feature vectors using lag values,
    and runs predictions.
    """
    # Trích xuất dữ liệu cảm biến gần nhất
    df_list = []
    for f in feeds:
        df_list.append({
            "created_at": f["created_at"],
            "temperature": float(f["field1"]) if f["field1"] is not None and f["field1"] != "" else None,
            "humidity": float(f["field2"]) if f["field2"] is not None and f["field2"] != "" else None,
            "pressure": float(f["field3"]) if f["field3"] is not None and f["field3"] != "" else None,
            "rain_analog": int(f["field4"]) if f["field4"] is not None and f["field4"] != "" else None,
            "battery": int(f["field5"]) if f["field5"] is not None and f["field5"] != "" else None
        })
    
    df = pd.DataFrame(df_list)
    
    # Phát hiện và làm sạch các giá trị bất thường
    df = detect_and_sanitize_anomalies(df)

    # Xử lý dự phòng nếu toàn bộ cột trống (do cảm biến offline lâu ngày)
    if df["temperature"].isna().all():
        print("CẢNH BÁO: Cảm biến Nhiệt độ offline. Dùng giá trị mặc định 28.0 °C")
        df["temperature"] = df["temperature"].fillna(28.0)
    if df["humidity"].isna().all():
        print("CẢNH BÁO: Cảm biến Độ ẩm offline. Dùng giá trị mặc định 75.0 %")
        df["humidity"] = df["humidity"].fillna(75.0)
    if df["pressure"].isna().all():
        print("CẢNH BÁO: Cảm biến Áp suất offline. Dùng giá trị mặc định 1010.0 hPa")
        df["pressure"] = df["pressure"].fillna(1010.0)
    if df["rain_analog"].isna().all():
        print("CẢNH BÁO: Cảm biến Mưa offline. Dùng giá trị mặc định 1023 (Không mưa)")
        df["rain_analog"] = df["rain_analog"].fillna(1023)
    if df["battery"].isna().all():
        print("CẢNH BÁO: Cảm biến Pin offline. Dùng giá trị mặc định 100 %")
        df["battery"] = df["battery"].fillna(100)
        
    df = df.ffill().bfill()
    
    # Chuyển cột created_at sang datetime để tính toán chênh lệch thời gian chuẩn xác
    df["created_at_dt"] = pd.to_datetime(df["created_at"])
    
    # Lấy bản ghi mới nhất (t)
    latest_row = df.iloc[-1]
    latest_time = latest_row["created_at_dt"]
    
    # Chuyển đổi sang múi giờ Việt Nam (UTC+7)
    local_time = latest_time + datetime.timedelta(hours=7)
    current_hour = local_time.hour
    
    # Tìm bản ghi gần nhất với 1 giờ trước (t-1h)
    target_time_1h = latest_time - pd.Timedelta(hours=1)
    idx_1h = (df["created_at_dt"] - target_time_1h).abs().idxmin()
    row_1h = df.loc[idx_1h]
    
    # Kiểm tra nếu khoảng cách thời gian vượt quá 20 phút thì dùng fallback (ví dụ index -60 hoặc dòng đầu tiên)
    # Với chu kỳ gửi 60 giây/lần (1 phút), 1 giờ trước tương đương bản ghi thứ 60 từ dưới lên (iloc[-60])
    time_diff_mins = abs((row_1h["created_at_dt"] - target_time_1h).total_seconds()) / 60.0
    if time_diff_mins > 20.0:
        if len(df) >= 60:
            row_1h = df.iloc[-60]
        else:
            row_1h = df.iloc[0]
        print(f"CẢNH BÁO: Không tìm thấy bản ghi lý tưởng từ 1 giờ trước. Sử dụng fallback từ timestamp {row_1h['created_at_dt']}")
    else:
        print(f"Đã tìm thấy bản ghi chuẩn xác từ 1 giờ trước: {row_1h['created_at_dt']} (lệch {time_diff_mins:.1f} phút)")
        
    # Tìm bản ghi gần nhất với 2 giờ trước (t-2h) để tính toán trung bình trượt
    target_time_2h = latest_time - pd.Timedelta(hours=2)
    idx_2h = (df["created_at_dt"] - target_time_2h).abs().idxmin()
    row_2h = df.loc[idx_2h]
    
    # Kiểm tra nếu khoảng cách thời gian vượt quá 30 phút thì dùng fallback (ví dụ index -120 hoặc dòng đầu tiên)
    # Với chu kỳ gửi 60 giây/lần (1 phút), 2 giờ trước tương đương bản ghi thứ 120 từ dưới lên (iloc[-120])
    time_diff_mins_2h = abs((row_2h["created_at_dt"] - target_time_2h).total_seconds()) / 60.0
    if time_diff_mins_2h > 30.0:
        if len(df) >= 120:
            row_2h = df.iloc[-120]
        else:
            row_2h = df.iloc[0]
        print(f"CẢNH BÁO: Không tìm thấy bản ghi lý tưởng từ 2 giờ trước. Sử dụng fallback từ timestamp {row_2h['created_at_dt']}")
    else:
        print(f"Đã tìm thấy bản ghi chuẩn xác từ 2 giờ trước: {row_2h['created_at_dt']} (lệch {time_diff_mins_2h:.1f} phút)")
 
    # Tìm bản ghi gần nhất với 4 giờ trước (t-4h) để tính toán xu hướng áp suất trung hạn
    target_time_4h = latest_time - pd.Timedelta(hours=4)
    idx_4h = (df["created_at_dt"] - target_time_4h).abs().idxmin()
    row_4h = df.loc[idx_4h]
    
    # Kiểm tra nếu khoảng cách thời gian vượt quá 40 phút thì dùng fallback
    # Với chu kỳ gửi 60 giây/lần (1 phút), 4 giờ trước tương đương bản ghi thứ 240 từ dưới lên (iloc[-240])
    time_diff_mins_4h = abs((row_4h["created_at_dt"] - target_time_4h).total_seconds()) / 60.0
    if time_diff_mins_4h > 40.0:
        if len(df) >= 240:
            row_4h = df.iloc[-240]
        else:
            row_4h = df.iloc[0]
        print(f"CẢNH BÁO: Không tìm thấy bản ghi lý tưởng từ 4 giờ trước. Sử dụng fallback từ timestamp {row_4h['created_at_dt']}")
    else:
        print(f"Đã tìm thấy bản ghi chuẩn xác từ 4 giờ trước: {row_4h['created_at_dt']} (lệch {time_diff_mins_4h:.1f} phút)")

    temp_t = latest_row["temperature"]
    temp_t1 = row_1h["temperature"]
    temp_t2 = row_2h["temperature"]
    
    hum_t = latest_row["humidity"]
    hum_t1 = row_1h["humidity"]
    hum_t2 = row_2h["humidity"]
    
    pres_t = latest_row["pressure"]
    pres_t1 = row_1h["pressure"]
    pres_t4 = row_4h["pressure"]
    
    rain_t = latest_row["rain_analog"]
    rain_t1 = row_1h["rain_analog"]
    
    bat_t = latest_row["battery"]
    
    # Tính toán các đặc trưng xu hướng, chỉ số oi bức và lượng giác hóa thời gian
    target_hour = (current_hour + 1) % 24
    hour_sin = np.sin(2 * np.pi * target_hour / 24)
    hour_cos = np.cos(2 * np.pi * target_hour / 24)
    
    current_month = local_time.month
    month_sin = np.sin(2 * np.pi * current_month / 12)
    month_cos = np.cos(2 * np.pi * current_month / 12)
    
    temp_roll_mean_3h = (temp_t + temp_t1 + temp_t2) / 3.0
    hum_roll_mean_3h = (hum_t + hum_t1 + hum_t2) / 3.0
    
    pres_trend_3h = pres_t1 - pres_t4
    heat_index_lag1 = calculate_heat_index(temp_t, hum_t)
    dew_point_lag1 = calculate_dew_point(temp_t, hum_t)
    
    # Tạo vector đặc trưng đầu vào khớp 100% với train.py
    input_data = {
        "hour_sin": hour_sin,
        "hour_cos": hour_cos,
        "month_sin": month_sin,
        "month_cos": month_cos,
        "temp_lag1": temp_t,
        "temp_lag2": temp_t1,
        "temp_diff": temp_t - temp_t1,
        "temp_roll_mean_3h": temp_roll_mean_3h,
        
        "hum_lag1": hum_t,
        "hum_lag2": hum_t1,
        "hum_diff": hum_t - hum_t1,
        "hum_roll_mean_3h": hum_roll_mean_3h,
        
        "pres_lag1": pres_t,
        "pres_lag2": pres_t1,
        "pres_diff": pres_t - pres_t1,
        "pres_trend_3h": pres_trend_3h,
        
        "rain_lag1": rain_t,
        "rain_lag2": rain_t1,
        "heat_index_lag1": heat_index_lag1,
        "dew_point_lag1": dew_point_lag1
    }
    
    # Chuyển đổi thành DataFrame và khớp thứ tự cột
    X_new = pd.DataFrame([input_data])[feature_cols]
    
    # Chạy suy luận
    pred_temp = model_temp.predict(X_new)[0]
    pred_hum = model_hum.predict(X_new)[0]
    
    # Dự đoán trạng thái thời tiết & Xác suất mưa
    pred_status = model_status.predict(X_new)[0]
    status_probs = model_status.predict_proba(X_new)[0]
    
    # Xác suất mưa được tính bằng xác suất của lớp Mưa (chỉ số 2 trong classes_ [0, 1, 2])
    classes = list(model_status.classes_)
    rain_prob = 0.0
    if 2 in classes:
        rain_class_idx = classes.index(2)
        rain_prob = status_probs[rain_class_idx] * 100.0
    else:
        # Nếu chưa có lớp mưa, sử dụng cảm biến mưa trực tiếp để suy luận xác suất
        rain_prob = 90.0 if rain_t < 700 else 10.0
        
    # Hiệu chỉnh vật lý thời gian thực: Nếu trạm đang phát hiện mưa, xác suất mưa tiếp diễn phải >= 60%
    if rain_t < 700:
        rain_prob = max(rain_prob, 60.0 + (700 - rain_t) * 0.05)
        
    print(f"--- KẾT QUẢ DỰ BÁO AI (+1h) ---")
    print(f"Thời gian hiện tại: {local_time.strftime('%Y-%m-%d %H:%M:%S')} (Giờ: {current_hour})")
    print(f"Nhiệt độ hiện tại: {temp_t:.1f} °C -> Dự báo (+1h): {pred_temp:.2f} °C")
    print(f"Độ ẩm hiện tại: {hum_t:.1f} % -> Dự báo (+1h): {pred_hum:.2f} %")
    print(f"Áp suất hiện tại: {pres_t:.1f} hPa | Xu hướng: {input_data['pres_diff']:.3f} hPa/chu kỳ")
    print(f"Xác suất mưa dự đoán: {rain_prob:.1f} %")
    
    status_map = {0: "Nắng ráo ☀️", 1: "Nhiều mây ☁️", 2: "Mưa dông 🌧️"}
    print(f"Trạng thái thời tiết dự đoán: {status_map.get(pred_status, 'Chưa rõ')}")
    
    return pred_temp, pred_hum, rain_prob, pred_status, input_data["pres_diff"], temp_t, hum_t, pres_t, rain_t, bat_t, latest_row["created_at"]

def upload_predictions(write_key, pred_temp, pred_hum, rain_prob, temp_t, hum_t, pres_t, rain_t, bat_t):
    """Uploads predictions to ThingSpeak (fields 6-8) using partial update."""
    url = f"https://api.thingspeak.com/update?api_key={write_key}"
    url += f"&field6={pred_temp:.2f}"
    url += f"&field7={rain_prob:.1f}"
    url += f"&field8={pred_hum:.2f}" # Field 8 đổi thành dự báo độ ẩm
    
    print(f"Đang tải kết quả dự báo lên ThingSpeak (Partial Update)...")
    try:
        response = requests.get(url, timeout=10)
        response.raise_for_status()
        result_id = response.text
        if result_id == "0":
            print("CẢNH BÁO: ThingSpeak trả về 0. Bạn cần đợi tối thiểu 15s giữa các lần gửi.")
        else:
            print(f"Cập nhật thành công! Entry ID trên ThingSpeak: {result_id}")
    except Exception as e:
        print(f"Lỗi khi gửi kết quả lên ThingSpeak: {e}")

def save_to_local_db(timestamp, temp, hum, pres, rain, bat, pred_temp, pred_hum, rain_prob, pred_status, pres_diff):
    """Lưu trữ dữ liệu cảm biến thực tế và dự báo AI vào SQLite cục bộ"""
    os.makedirs(DATA_DIR, exist_ok=True)
    db_path = os.path.join(DATA_DIR, "sensor_warehouse.db")
    
    print(f"Đang kết nối cơ sở dữ liệu SQLite cục bộ tại {db_path}...")
    try:
        conn = sqlite3.connect(db_path)
        cursor = conn.cursor()
        
        # Khởi tạo bảng nếu chưa tồn tại
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS weather_history (
                timestamp TEXT PRIMARY KEY,
                temperature REAL,
                humidity REAL,
                pressure REAL,
                rain_analog INTEGER,
                battery_pct INTEGER,
                predicted_temp REAL,
                predicted_humidity REAL,
                rain_probability REAL,
                predicted_status INTEGER,
                pressure_diff REAL
            )
        """)
        
        # Tự động nâng cấp cột predicted_humidity nếu bảng cũ chưa có
        cursor.execute("PRAGMA table_info(weather_history)")
        columns = [info[1] for info in cursor.fetchall()]
        if "predicted_humidity" not in columns:
            try:
                cursor.execute("ALTER TABLE weather_history ADD COLUMN predicted_humidity REAL")
                conn.commit()
                print("Đã tự động nâng cấp cấu trúc bảng SQLite: thêm cột 'predicted_humidity'.")
            except Exception as alter_err:
                print(f"Cảnh báo khi thêm cột 'predicted_humidity': {alter_err}")
        
        # Chèn hoặc ghi đè dữ liệu
        cursor.execute("""
            INSERT OR REPLACE INTO weather_history (
                timestamp, temperature, humidity, pressure, rain_analog, battery_pct,
                predicted_temp, predicted_humidity, rain_probability, predicted_status, pressure_diff
            ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        """, (str(timestamp), temp, hum, pres, rain, bat, pred_temp, pred_hum, rain_prob, int(pred_status), pres_diff))
        
        conn.commit()
        conn.close()
        print(f"Đã lưu bản ghi thời gian {timestamp} thành công vào SQLite (sensor_warehouse.db).")
    except Exception as e:
        print(f"Lỗi khi lưu dữ liệu vào SQLite cục bộ: {e}")

def main():
    check_models_exist()
    
    # Cho phép ghi đè thông tin bằng Environment Variables (tiện lợi cho GitHub Actions)
    channel_id = os.environ.get("THINGSPEAK_CHANNEL_ID", CHANNEL_ID)
    read_key = os.environ.get("THINGSPEAK_READ_KEY", READ_API_KEY)
    write_key = os.environ.get("THINGSPEAK_WRITE_KEY", WRITE_API_KEY)
    
    # Tải các mô hình
    print("Đang tải các mô hình ML...")
    model_temp = joblib.load(MODEL_TEMP_PATH)
    model_hum = joblib.load(MODEL_HUM_PATH)
    model_status = joblib.load(MODEL_STATUS_PATH)
    feature_cols = joblib.load(FEATURE_COLS_PATH)
    
    try:
        feeds = fetch_latest_feeds(channel_id, read_key)
        pred_temp, pred_hum, rain_prob, pred_status, pres_diff, temp_t, hum_t, pres_t, rain_t, bat_t, timestamp = preprocess_and_predict(
            feeds, feature_cols, model_temp, model_hum, model_status
        )
        upload_predictions(write_key, pred_temp, pred_hum, rain_prob, temp_t, hum_t, pres_t, rain_t, bat_t)
        save_to_local_db(timestamp, temp_t, hum_t, pres_t, rain_t, bat_t, pred_temp, pred_hum, rain_prob, pred_status, pres_diff)
    except Exception as e:
        print(f"Lỗi xảy ra trong quá trình thực thi: {e}")

if __name__ == "__main__":
    main()
