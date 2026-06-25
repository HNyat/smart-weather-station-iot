import React from 'react';

// Math Helper: computes slope (a) and intercept (b) of linear regression
function calculateLinearRegression(yArr) {
  const n = yArr.length;
  if (n < 2) return { a: 0, b: 0, predictNext: 0 };
  
  const xArr = Array.from({ length: n }, (_, i) => i + 1);
  
  let sumX = 0;
  let sumY = 0;
  let sumXY = 0;
  let sumXX = 0;
  
  for (let i = 0; i < n; i++) {
    sumX += xArr[i];
    sumY += yArr[i];
    sumXY += xArr[i] * yArr[i];
    sumXX += xArr[i] * xArr[i];
  }
  
  const denominator = (n * sumXX) - (sumX * sumX);
  if (denominator === 0) return { a: 0, b: yArr[0], predictNext: yArr[0] };
  
  const a = ((n * sumXY) - (sumX * sumY)) / denominator;
  const b = (sumY - (a * sumX)) / n;
  
  const nextX = n + 1;
  const predictNext = (a * nextX) + b;
  
  return { a, b, predictNext };
}

export default function LocalEngine({ historyData, rainValue }) {
  const tempHistory = historyData.temperature || [];
  const humHistory = historyData.humidity || [];
  const presHistory = historyData.pressure || [];
  
  const n = tempHistory.length;
  
  // Return early if not enough data points
  if (n < 5) {
    return (
      <div className="glass-card forecast-card">
        <div className="card-header">
          <div className="header-title">
            <div className="spark-icon">⚡</div>
            <h2>Dự Báo Cây Trồng AI Cục Bộ (Hồi Quy)</h2>
          </div>
          <span className="engine-badge">JS Engine v1.0</span>
        </div>
        <div className="recommendation-box" style={{ marginTop: '16px' }}>
          <div className="rec-icon">💡</div>
          <div className="rec-content">
            <h4>💡 Đang tích lũy dữ liệu:</h4>
            <p>Đang chờ thu thập đủ dữ liệu khí tượng cục bộ (Đã có {n}/5 điểm dữ liệu)...</p>
          </div>
        </div>
      </div>
    );
  }

  // Calculate regressions
  const regTemp = calculateLinearRegression(tempHistory);
  const regHum = calculateLinearRegression(humHistory);
  const regPres = calculateLinearRegression(presHistory);

  // Helper to render trend badges
  const renderTrendBadge = (slope, threshold) => {
    if (slope > threshold) {
      return <span className="pred-trend trend-up">Tăng 📈</span>;
    } else if (slope < -threshold) {
      return <span className="pred-trend trend-down">Giảm 📉</span>;
    } else {
      return <span className="pred-trend trend-stable">Ổn định ➔</span>;
    }
  };

  // Generate agricultural recommendation text based on slopes
  const generateRecommendation = (slopeT, slopeH, slopeP) => {
    if (slopeT < -0.05 && slopeH > 0.1) {
      return "Khuyến nghị canh tác: Nhiệt độ giảm nhanh và độ ẩm tăng mạnh. Khả năng cao có mưa rào hoặc dông lốc trong 30-60 phút tới. Bà con nên ngưng ngay việc bón phân, thu hồi nông sản đang phơi sấy (cà phê, lúa) và rà soát hệ thống tiêu úng.";
    } else if (slopeP < -0.05 && rainValue < 900) {
      return "Khuyến nghị canh tác: Khí áp giảm kèm lượng mưa tích tụ. Dấu hiệu của áp thấp nhiệt đới hoặc mưa dông kéo dài. Cảnh báo nguy cơ thối rễ cây ăn trái ở Miền Tây. Hãy gia cố bờ bao vườn sầu riêng, cam sành để chủ động chống ngập úng.";
    } else if (slopeT > 0.05 && slopeH < -0.1) {
      return "Khuyến nghị canh tác: Nhiệt độ tăng và độ ẩm giảm dần. Trời nắng ấm tạnh ráo. Thời tiết rất thích hợp để bón phân thúc nông sản, tiến hành phun thuốc phòng trừ sâu bệnh hoặc phơi sấy hạt tiêu, hạt cà phê.";
    } else if (Math.abs(slopeT) <= 0.05 && Math.abs(slopeH) <= 0.1) {
      return "Khuyến nghị canh tác: Khí áp và độ ẩm không khí rất ổn định. Không phát hiện biến động thời tiết cực đoan trong 2 giờ tới. Điều kiện lý tưởng để cấy lúa, xuống giống cây trồng mới hoặc cắt tỉa cành tạo tán.";
    } else {
      return "Nhận định khuyến nông: Chỉ số nhiệt ẩm dao động ổn định trong biên độ canh tác an toàn. Bà con duy trì lượng nước tưới tiêu buổi sáng/chiều và chăm sóc cây trồng theo lịch định kỳ.";
    }
  };

  const recText = generateRecommendation(regTemp.a, regHum.a, regPres.a);

  return (
    <div className="glass-card forecast-card">
      <div className="card-header">
        <div className="header-title">
          <div className="spark-icon">⚡</div>
          <h2>Dự Báo Cây Trồng AI Cục Bộ (Hồi Quy)</h2>
        </div>
        <span className="engine-badge">JS Engine v1.0</span>
      </div>
      
      <div className="forecast-grid">
        <div className="prediction-box">
          <div className="pred-item">
            <span className="pred-label">Nhiệt độ dự tính (+1h)</span>
            <span className="pred-val">{regTemp.predictNext.toFixed(1)} °C</span>
            {renderTrendBadge(regTemp.a, 0.05)}
          </div>
          <div className="pred-item">
            <span className="pred-label">Độ ẩm dự tính (+1h)</span>
            <span className="pred-val">{regHum.predictNext.toFixed(1)} %</span>
            {renderTrendBadge(regHum.a, 0.1)}
          </div>
          <div className="pred-item">
            <span className="pred-label">Áp suất dự tính (+1h)</span>
            <span className="pred-val">{regPres.predictNext.toFixed(1)} hPa</span>
            {renderTrendBadge(regPres.a, 0.05)}
          </div>
        </div>

        <div className="regression-math-box">
          <h3>Phương trình xu hướng canh tác:</h3>
          <div className="math-formula">
            T(t) = {regTemp.a.toFixed(2)} * t {regTemp.b >= 0 ? '+' : '-'} {Math.abs(regTemp.b).toFixed(2)}
          </div>
          <div className="math-formula">
            H(t) = {regHum.a.toFixed(2)} * t {regHum.b >= 0 ? '+' : '-'} {Math.abs(regHum.b).toFixed(2)}
          </div>
          <div className="math-formula">
            P(t) = {regPres.a.toFixed(2)} * t {regPres.b >= 0 ? '+' : '-'} {Math.abs(regPres.b).toFixed(2)}
          </div>
        </div>
      </div>

      <hr className="card-divider" />

      <div className="recommendation-box">
        <div className="rec-icon">💡</div>
        <div className="rec-content">
          <h4>💡 Khuyên dùng cho canh tác & tưới tiêu:</h4>
          <p>{recText}</p>
        </div>
      </div>
    </div>
  );
}
