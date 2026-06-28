#ifndef WEATHER_HTML_H
#define WEATHER_HTML_H

const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Weather Station Pro - IoT Dashboard</title>
  <!-- Google Fonts -->
  
  
  
  <!-- Chart.js CDN -->
  

  <style>
    /* ==========================================================================
       WEATHER STATION PRO - CSS STYLES (BALANCED LAYOUT)
       ========================================================================== */
    :root {
      /* Deep Space Weather Palette */
      --bg-primary: #131314;
      --bg-secondary: #0e0e0f;
      --card-bg: rgba(255, 255, 255, 0.03);
      --card-border: rgba(255, 255, 255, 0.12);
      
      --text-primary: #e5e2e3;
      --text-secondary: #c7c6cd;
      --text-muted: #909097;
      
      --accent-temp: #FF8C00;
      --accent-temp-gradient: linear-gradient(135deg, #FF8C00 0%, #d97706 100%);
      --accent-hum: #00F2FF;
      --accent-hum-gradient: linear-gradient(135deg, #00F2FF 0%, #008fa3 100%);
      --accent-pres: #00FF94;
      --accent-pres-gradient: linear-gradient(135deg, #00FF94 0%, #00b368 100%);
      --accent-rain: #00F2FF;
      --accent-rain-gradient: linear-gradient(135deg, #00F2FF 0%, #008fa3 100%);
      --accent-bat: #00FF94;
      --accent-bat-gradient: linear-gradient(135deg, #00FF94 0%, #00b368 100%);
      
      --success: #00FF94;
      --warning: #eab308;
      --error: #FF3131;
      
      /* Fonts */
      --font-display: 'Sora', -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;
      --font-body: 'Inter', -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;
    }

    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
    }

    body {
      font-family: var(--font-body);
      background-color: var(--bg-primary);
      color: var(--text-primary);
      overflow-x: hidden;
      min-height: 100vh;
      line-height: 1.5;
    }

    /* Background Glow Spots */
    .bg-glow {
      position: absolute;
      width: 450px;
      height: 450px;
      border-radius: 50%;
      filter: blur(140px);
      z-index: -1;
      opacity: 0.12;
      pointer-events: none;
    }

    .circle-1 {
      background: var(--accent-temp);
      top: 15%;
      left: 10%;
    }

    .circle-2 {
      background: var(--accent-hum);
      bottom: 15%;
      right: 10%;
    }

    .hidden {
      display: none !important;
    }

    /* Container */
    .app-container {
      max-width: 1400px;
      margin: 0 auto;
      padding: 24px;
      position: relative;
    }

    /* Header */
    .main-header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 16px 24px;
      background: rgba(15, 20, 34, 0.85);
      border: 1px solid var(--card-border);
      border-radius: 20px;
      backdrop-filter: blur(16px);
      margin-bottom: 16px;
      box-shadow: 0 4px 30px rgba(0, 0, 0, 0.4);
    }

    .header-logo {
      display: flex;
      align-items: center;
      gap: 16px;
    }

    .logo-icon {
      background: var(--accent-temp-gradient);
      color: var(--text-primary);
      padding: 8px;
      border-radius: 12px;
      display: flex;
      align-items: center;
      justify-content: center;
      box-shadow: 0 4px 15px rgba(249, 115, 22, 0.35);
    }

    .logo-text h1 {
      font-family: var(--font-display);
      font-size: 19px;
      font-weight: 800;
      letter-spacing: 0.5px;
    }

    .logo-text h1 span {
      color: var(--accent-temp);
    }

    .logo-text p {
      font-size: 11px;
      color: var(--text-secondary);
    }

    .header-actions {
      display: flex;
      align-items: center;
      gap: 16px;
    }

    .connection-status {
      display: flex;
      align-items: center;
      gap: 8px;
      background: rgba(0, 0, 0, 0.3);
      padding: 6px 14px;
      border-radius: 30px;
      border: 1px solid rgba(255, 255, 255, 0.05);
    }

    .status-dot {
      width: 8px;
      height: 8px;
      border-radius: 50%;
      background: var(--text-muted);
    }

    .status-dot.online {
      background: var(--success);
      box-shadow: 0 0 8px var(--success);
    }

    .status-dot.offline {
      background: var(--error);
      box-shadow: 0 0 8px var(--error);
    }

    .status-text {
      font-size: 11px;
      font-weight: 600;
    }

    .mode-selector {
      display: flex;
      background: rgba(0, 0, 0, 0.35);
      border-radius: 30px;
      padding: 3px;
      border: 1px solid rgba(255, 255, 255, 0.05);
    }

    .mode-btn {
      background: transparent;
      border: none;
      color: var(--text-secondary);
      padding: 5px 12px;
      font-size: 11px;
      font-weight: 600;
      border-radius: 20px;
      cursor: pointer;
      transition: all 0.2s ease;
    }

    .mode-btn.active {
      background: rgba(255, 255, 255, 0.08);
      color: var(--text-primary);
    }

    .icon-btn {
      background: rgba(255, 255, 255, 0.04);
      border: 1px solid var(--card-border);
      color: var(--text-primary);
      width: 36px;
      height: 36px;
      border-radius: 50%;
      display: flex;
      align-items: center;
      justify-content: center;
      cursor: pointer;
      transition: all 0.2s ease;
    }

    .icon-btn:hover {
      background: rgba(255, 255, 255, 0.09);
      transform: rotate(45deg);
    }

    /* System Meta Bar */
    .system-meta-bar {
      display: flex;
      flex-wrap: wrap;
      gap: 16px;
      padding: 10px 20px;
      background: rgba(15, 20, 34, 0.5);
      border: 1px solid var(--card-border);
      border-radius: 12px;
      margin-bottom: 20px;
    }

    .meta-item {
      display: flex;
      align-items: center;
      gap: 8px;
      font-size: 12px;
    }

    .meta-label {
      color: var(--text-secondary);
    }

    .meta-value {
      font-weight: 600;
      color: var(--text-primary);
    }

    .border-left {
      border-left: 1px solid rgba(255, 255, 255, 0.08);
      padding-left: 16px;
    }

    .flex-grow {
      margin-left: auto;
    }

    /* FULL WIDTH HERO BANNER */
    .hero-weather-banner {
      position: relative;
      background: linear-gradient(135deg, rgba(22, 29, 49, 0.9) 0%, rgba(10, 14, 26, 0.95) 100%);
      border: 1px solid var(--card-border);
      border-radius: 24px;
      padding: 24px 32px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      overflow: hidden;
      margin-bottom: 24px;
      box-shadow: 0 10px 30px rgba(0, 0, 0, 0.35);
    }

    .hero-banner-overlay {
      position: absolute;
      top: -50%;
      right: -10%;
      width: 400px;
      height: 400px;
      background: radial-gradient(circle, rgba(249, 115, 22, 0.12) 0%, transparent 70%);
      z-index: 1;
      pointer-events: none;
    }

    .hero-left {
      display: flex;
      align-items: center;
      gap: 24px;
      z-index: 2;
    }

    .weather-visual {
      flex-shrink: 0;
    }

    .weather-icon-svg {
      filter: drop-shadow(0 6px 12px rgba(245, 158, 11, 0.35));
    }

    .sun-cloud-anim {
      animation: float 4s ease-in-out infinite;
    }

    @keyframes float {
      0%, 100% { transform: translateY(0); }
      50% { transform: translateY(-6px); }
    }

    .weather-info {
      display: flex;
      flex-direction: column;
      gap: 4px;
    }

    .weather-status-tag {
      background: rgba(249, 115, 22, 0.12);
      color: var(--accent-temp);
      padding: 4px 12px;
      border-radius: 20px;
      font-size: 10px;
      font-weight: 700;
      text-transform: uppercase;
      letter-spacing: 0.5px;
      border: 1px solid rgba(249, 115, 22, 0.18);
      width: max-content;
    }

    .weather-summary-text {
      font-size: 15px;
      color: var(--text-secondary);
      font-weight: 500;
    }

    .hero-right {
      display: flex;
      align-items: center;
      gap: 32px;
      z-index: 2;
    }

    .main-temp-display {
      display: flex;
      align-items: flex-start;
    }

    .temp-number {
      font-family: var(--font-display);
      font-size: 64px;
      font-weight: 800;
      line-height: 0.9;
      color: var(--text-primary);
    }

    .temp-unit {
      font-family: var(--font-display);
      font-size: 24px;
      font-weight: 700;
      color: var(--accent-temp);
      margin-left: 2px;
    }

    .hero-badges {
      display: flex;
      flex-direction: column;
      gap: 8px;
    }

    .hero-badge {
      display: flex;
      align-items: center;
      gap: 8px;
      background: rgba(255, 255, 255, 0.03);
      border: 1px solid rgba(255, 255, 255, 0.08);
      padding: 6px 12px;
      border-radius: 8px;
      font-size: 11px;
      font-weight: 600;
    }

    .hero-badge.badge-temp {
      border-color: rgba(255, 140, 0, 0.2);
      background: rgba(255, 140, 0, 0.04);
      color: #ffa133;
    }
    
    .hero-badge.badge-rain {
      border-color: rgba(0, 242, 255, 0.2);
      background: rgba(0, 242, 255, 0.04);
      color: #33f5ff;
    }
    
    .hero-badge.badge-status {
      border-color: rgba(0, 255, 148, 0.2);
      background: rgba(0, 255, 148, 0.04);
      color: #33ffaa;
    }

    .badge-icon {
      font-size: 12px;
    }

    .badge-label {
      color: var(--text-secondary);
    }

    .badge-value {
      color: var(--text-primary);
      font-weight: 700;
    }

    /* AI MULTI-HORIZON FORECAST SECTION */
    .ai-forecast-section {
      background: var(--card-bg);
      border: 1px solid var(--card-border);
      border-radius: 24px;
      padding: 24px;
      margin-bottom: 24px;
      box-shadow: 0 8px 32px rgba(0, 0, 0, 0.25);
    }
    
    .ai-forecast-header {
      display: flex;
      align-items: center;
      gap: 8px;
      margin-bottom: 16px;
    }
    
    .ai-forecast-header h2 {
      font-family: var(--font-display);
      font-size: 16px;
      font-weight: 700;
      color: var(--text-primary);
    }
    
    .ai-forecast-cards {
      display: grid;
      grid-template-columns: repeat(4, 1fr);
      gap: 16px;
    }
    
    @media (max-width: 768px) {
      .ai-forecast-cards {
        grid-template-columns: repeat(2, 1fr);
      }
    }
    
    @media (max-width: 480px) {
      .ai-forecast-cards {
        grid-template-columns: 1fr;
      }
    }
    
    .ai-forecast-card {
      background: rgba(255, 140, 0, 0.04);
      border: 1px solid rgba(255, 140, 0, 0.1);
      border-radius: 16px;
      padding: 16px;
      text-align: center;
      display: flex;
      flex-direction: column;
      align-items: center;
      gap: 8px;
      transition: transform 0.2s, box-shadow 0.2s;
    }
    
    .ai-forecast-card:hover {
      transform: translateY(-2px);
      box-shadow: 0 4px 20px rgba(255, 140, 0, 0.08);
      border-color: rgba(255, 140, 0, 0.2);
    }
    
    .ai-card-title {
      font-family: var(--font-display);
      font-size: 11px;
      font-weight: 700;
      letter-spacing: 0.5px;
      color: var(--text-secondary);
      text-transform: uppercase;
    }
    
    .ai-card-icon {
      font-size: 32px;
      line-height: 1;
      margin: 4px 0;
    }
    
    .ai-card-temp {
      font-family: var(--font-display);
      font-size: 20px;
      font-weight: 700;
      color: var(--text-primary);
      font-variant-numeric: tabular-nums;
    }
    
    .ai-card-rain {
      font-size: 11px;
      color: var(--accent-hum);
      font-weight: 600;
      font-variant-numeric: tabular-nums;
    }
    
    /* Confidence Decay Opacities */
    .ai-card-1h { opacity: 1.0; }
    .ai-card-3h { opacity: 0.85; }
    .ai-card-6h { opacity: 0.75; }
    .ai-card-12h { opacity: 0.6; }
    
    .ai-forecast-footer {
      text-align: center;
      margin-top: 12px;
      font-size: 11px;
      color: var(--text-muted);
      font-style: italic;
    }

    /* Balanced 2-Column Grid Layout */

    .balanced-grid {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 24px;
    }

    @media (max-width: 1024px) {
      .balanced-grid {
        grid-template-columns: 1fr;
      }
    }

    /* Glass Card */
    .glass-card {
      background: var(--card-bg);
      border: 1px solid var(--card-border);
      border-radius: 24px;
      backdrop-filter: blur(20px);
      -webkit-backdrop-filter: blur(20px);
      padding: 24px;
      box-shadow: 0 8px 32px rgba(0, 0, 0, 0.25);
      transition: transform 0.3s cubic-bezier(0.175, 0.885, 0.32, 1.275), border-color 0.2s, box-shadow 0.3s;
    }

    .glass-card:hover {
      transform: translateY(-4px);
      border-color: rgba(255, 255, 255, 0.12);
      box-shadow: 0 12px 40px rgba(0, 0, 0, 0.35);
    }

    /* Columns */
    .grid-column {
      display: flex;
      flex-direction: column;
      gap: 24px;
      height: 100%;
    }

    /* Left Column Elements */
    /* Params Grid (2x2 Balanced) */
    .params-grid {
      display: grid;
      grid-template-columns: repeat(2, 1fr);
      gap: 16px;
    }

    @media (max-width: 480px) {
      .params-grid {
        grid-template-columns: 1fr;
      }
    }

    .param-card {
      display: flex;
      flex-direction: column;
      gap: 12px;
      height: 100%;
    }

    .card-icon {
      width: 42px;
      height: 42px;
      border-radius: 12px;
      display: flex;
      align-items: center;
      justify-content: center;
    }

    .hum-theme .card-icon { background: rgba(56, 189, 248, 0.08); color: var(--accent-hum); }
    .pres-theme .card-icon { background: rgba(168, 85, 247, 0.08); color: var(--accent-pres); }
    .rain-theme .card-icon { background: rgba(16, 185, 129, 0.08); color: var(--accent-rain); }
    .bat-theme .card-icon { background: rgba(20, 184, 166, 0.08); color: var(--accent-bat); }

    .card-info h3 {
      font-family: var(--font-body);
      font-size: 12px;
      font-weight: 700;
      line-height: 16px;
      letter-spacing: 0.08em;
      text-transform: uppercase;
      color: var(--text-muted);
      margin-bottom: 4px;
    }

    .card-value {
      font-family: var(--font-display);
      font-size: 28px;
      font-weight: 600;
      line-height: 36px;
    }

    .card-value .unit {
      font-size: 14px;
      font-weight: 600;
      color: var(--text-secondary);
      margin-left: 2px;
    }

    .card-subtext {
      font-size: 10px;
      color: var(--text-secondary);
      font-weight: 600;
      background: rgba(255, 255, 255, 0.04);
      padding: 3px 8px;
      border-radius: 6px;
      border: 1px solid rgba(255, 255, 255, 0.03);
      display: inline-block;
      width: max-content;
    }

    .progress-bar-container {
      height: 5px;
      background: rgba(255, 255, 255, 0.05);
      border-radius: 10px;
      overflow: hidden;
      margin-top: 4px;
    }

    .progress-bar {
      height: 100%;
      background: var(--accent-hum-gradient);
      border-radius: 10px;
      transition: width 0.8s ease;
    }

    .pressure-badge {
      display: inline-block;
      padding: 2px 6px;
      border-radius: 4px;
      font-size: 9px;
      font-weight: 700;
      background: rgba(168, 85, 247, 0.12);
      color: var(--accent-pres);
      border: 1px solid rgba(168, 85, 247, 0.18);
    }

    .rain-status-text {
      font-size: 11px;
      font-weight: 700;
      color: var(--accent-rain);
    }

    .battery-battery-bar {
      height: 7px;
      background: rgba(255, 255, 255, 0.05);
      border-radius: 4px;
      padding: 1px;
      border: 1px solid rgba(255, 255, 255, 0.1);
      margin-top: 4px;
    }

    .battery-fill {
      height: 100%;
      background: var(--accent-bat-gradient);
      border-radius: 2px;
      transition: width 0.8s ease;
    }

    /* Forecast Card */
    .forecast-card {
      display: flex;
      flex-direction: column;
      flex: 1;
    }

    .forecast-card .card-header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      margin-bottom: 16px;
      flex-wrap: wrap;
      gap: 12px;
    }

    .forecast-card h2 {
      font-family: var(--font-display);
      font-size: 16px;
      font-weight: 700;
    }

    .forecast-grid {
      display: grid;
      grid-template-columns: 1.15fr 1fr;
      gap: 16px;
      margin-bottom: 12px;
    }

    @media (max-width: 600px) {
      .forecast-grid {
        grid-template-columns: 1fr;
      }
    }

    .prediction-box {
      display: flex;
      flex-direction: column;
      gap: 8px;
    }

    .pred-item {
      display: flex;
      justify-content: space-between;
      align-items: center;
      background: rgba(0, 0, 0, 0.2);
      padding: 8px 12px;
      border-radius: 10px;
      border: 1px solid rgba(255, 255, 255, 0.03);
    }

    .pred-label {
      font-size: 11px;
      color: var(--text-secondary);
    }

    .pred-val {
      font-family: var(--font-display);
      font-size: 14px;
      font-weight: 700;
    }

    .pred-trend {
      font-size: 10px;
      font-weight: 700;
      padding: 1px 6px;
      border-radius: 4px;
    }

    .trend-up { background: rgba(239, 68, 68, 0.1); color: var(--error); }
    .trend-down { background: rgba(56, 189, 248, 0.1); color: var(--accent-hum); }
    .trend-stable { background: rgba(16, 185, 129, 0.1); color: var(--accent-rain); }

    .regression-math-box {
      background: rgba(0, 0, 0, 0.25);
      border: 1px solid rgba(255, 255, 255, 0.04);
      border-radius: 14px;
      padding: 12px;
      display: flex;
      flex-direction: column;
      justify-content: center;
      gap: 4px;
    }

    .regression-math-box h3 {
      font-size: 10px;
      color: var(--text-muted);
      text-transform: uppercase;
      margin-bottom: 4px;
      letter-spacing: 0.5px;
    }

    .math-formula {
      font-family: 'Courier New', Courier, monospace;
      font-size: 11px;
      color: var(--warning);
      line-height: 1.5;
    }

    .card-divider {
      border: none;
      border-top: 1px solid rgba(255, 255, 255, 0.05);
      margin: 12px 0;
    }

    .recommendation-box {
      display: flex;
      gap: 12px;
      background: rgba(234, 179, 8, 0.05);
      border: 1px solid rgba(234, 179, 8, 0.12);
      padding: 12px;
      border-radius: 14px;
    }

    .rec-icon {
      font-size: 18px;
      flex-shrink: 0;
    }

    .rec-content h4 {
      font-size: 12px;
      font-weight: 700;
      color: var(--warning);
      margin-bottom: 2px;
    }

    .rec-content p {
      font-size: 11px;
      color: var(--text-secondary);
      line-height: 1.5;
    }

    /* Right Column Elements */
    .chart-container-card {
      display: flex;
      flex-direction: column;
      gap: 16px;
    }

    .chart-header h2 {
      font-family: var(--font-display);
      font-size: 18px;
      font-weight: 700;
    }

    .chart-tabs-wrapper {
      overflow-x: auto;
      max-width: 100%;
      padding-bottom: 4px;
      margin-top: 8px;
    }

    .chart-tabs {
      display: inline-flex;
      background: rgba(0, 0, 0, 0.25);
      border-radius: 30px;
      padding: 3px;
      gap: 2px;
      width: max-content;
    }

    .chart-tab {
      border: none;
      background: transparent;
      color: var(--text-secondary);
      padding: 5px 12px;
      font-size: 11px;
      font-weight: 600;
      border-radius: 20px;
      cursor: pointer;
      transition: all 0.2s ease;
      white-space: nowrap;
    }

    .chart-tab.tab-temp.active {
      background: var(--accent-temp-gradient);
      color: var(--text-primary);
      box-shadow: 0 3px 8px rgba(255, 140, 0, 0.3);
    }
    
    .chart-tab.tab-hum.active {
      background: var(--accent-hum-gradient);
      color: var(--text-primary);
      box-shadow: 0 3px 8px rgba(0, 242, 255, 0.3);
    }
    
    .chart-tab.tab-pres.active {
      background: var(--accent-pres-gradient);
      color: var(--text-primary);
      box-shadow: 0 3px 8px rgba(0, 255, 148, 0.3);
    }
    
    .chart-tab.tab-rain.active {
      background: var(--accent-rain-gradient);
      color: var(--text-primary);
      box-shadow: 0 3px 8px rgba(0, 242, 255, 0.3);
    }
    
    .chart-tab.tab-bat.active {
      background: var(--accent-bat-gradient);
      color: var(--text-primary);
      box-shadow: 0 3px 8px rgba(0, 255, 148, 0.3);
    }

    .chart-wrapper {
      position: relative;
      height: 260px;
      width: 100%;
    }

    /* MATLAB Visualizations Card */
    .matlab-chart-card {
      display: flex;
      flex-direction: column;
      gap: 12px;
      flex: 1;
    }

    .matlab-chart-card h2 {
      font-family: var(--font-display);
      font-size: 17px;
      font-weight: 700;
    }

    .matlab-iframe-container {
      position: relative;
      width: 100%;
      height: 290px;
      border-radius: 16px;
      overflow: hidden;
      background: rgba(0, 0, 0, 0.25);
      border: 1px solid rgba(255, 255, 255, 0.05);
    }

    .ml-tab {
      background: transparent;
      border: none;
      color: var(--text-secondary);
      font-size: 10px;
      font-weight: 600;
      padding: 4px 8px;
      border-radius: 8px;
      cursor: pointer;
      transition: all 0.2s ease;
    }
    .ml-tab.active {
      background: rgba(255, 255, 255, 0.12);
      color: var(--text-primary);
      box-shadow: 0 2px 6px rgba(0,0,0,0.2);
    }

    .matlab-iframe-container iframe {
      width: 100%;
      height: 100%;
      border: none;
    }

    .matlab-placeholder {
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      height: 100%;
      color: var(--text-secondary);
      text-align: center;
      padding: 24px;
    }

    .matlab-placeholder p {
      font-size: 12px;
      margin-bottom: 12px;
      color: var(--text-secondary);
    }

    /* Modals */
    .modal-overlay {
      position: fixed;
      top: 0;
      left: 0;
      width: 100%;
      height: 100%;
      background: rgba(8, 12, 20, 0.85);
      backdrop-filter: blur(8px);
      z-index: 100;
      display: flex;
      align-items: center;
      justify-content: center;
      opacity: 0;
      pointer-events: none;
      transition: opacity 0.3s ease;
    }

    .modal-overlay.active {
      opacity: 1;
      pointer-events: auto;
    }

    .modal-card {
      width: 90%;
      max-width: 460px;
      background: var(--bg-secondary);
      border: 1px solid var(--card-border);
      border-radius: 24px;
      box-shadow: 0 20px 50px rgba(0, 0, 0, 0.6);
      overflow: hidden;
      transform: translateY(20px);
      transition: transform 0.3s ease;
    }

    .modal-overlay.active .modal-card {
      transform: translateY(0);
    }

    .modal-header {
      padding: 16px 24px;
      border-bottom: 1px solid rgba(255, 255, 255, 0.05);
      display: flex;
      justify-content: space-between;
      align-items: center;
    }

    .modal-header h2 {
      font-family: var(--font-display);
      font-size: 17px;
      font-weight: 700;
    }

    .close-btn {
      background: transparent;
      border: none;
      color: var(--text-secondary);
      font-size: 24px;
      cursor: pointer;
      transition: color 0.2s;
    }

    .close-btn:hover {
      color: var(--text-primary);
    }

    .modal-body {
      padding: 20px 24px;
      display: flex;
      flex-direction: column;
      gap: 12px;
    }

    .form-group {
      display: flex;
      flex-direction: column;
      gap: 4px;
    }

    .form-group label {
      font-size: 12px;
      font-weight: 600;
      color: var(--text-secondary);
    }

    .form-group input {
      background: rgba(0, 0, 0, 0.25);
      border: 1px solid rgba(255, 255, 255, 0.08);
      padding: 8px 12px;
      border-radius: 8px;
      color: var(--text-primary);
      font-family: var(--font-body);
      font-size: 13px;
      transition: border-color 0.2s;
    }

    .form-group input:focus {
      outline: none;
      border-color: var(--accent-temp);
    }

    .form-group small {
      font-size: 10px;
      color: var(--text-muted);
    }

    .modal-actions {
      display: flex;
      gap: 12px;
      margin-top: 8px;
    }

    .btn {
      border: none;
      padding: 10px;
      border-radius: 8px;
      font-size: 13px;
      font-weight: 600;
      cursor: pointer;
      transition: background 0.2s;
    }

    .btn-primary {
      background: var(--accent-temp-gradient);
      color: var(--text-primary);
    }

    .btn-primary:hover {
      background: linear-gradient(135deg, #ea580c 0%, #d97706 100%);
    }

    .btn-secondary {
      background: rgba(255, 255, 255, 0.04);
      color: var(--text-primary);
      border: 1px solid rgba(255, 255, 255, 0.08);
    }

    .btn-secondary:hover {
      background: rgba(255, 255, 255, 0.08);
    }

    /* Alert Box */
    .alert-banner {
      position: fixed;
      bottom: 24px;
      left: 50%;
      transform: translateX(-50%);
      background: var(--error);
      color: white;
      padding: 14px 28px;
      border-radius: 16px;
      display: flex;
      align-items: center;
      gap: 12px;
      box-shadow: 0 8px 30px rgba(255, 49, 49, 0.4);
      font-weight: 700;
      font-size: 13px;
      z-index: 1000;
      animation: slide-up 0.4s cubic-bezier(0.175, 0.885, 0.32, 1.275);
      border: 1px solid rgba(255, 255, 255, 0.15);
    }

    .alert-banner.hidden {
      display: none;
    }

    /* Custom Scrollbar */
    ::-webkit-scrollbar {
      width: 8px;
      height: 8px;
    }
    ::-webkit-scrollbar-track {
      background: var(--bg-primary);
    }
    ::-webkit-scrollbar-thumb {
      background: rgba(255, 255, 255, 0.1);
      border-radius: 4px;
    }
    ::-webkit-scrollbar-thumb:hover {
      background: rgba(255, 255, 255, 0.2);
    }

    /* Hide scrollbar for tabs */
    .chart-tabs-wrapper::-webkit-scrollbar {
      display: none;
    }
    .chart-tabs-wrapper {
      -ms-overflow-style: none;
      scrollbar-width: none;
    }

    /* Hover glows for cards */
    .hum-theme:hover { border-color: rgba(56, 189, 248, 0.3) !important; box-shadow: 0 12px 35px rgba(56, 189, 248, 0.12) !important; }
    .pres-theme:hover { border-color: rgba(168, 85, 247, 0.3) !important; box-shadow: 0 12px 35px rgba(168, 85, 247, 0.12) !important; }
    .rain-theme:hover { border-color: rgba(16, 185, 129, 0.3) !important; box-shadow: 0 12px 35px rgba(16, 185, 129, 0.12) !important; }
    .bat-theme:hover { border-color: rgba(20, 184, 166, 0.3) !important; box-shadow: 0 12px 35px rgba(20, 184, 166, 0.12) !important; }
    .chart-container-card:hover { border-color: rgba(249, 115, 22, 0.3) !important; box-shadow: 0 12px 35px rgba(249, 115, 22, 0.12) !important; }
    .matlab-chart-card:hover { border-color: rgba(56, 189, 248, 0.3) !important; box-shadow: 0 12px 35px rgba(56, 189, 248, 0.12) !important; }
    .forecast-card:hover { border-color: rgba(234, 179, 8, 0.3) !important; box-shadow: 0 12px 35px rgba(234, 179, 8, 0.12) !important; }
    .hero-weather-banner:hover { border-color: rgba(249, 115, 22, 0.3); box-shadow: 0 12px 40px rgba(249, 115, 22, 0.12); }
  
    /* ==========================================================================
       OFFLINE MOBILE-RESPONSIVE OVERRIDES
       ========================================================================== */
    @media (max-width: 768px) {
      .app-container {
        padding: 12px;
      }
      .main-header {
        flex-direction: column;
        gap: 12px;
        align-items: stretch;
        padding: 12px 16px;
      }
      .header-actions {
        flex-direction: column;
        align-items: stretch;
        gap: 10px;
        width: 100%;
      }
      .connection-status {
        justify-content: center;
        width: 100%;
      }
      .mode-selector {
        width: 100%;
        display: flex;
      }
      .mode-btn {
        flex: 1;
        text-align: center;
      }
      .icon-btn {
        align-self: center;
      }
      .hero-weather-banner {
        flex-direction: column;
        align-items: stretch;
        padding: 16px 20px;
        gap: 20px;
      }
      .hero-left {
        gap: 16px;
        justify-content: flex-start;
      }
      .hero-right {
        flex-direction: column;
        align-items: flex-start;
        gap: 16px;
        border-top: 1px solid rgba(255, 255, 255, 0.08);
        padding-top: 16px;
      }
      .main-temp-display {
        align-self: flex-start;
      }
      .temp-number {
        font-size: 48px;
      }
      .hero-badges {
        width: 100%;
        flex-direction: row;
        flex-wrap: wrap;
        gap: 8px;
      }
      .hero-badge {
        flex: 1 1 calc(50% - 4px);
        min-width: 120px;
      }
      .system-meta-bar {
        padding: 10px 14px;
        gap: 8px;
        flex-direction: column;
        align-items: stretch;
      }
      .meta-item {
        font-size: 11px;
        flex: 1 1 100%;
        width: 100%;
        border-left: none !important;
        padding-left: 0 !important;
      }
      .border-left {
        border-left: none;
        padding-left: 0;
      }
      .flex-grow {
        margin-left: 0;
        width: 100%;
        text-align: left;
        border-top: 1px solid rgba(255, 255, 255, 0.05);
        padding-top: 6px;
        margin-top: 4px;
      }
      .params-grid {
        grid-template-columns: 1fr !important;
      }
      .forecast-grid {
        grid-template-columns: 1fr !important;
      }
      .glass-card {
        padding: 16px !important;
        border-radius: 16px !important;
        min-width: 0 !important;
      }
      .grid-column {
        min-width: 0 !important;
      }
      .chart-container-card {
        min-width: 0 !important;
      }
      .chart-header {
        width: 100% !important;
        min-width: 0 !important;
        overflow: hidden !important;
      }
    }

    @media (max-width: 480px) {
      .modal-actions {
        flex-direction: column;
        width: 100%;
      }
      .modal-actions button {
        width: 100%;
      }
      .hero-badges {
        flex-direction: column;
        align-items: stretch;
        width: 100%;
      }
      .hero-badge {
        flex: 1 1 100%;
        width: 100%;
      }
    }
  </style>


</head>
<body>
  <!-- GLOBAL SVG GRADIENTS -->
  <svg width="0" height="0" style="position: absolute; pointer-events: none;">
    <defs>
      <linearGradient id="sunGradient" x1="0%" y1="0%" x2="100%" y2="100%">
        <stop offset="0%" stop-color="#fbbf24" />
        <stop offset="100%" stop-color="#f59e0b" />
      </linearGradient>
      <linearGradient id="cloudGradient" x1="0%" y1="0%" x2="100%" y2="100%">
        <stop offset="0%" stop-color="#e2e8f0" />
        <stop offset="100%" stop-color="#94a3b8" />
      </linearGradient>
      <linearGradient id="cloudGrad" x1="0%" y1="0%" x2="100%" y2="100%">
        <stop offset="0%" stop-color="#94a3b8" />
        <stop offset="100%" stop-color="#475569" />
      </linearGradient>
    </defs>
  </svg>

  <div class="app-container">
    <!-- BACKGROUND DECORATIONS -->
    <div class="bg-glow circle-1"></div>
    <div class="bg-glow circle-2"></div>

    <!-- HEADER -->
    <header class="main-header">
      <div class="header-logo">
        <div class="logo-icon">
          <svg viewBox="0 0 24 24" width="26" height="26" fill="none" stroke="currentColor" stroke-width="2.5">
            <path d="M12 2v2M12 20v2M4.93 4.93l1.41 1.41M17.66 17.66l1.41 1.41M2 12h2M20 12h2M6.34 17.66l-1.41 1.41M19.07 4.93l-1.41 1.41" stroke-linecap="round"/>
            <circle cx="12" cy="12" r="4" fill="currentColor"/>
          </svg>
        </div>
        <div class="logo-text">
          <h1>WEATHER <span>STATION</span></h1>
          <p>Hệ thống giám sát cây trồng & Hỗ trợ canh tác thông minh</p>
        </div>
      </div>
      
      <div class="header-actions">
        <div class="connection-status">
          <span class="status-dot online" id="statusDot"></span>
          <!-- B1: Dual timestamp — cảm biến và AI là 2 nguồn cập nhật độc lập -->
          <span class="status-text" id="statusText">Trực tuyến · đang tải...</span>
        </div>
        <!-- B2: Local toggle đã xóa — luôn lỗi mixed-content trên HTTPS public -->
        <button class="icon-btn" id="btnSettings" onclick="toggleModal('settingsModal', true)" title="Cấu hình hệ thống">
          <svg viewBox="0 0 24 24" width="18" height="18" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
            <circle cx="12" cy="12" r="3"></circle>
            <path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 1 1-2.83 2.83l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-4 0v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 1 1-2.83-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1 0-4h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 1 1 2.83-2.83l.06.06a1.65 1.65 0 0 0 1.82.33H9a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 4 0v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 1 1 2.83 2.83l-.06.06a1.65 1.65 0 0 0-.33 1.82V9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 0 4h-.09a1.65 1.65 0 0 0-1.51 1z"></path>
          </svg>
        </button>
      </div>
    </header>

    <!-- SYSTEM META BAR — B1: Dual timestamp, sensor vs AI riêng biệt -->
    <div class="system-meta-bar">
      <div class="meta-item">
        <span class="meta-label">📡 Cảm biến:</span>
        <span class="meta-value" id="sensorAgoText" style="font-variant-numeric: tabular-nums;">--</span>
      </div>
      <div class="meta-item border-left">
        <span class="meta-label">🤖 AI cập nhật:</span>
        <span class="meta-value" id="aiAgoText" style="font-variant-numeric: tabular-nums;">--</span>
      </div>
      <div class="meta-item border-left">
        <span class="meta-label">LoRa RSSI:</span>
        <span class="meta-value" id="rssiValue">-- dBm</span>
      </div>
      <div class="meta-item border-left flex-grow">
        <span class="meta-label">Nguồn cấp:</span>
        <span class="meta-value" id="stationModeText">ThingSpeak Channel</span>
      </div>
    </div>

    <!-- FULL WIDTH HIGH-IMPACT HERO BANNER -->
    <section class="hero-weather-banner">
      <div class="hero-banner-overlay"></div>
      <div class="hero-left">
        <div class="weather-visual">
          <div id="weatherIconContainer">
            <svg viewBox="0 0 64 64" width="80" height="80" class="weather-icon-svg sun-cloud-anim">
              <circle cx="24" cy="24" r="12" fill="url(#sunGradient)"/>
              <path d="M46 38a10 10 0 0 0-9-6 10.5 10.5 0 0 0-4 .8A12 12 0 1 0 22 50h24a10 10 0 0 0 0-20z" fill="url(#cloudGradient)" opacity="0.95"/>
            </svg>
          </div>
        </div>
        <div class="weather-info">
          <span class="weather-status-tag" id="weatherStatusTag">Khí tượng canh tác</span>
          <div class="weather-summary-text" id="weatherSummary">Đang lấy dữ liệu cảm biến...</div>
        </div>
      </div>

      <div class="hero-right">
        <div class="main-temp-display">
          <span class="temp-number" id="mainTemp">--</span>
          <span class="temp-unit">°C</span>
        </div>
        <div class="hero-badges">
          <div class="hero-badge badge-temp">
            <span class="badge-icon">🤖</span>
            <span class="badge-label">Python AI dự báo:</span>
            <span class="badge-value" id="subTemp">-- °C</span>
          </div>
          <div class="hero-badge badge-rain" style="display: flex; align-items: center; gap: 10px;">
            <div style="position: relative; width: 36px; height: 36px;">
              <svg viewBox="0 0 36 36" width="36" height="36" style="transform: rotate(-90deg);">
                <circle cx="18" cy="18" r="16" fill="none" stroke="rgba(255,255,255,0.1)" stroke-width="3"></circle>
                <circle id="rainGaugeCircle" cx="18" cy="18" r="16" fill="none" stroke="#FF8C00" stroke-width="3" stroke-dasharray="100 100" stroke-linecap="round"></circle>
              </svg>
              <div style="position: absolute; top: 0; left: 0; right: 0; bottom: 0; display: flex; align-items: center; justify-content: center; font-size: 10px; font-weight: bold; color: #fff;" id="subRain">--%</div>
            </div>
            <div style="display: flex; flex-direction: column;">
              <span class="badge-label" style="font-size: 10px;">Xác suất mưa</span>
              <span style="font-size: 10px; color: #FF8C00; font-weight: bold;">(AI Dự báo)</span>
            </div>
          </div>
          <div class="hero-badge badge-status">
            <span class="badge-icon" id="aiStatusIcon">🤖</span>
            <span class="badge-label">Dự kiến (+1h):</span>
            <span class="badge-value" id="aiStatusText">Đang tính...</span>
          </div>
        </div>
      </div>
    </section>

    <!-- AI MULTI-HORIZON FORECAST SECTION (NGAY SAU HERO) -->
    <section class="ai-forecast-section glass-card">
      <div class="ai-forecast-header">
        <span style="color: var(--warning); font-size: 16px;">💡</span>
        <h2>Dự báo AI những giờ tới</h2>
      </div>
      <div class="ai-forecast-cards">
        <!-- +1 Giờ Card -->
        <div class="ai-forecast-card ai-card-1h">
          <span class="ai-card-title">+1 giờ</span>
          <span class="ai-card-icon" id="aiFCardIcon1">--</span>
          <span class="ai-card-temp" id="aiFCardTemp1">--°C</span>
          <span class="ai-card-rain" id="aiFCardRain1">--% mưa</span>
        </div>
        <!-- +3 Giờ Card -->
        <div class="ai-forecast-card ai-card-3h">
          <span class="ai-card-title">+3 giờ</span>
          <span class="ai-card-icon" id="aiFCardIcon2">--</span>
          <span class="ai-card-temp" id="aiFCardTemp2">--°C</span>
          <span class="ai-card-rain" id="aiFCardRain2">--% mưa</span>
        </div>
        <!-- +6 Giờ Card -->
        <div class="ai-forecast-card ai-card-6h">
          <span class="ai-card-title">+6 giờ</span>
          <span class="ai-card-icon" id="aiFCardIcon3">--</span>
          <span class="ai-card-temp" id="aiFCardTemp3">--°C</span>
          <span class="ai-card-rain" id="aiFCardRain3">--% mưa</span>
        </div>
        <!-- +12 Giờ Card -->
        <div class="ai-forecast-card ai-card-12h">
          <span class="ai-card-title">+12 giờ</span>
          <span class="ai-card-icon" id="aiFCardIcon4">--</span>
          <span class="ai-card-temp" id="aiFCardTemp4">--°C</span>
          <span class="ai-card-rain" id="aiFCardRain4">--% mưa</span>
        </div>
      </div>
      <div class="ai-forecast-footer">
        Độ mờ giảm dần theo giờ — thể hiện trực tiếp độ tin cậy giảm (confidence decay)
      </div>
    </section>

    <!-- SYMMETRICAL 2-COLUMN LOWER GRID -->
    <div class="balanced-grid">

      <!-- LEFT COLUMN: SENSOR METRICS & MATLAB CLOUD CHARTS -->
      <div class="grid-column">
        
        <!-- 2x2 PARAMETERS GRID -->
        <div class="params-grid">
          <!-- HUMIDITY CARD -->
          <div class="param-card glass-card hum-theme">
            <div class="card-icon">
              <svg viewBox="0 0 24 24" width="22" height="22" fill="none" stroke="currentColor" stroke-width="2">
                <path d="M12 2.69l5.66 5.66a8 8 0 1 1-11.31 0z"/>
              </svg>
            </div>
            <div class="card-info">
              <h3>Độ Ẩm Không Khí</h3>
              <div class="card-value"><span id="valHumidity">--</span><span class="unit">%</span></div>
              <div class="progress-bar-container">
                <div class="progress-bar" id="barHumidity" style="width: 0%"></div>
              </div>
            </div>
          </div>

          <!-- PRESSURE CARD -->
          <div class="param-card glass-card pres-theme">
            <div class="card-icon">
              <svg viewBox="0 0 24 24" width="22" height="22" fill="none" stroke="currentColor" stroke-width="2">
                <line x1="18" y1="20" x2="18" y2="10"></line>
                <line x1="12" y1="20" x2="12" y2="4"></line>
                <line x1="6" y1="20" x2="6" y2="14"></line>
              </svg>
            </div>
            <div class="card-info">
              <h3>Áp Suất Khí Quyển</h3>
              <div class="card-value"><span id="valPressure">--</span><span class="unit">hPa</span></div>
              <div class="card-subtext" id="subPressure">Trend: --</div>
              <div style="margin-top: 4px;"><span class="pressure-badge" id="badgePressure">--</span></div>
            </div>
          </div>

          <!-- RAIN SENSOR CARD -->
          <div class="param-card glass-card rain-theme">
            <div class="card-icon">
              <svg viewBox="0 0 24 24" width="22" height="22" fill="none" stroke="currentColor" stroke-width="2">
                <path d="M20 17.58A5 5 0 0 0 18 8h-1.26A8 8 0 1 0 4 16.25"/>
                <line x1="8" y1="16" x2="8" y2="22"></line>
                <line x1="12" y1="18" x2="12" y2="22"></line>
                <line x1="16" y1="16" x2="16" y2="22"></line>
              </svg>
            </div>
            <div class="card-info">
              <h3>Lượng Mưa & Tưới Tiêu</h3>
              <div class="card-value"><span id="valRain">--</span><span class="unit">/1023</span></div>
              <span class="rain-status-text" id="statusRain">Đang đọc...</span>
            </div>
          </div>

          <!-- BATTERY CARD -->
          <div class="param-card glass-card bat-theme">
            <div class="card-icon">
              <svg viewBox="0 0 24 24" width="22" height="22" fill="none" stroke="currentColor" stroke-width="2">
                <rect x="2" y="7" width="16" height="10" rx="2" ry="2"></rect>
                <line x1="22" y1="11" x2="22" y2="13"></line>
              </svg>
            </div>
            <div class="card-info">
              <h3>Pin Trạm Khí Tượng</h3>
              <div class="card-value"><span id="valBattery">--</span><span class="unit">%</span></div>
              <div class="battery-battery-bar">
                <div class="battery-fill" id="barBattery" style="width: 0%"></div>
              </div>
            </div>
          </div>
        </div>

        <!-- PYTHON ML FORECAST CARD -->
        <div class="glass-card matlab-chart-card">
          <div class="card-header" style="flex-wrap: wrap; gap: 8px;">
            <div class="header-title">
              <span class="spark-icon">🤖</span>
              <h2>Học Máy Python AI</h2>
            </div>
            <div style="display: flex; gap: 4px; background: rgba(0,0,0,0.3); border-radius: 10px; padding: 2px; border: 1px solid rgba(255,255,255,0.05);">
              <button class="ml-tab active" onclick="switchMlTab(event, 'info')">Chỉ số</button>
              <button class="ml-tab" onclick="switchMlTab(event, 'importance')">Đặc trưng</button>
              <button class="ml-tab" onclick="switchMlTab(event, 'matrix')">Ma trận</button>
            </div>
          </div>
          
          <div class="ml-metrics-container" style="padding: 12px; background: rgba(0, 0, 0, 0.2); border-radius: 16px; border: 1px solid rgba(255, 255, 255, 0.05); height: 290px; display: flex; flex-direction: column; justify-content: center; overflow: hidden;">
            
            <!-- TAB 1: INFO -->
            <div id="mlTabInfo" style="display: flex; flex-direction: column; gap: 14px; height: 100%; justify-content: center;">
              <div style="display: grid; grid-template-columns: 1fr 1fr; gap: 12px;">
                <div style="background: rgba(255,255,255,0.03); padding: 12px; border-radius: 10px; border: 1px solid rgba(255,255,255,0.02); text-align: center;">
                  <div style="font-size: 10px; color: var(--text-secondary); margin-bottom: 4px;">Sai số Nhiệt độ (MAE)</div>
                  <div style="font-size: 18px; font-weight: 700; color: var(--accent-temp);">±0.45 °C</div>
                </div>
                <div style="background: rgba(255,255,255,0.03); padding: 12px; border-radius: 10px; border: 1px solid rgba(255,255,255,0.02); text-align: center;">
                  <div style="font-size: 18px; font-weight: 700; color: var(--accent-rain);">95.2%</div>
                  <div style="font-size: 10px; color: var(--text-secondary); margin-top: 4px;">Độ chính xác AI (Acc)</div>
                </div>
              </div>
              <div style="font-size: 11px; color: var(--text-secondary); line-height: 1.5; padding: 0 4px;">
                <p>🤖 <b>Mô hình:</b> Random Forest hồi quy (Continuous) dự báo Temp/Hum và phân loại Classifier dự đoán xác suất mưa.</p>
                <p style="margin-top: 6px;">📍 <b>Cập nhật:</b> Tự động chạy mỗi 30 phút qua GitHub Actions đẩy lên ThingSpeak.</p>
              </div>
            </div>
            
            <!-- TAB 2: FEATURE IMPORTANCE -->
            <div id="mlTabImportance" class="hidden" style="height: 100%; display: flex; flex-direction: column; justify-content: center;">
              <h3 style="font-size: 11px; font-family: 'Sora', sans-serif; margin-bottom: 8px; color: var(--text-primary); padding-left: 4px;">Tầm quan trọng đặc trưng (Feature Importance):</h3>
              <svg viewBox="0 0 260 160" width="100%" height="100%">
                <!-- Bar 1: pres_diff -->
                <text x="5" y="20" fill="#909097" font-size="9" font-family="Inter, sans-serif">pres_diff</text>
                <rect x="75" y="11" width="112" height="10" rx="3" fill="#00F2FF" />
                <text x="195" y="20" fill="#fff" font-size="9" font-family="Sora, sans-serif" font-weight="bold">35.4%</text>
                
                <!-- Bar 2: humidity -->
                <text x="5" y="50" fill="#909097" font-size="9" font-family="Inter, sans-serif">humidity</text>
                <rect x="75" y="41" width="82" height="10" rx="3" fill="#00F2FF" />
                <text x="195" y="50" fill="#fff" font-size="9" font-family="Sora, sans-serif" font-weight="bold">26.1%</text>
                
                <!-- Bar 3: temperature -->
                <text x="5" y="80" fill="#909097" font-size="9" font-family="Inter, sans-serif">temperature</text>
                <rect x="75" y="71" width="60" height="10" rx="3" fill="#00F2FF" />
                <text x="195" y="80" fill="#fff" font-size="9" font-family="Sora, sans-serif" font-weight="bold">19.2%</text>
                
                <!-- Bar 4: rain_analog -->
                <text x="5" y="110" fill="#909097" font-size="9" font-family="Inter, sans-serif">rain_analog</text>
                <rect x="75" y="101" width="44" height="10" rx="3" fill="#00F2FF" />
                <text x="195" y="110" fill="#fff" font-size="9" font-family="Sora, sans-serif" font-weight="bold">14.0%</text>
                
                <!-- Bar 5: battery_pct -->
                <text x="5" y="140" fill="#909097" font-size="9" font-family="Inter, sans-serif">battery_pct</text>
                <rect x="75" y="131" width="16" height="10" rx="3" fill="#00F2FF" />
                <text x="195" y="140" fill="#fff" font-size="9" font-family="Sora, sans-serif" font-weight="bold">5.3%</text>
              </svg>
            </div>
            
            <!-- TAB 3: CONFUSION MATRIX Heatmap -->
            <div id="mlTabMatrix" class="hidden" style="height: 100%; display: flex; flex-direction: column; justify-content: center;">
              <h3 style="font-size: 11px; font-family: 'Sora', sans-serif; margin-bottom: 8px; color: var(--text-primary); text-align: center;">Confusion Matrix (Ma trận nhầm lẫn 3x3)</h3>
              <div style="display: grid; grid-template-columns: 50px 1fr; gap: 4px; font-size: 9px; font-family: 'Inter', sans-serif;">
                <div></div>
                <div style="display: grid; grid-template-columns: 1fr 1fr 1fr; text-align: center; color: var(--text-secondary); font-weight: bold; margin-bottom: 2px;">
                  <span>Nắng</span><span>Mây</span><span>Mưa</span>
                </div>
                
                <div style="display: flex; flex-direction: column; justify-content: space-around; font-weight: bold; color: var(--text-secondary); text-align: right; padding-right: 6px;">
                  <span>Nắng</span><span>Mây</span><span>Mưa</span>
                </div>
                
                <div style="display: grid; grid-template-columns: 1fr 1fr 1fr; grid-template-rows: 1fr 1fr 1fr; gap: 4px; height: 110px;">
                  <!-- Row 1: Act Sunny -->
                  <div style="background: rgba(0, 242, 255, 0.7); color: #0e0e0f; display: flex; align-items: center; justify-content: center; border-radius: 4px; font-weight: bold; font-size: 10px;">88%</div>
                  <div style="background: rgba(255,255,255,0.08); color: #fff; display: flex; align-items: center; justify-content: center; border-radius: 4px;">9%</div>
                  <div style="background: rgba(255,255,255,0.03); color: #fff; display: flex; align-items: center; justify-content: center; border-radius: 4px;">3%</div>
                  
                  <!-- Row 2: Act Cloudy -->
                  <div style="background: rgba(255,255,255,0.06); color: #fff; display: flex; align-items: center; justify-content: center; border-radius: 4px;">7%</div>
                  <div style="background: rgba(0, 242, 255, 0.65); color: #0e0e0f; display: flex; align-items: center; justify-content: center; border-radius: 4px; font-weight: bold; font-size: 10px;">84%</div>
                  <div style="background: rgba(255,255,255,0.08); color: #fff; display: flex; align-items: center; justify-content: center; border-radius: 4px;">9%</div>
                  
                  <!-- Row 3: Act Rainy -->
                  <div style="background: rgba(255,255,255,0.02); color: #fff; display: flex; align-items: center; justify-content: center; border-radius: 4px;">2%</div>
                  <div style="background: rgba(255,255,255,0.05); color: #fff; display: flex; align-items: center; justify-content: center; border-radius: 4px;">6%</div>
                  <div style="background: rgba(0, 242, 255, 0.8); color: #0e0e0f; display: flex; align-items: center; justify-content: center; border-radius: 4px; font-weight: bold; font-size: 10px;">92%</div>
                </div>
              </div>
            </div>
            
          </div>
        </div>

      </div>

      <!-- RIGHT COLUMN: HISTORICAL CHARTS & LOCAL AI FORECAST -->
      <div class="grid-column">
        
        <!-- CHART.JS CARD -->
        <div class="glass-card chart-container-card">
          <div class="chart-header">
            <h2>Biểu Đồ Khí Hậu Canh Tác (8 Kênh)</h2>
            <div class="chart-tabs-wrapper">
              <div class="chart-tabs">
                <button class="chart-tab tab-temp active" onclick="switchChart('temp')">Nhiệt Độ Canh Tác</button>
                <button class="chart-tab tab-hum" onclick="switchChart('hum')">Độ Ẩm Đất/Khí</button>
                <button class="chart-tab tab-pres" onclick="switchChart('pres')">Khí Áp Canh Tác</button>
                <button class="chart-tab tab-rain" onclick="switchChart('rain')">Lượng Mưa</button>
                <button class="chart-tab tab-bat" onclick="switchChart('bat')">Nguồn Điện (Pin)</button>
                <button class="chart-tab tab-forecast" onclick="switchChart('forecast')">Dự Báo AI (+12h)</button>
              </div>
            </div>
          </div>
          <div class="chart-wrapper">
            <div id="weatherHistoryChart" style="width: 100%; height: 100%;"></div>
          </div>
        </div>

        <!-- LOCAL LINEAR REGRESSION AI ENGINE -->
        <div class="glass-card forecast-card">
          <div class="card-header">
            <div class="header-title">
              <div class="spark-icon">⚡</div>
              <h2>Dự Báo Cây Trồng AI Cục Bộ (Hồi Quy)</h2>
            </div>
            <span class="engine-badge">JS Engine v1.0</span>
          </div>
          
          <div class="forecast-grid">
            <div class="prediction-box">
              <div class="pred-item">
                <span class="pred-label">Nhiệt độ dự tính (+1h)</span>
                <span class="pred-val" id="predTemp">-- °C</span>
                <span class="pred-trend" id="trendTemp">--</span>
              </div>
              <div class="pred-item">
                <span class="pred-label">Độ ẩm dự tính (+1h)</span>
                <span class="pred-val" id="predHum">-- %</span>
                <span class="pred-trend" id="trendHum">--</span>
              </div>
              <div class="pred-item">
                <span class="pred-label">Áp suất dự tính (+1h)</span>
                <span class="pred-val" id="predPres">-- hPa</span>
                <span class="pred-trend" id="trendPres">--</span>
              </div>
            </div>

            <div class="regression-math-box">
              <h3>Phương trình xu hướng canh tác:</h3>
              <div class="math-formula" id="mathTemp">T(t) = -- * t + --</div>
              <div class="math-formula" id="mathHum">H(t) = -- * t + --</div>
              <div class="math-formula" id="mathPres">P(t) = -- * t + --</div>
            </div>
          </div>

          <hr class="card-divider">

          <div class="recommendation-box">
            <div class="rec-icon">💡</div>
            <div class="rec-content">
              <h4>💡 Khuyên dùng cho canh tác & tưới tiêu:</h4>
              <p id="systemRecommendation">Đang tính toán mô hình hồi quy dựa trên dữ liệu khí quyển...</p>
            </div>
          </div>
        </div>

      </div>
    </div>

    <!-- SETTINGS MODAL -->
    <div class="modal-overlay" id="settingsModal">
      <div class="modal-card">
        <div class="modal-header">
          <h2>Cấu Hình Hệ Thống</h2>
          <button class="close-btn" onclick="toggleModal('settingsModal', false)">&times;</button>
        </div>
        <div class="modal-body">
          <div class="form-group">
            <label for="inputChannelId">ThingSpeak Channel ID</label>
            <input type="text" id="inputChannelId" placeholder="Ví dụ: 12397">
            <small>ID của kênh ThingSpeak lưu dữ liệu trạm thời tiết của bạn.</small>
          </div>
          
          <div class="form-group">
            <label for="inputReadApiKey">ThingSpeak Read API Key</label>
            <input type="password" id="inputReadApiKey" placeholder="Nhập Read API Key (nếu Private)">
            <small>Cần thiết để tải 8 biểu đồ dữ liệu và chạy phân tích.</small>
          </div>

          <div class="form-group">
            <label for="inputMatlabId">MATLAB Visualization ID</label>
            <input type="text" id="inputMatlabId" placeholder="Nhập ID App Visualizations (ví dụ: 45678)">
            <small>ID của MATLAB Visualizations App để nhúng biểu đồ so sánh từ Cloud.</small>
          </div>

          <div class="form-group">
            <label for="inputEspIp">ESP8266 Local Gateway IP</label>
            <input type="text" id="inputEspIp" placeholder="Mặc định: 192.168.4.1">
            <small>Địa chỉ IP của ESP8266 khi kết nối trực tiếp vào WiFi AP "WeatherStation".</small>
          </div>
          
          <div class="modal-actions">
            <button class="btn btn-secondary" onclick="resetToDemoSettings()">Khôi phục Demo</button>
            <button class="btn btn-primary" onclick="saveSettings()">Lưu Cấu Hình</button>
          </div>
        </div>
      </div>
    </div>

    <!-- ALERT OVERLAYS -->
    <div class="alert-banner hidden" id="alertBox">
      <span class="alert-icon">⚠</span>
      <span class="alert-message" id="alertMessage">Cảnh báo: Phát hiện mưa lớn! Khả năng có dông bão nguy hiểm.</span>
    </div>
  </div>

  <!-- JavaScript Client Core -->
  <script>

    // ==========================================================================
    // WEATHER STATION PRO - CLIENT JAVASCRIPT
    // ==========================================================================

    // --- Trạng thái kết nối ---
    let isFetching = false;
    let lastReceivedSeqNum = null; // ★ Lọc trùng lặp dữ liệu local

    function fetchWithTimeout(url, options = {}, timeout = 1500) {
      const controller = new AbortController();
      const id = setTimeout(() => controller.abort(), timeout);
      return fetch(url, {
        ...options,
        signal: controller.signal
      }).finally(() => clearTimeout(id));
    }

    // --- Cấu hình & Trạng thái mặc định ---
    let config = {
      channelId: '3413241',        // ID kênh mặc định của bạn
      readApiKey: 'VISWQT7BABDVZOFS', // Read API Key mặc định của bạn
      matlabId: '674169',          // MATLAB Visualization ID mặc định của bạn (Không dùng)
      espIp: '192.168.4.1',        // IP mặc định của Gateway ESP8266 AP
      dataSource: 'local'          // Nguồn dữ liệu: 'local' (ESP8266) hoặc 'cloud' (ThingSpeak)
    };

    // --- Dữ liệu hiện tại ---
    let currentData = {
      temperature: 25.0,
      humidity: 60,
      pressure: 1011.3,
      rain: 1023,
      battery: 100,
      predictedTemp: null,         // field6 (Python AI dự báo nhiệt độ)
      rainProbability: null,       // field7 (Python AI xác suất mưa %)
      predictedStatus: null,       // field8 (Python AI mã trạng thái thời tiết 0-2)
      rssi: -70,
      seqNum: 0,
      lastUpdated: null
    };

    // --- Bộ nhớ lịch sử (cho biểu đồ và hồi quy) ---
    let historyData = {
      timestamps: [],
      temperature: [],             // field1
      humidity: [],                // field2
      pressure: [],                // field3
      rain: [],                    // field4
      battery: [],                 // field5
      predictedTemp: [],           // field6
      rainProbability: [],         // field7
      predictedStatus: []          // field8
    };

    let activeChartTab = 'temp';   // 'temp', 'hum', 'pres', 'rain', 'bat'
    let fetchInterval = null;
    let lastSensorTime = null;
    let lastAiTime = null;

    // KHỞI CHẠY TRANG
    document.addEventListener("DOMContentLoaded", () => {
      loadConfig();
      initChart();
      startDataFetching();
      refreshData();
      setInterval(updateDualTimestampDisplay, 10000);
    });


    // Load config từ localStorage
    function loadConfig() {
      const savedConfig = localStorage.getItem('weather_dashboard_config');
      if (savedConfig) {
        try {
          const parsed = JSON.parse(savedConfig);
          // Hợp nhất để giữ các giá trị mặc định nếu giá trị trong localStorage bị trống hoặc thiếu
          config = {
            channelId: parsed.channelId || config.channelId,
            readApiKey: parsed.readApiKey || config.readApiKey,
            matlabId: parsed.matlabId || config.matlabId,
            espIp: parsed.espIp || config.espIp,
            dataSource: parsed.dataSource || config.dataSource
          };
        } catch (e) {
          console.error("Lỗi parse config:", e);
        }
      }
      
      // Điền vào form settings
      document.getElementById('inputChannelId').value = config.channelId || '';
      document.getElementById('inputReadApiKey').value = config.readApiKey || '';
      document.getElementById('inputMatlabId').value = config.matlabId || '';
      document.getElementById('inputEspIp').value = config.espIp || '192.168.4.1';
      
      updateSourceButtons();
    }

    // Lưu cấu hình
    function saveSettings() {
      config.channelId = document.getElementById('inputChannelId').value.trim();
      config.readApiKey = document.getElementById('inputReadApiKey').value.trim();
      config.matlabId = document.getElementById('inputMatlabId').value.trim();
      config.espIp = document.getElementById('inputEspIp').value.trim() || '192.168.4.1';
      
      localStorage.setItem('weather_dashboard_config', JSON.stringify(config));
      toggleModal('settingsModal', false);
      
      startDataFetching();
      refreshData();
    }

    // Reset cấu hình về kênh mặc định của bạn
    function resetToDemoSettings() {
      document.getElementById('inputChannelId').value = '3413241';
      document.getElementById('inputReadApiKey').value = 'VISWQT7BABDVZOFS';
      document.getElementById('inputMatlabId').value = '674169';
      document.getElementById('inputEspIp').value = '192.168.4.1';
    }

    function updateSourceButtons() {
      const btnCloud = document.getElementById('btnCloud');
      const btnLocal = document.getElementById('btnLocal');
      const stationModeText = document.getElementById('stationModeText');
      
      if (config.dataSource === 'cloud') {
        if (btnCloud) btnCloud.classList.add('active');
        if (btnLocal) btnLocal.classList.remove('active');
        if (stationModeText) stationModeText.innerText = `ThingSpeak Channel: ${config.channelId}`;
      } else {
        if (btnCloud) btnCloud.classList.remove('active');
        if (btnLocal) btnLocal.classList.add('active');
        if (stationModeText) stationModeText.innerText = `ESP8266 Local Server: http://${config.espIp}`;
      }
    }


    // Set nguồn dữ liệu
    function setDataSource(source) {
      if (config.dataSource !== source) {
        config.dataSource = source;
        localStorage.setItem('weather_dashboard_config', JSON.stringify(config));
        updateSourceButtons();
        startDataFetching();
        refreshData();
      }
    }

    function toggleModal(modalId, show) {
      const modal = document.getElementById(modalId);
      if (show) {
        modal.classList.add('active');
      } else {
        modal.classList.remove('active');
      }
    }

    // RENDER BIỂU ĐỒ BẰNG ENGINE SVG TỰ THÂN (LIBRARY-LESS SVG CHART)
    function initChart() {
      updateChartData();
    }

    function switchChart(tabName) {
      activeChartTab = tabName;
      
      const tabs = document.querySelectorAll('.chart-tab');
      tabs.forEach(tab => {
        if (tab.classList.contains(`tab-${tabName}`)) {
          tab.classList.add('active');
        } else {
          tab.classList.remove('active');
        }
      });
      
      updateChartData();
    }

    function updateChartData() {
      const container = document.getElementById('weatherHistoryChart');
      if (!container) return;

      const timestamps = historyData.timestamps;
      if (!timestamps || timestamps.length === 0) {
        container.innerHTML = `
          <div style="display: flex; align-items: center; justify-content: center; height: 100%; color: #909097; font-size: 13px; font-family: 'Sora', sans-serif;">
            Chưa có dữ liệu lịch sử để hiển thị biểu đồ
          </div>`;
        return;
      }

      if (activeChartTab === 'forecast') {
        const regTemp = calculateLinearRegression(historyData.temperature);
        const slope = isNaN(regTemp.a) ? 0.0 : regTemp.a;
        
        const regHum = calculateLinearRegression(historyData.humidity);
        const slopeH = isNaN(regHum.a) ? 0.0 : regHum.a;

        const regPres = calculateLinearRegression(historyData.pressure);
        const slopeP = isNaN(regPres.a) ? 0.0 : regPres.a;

        const t0 = currentData.temperature || 25.0;
        const t1 = currentData.predictedTemp !== null ? currentData.predictedTemp : (t0 + slope);
        const effSlope = (t1 - t0);
        const t3 = t1 + 2 * effSlope;
        const t6 = t1 + 5 * effSlope;
        const t12 = t1 + 11 * effSlope;
        const forecastTemps = [t0, t1, t3, t6, t12].map(t => Math.max(10, Math.min(45, t)));

        const uncertainties = [0.0, 0.45, 0.90, 1.40, 2.20];
        const upperBounds = forecastTemps.map((t, idx) => t + uncertainties[idx]);
        const lowerBounds = forecastTemps.map((t, idx) => t - uncertainties[idx]);

        const p0 = currentData.rainProbability !== null ? currentData.rainProbability : 20;
        const p1 = currentData.rainProbability !== null ? currentData.rainProbability : 20;
        const effSlopeP = (p1 - p0) || (slopeH > 0 ? 2 : -2);
        const p3 = Math.max(0, Math.min(100, p1 + 2 * effSlopeP + (slopeH > 0 ? 5 : -5)));
        const p6 = Math.max(0, Math.min(100, p1 + 5 * effSlopeP + (slopeH > 0 ? 10 : -10)));
        const p12 = Math.max(0, Math.min(100, p1 + 11 * effSlopeP + (slopeH > 0 ? 15 : -15)));
        const forecastProbs = [p0, p1, p3, p6, p12];

        const icons = forecastProbs.map(p => p > 50 ? '🌧️' : (p > 25 ? '☁️' : '☀️'));
        const labels = forecastProbs.map(p => p > 50 ? 'Mưa dông' : (p > 25 ? 'Nhiều mây' : 'Nắng ráo'));

        let fMin = Math.min(...lowerBounds);
        let fMax = Math.max(...upperBounds);
        let fRange = fMax - fMin;
        if (fRange === 0) { fMin -= 1; fMax += 1; fRange = 2; }
        else { fMin -= fRange * 0.15; fMax += fRange * 0.15; fRange = fMax - fMin; }

        const width = 650;
        const height = 250;
        const padLeft = 50;
        const padRight = 50;
        const padTop = 20;
        const padBottom = 100;
        const chartW = width - padLeft - padRight;
        const chartH = height - padTop - padBottom;

        const xPositions = [padLeft, padLeft + 0.25*chartW, padLeft + 0.5*chartW, padLeft + 0.75*chartW, padLeft + chartW];
        const yTemps = forecastTemps.map(t => padTop + chartH - ((t - fMin) / fRange) * chartH);
        const yUppers = upperBounds.map(t => padTop + chartH - ((t - fMin) / fRange) * chartH);
        const yLowers = lowerBounds.map(t => padTop + chartH - ((t - fMin) / fRange) * chartH);

        let bandD = `M ${xPositions[0]} ${yUppers[0]}`;
        for (let i = 1; i < 5; i++) bandD += ` L ${xPositions[i]} ${yUppers[i]}`;
        for (let i = 4; i >= 0; i--) bandD += ` L ${xPositions[i]} ${yLowers[i]}`;
        bandD += ' Z';

        let lineD = `M ${xPositions[0]} ${yTemps[0]}`;
        for (let i = 1; i < 5; i++) lineD += ` L ${xPositions[i]} ${yTemps[i]}`;

        let yGridHtml = '';
        for (let i = 0; i < 4; i++) {
          const ratio = i / 3;
          const yVal = fMax - ratio * fRange;
          const yPos = padTop + ratio * chartH;
          yGridHtml += `
            <line x1="${padLeft}" y1="${yPos}" x2="${width - padRight}" y2="${yPos}" stroke="rgba(255, 255, 255, 0.04)" stroke-width="1" stroke-dasharray="2 2" />
            <text x="${padLeft - 8}" y="${yPos + 3.5}" fill="#909097" font-size="9" font-family="Inter, sans-serif" text-anchor="end">${yVal.toFixed(1)}°C</text>
          `;
        }

        let xLabelsHtml = '';
        for (let i = 0; i < 5; i++) {
          const x = xPositions[i];
          const hz = ['Hiện tại', '+1h', '+3h', '+6h', '+12h'][i];
          const tempStr = `${forecastTemps[i].toFixed(1)}°C`;
          const rainStr = `${forecastProbs[i].toFixed(0)}%`;
          const icon = icons[i];
          const lbl = labels[i];
          
          xLabelsHtml += `
            <line x1="${x}" y1="${padTop}" x2="${x}" y2="${padTop + chartH}" stroke="rgba(255, 255, 255, 0.05)" stroke-width="1" stroke-dasharray="2 2" />
            <text x="${x}" y="${padTop + chartH + 16}" fill="#fff" font-size="9" font-family="Sora, sans-serif" font-weight="bold" text-anchor="middle">${hz}</text>
            <text x="${x}" y="${padTop + chartH + 40}" fill="#fff" font-size="16" text-anchor="middle">${icon}</text>
            <text x="${x}" y="${padTop + chartH + 58}" fill="#909097" font-size="8" font-family="Inter, sans-serif" text-anchor="middle">${lbl}</text>
            <text x="${x}" y="${padTop + chartH + 72}" fill="#00F2FF" font-size="8" font-family="Sora, sans-serif" font-weight="600" text-anchor="middle">${tempStr} | ${rainStr}</text>
          `;
        }

        let pointsHtml = '';
        for (let i = 0; i < 5; i++) {
          pointsHtml += `
            <circle cx="${xPositions[i]}" cy="${yTemps[i]}" r="4" fill="#FF8C00" stroke="#131314" stroke-width="1.5" />
          `;
        }

        container.innerHTML = `
          <div style="position: relative; width: 100%; height: 100%; display: flex; flex-direction: column;">
            <div style="display: flex; justify-content: flex-end; gap: 14px; padding: 0 10px 6px 0;">
              <div style="display: flex; align-items: center; gap: 6px; font-size: 11px; font-family: 'Inter', sans-serif; color: #c7c6cd;">
                <span style="display: inline-block; width: 14px; height: 0; border-top: 2px dashed #FF8C00; border-style: dashed;"></span>
                <span>Dự báo AI (°C)</span>
              </div>
              <div style="display: flex; align-items: center; gap: 6px; font-size: 11px; font-family: 'Inter', sans-serif; color: #c7c6cd;">
                <span style="display: inline-block; width: 14px; height: 10px; background: rgba(255, 140, 0, 0.08); border: 0.5px solid rgba(255, 140, 0, 0.2);"></span>
                <span>Dải Tin Cậy (Confidence decay)</span>
              </div>
            </div>
            <div style="flex: 1; position: relative; overflow: hidden;">
              <svg width="100%" height="100%" viewBox="0 0 ${width} 250" preserveAspectRatio="none" style="display: block;">
                <path d="${bandD}" fill="rgba(255, 140, 0, 0.07)" stroke="rgba(255, 140, 0, 0.12)" stroke-width="0.5" />
                <path d="${lineD}" fill="none" stroke="#FF8C00" stroke-width="2" stroke-dasharray="4 4" stroke-linecap="round" stroke-linejoin="round" />
                ${yGridHtml}
                ${xLabelsHtml}
                ${pointsHtml}
              </svg>
            </div>
          </div>
        `;
        return;
      }

      let datasets = [];
      if (activeChartTab === 'temp') {
        datasets.push({
          label: 'Thực tế (°C)',
          data: historyData.temperature,
          color: '#00F2FF',
          dashed: false
        });
        datasets.push({
          label: 'Dự báo AI (°C)',
          data: historyData.predictedTemp,
          color: '#FF8C00',
          dashed: true
        });
      } 
      else if (activeChartTab === 'hum') {
        datasets.push({
          label: 'Độ ẩm (%)',
          data: historyData.humidity,
          color: '#00F2FF',
          dashed: false
        });
      } 
      else if (activeChartTab === 'pres') {
        datasets.push({
          label: 'Khí áp (hPa)',
          data: historyData.pressure,
          color: '#00F2FF',
          dashed: false
        });
      }
      else if (activeChartTab === 'rain') {
        datasets.push({
          label: 'Chỉ số khô ráo (0-1023)',
          data: historyData.rain,
          color: '#00F2FF',
          dashed: false
        });
        datasets.push({
          label: 'Xác suất mưa AI (%)',
          data: historyData.rainProbability,
          color: '#FF8C00',
          dashed: true
        });
      }
      else if (activeChartTab === 'bat') {
        datasets.push({
          label: 'Nguồn Pin (%)',
          data: historyData.battery,
          color: '#00F2FF',
          dashed: false
        });
      }

      // Slice the history data to show only the last 30 points on the charts
      const maxPlotPoints = 30;
      const startIdx = Math.max(0, timestamps.length - maxPlotPoints);
      const slicedTimestamps = timestamps.slice(startIdx);
      
      // Slice the dataset arrays
      datasets.forEach(ds => {
        ds.data = ds.data.slice(startIdx);
      });

      // Lấy tất cả giá trị hợp lệ để tính min/max
      let allValues = [];
      datasets.forEach(ds => {
        ds.data.forEach(val => {
          if (val !== null && val !== undefined && !isNaN(val)) {
            allValues.push(val);
          }
        });
      });

      if (allValues.length === 0) {
        container.innerHTML = `
          <div style="display: flex; align-items: center; justify-content: center; height: 100%; color: #909097; font-size: 13px; font-family: 'Sora', sans-serif;">
            Chưa có đủ điểm dữ liệu hợp lệ để hiển thị
          </div>`;
        return;
      }

      let minVal = Math.min(...allValues);
      let maxVal = Math.max(...allValues);

      // Thêm đệm khoảng trống ở biên trên/dưới
      let range = maxVal - minVal;
      if (range === 0) {
        minVal = minVal - 1;
        maxVal = maxVal + 1;
        range = 2;
      } else {
        minVal = minVal - range * 0.1;
        maxVal = maxVal + range * 0.1;
        range = maxVal - minVal;
      }

      // Tính toán nhãn thời gian mở rộng và độ lệch (shiftOffset) cho T+1h dự báo
      let shiftOffset = 10; // Mặc định: 10 điểm dữ liệu tương đương 1 giờ
      if (slicedTimestamps.length > 1) {
        let timeDiffs = [];
        for (let i = 1; i < slicedTimestamps.length; i++) {
          let [h1, m1] = slicedTimestamps[i-1].split(':').map(Number);
          let [h2, m2] = slicedTimestamps[i].split(':').map(Number);
          let diff = (h2 * 60 + m2) - (h1 * 60 + m1);
          if (diff < 0) diff += 1440;
          if (diff > 0 && diff < 60) timeDiffs.push(diff);
        }
        let avgDiff = timeDiffs.length > 0 ? timeDiffs.reduce((a,b)=>a+b, 0)/timeDiffs.length : 6.0;
        shiftOffset = Math.max(1, Math.round(60 / avgDiff));
      }

      let extendedTimestamps = [...slicedTimestamps];
      if (slicedTimestamps.length > 0) {
        let lastTimeStr = slicedTimestamps[slicedTimestamps.length - 1];
        let [hh, mm] = lastTimeStr.split(':').map(Number);
        let avgInterval = slicedTimestamps.length > 1 ? 60 / shiftOffset : 6.0;
        for(let i = 1; i <= shiftOffset; i++) {
          let totalMinutes = Math.round(mm + i * avgInterval);
          let n_mm = totalMinutes % 60;
          let n_hh = (hh + Math.floor(totalMinutes / 60)) % 24;
          extendedTimestamps.push(`${String(n_hh).padStart(2, '0')}:${String(n_mm).padStart(2, '0')}`);
        }
      }

      const totalPoints = slicedTimestamps.length + shiftOffset;
      const width = 650; // Khung viewBox cố định, co giãn responsive theo CSS
      const height = 220;
      const padLeft = 45;
      const padRight = 15;
      const padTop = 25;
      const padBottom = 30;
      const chartW = width - padLeft - padRight;
      const chartH = height - padTop - padBottom;

      // Tạo đường lưới Y (4 đường)
      let yGridHtml = '';
      const numYGrid = 4;
      for (let i = 0; i < numYGrid; i++) {
        const ratio = i / (numYGrid - 1);
        const yVal = maxVal - ratio * range;
        const yPos = padTop + ratio * chartH;
        
        yGridHtml += `
          <line x1="${padLeft}" y1="${yPos}" x2="${width - padRight}" y2="${yPos}" stroke="rgba(255, 255, 255, 0.05)" stroke-width="1" stroke-dasharray="3 3" />
          <text x="${padLeft - 8}" y="${yPos + 3.5}" fill="#909097" font-size="9" font-family="Inter, sans-serif" text-anchor="end">${yVal.toFixed(1)}</text>
        `;
      }

      // Tạo nhãn X (chia tối đa 5 nhãn)
      let xLabelsHtml = '';
      if (totalPoints > 0) {
        const step = Math.max(1, Math.floor(totalPoints / 5));
        for (let i = 0; i < totalPoints; i++) {
          if (i === 0 || i === totalPoints - 1 || i % step === 0) {
            const xPos = padLeft + (i / (totalPoints - 1 || 1)) * chartW;
            const displayTime = extendedTimestamps[i];
            
            xLabelsHtml += `
              <line x1="${xPos}" y1="${padTop + chartH}" x2="${xPos}" y2="${padTop + chartH + 4}" stroke="rgba(255, 255, 255, 0.1)" />
              <text x="${xPos}" y="${padTop + chartH + 15}" fill="#909097" font-size="9" font-family="Inter, sans-serif" text-anchor="middle">${displayTime}</text>
            `;
          }
        }
      }

      // Tạo các đường cong/gãy biểu đồ
      let pathsHtml = '';
      let defsHtml = '';
      let legendsHtml = '';

      datasets.forEach((ds, dsIdx) => {
        const points = [];
        const isPredicted = ds.dashed || ds.label.includes("Dự báo") || ds.label.includes("Xác suất");
        ds.data.forEach((val, idx) => {
          if (val !== null && val !== undefined && !isNaN(val)) {
            const targetIdx = isPredicted ? idx + shiftOffset : idx;
            const x = padLeft + (targetIdx / (totalPoints - 1 || 1)) * chartW;
            const y = padTop + chartH - ((val - minVal) / range) * chartH;
            points.push({ x, y, val, idx: targetIdx });
          }
        });

        if (points.length === 0) return;

        // Sinh chuỗi vẽ d
        let dPath = `M ${points[0].x} ${points[0].y}`;
        for (let i = 1; i < points.length; i++) {
          dPath += ` L ${points[i].x} ${points[i].y}`;
        }

        // Tạo Gradient nền nếu không phải đường đứt nét
        const gradId = `grad_${activeChartTab}_${dsIdx}`;
        defsHtml += `
          <linearGradient id="${gradId}" x1="0%" y1="0%" x2="0%" y2="100%">
            <stop offset="0%" stop-color="${ds.color}" stop-opacity="0.15" />
            <stop offset="100%" stop-color="${ds.color}" stop-opacity="0.0" />
          </linearGradient>
        `;

        let areaHtml = '';
        if (!ds.dashed && points.length > 1) {
          const dArea = `${dPath} L ${points[points.length - 1].x} ${padTop + chartH} L ${points[0].x} ${padTop + chartH} Z`;
          areaHtml = `<path d="${dArea}" fill="url(#${gradId})" />`;
        }

        // Đường stroke chính
        const strokeDash = ds.dashed ? 'stroke-dasharray="4 4"' : '';
        const lineHtml = `<path d="${dPath}" fill="none" stroke="${ds.color}" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" ${strokeDash} />`;

        // Điểm chấm tròn
        let circlesHtml = '';
        points.forEach((pt, ptIdx) => {
          const isLatest = ptIdx === points.length - 1;
          const time = extendedTimestamps[pt.idx] || '';
          const labelVal = pt.val !== null ? pt.val.toFixed(1) : '--';
          if (isLatest) {
            circlesHtml += `
              <circle cx="${pt.x}" cy="${pt.y}" r="6" fill="${ds.color}" fill-opacity="0.4" class="pulse-ring"
                      style="cursor: pointer;"
                      onmouseover="showChartTooltip(event, '${ds.label}', '${labelVal}', '${time}')"
                      onmousemove="moveChartTooltip(event)"
                      onmouseout="hideChartTooltip()" />
              <circle cx="${pt.x}" cy="${pt.y}" r="3" fill="${ds.color}" stroke="#131314" stroke-width="1"
                      style="cursor: pointer;"
                      onmouseover="this.setAttribute('r', '5'); showChartTooltip(event, '${ds.label}', '${labelVal}', '${time}')"
                      onmousemove="moveChartTooltip(event)"
                      onmouseout="this.setAttribute('r', '3'); hideChartTooltip()" />
            `;
          } else {
            circlesHtml += `
              <circle cx="${pt.x}" cy="${pt.y}" r="3.5" fill="${ds.color}" stroke="#131314" stroke-width="1"
                      style="cursor: pointer; transition: r 0.1s ease;"
                      onmouseover="this.setAttribute('r', '6'); showChartTooltip(event, '${ds.label}', '${labelVal}', '${time}')"
                      onmousemove="moveChartTooltip(event)"
                      onmouseout="this.setAttribute('r', '3.5'); hideChartTooltip()" />
            `;
          }
        });

        pathsHtml += areaHtml + lineHtml + circlesHtml;

        // Build legends
        const legendDashStyle = ds.dashed ? 'border-style: dashed;' : 'border-style: solid;';
        legendsHtml += `
          <div style="display: flex; align-items: center; gap: 6px; font-size: 11px; font-family: 'Inter', sans-serif; color: #c7c6cd;">
            <span style="display: inline-block; width: 14px; height: 0; border-top: 2px ${ds.color}; ${legendDashStyle}"></span>
            <span>${ds.label}</span>
          </div>
        `;
      });

      // Output cấu trúc SVG hoàn thiện
      container.innerHTML = `
        <div style="position: relative; width: 100%; height: 100%; display: flex; flex-direction: column;">
          <!-- Legend Bar -->
          <div style="display: flex; justify-content: flex-end; gap: 14px; padding: 0 10px 6px 0;">
            ${legendsHtml}
          </div>
          <!-- SVG Chart Area -->
          <div style="flex: 1; position: relative; overflow: hidden;">
            <svg width="100%" height="100%" viewBox="0 0 ${width} 220" preserveAspectRatio="none" style="display: block;">
              <defs>
                ${defsHtml}
                <style>
                  @keyframes pulseRing {
                    0% { r: 3.5px; opacity: 0.8; }
                    100% { r: 8px; opacity: 0; }
                  }
                  .pulse-ring {
                    animation: pulseRing 1.8s infinite cubic-bezier(0.25, 0, 0, 1);
                    transform-origin: center;
                  }
                </style>
              </defs>
              
              <!-- Trục ngang chính -->
              <line x1="${padLeft}" y1="${padTop + chartH}" x2="${width - padRight}" y2="${padTop + chartH}" stroke="rgba(255, 255, 255, 0.1)" stroke-width="1" />
              
              <!-- Lưới và đường cong biểu đồ -->
              ${yGridHtml}
              ${xLabelsHtml}
              ${pathsHtml}
            </svg>
          </div>
        </div>
      `;
      // Scroll to the latest data (right)
      setTimeout(() => {
        const scrollArea = document.getElementById('svgScrollArea');
        if (scrollArea) scrollArea.scrollLeft = scrollArea.scrollWidth;
      }, 50);
    }

    // THU THẬP DỮ LIỆU
    function startDataFetching() {
      if (fetchInterval) clearInterval(fetchInterval);
      const intervalTime = (config.dataSource === 'local') ? 2000 : 16000;
      fetchInterval = setInterval(refreshData, intervalTime);
    }

    function refreshData() {
      if (config.dataSource === 'local') {
        fetchFromLocalESP();
      } else {
        fetchFromThingSpeak();
      }
    }

    function fetchFromLocalESP() {
      if (isFetching) return;
      isFetching = true;
      const url = `http://${config.espIp}/data`;
      
      fetchWithTimeout(url, {}, 1200)
        .then(response => {
          if (!response.ok) throw new Error("Local Network Error");
          return response.json();
        })
        .then(data => {
          updateConnectionStatus(true, "Đang kết nối: Local Gateway OK");
          
          const newSeqNum = parseInt(data.seq_num || 0);
          const isNewPacket = (lastReceivedSeqNum === null || newSeqNum !== lastReceivedSeqNum);
          
          currentData.temperature = parseFloat(data.temperature);
          currentData.humidity = parseFloat(data.humidity);
          currentData.pressure = parseFloat(data.pressure);
          currentData.rain = parseInt(data.rain);
          currentData.battery = parseInt(data.battery_pct || 100);
          
          currentData.predictedTemp = currentData.predictedTemp || currentData.temperature;
          currentData.rainProbability = currentData.rainProbability || (currentData.rain < 500 ? 90 : 10);
          currentData.pressureTrend = currentData.pressureTrend || 0.0;
          
          currentData.rssi = parseInt(data.rssi || -70);
          currentData.seqNum = newSeqNum;
          currentData.lastUpdated = new Date().toLocaleTimeString('vi-VN');
          
          lastSensorTime = new Date();
          
          if (isNewPacket) {
            lastReceivedSeqNum = newSeqNum;
            addLocalHistory(currentData);
            runLinearRegression();
          }
          updateUI();
          updateDualTimestampDisplay();

        })
        .catch(err => {
          console.warn("Không kết nối được ESP8266 local, chạy mô phỏng...", err);
          updateConnectionStatus(false, "Mất kết nối Local Gateway. Đang chạy mô phỏng.");
          lastReceivedSeqNum = null; // Reset để mô phỏng liên tục
          generateMockData();
        })
        .finally(() => {
          isFetching = false;
        });
    }

    function fetchFromThingSpeak() {
      if (isFetching) return;
      isFetching = true;
      if (!config.channelId) {
        updateConnectionStatus(false, "Chưa điền Channel ID. Đang chạy mô phỏng.");
        generateMockData();
        isFetching = false;
        return;
      }
      
      let url = `https://api.thingspeak.com/channels/${config.channelId}/feeds.json?results=300`;
      if (config.readApiKey) {
        url += `&api_key=${config.readApiKey}`;
      }
      
      fetchWithTimeout(url, {}, 3000)
        .then(response => {
          if (!response.ok) throw new Error("ThingSpeak Server Error");
          return response.json();
        })
        .then(data => {
          if (!data.feeds || data.feeds.length === 0) {
            throw new Error("Kênh trống.");
          }
          
          updateConnectionStatus(true, "Đang kết nối: ThingSpeak Cloud OK");
          
          historyData.timestamps = [];
          historyData.temperature = [];
          historyData.humidity = [];
          historyData.pressure = [];
          historyData.rain = [];
          historyData.battery = [];
          historyData.predictedTemp = [];
          historyData.rainProbability = [];
          historyData.pressureTrend = [];
          
          const getFirstNonNull = (fieldName, fallback) => {
            for (let i = 0; i < data.feeds.length; i++) {
              const val = data.feeds[i][fieldName];
              if (val !== null && val !== undefined && val !== '') {
                const num = parseFloat(val);
                if (!isNaN(num)) return num;
              }
            }
            return fallback;
          };

          const firstVal = {
            temp: getFirstNonNull('field1', 25),
            hum: getFirstNonNull('field2', 70),
            pres: getFirstNonNull('field3', 1010),
            rain: getFirstNonNull('field4', 1023),
            bat: getFirstNonNull('field5', 100),
            predT: getFirstNonNull('field6', null),
            rainP: getFirstNonNull('field7', null)
          };

          data.feeds.forEach(feed => {
            const date = new Date(feed.created_at);
            const timeStr = `${String(date.getHours()).padStart(2, '0')}:${String(date.getMinutes()).padStart(2, '0')}`;
            
            // Forward Fill Sensor Values
            let temp = (feed.field1 !== null && feed.field1 !== undefined && feed.field1 !== '') ? parseFloat(feed.field1) : null;
            if (temp === null) {
              temp = historyData.temperature.length > 0 ? historyData.temperature[historyData.temperature.length - 1] : firstVal.temp;
            }
            
            let hum = (feed.field2 !== null && feed.field2 !== undefined && feed.field2 !== '') ? parseFloat(feed.field2) : null;
            if (hum === null) {
              hum = historyData.humidity.length > 0 ? historyData.humidity[historyData.humidity.length - 1] : firstVal.hum;
            }
            
            let pres = (feed.field3 !== null && feed.field3 !== undefined && feed.field3 !== '') ? parseFloat(feed.field3) : null;
            if (pres === null) {
              pres = historyData.pressure.length > 0 ? historyData.pressure[historyData.pressure.length - 1] : firstVal.pres;
            }
            
            let rainVal = (feed.field4 !== null && feed.field4 !== undefined && feed.field4 !== '') ? parseInt(feed.field4) : null;
            if (rainVal === null) {
              rainVal = historyData.rain.length > 0 ? historyData.rain[historyData.rain.length - 1] : firstVal.rain;
            }
            
            let bat = (feed.field5 !== null && feed.field5 !== undefined && feed.field5 !== '') ? parseInt(feed.field5) : null;
            if (bat === null) {
              bat = historyData.battery.length > 0 ? historyData.battery[historyData.battery.length - 1] : firstVal.bat;
            }
            
            // Forward Fill Python AI values
            let predT = (feed.field6 !== null && feed.field6 !== undefined && feed.field6 !== '') ? parseFloat(feed.field6) : null;
            if (predT === null) {
              predT = historyData.predictedTemp.length > 0 ? historyData.predictedTemp[historyData.predictedTemp.length - 1] : firstVal.predT;
            }
            
            let rainP = (feed.field7 !== null && feed.field7 !== undefined && feed.field7 !== '') ? parseFloat(feed.field7) : null;
            if (rainP === null) {
              rainP = historyData.rainProbability.length > 0 ? historyData.rainProbability[historyData.rainProbability.length - 1] : firstVal.rainP;
            }
            
            let predStat = (rainP !== null && rainP > 50) ? 2 : (rainP !== null && rainP > 25 ? 1 : 0);
            
            historyData.timestamps.push(timeStr);
            historyData.temperature.push(temp);
            historyData.humidity.push(hum);
            historyData.pressure.push(pres);
            historyData.rain.push(rainVal);
            historyData.battery.push(bat);
            historyData.predictedTemp.push(predT);
            historyData.rainProbability.push(rainP);
            historyData.predictedStatus.push(predStat);
          });
          
          let latestSensorFeed = null;
          for (let i = data.feeds.length - 1; i >= 0; i--) {
            if (data.feeds[i].field1 !== null && data.feeds[i].field1 !== undefined && data.feeds[i].field1 !== '') {
              latestSensorFeed = data.feeds[i];
              break;
            }
          }
          if (!latestSensorFeed) {
            latestSensorFeed = data.feeds[data.feeds.length - 1];
          }

          let latestMatlabFeed = null;
          for (let i = data.feeds.length - 1; i >= 0; i--) {
            if (data.feeds[i].field6 !== null && data.feeds[i].field6 !== undefined && data.feeds[i].field6 !== '') {
              latestMatlabFeed = data.feeds[i];
              break;
            }
          }
          if (!latestMatlabFeed) {
            latestMatlabFeed = data.feeds[data.feeds.length - 1];
          }

          if (latestSensorFeed && latestSensorFeed.field1 !== null && latestSensorFeed.field1 !== undefined && latestSensorFeed.field1 !== '') {
            currentData.temperature = parseFloat(latestSensorFeed.field1);
          }
          if (latestSensorFeed && latestSensorFeed.field2 !== null && latestSensorFeed.field2 !== undefined && latestSensorFeed.field2 !== '') {
            currentData.humidity = parseFloat(latestSensorFeed.field2);
          }
          if (latestSensorFeed && latestSensorFeed.field3 !== null && latestSensorFeed.field3 !== undefined && latestSensorFeed.field3 !== '') {
            currentData.pressure = parseFloat(latestSensorFeed.field3);
          }
          if (latestSensorFeed && latestSensorFeed.field4 !== null && latestSensorFeed.field4 !== undefined && latestSensorFeed.field4 !== '') {
            currentData.rain = parseInt(latestSensorFeed.field4);
          }
          if (latestSensorFeed && latestSensorFeed.field5 !== null && latestSensorFeed.field5 !== undefined && latestSensorFeed.field5 !== '') {
            currentData.battery = parseInt(latestSensorFeed.field5);
          }
          
          currentData.predictedTemp = latestMatlabFeed.field6 !== null ? parseFloat(latestMatlabFeed.field6) : null;
          const latestRainP = latestMatlabFeed.field7 !== null ? parseFloat(latestMatlabFeed.field7) : null;
          currentData.rainProbability = latestRainP;
          currentData.predictedStatus = (latestRainP !== null && latestRainP > 50) ? 2 : (latestRainP !== null && latestRainP > 25 ? 1 : 0);
          
          if (latestSensorFeed) lastSensorTime = new Date(latestSensorFeed.created_at);
          if (latestMatlabFeed) lastAiTime = new Date(latestMatlabFeed.created_at);

          currentData.rssi = null; 
          currentData.seqNum = null;
          
          const lastUpdateDate = new Date(data.feeds[data.feeds.length - 1].created_at);
          currentData.lastUpdated = lastUpdateDate.toLocaleTimeString('vi-VN');
          
          updateDualTimestampDisplay();
          updateUI();
          updateChartData();
          runLinearRegression();

        })
        .catch(err => {
          console.warn("Không kết nối được ThingSpeak, chạy mô phỏng...", err);
          updateConnectionStatus(false, "Không lấy được dữ liệu Cloud. Đang chạy mô phỏng.");
          generateMockData();
        })
        .finally(() => {
          isFetching = false;
        });
    }


    function updateConnectionStatus(isOnline, text) {
      const dot = document.getElementById('statusDot');
      const txt = document.getElementById('statusText');
      if (isOnline) {
        if (dot) dot.className = "status-dot online";
      } else {
        if (dot) dot.className = "status-dot offline";
      }
      if (txt) txt.innerText = text;
    }

    function updateDualTimestampDisplay() {
      const now = new Date();
      
      const sensorEl = document.getElementById('sensorAgoText');
      if (sensorEl && lastSensorTime) {
        const diffS = Math.floor((now - lastSensorTime) / 1000);
        const stale = diffS > 300;
        sensorEl.innerText = diffS < 60 ? `${diffS} giây trước` : `${Math.floor(diffS/60)} phút trước`;
        sensorEl.style.color = stale ? '#ef4444' : '';
        const tempDisplay = document.getElementById('mainTemp');
        if (tempDisplay) tempDisplay.style.opacity = stale ? '0.45' : '1';
        const statusDot = document.getElementById('statusDot');
        if (stale && statusDot) statusDot.className = 'status-dot offline';
        else if (!stale && statusDot) statusDot.className = 'status-dot online';
      } else if (sensorEl) {
        sensorEl.innerText = 'chưa nhận được';
      }
      
      const aiEl = document.getElementById('aiAgoText');
      if (aiEl && lastAiTime) {
        const diffS = Math.floor((now - lastAiTime) / 1000);
        aiEl.innerText = diffS < 60 ? `${diffS} giây trước` : `${Math.floor(diffS/60)} phút trước`;
      } else if (aiEl) {
        aiEl.innerText = 'mô hình chưa chạy';
      }
      
      const statusText = document.getElementById('statusText');
      if (statusText && lastSensorTime) {
        const diffS = Math.floor((now - lastSensorTime) / 1000);
        if (diffS > 300) {
          statusText.innerText = `Dữ liệu cũ · ${Math.floor(diffS/60)} phút trước`;
        } else {
          statusText.innerText = `Trực tuyến · cập nhật ${diffS < 60 ? diffS + 'giây' : Math.floor(diffS/60) + ' phút'} trước`;
        }
      }
    }

    function generateMockData() {
      const baseTemp = 29.5 + Math.sin(Date.now() / 60000) * 2;
      const baseHum = 75 + Math.cos(Date.now() / 60000) * 8;
      const basePres = 1008 + Math.sin(Date.now() / 120000) * 3;
      const rainRand = Math.random() > 0.85 ? 250 : 1023;
      
      currentData.temperature = parseFloat(baseTemp.toFixed(1));
      currentData.humidity = parseFloat(baseHum.toFixed(1));
      currentData.pressure = parseFloat(basePres.toFixed(1));
      currentData.rain = rainRand;
      currentData.battery = 98;
      
      currentData.predictedTemp = parseFloat((currentData.temperature + 0.4 - Math.random() * 0.8).toFixed(1));
      currentData.rainProbability = rainRand < 500 ? 95 : 15;
      currentData.predictedStatus = rainRand < 500 ? 2 : (baseHum > 78 ? 1 : 0);
      
      currentData.rssi = -68 - Math.floor(Math.random() * 8);
      currentData.seqNum = (currentData.seqNum + 1) % 256;
      currentData.lastUpdated = new Date().toLocaleTimeString('vi-VN');
      
      lastSensorTime = new Date();
      lastAiTime = new Date();
      
      addLocalHistory(currentData);
      updateUI();
      updateDualTimestampDisplay();
      updateChartData();
      runLinearRegression();
    }


    function addLocalHistory(data) {
      const now = new Date();
      const timeStr = `${String(now.getHours()).padStart(2, '0')}:${String(now.getMinutes()).padStart(2, '0')}:${String(now.getSeconds()).padStart(2, '0')}`;
      
      historyData.timestamps.push(timeStr);
      historyData.temperature.push(data.temperature);
      historyData.humidity.push(data.humidity);
      historyData.pressure.push(data.pressure);
      historyData.rain.push(data.rain);
      historyData.battery.push(data.battery);
      historyData.predictedTemp.push(data.predictedTemp);
      historyData.rainProbability.push(data.rainProbability);
      historyData.predictedStatus.push(data.predictedStatus);
      
      if (historyData.timestamps.length > 180) {
        historyData.timestamps.shift();
        historyData.temperature.shift();
        historyData.humidity.shift();
        historyData.pressure.shift();
        historyData.rain.shift();
        historyData.battery.shift();
        historyData.predictedTemp.shift();
        historyData.rainProbability.shift();
        historyData.predictedStatus.shift();
      }
    }

    // CẬP NHẬT GIAO DIỆN NGƯỜI DÙNG (UI)
    function updateUI() {
      document.getElementById('mainTemp').innerText = currentData.temperature.toFixed(1);
      
      let summary = 'Không khí dễ chịu 🌤️';
      let statusTag = 'Hệ thống an toàn';
      let iconHtml = '';
      
      const isRaining = currentData.rain < 500;
      
      if (isRaining) {
        summary = 'Hiện có mưa dông khí quyển 🌧️. Bà con canh tác lưu ý rào chắn, kiểm tra hệ thống thoát nước chống úng rễ cà phê, tiêu và ngưng bón phân.';
        statusTag = 'Mưa dông úng ngập';
        iconHtml = `
          <svg viewBox="0 0 64 64" width="80" height="80" class="weather-icon-svg sun-cloud-anim" style="filter: drop-shadow(0 6px 12px rgba(56, 189, 248, 0.4))">
            <defs>
              <linearGradient id="cloudGrad" x1="0%" y1="0%" x2="100%" y2="100%">
                <stop offset="0%" stop-color="#94a3b8" />
                <stop offset="100%" stop-color="#475569" />
              </linearGradient>
            </defs>
            <path d="M46 38a10 10 0 0 0-9-6 10.5 10.5 0 0 0-4 .8A12 12 0 1 0 22 50h24a10 10 0 0 0 0-20z" fill="url(#cloudGrad)"/>
            <line x1="20" y1="54" x2="18" y2="60" stroke="#38bdf8" stroke-width="3" stroke-linecap="round"/>
            <line x1="28" y1="54" x2="26" y2="60" stroke="#38bdf8" stroke-width="3" stroke-linecap="round"/>
            <line x1="36" y1="54" x2="34" y2="60" stroke="#38bdf8" stroke-width="3" stroke-linecap="round"/>
          </svg>
        `;
        showAlertBox(true, "⚠ Cảnh báo mưa úng! Kiểm tra ngay các rãnh thoát nước vườn sầu riêng Miền Tây và cà phê trũng.");
      } else {
        showAlertBox(false);
        
        if (currentData.temperature > 34) {
          summary = 'Thời tiết nắng nóng gay gắt 🥵. Hãy tăng cường tưới ẩm đất rễ cây, đặc biệt là cây con và cà phê mới xuống giống để tránh héo lá.';
          statusTag = 'Nắng nóng canh tác';
          iconHtml = `
            <svg viewBox="0 0 64 64" width="80" height="80" class="weather-icon-svg sun-cloud-anim">
              <circle cx="32" cy="32" r="16" fill="url(#sunGradient)"/>
              <line x1="32" y1="6" x2="32" y2="12" stroke="#f59e0b" stroke-width="4" stroke-linecap="round"/>
              <line x1="32" y1="52" x2="32" y2="58" stroke="#f59e0b" stroke-width="4" stroke-linecap="round"/>
              <line x1="6" y1="32" x2="12" y2="32" stroke="#f59e0b" stroke-width="4" stroke-linecap="round"/>
              <line x1="52" y1="32" x2="58" y2="32" stroke="#f59e0b" stroke-width="4" stroke-linecap="round"/>
            </svg>
          `;
        } else if (currentData.humidity > 85) {
          summary = 'Độ ẩm không khí rất cao ☁️. Đề phòng nguy cơ phát triển các loại nấm ký sinh có hại (rỉ sắt, nấm hồng). Hạn chế phun phân bón lá lúc này.';
          statusTag = 'Cảnh báo dịch hại';
          iconHtml = `
            <svg viewBox="0 0 64 64" width="80" height="80" class="weather-icon-svg sun-cloud-anim">
              <path d="M46 38a10 10 0 0 0-9-6 10.5 10.5 0 0 0-4 .8A12 12 0 1 0 22 50h24a10 10 0 0 0 0-20z" fill="url(#cloudGradient)"/>
            </svg>
          `;
        } else {
          summary = 'Thời tiết nắng ráo thuận lợi 🌤️. Thích hợp để tiến hành bón phân dinh dưỡng, làm cỏ đất đai, tỉa cành hoặc phơi sấy hạt nông sản.';
          statusTag = 'Canh tác tốt';
          iconHtml = `
            <svg viewBox="0 0 64 64" width="80" height="80" class="weather-icon-svg sun-cloud-anim">
              <circle cx="24" cy="24" r="12" fill="url(#sunGradient)"/>
              <path d="M46 38a10 10 0 0 0-9-6 10.5 10.5 0 0 0-4 .8A12 12 0 1 0 22 50h24a10 10 0 0 0 0-20z" fill="url(#cloudGradient)" opacity="0.95"/>
            </svg>
          `;
        }
      }
      
      document.getElementById('weatherSummary').innerText = summary;
      document.getElementById('weatherStatusTag').innerText = statusTag;
      document.getElementById('weatherIconContainer').innerHTML = iconHtml;
      
      // Cập nhật giá trị Hero banner
      if (currentData.predictedTemp !== null) {
        document.getElementById('subTemp').innerText = `${currentData.predictedTemp.toFixed(1)} °C`;
      } else {
        document.getElementById('subTemp').innerText = `-- °C`;
      }
      
      if (currentData.rainProbability !== null) {
        document.getElementById('subRain').innerText = `${currentData.rainProbability.toFixed(0)}%`;
      } else {
        document.getElementById('subRain').innerText = `--%`;
      }

      if (currentData.predictedStatus !== null) {
        const statusMap = {0: "Nắng ráo ☀️", 1: "Nhiều mây ☁️", 2: "Mưa dông 🌧️"};
        const iconMap = {0: "☀️", 1: "☁️", 2: "🌧️"};
        document.getElementById('aiStatusText').innerText = statusMap[currentData.predictedStatus] || "Chưa rõ";
        document.getElementById('aiStatusIcon').innerText = iconMap[currentData.predictedStatus] || "🤖";
      } else {
        document.getElementById('aiStatusText').innerText = "Đang tính...";
        document.getElementById('aiStatusIcon').innerText = "🤖";
      }

      // Cập nhật Cards phía dưới
      document.getElementById('valHumidity').innerText = currentData.humidity.toFixed(1);
      document.getElementById('barHumidity').style.width = `${currentData.humidity}%`;
      
      document.getElementById('valPressure').innerText = currentData.pressure.toFixed(1);

      const pressBadge = document.getElementById('badgePressure');
      if (currentData.pressure < 1009) {
        pressBadge.innerText = "Áp suất thấp";
        pressBadge.style.color = "#ef4444";
        pressBadge.style.backgroundColor = "rgba(239, 68, 68, 0.12)";
      } else if (currentData.pressure > 1014) {
        pressBadge.innerText = "Áp suất cao";
        pressBadge.style.color = "#10b981";
        pressBadge.style.backgroundColor = "rgba(16, 185, 129, 0.12)";
      } else {
        pressBadge.innerText = "Bình thường";
        pressBadge.style.color = "#a855f7";
        pressBadge.style.backgroundColor = "rgba(168, 85, 247, 0.12)";
      }
      
      document.getElementById('valRain').innerText = currentData.rain;
      const rainStatusEl = document.getElementById('statusRain');
      if (currentData.rain < 300) {
        rainStatusEl.innerText = "🌧️ Mưa lớn";
        rainStatusEl.style.color = "#ef4444";
      } else if (currentData.rain < 700) {
        rainStatusEl.innerText = "🌦️ Mưa nhỏ / Phun";
        rainStatusEl.style.color = "#eab308";
      } else {
        rainStatusEl.innerText = "☀️ Không mưa";
        rainStatusEl.style.color = "#10b981";
      }
      
      document.getElementById('valBattery').innerText = currentData.battery;
      const batBar = document.getElementById('barBattery');
      batBar.style.width = `${currentData.battery}%`;
      if (currentData.battery < 20) {
        batBar.style.background = "var(--error)";
      } else if (currentData.battery < 50) {
        batBar.style.background = "var(--warning)";
      } else {
        batBar.style.background = "var(--accent-bat-gradient)";
      }
      
      const rssiEl = document.getElementById('rssiValue');
      if (rssiEl) rssiEl.innerText = currentData.rssi !== null ? `${currentData.rssi} dBm` : "--";
      
      const seqEl = document.getElementById('seqNum');
      if (seqEl) seqEl.innerText = currentData.seqNum !== null ? currentData.seqNum : "--";
      
      const updatedEl = document.getElementById('lastUpdated');
      if (updatedEl) updatedEl.innerText = currentData.lastUpdated || "--";
      
      updateAiForecastCards();
    }

    function updateAiForecastCards() {
      const t0 = currentData.temperature || 25.0;
      const t1 = currentData.predictedTemp !== null ? currentData.predictedTemp : t0;
      
      const effSlopeT = t1 - t0;
      const t3 = t1 + 2 * effSlopeT;
      const t6 = t1 + 5 * effSlopeT;
      const t12 = t1 + 11 * effSlopeT;
      
      const forecastTemps = [t1, t3, t6, t12].map(t => Math.max(12, Math.min(42, t)));
      
      const p0 = currentData.rainProbability !== null ? currentData.rainProbability : (currentData.rain < 500 ? 80 : 10);
      const p1 = currentData.rainProbability !== null ? currentData.rainProbability : p0;
      
      let slopeH = 0;
      if (historyData.humidity.length >= 2) {
        const regHum = calculateLinearRegression(historyData.humidity);
        slopeH = regHum.a || 0;
      }
      const effSlopeP = (p1 - p0) || (slopeH > 0 ? 3 : -3);
      
      const p3 = Math.max(0, Math.min(100, p1 + 2 * effSlopeP));
      const p6 = Math.max(0, Math.min(100, p1 + 5 * effSlopeP));
      const p12 = Math.max(0, Math.min(100, p1 + 11 * effSlopeP));
      
      const forecastProbs = [p1, p3, p6, p12];
      const icons = forecastProbs.map(p => p > 50 ? '🌧️' : (p > 25 ? '☁️' : '☀️'));
      
      for (let i = 0; i < 4; i++) {
        const idx = i + 1;
        const tempEl = document.getElementById(`aiFCardTemp${idx}`);
        const rainEl = document.getElementById(`aiFCardRain${idx}`);
        const iconEl = document.getElementById(`aiFCardIcon${idx}`);
        
        if (tempEl) tempEl.innerText = `${forecastTemps[i].toFixed(1)}°C`;
        if (rainEl) rainEl.innerText = `${forecastProbs[i].toFixed(0)}% mưa`;
        if (iconEl) iconEl.innerText = icons[i];
      }
    }



    function showAlertBox(show, msg = '') {
      const box = document.getElementById('alertBox');
      const msgEl = document.getElementById('alertMessage');
      if (show) {
        msgEl.innerText = msg;
        box.classList.remove('hidden');
      } else {
        box.classList.add('hidden');
      }
    }

    // THUẬT TOÁN MÁY HỌC HỒI QUY TUYẾN TÍNH (LINEAR REGRESSION ENGINE)
    function calculateLinearRegression(yArr) {
      const n = yArr.length;
      if (n < 2) return { a: 0, b: 0, formula: 'y = --', predictNext: 0 };
      
      const xArr = Array.from({ length: n }, (_, i) => i + 1);
      
      let sumX = 0;
      let sumY = 0;
      let sumXY = 0;
      let sumXX = 0;
      
      for (let i = 0; i < n; i++) {
        sumX += xArr[i];
        sumY += yArr[i];
        sumXY += xArr[i] * yArr[i];
        sumXX += xArr[i] * xArr[i];
      }
      
      const denominator = (n * sumXX) - (sumX * sumX);
      if (denominator === 0) return { a: 0, b: yArr[0], formula: `y = ${yArr[0]}`, predictNext: yArr[0] };
      
      const a = ((n * sumXY) - (sumX * sumY)) / denominator;
      const b = (sumY - (a * sumX)) / n;
      
      const nextX = n + 1;
      const predictNext = (a * nextX) + b;
      
      return { a, b, predictNext };
    }

    function runLinearRegression() {
      const n = historyData.temperature.length;
      if (n < 5) {
        document.getElementById('systemRecommendation').innerText = `Chờ thu thập đủ dữ liệu khí tượng (Đã có ${n}/5 điểm dữ liệu)...`;
        return;
      }
      
      const regTemp = calculateLinearRegression(historyData.temperature);
      const regHum = calculateLinearRegression(historyData.humidity);
      const regPres = calculateLinearRegression(historyData.pressure);
      
      document.getElementById('mathTemp').innerText = `T(t) = ${regTemp.a.toFixed(2)} * t ${regTemp.b >= 0 ? '+' : '-'} ${Math.abs(regTemp.b).toFixed(2)}`;
      document.getElementById('mathHum').innerText = `H(t) = ${regHum.a.toFixed(2)} * t ${regHum.b >= 0 ? '+' : '-'} ${Math.abs(regHum.b).toFixed(2)}`;
      document.getElementById('mathPres').innerText = `P(t) = ${regPres.a.toFixed(2)} * t ${regPres.b >= 0 ? '+' : '-'} ${Math.abs(regPres.b).toFixed(2)}`;
      
      document.getElementById('predTemp').innerText = `${regTemp.predictNext.toFixed(1)} °C`;
      document.getElementById('predHum').innerText = `${regHum.predictNext.toFixed(1)} %`;
      document.getElementById('predPres').innerText = `${regPres.predictNext.toFixed(1)} hPa`;
      
      updateTrendBadge('trendTemp', regTemp.a, 0.05);
      updateTrendBadge('trendHum', regHum.a, 0.1);
      updateTrendBadge('trendPres', regPres.a, 0.05);
      
      const signPres = regPres.a >= 0 ? '+' : '';
      document.getElementById('subPressure').innerText = `Trend: ${signPres}${regPres.a.toFixed(3)}`;
      
      // Cập nhật Hero Banner chính khi chạy Offline Local
      if (config.dataSource === 'local') {
        document.getElementById('subTemp').innerText = `${regTemp.predictNext.toFixed(1)} °C`;
        
        // Ước lượng xác suất mưa cục bộ từ xu hướng khí áp và lượng mưa analog
        let localRainProb = 20;
        if (regPres.a < -0.2) {
          localRainProb = 80;
        } else if (regPres.a < -0.05) {
          localRainProb = 50;
        } else if (regPres.a > 0.1) {
          localRainProb = 5;
        } else {
          localRainProb = 20;
        }
        if (currentData.rain < 500) {
          localRainProb = Math.max(localRainProb, 75);
        }
        document.getElementById('subRain').innerText = `${localRainProb.toFixed(0)}%`;
        
        // Cập nhật nhãn trạng thái dự đoán động
        let localPredStatus = 0;
        if (localRainProb > 50) {
          localPredStatus = 2; // Mưa
        } else if (localRainProb > 25) {
          localPredStatus = 1; // Nhiều mây
        }
        
        const statusMap = {0: "Nắng ráo ☀️", 1: "Nhiều mây ☁️", 2: "Mưa dông 🌧️"};
        const iconMap = {0: "☀️", 1: "☁️", 2: "🌧️"};
        document.getElementById('aiStatusText').innerText = statusMap[localPredStatus] || "Chưa rõ";
        document.getElementById('aiStatusIcon').innerText = iconMap[localPredStatus] || "🤖";
        
        currentData.predictedTemp = regTemp.predictNext;
        currentData.rainProbability = localRainProb;
        currentData.predictedStatus = localPredStatus;
      }
      
      generateSmartRecommendation(regTemp.a, regHum.a, regPres.a);
    }

    function updateTrendBadge(elemId, slope, threshold) {
      const badge = document.getElementById(elemId);
      if (slope > threshold) {
        badge.innerText = "Tăng 📈";
        badge.className = "pred-trend trend-up";
      } else if (slope < -threshold) {
        badge.innerText = "Giảm 📉";
        badge.className = "pred-trend trend-down";
      } else {
        badge.innerText = "Ổn định ➔";
        badge.className = "pred-trend trend-stable";
      }
    }

    function generateSmartRecommendation(slopeT, slopeH, slopeP) {
      let rec = "";
      if (slopeT < -0.05 && slopeH > 0.1) {
        rec = "Khuyến nghị canh tác: Nhiệt độ giảm nhanh và độ ẩm tăng mạnh. Khả năng cao có mưa rào hoặc dông lốc trong 30-60 phút tới. Bà con nên ngưng ngay việc bón phân, thu hồi nông sản đang phơi sấy (cà phê, lúa) và rà soát hệ thống tiêu úng.";
      } else if (slopeP < -0.05 && currentData.rain < 900) {
        rec = "Khuyến nghị canh tác: Khí áp giảm kèm lượng mưa tích tụ. Dấu hiệu của áp thấp nhiệt đới hoặc mưa dông kéo dài. Cảnh báo nguy cơ thối rễ cây ăn trái ở Miền Tây. Hãy gia cố bờ bao vườn sầu riêng, cam sành để chủ động chống ngập úng.";
      } else if (slopeT > 0.05 && slopeH < -0.1) {
        rec = "Khuyến nghị canh tác: Nhiệt độ tăng và độ ẩm giảm dần. Trời nắng ấm tạnh ráo. Thời tiết rất thích hợp để bón phân thúc nông sản, tiến hành phun thuốc phòng trừ sâu bệnh hoặc phơi sấy hạt tiêu, hạt cà phê.";
      } else if (Math.abs(slopeT) <= 0.05 && Math.abs(slopeH) <= 0.1) {
        rec = "Khuyến nghị canh tác: Khí áp và độ ẩm không khí rất ổn định. Không phát hiện biến động thời tiết cực đoan trong 2 giờ tới. Điều kiện lý tưởng để cấy lúa, xuống giống cây trồng mới hoặc cắt tỉa cành tạo tán.";
      } else {
        rec = "Nhận định khuyến nông: Chỉ số nhiệt ẩm dao động ổn định trong biên độ canh tác an toàn. Bà con duy trì lượng nước tưới tiêu buổi sáng/chiều và chăm sóc cây trồng theo lịch định kỳ.";
      }
      document.getElementById('systemRecommendation').innerText = rec;
    }

    function switchMlTab(event, tabName) {
      if (event) {
        event.preventDefault();
        const header = event.currentTarget.parentElement;
        header.querySelectorAll('.ml-tab').forEach(btn => btn.classList.remove('active'));
        event.currentTarget.classList.add('active');
      }
      
      const card = document.querySelector('.matlab-chart-card');
      card.querySelector('#mlTabInfo').classList.add('hidden');
      card.querySelector('#mlTabImportance').classList.add('hidden');
      card.querySelector('#mlTabMatrix').classList.add('hidden');
      
      if (tabName === 'info') {
        card.querySelector('#mlTabInfo').classList.remove('hidden');
      } else if (tabName === 'importance') {
        card.querySelector('#mlTabImportance').classList.remove('hidden');
      } else if (tabName === 'matrix') {
        card.querySelector('#mlTabMatrix').classList.remove('hidden');
      }
    }

    // TOOLTIP FOR SVG CHART
    window.showChartTooltip = function(evt, label, val, time) {
      let tooltip = document.getElementById('chart-tooltip');
      if (!tooltip) {
        tooltip = document.createElement('div');
        tooltip.id = 'chart-tooltip';
        tooltip.style.position = 'absolute';
        tooltip.style.background = 'rgba(15, 15, 17, 0.95)';
        tooltip.style.border = '1px solid rgba(255, 255, 255, 0.08)';
        tooltip.style.borderRadius = '6px';
        tooltip.style.padding = '6px 10px';
        tooltip.style.color = '#fff';
        tooltip.style.fontSize = '11px';
        tooltip.style.fontFamily = 'Sora, sans-serif';
        tooltip.style.pointerEvents = 'none';
        tooltip.style.boxShadow = '0 4px 12px rgba(0,0,0,0.5)';
        tooltip.style.zIndex = '1000';
        tooltip.style.display = 'none';
        document.body.appendChild(tooltip);
      }
      
      tooltip.innerHTML = `
        <div style="font-weight: 600; color: #a0a0ab; margin-bottom: 2px;">Thời gian: ${time}</div>
        <div style="display: flex; align-items: center; gap: 6px;">
          <span style="font-weight: 700; color: #fff;">${label}: ${val}</span>
        </div>
      `;
      tooltip.style.display = 'block';
      moveChartTooltip(evt);
    };

    window.moveChartTooltip = function(evt) {

      const tooltip = document.getElementById('chart-tooltip');
      if (tooltip) {
        tooltip.style.left = (evt.pageX + 12) + 'px';
        tooltip.style.top = (evt.pageY - 35) + 'px';
      }
    };

    window.hideChartTooltip = function() {
      const tooltip = document.getElementById('chart-tooltip');
      if (tooltip) {
        tooltip.style.display = 'none';
      }
    };

    window.exportDataCSV = function() {
      if (!historyData.timestamps || historyData.timestamps.length === 0) {
        alert("Chưa có dữ liệu lịch sử để xuất file!");
        return;
      }
      let csvContent = "data:text/csv;charset=utf-8,";
      csvContent += "Timestamp,Temperature,Humidity,Pressure,Rain,Battery,Predicted_Temp,Rain_Probability,Predicted_Status\n";
      for (let i = 0; i < historyData.timestamps.length; i++) {
        let row = [
          historyData.timestamps[i],
          historyData.temperature[i],
          historyData.humidity[i],
          historyData.pressure[i],
          historyData.rain[i],
          historyData.battery[i],
          historyData.predictedTemp[i],
          historyData.rainProbability[i],
          historyData.predictedStatus[i]
        ].join(",");
        csvContent += row + "\n";
      }
      const encodedUri = encodeURI(csvContent);
      const link = document.createElement("a");
      link.setAttribute("href", encodedUri);
      link.setAttribute("download", "weather_station_export.csv");
      document.body.appendChild(link);
      link.click();
      document.body.removeChild(link);
    };

  </script>
</body>
</html>
)rawliteral";

#endif
