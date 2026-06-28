import re

def compile_html():
    print("Reading base HTML template from 'data/viewWeatherStation.html'...")
    # Read viewWeatherStation.html (which has clean, correct Vietnamese)
    with open("data/viewWeatherStation.html", "r", encoding="utf-8") as f:
        html = f.read()

    # 1. Remove external network dependencies that cause rendering delays/timeouts when offline
    external_links = [
        '<link rel="preconnect" href="https://fonts.googleapis.com">',
        '<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>',
        '<link href="https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600;700;800&family=Sora:wght@300;400;500;600;700;800&display=swap" rel="stylesheet">',
        '<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>'
    ]

    for link in external_links:
        html = html.replace(link, "")

    # 2. Modify CSS font variables in :root for system font fallback stack
    html = html.replace("--font-display: 'Sora', sans-serif;", "--font-display: 'Sora', -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;")
    html = html.replace("--font-body: 'Inter', sans-serif;", "--font-body: 'Inter', -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;")

    # 3. Add mobile-responsive CSS overrides to the end of the <style> block
    responsive_css = """
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

"""

    # Append the responsive CSS before the closing </style> tag
    html = html.replace("</style>", responsive_css)

    # 4. Replace <canvas id="weatherHistoryChart"></canvas> with a <div> container for the SVG chart
    html = html.replace('<canvas id="weatherHistoryChart"></canvas>', '<div id="weatherHistoryChart" style="width: 100%; height: 100%;"></div>')

    # 5. Inject clean, offline Javascript code
    js_code = """
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

    // KHỞI CHẠY TRANG
    document.addEventListener("DOMContentLoaded", () => {
      loadConfig();
      initChart();
      startDataFetching();
      refreshData();
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
        btnCloud.classList.add('active');
        btnLocal.classList.remove('active');
        stationModeText.innerText = `ThingSpeak Channel: ${config.channelId}`;
      } else {
        btnCloud.classList.remove('active');
        btnLocal.classList.add('active');
        stationModeText.innerText = `ESP8266 Local Server: http://${config.espIp}`;
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
      if (timestamps.length > 1) {
        let timeDiffs = [];
        for (let i = 1; i < timestamps.length; i++) {
          let [h1, m1] = timestamps[i-1].split(':').map(Number);
          let [h2, m2] = timestamps[i].split(':').map(Number);
          let diff = (h2 * 60 + m2) - (h1 * 60 + m1);
          if (diff < 0) diff += 1440;
          if (diff > 0 && diff < 60) timeDiffs.push(diff);
        }
        let avgDiff = timeDiffs.length > 0 ? timeDiffs.reduce((a,b)=>a+b, 0)/timeDiffs.length : 6.0;
        shiftOffset = Math.max(1, Math.round(60 / avgDiff));
      }

      let extendedTimestamps = [...timestamps];
      if (timestamps.length > 0) {
        let lastTimeStr = timestamps[timestamps.length - 1];
        let [hh, mm] = lastTimeStr.split(':').map(Number);
        let avgInterval = timestamps.length > 1 ? 60 / shiftOffset : 6.0;
        for(let i = 1; i <= shiftOffset; i++) {
          let totalMinutes = Math.round(mm + i * avgInterval);
          let n_mm = totalMinutes % 60;
          let n_hh = (hh + Math.floor(totalMinutes / 60)) % 24;
          extendedTimestamps.push(`${String(n_hh).padStart(2, '0')}:${String(n_mm).padStart(2, '0')}`);
        }
      }

      const totalPoints = timestamps.length + shiftOffset;
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
          
          if (isNewPacket) {
            lastReceivedSeqNum = newSeqNum;
            addLocalHistory(currentData);
            runLinearRegression();
          }
          updateUI();
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
      
      let url = `https://api.thingspeak.com/channels/${config.channelId}/feeds.json?results=100`;
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
          
          currentData.rssi = null; 
          currentData.seqNum = null;
          
          const lastUpdateDate = new Date(data.feeds[data.feeds.length - 1].created_at);
          currentData.lastUpdated = lastUpdateDate.toLocaleTimeString('vi-VN');
          
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
        dot.className = "status-dot online";
      } else {
        dot.className = "status-dot offline";
      }
      txt.innerText = text;
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
      
      addLocalHistory(currentData);
      updateUI();
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
      
      document.getElementById('rssiValue').innerText = currentData.rssi !== null ? `${currentData.rssi} dBm` : "--";
      document.getElementById('seqNum').innerText = currentData.seqNum !== null ? currentData.seqNum : "--";
      document.getElementById('lastUpdated').innerText = currentData.lastUpdated || "--";
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
      csvContent += "Timestamp,Temperature,Humidity,Pressure,Rain,Battery,Predicted_Temp,Rain_Probability,Predicted_Status\\\\n";
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
        csvContent += row + "\\\\n";
      }
      const encodedUri = encodeURI(csvContent);
      const link = document.createElement("a");
      link.setAttribute("href", encodedUri);
      link.setAttribute("download", "weather_station_export.csv");
      document.body.appendChild(link);
      link.click();
      document.body.removeChild(link);
    };
"""

    # Replace the script block inside html
    html_fixed = re.sub(r'<script>(.*?)</script>', f'<script>\\n{js_code}\\n  </script>', html, flags=re.DOTALL)

    # 6. Save the optimized single-page HTML directly to root index.html
    print("Writing optimized HTML dashboard to 'index.html' at workspace root...")
    with open("index.html", "w", encoding="utf-8") as f:
        f.write(html_fixed)

    # 7. Wrap it in weather_html.h using a C++ raw string literal
    header_content = f"""#ifndef WEATHER_HTML_H
#define WEATHER_HTML_H

const char htmlPage[] PROGMEM = R"rawliteral(
{html_fixed}
)rawliteral";

#endif
"""

    print("Writing C++ Header wrapper to 'gateway_node/weather_html.h'...")
    # Save to weather_html.h
    with open("gateway_node/weather_html.h", "w", encoding="utf-8") as f:
        f.write(header_content)

    print("Compilation completed successfully! Both 'index.html' and 'gateway_node/weather_html.h' are up to date.")

if __name__ == "__main__":
    compile_html()



