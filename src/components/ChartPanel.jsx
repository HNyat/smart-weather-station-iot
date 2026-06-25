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
  
  // Prepare labels (timestamps)
  const labels = historyData.timestamps || [];
  
  // Build chart dataset config based on active tab
  let primaryLabel = "";
  let primaryData = [];
  let primaryColor = "#f97316";
  let primaryGradColor = "rgba(249, 115, 22, 0.25)";
  
  let secondaryLabel = "";
  let secondaryData = [];
  let secondaryColor = "#eab308";
  
  let showSecondary = false;

  switch (activeChartTab) {
    case 'temp':
      primaryLabel = 'Nhiệt độ Thực tế (°C)';
      primaryData = historyData.temperature || [];
      primaryColor = '#f97316';
      primaryGradColor = 'rgba(249, 115, 22, 0.25)';
      
      secondaryLabel = 'Dự báo Nhiệt độ Python AI (°C)';
      secondaryData = historyData.predictedTemp || [];
      secondaryColor = '#eab308';
      showSecondary = true;
      break;
      
    case 'hum':
      primaryLabel = 'Độ ẩm (%)';
      primaryData = historyData.humidity || [];
      primaryColor = '#38bdf8';
      primaryGradColor = 'rgba(56, 189, 248, 0.25)';
      showSecondary = false;
      break;
      
    case 'pres':
      primaryLabel = 'Áp suất khí quyển (hPa)';
      primaryData = historyData.pressure || [];
      primaryColor = '#a855f7';
      primaryGradColor = 'rgba(168, 85, 247, 0.25)';
      showSecondary = false;
      break;
      
    case 'rain':
      primaryLabel = 'Lượng Mưa / Chỉ số khô ráo (0-1023)';
      primaryData = historyData.rain || [];
      primaryColor = '#10b981';
      primaryGradColor = 'rgba(16, 185, 129, 0.25)';
      
      secondaryLabel = 'Xác suất mưa Python AI (%)';
      secondaryData = historyData.rainProbability || [];
      secondaryColor = '#06b6d4';
      showSecondary = true;
      break;
      
    case 'bat':
      primaryLabel = 'Nguồn Pin Trạm Đo (%)';
      primaryData = historyData.battery || [];
      primaryColor = '#14b8a6';
      primaryGradColor = 'rgba(20, 184, 166, 0.25)';
      showSecondary = false;
      break;

    default:
      break;
  }

  // Create datasets list
  const datasets = [
    {
      label: primaryLabel,
      data: primaryData,
      borderColor: primaryColor,
      backgroundColor: (context) => {
        const ctx = context.chart.ctx;
        const gradient = ctx.createLinearGradient(0, 0, 0, 250);
        gradient.addColorStop(0, primaryGradColor);
        gradient.addColorStop(1, 'rgba(0,0,0,0)');
        return gradient;
      },
      fill: true,
      tension: 0.4,
      borderWidth: 3,
      pointBackgroundColor: primaryColor,
      pointHoverRadius: 6
    }
  ];

  if (showSecondary) {
    datasets.push({
      label: secondaryLabel,
      data: secondaryData,
      borderColor: secondaryColor,
      borderDash: [4, 4],
      fill: false,
      tension: 0.4,
      borderWidth: 2,
      pointBackgroundColor: secondaryColor
    });
  }

  const chartData = {
    labels,
    datasets
  };

  const chartOptions = {
    responsive: true,
    maintainAspectRatio: false,
    plugins: {
      legend: {
        display: true,
        labels: {
          color: '#94a3b8',
          font: {
            family: 'Plus Jakarta Sans',
            size: 10
          }
        }
      }
    },
    scales: {
      x: {
        grid: {
          color: 'rgba(255, 255, 255, 0.05)'
        },
        ticks: {
          color: '#94a3b8',
          font: {
            family: 'Plus Jakarta Sans',
            size: 10
          }
        }
      },
      y: {
        grid: {
          color: 'rgba(255, 255, 255, 0.05)'
        },
        ticks: {
          color: '#94a3b8',
          font: {
            family: 'Plus Jakarta Sans',
            size: 11
          }
        }
      }
    }
  };

  return (
    <div className="glass-card chart-container-card">
      <div className="chart-header">
        <h2>Biểu Đồ Khí Hậu Canh Tác (8 Kênh)</h2>
        <div className="chart-tabs-wrapper">
          <div className="chart-tabs">
            <button 
              className={`chart-tab ${activeChartTab === 'temp' ? 'active' : ''}`}
              onClick={() => setActiveChartTab('temp')}
            >
              Nhiệt Độ Canh Tác
            </button>
            <button 
              className={`chart-tab ${activeChartTab === 'hum' ? 'active' : ''}`}
              onClick={() => setActiveChartTab('hum')}
            >
              Độ Ẩm Đất/Khí
            </button>
            <button 
              className={`chart-tab ${activeChartTab === 'pres' ? 'active' : ''}`}
              onClick={() => setActiveChartTab('pres')}
            >
              Khí Áp Canh Tác
            </button>
            <button 
              className={`chart-tab ${activeChartTab === 'rain' ? 'active' : ''}`}
              onClick={() => setActiveChartTab('rain')}
            >
              Lượng Mưa
            </button>
            <button 
              className={`chart-tab ${activeChartTab === 'bat' ? 'active' : ''}`}
              onClick={() => setActiveChartTab('bat')}
            >
              Nguồn Điện (Pin)
            </button>
          </div>
        </div>
      </div>
      <div className="chart-wrapper">
        <Line data={chartData} options={chartOptions} />
      </div>
    </div>
  );
}
