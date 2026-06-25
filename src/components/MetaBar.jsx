import React from 'react';

export default function MetaBar({ rssi, seqNum, lastUpdated, dataSource, channelId, espIp }) {
  return (
    <div className="flex flex-wrap items-center justify-between gap-4 mb-6 glass-card px-4 py-2 text-xs font-data-mono text-on-surface">
      <div className="flex flex-wrap gap-4 md:gap-6">
        <span>
          LoRa RSSI: <span className="text-on-surface-variant">{rssi !== null ? `${rssi} dBm` : '-- dBm'}</span>
        </span>
        <span className="hidden md:inline text-glass-border">|</span>
        <span>
          Mã Gói (Seq): <span className="text-on-surface-variant">{seqNum !== null ? seqNum : '--'}</span>
        </span>
        <span className="hidden md:inline text-glass-border">|</span>
        <span>
          Cập nhật lần cuối: <span className="text-primary">{lastUpdated || '--'}</span>
        </span>
      </div>
      <div className="flex gap-4">
        <span>
          Nguồn cấp:{' '}
          <span className="text-secondary-fixed-dim">
            {dataSource === 'cloud' 
              ? `ThingSpeak Channel: ${channelId}` 
              : `ESP8266 Local Server: http://${espIp}`
            }
          </span>
        </span>
      </div>
    </div>
  );
}
