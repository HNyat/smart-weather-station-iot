import React, { useState, useEffect, useRef } from 'react';
import Header from './components/Header';
import MetaBar from './components/MetaBar';
import HeroBanner from './components/HeroBanner';
import MetricCard from './components/MetricCard';
import MLPanel from './components/MLPanel';
import ChartPanel from './components/ChartPanel';
import LocalEngine from './components/LocalEngine';
import SettingsModal from './components/SettingsModal';

// Default configuration settings
const DEFAULT_CONFIG = {
  channelId: '3413241',
  readApiKey: 'VISWQT7BABDVZOFS',
  matlabId: '674169',
  espIp: '192.168.4.1',
  dataSource: 'cloud'
};

export default function App() {
  const [config, setConfig] = useState(DEFAULT_CONFIG);
  const [isSettingsOpen, setIsSettingsOpen] = useState(false);
  const [isOnline, setIsOnline] = useState(false);
  const [connectionText, setConnectionText] = useState('Đang kết nối...');
  const [activeChartTab, setActiveChartTab] = useState('temp');
  const [relayState, setRelayState] = useState(false);

  const [currentData, setCurrentData] = useState({
    temperature: null,
    humidity: null,
    pressure: null,
    rain: null,
    battery: null,
    predictedTemp: null,
    predictedHumidity: null,
    predictedPressure: null,
    rainProbability: null,
    predictedStatus: null,
    rssi: -70,
    seqNum: 0,
    lastUpdated: null
  });

  const [historyData, setHistoryData] = useState({
    timestamps: [],
    temperature: [],
    humidity: [],
    pressure: [],
    rain: [],
    battery: [],
    predictedTemp: [],
    rainProbability: [],
    predictedStatus: []
  });

  // Keep latest history state in a ref to avoid stale closure in polling intervals
  const historyRef = useRef(historyData);
  useEffect(() => {
    historyRef.current = historyData;
  }, [historyData]);

  // Load config from localStorage on boot
  useEffect(() => {
    const savedConfig = localStorage.getItem('weather_dashboard_config');
    if (savedConfig) {
      try {
        setConfig(JSON.parse(savedConfig));
      } catch (e) {
        console.error("Lỗi khi tải cấu hình localStorage:", e);
      }
    } else {
      localStorage.setItem('weather_dashboard_config', JSON.stringify(DEFAULT_CONFIG));
    }
  }, []);

  // Set up polling interval based on dataSource
  useEffect(() => {
    const intervalTime = config.dataSource === 'local' ? 2000 : 16000;
    
    // Initial fetch
    refreshData();
    
    const intervalId = setInterval(refreshData, intervalTime);
    return () => clearInterval(intervalId);
  }, [config.dataSource, config.channelId, config.readApiKey, config.espIp]);

  const refreshData = () => {
    if (config.dataSource === 'local') {
      fetchFromLocalESP();
    } else {
      fetchFromThingSpeak();
    }
  };

  const handleToggleRelay = () => {
    const newState = !relayState;
    setRelayState(newState);
    
    if (config.dataSource === 'local') {
      const url = `http://${config.espIp}/control?relay=${newState ? 1 : 0}`;
      fetch(url)
        .then(res => {
          if (!res.ok) throw new Error("Không kết nối được gateway");
          return res.json();
        })
        .then(data => {
          console.log("Cập nhật trạng thái relay từ gateway:", data);
          setRelayState(data.relay === 1);
        })
        .catch(err => {
          console.error("Lỗi điều khiển relay cục bộ:", err);
          alert("Lỗi: Không kết nối được Gateway ESP8266 để điều khiển máy bơm!");
          setRelayState(!newState); // rollback
        });
    } else {
      // Cloud mode: simulation with alert
      alert(`[Chế độ Cloud] Đang mô phỏng gửi tín hiệu điều khiển máy bơm: ${newState ? 'BẬT' : 'TẮT'}`);
    }
  };

  const addLocalHistory = (data) => {
    const now = new Date();
    const timeStr = `${String(now.getHours()).padStart(2, '0')}:${String(now.getMinutes()).padStart(2, '0')}:${String(now.getSeconds()).padStart(2, '0')}`;
    
    setHistoryData(prev => {
      const updated = {
        timestamps: [...prev.timestamps, timeStr],
        temperature: [...prev.temperature, data.temperature],
        humidity: [...prev.humidity, data.humidity],
        pressure: [...prev.pressure, data.pressure],
        rain: [...prev.rain, data.rain],
        battery: [...prev.battery, data.battery],
        predictedTemp: [...prev.predictedTemp, data.predictedTemp],
        rainProbability: [...prev.rainProbability, data.rainProbability],
        predictedStatus: [...prev.predictedStatus, data.predictedStatus]
      };

      // Cap at 20 historical items
      if (updated.timestamps.length > 20) {
        Object.keys(updated).forEach(key => {
          updated[key].shift();
        });
      }
      return updated;
    });
  };

  const generateMockData = () => {
    const baseTemp = 29.5 + Math.sin(Date.now() / 60000) * 2;
    const baseHum = 75 + Math.cos(Date.now() / 60000) * 8;
    const basePres = 1008 + Math.sin(Date.now() / 120000) * 3;
    const rainRand = Math.random() > 0.85 ? 250 : 1023;
    
    const tempVal = parseFloat(baseTemp.toFixed(1));
    const humVal = parseFloat(baseHum.toFixed(1));
    const presVal = parseFloat(basePres.toFixed(1));
    
    const mock = {
      temperature: tempVal,
      humidity: humVal,
      pressure: presVal,
      rain: rainRand,
      battery: 98,
      predictedTemp: parseFloat((tempVal + 0.4 - Math.random() * 0.8).toFixed(1)),
      predictedHumidity: parseFloat((humVal + 2 - Math.random() * 4).toFixed(1)),
      predictedPressure: parseFloat((presVal + 0.5 - Math.random() * 1).toFixed(1)),
      rainProbability: rainRand < 500 ? 95 : 15,
      predictedStatus: rainRand < 500 ? 2 : (humVal > 78 ? 1 : 0),
      rssi: -68 - Math.floor(Math.random() * 8),
      seqNum: (currentData.seqNum + 1) % 256,
      lastUpdated: new Date().toLocaleTimeString('vi-VN')
    };

    setCurrentData(mock);
    addLocalHistory(mock);
  };

  const fetchFromLocalESP = () => {
    const url = `http://${config.espIp}/data`;
    fetch(url)
      .then(res => {
        if (!res.ok) throw new Error("Local Network Error");
        return res.json();
      })
      .then(data => {
        setIsOnline(true);
        setConnectionText("Đang kết nối: Local Gateway OK");
        
        const temp = parseFloat(data.temperature);
        const hum = parseFloat(data.humidity);
        const pres = parseFloat(data.pressure);
        const rainVal = parseInt(data.rain);
        
        const live = {
          temperature: temp,
          humidity: hum,
          pressure: pres,
          rain: rainVal,
          battery: parseInt(data.battery_pct || 100),
          predictedTemp: currentData.predictedTemp || temp,
          predictedHumidity: currentData.predictedHumidity || hum,
          predictedPressure: currentData.predictedPressure || pres,
          rainProbability: currentData.rainProbability || (rainVal < 500 ? 90 : 10),
          predictedStatus: currentData.predictedStatus || (rainVal < 500 ? 2 : (hum > 78 ? 1 : 0)),
          rssi: parseInt(data.rssi || -70),
          seqNum: parseInt(data.seq_num || 0),
          lastUpdated: new Date().toLocaleTimeString('vi-VN')
        };
        
        setCurrentData(live);
        addLocalHistory(live);
      })
      .catch(err => {
        console.warn("Không kết nối được ESP8266 local, chạy mô phỏng...", err);
        setIsOnline(false);
        setConnectionText("Mất kết nối Local Gateway. Đang chạy mô phỏng.");
        generateMockData();
      });
  };

  const fetchFromThingSpeak = () => {
    if (!config.channelId) {
      setIsOnline(false);
      setConnectionText("Chưa điền Channel ID. Đang chạy mô phỏng.");
      generateMockData();
      return;
    }
    
    let url = `https://api.thingspeak.com/channels/${config.channelId}/feeds.json?results=80`;
    if (config.readApiKey) {
      url += `&api_key=${config.readApiKey}`;
    }
    
    fetch(url)
      .then(res => {
        if (!res.ok) throw new Error("ThingSpeak Server Error");
        return res.json();
      })
      .then(data => {
        if (!data.feeds || data.feeds.length === 0) {
          throw new Error("Kênh trống.");
        }
        
        setIsOnline(true);
        setConnectionText("Đang kết nối: ThingSpeak Cloud OK");
        
        const timestamps = [];
        const temperature = [];
        const humidity = [];
        const pressure = [];
        const rain = [];
        const battery = [];
        const predictedTemp = [];
        const rainProbability = [];
        const predictedStatus = [];
        
        const getFirstNonNull = (fieldName, fallback) => {
          for (let i = 0; i < data.feeds.length; i++) {
            const val = data.feeds[i][fieldName];
            if (val !== null && val !== undefined && val !== '' && val !== 'null') {
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
          
          let temp = null;
          if (feed.field1 !== null && feed.field1 !== undefined && feed.field1 !== '' && feed.field1 !== 'null') {
            const parsed = parseFloat(feed.field1);
            if (!isNaN(parsed)) temp = parsed;
          }
          if (temp === null) {
            temp = temperature.length > 0 ? temperature[temperature.length - 1] : firstVal.temp;
          }
          
          let hum = null;
          if (feed.field2 !== null && feed.field2 !== undefined && feed.field2 !== '' && feed.field2 !== 'null') {
            const parsed = parseFloat(feed.field2);
            if (!isNaN(parsed)) hum = parsed;
          }
          if (hum === null) {
            hum = humidity.length > 0 ? humidity[humidity.length - 1] : firstVal.hum;
          }
          
          let pres = null;
          if (feed.field3 !== null && feed.field3 !== undefined && feed.field3 !== '' && feed.field3 !== 'null') {
            const parsed = parseFloat(feed.field3);
            if (!isNaN(parsed)) pres = parsed;
          }
          if (pres === null) {
            pres = pressure.length > 0 ? pressure[pressure.length - 1] : firstVal.pres;
          }
          
          let rainVal = null;
          if (feed.field4 !== null && feed.field4 !== undefined && feed.field4 !== '' && feed.field4 !== 'null') {
            const parsed = parseInt(feed.field4);
            if (!isNaN(parsed)) rainVal = parsed;
          }
          if (rainVal === null) {
            rainVal = rain.length > 0 ? rain[rain.length - 1] : firstVal.rain;
          }
          
          let bat = null;
          if (feed.field5 !== null && feed.field5 !== undefined && feed.field5 !== '' && feed.field5 !== 'null') {
            const parsed = parseInt(feed.field5);
            if (!isNaN(parsed)) bat = parsed;
          }
          if (bat === null) {
            bat = battery.length > 0 ? battery[battery.length - 1] : firstVal.bat;
          }
          
          let predT = null;
          if (feed.field6 !== null && feed.field6 !== undefined && feed.field6 !== '' && feed.field6 !== 'null') {
            const parsed = parseFloat(feed.field6);
            if (!isNaN(parsed)) predT = parsed;
          }
          if (predT === null) {
            predT = predictedTemp.length > 0 ? predictedTemp[predictedTemp.length - 1] : firstVal.predT;
          }
          
          let rainP = null;
          if (feed.field7 !== null && feed.field7 !== undefined && feed.field7 !== '' && feed.field7 !== 'null') {
            const parsed = parseFloat(feed.field7);
            if (!isNaN(parsed)) rainP = parsed;
          }
          if (rainP === null) {
            rainP = rainProbability.length > 0 ? rainProbability[rainProbability.length - 1] : firstVal.rainP;
          }
          
          // Tính toán trạng thái thời tiết dự tính động dựa trên xác suất mưa (field7) và độ ẩm (field2)
          let predStat = (rainP !== null && rainP > 50) ? 2 : (hum > 78 ? 1 : 0);
          
          timestamps.push(timeStr);
          temperature.push(temp);
          humidity.push(hum);
          pressure.push(pres);
          rain.push(rainVal);
          battery.push(bat);
          predictedTemp.push(predT);
          rainProbability.push(rainP);
          predictedStatus.push(predStat);
        });

        // Set history state
        setHistoryData({
          timestamps,
          temperature,
          humidity,
          pressure,
          rain,
          battery,
          predictedTemp,
          rainProbability,
          predictedStatus
        });
        
        // Trích xuất giá trị mới nhất cho từng trường bằng cách quét ngược từ cuối danh sách (field-by-field backtracking)
        const getLatestNonNull = (fieldName, fallback) => {
          for (let i = data.feeds.length - 1; i >= 0; i--) {
            const val = data.feeds[i][fieldName];
            if (val !== null && val !== undefined && val !== '' && val !== 'null') {
              const num = parseFloat(val);
              if (!isNaN(num)) return num;
            }
          }
          return fallback;
        };

        const latestTemp = getLatestNonNull('field1', 28.0);
        const latestHum = getLatestNonNull('field2', 75.0);
        const latestPres = getLatestNonNull('field3', 1010.0);
        const latestRain = getLatestNonNull('field4', 1023);
        const latestBat = getLatestNonNull('field5', 100);
        const latestPredTemp = getLatestNonNull('field6', null);
        const latestRainProb = getLatestNonNull('field7', null);
        const latestPredHum = getLatestNonNull('field8', null); // field8 đổi thành dự báo độ ẩm AI
        
        // Tính toán dự báo áp suất khí quyển vật lý qua hồi quy tuyến tính của chuỗi lịch sử nhận được
        const getPressureSlope = (presList) => {
          if (!presList || presList.length < 5) return 0;
          const n = presList.length;
          const x = Array.from({length: n}, (_, i) => i + 1);
          let sumX = 0, sumY = 0, sumXY = 0, sumXX = 0;
          for (let i = 0; i < n; i++) {
            sumX += x[i];
            sumY += presList[i];
            sumXY += x[i] * presList[i];
            sumXX += x[i] * x[i];
          }
          const denom = (n * sumXX) - (sumX * sumX);
          if (denom === 0) return 0;
          return ((n * sumXY) - (sumX * sumY)) / denom;
        };

        const presSlope = getPressureSlope(pressure);
        const latestPredPres = latestPres !== null ? latestPres + presSlope * 5 : null;

        const lastUpdateDate = new Date(data.feeds[data.feeds.length - 1].created_at);
        
        setCurrentData({
          temperature: latestTemp,
          humidity: latestHum,
          pressure: latestPres,
          rain: latestRain,
          battery: latestBat,
          predictedTemp: latestPredTemp,
          predictedHumidity: latestPredHum,
          predictedPressure: latestPredPres,
          rainProbability: latestRainProb,
          predictedStatus: (latestRainProb !== null && latestRainProb > 50) ? 2 : (latestHum > 78 ? 1 : 0),
          rssi: null,
          seqNum: null,
          lastUpdated: lastUpdateDate.toLocaleTimeString('vi-VN')
        });
      })
      .catch(err => {
        console.warn("Không kết nối được ThingSpeak, chạy mô phỏng...", err);
        setIsOnline(false);
        setConnectionText("Không lấy được dữ liệu Cloud. Đang chạy mô phỏng.");
        generateMockData();
      });
  };

  const calculatePressureTrend = () => {
    const pres = historyData.pressure || [];
    if (pres.length < 5) return 'Hồi quy: Đang tính...';
    const n = pres.length;
    const x = Array.from({length: n}, (_, i) => i + 1);
    let sumX = 0, sumY = 0, sumXY = 0, sumXX = 0;
    for (let i = 0; i < n; i++) {
      sumX += x[i];
      sumY += pres[i];
      sumXY += x[i] * pres[i];
      sumXX += x[i] * x[i];
    }
    const denom = (n * sumXX) - (sumX * sumX);
    if (denom === 0) return 'Hồi quy: Ổn định';
    const slope = ((n * sumXY) - (sumX * sumY)) / denom;
    if (slope > 0.05) return `Hồi quy: Tăng (+${(slope * 5).toFixed(1)} hPa/h)`;
    if (slope < -0.05) return `Hồi quy: Giảm (${(slope * 5).toFixed(1)} hPa/h)`;
    return 'Hồi quy: Ổn định';
  };

  const handleSaveSettings = (newConfig) => {
    const updatedConfig = { ...config, ...newConfig };
    setConfig(updatedConfig);
    localStorage.setItem('weather_dashboard_config', JSON.stringify(updatedConfig));
    setIsSettingsOpen(false);
  };

  const handleResetToDemoSettings = () => {
    const demo = {
      channelId: '3413241',
      readApiKey: 'VISWQT7BABDVZOFS',
      matlabId: '674169',
      espIp: '192.168.4.1'
    };
    return demo;
  };

  const isRaining = currentData.rain !== null && currentData.rain < 500;

  return (
    <div className="bg-background text-on-surface font-body-md min-h-screen selection:bg-secondary-container selection:text-on-secondary-container relative">
      {/* GLOBAL SVG GRADIENTS (Decorative) */}
      <svg width="0" height="0" style={{ position: 'absolute', pointerEvents: 'none' }}>
        <defs>
          <linearGradient id="sunGradient" x1="0%" y1="0%" x2="100%" y2="100%">
            <stop offset="0%" stopColor="#fbbf24" />
            <stop offset="100%" stopColor="#f59e0b" />
          </linearGradient>
          <linearGradient id="cloudGradient" x1="0%" y1="0%" x2="100%" y2="100%">
            <stop offset="0%" stopColor="#e2e8f0" />
            <stop offset="100%" stopColor="#94a3b8" />
          </linearGradient>
          <linearGradient id="cloudGrad" x1="0%" y1="0%" x2="100%" y2="100%">
            <stop offset="0%" stopColor="#94a3b8" />
            <stop offset="100%" stopColor="#475569" />
          </linearGradient>
        </defs>
      </svg>

      {/* BACKGROUND GLOW SPOTS */}
      <div className="absolute top-[15%] left-[10%] w-[450px] h-[450px] bg-temp-orange/10 rounded-full blur-[140px] z-0 pointer-events-none"></div>
      <div className="absolute bottom-[15%] right-[10%] w-[450px] h-[450px] bg-humidity-cyan/10 rounded-full blur-[140px] z-0 pointer-events-none"></div>

      {/* HEADER */}
      <Header 
        dataSource={config.dataSource} 
        setDataSource={(source) => setConfig(prev => ({ ...prev, dataSource: source }))}
        toggleModal={setIsSettingsOpen}
        isOnline={isOnline}
        connectionText={connectionText}
        relayState={relayState}
      />

      {/* SIDEBAR NAVIGATION */}
      <aside className="fixed left-0 top-0 h-full flex flex-col pt-24 pb-8 px-4 bg-surface-container-lowest/50 backdrop-blur-2xl border-r border-glass-border shadow-none w-64 transition-all duration-300 ease-in-out hidden md:flex z-40">
        <div className="mb-8 px-4">
          <div className="flex items-center gap-3">
            <div className="w-10 h-10 rounded-lg bg-surface-container flex items-center justify-center border border-glass-border">
              <span className="material-symbols-outlined text-primary">router</span>
            </div>
            <div>
              <h2 className="font-headline-sm text-sm font-bold text-on-surface">IOT102 Node</h2>
              <p className="font-data-mono text-[10px] text-air-quality-green font-bold">Station Active</p>
            </div>
          </div>
        </div>
        
        <nav className="flex-1 space-y-2">
          <a className="flex items-center gap-3 px-4 py-3 rounded-lg bg-secondary-container/20 text-secondary-fixed-dim border-r-4 border-secondary-fixed-dim font-label-caps text-xs hover:bg-glass-fill transition-all uppercase font-bold" href="#" onClick={(e) => e.preventDefault()}>
            <span className="material-symbols-outlined text-lg" style={{ fontVariationSettings: "'FILL' 1" }}>dashboard</span>
            Overview
          </a>
          <a className="flex items-center gap-3 px-4 py-3 rounded-lg text-on-surface-variant hover:text-on-surface hover:bg-glass-fill transition-all font-label-caps text-xs uppercase" href="#" onClick={(e) => e.preventDefault()}>
            <span className="material-symbols-outlined text-lg">show_chart</span>
            Analytics
          </a>
          <a className="flex items-center gap-3 px-4 py-3 rounded-lg text-on-surface-variant hover:text-on-surface hover:bg-glass-fill transition-all font-label-caps text-xs uppercase" href="#" onClick={(e) => e.preventDefault()}>
            <span className="material-symbols-outlined text-lg">sensors</span>
            Sensors
          </a>
          <a className="flex items-center gap-3 px-4 py-3 rounded-lg text-on-surface-variant hover:text-on-surface hover:bg-glass-fill transition-all font-label-caps text-xs uppercase" href="#" onClick={(e) => e.preventDefault()}>
            <span className="material-symbols-outlined text-lg">psychology</span>
            AI Insights
          </a>
          <a className="flex items-center gap-3 px-4 py-3 rounded-lg text-on-surface-variant hover:text-on-surface hover:bg-glass-fill transition-all font-label-caps text-xs uppercase" href="#" onClick={(e) => e.preventDefault()}>
            <span className="material-symbols-outlined text-lg">hub</span>
            Network
          </a>
        </nav>

        <div className="mt-auto space-y-4">
          <button 
            className="w-full py-2 bg-surface-container rounded-lg border border-glass-border text-on-surface hover:bg-surface-variant transition-colors font-label-caps text-xs flex items-center justify-center gap-2"
            onClick={() => {
              if (!historyData.timestamps || historyData.timestamps.length === 0) {
                alert("Không có dữ liệu lịch sử để xuất file!");
                return;
              }
              const headers = "Thời gian,Nhiệt độ (°C),Độ ẩm (%),Khí áp (hPa),Cảm biến mưa,Dung lượng pin (%),Dự báo nhiệt độ AI (°C),Xác suất mưa AI (%),Trạng thái AI\n";
              const rows = historyData.timestamps.map((t, idx) => {
                return [
                  t,
                  historyData.temperature[idx] !== null ? historyData.temperature[idx] : '',
                  historyData.humidity[idx] !== null ? historyData.humidity[idx] : '',
                  historyData.pressure[idx] !== null ? historyData.pressure[idx] : '',
                  historyData.rain[idx] !== null ? historyData.rain[idx] : '',
                  historyData.battery[idx] !== null ? historyData.battery[idx] : '',
                  historyData.predictedTemp[idx] !== null ? historyData.predictedTemp[idx] : '',
                  historyData.rainProbability[idx] !== null ? historyData.rainProbability[idx] : '',
                  historyData.predictedStatus[idx] !== null ? historyData.predictedStatus[idx] : ''
                ].join(',');
              }).join('\n');
              
              const csvContent = "data:text/csv;charset=utf-8,\uFEFF" + encodeURIComponent(headers + rows);
              const downloadAnchor = document.createElement('a');
              downloadAnchor.setAttribute("href", csvContent);
              downloadAnchor.setAttribute("download", `weather_data_${Date.now()}.csv`);
              document.body.appendChild(downloadAnchor);
              downloadAnchor.click();
              downloadAnchor.remove();
            }}
          >
            <span className="material-symbols-outlined text-[18px]">download</span>
            Export Data
          </button>
          
          <div className="border-t border-glass-border pt-4 space-y-2">
            <a className="flex items-center gap-3 px-4 py-2 rounded-lg text-on-surface-variant hover:text-on-surface hover:bg-glass-fill transition-all font-label-caps text-xs uppercase" href="#" onClick={(e) => { e.preventDefault(); setIsSettingsOpen(true); }}>
              <span className="material-symbols-outlined text-[18px]">settings</span>
              Settings
            </a>
            <a className="flex items-center gap-3 px-4 py-2 rounded-lg text-on-surface-variant hover:text-on-surface hover:bg-glass-fill transition-all font-label-caps text-xs uppercase" href="#" onClick={(e) => e.preventDefault()}>
              <span className="material-symbols-outlined text-[18px]">help</span>
              Support
            </a>
          </div>
        </div>
      </aside>

      {/* MAIN CONTAINER */}
      <main className="pt-24 pb-8 px-4 md:pl-72 md:pr-8 min-h-screen relative z-10">
        {/* WARNING BANNER FOR RAIN */}
        {isRaining && (
          <div className="mb-6 flex items-center gap-3 bg-warning-red/20 border border-warning-red/40 p-4 rounded-xl text-warning-red" id="alertBox">
            <span className="material-symbols-outlined animate-bounce">warning</span>
            <span className="text-sm font-semibold">⚠ Cảnh báo mưa úng! Kiểm tra ngay các rãnh thoát nước vườn sầu riêng Miền Tây và cà phê trũng.</span>
          </div>
        )}

        {/* METALS & STATUS BAR */}
        <MetaBar 
          rssi={currentData.rssi}
          seqNum={currentData.seqNum}
          lastUpdated={currentData.lastUpdated}
          dataSource={config.dataSource}
          channelId={config.channelId}
          espIp={config.espIp}
        />

        {/* HERO DISPLAY */}
        <HeroBanner 
          temperature={currentData.temperature}
          humidity={currentData.humidity}
          rain={currentData.rain}
          predictedTemp={currentData.predictedTemp}
          rainProbability={currentData.rainProbability}
          predictedStatus={currentData.predictedStatus}
        />

        {/* LOWER GRID LAYOUT */}
        <div className="grid grid-cols-1 xl:grid-cols-12 gap-6">
          
          {/* LEFT COLUMN: PARAM METRICS & ML STATISTICS */}
          <div className="xl:col-span-4 grid grid-cols-1 md:grid-cols-2 gap-4 h-fit">
            <MetricCard 
              type="humidity" 
              value={currentData.humidity} 
              predictedValue={currentData.predictedHumidity}
            />
            <MetricCard 
              type="pressure" 
              value={currentData.pressure} 
              trendText={calculatePressureTrend()}
              predictedValue={currentData.predictedPressure}
            />
            <MetricCard 
              type="rain" 
              rawValue={currentData.rain} 
            />
            <MetricCard 
              type="battery" 
              value={currentData.battery} 
            />
            
            {/* Actuator/Relay control card */}
            <div className="col-span-1 md:col-span-2 glass-card p-5 flex flex-col gap-4">
              <div className="flex items-center justify-between">
                <div className="flex items-center gap-3">
                  <div className={`w-10 h-10 rounded-full flex items-center justify-center border transition-all duration-300 ${
                    relayState 
                      ? 'bg-secondary-container/20 border-secondary-container text-secondary-fixed-dim shadow-[0_0_15px_rgba(0,242,255,0.4)]' 
                      : 'bg-surface-container/50 border-glass-border text-on-surface-variant'
                  }`}>
                    <span className={`material-symbols-outlined text-lg ${relayState ? 'animate-spin' : ''}`}>
                      {relayState ? 'nest_eco_leaf' : 'valve'}
                    </span>
                  </div>
                  <div>
                    <h3 className="text-xs font-bold font-label-caps uppercase text-on-surface-variant">Hệ thống máy bơm tưới</h3>
                    <p className="font-data-mono text-[10px] text-on-surface-variant">Điều khiển D2 (Relay)</p>
                  </div>
                </div>
                <button
                  onClick={handleToggleRelay}
                  className={`px-4 py-1.5 rounded-full text-xs font-bold transition-all duration-200 ${
                    relayState 
                      ? 'bg-secondary-container/20 border border-secondary-container text-secondary-fixed-dim shadow-[0_0_8px_rgba(0,242,255,0.2)]' 
                      : 'bg-surface-container border border-glass-border text-on-surface-variant hover:text-on-surface'
                  }`}
                >
                  {relayState ? 'ĐANG BẬT' : 'ĐANG TẮT'}
                </button>
              </div>
            </div>

            <div className="col-span-1 md:col-span-2">
              <MLPanel />
            </div>
          </div>

          {/* RIGHT COLUMN: CHARTING & LOCAL REGRESSION */}
          <div className="xl:col-span-8 flex flex-col gap-6">
            <ChartPanel 
              historyData={historyData}
              activeChartTab={activeChartTab}
              setActiveChartTab={setActiveChartTab}
            />

            <LocalEngine 
              historyData={historyData}
              rainValue={currentData.rain}
            />
          </div>
        </div>
      </main>

      {/* CONFIGURATION MODAL */}
      <SettingsModal 
        isOpen={isSettingsOpen}
        toggleModal={setIsSettingsOpen}
        config={config}
        saveSettings={handleSaveSettings}
        resetToDemoSettings={handleResetToDemoSettings}
      />
    </div>
  );
}
