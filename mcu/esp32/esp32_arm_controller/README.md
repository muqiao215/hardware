# ESP32 机械臂控制器 - 双模固件

## 功能

- **WiFi AP 模式**: 设备创建名为 `RobotArm_AP` 的热点
- **WebSocket 服务器**: 端口 81，路径 `/ws`
- **BLE GATT 服务**: Nordic UART Service 兼容
- **指令兼容**: 支持原有 `#1P1500T100!` 格式

## 硬件需求

- ESP32 开发板 (ESP32-WROOM-32 或类似)
- 6 路舵机 (与 Arduino 机械臂相同)

## 编译上传

### 使用 PlatformIO

```bash
# 编译
pio run

# 上传
pio run --target upload

# 串口监视
pio device monitor
```

### 使用 Arduino IDE

1. 安装 ESP32 开发板支持
2. 安装库: `WebSockets`, `ESP32Servo`
3. 选择开发板: ESP32 Dev Module
4. 上传

## WiFi 连接

1. 手机/电脑连接热点 `RobotArm_AP` (密码: `12345678`)
2. 打开 Android App，选择 WiFi 连接
3. 输入 IP: `192.168.4.1`, 端口: `81`

## BLE 连接

1. 打开 Android App，选择 BLE 连接
2. 扫描设备，选择 `RobotArm_ESP32`
3. 自动连接 GATT 服务

## 指令格式

```
#<ID>P<PWM>T<TIME>!

- ID: 舵机编号 (1-6)
- PWM: 脉宽值 (500-2500)
- TIME: 运动时间 (ms)

示例: #1P1500T100!  (1号舵机移动到中位，耗时100ms)
```

## 特殊指令

- `STATUS` - 返回所有舵机当前位置
- `CENTER` - 所有舵机回中位
