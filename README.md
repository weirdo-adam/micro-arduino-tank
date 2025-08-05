# 智能坦克控制系统 🚀

基于 **ESP8266** 的无线坦克炮台控制系统，支持通过 **Web 界面** 实时控制舵机旋转、俯仰和模拟开火。

---

## ✨ 功能特性
- **📶 WiFi 远程控制**
  支持手机/PC 浏览器访问，无需安装额外应用。
- **🎮 双舵机控制**
  - 炮台 **无级变速旋转**（带加速度控制）。
  - 炮管 **精准俯仰**（30°-150° 范围）。
- **🔥 模拟开火特效**
  LED 视觉反馈，增强交互体验。
- **💾 俯仰角度记忆**
  使用 EEPROM 存储最后一次俯仰角度，断电不丢失。
- **📱 自适应 Web 界面**
  兼容手机和 PC，操作直观。

---

## 🛠️ 硬件组成
| 组件         | 型号/参数          | 连接引脚 |
|--------------|-------------------|----------|
| **主控芯片**  | ESP8266 (NodeMCU) | -        |
| **旋转舵机**  | SG90 (180°)       | D4       |
| **俯仰舵机**  | MG996R (270°)     | D5       |
| **LED 指示灯**| 板载 LED          | Default  |

---

## 🚀 快速开始
### 1. 环境准备
- **开发工具**
  - [PlatformIO CLI](https://docs.platformio.org/en/latest/core/index.html)（推荐）或 Arduino IDE。
  - 安装依赖库：
    ```bash
    ESP8266 Core (3.1.2)
    Servo.h (内置)
    ESP8266WebServer.h (内置)
    WiFiManager.h (需手动安装)
    ```

### 2. 烧录步骤
#### 通过 PlatformIO
1. 克隆仓库：
   ```bash
   git clone git@github.com:weirdo-adam/micro-tank.git
   ```
2. 构建并上传：
   ```bash
   pio run -t upload -e nodemcuv2
   ```
3. 上传并监控串口输出：
   ```bash
   pio run -t upload -t monitor -e nodemcuv2
   ```

#### 通过 Arduino IDE
1. 打开 `main.cpp`。
2. 选择开发板：`NodeMCU 1.0 (ESP-12E Module)`。
3. 设置上传速度：`115200`。
4. 点击 **上传**。

### 3. 使用指南
1. **上电后**，设备将创建 WiFi AP：`Tank-Control-System`。
2. **连接该 WiFi**，默认无密码。
3. **访问控制界面**：
   - 默认地址：`http://192.168.4.1`（IP 会在串口输出）。
4. **操作说明**：
   - 🕹️ **左摇杆**：控制炮台旋转速度和方向。
   - 🎚️ **右滑块**：调节俯仰角度。
   - 🔘 **红色按钮**：触发开火动画（LED 闪烁）。

---

## ⚙️ 高级配置
### 关键参数调整
编辑 `config.h` 文件：
```cpp
const int ROTATION_ACCELERATION = 3;  // 旋转加速度（值越大加速越快）
const int ELEVATION_STEP = 1;         // 俯仰步进角度
const int MIN_ELEVATION_ANGLE = 30;   // 炮管最低角度
```

### 舵机校准
1. **旋转舵机中位校准**：
   ```cpp
   turretRotation.writeMicroseconds(1500); // 发送停止信号
   ```
2. **俯仰舵机限位保护**：
   ```cpp
   cannonElevation.write(90); // 上电初始位置
   ```

---

## 🔧 故障排查
| 现象            | 可能原因               | 解决方案                     |
|-----------------|-----------------------|-----------------------------|
| **无法连接 WiFi** | WiFiManager 配置失败   | 检查 ESP8266 天线连接        |
| **舵机抖动**     | 电源功率不足          | 使用外接 5V 2A 电源          |
| **网页无法加载** | 浏览器缓存问题        | 按 `Ctrl+F5` 强制刷新        |

---

## 📜 开源协议
本项目基于 **MIT License**，欢迎贡献代码或提出建议！

---

## 🌟 贡献者
- [Weirdo Adam](https://github.com/weirdo-adam)

```
