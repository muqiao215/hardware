# PWM 脉冲宽度调制详解

## PWM 是什么？

PWM = **Pulse Width Modulation** = 脉冲宽度调制

**一句话解释**：PWM 是通过快速开关控制"平均效果"的方法。

---

## 直观类比

想象一个房间的灯：
- **普通开关**：要么全亮，要么全灭
- **PWM 调光**：每秒开关 1000 次，控制"开"的时间比例
  - 开 10%  → 很暗
  - 开 50%  → 中等亮度
  - 开 100% → 最亮

人眼反应不过来，看到的是"平均亮度"。

---

## PWM 波形图

```
占空比 50%（50% 高电平，50% 低电平）：
    ┌───┐     ┌───┐     ┌───┐
HIGH   │   │     │   │     │   │
LOW    ─┘   ──────┘   ──────┘   ──
        ├─────┤
      高电平 50%  低电平 50%

占空比 25%（25% 高电平，75% 低电平）：
    ┌─┐     ┌─┐     ┌─┐
HIGH   │ │     │ │     │ │
LOW    ─┘ ──────┘ ──────┘ ──
        ├─┤
      高 25%  低 75%
```

---

## 三个关键概念

| 概念 | 英文 | 含义 | 公式 |
|------|------|------|------|
| **频率** | Frequency | 每秒开关多少次 | f = 1 / T |
| **周期** | Period | 开一次关一次多长时间 | T = 1 / f |
| **占空比** | Duty Cycle | "开"时间占比 | Duty = 高电平时间 / 周期 × 100% |

---

## PWM 在 STM32 中的实现

### 定时器四个寄存器回顾

| 寄存器 | 全名 | 作用 | PWM 中的作用 |
|--------|------|------|-------------|
| **PSC** | PreScaler | 预分频器 | 减慢时钟频率 |
| **CNT** | Counter | 计数器 | 不断循环 0 → ARR |
| **ARR** | Auto-Reload Register | 自动重装载 | 决定 PWM 周期 |
| **CCR** | Capture/Compare Register | 捕获/比较 | 决定占空比（分界线） |

---

### STM32 PWM 工作原理

```
┌─────────────────────────────────────────────────────────────────┐
│           定时器产生 PWM 的硬件逻辑                          │
├─────────────────────────────────────────────────────────────────┤
│                                                             │
│   CNT 计数器不断循环：0, 1, 2, ..., ARR → 0, 1, ...    │
│                                                             │
│   每个 PWM 周期内，硬件自动比较 CNT 和 CCR：            │
│                                                             │
│       CNT < CCR  → 输出 HIGH (PWM 引脚变高电平)         │
│       CNT ≥ CCR  → 输出 LOW  (PWM 引脚变低电平)         │
│                                                             │
│   CCR 越大 → HIGH 时间越长 → 占空比越高                  │
│   ARR 越大 → 周期越长 → 频率越低                      │
│                                                             │
└─────────────────────────────────────────────────────────────────┘
```

---

### PWM 代码实现

#### GPIO 配置（关键！）

```c
// 复用推挽输出模式
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // ★ AF_PP = 复用模式
GPIO_Init(GPIOA, &GPIO_InitStructure);
```

**为什么是 AF_PP？**
- 普通推挽 `GPIO_Mode_Out_PP` → 软件控制引脚电平
- 复用推挽 `GPIO_Mode_AF_PP` → 定时器硬件自动控制引脚电平

---

### 时基单元配置

```c
TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;

// PWM 频率 = 72MHz / (PSC+1) / (ARR+1)
TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;      // ARR
TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1;   // PSC

TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
```

---

### 输出比较配置（PWM 核心）

```c
TIM_OCInitTypeDef TIM_OCInitStructure;
TIM_OCStructInit(&TIM_OCInitStructure);

TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;       // ★ PWM 模式1
TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;   // 高电平有效
TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 输出使能
TIM_OCInitStructure.TIM_Pulse = 0;                      // 初始 CCR = 0

TIM_OC2Init(TIM2, &TIM_OCInitStructure);  // 使用通道 2
```

---

### 动态修改占空比

```c
void PWM_SetCompare2(uint16_t Compare)
{
    TIM_SetCompare2(TIM2, Compare);  // 直接修改 CCR 寄存器
}
```

**硬件会立即应用新的占空比**，无需重新配置定时器。

---

## 三种 PWM 应用场景

### 1. LED 呼吸灯（Lesson 6-3）

| 参数 | 值 |
|------|------|
| 目的 | 控制亮度 |
| 引脚 | PA0 (TIM2_CH1) |
| PSC | 719 |
| ARR | 99 |
| PWM 频率 | 1kHz (1ms 周期) |
| CCR 范围 | 0-100 |
| 占空比 | 0-100% |

