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
      tension: 0.4, // Tạo đường cong mềm mại hơn
      borderWidth: 2.5,
      pointRadius: 0, // Ẩn các chấm tròn mặc định để đường đồ thị trơn và chuyên nghiệp hơn
      pointHoverRadius: 6,
      pointHoverBackgroundColor: primaryColor,
      pointHoverBorderColor: '#ffffff',
      pointHoverBorderWidth: 2,
      pointHitRadius: 15
    }
  ];

  if (showSecondary) {
    datasets.push({
      label: secondaryLabel,
      data: alignedPredicted,
      borderColor: secondaryColor,
      borderDash: [6, 4], // Đường đứt nét dự báo AI đẹp hơn
      fill: false,
      tension: 0.4,
      borderWidth: 2,
      pointRadius: 0,
      pointHoverRadius: 6,
      pointHoverBackgroundColor: secondaryColor,
      pointHoverBorderColor: '#ffffff',
      pointHoverBorderWidth: 2,
      pointHitRadius: 15
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
        display: false
      },
      tooltip: {
        backgroundColor: 'rgba(17, 17, 19, 0.95)',
        titleColor: '#ffffff',
        bodyColor: 'rgba(255, 255, 255, 0.85)',
        borderColor: 'rgba(255, 255, 255, 0.08)',
        borderWidth: 1,
        borderRadius: 12,
        padding: 12,
        titleFont: {
          family: 'Sora',
          size: 12,
          weight: '600'
        },
        bodyFont: {
          family: 'Inter',
          size: 11
        },
        boxWidth: 8,
        boxHeight: 8,
        boxPadding: 6,
        usePointStyle: true,
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
          display: false // Ẩn hoàn toàn lưới dọc giúp biểu đồ thoáng hơn
        },
        ticks: {
          color: 'rgba(255, 255, 255, 0.4)',
          maxRotation: 0,
          autoSkip: true,
          maxTicksLimit: 8,
          padding: 8,
          font: {
            family: 'Inter',
            size: 10,
            weight: '500'
          }
        }
      },
      y: {
        grid: {
          color: 'rgba(255, 255, 255, 0.04)', // Lưới ngang mờ ảo
          drawTicks: false
        },
        border: {
          display: false // Ẩn đường viền trục Y
        },
        ticks: {
          color: 'rgba(255, 255, 255, 0.4)',
          padding: 12,
          font: {
            family: 'Inter',
            size: 10,
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
        ctx.strokeStyle = 'rgba(255, 255, 255, 0.1)';
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
        <div className="flex flex-col gap-1">
          <h3 className="font-headline-sm text-lg md:text-xl font-bold text-on-surface">Biểu Đồ Khí Hậu Canh Tác</h3>
          <p className="text-[11px] text-on-surface-variant">So sánh thực tế với dự báo AI (+1h) và kéo dài tương lai</p>
        </div>
        
        {/* Navigation Tabs */}
        <div className="flex flex-wrap bg-surface-container/60 border border-glass-border/30 rounded-xl p-1 gap-1 w-full md:w-auto overflow-x-auto">
          <button 
            className={`px-3.5 py-1.5 text-[10px] font-bold rounded-lg transition-all duration-200 ${
              activeChartTab === 'temp' 
                ? 'bg-temp-orange/15 text-temp-orange border border-temp-orange/30 shadow-[0_2px_8px_rgba(249,115,22,0.1)]' 
                : 'text-on-surface-variant hover:text-on-surface hover:bg-surface-container-highest/40'
            }`}
            onClick={() => setActiveChartTab('temp')}
          >
            Nhiệt Độ
          </button>
          <button 
            className={`px-3.5 py-1.5 text-[10px] font-bold rounded-lg transition-all duration-200 ${
              activeChartTab === 'hum' 
                ? 'bg-humidity-cyan/15 text-humidity-cyan border border-humidity-cyan/30 shadow-[0_2px_8px_rgba(0,242,255,0.1)]' 
                : 'text-on-surface-variant hover:text-on-surface hover:bg-surface-container-highest/40'
            }`}
            onClick={() => setActiveChartTab('hum')}
          >
            Độ Ẩm
          </button>
          <button 
            className={`px-3.5 py-1.5 text-[10px] font-bold rounded-lg transition-all duration-200 ${
              activeChartTab === 'pres' 
                ? 'bg-primary/15 text-primary border border-primary/30 shadow-[0_2px_8px_rgba(249,115,22,0.1)]' 
                : 'text-on-surface-variant hover:text-on-surface hover:bg-surface-container-highest/40'
            }`}
            onClick={() => setActiveChartTab('pres')}
          >
            Áp Suất
          </button>
          <button 
            className={`px-3.5 py-1.5 text-[10px] font-bold rounded-lg transition-all duration-200 ${
              activeChartTab === 'rain' 
                ? 'bg-air-quality-green/15 text-air-quality-green border border-air-quality-green/30 shadow-[0_2px_8px_rgba(16,185,129,0.1)]' 
                : 'text-on-surface-variant hover:text-on-surface hover:bg-surface-container-highest/40'
            }`}
            onClick={() => setActiveChartTab('rain')}
          >
            Lượng Mưa
          </button>
          <button 
            className={`px-3.5 py-1.5 text-[10px] font-bold rounded-lg transition-all duration-200 ${
              activeChartTab === 'bat' 
                ? 'bg-secondary-fixed-dim/15 text-secondary-fixed-dim border border-secondary-fixed-dim/30 shadow-[0_2px_8px_rgba(255,255,255,0.05)]' 
                : 'text-on-surface-variant hover:text-on-surface hover:bg-surface-container-highest/40'
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
      <div className="flex flex-wrap gap-x-4 gap-y-2 mt-5 justify-end font-data-mono text-[10px]">
        <div className="flex items-center gap-2 px-2.5 py-1 bg-surface-container-lowest/30 border border-glass-border/30 rounded-full">
          <div className="w-2 h-2 rounded-full" style={{ backgroundColor: primaryColor }}></div>
          <span className="text-on-surface">{primaryLabel}</span>
        </div>
        {showSecondary && (
          <div className="flex items-center gap-2 px-2.5 py-1 bg-surface-container-lowest/30 border border-glass-border/30 rounded-full">
            <div className="w-2.5 h-0.5 border-t border-dashed" style={{ borderColor: secondaryColor, borderWidth: '2px' }}></div>
            <span className="text-on-surface">{secondaryLabel}</span>
          </div>
        )}
      </div>
    </div>
  );
}
