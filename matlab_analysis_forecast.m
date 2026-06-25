%% =============================================================
%   MATLAB Analysis — Dự đoán thời tiết (Linear Regression)
%   Dán vào: ThingSpeak > Apps > MATLAB Analysis > New > Blank
% =============================================================
%   Dùng 1 CHANNEL DUY NHẤT: đọc field1(Temp),field3(Pressure)
%   để hồi quy, rồi ghi kết quả vào field6,7,8 của CHÍNH channel đó.
%
%   ► ĐIỀN 3 GIÁ TRỊ DƯỚI THEO CHANNEL "WeatherStation" CỦA BẠN:
% =============================================================

channelID   = 0000000;               % Channel ID (số trên URL ThingSpeak)
readAPIKey  = 'YOUR_READ_API_KEY';
writeAPIKey = 'YOUR_WRITE_API_KEY';

numPoints = 12;   % Số điểm gần nhất dùng để hồi quy

% ── 1. Đọc lịch sử Temp (field1) + Pressure (field3) ─────────────
try
    data = thingSpeakRead(channelID, 'Fields', [1, 3], ...
        'NumPoints', numPoints, 'ReadKey', readAPIKey, ...
        'OutputFormat', 'table');
catch ME
    error('Không đọc được dữ liệu: %s', ME.message);
end

if height(data) < 3
    warning('Chưa đủ dữ liệu để hồi quy (cần ≥ 3 điểm) — bỏ qua lần này.');
    return;
end

temp = data.Temperature;
pres = data.Pressure;
t    = (1:length(temp))';

% ── 2. Hồi quy tuyến tính: dự đoán nhiệt độ chu kỳ tới ───────────
pTemp    = polyfit(t, temp, 1);
nextTemp = polyval(pTemp, length(temp) + 1);
nextTemp = max(min(nextTemp, max(temp) + 5), min(temp) - 5); % Giới hạn ngoại suy

% ── 3. Xu hướng áp suất → ước lượng khả năng mưa & Trạng thái ────
pPres         = polyfit(t, pres, 1);
pressureSlope = pPres(1);

if pressureSlope < -1.0
    rainProb = 80;
    status = 2; % Mưa dông
elseif pressureSlope < -0.3
    rainProb = 50;
    status = 1; % Nhiều mây
elseif pressureSlope > 1.0
    rainProb = 5;
    status = 0; % Nắng ráo
else
    rainProb = 20;
    status = 0; % Nắng ráo
end

% ── 4. Ghi kết quả vào field6,7,8 CỦA CÙNG CHANNEL ───────────────
try
    % Ghi [nextTemp, rainProb, status] vào các trường tương ứng để đồng bộ với UI React/HTML
    thingSpeakWrite(channelID, [nextTemp, rainProb, status], ...
        'Fields', [6, 7, 8], 'WriteKey', writeAPIKey);
catch ME
    error('Không ghi được kết quả: %s', ME.message);
end

fprintf('Dự đoán nhiệt độ chu kỳ tới : %.2f °C\n', nextTemp);
fprintf('Khả năng mưa                : %d %%\n',   rainProb);
fprintf('Xu hướng áp suất             : %.3f hPa/chu kỳ\n', pressureSlope);
