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


# Configuration defaults
CHANNEL_ID = "3413241"
READ_API_KEY = "VISWQT7BABDVZOFS"
WRITE_API_KEY = "IWSWJL8BLKHHGSG2"

# Paths to models
MODEL_DIR = "models"
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
    """Fetches the last 150 feeds from ThingSpeak to compute lag values"""
    url = f"https://api.thingspeak.com/channels/{channel_id}/feeds.json?results=150"
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
            "rain_analog": int(f["field4"]) if f["field4"] is not None and f["field4"] != "" else None
        })
    
    df = pd.DataFrame(df_list)
    
    # Điền khuyết dữ liệu bằng forward fill / backward fill
    df = df.ffill().bfill()

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
    
    # Tính hiệu số thời gian tuyệt đối của mỗi hàng so với mốc 1 giờ trước và lấy hàng có khoảng cách nhỏ nhất
    idx_1h = (df["created_at_dt"] - target_time_1h).abs().idxmin()
    row_1h = df.loc[idx_1h]
    
    # Kiểm tra nếu khoảng cách thời gian vượt quá 20 phút thì dùng fallback (ví dụ index -60 hoặc dòng đầu tiên)
    time_diff_mins = abs((row_1h["created_at_dt"] - target_time_1h).total_seconds()) / 60.0
    if time_diff_mins > 20.0:
        if len(df) >= 60:
            row_1h = df.iloc[-60]
        else:
            row_1h = df.iloc[0]
        print(f"CẢNH BÁO: Không tìm thấy bản ghi lý tưởng từ 1 giờ trước. Sử dụng fallback từ timestamp {row_1h['created_at_dt']}")
    else:
        print(f"Đã tìm thấy bản ghi chuẩn xác từ 1 giờ trước: {row_1h['created_at_dt']} (lệch {time_diff_mins:.1f} phút)")
        
    temp_t = latest_row["temperature"]
    temp_t1 = row_1h["temperature"]
    
    hum_t = latest_row["humidity"]
    hum_t1 = row_1h["humidity"]
    
    pres_t = latest_row["pressure"]
    pres_t1 = row_1h["pressure"]
    
    rain_t = latest_row["rain_analog"]
    rain_t1 = row_1h["rain_analog"]
    
    # Tạo vector đặc trưng đầu vào
    input_data = {
        "hour": (current_hour + 1) % 24,
        "temp_lag1": temp_t,
        "temp_lag2": temp_t1,
        "temp_diff": temp_t - temp_t1,
        
        "hum_lag1": hum_t,
        "hum_lag2": hum_t1,
        "hum_diff": hum_t - hum_t1,
        
        "pres_lag1": pres_t,
        "pres_lag2": pres_t1,
        "pres_diff": pres_t - pres_t1,
        
        "rain_lag1": rain_t,
        "rain_lag2": rain_t1
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
    # Nếu mô hình phân loại chưa train đủ các lớp, cần kiểm tra kích thước xác suất
    classes = list(model_status.classes_)
    rain_prob = 0.0
    if 2 in classes:
        rain_class_idx = classes.index(2)
        rain_prob = status_probs[rain_class_idx] * 100.0
    else:
        # Nếu chưa có lớp mưa, sử dụng cảm biến mưa trực tiếp để suy luận xác suất
        rain_prob = 90.0 if rain_t < 500 else 10.0
        
    print(f"--- KẾT QUẢ DỰ BÁO AI (+1h) ---")
    print(f"Thời gian hiện tại: {local_time.strftime('%Y-%m-%d %H:%M:%S')} (Giờ: {current_hour})")
    print(f"Nhiệt độ hiện tại: {temp_t:.1f} °C -> Dự báo (+1h): {pred_temp:.2f} °C")
    print(f"Độ ẩm hiện tại: {hum_t:.1f} % -> Dự báo (+1h): {pred_hum:.2f} %")
    print(f"Áp suất hiện tại: {pres_t:.1f} hPa | Xu hướng: {input_data['pres_diff']:.3f} hPa/chu kỳ")
    print(f"Xác suất mưa dự đoán: {rain_prob:.1f} %")
    
    status_map = {0: "Nắng ráo ☀️", 1: "Nhiều mây ☁️", 2: "Mưa dông 🌧️"}
    print(f"Trạng thái thời tiết dự đoán: {status_map.get(pred_status, 'Chưa rõ')}")
    
    return pred_temp, rain_prob, pred_status

def upload_predictions(write_key, pred_temp, rain_prob, pred_status):
    """Uploads the predicted values back to ThingSpeak (field6, field7, field8)"""
    url = f"https://api.thingspeak.com/update?api_key={write_key}"
    url += f"&field6={pred_temp:.2f}"
    url += f"&field7={rain_prob:.1f}"
    url += f"&field8={int(pred_status)}"
    
    print(f"Đang tải kết quả dự báo lên ThingSpeak...")
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
        pred_temp, rain_prob, pred_status = preprocess_and_predict(
            feeds, feature_cols, model_temp, model_hum, model_status
        )
        upload_predictions(write_key, pred_temp, rain_prob, pred_status)
    except Exception as e:
        print(f"Lỗi xảy ra trong quá trình thực thi: {e}")

if __name__ == "__main__":
    main()
