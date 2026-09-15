# STM32 课程综合项目（多 APP）

该工程位于 `stm32/projects/course_capstone/`，基于课程既有模块与接线习惯，使用 `-DAPP=<name>` 选择不同固件进行构建/烧录，尽量做到对 `stm32/learning/lesson_*` 零侵入。

## APP 列表

- `gesture_arm`：手势/编码器控制机械臂（3 路舵机 + 编码器 + 串口 + 内部 Flash 存档）
- `smart_home_gateway`：智能家居网关（ADC+DMA + 串口 + RTC + 低功耗 + 看门狗 + Flash）
- `balance_ball`：MPU6050 平衡控制演示（I2C + 双电机 PWM + 双编码器 + PID + ADC+DMA）
- `bluetooth_car`：蓝牙遥控小车（串口/HC-05 + 双电机 PWM + 双编码器 + ADC + 看门狗）
- `music_light`：音乐律动灯（ADC+DMA 采样 + 频域能量分析 + PWM 灯效 + 定时刷新）
- `pomodoro`：倒计时番茄钟（RTC/定时 + OLED + 按键 + 蜂鸣器 + Flash 存档）
- `self_test`：一键自检/生产测试（OLED/按键/蜂鸣器/编码器/MPU/ADC/RTC/W25Q64/对射计次）
- `data_logger`：时间戳数据记录器（MPU6050 + ADC + W25Q64，串口 CSV 导出）
- `low_power_sentry`：低功耗前哨站（RTC Alarm 唤醒 + STOP 模式 + ADC 采样）
- `light_alarm`：光敏暗光报警器（PA2 光敏 + PA1 按键静音 + PA0 蜂鸣器 + OLED）
- `multi_sensor_panel`：多传感器面板（PA2 光敏 + PA3 TCRT5000 + PA4 DHT11 + PB10/PB11 MPU6050 + PA1 按键 + PA0 蜂鸣器 + OLED）
- `inclinometer`：电子水平仪/倾角仪（MPU6050 + 校准存档）
- `motor_pid_tuner`：电机闭环调参台（编码器测速 + PID，串口在线调参）
- `spectrum_oled`：频谱显示（音频 ADC+DMA + 8 段能量 + OLED + PWM）
- `habit_tracker`：事件/习惯统计（PB14 外部事件计数 + RTC 按天滚动存档）

## 构建

```bash
cd stm32/projects/course_capstone
cmake -S . -B build \
  -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-gcc.cmake \
  -DBOARD=stm32f103c8t6 \
  -DAPP=pomodoro \
  -G Ninja
cmake --build build
```

## 烧录

```bash
cmake --build build --target flash
```

## 说明

- 默认板卡：`stm32f103c8t6`（BluePill）
- OLED：PB8/PB9（软件 I2C，4-pin OLED）
- 串口：USART1（PA9/PA10，默认 9600）
- 内部 Flash 存储：保留最后 1KB 页（`0x0800FC00`）用于参数/存档

## 引脚与接线（建议）

> 为了让多个 APP 尽量复用同一套接线，本工程在电机/编码器上使用固定引脚；个别 APP（如 `music_light`、`smart_home_gateway`）会占用 ADC 引脚，建议按 APP 单独接线或用跳线切换。

**通用**
- OLED：PB8(SCL) / PB9(SDA)
- 串口：PA9(TX) / PA10(RX)

**舵机（`gesture_arm`）**
- 舵机信号：PA0(TIM2_CH1) / PA1(TIM2_CH2) / PA2(TIM2_CH3)

**双电机（`bluetooth_car` / `balance_ball`）**
- PWM：PA0(TIM2_CH1)=Left_EN、PA1(TIM2_CH2)=Right_EN
- 方向：PB0/PB1=Left_IN1/IN2，PB12/PB13=Right_IN3/IN4

**双编码器（`gesture_arm` / `bluetooth_car` / `balance_ball`）**
- 编码器1（TIM3 Encoder）：PA6/PA7
- 编码器2（TIM4 Encoder）：PB6/PB7

**ADC**
- `music_light` 音频输入：PA3(ADC_CH3)
- `spectrum_oled` 音频输入：PA3(ADC_CH3)
- `light_alarm` 光敏输入：PA2(ADC_CH2)
- `multi_sensor_panel`：PA2(光敏 AO) / PA3(TCRT5000 AO)，PA4 为 DHT11 DATA 数字单总线
- `bluetooth_car`/`balance_ball` 默认启用 ADC：PA2/PA3（你可按传感器实际接线改 `main_*.c` 里的通道数组）
- `smart_home_gateway` 默认启用 ADC：PA0/PA1/PA2/PA3

**蜂鸣器 / 按键（`light_alarm` / `multi_sensor_panel`）**
- 蜂鸣器：PA0，低电平触发
- 按键：PA1，按下接地触发

**陀螺仪（`multi_sensor_panel`）**
- MPU6050：PB10(SCL) / PB11(SDA)，AD0 接 GND

**W25Q64（`self_test` / `data_logger`）**
- 软件 SPI：PA4(CS) / PA5(SCK) / PA6(MISO) / PA7(MOSI)

**对射计次（`self_test` / `habit_tracker`）**
- PB14（EXTI 下降沿计数）
