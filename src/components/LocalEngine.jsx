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
  
  // Helper to render trend badges matching Stitch styles
  const renderTrendBadge = (slope, threshold) => {
    if (slope > threshold) {
      return (
        <span className="px-2 py-0.5 bg-error-container/20 border border-error-container/50 text-error rounded text-[10px] font-bold font-data-mono">
          Tăng 📈
        </span>
      );
    } else if (slope < -threshold) {
      return (
        <span className="px-2 py-0.5 bg-humidity-cyan/10 border border-humidity-cyan/20 text-humidity-cyan rounded text-[10px] font-bold font-data-mono">
          Giảm 📉
        </span>
      );
    } else {
      return (
        <span className="px-2 py-0.5 bg-air-quality-green/10 border border-air-quality-green/20 text-air-quality-green rounded text-[10px] font-bold font-data-mono">
          Ổn định ➔
        </span>
      );
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

  if (n < 5) {
    return (
      <div className="glass-card p-6 relative overflow-hidden">
        <div className="flex justify-between items-start mb-6">
          <div className="flex items-center gap-2">
            <span className="material-symbols-outlined text-temp-orange text-xl">bolt</span>
            <h3 className="font-headline-sm text-lg font-bold text-on-surface">Dự Báo Cây Trồng AI Cục Bộ (Hồi Quy)</h3>
          </div>
          <span className="font-data-mono text-xs text-on-surface-variant border border-glass-border px-3 py-1 rounded-full">JS Engine v1.0</span>
        </div>
        <div className="bg-temp-orange/5 border border-temp-orange/20 p-4 rounded-lg flex items-start gap-3">
          <span className="material-symbols-outlined text-temp-orange">lightbulb</span>
          <div>
            <h4 className="font-bold text-temp-orange text-sm mb-1">Đang tích lũy dữ liệu:</h4>
            <p className="text-sm text-on-surface-variant">Đang chờ thu thập đủ dữ liệu khí tượng cục bộ (Đã có {n}/5 điểm dữ liệu)...</p>
          </div>
        </div>
      </div>
    );
  }

  // Calculate regressions
  const regTemp = calculateLinearRegression(tempHistory);
  const regHum = calculateLinearRegression(humHistory);
  const regPres = calculateLinearRegression(presHistory);
  const recText = generateRecommendation(regTemp.a, regHum.a, regPres.a);

  return (
    <div className="glass-card p-6 relative overflow-hidden">
      {/* Decorative background brain symbol */}
      <div className="absolute right-0 top-0 opacity-5 pointer-events-none">
        <span className="material-symbols-outlined text-[150px]">psychology</span>
      </div>

      {/* Title & Badge */}
      <div className="flex justify-between items-start mb-6">
        <div className="flex items-center gap-2">
          <span className="material-symbols-outlined text-temp-orange text-xl">bolt</span>
          <h3 className="font-headline-sm text-lg font-bold text-on-surface">Dự Báo Cây Trồng AI Cục Bộ (Hồi Quy)</h3>
        </div>
        <span className="font-data-mono text-xs text-on-surface-variant border border-glass-border px-3 py-1 rounded-full">JS Engine v1.0</span>
      </div>

      {/* Grid Layout for Insights */}
      <div className="grid grid-cols-1 lg:grid-cols-2 gap-6 relative z-10">
        
        {/* Left Column: Predictions table */}
        <div className="space-y-3">
          <div className="flex items-center justify-between bg-surface-container/40 p-3 rounded-lg border border-glass-border">
            <span className="font-data-mono text-[11px] md:text-xs text-on-surface-variant">Nhiệt độ dự tính (+1h)</span>
            <span className="font-data-mono font-bold text-sm text-on-surface">{regTemp.predictNext.toFixed(1)} °C</span>
            {renderTrendBadge(regTemp.a, 0.05)}
          </div>
          <div className="flex items-center justify-between bg-surface-container/40 p-3 rounded-lg border border-glass-border">
            <span className="font-data-mono text-[11px] md:text-xs text-on-surface-variant">Độ ẩm dự tính (+1h)</span>
            <span className="font-data-mono font-bold text-sm text-on-surface">{regHum.predictNext.toFixed(1)} %</span>
            {renderTrendBadge(regHum.a, 0.1)}
          </div>
          <div className="flex items-center justify-between bg-surface-container/40 p-3 rounded-lg border border-glass-border">
            <span className="font-data-mono text-[11px] md:text-xs text-on-surface-variant">Áp suất dự tính (+1h)</span>
            <span className="font-data-mono font-bold text-sm text-on-surface">{regPres.predictNext.toFixed(1)} hPa</span>
            {renderTrendBadge(regPres.a, 0.05)}
          </div>
        </div>

        {/* Right Column: Recommendations & Equations */}
        <div className="flex flex-col justify-between">
          <div className="bg-surface-container/30 p-4 rounded-lg border border-glass-border font-data-mono text-[11px] text-on-tertiary-container mb-4 space-y-1">
            <p className="mb-2 uppercase font-bold text-on-surface-variant">Phương trình xu hướng canh tác:</p>
            <p>T(t) = {regTemp.a.toFixed(2)} * t {regTemp.b >= 0 ? '+' : '-'} {Math.abs(regTemp.b).toFixed(2)}</p>
            <p>H(t) = {regHum.a.toFixed(2)} * t {regHum.b >= 0 ? '+' : '-'} {Math.abs(regHum.b).toFixed(2)}</p>
            <p>P(t) = {regPres.a.toFixed(2)} * t {regPres.b >= 0 ? '+' : '-'} {Math.abs(regPres.b).toFixed(2)}</p>
          </div>
          <div className="bg-temp-orange/5 border border-temp-orange/20 p-4 rounded-lg">
            <div className="flex items-start gap-3">
              <span className="material-symbols-outlined text-temp-orange text-xl mt-0.5">lightbulb</span>
              <div>
                <h4 className="font-bold text-temp-orange text-sm mb-1">Khuyên dùng cho canh tác &amp; tưới tiêu:</h4>
                <p className="text-xs text-on-surface-variant leading-relaxed">{recText}</p>
              </div>
            </div>
          </div>
        </div>

      </div>
    </div>
  );
}
