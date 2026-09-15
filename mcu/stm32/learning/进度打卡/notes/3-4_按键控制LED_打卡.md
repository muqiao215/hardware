# 3-4 按键控制LED - 学习打卡

**完成日期**: 2025-12-07
**学习章节**: 江协科技STM32入门教程 3-4
**项目路径**: `/home/muqiao/dev/bolt/STM32PROJECT/learning/ch03_gpio/3-4 按键控制LED`

---

## 核心知识点

### 1. 硬件抽象层(HAL)架构

从3-3的直接寄存器操作跃升到模块化设计:

```
项目目录结构:
├── Hardware/          ← 硬件驱动抽象层
│   ├── LED.c/h       ← LED驱动模块
│   └── Key.c/h       ← 按键驱动模块
├── System/           ← 系统工具(Delay)
├── User/main.c       ← 业务逻辑
└── Library/          ← STM32标准外设库
```

**设计哲学**:
- 向上提供稳定API(`LED_Turn`, `Key_GetNum`)
- 向下封装硬件细节(寄存器操作)
- 便于移植和复用

### 2. GPIO输入模式 - 上拉输入

```c
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // Input Pull-Up
```

**电路原理**:
```
        VDD(3.3V)
           |
        [内部上拉电阻 ~40kΩ]
           |
    GPIO引脚 ----[按键]---- GND
           |
      (读到0或1)
```

- 未按下: 上拉电阻拉高 → 读到1
- 按下: 直接接地 → 读到0

**其他输入模式对比**:

| 模式 | 特点 | 应用场景 |
|------|------|----------|
| IPU上拉输入 | 内部上拉,默认高电平 | 按键、开关 |
| IPD下拉输入 | 内部下拉,默认低电平 | 需要默认0的场合 |
| IN_FLOATING浮空输入 | 不确定状态 | ADC采样 |
| AIN模拟输入 | 关闭数字缓冲器 | ADC专用 |

### 3. 按键消抖算法 - 双延时阻塞式

**机械按键抖动现象**:
```
         按下瞬间              松开瞬间
            ↓                     ↓
      ______|  |_|_|___      _____|_|  |______
            抖动(5-10ms)          抖动
```

**消抖代码实现**:
```c
if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0) {
    Delay_ms(20);                                      // ① 按下消抖
    while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0);  // ② 等待松手
    Delay_ms(20);                                      // ③ 松开消抖
    KeyNum = 1;
}
```

**三阶段分析**:
1. **检测到低电平** → 延时20ms等待抖动稳定
2. **while循环阻塞** → 必须松手才能继续(防止连击)
3. **松手后再延时** → 消除松开时的抖动

**为什么选20ms?**
- 机械开关抖动通常<10ms
- 20ms提供安全余量
- 人类感知阈值>50ms,无明显卡顿

### 4. LED驱动API设计

```c
// 初始化
void LED_Init(void);

// 控制接口
void LED1_ON(void);    // 低电平点亮
void LED1_OFF(void);   // 高电平熄灭
void LED1_Turn(void);  // 翻转状态
```

**翻转函数的读-改-写实现**:
```c
void LED1_Turn(void) {
    if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_1) == 0) {
        GPIO_SetBits(GPIOA, GPIO_Pin_1);      // 0→1
    } else {
        GPIO_ResetBits(GPIOA, GPIO_Pin_1);    // 1→0
    }
}
```

**为什么不用 `^` 异或?**
- STM32标准库无XOR寄存器接口
- 直接操作ODR寄存器可读性差
- 读-改-写模式更清晰可靠

---

## 实践验证

### 硬件连接

| 引脚 | 功能 | 电路连接 |
|------|------|----------|
| PA1 | LED1输出 | 经限流电阻接LED正极 |
| PA2 | LED2输出 | 经限流电阻接LED正极 |
| PB1 | 按键1输入 | 一端接地,另一端接GPIO(内部上拉) |
| PB11 | 按键2输入 | 一端接地,另一端接GPIO(内部上拉) |

### 测试步骤

1. **编译工程**:
   - 用Keil打开 `Project.uvprojx`
   - 确认Include路径包含Hardware文件夹
   - Build(F7) → 0 Error(s), 0 Warning(s)

2. **下载运行**:
   - 连接ST-Link
   - Download(F8)
   - Reset复位芯片

3. **功能验证**:
   - 按下按键1 → LED1翻转(亮/灭切换)
   - 按下按键2 → LED2翻转(亮/灭切换)
   - 长按按键不松手 → 程序阻塞在while循环,松手后才继续

---

## 代码实现细节

### Key.c 完整注释版

