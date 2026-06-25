import React from 'react';
import {
  Chart as ChartJS,
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
  Filler
} from 'chart.js';
import { Line } from 'react-chartjs-2';

// Register ChartJS elements
ChartJS.register(
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
  Filler
);

export default function ChartPanel({ historyData, activeChartTab, setActiveChartTab }) {
  
  const labels = historyData.timestamps || [];
  
  // Ước lượng khoảng cách thời gian và mở rộng trục X thêm 1 tiếng (6 bước với chu kỳ 10 phút)
  const stepsPerHour = 6;
  const extendedLabels = [...labels];
  
  if (labels.length > 1) {
    let intervalMins = 10;
    try {
      const getMinutes = (timeStr) => {
        const parts = timeStr.split(':');
        const h = parseInt(parts[0], 10);
        const m = parseInt(parts[1], 10);
        return h * 60 + m;
      };
      const diff = getMinutes(labels[labels.length - 1]) - getMinutes(labels[labels.length - 2]);
      if (diff > 0 && diff <= 60) {
        intervalMins = diff;
      }
    } catch (e) {}
    
    const lastTimestampStr = labels[labels.length - 1];
    if (lastTimestampStr) {
      try {
        const parts = lastTimestampStr.split(':');
        const h = parseInt(parts[0], 10);
        const m = parseInt(parts[1], 10);
        let lastTime = new Date();
        lastTime.setHours(h, m, 0, 0);
        
        for (let i = 1; i <= stepsPerHour; i++) {
          const futureTime = new Date(lastTime.getTime() + i * intervalMins * 60 * 1000);
          const timeStr = `${String(futureTime.getHours()).padStart(2, '0')}:${String(futureTime.getMinutes()).padStart(2, '0')}`;
          extendedLabels.push(timeStr);
        }
      } catch (e) {}
    }
  }

  // Hàm đồng bộ và kéo dài dữ liệu thực tế & dự báo:
  // - Thực tế (Actual) kéo dài thêm 6 giá trị null ở cuối (vì chưa xảy ra)
  // - Dự đoán (Predicted) tại thời điểm i là dự đoán cho i + 6 (1 tiếng sau), nên dịch chuyển sang phải 6 đơn vị.
  const alignData = (actual, predicted) => {
    const alignedActual = [...actual];
    for (let i = 0; i < stepsPerHour; i++) {
      alignedActual.push(null);
    }
    
    const alignedPredicted = Array(extendedLabels.length).fill(null);
    if (predicted && predicted.length > 0) {
      for (let i = 0; i < predicted.length; i++) {
        if (predicted[i] !== null && predicted[i] !== undefined) {
          alignedPredicted[i + stepsPerHour] = predicted[i];
        }
      }
    }
    return { alignedActual, alignedPredicted };
  };

  // Build chart dataset config based on active tab
  let primaryLabel = "";
  let rawPrimaryData = [];
  let primaryColor = "#FF8C00";
  let primaryGradColor = "rgba(255, 140, 0, 0.12)";
  
  let secondaryLabel = "";
  let rawSecondaryData = [];
  let secondaryColor = "#eab308";
  
  let showSecondary = false;

  switch (activeChartTab) {
    case 'temp':
      primaryLabel = 'Nhiệt độ Thực tế (°C)';
      rawPrimaryData = historyData.temperature || [];
      primaryColor = '#FF8C00';
      primaryGradColor = 'rgba(255, 140, 0, 0.12)';
      
      secondaryLabel = 'Dự báo AI (+1h) (°C)';
      rawSecondaryData = historyData.predictedTemp || [];
      secondaryColor = '#f97316';
      showSecondary = true;
      break;
      
    case 'hum':
      primaryLabel = 'Độ ẩm thực tế (%)';
      rawPrimaryData = historyData.humidity || [];
      primaryColor = '#00F2FF';
      primaryGradColor = 'rgba(0, 242, 255, 0.12)';
      
      secondaryLabel = 'Dự báo AI (+1h) (%)';
      rawSecondaryData = historyData.predictedHumidity || [];
      secondaryColor = '#38bdf8';
      showSecondary = true;
      break;
      
    case 'pres':
      primaryLabel = 'Áp suất khí quyển (hPa)';
      rawPrimaryData = historyData.pressure || [];
      primaryColor = '#00FF94';
      primaryGradColor = 'rgba(0, 255, 148, 0.12)';
      showSecondary = false;
      break;
      
    case 'rain':
      primaryLabel = 'Cảm biến mưa (Analog)';
      rawPrimaryData = historyData.rain || [];
      primaryColor = '#10b981';
      primaryGradColor = 'rgba(16, 185, 129, 0.12)';
      
      secondaryLabel = 'Xác suất mưa AI (%)';
      rawSecondaryData = historyData.rainProbability || [];
      secondaryColor = '#6366f1';
      showSecondary = true;
      break;
      
    case 'bat':
      primaryLabel = 'Nguồn Pin Trạm Đo (%)';
      rawPrimaryData = historyData.battery || [];
      primaryColor = '#74f5ff';
      primaryGradColor = 'rgba(116, 245, 255, 0.12)';
      showSecondary = false;
      break;

    default:
      break;
  }

  // Thực hiện đồng bộ và mở rộng dữ liệu cho biểu đồ
  const { alignedActual, alignedPredicted } = alignData(rawPrimaryData, rawSecondaryData);

  // Create datasets list
  const datasets = [
    {
      label: primaryLabel,
      data: alignedActual,
      borderColor: primaryColor,
      backgroundColor: (context) => {
        const chart = context.chart;
        const { ctx, chartArea } = chart;
        if (!chartArea) return null;
        const gradient = ctx.createLinearGradient(0, chartArea.top, 0, chartArea.bottom);
        gradient.addColorStop(0, primaryGradColor);
        gradient.addColorStop(1, 'rgba(19, 19, 20, 0)');
        return gradient;
      },
      fill: true,
      tension: 0.35,
      borderWidth: 2,
      pointRadius: (context) => {
        const idx = context.dataIndex;
        return idx < rawPrimaryData.length ? 1 : 0;
      },
      pointHoverRadius: 6,
      pointHitRadius: 15,
      pointBackgroundColor: primaryColor,
      pointBorderColor: 'rgba(255, 255, 255, 0.8)',
      pointBorderWidth: 1.5
    }
  ];

  if (showSecondary) {
    datasets.push({
      label: secondaryLabel,
      data: alignedPredicted,
      borderColor: secondaryColor,
      borderDash: [5, 5],
      fill: false,
      tension: 0.35,
      borderWidth: 2,
      pointRadius: 1.5,
      pointHoverRadius: 6,
      pointHitRadius: 15,
      pointBackgroundColor: secondaryColor,
      pointBorderColor: 'rgba(255, 255, 255, 0.8)',
      pointBorderWidth: 1.5
    });
  }

  const chartData = {
    labels: extendedLabels,
    datasets
  };

  const chartOptions = {
    responsive: true,
    maintainAspectRatio: false,
    plugins: {
      legend: {
        display: false // We will render a custom legend to match the mockups
      },
      tooltip: {
        backgroundColor: 'rgba(28, 27, 29, 0.95)',
        titleColor: '#e5e2e3',
        bodyColor: '#c7c6cd',
        titleFont: {
          family: 'Sora',
          size: 12,
          weight: '600'
        },
        bodyFont: {
          family: 'Inter',
          size: 12
        },
        padding: 12,
        borderColor: 'rgba(255, 255, 255, 0.08)',
        borderWidth: 1,
        borderRadius: 10,
        usePointStyle: true,
        boxPadding: 6,
        callbacks: {
          labelColor: function(context) {
            return {
              borderColor: context.dataset.borderColor,
              backgroundColor: context.dataset.borderColor,
              borderWidth: 0,
              borderRadius: 2
            };
          }
        }
      }
    },
    interaction: {
      mode: 'index',
      intersect: false,
    },
    scales: {
      x: {
        grid: {
          color: 'rgba(255, 255, 255, 0.025)',
          border: {
            dash: [4, 4],
            display: false
          }
        },
        ticks: {
          color: '#909097',
          maxRotation: 0,
          autoSkip: true,
          maxTicksLimit: 8,
          font: {
            family: 'Inter',
            size: 10,
            weight: '500'
          }
        }
      },
      y: {
        grid: {
          color: 'rgba(255, 255, 255, 0.025)',
          border: {
            dash: [4, 4],
            display: false
          }
        },
        ticks: {
          color: '#909097',
          font: {
            family: 'Inter',
            size: 11,
            weight: '500'
          }
        }
      }
    }
  };

  // Crosshair Guideline Plugin
  const verticalLinePlugin = {
    id: 'verticalLine',
    afterDraw: (chart) => {
      if (chart.tooltip?._active && chart.tooltip._active.length) {
        const activePoint = chart.tooltip._active[0];
        const ctx = chart.ctx;
        const x = activePoint.element.x;
        const topY = chart.scales.y.top;
        const bottomY = chart.scales.y.bottom;
        
        ctx.save();
        ctx.beginPath();
        ctx.moveTo(x, topY);
        ctx.lineTo(x, bottomY);
        ctx.lineWidth = 1;
        ctx.strokeStyle = 'rgba(255, 255, 255, 0.15)';
        ctx.setLineDash([4, 4]);
        ctx.stroke();
        ctx.restore();
      }
    }
  };

  return (
    <div className="glass-card p-6 flex flex-col min-h-[400px]">
      {/* Title & Tabs */}
      <div className="flex flex-col md:flex-row justify-between items-start md:items-center gap-4 mb-6">
        <h3 className="font-headline-sm text-lg md:text-xl font-bold text-on-surface">Biểu Đồ Khí Hậu Canh Tác (8 Kênh)</h3>
        
        {/* Navigation Tabs */}
        <div className="flex flex-wrap bg-surface-container-lowest/50 border border-glass-border rounded-full p-1 gap-1 w-full md:w-auto overflow-x-auto">
          <button 
            className={`px-3 py-1.5 text-[10px] font-label-caps uppercase rounded-full transition-all duration-200 ${
              activeChartTab === 'temp' 
                ? 'bg-temp-orange/20 border border-temp-orange/30 text-temp-orange font-bold' 
                : 'text-on-surface-variant hover:text-on-surface'
            }`}
            onClick={() => setActiveChartTab('temp')}
          >
            Nhiệt Độ
          </button>
          <button 
            className={`px-3 py-1.5 text-[10px] font-label-caps uppercase rounded-full transition-all duration-200 ${
              activeChartTab === 'hum' 
                ? 'bg-humidity-cyan/20 border border-humidity-cyan/30 text-humidity-cyan font-bold' 
                : 'text-on-surface-variant hover:text-on-surface'
            }`}
            onClick={() => setActiveChartTab('hum')}
          >
            Độ Ẩm
          </button>
          <button 
            className={`px-3 py-1.5 text-[10px] font-label-caps uppercase rounded-full transition-all duration-200 ${
              activeChartTab === 'pres' 
                ? 'bg-primary/20 border border-primary/30 text-primary font-bold' 
                : 'text-on-surface-variant hover:text-on-surface'
            }`}
            onClick={() => setActiveChartTab('pres')}
          >
            Áp Suất
          </button>
          <button 
            className={`px-3 py-1.5 text-[10px] font-label-caps uppercase rounded-full transition-all duration-200 ${
              activeChartTab === 'rain' 
                ? 'bg-air-quality-green/20 border border-air-quality-green/30 text-air-quality-green font-bold' 
                : 'text-on-surface-variant hover:text-on-surface'
            }`}
            onClick={() => setActiveChartTab('rain')}
          >
            Lượng Mưa
          </button>
          <button 
            className={`px-3 py-1.5 text-[10px] font-label-caps uppercase rounded-full transition-all duration-200 ${
              activeChartTab === 'bat' 
                ? 'bg-secondary-fixed-dim/20 border border-secondary-fixed-dim/30 text-secondary-fixed-dim font-bold' 
                : 'text-on-surface-variant hover:text-on-surface'
            }`}
            onClick={() => setActiveChartTab('bat')}
          >
            Vạch Pin
          </button>
        </div>
      </div>

      {/* Chart Canvas Area */}
      <div className="flex-grow h-[280px] w-full relative">
        <Line data={chartData} options={chartOptions} plugins={[verticalLinePlugin]} />
      </div>

      {/* Custom Legend */}
      <div className="flex flex-wrap gap-x-6 gap-y-2 mt-4 font-data-mono text-xs text-on-surface-variant justify-end">
        <div className="flex items-center gap-2">
          <div className="w-3 h-3 rounded-full" style={{ backgroundColor: primaryColor }}></div>
          <span>{primaryLabel}</span>
        </div>
        {showSecondary && (
          <div className="flex items-center gap-2">
            <div className="w-3 h-0.5 border-t border-dashed" style={{ borderColor: secondaryColor, borderWidth: '2px' }}></div>
            <span>{secondaryLabel}</span>
          </div>
        )}
      </div>
    </div>
  );
}
