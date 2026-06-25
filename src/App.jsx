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

  const [currentData, setCurrentData] = useState({
    temperature: null,
    humidity: null,
    pressure: null,
    rain: null,
    battery: null,
    predictedTemp: null,
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
    
    let url = `https://api.thingspeak.com/channels/${config.channelId}/feeds.json?results=20`;
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
        
        data.feeds.forEach(feed => {
          const date = new Date(feed.created_at);
          const timeStr = `${String(date.getHours()).padStart(2, '0')}:${String(date.getMinutes()).padStart(2, '0')}`;
          
          let temp = (feed.field1 !== null && feed.field1 !== undefined && feed.field1 !== '') ? parseFloat(feed.field1) : null;
          if (temp === null) {
            temp = temperature.length > 0 ? temperature[temperature.length - 1] : 0;
          }
          
          let hum = (feed.field2 !== null && feed.field2 !== undefined && feed.field2 !== '') ? parseFloat(feed.field2) : null;
          if (hum === null) {
            hum = humidity.length > 0 ? humidity[humidity.length - 1] : 0;
          }
          
          let pres = (feed.field3 !== null && feed.field3 !== undefined && feed.field3 !== '') ? parseFloat(feed.field3) : null;
          if (pres === null) {
            pres = pressure.length > 0 ? pressure[pressure.length - 1] : 0;
          }
          
          let rainVal = (feed.field4 !== null && feed.field4 !== undefined && feed.field4 !== '') ? parseInt(feed.field4) : null;
          if (rainVal === null) {
            rainVal = rain.length > 0 ? rain[rain.length - 1] : 1023;
          }
          
          let bat = (feed.field5 !== null && feed.field5 !== undefined && feed.field5 !== '') ? parseInt(feed.field5) : null;
          if (bat === null) {
            bat = battery.length > 0 ? battery[battery.length - 1] : 100;
          }
          
          let predT = (feed.field6 !== null && feed.field6 !== undefined && feed.field6 !== '') ? parseFloat(feed.field6) : null;
          if (predT === null) {
            predT = predictedTemp.length > 0 ? predictedTemp[predictedTemp.length - 1] : null;
          }
          
          let rainP = (feed.field7 !== null && feed.field7 !== undefined && feed.field7 !== '') ? parseFloat(feed.field7) : null;
          if (rainP === null) {
            rainP = rainProbability.length > 0 ? rainProbability[rainProbability.length - 1] : null;
          }
          
          let predStat = (feed.field8 !== null && feed.field8 !== undefined && feed.field8 !== '') ? parseInt(feed.field8) : null;
          if (predStat === null) {
            predStat = predictedStatus.length > 0 ? predictedStatus[predictedStatus.length - 1] : null;
          }
          
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
        
        // Find latest sensor feed
        let latestSensorFeed = null;
        for (let i = data.feeds.length - 1; i >= 0; i--) {
          if (data.feeds[i].field1 !== null && data.feeds[i].field1 !== undefined && data.feeds[i].field1 !== '') {
            latestSensorFeed = data.feeds[i];
            break;
          }
        }
        if (!latestSensorFeed) latestSensorFeed = data.feeds[data.feeds.length - 1];

        // Find latest ML forecast feed
        let latestMLFeed = null;
        for (let i = data.feeds.length - 1; i >= 0; i--) {
          if (data.feeds[i].field6 !== null && data.feeds[i].field6 !== undefined && data.feeds[i].field6 !== '') {
            latestMLFeed = data.feeds[i];
            break;
          }
        }
        if (!latestMLFeed) latestMLFeed = data.feeds[data.feeds.length - 1];

        const lastUpdateDate = new Date(data.feeds[data.feeds.length - 1].created_at);
        
        setCurrentData({
          temperature: latestSensorFeed.field1 !== null ? parseFloat(latestSensorFeed.field1) : 0,
          humidity: latestSensorFeed.field2 !== null ? parseFloat(latestSensorFeed.field2) : 0,
          pressure: latestSensorFeed.field3 !== null ? parseFloat(latestSensorFeed.field3) : 0,
          rain: latestSensorFeed.field4 !== null ? parseInt(latestSensorFeed.field4) : 1023,
          battery: latestSensorFeed.field5 !== null ? parseInt(latestSensorFeed.field5) : 100,
          predictedTemp: latestMLFeed.field6 !== null ? parseFloat(latestMLFeed.field6) : null,
          rainProbability: latestMLFeed.field7 !== null ? parseFloat(latestMLFeed.field7) : null,
          predictedStatus: latestMLFeed.field8 !== null ? parseInt(latestMLFeed.field8) : null,
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
    <div className="app-container">
      {/* GLOBAL SVG GRADIENTS */}
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
      <div className="bg-glow circle-1"></div>
      <div className="bg-glow circle-2"></div>

      {/* HEADER */}
      <Header 
        dataSource={config.dataSource} 
        setDataSource={(source) => setConfig(prev => ({ ...prev, dataSource: source }))}
        toggleModal={setIsSettingsOpen}
        isOnline={isOnline}
        connectionText={connectionText}
      />

      {/* META BAR */}
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

      {/* LOWER SYMMETRICAL GRID */}
      <div className="balanced-grid">
        
        {/* LEFT COLUMN: PARAM METRICS & ML STATISTICS */}
        <div className="grid-column">
          
          <div className="params-grid">
            <MetricCard 
              type="humidity" 
              value={currentData.humidity} 
            />
            <MetricCard 
              type="pressure" 
              value={currentData.pressure} 
              trendText={document.getElementById('subPressure')?.innerText} // Calculated by local regression
            />
            <MetricCard 
              type="rain" 
              rawValue={currentData.rain} 
            />
            <MetricCard 
              type="battery" 
              value={currentData.battery} 
            />
          </div>

          <MLPanel />
        </div>

        {/* RIGHT COLUMN: CHARTING & LOCAL REGRESSION */}
        <div className="grid-column">
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

      {/* CONFIGURATION MODAL */}
      <SettingsModal 
        isOpen={isSettingsOpen}
        toggleModal={setIsSettingsOpen}
        config={config}
        saveSettings={handleSaveSettings}
        resetToDemoSettings={handleResetToDemoSettings}
      />

      {/* WARNING BANNER FOR RAIN */}
      {isRaining && (
        <div className="alert-banner" id="alertBox">
          <span className="alert-icon">⚠</span>
          <span className="alert-message">⚠ Cảnh báo mưa úng! Kiểm tra ngay các rãnh thoát nước vườn sầu riêng Miền Tây và cà phê trũng.</span>
        </div>
      )}
    </div>
  );
}
