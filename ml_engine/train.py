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
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestRegressor, RandomForestClassifier
from sklearn.metrics import mean_squared_error, mean_absolute_error, accuracy_score, classification_report
import joblib


# Ensure output directories exist
os.makedirs("models", exist_ok=True)
os.makedirs("data", exist_ok=True)

# 1. Configuration
# Default coordinates: Hanoi, Vietnam (Latitude: 21.0285, Longitude: 105.8542)
LATITUDE = 21.0285
LONGITUDE = 105.8542
DATASET_PATH = "data/historical_weather.csv"

def fetch_historical_data():
    """
    Fetches historical weather data from Open-Meteo Archive API.
    Retrieves data for the last 60 days to train the ML models.
    """
    print("=== 1. Tải dữ liệu thời tiết lịch sử từ Open-Meteo API ===")
    
    end_date = datetime.date.today() - datetime.timedelta(days=2)
    start_date = end_date - datetime.timedelta(days=365 * 3) # 3 năm dữ liệu lịch sử để học tốt các mô hình mùa
    
    start_str = start_date.strftime("%Y-%m-%d")
    end_str = end_date.strftime("%Y-%m-%d")
    
    url = (
        f"https://archive-api.open-meteo.com/v1/archive?"
        f"latitude={LATITUDE}&longitude={LONGITUDE}&"
        f"start_date={start_str}&end_date={end_str}&"
        f"hourly=temperature_2m,relative_humidity_2m,surface_pressure,rain&"
        f"timezone=Asia%2FBangkok"
    )
    
    print(f"URL API: {url}")
    try:
        response = requests.get(url, timeout=15)
        response.raise_for_status()
        data = response.json()
        
        hourly = data["hourly"]
        df = pd.DataFrame({
            "timestamp": pd.to_datetime(hourly["time"]),
            "temperature": hourly["temperature_2m"],
            "humidity": hourly["relative_humidity_2m"],
            "pressure": hourly["surface_pressure"],
            "rain_mm": hourly["rain"]
        })
        
        # Tiền xử lý dữ liệu bị thiếu
        df = df.interpolate(method="linear").bfill().ffill()
        
        # Chuyển đổi lượng mưa vật lý (mm) sang giá trị giả lập của cảm biến mưa Analog (0 - 1023)
        # Nguyên lý cảm biến: Càng mưa to giá trị analog càng thấp (0 là ngập/ướt sũng, 1023 là khô ráo)
        analog_rain = []
        np.random.seed(42) # Cho kết quả ổn định
        for r in df["rain_mm"]:
            if r > 2.0: # Mưa to
                val = np.random.randint(100, 300)
            elif r > 0.1: # Mưa nhỏ/phun
                val = np.random.randint(350, 650)
            else: # Không mưa
                val = np.random.randint(950, 1023)
            analog_rain.append(val)
            
        df["rain_analog"] = analog_rain
        df["battery_pct"] = np.random.randint(90, 100, size=len(df))
        
        df.to_csv(DATASET_PATH, index=False)
        print(f"Đã tải thành công và lưu {len(df)} dòng dữ liệu vào {DATASET_PATH}\n")
        return df
        
    except Exception as e:
        print(f"Lỗi khi tải dữ liệu từ API: {e}")
        if os.path.exists(DATASET_PATH):
            print("Sử dụng tệp dữ liệu đã lưu trước đó làm dự phòng...")
            return pd.read_csv(DATASET_PATH)
        else:
            print("Không có dữ liệu dự phòng. Tạo dữ liệu giả lập cho huấn luyện...")
            return generate_mock_historical_data()

def generate_mock_historical_data():
    """Generates mock historical data if API call fails and no backup file exists"""
    print("Tạo dữ liệu thời tiết mô phỏng...")
    dates = pd.date_range(end=datetime.datetime.now(), periods=1000, freq='H')
    np.random.seed(42)
    
    # Giả lập thời tiết nhiệt đới
    temp = 28 + 5 * np.sin(np.pi * dates.hour / 12) + np.random.normal(0, 1, len(dates))
    humidity = 80 - 15 * np.sin(np.pi * dates.hour / 12) + np.random.normal(0, 2, len(dates))
    humidity = np.clip(humidity, 30, 100)
    pressure = 1008 + 3 * np.cos(np.pi * dates.hour / 12) + np.random.normal(0, 0.5, len(dates))
    
    # Giả lập mưa
    rain_analog = []
    for h in humidity:
        if h > 85 and np.random.rand() > 0.4:
            rain_analog.append(np.random.randint(150, 450)) # Có mưa
        else:
            rain_analog.append(np.random.randint(980, 1023)) # Khô
            
    df = pd.DataFrame({
        "timestamp": dates,
        "temperature": temp,
        "humidity": humidity,
        "pressure": pressure,
        "rain_analog": rain_analog,
        "battery_pct": np.random.randint(92, 100, size=len(dates))
    })
    df.to_csv(DATASET_PATH, index=False)
    return df

