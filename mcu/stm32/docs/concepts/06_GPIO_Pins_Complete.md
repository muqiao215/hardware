# STM32 管脚/引脚完整讲解

## 一句话总结

STM32 的管脚可以分为：**GPIO（通用输入输出）** 和 **特殊功能引脚**（通信、定时器、ADC、调试等）

---

## 一、GPIO = 通用输入/输出端口

GPIO = **General Purpose Input/Output**

### 两种基本模式

| 模式 | 英文 | 作用 | 例子 |
|------|------|------|------|
| **输入** | Input | 读取外部信号 | 读取按键状态、传感器数据 |
| **输出** | Output | 控制外部设备 | 点亮 LED、控制继电器 |

---

### GPIO 输出模式

| 模式 | 英文 | 用途 |
|------|------|------|
| `GPIO_Mode_Out_PP` | 推挽输出 | 软件控制电平（LED、电机方向） |
| `GPIO_Mode_AF_PP` | 复用推挽 | 外设控制电平（PWM、串口、SPI） |

**区别**：
- `Out_PP` = 软件用 `GPIO_SetBits/ResetBits` 控制
- `AF_PP` = 外设（定时器、串口）自动控制，引脚控制权交给外设

---

### GPIO 输入模式

| 模式 | 英文 | 用途 |
|------|------|------|
| `GPIO_Mode_IPU` | 上拉输入 | 默认高电平（按键常用） |
| `GPIO_Mode_IPD` | 下拉输入 | 默认低电平 |
| `GPIO_Mode_IN_FLOATING` | 浮空输入 | 不拉，外部决定电平 |

---

### GPIO 常用操作函数

```c
// 设置引脚为高电平
GPIO_SetBits(GPIOA, GPIO_Pin_0);

// 设置引脚为低电平
GPIO_ResetBits(GPIOA, GPIO_Pin_0);

// 读取引脚电平（返回高或低）
uint8_t state = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);

// 同时设置多个引脚
GPIO_SetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1);
```

---

## 二、特殊功能引脚

### 1. 电源与复位引脚

| 引脚 | 名称 | 作用 |
|------|------|------|
| **VDD / VSS** | 数字电源正/负 | 3.3V 供电 / GND |
| **VDDA / VSSA** | 模拟电源正/负 | ADC 模块供电 |
| **VBAT** | 备用电池 | 断电后保持 RTC 运行 |
| **NRST** | 系统复位 | 低电平复位芯片 |

---

### 2. 时钟引脚

| 引脚 | 名称 | 作用 |
|------|------|------|
| **OSC_IN / OSC_OUT** | 外部晶振 | 接 8MHz 晶振 |
| **OSC32_IN** | 外部低速时钟 | 接 32.768kHz 晶振（用于 RTC） |

---

### 3. 调试接口 (SWD/JTAG)

| 引脚 | 名称 | 作用 |
|------|------|------|
| **PA13 / SWDIO** | SWD 数据线 | SWD 调试数据 |
| **PA14 / SWCLK** | SWD 时钟线 | SWD 调试时钟 |
| **PA15 / JTDI** | JTAG 接口 | 全功能调试 |
| **PB3 / JDO** | JTAG 接口 | 全功能调试 |
| **PB4 / NJTRST** | JTAG 复位 | JTAG 复位信号 |

**注意**：调试时这些引脚不能当普通 GPIO 使用。

---

### 4. 定时器特殊引脚（与 GPIO 复用）

这些引脚本质是 GPIO，但有特殊名称：

| 特殊名称 | 对应 GPIO | 功能 |
|----------|-----------|------|
| **TIM2_CH1** | PA0 | PWM 输出 / 捕获输入 |
| **TIM2_CH2** | PA1 | PWM 输出 / 捕获输入 |
| **TIM2_ETR** | PA0 | 外部时钟输入 |
| **TIM3_CH1** | PA6 | 编码器接口 A 相 |
| **TIM3_CH2** | PA7 | 编码器接口 B 相 |

**关键**：用作 PWM 时，需要配置为 `GPIO_Mode_AF_PP`

---

### 5. 通信接口引脚

#### USART 串口

| 引脚 | 名称 | 功能 |
|------|------|------|
| **PA9** | USART1_TX | 发送数据 |
| **PA10** | USART1_RX | 接收数据 |

