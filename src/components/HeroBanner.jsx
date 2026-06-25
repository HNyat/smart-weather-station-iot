import React from 'react';

export default function HeroBanner({ temperature, humidity, rain, predictedTemp, rainProbability, predictedStatus }) {
  const isRaining = rain !== null && rain < 500;

  let summary = 'Đang lấy dữ liệu cảm biến...';
  let statusTag = 'Đang đọc...';
  let iconName = 'device_thermostat';
  let iconColor = 'text-primary';
  let glowClass = '';
  let badgeColor = 'bg-primary/10 border-primary/30 text-primary';

  if (temperature !== null && humidity !== null) {
    if (isRaining) {
      summary = 'Hiện có mưa dông khí quyển 🌧️. Bà con canh tác lưu ý rào chắn, kiểm tra hệ thống thoát nước chống úng rễ cà phê, tiêu và ngưng bón phân.';
      statusTag = 'Mưa dông úng ngập';
      iconName = 'rainy';
      iconColor = 'text-humidity-cyan';
      glowClass = 'neon-glow-cyan';
      badgeColor = 'bg-humidity-cyan/10 border-humidity-cyan/30 text-humidity-cyan';
    } else {
      if (temperature > 34) {
        summary = 'Thời tiết nắng nóng gay gắt 🥵. Hãy tăng cường tưới ẩm đất rễ cây, đặc biệt là cây con và cà phê mới xuống giống để tránh héo lá.';
        statusTag = 'Nắng nóng canh tác';
        iconName = 'sunny';
        iconColor = 'text-temp-orange';
        glowClass = 'neon-glow-temp';
        badgeColor = 'bg-temp-orange/10 border-temp-orange/30 text-temp-orange';
      } else if (humidity > 85) {
        summary = 'Độ ẩm không khí rất cao ☁️. Đề phòng nguy cơ phát triển các loại nấm ký sinh có hại (rỉ sắt, nấm hồng). Hạn chế phun phân bón lá lúc này.';
        statusTag = 'Cảnh báo dịch hại';
        iconName = 'cloudy';
        iconColor = 'text-primary';
        glowClass = '';
        badgeColor = 'bg-primary/10 border-primary/30 text-primary';
      } else {
        summary = 'Thời tiết nắng ráo thuận lợi ☀️. Thích hợp để tiến hành bón phân dinh dưỡng, làm cỏ đất đai, tỉa cành hoặc phơi sấy hạt nông sản.';
        statusTag = 'CANH TÁC TỐT';
        iconName = 'sunny';
        iconColor = 'text-temp-orange';
        glowClass = 'neon-glow-temp';
        badgeColor = 'bg-temp-orange/10 border-temp-orange/30 text-temp-orange';
      }
    }
  }

  const aiStatusTextMap = { 0: 'Nắng ráo', 1: 'Nhiều mây', 2: 'Mưa dông' };
  const aiStatusIconMap = { 0: 'sunny', 1: 'cloudy', 2: 'rainy' };

  return (
    <div className="glass-card p-6 md:p-8 mb-6 relative overflow-hidden flex flex-col md:flex-row items-center justify-between gap-6">
      {/* Background glow overlay */}
      <div className={`absolute top-0 right-0 w-64 h-64 ${isRaining ? 'bg-humidity-cyan/10' : 'bg-temp-orange/10'} rounded-full blur-[80px] -mr-32 -mt-32 pointer-events-none`}></div>
      
      {/* Left Column: Agricultural recommendation */}
      <div className="flex-1 space-y-4 relative z-10">
        <div className={`inline-flex items-center gap-2 px-3 py-1 border ${badgeColor} rounded-full`}>
          <span className="material-symbols-outlined text-sm">agriculture</span>
          <span className="font-label-caps text-[10px] uppercase font-bold tracking-wider">{statusTag}</span>
        </div>
        <p className="font-body-lg text-[16px] md:text-[18px] text-on-surface leading-relaxed max-w-2xl">
          {summary}
        </p>
      </div>

      {/* Right Column: Visual metrics and AI Panel */}
      <div className="flex flex-wrap items-center gap-6 md:gap-8 relative z-10 w-full md:w-auto justify-between md:justify-end">
        <div className="flex items-center gap-4">
          <span className={`material-symbols-outlined text-6xl md:text-7xl ${iconColor} ${glowClass}`} style={{ fontVariationSettings: "'FILL' 1" }}>
            {iconName}
          </span>
          <div className="text-right">
            <span className={`font-display-lg text-5xl md:text-6xl font-extrabold text-on-surface ${glowClass}`}>
              {temperature !== null ? temperature.toFixed(1) : '--'}
              <span className={iconColor}>°C</span>
            </span>
          </div>
        </div>

        {/* Embedded mini forecast status */}
        <div className="flex flex-col gap-2 min-w-[200px] border-t md:border-t-0 md:border-l border-glass-border pt-4 md:pt-0 md:pl-6 w-full md:w-auto">
          <div className="flex items-center justify-between gap-4 bg-surface-container/50 px-3 py-1.5 rounded-md border border-glass-border">
            <div className="flex items-center gap-2">
              <span className="material-symbols-outlined text-[16px] text-secondary-fixed-dim">smart_toy</span>
              <span className="font-data-mono text-[11px] text-on-surface-variant">AI Dự Báo (+1h):</span>
            </div>
            <span className="font-data-mono text-[12px] font-bold text-on-surface">
              {predictedTemp !== null ? `${predictedTemp.toFixed(1)} °C` : '-- °C'}
            </span>
          </div>
          <div className="flex items-center justify-between gap-4 bg-surface-container/50 px-3 py-1.5 rounded-md border border-glass-border">
            <div className="flex items-center gap-2">
              <span className="material-symbols-outlined text-[16px] text-humidity-cyan">rainy</span>
              <span className="font-data-mono text-[11px] text-on-surface-variant">Xác suất mưa:</span>
            </div>
            <span className="font-data-mono text-[12px] font-bold text-on-surface">
              {rainProbability !== null ? `${rainProbability.toFixed(0)}%` : '--%'}
            </span>
          </div>
          <div className="flex items-center justify-between gap-4 bg-surface-container/50 px-3 py-1.5 rounded-md border border-glass-border">
            <div className="flex items-center gap-2">
              <span className="material-symbols-outlined text-[16px] text-air-quality-green">psychology</span>
              <span className="font-data-mono text-[11px] text-on-surface-variant">Dự kiến:</span>
            </div>
            <span className="font-data-mono text-[12px] font-bold text-air-quality-green">
              {predictedStatus !== null ? `${aiStatusTextMap[predictedStatus]}` : 'Tính toán...'}
            </span>
          </div>
        </div>
      </div>
    </div>
  );
}
