#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

/**
  * 函    数：串口初始化
  * 参    数：无
  * 返 回 值：无
  */
void Serial_Init(void)
{
	/* ========== 第一步：开启时钟 ========== */
	// 说明：STM32 外设默认关闭，需先使能时钟才能配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	// USART1 挂载在 APB2 总线（最高 72MHz）
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	// GPIOA 也在 APB2 总线上
	
	/* ========== 第二步：配置 GPIO（PA9 作为 TX）========== */
	GPIO_InitTypeDef GPIO_InitStructure;
	// GPIO_Mode_AF_PP：复用推挽输出（Alternate Function Push-Pull）
	// - "复用"：引脚控制权交给 USART 外设，不再由 GPIO 寄存器直接控制
	// - "推挽"：可主动输出高/低电平（相比开漏，无需外部上拉）
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	
	// GPIO_Pin_9：PA9 是 USART1 的 TX 引脚（固定映射，不可更改）
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	
	// GPIO_Speed_50MHz：输出速度（响应速度），越快功耗越高
	// 说明：9600 波特率实际不需要 50MHz，但设高一点更稳定
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);					// 应用配置到 GPIOA
	
	/* ========== 第三步：配置 USART1 参数 ========== */
	USART_InitTypeDef USART_InitStructure;
	
	// 1. 波特率（Baud Rate）：9600 bit/s
	// - 定义：每秒传输的位数（包括数据位、起始位、停止位、校验位）
	// - 计算公式：实际波特率 = PCLK / (16 × USARTDIV)
	//   其中 PCLK = 72MHz（APB2），USARTDIV 由库函数自动计算
	// - 常见值：9600（低速稳定）、115200（高速，常用于调试）
	USART_InitStructure.USART_BaudRate = 9600;
	
	// 2. 硬件流控制（Hardware Flow Control）：无
	// - 作用：防止接收方缓冲区溢出（通过 RTS/CTS 引脚握手）
	// - 本课只发送，不需要流控制
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	
	// 3. 工作模式（Mode）：仅发送（Transmit）
	// - 可选：USART_Mode_Rx（接收）、USART_Mode_Tx（发送）、USART_Mode_Tx | USART_Mode_Rx（收发）
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
	
	// 4. 奇偶校验（Parity）：无
	// - 作用：检测传输错误（奇校验/偶校验）
	// - 本课使用可靠的 USB-TTL，无需校验
	USART_InitStructure.USART_Parity = USART_Parity_No;
	
	// 5. 停止位（Stop Bits）：1 位
	// - 作用：标志一帧数据结束
	// - 可选：1 位（标准）、0.5 位、2 位、1.5 位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	
	// 6. 字长（Word Length）：8 位
	// - 定义：一帧数据的有效位数（不包括起始位、停止位、校验位）
	// - 可选：8 位（标准 ASCII）、9 位（用于 9 位数据或 8 位数据+1 位校验）
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	
	USART_Init(USART1, &USART_InitStructure);				// 将配置写入 USART1 寄存器
	
	/* ========== 第四步：使能 USART1 ========== */
	// 说明：配置完成后才使能，此时 USART1 开始工作
	USART_Cmd(USART1, ENABLE);
}

/**
  * 函    数：串口发送一个字节
  * 参    数：Byte 要发送的一个字节
  * 返 回 值：无
  */
void Serial_SendByte(uint8_t Byte)
{
	/* ========== 步骤 1：写入数据寄存器（TDR）========== */
	// USART_SendData()：将字节写入 USART1->DR 寄存器
	// 硬件自动完成以下操作：
	//   1. 从 DR 寄存器取出数据
	//   2. 自动添加起始位（逻辑 0，低电平）
	//   3. 按波特率逐位发送（LSB 先发送）
	//   4. 自动添加停止位（逻辑 1，高电平）
	//   5. 更新 TXE 标志位（数据寄存器为空时置 1）
	USART_SendData(USART1, Byte);
	
	/* ========== 步骤 2：等待发送完成 ========== */
	// USART_FLAG_TXE：发送数据寄存器空标志（Transmit Data Register Empty）
	// - RESET（0）：数据寄存器非空，硬件正在发送
	// - SET（1）：数据寄存器空，可写入下一个字节
	// 为什么要等待？
	//   - 如果不等待，连续调用 USART_SendData() 会覆盖未发送的数据
	//   - TXE 标志在下次写入时自动清零，无需手动清除
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

/**
  * 函    数：串口发送一个数组
  * 参    数：Array 要发送数组的首地址
  * 参    数：Length 要发送数组的长度
  * 返 回 值：无
  */
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)		//遍历数组
	{
		Serial_SendByte(Array[i]);		//依次调用Serial_SendByte发送每个字节数据
	}
}

