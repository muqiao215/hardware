# 51 MCU Learning Project (STC89C52/AT89S52)

Refactored from "51单片机入门教程资料" (JiangKeDa/Generic 51 Tutorial).

## 📂 Project Structure

```text
E:\web\embedded\51_mcu\
├── docs\
│   ├── manuals\       # Datasheets, Schematics, ISP Tools (Updated 2025)
│   └── ppt\           # Course PPT Slides
├── learning\          # Source Code (Renamed & Structured)
│   ├── lesson_02_01_led_on\
│   └── ...
└── tools\
    └── softwares\     # Programming Software (STC-ISP, etc.)
```

## 📚 Lesson Mapping

| Original Name (Chinese) | New Directory Name | Description |
|-------------------------|-------------------|-------------|
| 2-1 点亮一个LED | `lesson_02_01_led_on` | GPIO Output High/Low |
| 2-2 LED闪烁 | `lesson_02_02_led_blink` | Simple Delay Loop |
| 2-3 LED流水灯 | `lesson_02_03_led_flow` | Bit shifting |
| 2-4 LED流水灯Plus | `lesson_02_04_led_flow_plus` | Array/Function based flow |
| 3-1 独立按键控制LED亮灭 | `lesson_03_01_key_led_on` | GPIO Input (Polling) |
| 3-2 独立按键控制LED状态 | `lesson_03_02_key_led_state` | State toggling |
| 3-3 独立按键控制LED显示二进制 | `lesson_03_03_key_led_bin` | Binary counter |
| 3-4 独立按键控制LED移位 | `lesson_03_04_key_led_shift` | Shift register logic |
| 4-1 静态数码管显示 | `lesson_04_01_static_segment` | 7-Segment Static |
| 4-2 动态数码管显示 | `lesson_04_02_dynamic_segment` | 7-Segment Multiplexing |
| 5-1 模块化编程 | `lesson_05_01_modular_programming` | .c/.h file separation |
| 5-2 LCD1602调试工具 | `lesson_05_02_lcd1602_debug` | LCD driver integration |
| 6-1 矩阵键盘 | `lesson_06_01_matrix_key` | Matrix Keypad Scanning |
| 6-2 矩阵键盘密码锁 | `lesson_06_02_matrix_key_lock` | Password logic |
| 7-1 按键控制LED流水灯模式 | `lesson_07_01_key_led_flow_timer` | Timer interrupt basics |
| 7-2 定时器时钟 | `lesson_07_02_timer_clock` | Real-time clock via Timer |
| 8-1 串口向电脑发送数据 | `lesson_08_01_uart_send` | UART TX |
| 8-2 电脑通过串口控制LED | `lesson_08_02_uart_control_led` | UART RX |
| 9-1 LED点阵屏显示图形 | `lesson_09_01_led_matrix_graphic` | 8x8 Matrix (74HC595) |
| 9-2 LED点阵屏显示动画 | `lesson_09_02_led_matrix_anim` | Animation frames |
| 10-1 DS1302时钟 | `lesson_10_01_ds1302` | RTC Module |
| 10-2 DS1302可调时钟 | `lesson_10_02_ds1302_adjust` | RTC with Keys |
| 11-1 蜂鸣器播放提示音 | `lesson_11_01_buzzer_beep` | Passive/Active Buzzer |
| 11-2 蜂鸣器播放音乐 | `lesson_11_02_buzzer_music` | Frequency generation |
| 12-1 AT24C02数据存储 | `lesson_12_01_at24c02` | I2C EEPROM |
| 12-2 秒表(定时器扫描按键数码管) | `lesson_12_02_stopwatch` | Integrated timer project |
| 13-1 DS18B20温度读取 | `lesson_13_01_ds18b20` | 1-Wire Temp Sensor |
| 13-2 DS18B20温度报警器 | `lesson_13_02_ds18b20_alarm` | Temp Threshold Logic |
| 14-1 LCD1602液晶显示屏 | `lesson_14_01_lcd1602` | Display Driver |
| 15-1 LED呼吸灯 | `lesson_15_01_led_breath` | PWM (Software) |
| 15-2 直流电机调速 | `lesson_15_02_motor_pwm` | Motor Control |
| 16-1 AD模数转换 | `lesson_16_01_ad` | ADC (XPT2046/ADC0804) |
| 16-2 DA数模转换 | `lesson_16_02_da` | DAC (PWM/DAC0832) |
| 17-1 红外遥控 | `lesson_17_01_ir_remote` | NEC Protocol (VS1838B) |
| 17-2 红外遥控电机调速 | `lesson_17_02_ir_motor` | IR Integration |

## 🛠 Toolchain
- **Original Source**: Keil C51 (Target: AT89C52/STC89C52)
- **Migration Goal**: Compatible with SDCC + VS Code (Work in Progress)

```