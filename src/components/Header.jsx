import React, { useState, useEffect } from 'react';

export default function Header({ dataSource, setDataSource, toggleModal, isOnline, connectionText }) {
  const [timeStr, setTimeStr] = useState('');

  // Update clock every second
  useEffect(() => {
    const updateTime = () => {
      const now = new Date();
      setTimeStr(now.toLocaleTimeString('vi-VN', { hour12: false }));
    };
    updateTime();
    const intervalId = setInterval(updateTime, 1000);
    return () => clearInterval(intervalId);
  }, []);

  return (
    <nav className="fixed top-0 left-0 w-full z-50 flex justify-between items-center px-6 md:px-8 py-4 bg-background/85 backdrop-blur-xl border-b border-glass-border shadow-none">
      {/* Left logo & Status */}
      <div className="flex items-center gap-4">
        <span className="font-headline-md text-xl md:text-2xl font-bold tracking-tighter text-air-quality-green uppercase">
          WEATHER <span className="text-temp-orange">STATION</span>
        </span>
        <div className="hidden md:flex items-center gap-2 px-3 py-1 bg-surface-container rounded-full border border-glass-border">
          <span className={`w-2 h-2 rounded-full ${isOnline ? 'bg-air-quality-green animate-pulse neon-glow-green' : 'bg-warning-red'}`}></span>
          <span className="font-label-caps text-[10px] text-on-surface-variant uppercase">
            {connectionText}
          </span>
        </div>
      </div>

      {/* Center Data Source Switcher */}
      <div className="flex bg-surface-container-lowest/50 border border-glass-border rounded-full p-1 gap-1">
        <button
          className={`px-3 py-1 text-[11px] font-semibold rounded-full transition-all duration-200 ${
            dataSource === 'cloud'
              ? 'bg-glass-fill text-on-surface border border-glass-border shadow-sm font-bold'
              : 'text-on-surface-variant hover:text-on-surface'
          }`}
          onClick={() => setDataSource('cloud')}
        >
          Cloud (ThingSpeak)
        </button>
        <button
          className={`px-3 py-1 text-[11px] font-semibold rounded-full transition-all duration-200 ${
            dataSource === 'local'
              ? 'bg-glass-fill text-on-surface border border-glass-border shadow-sm font-bold'
              : 'text-on-surface-variant hover:text-on-surface'
          }`}
          onClick={() => setDataSource('local')}
        >
          Local (ESP8266)
        </button>
      </div>

      {/* Right actions & Clock */}
      <div className="flex items-center gap-4">
        <span className="font-data-mono text-sm text-on-surface-variant hidden lg:block">
          {timeStr}
        </span>
        <button 
          className="p-2 rounded-full hover:bg-glass-fill transition-all active:scale-95 duration-200 relative"
          title="Thông báo"
        >
          <span className="material-symbols-outlined text-primary text-xl">notifications</span>
          {isOnline && <span className="absolute top-1.5 right-1.5 w-2 h-2 bg-temp-orange rounded-full"></span>}
        </button>
        <button 
          className="p-2 rounded-full hover:bg-glass-fill transition-all active:scale-95 duration-200"
          onClick={() => toggleModal(true)}
          title="Cấu hình hệ thống"
        >
          <span className="material-symbols-outlined text-primary text-xl">settings</span>
        </button>
      </div>
    </nav>
  );
}
