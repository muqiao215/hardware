#include <8052.h>
#include <stdint.h>

// ==============================================================================================
// 文件名: shift595_stopwatch_hhmmss_btn_p32_int0.c
// 功能: 基于 P3.2 (INT0) 中断按键的 HH:MM:SS 秒表
// 硬件:
//   - 6个串联的 74HC595 驱动 6位共阴数码管
//   - P3.7: DS   (串行数据)
//   - P3.6: SHCP (移位时钟)
//   - P3.5: STCP (锁存时钟)
//   - P3.2: 按键 (INT0, 外部中断0, 默认为低电平触发或下降沿触发)
// 说明:
//   - 上电初始为 00:00:00 (停止状态)
//   - 按下 P3.2 按键切换 [启动] / [停止]
//   - 计时范围 00:00:00 ~ 99:59:59
// ==============================================================================================

// 晶振频率定义 (默认 11.0592MHz)
#ifndef FOSC_HZ
#define FOSC_HZ 11059200UL
#endif

// 数码管显示顺序翻转 (1=翻转, 0=正常)
// 如果发现数码管顺序反了(比如秒显示在小时位置), 可修改此宏
#ifndef DIGIT_REVERSE
#define DIGIT_REVERSE 1
#endif

// 按键有效电平 (1=低电平有效, 0=高电平有效)
#ifndef BUTTON_ACTIVE_LOW
#define BUTTON_ACTIVE_LOW 1
#endif

// 消抖时间 (毫秒)
#ifndef DEBOUNCE_MS
#define DEBOUNCE_MS 30u
#endif

// 引脚定义
#define SI  P3_7   // 74HC595 DS
#define SCK P3_6   // 74HC595 SHCP
#define RCK P3_5   // 74HC595 STCP
#define BTN P3_2   // 按键 IO (INT0)

// 定时器0配置 (产生 ~1ms 中断)
// 12分频模式下 (标准8051), 1个机器周期 = 12/FOSC
// 1ms 需要的计数值 = FOSC / 12 / 1000
#define TICKS_PER_MS (FOSC_HZ / 12UL / 1000UL)
#define TIMER0_RELOAD (65536UL - TICKS_PER_MS)
#define TH0_RELOAD ((uint8_t)((TIMER0_RELOAD >> 8) & 0xFF))
#define TL0_RELOAD ((uint8_t)(TIMER0_RELOAD & 0xFF))

