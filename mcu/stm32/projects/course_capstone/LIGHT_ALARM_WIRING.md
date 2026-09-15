# multi_sensor_panel 接线

## Wokwi 文件

- `diagram.json`：可视化接线图
- `wokwi.toml`：指向 `build_multi_sensor_panel/firmware.hex` 和 `firmware.elf`

## 外设接线

| 模块 | 模块引脚 | Blue Pill 引脚 | 说明 |
| --- | --- | --- | --- |
| OLED SSD1306 | VCC | 3V3 | 3.3V 供电 |
| OLED SSD1306 | GND | GND | 地 |
| OLED SSD1306 | SCL | PB8 | 软件 I2C 时钟 |
| OLED SSD1306 | SDA | PB9 | 软件 I2C 数据 |
| 蜂鸣器 | + | 3V3 | 低电平触发模块电源端 |
| 蜂鸣器 | - / I/O | PA0 | PA0 输出低电平时响 |
| 按键 | 一端 | PA1 | 程序内部上拉 |
| 按键 | 另一端 | GND | 按下接地 |
| 光敏传感器 | VCC | 3V3 | 3.3V 供电 |
| 光敏传感器 | GND | GND | 地 |
| 光敏传感器 | AO | PA2 | ADC1 Channel 2 |
| TCRT5000 | VCC | 3V3 | 3.3V 供电 |
| TCRT5000 | GND | GND | 地 |
| TCRT5000 | AO | PA3 | ADC1 Channel 3 |
| DHT11 温湿度模块 | VCC | 3V3 | 3.3V 供电 |
| DHT11 温湿度模块 | GND | GND | 地 |
| DHT11 温湿度模块 | DATA | PA4 | 数字单总线，不是 ADC |
| MPU6050 | VCC | 3V3 | 3.3V 供电 |
| MPU6050 | GND | GND | 地 |
| MPU6050 | SCL | PB10 | I2C2 SCL |
| MPU6050 | SDA | PB11 | I2C2 SDA |
| MPU6050 | AD0 | GND | 地址 0x68 |

## 当前程序行为

- 第 1 页显示综合状态：光敏、TCRT5000、DHT11 温湿度。
- 第 2 页显示 `PA2/PA3` 原始 ADC 数值和百分比，以及 DHT11 温湿度。
- 第 3 页显示报警阈值。
- 第 4 页显示 MPU6050 三轴加速度：AX/AY/AZ。
- 第 5 页显示 MPU6050 三轴陀螺仪：GX/GY/GZ。
- 第 6 页显示 MPU6050 姿态估算：Roll/Pitch。
- 短按 `PA1` 切换页面。
- 长按 `PA1` 切换蜂鸣器静音。
- `PA2 < 1800`、`PA3 > 2200`、DHT11 温度 `>=35C` 或湿度 `>=85%` 时触发报警，若未静音则 `PA0` 低电平驱动蜂鸣器。

## Wokwi 注意

Wokwi 的 STM32 Blue Pill 仿真目前不完整支持 DMA；当前真板固件使用 `ADC_DMA`，适合实际硬件烧录。`diagram.json` 主要用于接线可视化。
