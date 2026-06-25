import React from 'react';

export default function Header({ dataSource, setDataSource, toggleModal, isOnline, connectionText }) {
  return (
    <header className="main-header">
      <div className="header-logo">
        <div className="logo-icon">
          <svg viewBox="0 0 24 24" width="26" height="26" fill="none" stroke="currentColor" strokeWidth="2.5">
            <path d="M12 2v2M12 20v2M4.93 4.93l1.41 1.41M17.66 17.66l1.41 1.41M2 12h2M20 12h2M6.34 17.66l-1.41 1.41M19.07 4.93l-1.41 1.41" strokeLinecap="round"/>
            <circle cx="12" cy="12" r="4" fill="currentColor"/>
          </svg>
        </div>
        <div className="logo-text">
          <h1>WEATHER <span>STATION</span></h1>
          <p>Hệ thống giám sát cây trồng & Hỗ trợ canh tác thông minh</p>
        </div>
      </div>
      
      <div className="header-actions">
        <div className="connection-status">
          <span className={`status-dot ${isOnline ? 'online' : 'offline'}`}></span>
          <span className="status-text">{connectionText}</span>
        </div>
        <div className="mode-selector">
          <button 
            className={`mode-btn ${dataSource === 'cloud' ? 'active' : ''}`} 
            onClick={() => setDataSource('cloud')}
          >
            Cloud (ThingSpeak)
          </button>
          <button 
            className={`mode-btn ${dataSource === 'local' ? 'active' : ''}`} 
            onClick={() => setDataSource('local')}
          >
            Local (ESP8266)
          </button>
        </div>
        <button className="icon-btn" onClick={() => toggleModal(true)} title="Cấu hình hệ thống">
          <svg viewBox="0 0 24 24" width="18" height="18" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
            <circle cx="12" cy="12" r="3"></circle>
            <path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 1 1-2.83 2.83l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-4 0v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 1 1-2.83-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1 0-4h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 1 1 2.83-2.83l.06.06a1.65 1.65 0 0 0 1.82.33H9a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 4 0v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 1 1 2.83 2.83l-.06.06a1.65 1.65 0 0 0-.33 1.82V9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 0 4h-.09a1.65 1.65 0 0 0-1.51 1z"></path>
          </svg>
        </button>
      </div>
    </header>
  );
}