/**
  * 函    数：串口发送一个字符串
  * 参    数：String 要发送字符串的首地址
  * 返 回 值：无
  */
void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)//遍历字符数组（字符串），遇到字符串结束标志位后停止
	{
		Serial_SendByte(String[i]);		//依次调用Serial_SendByte发送每个字节数据
	}
}

/**
  * 函    数：次方函数（内部使用）
  * 返 回 值：返回值等于X的Y次方
  */
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;	//设置结果初值为1
	while (Y --)			//执行Y次
	{
		Result *= X;		//将X累乘到结果
	}
	return Result;
}

/**
  * 函    数：串口发送数字
  * 参    数：Number 要发送的数字，范围：0~4294967295
  * 参    数：Length 要发送数字的长度，范围：0~10
  * 返 回 值：无
  */
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)		// 根据 Length 遍历数字的每一位（从高位到低位）
	{
		// 【数字转 ASCII 的核心算法】
		// 示例：Number=123, Length=3, 发送 "123"
		// 
		// 第 1 次循环（i=0）：
		//   Number / Serial_Pow(10, 3-0-1) % 10 + '0'
		//   = 123 / 100 % 10 + '0'
		//   = 1 % 10 + '0'
		//   = 1 + 48 = 49（ASCII 字符 '1'）
		//
		// 第 2 次循环（i=1）：
		//   123 / 10 % 10 + '0'
		//   = 12 % 10 + '0'
		//   = 2 + 48 = 50（ASCII 字符 '2'）
		//
		// 第 3 次循环（i=2）：
		//   123 / 1 % 10 + '0'
		//   = 123 % 10 + '0'
		//   = 3 + 48 = 51（ASCII 字符 '3'）
		//
		// 说明：
		//   - '0' 的 ASCII 码是 48，'0'+1='1'，以此类推
		//   - Serial_Pow(10, n) 计算 10 的 n 次方
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * 函    数：使用printf需要重定向的底层函数
  * 参    数：保持原始格式即可，无需变动
  * 返 回 值：保持原始格式即可，无需变动
  */
int fputc(int ch, FILE *f)
{
	// 【全局 printf 重定向的关键函数】
	// 原理：
	//   - printf() 在底层会调用 fputc() 输出每个字符
	//   - 标准 C 库的 fputc() 默认输出到标准输出（stdout，通常是屏幕）
	//   - 重定义此函数后，printf() 的输出会自动重定向到串口
	// 
	// 参数：
	//   - ch：要输出的字符（ASCII 值）
	//   - f：文件指针（通常不使用，保留参数即可）
	// 
	// 要求：
	//   - Keil：勾选 "Use MicroLIB"（Options -> Target -> Use MicroLIB）
	//   - GCC：链接参数添加 --specs=nano.specs 或 --specs=nosys.specs
	Serial_SendByte(ch);			// 将字符发送到串口
	return ch;						// 返回字符（标准要求）
}

/**
  * 函    数：自己封装的prinf函数
  * 参    数：format 格式化字符串
  * 参    数：... 可变的参数列表
  * 返 回 值：无
  */
void Serial_Printf(char *format, ...)
{
	// 【封装的格式化输出函数（推荐）】
	// 功能：类似 printf()，但不占用全局 printf，可多处同时使用
	// 
	// 实现步骤：
	//   1. 使用 va_list 接收可变参数（如 Serial_Printf("X=%d", 123) 中的 123）
	//   2. 使用 vsprintf() 格式化到临时字符数组
	//   3. 通过串口发送字符数组
	// 
	// 示例：
	//   Serial_Printf("Temp=%d, Humi=%d\r\n", 25, 60);
	//   等价于：
	//     sprintf(String, "Temp=%d, Humi=%d\r\n", 25, 60);
	//     Serial_SendString(String);
	
	char String[100];				// 定义缓冲区（注意：如果格式化后的字符串超过 100 字节会溢出！）
	
	va_list arg;					// 定义可变参数列表变量
	va_start(arg, format);			// 从 format 参数后开始，提取可变参数到 arg
	vsprintf(String, format, arg);	// 格式化字符串：将 format 和 arg 组合输出到 String
	va_end(arg);					// 结束可变参数提取（释放资源）
	
	Serial_SendString(String);		// 通过串口发送格式化后的字符串
}