**原理**：占空比越高，LED 越亮

```c
for (i = 0; i <= 100; i++)  // 渐亮
{
    PWM_SetCompare1(i);          // CCR 0 → 100
    Delay_ms(10);
}
```

---

### 2. 舵机角度控制（Lesson 6-4）

| 参数 | 值 |
|------|------|
| 目的 | 控制角度 |
| 引脚 | PA1 (TIM2_CH2) |
| PSC | 71 |
| ARR | 19999 |
| PWM 频率 | 50Hz (20ms 周期) |
| CCR 范围 | 500-2500 |
| 占空比 | 2.5-12.5% |

**原理**：高电平脉冲宽度决定角度

```
0.5ms 高电平 → 0°   (最左边)
1.5ms 高电平 → 90°  (中间位置)
2.5ms 高电平 → 180° (最右边)
```

#### 角度到 CCR 转换公式（核心代码！）

```c
void Servo_SetAngle(float Angle)
{
    // CCR = Angle / 180 × 2000 + 500
    //
    // 验证：
    //   0°  → 0/180 × 2000 + 500 = 500   (0.5ms 高电平)
    //   90° → 90/180 × 2000 + 500 = 1500  (1.5ms 高电平)
    //   180° → 180/180 × 2000 + 500 = 2500  (2.5ms 高电平)

    PWM_SetCompare2((uint16_t)(Angle / 180 * 2000 + 500));
}
```

#### 公式推导

```
已知：
- 角度范围：0° ~ 180°
- 脉宽范围：0.5ms ~ 2.5ms
- CCR 范围：500 ~ 2500 (在 50Hz, 20ms 周期下)

公式：CCR = (角度 - 最小角度) / (最大角度 - 最小角度) × (最大CCR - 最小CCR) + 最小CCR

代入：CCR = (Angle - 0) / (180 - 0) × (2500 - 500) + 500
简化：CCR = Angle / 180 × 2000 + 500
```

---

### 3. 直流电机速度控制（Lesson 6-5）

| 参数 | 值 |
|------|------|
| 目的 | 控制转速 |
| 引脚 | PA0 (TIM2_CH1) |
| PSC | 719 |
| ARR | 99 |
| PWM 频率 | 10kHz-20kHz |
| CCR 范围 | 0-100 |
| 占空比 | 0-100% |

**原理**：占空比越高，电机速度越快

---

## PWM 计算公式汇总

### 频率与周期

```
fPWM = 72MHz / (PSC + 1) / (ARR + 1)

TPWM = 1 / fPWM

例 6-3 (LED 呼吸灯）：
fPWM = 72,000,000 / 720 / 100 = 1000 Hz
TPWM = 1 / 1000 = 0.001s = 1ms

例 6-4 (舵机）：
fPWM = 72,000,000 / 72 / 20000 = 50 Hz
TPWM = 1 / 50 = 0.02s = 20ms
```

### 占空比与高电平时间

```
占空比 = CCR / (ARR + 1) × 100%

高电平时间 = 占空比 × TPWM

例 6-4 (舵机, CCR=1500）：
占空比 = 1500 / 20000 × 100% = 7.5%
高电平时间 = 7.5% × 20ms = 1.5ms  → 对应 90°
```

---

## 关键记忆点

1. **PSC = 减速齿轮**：把 72MHz 变慢
2. **ARR = 终点线**：决定 PWM 周期（频率）
3. **CNT = 跑步者**：不断循环 0 → ARR
4. **CCR = 分界线**：CNT < CCR 输出 HIGH，否则 LOW
5. **GPIO_Mode_AF_PP**：复用模式，引脚控制权交给定时器
6. **TIM_OCMode_PWM1**：PWM 模式 1，硬件自动比较输出
7. **TIM_SetCompareX()**：实时修改 CCR，改变占空比

---

## CCR 决定占空比的原理

```
┌─────────────────────────────────────────────────────────────────┐
│              CNT 循环 + CCR 比较 = PWM 输出               │
├─────────────────────────────────────────────────────────────────┤
│                                                             │
│  假设 ARR = 19 (周期 20)，CCR = 10，占空比 50%        │
│                                                             │
│  CNT:  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 0...    │
│        └────────┤────────────────────────────────┤                │
│            HIGH (10个)            LOW (10个)               │
│                                                             │
│  硬件在时钟边沿自动比较，软件只需设置 CCR 的值            │
│  CCR 越大 → HIGH 区间越长 → 占空比越高                  │
│                                                             │
└─────────────────────────────────────────────────────────────────┘
```

---

## 扩展阅读

- STM32 参考手册：定时器章节 - PWM 模式
- 定时器基础笔记：`docs/concepts/04_Timer_Basics.md`