```c
#include "stm32f10x.h"
#include "Delay.h"

void Key_Init(void) {
    // 1. 使能GPIOB时钟(APB2总线外设)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // 2. 配置GPIO
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;        // 上拉输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_11;  // 同时配置两个引脚
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    // 输入模式下此参数无效
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

uint8_t Key_GetNum(void) {
    uint8_t KeyNum = 0;  // 默认无按键

    // 扫描按键1(PB1)
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0) {
        Delay_ms(20);  // 消抖
        while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0);  // 等待松手
        Delay_ms(20);  // 松手消抖
        KeyNum = 1;
    }

    // 扫描按键2(PB11) - 逻辑同上
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0) {
        Delay_ms(20);
        while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0);
        Delay_ms(20);
        KeyNum = 2;
    }

    return KeyNum;  // 返回0-2的键码
}
```

### main.c 状态机逻辑

```c
#include "stm32f10x.h"
#include "Delay.h"
#include "LED.h"
#include "Key.h"

uint8_t KeyNum;

int main(void) {
    // 初始化阶段
    LED_Init();  // 配置PA1/PA2为推挽输出
    Key_Init();  // 配置PB1/PB11为上拉输入

    // 主循环 - 事件驱动状态机
    while (1) {
        KeyNum = Key_GetNum();  // 轮询按键(阻塞式)

        if (KeyNum == 1) {
            LED1_Turn();  // 按键1 → LED1翻转
        }

        if (KeyNum == 2) {
            LED2_Turn();  // 按键2 → LED2翻转
        }
    }
}
```

---

## 技术见解

### 1. 阻塞式设计的局限性

**当前代码的问题**:
- `Key_GetNum` 中的 `while` 循环会阻塞CPU
- 按键按住时无法执行其他任务(如LED动画、传感器采样)
- 不适合多任务并发场景

**改进方向**(后续章节会学习):
- **外部中断(EXTI)**: 按键触发中断,主循环不阻塞
- **定时器扫描**: 定时器中断周期扫描按键,主循环异步处理
- **状态机消抖**: 用软件状态机实现非阻塞消抖

### 2. 硬件抽象的价值

**封装前**(3-3风格):
```c
// main.c中直接操作寄存器
GPIO_ResetBits(GPIOB, GPIO_Pin_12);  // 这是在控制什么?需要查原理图
```

**封装后**(3-4风格):
```c
// 语义明确,无需关心底层引脚
LED1_ON();   // 一看就懂
Buzzer_Turn();  // 自文档化
```

**好处**:
- **可移植性**: 换芯片只需修改Hardware层
- **可测试性**: 可以Mock硬件接口进行单元测试
- **团队协作**: 接口约定清晰,分工明确

### 3. 标准库API的设计思想

```c
// STM32标准库的命名规范
GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);   // 读输入数据寄存器(IDR)
GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_1);  // 读输出数据寄存器(ODR)
GPIO_SetBits(GPIOA, GPIO_Pin_1);            // 置位(写BSRR寄存器)
GPIO_ResetBits(GPIOA, GPIO_Pin_1);          // 复位(写BRR寄存器)
```

**为什么Set/Reset分开而不用Write?**
- BSRR/BRR寄存器支持原子操作
- 避免读-改-写过程中的中断风险
- 多线程安全(无需关中断保护)

---

## 对比总结

### 3-3 蜂鸣器 vs 3-4 按键控制LED

| 对比项 | 3-3 蜂鸣器 | 3-4 按键控制LED |
|--------|------------|-----------------|
| 架构 | 平铺式,全部在main.c | 分层架构(Hardware/System/User) |
| GPIO使用 | 仅输出(蜂鸣器) | 输入(按键)+输出(LED) |
| 抽象程度 | 直接操作寄存器 | 硬件抽象API |
| 可扩展性 | 差 | 好(新增模块无需改main) |
| 适用场景 | 教学演示 | 工程实践 |

---

## 下一步学习计划

### 即将进入的章节

1. **4-1 OLED显示屏** (已复制到learning目录)
   - 学习I2C/SPI通信协议
   - 掌握字符/图形显示原理
   - 建立调试信息输出手段

2. **5-1 对射式红外传感器计次** (已复制)
   - 引入外部中断(EXTI)机制
   - 学习非阻塞式事件处理
   - 解决阻塞式按键的缺陷

3. **6-1 定时器定时中断** (已复制)
   - STM32最核心的外设 - 定时器
   - 精准时基生成
   - 为PWM/输入捕获打基础

---

## 知识掌握自评

- [x] 理解上拉/下拉/浮空输入的区别
- [x] 掌握机械按键消抖的原理和实现
- [x] 能独立搭建Hardware抽象层架构
- [x] 理解阻塞式扫描的优缺点
- [ ] 能用外部中断改进按键检测(待学习5-1)
- [ ] 能用定时器实现非阻塞消抖(待学习6-1)

---

**备注**: 本次学习完成了从"直接操作寄存器"到"硬件抽象层"的思维跃升,为后续复杂项目打下了架构基础。
