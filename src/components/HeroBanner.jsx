import React from 'react';

export default function HeroBanner({ temperature, humidity, rain, predictedTemp, rainProbability, predictedStatus }) {
  const isRaining = rain < 500;
  
  let summary = 'Đang lấy dữ liệu cảm biến...';
  let statusTag = 'Đang đọc...';
  let iconSvg = null;
  
  if (temperature !== null && humidity !== null) {
    if (isRaining) {
      summary = 'Hiện có mưa dông khí quyển 🌧️. Bà con canh tác lưu ý rào chắn, kiểm tra hệ thống thoát nước chống úng rễ cà phê, tiêu và ngưng bón phân.';
      statusTag = 'Mưa dông úng ngập';
      iconSvg = (
        <svg viewBox="0 0 64 64" width="80" height="80" className="weather-icon-svg sun-cloud-anim" style={{ filter: 'drop-shadow(0 6px 12px rgba(56, 189, 248, 0.4))' }}>
          <path d="M46 38a10 10 0 0 0-9-6 10.5 10.5 0 0 0-4 .8A12 12 0 1 0 22 50h24a10 10 0 0 0 0-20z" fill="url(#cloudGrad)"/>
          <line x1="20" y1="54" x2="18" y2="60" stroke="#38bdf8" strokeWidth="3" strokeLinecap="round"/>
          <line x1="28" y1="54" x2="26" y2="60" stroke="#38bdf8" strokeWidth="3" strokeLinecap="round"/>
          <line x1="36" y1="54" x2="34" y2="60" stroke="#38bdf8" strokeWidth="3" strokeLinecap="round"/>
        </svg>
      );
    } else {
      if (temperature > 34) {
        summary = 'Thời tiết nắng nóng gay gắt 🥵. Hãy tăng cường tưới ẩm đất rễ cây, đặc biệt là cây con và cà phê mới xuống giống để tránh héo lá.';
        statusTag = 'Nắng nóng canh tác';
        iconSvg = (
          <svg viewBox="0 0 64 64" width="80" height="80" className="weather-icon-svg sun-cloud-anim">
            <circle cx="32" cy="32" r="16" fill="url(#sunGradient)"/>
            <line x1="32" y1="6" x2="32" y2="12" stroke="#f59e0b" strokeWidth="4" strokeLinecap="round"/>
            <line x1="32" y1="52" x2="32" y2="58" stroke="#f59e0b" strokeWidth="4" strokeLinecap="round"/>
            <line x1="6" y1="32" x2="12" y2="32" stroke="#f59e0b" strokeWidth="4" strokeLinecap="round"/>
            <line x1="52" y1="32" x2="58" y2="32" stroke="#f59e0b" strokeWidth="4" strokeLinecap="round"/>
          </svg>
        );
      } else if (humidity > 85) {
        summary = 'Độ ẩm không khí rất cao ☁️. Đề phòng nguy cơ phát triển các loại nấm ký sinh có hại (rỉ sắt, nấm hồng). Hạn chế phun phân bón lá lúc này.';
        statusTag = 'Cảnh báo dịch hại';
        iconSvg = (
          <svg viewBox="0 0 64 64" width="80" height="80" className="weather-icon-svg sun-cloud-anim">
            <path d="M46 38a10 10 0 0 0-9-6 10.5 10.5 0 0 0-4 .8A12 12 0 1 0 22 50h24a10 10 0 0 0 0-20z" fill="url(#cloudGradient)"/>
          </svg>
        );
      } else {
        summary = 'Thời tiết nắng ráo thuận lợi 🌤️. Thích hợp để tiến hành bón phân dinh dưỡng, làm cỏ đất đai, tỉa cành hoặc phơi sấy hạt nông sản.';
        statusTag = 'Canh tác tốt';
        iconSvg = (
          <svg viewBox="0 0 64 64" width="80" height="80" className="weather-icon-svg sun-cloud-anim">
            <circle cx="24" cy="24" r="12" fill="url(#sunGradient)"/>
            <path d="M46 38a10 10 0 0 0-9-6 10.5 10.5 0 0 0-4 .8A12 12 0 1 0 22 50h24a10 10 0 0 0 0-20z" fill="url(#cloudGradient)" opacity="0.95"/>
          </svg>
        );
      }
    }
  }

  const aiStatusTextMap = { 0: 'Nắng ráo', 1: 'Nhiều mây', 2: 'Mưa dông' };
  const aiStatusIconMap = { 0: '☀️', 1: '☁️', 2: '🌧️' };
  
  return (
    <section className="hero-weather-banner">
      <div className="hero-banner-overlay"></div>
      <div className="hero-left">
        <div className="weather-visual">
          <div id="weatherIconContainer">
            {iconSvg || (
              <svg viewBox="0 0 64 64" width="80" height="80" className="weather-icon-svg sun-cloud-anim">
                <circle cx="24" cy="24" r="12" fill="url(#sunGradient)"/>
                <path d="M46 38a10 10 0 0 0-9-6 10.5 10.5 0 0 0-4 .8A12 12 0 1 0 22 50h24a10 10 0 0 0 0-20z" fill="url(#cloudGradient)" opacity="0.95"/>
              </svg>
            )}
          </div>
        </div>
        <div className="weather-info">
          <span className="weather-status-tag">{statusTag}</span>
          <div className="weather-summary-text">{summary}</div>
        </div>
      </div>

      <div className="hero-right">
        <div className="main-temp-display">
          <span className="temp-number">{temperature !== null ? temperature.toFixed(1) : '--'}</span>
          <span className="temp-unit">°C</span>
        </div>
        <div className="hero-badges">
          <div className="hero-badge">
            <span className="badge-icon">🤖</span>
            <span className="badge-label">Python AI dự báo:</span>
            <span className="badge-value">{predictedTemp !== null ? `${predictedTemp.toFixed(1)} °C` : '-- °C'}</span>
          </div>
          <div className="hero-badge">
            <span className="badge-icon">⛈️</span>
            <span className="badge-label">Xác suất mưa:</span>
            <span className="badge-value">{rainProbability !== null ? `${rainProbability.toFixed(0)}%` : '--%'}</span>
          </div>
          <div className="hero-badge">
            <span className="badge-icon">
              {predictedStatus !== null ? aiStatusIconMap[predictedStatus] : '🤖'}
            </span>
            <span className="badge-label">Dự kiến (+1h):</span>
            <span className="badge-value">
              {predictedStatus !== null ? `${aiStatusTextMap[predictedStatus]} ${aiStatusIconMap[predictedStatus]}` : 'Đang tính...'}
            </span>
          </div>
        </div>
      </div>
    </section>
  );
}
