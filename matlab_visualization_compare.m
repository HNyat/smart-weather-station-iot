%% =============================================================
%   MATLAB Visualization — So sánh Nhiệt độ Thực tế vs Dự đoán AI
%   Dán vào: ThingSpeak > Apps > MATLAB Visualizations > New > Custom
% =============================================================
%   Đây là biểu đồ CHỨNG MINH AI hoạt động: vẽ chung 2 đường trên
%   1 chart — Thực tế (field1) và Dự đoán (field6) — để thấy đường
%   dự đoán có "bám theo/đón đầu" xu hướng thực tế hay không.
% =============================================================

channelID  = 0000000;               % ← ĐIỀN Channel ID
readAPIKey = 'YOUR_READ_API_KEY';   % ← ĐIỀN Read API Key

numPoints = 50;   % Số điểm gần nhất hiển thị trên chart

data = thingSpeakRead(channelID, 'Fields', [1, 6], ...
    'NumPoints', numPoints, 'ReadKey', readAPIKey, ...
    'OutputFormat', 'table');

if height(data) < 2
    error('Chưa đủ dữ liệu để vẽ — đợi thêm vài chu kỳ MATLAB Analysis chạy.');
end

% Dịch chuyển đường dự báo xuống 1 bước (shift down) để khớp đúng mốc thời gian thực tế.
% Giải thích: Giá trị dự báo ghi ở hàng k là kết quả dự đoán cho hàng k+1.
predTempShifted = [NaN; data.PredictedTemp(1:end-1)];

% Tính toán sai số tuyệt đối trung bình (MAE) để đánh giá độ chính xác của mô hình
validIdx = ~isnan(predTempShifted) & ~isnan(data.Temperature);
if any(validIdx)
    maeVal = mean(abs(data.Temperature(validIdx) - predTempShifted(validIdx)));
    titleStr = sprintf('So sánh Nhiệt độ: Thực tế vs Dự đoán AI (MAE = %.2f °C)', maeVal);
else
    titleStr = 'So sánh Nhiệt độ: Thực tế vs Dự đoán AI';
end

figure;
plot(data.Timestamps, data.Temperature, '-o', ...
    'LineWidth', 1.6, 'DisplayName', 'Thực tế (Sensor)');
hold on;
plot(data.Timestamps, predTempShifted, '--s', ...
    'LineWidth', 1.6, 'DisplayName', 'Dự đoán (AI - Linear Regression)');
hold off;

xlabel('Thời gian');
ylabel('Nhiệt độ (°C)');
title(titleStr);
legend('Location', 'best');
grid on;
