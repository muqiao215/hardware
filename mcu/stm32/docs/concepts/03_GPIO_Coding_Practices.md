# 💻 第3章：GPIO 编程实战指南

> **摘要**：本章将理论转化为代码。详细讲解标准库函数的正确用法、位掩码技巧，以及如何根据电路原理编写控制逻辑。

---

## 1. 🎭 库函数背后的位掩码 (Bitmask)

在 `stm32f10x_gpio.h` 中，`GPIO_Pin_x` 并不是简单的数字，而是**位掩码**。

```c
#define GPIO_Pin_0    ((uint16_t)0x0001)  // ...0000 0001
#define GPIO_Pin_1    ((uint16_t)0x0002)  // ...0000 0010
#define GPIO_Pin_15   ((uint16_t)0x8000)  // ...1000 0000
```

### 组合操作的艺术
因为每一位都是独立的，我们可以用 **按位或 (`|`)** 一次性选中多个引脚：

```c
// 同时选中 Pin 0 和 Pin 2
uint16_t pins = GPIO_Pin_0 | GPIO_Pin_2; // 结果 0x0005 (...0101)

// 初始化时的经典写法
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
```

---

## 2. 🛠️ 常用操作函数全解析

### 2.1 原子操作：`GPIO_SetBits` / `GPIO_ResetBits`
*   **原理**: 操作 **BSRR** 寄存器。
*   **特点**: **原子性**。只修改指定的位，绝对不会影响其他引脚。
*   **场景**: 多任务环境、中断中、或者只想控制单一设备（如开关一个 LED）。

```c
GPIO_SetBits(GPIOA, GPIO_Pin_0);   // PA0 置高 (1)
GPIO_ResetBits(GPIOA, GPIO_Pin_0); // PA0 置低 (0)
```

### 2.2 整体写入：`GPIO_Write`
*   **原理**: 直接覆盖 **ODR** 寄存器。
*   **特点**: **非原子性**。一次性刷新端口上的所有 16 个引脚状态。
*   **场景**: 并行通信、**流水灯**、数码管显示。

```c
GPIO_Write(GPIOA, 0xFFFF); // PA0-PA15 全部置高
GPIO_Write(GPIOA, 0x0000); // PA0-PA15 全部置低
```

### 2.3 单个位操作：`GPIO_WriteBit`
*   **特点**: 这是一个封装函数，底层还是调用的 BSRR。
*   **场景**: 只有当你手里有一个 `BitAction` (0或1) 变量时才用它，否则不如直接用 Set/Reset 明确。

---

## 3. 💡 LED 点亮逻辑：灌电流 vs 拉电流

**初学者由于直觉，常认为“高电平点亮”，但事实往往相反。**

### 3.1 灌电流 (Sink Current) —— 推荐 🔥
*   **接法**: VCC -> 电阻 -> LED -> **GPIO 引脚**
*   **逻辑**:
    *   GPIO 输出 **低电平 (0)** -> 形成压差 -> **灯亮**
    *   GPIO 输出 **高电平 (1)** -> 无压差 -> **灯灭**
*   **优点**: STM32 的 N-MOS (拉低) 驱动能力通常比 P-MOS (拉高) 强，适合驱动大负载。

### 3.2 拉电流 (Source Current)
*   **接法**: **GPIO 引脚** -> LED -> 电阻 -> GND
*   **逻辑**:
    *   GPIO 输出 **高电平 (1)** -> **灯亮**
    *   GPIO 输出 **低电平 (0)** -> **灯灭**

---

## 4. 🎹 经典案例逻辑分析

### 4.1 流水灯 (Running LED)
利用 `GPIO_Write` 和位运算实现移位效果。

```c
uint16_t pattern = 0x0001; // 初始只有第0位亮 (假设高电平亮)
while(1) {
    GPIO_Write(GPIOA, pattern);
    Delay_ms(500);
    pattern = pattern << 1; // 左移一位
    if(pattern == 0) pattern = 0x0001; // 循环
}
```
*(注：如果是低电平点亮，pattern 取反 `~pattern` 即可)*

### 4.2 有源蜂鸣器 (Active Buzzer)
*   **特点**: 只要给电就响，不需要音频信号。
*   **控制**: 像控制 LED 一样控制高低电平。
*   **节奏**:
    ```c
    GPIO_ResetBits(GPIOB, GPIO_Pin_12); // 响 (假设低触发)
    Delay_ms(100);
    GPIO_SetBits(GPIOB, GPIO_Pin_12);   // 停
    Delay_ms(100);
    ```