def calculate_heat_index(temp, hum):
    """
    Tính toán chỉ số nhiệt oi bức (Heat Index) dựa trên nhiệt độ và độ ẩm thực tế.
    Với các giá trị nhiệt độ thấp hơn 27°C, chỉ số nhiệt tương đương với nhiệt độ thực tế.
    """
    hi = 0.5 * (temp + 61.0 + ((temp - 68.0) * 1.2) + (hum * 0.094))
    return np.where(temp < 27.0, temp, hi)

def feature_engineering(df):
    """
    Creates lag features and diff features for time series forecasting.
    Predicts the next hour's state based on current and past hours.
    """
    print("=== 2. Thiết kế Đặc trưng (Feature Engineering) ===")
    
    # 1. Trích xuất đặc trưng tuần hoàn thời gian (Cyclic hour)
    hour = pd.to_datetime(df["timestamp"]).dt.hour
    df["hour_sin"] = np.sin(2 * np.pi * hour / 24)
    df["hour_cos"] = np.cos(2 * np.pi * hour / 24)
    
    # 2. Tạo các biến trễ (Lag features)
    df["temp_lag1"] = df["temperature"].shift(1)
    df["temp_lag2"] = df["temperature"].shift(2)
    df["temp_diff"] = df["temp_lag1"] - df["temp_lag2"]
    
    df["hum_lag1"] = df["humidity"].shift(1)
    df["hum_lag2"] = df["humidity"].shift(2)
    df["hum_diff"] = df["hum_lag1"] - df["hum_lag2"]
    
    df["pres_lag1"] = df["pressure"].shift(1)
    df["pres_lag2"] = df["pressure"].shift(2)
    df["pres_diff"] = df["pres_lag1"] - df["pres_lag2"] # Xu hướng khí áp ngắn hạn (1h)
    
    # Xu hướng khí áp trung hạn (3h: thay đổi giữa 1h trước và 4h trước)
    df["pres_trend_3h"] = df["pressure"].shift(1) - df["pressure"].shift(4)
    
    df["rain_lag1"] = df["rain_analog"].shift(1)
    df["rain_lag2"] = df["rain_analog"].shift(2)
    
    # 3. Tính toán chỉ số nhiệt cảm nhận (Heat Index) và độ trễ của nó
    df["heat_index"] = calculate_heat_index(df["temperature"], df["humidity"])
    df["heat_index_lag1"] = df["heat_index"].shift(1)
    
    # 4. Tạo các biến mục tiêu (Targets) cần dự đoán cho chu kỳ TIẾP THEO (+1h)
    df["target_temp"] = df["temperature"]
    df["target_hum"] = df["humidity"]
    
    # Trạng thái thời tiết chu kỳ tiếp theo:
    # 2 (Mưa): Cảm biến mưa < 700
    # 1 (Nhiều mây): Cảm biến mưa >= 700 và Độ ẩm >= 78%
    # 0 (Nắng ráo): Cảm biến mưa >= 700 và Độ ẩm < 78%
    weather_status = []
    for idx, row in df.iterrows():
        if row["rain_analog"] < 700:
            status = 2 # Mưa
        elif row["humidity"] >= 78.0:
            status = 1 # Nhiều mây / u ám
        else:
            status = 0 # Nắng ráo
        weather_status.append(status)
    df["weather_status"] = weather_status
    df["target_status"] = df["weather_status"]
    
    # Loại bỏ các hàng có giá trị NaN do dịch chuyển (shift)
    df = df.dropna().copy()
    
    # Định nghĩa các đặc trưng đầu vào cho mô hình (Features)
    feature_cols = [
        "hour_sin", "hour_cos",
        "temp_lag1", "temp_lag2", "temp_diff",
        "hum_lag1", "hum_lag2", "hum_diff",
        "pres_lag1", "pres_lag2", "pres_diff", "pres_trend_3h",
        "rain_lag1", "rain_lag2",
        "heat_index_lag1"
    ]
    
    return df, feature_cols