// 共阴极数码管段码表 (0~9)
static const uint8_t __code seg_cc[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

// 全局时间变量
static volatile uint16_t uptime_ms = 0;   // 系统运行总毫秒数 (用于消抖)
static volatile uint16_t ms_in_sec = 0;   // 当前秒内的毫秒数 (0-999)
static volatile uint8_t tick_1s = 0;      // 1秒标志位 (1=发生了1秒进位)

// 按键状态变量
static volatile uint8_t btn_pending = 0;     // 按键处理挂起标志
static volatile uint16_t btn_pending_at = 0; // 按键触发时刻 (用于计算消抖时间)
static volatile uint8_t btn_pressed = 0;     // 有效按键确认标志

// 业务逻辑变量
static uint8_t running = 0;     // 运行状态 (1=正在计时, 0=停止)
static uint32_t seconds = 0;    // 当前累计秒数

// 检测按键物理状态
static uint8_t btn_is_active(void) {
#if BUTTON_ACTIVE_LOW
  return (BTN == 0) ? 1 : 0;
#else
  return (BTN == 1) ? 1 : 0;
#endif
}

// 74HC595: 发送一个字节 (8位)
static void shift595_send_byte(uint8_t value) {
  uint8_t i;
  for (i = 0; i < 8; i++) {
    // 先发高位 (MSB First)
    SI = (value & 0x80) ? 1 : 0;
    value <<= 1;
    SCK = 0; // 产生移位时钟上升沿
    SCK = 1;
  }
}

// 74HC595: 锁存并显示 6 个字节
static void shift595_latch_6bytes(const uint8_t bytes[6]) {
  uint8_t i;

  RCK = 0; // 准备锁存

#if DIGIT_REVERSE
  // 如果数码管级联顺序相反, 倒序发送
  for (i = 0; i < 6; i++) {
    shift595_send_byte(bytes[5 - i]);
  }
#else
  // 正序发送
  for (i = 0; i < 6; i++) {
    shift595_send_byte(bytes[i]);
  }
#endif

  RCK = 1; // 产生锁存时钟上升沿 (更新显示)
  RCK = 0;
}

// 将总秒数转换为 HH, MM, SS 的段码
static void render_hhmmss(uint32_t total_seconds, uint8_t out_bytes[6]) {
  uint32_t hh32 = total_seconds / 3600u;
  // 限制最大显示 99 小时
  uint8_t hh = (hh32 > 99u) ? 99u : (uint8_t)hh32;
  uint8_t mm = (uint8_t)((total_seconds / 60u) % 60u);
  uint8_t ss = (uint8_t)(total_seconds % 60u);

  // 查表填充段码
  out_bytes[0] = seg_cc[hh / 10u];    // 时-十位
  out_bytes[1] = seg_cc[hh % 10u];    // 时-个位
  out_bytes[2] = seg_cc[mm / 10u];    // 分-十位
  out_bytes[3] = seg_cc[mm % 10u];    // 分-个位
  out_bytes[4] = seg_cc[ss / 10u];    // 秒-十位
  out_bytes[5] = seg_cc[ss % 10u];    // 秒-个位
}

// 初始化 Timer0 (模式1, 16位, 1ms中断)
static void timer0_init(void) {
  TMOD = (TMOD & 0xF0) | 0x01; // 只修改 Timer0 设置, 保留 Timer1
  TR0 = 0;
  TF0 = 0;
  TH0 = TH0_RELOAD;
  TL0 = TL0_RELOAD;
  ET0 = 1; // 开启 Timer0 中断
  EA = 1;  // 开启全局中断
  TR0 = 1; // 启动定时器
}

// Timer0 中断服务函数 (每 1ms 触发一次)
void timer0_isr(void) __interrupt(1) {
  TH0 = TH0_RELOAD; // 重装载初值
  TL0 = TL0_RELOAD;

  uptime_ms++;      // 更新系统时间
  ms_in_sec++;      // 更新秒内计数
  if (ms_in_sec >= 1000u) {
    ms_in_sec = 0;
    tick_1s = 1;    // 触发秒信号
  }
}

// INT0 (外部中断0) 服务函数
// 按下 P3.2 时触发 (下降沿)
void int0_isr(void) __interrupt(0) {
  if (!btn_pending && !btn_pressed) {
    // 首次检测到按键, 标记挂起等待消抖
    btn_pending = 1;
    btn_pending_at = uptime_ms; // 记录当前时间
    EX0 = 0; // 暂时关闭 INT0 中断, 防止抖动反复触发
  }
}

void main(void) {
  uint8_t bytes[6];

  // 初始化 IO
  SI = 0;
  SCK = 1;
  RCK = 0;

  // 配置外部中断 0
  IT0 = 1; // 1=下降沿触发, 0=低电平触发
  EX0 = 1; // 开启 INT0

  // 初始显示 00 00 00
  render_hhmmss(seconds, bytes);
  shift595_latch_6bytes(bytes);

  // 启动系统滴答定时器
  timer0_init();

  while (1) {
    // --- 按键消抖处理 ---
    if (btn_pending) {
      // 检查消抖时间是否到达
      if ((uint16_t)(uptime_ms - btn_pending_at) >= (uint16_t)DEBOUNCE_MS) {
        btn_pending = 0;
        // 消抖完成后再次检查按键物理状态
        if (btn_is_active()) {
          // 确认为有效按键: 切换运行状态
          // Press behavior:
          // - If running: stop and reset to 00:00:00
          // - If stopped: start from 00:00:00
          seconds = 0;
          running = running ? 0 : 1;
          btn_pressed = 1; // 标记按键已处理 (等待释放)
        } else {
          // 误触或抖动, 重新开启中断
          EX0 = 1;
        }
      }
    }

    // --- 按键释放处理 ---
    if (btn_pressed && !btn_is_active()) {
      // 按键已抬起, 恢复中断响应, 准备下一次按键
      btn_pressed = 0;
      EX0 = 1;
    }

    // --- 1秒时间处理 ---
    if (tick_1s) {
      tick_1s = 0; // 清除标志

      if (running) {
        // 99小时 59分 59秒 = 359999 秒
        if (seconds < (uint32_t)99u * 3600u + 59u * 60u + 59u) {
          seconds++;
        } else {
          running = 0; // 达到最大值自动停止
        }
      }

      // 刷新显示
      render_hhmmss(seconds, bytes);
      shift595_latch_6bytes(bytes);
    }
  }
}
