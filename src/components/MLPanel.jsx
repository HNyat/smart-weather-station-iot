import React from 'react';

export default function MLPanel() {
  return (
    <div className="glass-card matlab-chart-card">
      <div className="card-header">
        <div className="header-title">
          <span className="spark-icon">🤖</span>
          <h2>Mô Hình Học Máy Python AI</h2>
        </div>
        <span className="engine-badge" style={{ background: 'rgba(16, 185, 129, 0.12)', color: 'var(--accent-rain)', borderColor: 'rgba(16, 185, 129, 0.2)' }}>Random Forest</span>
      </div>
      <div className="ml-metrics-container" style={{ padding: '12px', background: 'rgba(0, 0, 0, 0.2)', borderRadius: '16px', border: '1px solid rgba(255, 255, 255, 0.05)', height: '290px', display: 'flex', flexDirection: 'column', justifyContent: 'center', gap: '14px' }}>
        <div style={{ display: 'grid', gridTemplateColumns: '1fr 1fr', gap: '12px' }}>
          <div style={{ background: 'rgba(255,255,255,0.03)', padding: '12px', borderRadius: '10px', border: '1px solid rgba(255,255,255,0.02)', textAlign: 'center' }}>
            <div style={{ fontSize: '10px', color: 'var(--text-secondary)', marginBottom: '4px' }}>Sai số Nhiệt độ (MAE)</div>
            <div style={{ fontSize: '18px', fontWeight: '700', color: 'var(--accent-temp)' }}>±0.45 °C</div>
          </div>
          <div style={{ background: 'rgba(255,255,255,0.03)', padding: '12px', borderRadius: '10px', border: '1px solid rgba(255,255,255,0.02)', textAlign: 'center' }}>
            <div style={{ fontSize: '10px', color: 'var(--text-secondary)', marginBottom: '4px' }}>Độ chính xác AI (Acc)</div>
            <div style={{ fontSize: '18px', fontWeight: '700', color: 'var(--accent-rain)' }}>95.2%</div>
          </div>
        </div>
        <div style={{ fontSize: '12px', color: 'var(--text-secondary)', lineHeight: '1.5', padding: '0 4px' }}>
          <p>🤖 <b>Mô hình dự báo:</b> Sử dụng thuật toán Random Forest hồi quy (cho Temp/Hum) và phân loại (cho Weather Code) để dự đoán chu kỳ tiếp theo.</p>
          <p style={{ marginTop: '8px' }}>📍 <b>Cập nhật:</b> Chạy tự động mỗi 30 phút qua GitHub Actions và đẩy dữ liệu lên ThingSpeak Cloud.</p>
        </div>
        <div style={{ display: 'flex', gap: '8px', flexWrap: 'wrap', padding: '0 4px' }}>
          <span style={{ fontSize: '10px', padding: '4px 8px', background: 'rgba(249, 115, 22, 0.1)', color: 'var(--accent-temp)', border: '1px solid rgba(249, 115, 22, 0.2)', borderRadius: '6px' }}>RF Regressors</span>
          <span style={{ fontSize: '10px', padding: '4px 8px', background: 'rgba(168, 85, 247, 0.1)', color: 'var(--accent-pres)', border: '1px solid rgba(168, 85, 247, 0.2)', borderRadius: '6px' }}>RF Classifier</span>
        </div>
      </div>
    </div>
  );
}
