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
    <div className="fixed inset-0 z-[100] flex items-center justify-center bg-background/80 backdrop-blur-sm p-4">
      <div className="glass-card p-6 w-full max-w-lg border border-glass-border flex flex-col gap-6 shadow-2xl relative animate-in fade-in zoom-in-95 duration-200">
        
        {/* Header */}
        <div className="flex justify-between items-center pb-4 border-b border-glass-border">
          <h2 className="font-headline-sm text-lg font-bold text-on-surface">Cấu Hình Giao Diện Hệ Thống</h2>
          <button 
            className="text-on-surface-variant hover:text-on-surface text-2xl font-bold p-1 leading-none transition-colors"
            onClick={() => toggleModal(false)}
          >
            &times;
          </button>
        </div>

        {/* Form Body */}
        <form onSubmit={handleSave} className="flex flex-col gap-4">
          <div className="flex flex-col gap-4 max-h-[60vh] overflow-y-auto pr-1">
            <div className="flex flex-col gap-1">
              <label htmlFor="inputChannelId" className="text-xs font-bold font-label-caps uppercase text-on-surface-variant">
                ThingSpeak Channel ID
              </label>
              <input 
                type="text" 
                id="inputChannelId" 
                value={channelId}
                onChange={(e) => setChannelId(e.target.value)}
                placeholder="Ví dụ: 12397" 
                required
                className="w-full bg-surface-container/50 border border-glass-border rounded-lg px-4 py-2 text-sm text-on-surface placeholder:text-on-surface-variant focus:outline-none focus:border-primary/50 transition-colors mt-1"
              />
              <small className="text-[10px] text-on-surface-variant">ID của kênh ThingSpeak lưu dữ liệu trạm thời tiết của bạn.</small>
            </div>
            
            <div className="flex flex-col gap-1">
              <label htmlFor="inputReadApiKey" className="text-xs font-bold font-label-caps uppercase text-on-surface-variant">
                ThingSpeak Read API Key
              </label>
              <input 
                type="password" 
                id="inputReadApiKey" 
                value={readApiKey}
                onChange={(e) => setReadApiKey(e.target.value)}
                placeholder="Nhập Read API Key (nếu Private)"
                className="w-full bg-surface-container/50 border border-glass-border rounded-lg px-4 py-2 text-sm text-on-surface placeholder:text-on-surface-variant focus:outline-none focus:border-primary/50 transition-colors mt-1"
              />
              <small className="text-[10px] text-on-surface-variant">Cần thiết để tải dữ liệu lịch sử và chạy phân tích.</small>
            </div>

            <div className="flex flex-col gap-1">
              <label htmlFor="inputMatlabId" className="text-xs font-bold font-label-caps uppercase text-on-surface-variant">
                MATLAB Visualization ID (Không bắt buộc)
              </label>
              <input 
                type="text" 
                id="inputMatlabId" 
                value={matlabId}
                onChange={(e) => setMatlabId(e.target.value)}
                placeholder="Nhập ID App Visualizations (ví dụ: 45678)"
                className="w-full bg-surface-container/50 border border-glass-border rounded-lg px-4 py-2 text-sm text-on-surface placeholder:text-on-surface-variant focus:outline-none focus:border-primary/50 transition-colors mt-1"
              />
              <small className="text-[10px] text-on-surface-variant">Để tương thích cấu hình cũ (hiện đã được thay thế bằng Python AI panel).</small>
            </div>

            <div className="flex flex-col gap-1">
              <label htmlFor="inputEspIp" className="text-xs font-bold font-label-caps uppercase text-on-surface-variant">
                ESP8266 Local Gateway IP
              </label>
              <input 
                type="text" 
                id="inputEspIp" 
                value={espIp}
                onChange={(e) => setEspIp(e.target.value)}
                placeholder="Mặc định: 192.168.4.1"
                className="w-full bg-surface-container/50 border border-glass-border rounded-lg px-4 py-2 text-sm text-on-surface placeholder:text-on-surface-variant focus:outline-none focus:border-primary/50 transition-colors mt-1"
              />
              <small className="text-[10px] text-on-surface-variant">Địa chỉ IP của ESP8266 khi kết nối trực tiếp vào WiFi AP "WeatherStation".</small>
            </div>
          </div>
          
          {/* Actions */}
          <div className="flex justify-end gap-3 pt-4 border-t border-glass-border">
            <button 
              type="button" 
              className="px-4 py-2 bg-surface-container hover:bg-surface-variant rounded-lg border border-glass-border text-xs font-label-caps uppercase text-on-surface transition-colors font-semibold" 
              onClick={handleReset}
            >
              Khôi phục Demo
            </button>
            <button 
              type="submit" 
              className="px-4 py-2 bg-temp-orange/20 border border-temp-orange/30 hover:bg-temp-orange/35 rounded-lg text-xs font-label-caps uppercase text-temp-orange transition-all font-bold"
            >
              Lưu Cấu Hình
            </button>
          </div>
        </form>

      </div>
    </div>
  );
}
