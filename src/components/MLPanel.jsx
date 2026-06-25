import React from 'react';

export default function MLPanel() {
  return (
    <div className="glass-card p-5 col-span-1 md:col-span-2">
      <div className="flex items-center gap-2 mb-4">
        <span className="material-symbols-outlined text-primary text-xl">smart_toy</span>
        <h3 className="font-headline-sm text-lg font-bold text-on-surface">Mô Hình Học Máy Python AI</h3>
      </div>
      <p className="text-air-quality-green font-label-caps text-xs font-bold tracking-wider mb-4 uppercase">Random Forest</p>
      
      <div className="grid grid-cols-2 gap-4 mb-4">
        <div className="bg-surface-container-low p-3 rounded-lg border border-glass-border text-center">
          <p className="font-data-mono text-xs text-on-surface-variant mb-1">Sai số Nhiệt độ (MAE)</p>
          <p className="font-data-mono text-base md:text-lg text-temp-orange font-bold">±0.45 °C</p>
        </div>
        <div className="bg-surface-container-low p-3 rounded-lg border border-glass-border text-center">
          <p className="font-data-mono text-xs text-on-surface-variant mb-1">Độ chính xác AI (Acc)</p>
          <p className="font-data-mono text-base md:text-lg text-air-quality-green font-bold">95.2%</p>
        </div>
      </div>

      <div className="space-y-2 text-sm text-on-surface-variant font-body-md leading-relaxed">
        <p>
          <strong className="text-on-surface">🤖 Mô hình dự báo:</strong> Sử dụng thuật toán Random Forest hồi quy (cho Temp/Hum) và phân loại (cho Weather Code) để dự đoán chu kỳ tiếp theo.
        </p>
        <p>
          <strong className="text-on-surface">📍 Cập nhật:</strong> Chạy tự động mỗi 30 phút qua GitHub Actions và đẩy dữ liệu lên ThingSpeak Cloud.
        </p>
      </div>
    </div>
  );
}
