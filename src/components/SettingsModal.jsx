import React, { useState, useEffect } from 'react';

export default function SettingsModal({ isOpen, toggleModal, config, saveSettings, resetToDemoSettings }) {
  const [channelId, setChannelId] = useState(config.channelId || '');
  const [readApiKey, setReadApiKey] = useState(config.readApiKey || '');
  const [matlabId, setMatlabId] = useState(config.matlabId || '');
  const [espIp, setEspIp] = useState(config.espIp || '192.168.4.1');

  // Sync inputs with config when modal opens
  useEffect(() => {
    if (isOpen) {
      setChannelId(config.channelId || '');
      setReadApiKey(config.readApiKey || '');
      setMatlabId(config.matlabId || '');
      setEspIp(config.espIp || '192.168.4.1');
    }
  }, [isOpen, config]);

  if (!isOpen) return null;

  const handleSave = (e) => {
    e.preventDefault();
    saveSettings({
      channelId: channelId.trim(),
      readApiKey: readApiKey.trim(),
      matlabId: matlabId.trim(),
      espIp: espIp.trim()
    });
  };

  const handleReset = () => {
    const demo = resetToDemoSettings();
    setChannelId(demo.channelId);
    setReadApiKey(demo.readApiKey);
    setMatlabId(demo.matlabId);
    setEspIp(demo.espIp);
  };

  return (
    <div className={`modal-overlay ${isOpen ? 'active' : ''}`} id="settingsModal">
      <div className="modal-card">
        <div className="modal-header">
          <h2>Cấu Hình Giao Diện Hệ Thống</h2>
          <button className="close-btn" onClick={() => toggleModal(false)}>&times;</button>
        </div>
        <form onSubmit={handleSave}>
          <div className="modal-body">
            <div className="form-group">
              <label htmlFor="inputChannelId">ThingSpeak Channel ID</label>
              <input 
                type="text" 
                id="inputChannelId" 
                value={channelId}
                onChange={(e) => setChannelId(e.target.value)}
                placeholder="Ví dụ: 12397" 
                required
              />
              <small>ID của kênh ThingSpeak lưu dữ liệu trạm thời tiết của bạn.</small>
            </div>
            
            <div className="form-group">
              <label htmlFor="inputReadApiKey">ThingSpeak Read API Key</label>
              <input 
                type="password" 
                id="inputReadApiKey" 
                value={readApiKey}
                onChange={(e) => setReadApiKey(e.target.value)}
                placeholder="Nhập Read API Key (nếu Private)"
              />
              <small>Cần thiết để tải dữ liệu lịch sử và chạy phân tích.</small>
            </div>

            <div className="form-group">
              <label htmlFor="inputMatlabId">MATLAB Visualization ID (Không bắt buộc)</label>
              <input 
                type="text" 
                id="inputMatlabId" 
                value={matlabId}
                onChange={(e) => setMatlabId(e.target.value)}
                placeholder="Nhập ID App Visualizations (ví dụ: 45678)"
              />
              <small>Để tương thích cấu hình cũ (hiện đã được thay thế bằng Python AI panel).</small>
            </div>

            <div className="form-group">
              <label htmlFor="inputEspIp">ESP8266 Local Gateway IP</label>
              <input 
                type="text" 
                id="inputEspIp" 
                value={espIp}
                onChange={(e) => setEspIp(e.target.value)}
                placeholder="Mặc định: 192.168.4.1"
              />
              <small>Địa chỉ IP của ESP8266 khi kết nối trực tiếp vào WiFi AP "WeatherStation".</small>
            </div>
            
            <div className="modal-actions">
              <button type="button" className="btn btn-secondary" onClick={handleReset}>Khôi phục Demo</button>
              <button type="submit" className="btn btn-primary">Lưu Cấu Hình</button>
            </div>
          </div>
        </form>
      </div>
    </div>
  );
}
