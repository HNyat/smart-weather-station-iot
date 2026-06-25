import React from 'react';

export default function MetricCard({ type, value, rawValue, trendText, predictedValue }) {
  let icon = null;
  let title = "";
  let unit = "";
  let valueDisplay = (value !== null && value !== undefined) ? value.toFixed(1) : '--';
  let extraContent = null;

  switch (type) {
    case 'humidity':
      title = "Độ Ẩm Không Khí";
      unit = "%";
      icon = (
        <div className="w-10 h-10 rounded-full bg-humidity-cyan/10 flex items-center justify-center border border-humidity-cyan/30">
          <span className="material-symbols-outlined text-humidity-cyan neon-glow-cyan">water_drop</span>
        </div>
      );
      extraContent = (
        <div className="mt-4 flex flex-col gap-2 w-full">
          <div className="h-1 w-full bg-surface-container rounded-full overflow-hidden">
            <div 
              className="h-full bg-humidity-cyan transition-all duration-500" 
              style={{ width: `${value !== null ? Math.min(Math.max(value, 0), 100) : 0}%` }}
            ></div>
          </div>
          {predictedValue !== null && (
            <span className="text-[10px] text-humidity-cyan font-bold font-data-mono">
              Dự báo AI (+1h): {predictedValue.toFixed(1)}%
            </span>
          )}
        </div>
      );
      break;

    case 'pressure':
      title = "Áp Suất Khí Quyển";
      unit = "hPa";
      icon = (
        <div className="w-10 h-10 rounded-full bg-primary/10 flex items-center justify-center border border-primary/30">
          <span className="material-symbols-outlined text-primary">bar_chart</span>
        </div>
      );
      
      let badgeText = "Đang đọc...";
      let badgeClass = "bg-glass-fill border border-glass-border text-on-surface-variant";
      
      if (value !== null) {
        if (value < 1009) {
          badgeText = "Áp suất thấp";
          badgeClass = "bg-error-container/20 border border-error-container/50 text-error";
        } else if (value > 1014) {
          badgeText = "Áp suất cao";
          badgeClass = "bg-air-quality-green/10 border border-air-quality-green/20 text-air-quality-green";
        } else {
          badgeText = "Bình thường";
          badgeClass = "bg-air-quality-green/10 border border-air-quality-green/20 text-air-quality-green";
        }
      }

      extraContent = (
        <div className="mt-2 flex flex-col gap-1.5">
          <span className="text-[10px] text-on-surface-variant font-data-mono">{trendText || 'Hồi quy: Đang tính...'}</span>
          {predictedValue !== null && (
            <span className="text-[10px] text-primary font-bold font-data-mono">
              Dự báo AI (+1h): {predictedValue.toFixed(1)} hPa
            </span>
          )}
          <div>
            <span className={`inline-flex items-center gap-1 px-2 py-0.5 rounded text-[10px] font-bold ${badgeClass}`}>
              {badgeText}
            </span>
          </div>
        </div>
      );
      break;

    case 'rain':
      title = "Lượng Mưa & Tưới Tiêu";
      icon = (
        <div className="w-10 h-10 rounded-full bg-air-quality-green/10 flex items-center justify-center border border-air-quality-green/30">
          <span className="material-symbols-outlined text-air-quality-green neon-glow-green">rainy</span>
        </div>
      );
      valueDisplay = rawValue !== null && rawValue !== undefined ? rawValue : '--';
      unit = "/1023";
      
      let rainText = "Đang đọc...";
      let rainClass = "text-on-surface-variant";
      let rainIcon = "device_thermostat";
      if (rawValue !== null && rawValue !== undefined) {
        if (rawValue < 300) {
          rainText = "Mưa lớn";
          rainClass = "text-warning-red font-bold";
          rainIcon = "rainy";
        } else if (rawValue < 700) {
          rainText = "Mưa nhỏ / Phun";
          rainClass = "text-temp-orange font-bold";
          rainIcon = "cloudy_snowing";
        } else {
          rainText = "Không mưa";
          rainClass = "text-air-quality-green font-bold";
          rainIcon = "sunny";
        }
      }

      extraContent = (
        <div className={`mt-2 text-xs flex items-center gap-1 ${rainClass}`}>
          <span className="material-symbols-outlined text-[14px]">{rainIcon}</span>
          {rainText}
        </div>
      );
      break;

    case 'battery':
      title = "Pin Trạm Khí Tượng";
      unit = "%";
      icon = (
        <div className="w-10 h-10 rounded-full bg-secondary-fixed-dim/10 flex items-center justify-center border border-secondary-fixed-dim/30">
          <span className="material-symbols-outlined text-secondary-fixed-dim">battery_full_alt</span>
        </div>
      );
      valueDisplay = (value !== null && value !== undefined) ? value.toFixed(0) : '--';
      
      let batteryFillClass = "bg-secondary-fixed-dim shadow-[0_0_10px_rgba(0,219,231,0.5)]";
      if (value !== null) {
        if (value < 20) {
          batteryFillClass = "bg-warning-red shadow-[0_0_10px_rgba(255,49,49,0.5)]";
        } else if (value < 50) {
          batteryFillClass = "bg-temp-orange shadow-[0_0_10px_rgba(255,140,0,0.5)]";
        }
      }

      extraContent = (
        <div className="mt-4 h-1.5 w-full bg-surface-container rounded-full overflow-hidden p-[1px] border border-glass-border">
          <div 
            className={`h-full rounded-full transition-all duration-500 ${batteryFillClass}`}
            style={{ width: `${value !== null ? Math.min(Math.max(value, 0), 100) : 0}%` }}
          ></div>
        </div>
      );
      break;

    default:
      break;
  }

  return (
    <div className="glass-card p-5 flex flex-col justify-between h-full transition-all duration-300 hover:-translate-y-1">
      <div className="flex items-start justify-between mb-4">
        {icon}
      </div>
      <div>
        <p className="font-label-caps text-label-caps text-on-surface-variant mb-1">{title}</p>
        <div className="flex items-baseline gap-1">
          <span className="font-headline-md text-2xl md:text-3xl font-bold text-on-surface">{valueDisplay}</span>
          <span className="font-data-mono text-xs text-on-surface-variant">{unit}</span>
        </div>
        {extraContent}
      </div>
    </div>
  );
}
