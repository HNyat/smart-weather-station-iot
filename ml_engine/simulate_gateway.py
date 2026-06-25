import sys
import io
import json
import random
import time
from http.server import HTTPServer, BaseHTTPRequestHandler
from urllib.parse import urlparse, parse_qs

# Cấu hình UTF-8 cho stdout/stderr để tránh lỗi UnicodeEncodeError trên terminal Windows
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')
sys.stderr = io.TextIOWrapper(sys.stderr.buffer, encoding='utf-8')

PORT = 5000

class MockGatewayHandler(BaseHTTPRequestHandler):
    relay_state = 0
    seq_counter = 0

    # Tắt tính năng log mặc định của http.server để tự in log đẹp hơn
    def log_message(self, format, *args):
        pass

    def do_OPTIONS(self):
        """Xử lý Preflight Request cho CORS"""
        self.send_response(200, "ok")
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, OPTIONS, POST')
        self.send_header("Access-Control-Allow-Headers", "X-Requested-With, Content-Type")
        self.end_headers()

    def do_GET(self):
        """Xử lý các truy vấn GET từ Dashboard"""
        if self.path == '/data':
            MockGatewayHandler.seq_counter = (MockGatewayHandler.seq_counter + 1) % 256
            
            # Sinh số liệu cảm biến thực tế
            # Giả lập cảm biến nhiệt độ & độ ẩm
            temp = round(28.5 + random.uniform(-1.5, 1.5), 1)
            hum = round(74.0 + random.uniform(-4.0, 4.0), 1)
            pres = round(1011.2 + random.uniform(-0.6, 0.6), 1)
            
            # Giả lập mưa (1023 là khô ráo, nhỏ hơn 700 là có mưa)
            # 15% cơ hội có mưa dông
            if random.random() > 0.85:
                rain = random.randint(250, 650)
            else:
                rain = random.randint(1010, 1023)
                
            battery = random.randint(95, 100)
            rssi = random.randint(-72, -60)
            
            data = {
                "temperature": temp,
                "humidity": hum,
                "pressure": pres,
                "rain": rain,
                "battery_pct": battery,
                "rssi": rssi,
                "seq_num": MockGatewayHandler.seq_counter
            }
            
            response_json = json.dumps(data)
            
            # Trả phản hồi HTTP kèm CORS header
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.send_header('Access-Control-Allow-Origin', '*')
            self.end_headers()
            self.wfile.write(response_json.encode('utf-8'))
            
            print(f"[GET /data] - Gửi dữ liệu: Temp={temp}°C, Hum={hum}%, Pres={pres}hPa, Rain={rain}, RSSI={rssi}dBm, Seq={MockGatewayHandler.seq_counter}")
            
        elif self.path.startswith('/control'):
            # Xử lý lệnh điều khiển relay của máy bơm/thiết bị từ Dashboard
            parsed_path = urlparse(self.path)
            params = parse_qs(parsed_path.query)
            
            if 'relay' in params:
                relay_val = params['relay'][0]
                MockGatewayHandler.relay_state = int(relay_val)
                state_str = "BẬT 🟢" if MockGatewayHandler.relay_state == 1 else "TẮT 🔴"
                print(f"[GET /control] - Nhận lệnh điều khiển Relay: {state_str}")
            
            data = {
                "status": "ok",
                "relay": MockGatewayHandler.relay_state
            }
            response_json = json.dumps(data)
            
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.send_header('Access-Control-Allow-Origin', '*')
            self.end_headers()
            self.wfile.write(response_json.encode('utf-8'))
            
        else:
            self.send_response(404)
            self.send_header('Access-Control-Allow-Origin', '*')
            self.end_headers()

def main():
    server_address = ('', PORT)
    httpd = HTTPServer(server_address, MockGatewayHandler)
    print("====================================================")
    print("      MOCK LOCAL GATEWAY SERVER (CORS ENABLED)     ")
    print("====================================================")
    print(f"Server giả lập Local Gateway đang chạy tại cổng {PORT}...")
    print(f"Địa chỉ API dữ liệu: http://localhost:{PORT}/data")
    print(f"Địa chỉ API điều khiển: http://localhost:{PORT}/control")
    print("----------------------------------------------------")
    print("HƯỚNG DẪN CẤU HÌNH TRÊN REACT DASHBOARD:")
    print(f"1. Nhấn nút Bánh răng cài đặt (⚙️) ở góc trên bên phải UI.")
    print(f"2. Đổi 'Địa chỉ IP ESP8266' từ '192.168.4.1' thành 'localhost:{PORT}'")
    print(f"3. Lưu lại và chọn nguồn cấp dữ liệu là 'Local (ESP8266)'.")
    print("----------------------------------------------------")
    print("Nhấn Ctrl + C để dừng server.")
    print("----------------------------------------------------")
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\nĐã dừng Mock Local Gateway Server.")

if __name__ == "__main__":
    main()
