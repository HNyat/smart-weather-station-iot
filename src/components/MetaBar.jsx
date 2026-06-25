import React from 'react';

export default function MetaBar({ rssi, seqNum, lastUpdated, dataSource, channelId, espIp }) {
  return (
    <div className="system-meta-bar">
      <div className="meta-item">
        <span className="meta-label">LoRa RSSI:</span>
        <span className="meta-value">{rssi !== null ? `${rssi} dBm` : '-- dBm'}</span>
      </div>
      <div className="meta-item border-left">
        <span className="meta-label">Mã Gói (Seq):</span>
        <span className="meta-value">{seqNum !== null ? seqNum : '--'}</span>
      </div>
      <div className="meta-item border-left">
        <span className="meta-label">Cập nhật lần cuối:</span>
        <span className="meta-value">{lastUpdated || '--'}</span>
      </div>
      <div className="meta-item border-left flex-grow">
        <span className="meta-label">Nguồn cấp:</span>
        <span className="meta-value">
          {dataSource === 'cloud' 
            ? `ThingSpeak Channel: ${channelId}` 
            : `ESP8266 Local Server: http://${espIp}`
          }
        </span>
      </div>
    </div>
  );
}
