#ifndef CONTROL_HTML_H
#define CONTROL_HTML_H

const char control_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="zh">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>坦克控制系统</title>
    <style>
        :root {
            --primary: #3498db;
            --secondary: #2c3e50;
            --danger: #e74c3c;
            --success: #2ecc71;
            --warning: #f39c12;
            --dark: #1a1a2e;
            --light: #f8f9fa;
        }

        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
        }

        body {
            background: linear-gradient(135deg, #1a1a2e, #16213e);
            color: var(--light);
            min-height: 100vh;
            padding: 20px;
            display: flex;
            justify-content: center;
            align-items: center;
        }

        .container {
            max-width: 500px;
            width: 100%;
            background: rgba(255, 255, 255, 0.05);
            backdrop-filter: blur(10px);
            border-radius: 20px;
            box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3);
            border: 1px solid rgba(255, 255, 255, 0.1);
            overflow: hidden;
        }

        header {
            background: rgba(0, 0, 0, 0.2);
            padding: 20px;
            text-align: center;
            border-bottom: 1px solid rgba(255, 255, 255, 0.1);
        }

        header h1 {
            font-size: 1.8rem;
            margin-bottom: 5px;
            color: var(--primary);
            text-shadow: 0 0 10px rgba(52, 152, 219, 0.5);
        }

        .status-bar {
            display: flex;
            justify-content: space-around;
            padding: 15px;
            background: rgba(0, 0, 0, 0.15);
            border-bottom: 1px solid rgba(255, 255, 255, 0.1);
        }

        .status-item {
            text-align: center;
        }

        .status-value {
            font-size: 1.4rem;
            font-weight: bold;
            color: var(--success);
        }

        .control-section {
            padding: 20px;
        }

        .section-title {
            display: flex;
            align-items: center;
            margin-bottom: 15px;
            color: var(--primary);
        }

        .slider-container {
            margin-bottom: 25px;
        }

        .slider-header {
            display: flex;
            justify-content: space-between;
            margin-bottom: 8px;
        }

        .slider-label {
            font-weight: 500;
        }

        input[type="range"] {
            -webkit-appearance: none;
            width: 100%;
            height: 12px;
            border-radius: 10px;
            background: rgba(255, 255, 255, 0.1);
            outline: none;
        }

        input[type="range"]::-webkit-slider-thumb {
            -webkit-appearance: none;
            width: 25px;
            height: 25px;
            border-radius: 50%;
            background: var(--primary);
            cursor: pointer;
            box-shadow: 0 0 10px rgba(52, 152, 219, 0.7);
            border: 2px solid white;
            transition: all 0.3s ease;
        }

        input[type="range"]::-webkit-slider-thumb:hover {
            transform: scale(1.1);
            background: #2980b9;
        }

        #rot {
            background: linear-gradient(to right, #e74c3c, #f1c40f, #2ecc71);
        }

        #elev {
            background: linear-gradient(to right, #2ecc71, #3498db, #9b59b6);
        }

        .button-group {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 15px;
            margin-top: 20px;
        }

        .btn {
            padding: 15px;
            border: none;
            border-radius: 12px;
            font-weight: bold;
            font-size: 1.1rem;
            cursor: pointer;
            transition: all 0.3s ease;
            display: flex;
            justify-content: center;
            align-items: center;
            text-transform: uppercase;
            letter-spacing: 1px;
            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.2);
        }

        .btn:active {
            transform: translateY(2px);
            box-shadow: 0 2px 8px rgba(0, 0, 0, 0.2);
        }

        .btn-stop {
            background: linear-gradient(135deg, #e74c3c, #c0392b);
            color: white;
        }

        /* 显著增大的开火按钮 */
        .btn-fire {
            background: linear-gradient(135deg, #f39c12, #e74c3c);
            color: white;
            width: 100%;
            padding: 20px;
            font-size: 1.3rem;
            margin: 15px 0;
            animation: pulse 2s infinite;
        }

        .btn-direction {
            background: rgba(255, 255, 255, 0.1);
            color: var(--light);
            border: 1px solid rgba(255, 255, 255, 0.2);
        }

        .btn-direction:active {
            background: rgba(255, 255, 255, 0.2);
        }

        @keyframes pulse {
            0% { box-shadow: 0 0 0 0 rgba(231, 76, 60, 0.7); }
            70% { box-shadow: 0 0 0 15px rgba(231, 76, 60, 0); }
            100% { box-shadow: 0 0 0 0 rgba(231, 76, 60, 0); }
        }

        footer {
            text-align: center;
            padding: 15px;
            font-size: 0.9rem;
            color: rgba(255, 255, 255, 0.6);
            border-top: 1px solid rgba(255, 255, 255, 0.1);
        }

        /* 响应式调整 */
        @media (max-width: 480px) {
            body {
                padding: 10px;
            }

            header h1 {
                font-size: 1.5rem;
            }

            .button-group {
                grid-template-columns: 1fr;
            }

            .btn-fire {
                font-size: 1.2rem;
                padding: 18px;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>坦克控制系统</h1>
            <p>远程控制您的坦克炮台</p>
        </header>

        <div class="status-bar">
            <div class="status-item">
                <div>旋转速度</div>
                <div id="rotVal" class="status-value">0%</div>
            </div>
            <div class="status-item">
                <div>俯仰角度</div>
                <div id="elevVal" class="status-value">90°</div>
            </div>
        </div>

        <div class="control-section">
            <div class="section-title">
                <span style="font-size:1.5rem;margin-right:10px;">↻</span>
                <h2>炮台旋转控制</h2>
            </div>

            <div class="slider-container">
                <div class="slider-header">
                    <span class="slider-label">速度控制</span>
                    <span id="rotVal2">0%</span>
                </div>
                <input type="range" min="-100" max="100" value="0" id="rot" class="slider">
            </div>

            <div class="button-group">
                <button class="btn btn-direction" onclick="setRot(-100)">↶ 左转</button>
                <button class="btn btn-stop" onclick="setRot(0)">■ 停止</button>
                <button class="btn btn-direction" onclick="setRot(100)">↷ 右转</button>
            </div>
        </div>

        <div class="control-section">
            <div class="section-title">
                <span style="font-size:1.5rem;margin-right:10px;">↕</span>
                <h2>炮筒俯仰控制</h2>
            </div>

            <div class="slider-container">
                <div class="slider-header">
                    <span class="slider-label">角度控制</span>
                    <span id="elevVal2">90°</span>
                </div>
                <input type="range" min="30" max="150" value="90" id="elev" class="slider">
            </div>

            <div class="button-group">
                <button class="btn btn-direction" onclick="setElev(30)">↓ 最低</button>
                <button class="btn btn-direction" onclick="setElev(90)">↕ 水平</button>
                <button class="btn btn-direction" onclick="setElev(150)">↑ 最高</button>
            </div>
        </div>

        <div class="control-section" style="padding-top: 0;">
            <button id="fireBtn" class="btn btn-fire" onclick="fire()">
                <span style="font-size:1.5rem;margin-right:10px;">🔥</span> 开火
            </button>
        </div>

        <footer>
            <p>坦克控制系统 v1.0 | 当前连接: <span id="ipAddress">192.168.4.1</span></p>
        </footer>
    </div>

    <script>
        // 更新显示值
        function updateDisplay() {
            const rotVal = document.getElementById('rot').value;
            const elevVal = document.getElementById('elev').value;

            document.getElementById('rotVal').innerText = `${rotVal}%`;
            document.getElementById('rotVal2').innerText = `${rotVal}%`;
            document.getElementById('elevVal').innerText = `${elevVal}°`;
            document.getElementById('elevVal2').innerText = `${elevVal}°`;
        }

        // 设置旋转速度
        function setRot(value) {
            document.getElementById('rot').value = value;
            updateDisplay();
            sendRotationRequest(value);
        }

        // 设置俯仰角度
        function setElev(value) {
            document.getElementById('elev').value = value;
            updateDisplay();
            sendElevationRequest(value);
        }

        // 发送旋转请求
        function sendRotationRequest(value) {
            fetch(`/set?rotation=${value}`)
                .catch(error => console.error('旋转请求错误:', error));
        }

        // 发送俯仰请求
        function sendElevationRequest(value) {
            fetch(`/set?elevation=${value}`)
                .catch(error => console.error('俯仰请求错误:', error));
        }

        // 开火命令
        function fire() {
            const btn = document.getElementById('fireBtn');
            btn.disabled = true;
            btn.innerHTML = '<span style="font-size:1.5rem;margin-right:10px;">💥</span> 发射中...';
            document.getElementById('rot').value = 0;

            fetch('/fire')
                .then(response => response.text())
                .then(() => {
                    setTimeout(() => {
                        btn.innerHTML = '<span style="font-size:1.5rem;margin-right:10px;">🔥</span> 开火';
                        btn.disabled = false;
                    }, 2000);
                })
                .catch(error => {
                    console.error('Error:', error);
                    btn.innerHTML = '<span style="font-size:1.5rem;margin-right:10px;">❌</span> 错误!';
                    setTimeout(() => {
                        btn.innerHTML = '<span style="font-size:1.5rem;margin-right:10px;">🔥</span> 开火';
                        btn.disabled = false;
                    }, 2000);
                });
        }

        // 初始化事件监听器
        document.addEventListener('DOMContentLoaded', () => {
            // 旋转滑块事件监听
            document.getElementById('rot').addEventListener('input', function() {
                const value = this.value;
                updateDisplay();
                sendRotationRequest(value);
            });

            // 俯仰滑块事件监听
            document.getElementById('elev').addEventListener('input', function() {
                const value = this.value;
                updateDisplay();
                sendElevationRequest(value);
            });

            updateDisplay();

            // 获取并显示IP地址
            fetch('/ip')
                .then(response => response.text())
                .then(ip => {
                    document.getElementById('ipAddress').textContent = ip;
                })
                .catch(console.error);
        });
    </script>
</body>
</html>
)rawliteral";

#endif