import React from 'react';

export default function MetricCard({ type, value, rawValue, statusText, trendText, progressValue }) {
  // Configs for different metric card types
  let cardClass = "";
  let icon = null;
  let title = "";
  let unit = "";
  let valueDisplay = (value !== null && value !== undefined) ? value.toFixed(1) : '--';
  let extraContent = null;

  switch (type) {
    case 'humidity':
      cardClass = "param-card glass-card hum-theme";
      title = "Độ Ẩm Không Khí";
      unit = "%";
      icon = (
        <svg viewBox="0 0 24 24" width="22" height="22" fill="none" stroke="currentColor" strokeWidth="2">
          <path d="M12 2.69l5.66 5.66a8 8 0 1 1-11.31 0z"/>
        </svg>
      );
      extraContent = (
        <div className="progress-bar-container">
          <div className="progress-bar" style={{ width: `${value !== null ? value : 0}%` }}></div>
        </div>
      );
      break;

    case 'pressure':
      cardClass = "param-card glass-card pres-theme";
      title = "Áp Suất Khí Quyển";
      unit = "hPa";
      icon = (
        <svg viewBox="0 0 24 24" width="22" height="22" fill="none" stroke="currentColor" strokeWidth="2">
          <line x1="18" y1="20" x2="18" y2="10"></line>
          <line x1="12" y1="20" x2="12" y2="4"></line>
          <line x1="6" y1="20" x2="6" y2="14"></line>
        </svg>
      );
      
      let badgeText = "--";
      let badgeStyle = { color: "#a855f7", backgroundColor: "rgba(168, 85, 247, 0.12)" };
      
      if (value !== null) {
        if (value < 1009) {
          badgeText = "Áp suất thấp";
          badgeStyle = { color: "#ef4444", backgroundColor: "rgba(239, 68, 68, 0.12)" };
        } else if (value > 1014) {
          badgeText = "Áp suất cao";
          badgeStyle = { color: "#10b981", backgroundColor: "rgba(16, 185, 129, 0.12)" };
        } else {
          badgeText = "Bình thường";
        }
      }

      extraContent = (
        <>
          <div className="card-subtext">{trendText || 'Trend: --'}</div>
          <div style={{ marginTop: '4px' }}>
            <span className="pressure-badge" style={badgeStyle}>{badgeText}</span>
          </div>
        </>
      );
      break;

    case 'rain':
      cardClass = "param-card glass-card rain-theme";
      title = "Lượng Mưa & Tưới Tiêu";
      icon = (
        <svg viewBox="0 0 24 24" width="22" height="22" fill="none" stroke="currentColor" strokeWidth="2">
          <path d="M20 17.58A5 5 0 0 0 18 8h-1.26A8 8 0 1 0 4 16.25"/>
          <line x1="8" y1="16" x2="8" y2="22"></line>
          <line x1="12" y1="18" x2="12" y2="22"></line>
          <line x1="16" y1="16" x2="16" y2="22"></line>
        </svg>
      );
      valueDisplay = rawValue !== null && rawValue !== undefined ? rawValue : '--';
      unit = "/1023";
      
      let rainText = "Đang đọc...";
      let rainColor = "#10b981";
      if (rawValue !== null && rawValue !== undefined) {
        if (rawValue < 300) {
          rainText = "🌧️ Mưa lớn";
          rainColor = "#ef4444";
        } else if (rawValue < 700) {
          rainText = "🌦️ Mưa nhỏ / Phun";
          rainColor = "#eab308";
        } else {
          rainText = "☀️ Không mưa";
          rainColor = "#10b981";
        }
      }

      extraContent = (
        <span className="rain-status-text" style={{ color: rainColor }}>{rainText}</span>
      );
      break;

    case 'battery':
      cardClass = "param-card glass-card bat-theme";
      title = "Pin Trạm Khí Tượng";
      unit = "%";
      icon = (
        <svg viewBox="0 0 24 24" width="22" height="22" fill="none" stroke="currentColor" strokeWidth="2">
          <rect x="2" y="7" width="16" height="10" rx="2" ry="2"></rect>
          <line x1="22" y1="11" x2="22" y2="13"></line>
        </svg>
      );
      valueDisplay = (value !== null && value !== undefined) ? value.toFixed(0) : '--';
      
      let fillBackground = "var(--accent-bat-gradient)";
      if (value !== null) {
        if (value < 20) fillBackground = "var(--error)";
        else if (value < 50) fillBackground = "var(--warning)";
      }

      extraContent = (
        <div className="battery-battery-bar">
          <div className="battery-fill" style={{ width: `${value !== null ? value : 0}%`, background: fillBackground }}></div>
        </div>
      );
      break;

    default:
      break;
  }

  return (
    <div className={cardClass}>
      <div className="card-icon">
        {icon}
      </div>
      <div className="card-info">
        <h3>{title}</h3>
        <div className="card-value">
          {valueDisplay}
          <span className="unit">{unit}</span>
        </div>
        {extraContent}
      </div>
    </div>
  );
}