def train_and_evaluate():
    # Tải dữ liệu
    df = fetch_historical_data()
    
    # Trích xuất đặc trưng
    df_feat, feature_cols = feature_engineering(df)
    
    X = df_feat[feature_cols]
    y_temp = df_feat["target_temp"]
    y_hum = df_feat["target_hum"]
    y_status = df_feat["target_status"]
    
    # Chia tập Train/Test (80% / 20%)
    X_train, X_test, y_temp_train, y_temp_test = train_test_split(X, y_temp, test_size=0.2, random_state=42)
    _, _, y_hum_train, y_hum_test = train_test_split(X, y_hum, test_size=0.2, random_state=42)
    _, _, y_status_train, y_status_test = train_test_split(X, y_status, test_size=0.2, random_state=42)
    
    print("\n=== 3. Huấn luyện Mô hình Học máy (Random Forest) ===")
    
    # A. Model dự đoán Nhiệt độ
    print("Đang huấn luyện mô hình dự đoán Nhiệt độ...")
    model_temp = RandomForestRegressor(n_estimators=100, max_depth=12, random_state=42)
    model_temp.fit(X_train, y_temp_train)
    pred_temp = model_temp.predict(X_test)
    rmse_temp = np.sqrt(mean_squared_error(y_temp_test, pred_temp))
    mae_temp = mean_absolute_error(y_temp_test, pred_temp)
    print(f"Nhiệt độ -> RMSE: {rmse_temp:.3f} °C, MAE: {mae_temp:.3f} °C")
    
    # B. Model dự đoán Độ ẩm
    print("Đang huấn luyện mô hình dự đoán Độ ẩm...")
    model_hum = RandomForestRegressor(n_estimators=100, max_depth=12, random_state=42)
    model_hum.fit(X_train, y_hum_train)
    pred_hum = model_hum.predict(X_test)
    rmse_hum = np.sqrt(mean_squared_error(y_hum_test, pred_hum))
    mae_hum = mean_absolute_error(y_hum_test, pred_hum)
    print(f"Độ ẩm    -> RMSE: {rmse_hum:.3f} %, MAE: {mae_hum:.3f} %")
    
    # C. Model dự đoán Trạng thái Thời tiết & Xác suất mưa
    print("Đang huấn luyện mô hình phân loại Trạng thái Thời tiết...")
    model_status = RandomForestClassifier(n_estimators=150, max_depth=10, class_weight="balanced", random_state=42)
    model_status.fit(X_train, y_status_train)
    pred_status = model_status.predict(X_test)
    acc_status = accuracy_score(y_status_test, pred_status)
    print(f"Trạng thái thời tiết -> Độ chính xác (Accuracy): {acc_status*100:.2f}%")
    print("\nBáo cáo phân loại chi tiết (Classification Report):")
    print(classification_report(y_status_test, pred_status, target_names=["Nắng ráo", "Nhiều mây", "Mưa"]))
    
    # Đánh giá tầm quan trọng của các đặc trưng (Feature Importance) cho Báo cáo môn học
    importances = model_status.feature_importances_
    indices = np.argsort(importances)[::-1]
    print("\nTầm quan trọng đặc trưng (Feature Importance) trong dự báo thời tiết:")
    for f in range(X.shape[1]):
        print(f"{f + 1}. {feature_cols[indices[f]]:12} : {importances[indices[f]]*100:.2f}%")
        
    # 4. Lưu trữ Mô hình
    print("\n=== 4. Lưu trữ Mô hình đã huấn luyện thành công ===")
    joblib.dump(model_temp, "models/temp_model.joblib")
    joblib.dump(model_hum, "models/hum_model.joblib")
    joblib.dump(model_status, "models/status_model.joblib")
    joblib.dump(feature_cols, "models/feature_cols.joblib")
    print("Đã lưu các file model (.joblib) vào thư mục 'models/'.")
    print("Quá trình huấn luyện hoàn tất!")

if __name__ == "__main__":
    train_and_evaluate()