---

## 5. 📚 高效学习方法论

根据之前的复盘，总结出适合你的进阶路径：

1.  **代码驱动 (第一层)**: 先看 `.h` 和 `.c`，会用函数，看到灯亮。建立成就感。
2.  **原理深究 (第二层)**: 遇到不懂的函数（如 `GPIO_Init`），点进去看源码，对照参考手册，理解它配置了哪个寄存器。
3.  **问题解决 (第三层)**: 遇到 Bug (如灯微亮、无法烧录) 时，不要慌，这是学习的最佳时机。记录下每一个“坑”和解决办法。

> **清梦的嘱托**: 
> 别被厚厚的手册吓倒。手册是**字典**，不是小说。不需要从头读到尾，哪里不会查哪里！

---

## 6. 📖 附录：常用标准库函数速查手册

这里整理了最常用的库函数，附带了清梦的独家注释，方便你随时查阅！

### 6.1 时钟控制 (RCC)
**⚠️ 只有开启了时钟，外设才会工作！这是初学者最容易忘的一步！**

```c
// 开启 AHB 总线外设时钟 (如 DMA, SDIO)
void RCC_AHBPeriphClockCmd(uint32_t RCC_AHBPeriph, FunctionalState NewState);

// 开启 APB2 高速总线外设时钟 (GPIO, USART1, ADC, SPI1)
// 例如: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);

// 开启 APB1 低速总线外设时钟 (USART2-5, I2C, SPI2/3, TIM2-7)
void RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState);
```

### 6.2 初始化与复位 (GPIO)

```c
// 将 GPIO 恢复为默认设置 (通常是浮空输入)
void GPIO_DeInit(GPIO_TypeDef* GPIOx);

// 复位 AFIO (复用功能 IO) 配置
void GPIO_AFIODeInit(void);

// ⭐ 核心初始化函数
// 根据结构体参数配置 GPIO 模式、速度等
void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);

// 将结构体填充为默认值 (避免未初始化的变量导致错误)
void GPIO_StructInit(GPIO_InitTypeDef* GPIO_InitStruct);
```

### 6.3 读操作 (Read)

```c
// 读取某个引脚的输入电平 (返回 0 或 1)
// 适用于：检测按键、传感器状态
uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

// 读取整个端口的输入状态 (返回 16 位整数)
uint16_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx);

// 读取某个引脚当前的输出状态
// 适用于：翻转电平逻辑 (Output = !Output)
uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

// 读取整个端口的输出状态
uint16_t GPIO_ReadOutputData(GPIO_TypeDef* GPIOx);
```

### 6.4 写操作 (Write)

```c
// ⭐ 置位 (Set): 输出高电平 (1)
// 原子操作，不影响其他位
void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

// ⭐ 复位 (Reset): 输出低电平 (0)
// 原子操作，不影响其他位
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

// 根据 BitVal (Bit_RESET/Bit_SET) 写某一位
void GPIO_WriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal);

// ⭐ 端口写入: 一次性改变所有 16 个引脚
// 适用于：流水灯、并行数据传输
void GPIO_Write(GPIO_TypeDef* GPIOx, uint16_t PortVal);
```

### 6.5 GPIO 模式枚举 (`GPIOMode_TypeDef`)

配置 `GPIO_InitStructure.GPIO_Mode` 时使用的参数：

```c
typedef enum
{ 
  GPIO_Mode_AIN = 0x0,          // 模拟输入 (ADC专属，信号纯净)
  GPIO_Mode_IN_FLOATING = 0x04, // 浮空输入 (危险，易受干扰)
  GPIO_Mode_IPD = 0x28,         // 下拉输入 (默认低电平)
  GPIO_Mode_IPU = 0x48,         // 上拉输入 (默认高电平，接按键常用)
  
  GPIO_Mode_Out_OD = 0x14,      // 开漏输出 (I2C, 5V兼容)
  GPIO_Mode_Out_PP = 0x10,      // 推挽输出 (最常用，强驱动)
  
  GPIO_Mode_AF_OD = 0x1C,       // 复用开漏 (硬件I2C)
  GPIO_Mode_AF_PP = 0x18        // 复用推挽 (UART TX, SPI, PWM)
} GPIOMode_TypeDef;
```