#### I2C

| 引脚 | 名称 | 功能 |
|------|------|------|
| **PB6** | I2C1_SCL | 时钟线 |
| **PB7** | I2C1_SDA | 数据线 |

#### SPI

| 引脚 | 名称 | 功能 |
|------|------|------|
| **PA5** | SPI1_SCK | 时钟 |
| **PA6** | SPI1_MISO | 主入从出 |
| **PA7** | SPI1_MOSI | 主出从入 |
| **PA4** | SPI1_NSS | 片选 |

---

### 6. ADC 模拟输入引脚

| 引脚 | 名称 | 功能 |
|------|------|------|
| **PA0 ~ PA7** | ADC_IN0 ~ 7 | 模拟电压输入 |
| **PB0 ~ PB1** | ADC_IN8 ~ 9 | 模拟电压输入 |

---

### 7. USB 接口

| 引脚 | 名称 | 功能 |
|------|------|------|
| **PA11** | USB_DM | USB 数据负 |
| **PA12** | USB_DP | USB 数据正 |

---

## 三、引脚复用机制

### 复用原理

```
┌─────────────────────────────────────────────────────────────────┐
│                  STM32 引脚复用机制                         │
├─────────────────────────────────────────────────────────────────┤
│                                                             │
│  同一个物理引脚可以有多种功能：                             │
│                                                             │
│  PA0 可以是：                                              │
│    • GPIO 输出（点灯）                                    │
│    • TIM2_CH1（PWM 输出）                                 │
│    • TIM2_ETR（外部时钟）                                  │
│    • ADC_IN0（模拟输入）                                   │
│                                                             │
│  配置哪个功能，引脚就工作在哪个模式下                         │
│                                                             │
└─────────────────────────────────────────────────────────────────┘
```

### 复用配置

```c
// GPIO 作为普通输出
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

// GPIO 作为复用输出（PWM、串口等）
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
```

---

## 四、我们学过的引脚汇总

| 引脚 | GPIO 编号 | 学过的功能 | 对应课程 |
|------|-----------|------------|----------|
| PA0 | GPIO_Pin_0 | PWM LED (6-3)、外部时钟 (6-2) | 6-2, 6-3 |
| PA1 | GPIO_Pin_1 | PWM 舵机 (6-4) | 6-4 |
| PA2 | GPIO_Pin_2 | PWM 直流电机 (6-5) | 6-5 |
| PA4/PA5 | GPIO_Pin_4/5 | 电机方向控制 (6-5) | 6-5 |
| PA6/PA7 | GPIO_Pin_6/7 | 编码器 A/B 相 (5-2, 6-8) | 5-2, 6-8 |
| PB8/PB9 | GPIO_Pin_8/9 | OLED I2C (SCL/SDA) | 3-1 |
| PB1 | GPIO_Pin_1 | 按键 (3-4) | 3-4 |

---

## 五、6-5 电机控制中的 GPIO

```c
// Motor.c - 用 GPIO 控制电机转向
void Motor_SetSpeed(int8_t Speed)
{
    if (Speed >= 0)  // 正转
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_4);      // PA4 = H
        GPIO_ResetBits(GPIOA, GPIO_Pin_5);    // PA5 = L → 正转
        PWM_SetCompare3(Speed);                // PWM 控制速度 0~100
    }
    else  // 反转
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_4);    // PA4 = L
        GPIO_SetBits(GPIOA, GPIO_Pin_5);      // PA5 = H → 反转
        PWM_SetCompare3(-Speed);               // Speed 是负数，取绝对值
    }
}
```

---

## 记忆口诀

| 类别 | 口诀 |
|------|------|
| **GPIO 输出** | Out_PP 软件控，AF_PP 外设控 |
| **GPIO 输入** | IPU 上拉常用，IPD 下拉用 |
| **复用** | 一脚多用，配置选职业 |
| **调试脚** | 调试时不可用，普通 GPIO |
| **PWM 脚** | 复用模式 + AF_PP |

---

## 扩展阅读

- STM32 参考手册：GPIO 章节、复用功能章节
- 定时器基础：`docs/concepts/04_Timer_Basics.md`
- PWM 完整讲解：`docs/concepts/05_PWM_Complete.md`
