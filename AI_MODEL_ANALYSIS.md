# Phân tích Mô hình Dự đoán AI (Edge AI Weather Station)

Hệ thống Trạm khí tượng thông minh tích hợp giải pháp học máy (Machine Learning) để đưa ra dự báo thời tiết cục bộ ngắn hạn (Multi-Horizon Forecast) trực tiếp trên Web Dashboard và Gateway. Dưới đây là phân tích chi tiết về kiến trúc, thuật toán, kỹ thuật đặc trưng và giải pháp huấn luyện của mô hình.

---

## 1. Kiến trúc Tổng quan của Mô hình
Mô hình AI trong dự án hoạt động theo cơ chế **Đa nhiệm Song song (Parallel Multi-Task Inference)**. Thay vì dùng một mô hình lớn duy nhất, hệ thống chia nhỏ thành 3 mô hình độc lập chuyên biệt hoạt động song song để tối ưu hóa độ chính xác và tốc độ suy luận:

- **Mô hình dự báo Nhiệt độ**: Dự báo giá trị nhiệt độ cho giờ tiếp theo ($t+1$h) sử dụng thuật toán hồi quy Random Forest.
- **Mô hình dự báo Độ ẩm**: Dự báo giá trị độ ẩm cho giờ tiếp theo ($t+1$h) sử dụng thuật toán hồi quy Random Forest.
- **Mô hình dự báo Trạng thái & Xác suất mưa**: Phân loại thời tiết thành 3 lớp (0: Nắng ráo, 1: Nhiều mây, 2: Mưa dông) và trích xuất xác suất dự báo mưa.

---

## 2. Thiết kế Đặc trưng (Feature Engineering)
Kỹ thuật thiết kế đặc trưng đóng vai trò quyết định hiệu năng của mô hình chuỗi thời gian (Time-series Forecasting). Các đặc trưng được phân thành 4 nhóm chính:

### A. Đặc trưng chu kỳ thời gian (Cyclic Temporal Features)
Thời gian (Giờ trong ngày, Tháng trong năm) mang tính chất tuần hoàn. Nhằm giúp mô hình học được mối quan hệ này (như 23h kề sát 0h), chúng ta sử dụng mã hóa vòng tròn hình sin/cos:
- `hour_sin` / `hour_cos`
- `month_sin` / `month_cos`

### B. Đặc trưng trễ & biến động (Lag & Trend Features)
Mô hình sử dụng lịch sử biến đổi gần đây để hiểu hướng đi của thời tiết:
- **Biến trễ (Lag 1h, Lag 2h)**: Nhiệt độ, độ ẩm, khí áp, lượng mưa tại $t-1$ và $t-2$.
- **Độ lệch (Diffs)**: Tốc độ biến thiên ngắn hạn (ví dụ: chênh lệch áp suất khí quyển giảm nhanh cảnh báo mưa dông).
- **Trung bình trượt (Rolling Mean 3h)**: Giúp triệt tiêu nhiễu tức thời của cảm biến.

### C. Chỉ số vật lý & sinh học nông nghiệp (Biometeorological Indices)
- **Chỉ số nhiệt oi bức (Heat Index)**: Kết hợp nhiệt độ và độ ẩm biểu thị mức độ oi bức thực tế ảnh hưởng cây trồng.
- **Điểm sương (Dew point)**: Ngưỡng ngưng tụ hơi nước thành sương, chỉ số đáng tin cậy dự báo khả năng mưa.

---

## 3. Thuật toán Huấn luyện
Thuật toán **Rừng ngẫu nhiên (Random Forest)** được lựa chọn:
- **Nhiệm vụ hồi quy (Regressor)**: Dự báo Nhiệt độ và Độ ẩm liên tục.
- **Nhiệm vụ phân loại (Classifier)**: Phân loại Trạng thái Thời tiết với cấu hình trọng số cân bằng lớp (`class_weight="balanced"`) nhằm triệt tiêu thiên vị do dữ liệu mất cân bằng (số ngày nắng nhiều hơn số ngày mưa dông).

---

## 4. Giải pháp Chặn đứng Rò rỉ Dữ liệu (Data Leakage Prevention)
- **Phân chia tuần tự (Time-series Split)**: Tập dữ liệu huấn luyện và kiểm thử được chia tách liên tục theo dòng thời gian (80% trước để huấn luyện, 20% sau để kiểm thử), tuyệt đối không xáo trộn dữ liệu ngẫu nhiên (no random shuffle) nhằm mô phỏng đúng thực tế vận hành.
- **Đồng bộ đặc trưng (Train-Serve Alignment)**: Thiết kế đầu vào suy luận (`predict_live.py`) đồng nhất hoàn toàn với cấu trúc đặc trưng khi huấn luyện (`train.py`).

---

## 5. Cơ chế Dự báo Đa tầng (Multi-Horizon Inference)
- **Dự báo +1h (AI Core)**: Do mô hình ML Random Forest xử lý và cập nhật lên các trường `field6-8` trên ThingSpeak.
- **Ngoại suy dài hạn (+3h, +6h, +12h)**: Trình duyệt Client tính toán ngoại suy bằng thuật toán Hồi quy tuyến tính (Linear Regression) nội bộ nhanh chóng.
- **Hao hụt độ tin cậy (Confidence Decay)**: Giảm độ mờ (opacity) trên giao diện Web Dashboard cho các thẻ dự báo xa để phản ánh trực quan sai số tăng dần theo thời gian.
