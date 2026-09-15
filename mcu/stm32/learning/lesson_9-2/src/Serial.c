#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

/* ========== 串口接收相关全局变量 ========== */
// Serial_RxData：存储从 USART1 接收到的最新一个字节
// - 由中断服务函数（ISR）写入
// - 由主循环通过 Serial_GetRxData() 读取
// - 注意：单字节缓存，如果主循环处理不及时，新数据会覆盖旧数据
uint8_t Serial_RxData;

// Serial_RxFlag：接收数据的标志位
// - 中断服务函数收到数据后置 1
// - Serial_GetRxFlag() 读取后自动清零
// - 作用：通知主循环"有新数据待处理"
uint8_t Serial_RxFlag;

/**
  * 函    数：串口初始化
  * 参    数：无
  * 返 回 值：无
  */
void Serial_Init(void)
{
	/* ========== 第一步：开启时钟 ========== */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	// USART1 挂载在 APB2 总线
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	// GPIOA 也在 APB2 总线上
	
	/* ========== 第二步：配置 GPIO ========== */
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* 2.1 配置 PA9(TX)：复用推挽输出 */
	// 说明：与 Lesson 9-1 相同，用于发送数据
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			// 复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				// PA9 是 USART1_TX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* 2.2 配置 PA10(RX)：上拉输入 */
	// GPIO_Mode_IPU：上拉输入（Input Pull-Up）
	// - "上拉"：内部电阻拉高，悬空时默认为高电平（串口空闲状态）
	// - 为什么用上拉？
	//   1. 串口空闲时 RX 引脚应为高电平（符合 RS232 标准）
	//   2. 防止悬空引脚接收到噪声触发误中断
	//   3. 如果外部已有上拉（如 USB-TTL 自带），也可用浮空输入（GPIO_Mode_IN_FLOATING）
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;			// 上拉输入模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				// PA10 是 USART1_RX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		// 输入模式下速度参数无实际作用，保留设置
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* ========== 第三步：配置 USART1 参数 ========== */
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;								// 波特率：9600 bit/s
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	// 无硬件流控
	
	// 【核心变化】工作模式：发送 + 接收
	// USART_Mode_Tx：使能发送功能
	// USART_Mode_Rx：使能接收功能
	// 说明：两者可独立配置，按需使用
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	
	USART_InitStructure.USART_Parity = USART_Parity_No;					// 无奇偶校验
	USART_InitStructure.USART_StopBits = USART_StopBits_1;				// 1 位停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;			// 8 位数据位
	USART_Init(USART1, &USART_InitStructure);
	
	/* ========== 第四步：配置中断 ========== */
	/* 4.1 开启 USART 接收中断 */
	// USART_IT_RXNE：接收数据寄存器非空中断（Receive Data Register Not Empty）
	// 触发时机：
	//   - USART 硬件接收到一个完整字节后，自动将数据写入 DR 寄存器
	//   - 硬件置位 RXNE 标志，触发中断
	//   - CPU 跳转到 USART1_IRQHandler 执行
	// 优点：
	//   - 无需轮询，CPU 可执行其他任务
	//   - 响应及时，不会因主循环阻塞而丢失数据
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	/* 4.2 配置 NVIC（嵌套向量中断控制器）*/
	// 中断分组：决定抢占优先级和响应优先级的位数分配
	// NVIC_PriorityGroup_2：
	//   - 抢占优先级：2 位（0~3）
	//   - 响应优先级：2 位（0~3）
	// 说明：
	//   - 抢占优先级高的中断可打断抢占优先级低的中断
	//   - 响应优先级只在抢占优先级相同时有效（决定谁先响应）
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;						// USART1 中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						// 使能中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;				// 抢占优先级 1（数值越小优先级越高）
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;					// 响应优先级 1
	NVIC_Init(&NVIC_InitStructure);
	
	/* ========== 第五步：使能 USART1 ========== */
	USART_Cmd(USART1, ENABLE);
}

/**
  * 函    数：串口发送一个字节
  * 参    数：Byte 要发送的一个字节
  * 返 回 值：无
  */
void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);		//将字节数据写入数据寄存器，写入后USART自动生成时序波形
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	//等待发送完成
	/*下次写入数据寄存器会自动清除发送完成标志位，故此循环后，无需清除标志位*/
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
	for (i = 0; i < Length; i ++)		//根据数字长度遍历数字的每一位
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');	//依次调用Serial_SendByte发送每位数字
	}
}

/**
  * 函    数：使用printf需要重定向的底层函数
  * 参    数：保持原始格式即可，无需变动
  * 返 回 值：保持原始格式即可，无需变动
  */
int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);			//将printf的底层重定向到自己的发送字节函数
	return ch;
}

/**
  * 函    数：自己封装的prinf函数
  * 参    数：format 格式化字符串
  * 参    数：... 可变的参数列表
  * 返 回 值：无
  */
void Serial_Printf(char *format, ...)
{
	char String[100];				//定义字符数组
	va_list arg;					//定义可变参数列表数据类型的变量arg
	va_start(arg, format);			//从format开始，接收参数列表到arg变量
	vsprintf(String, format, arg);	//使用vsprintf打印格式化字符串和参数列表到字符数组中
	va_end(arg);					//结束变量arg
	Serial_SendString(String);		//串口发送字符数组（字符串）
}

/**
  * 函    数：获取串口接收标志位
  * 参    数：无
  * 返 回 值：串口接收标志位，范围：0~1，接收到数据后，标志位置1，读取后标志位自动清零
  */
uint8_t Serial_GetRxFlag(void)
{
	// 【标志位自动清零机制】
	// 为什么需要自动清零？
	//   - 避免重复处理同一个数据
	//   - 简化主循环逻辑（不需要手动清零）
	// 
	// 工作流程：
	//   1. 中断服务函数收到数据后，Serial_RxFlag = 1
	//   2. 主循环调用 Serial_GetRxFlag()
	//   3. 如果标志位为 1，返回 1 并清零
	//   4. 下次调用返回 0，直到收到新数据
	
	if (Serial_RxFlag == 1)			// 检查是否有新数据
	{
		Serial_RxFlag = 0;			// 清零标志位（已处理）
		return 1;					// 返回"有新数据"
	}
	return 0;						// 返回"无新数据"
}

/**
  * 函    数：获取串口接收的数据
  * 参    数：无
  * 返 回 值：接收的数据，范围：0~255
  */
uint8_t Serial_GetRxData(void)
{
	// 【数据读取】
	// 注意事项：
	//   1. 只返回最新接收到的一个字节
	//   2. 如果主循环处理速度跟不上接收速度，会丢失数据
	//   3. 建议在 Serial_GetRxFlag()==1 后立即调用此函数
	// 
	// 改进方案（下一课学习）：
	//   - 使用环形缓冲区（Ring Buffer）存储多个字节
	//   - 使用 DMA 自动搬运数据，降低 CPU 负担
	
	return Serial_RxData;			// 返回接收的数据变量
}

/**
  * 函    数：USART1中断函数
  * 参    数：无
  * 返 回 值：无
  * 注意事项：此函数为中断函数，无需调用，中断触发后自动执行
  *           函数名为预留的指定名称，可以从启动文件复制
  *           请确保函数名正确，不能有任何差异，否则中断函数将不能进入
  */
void USART1_IRQHandler(void)
{
	/* ========== 中断服务函数（ISR）设计原则 ========== */
	// 1. 快速进出：尽量减少执行时间（< 10μs）
	// 2. 避免阻塞：不要调用 Delay、printf 等耗时函数
	// 3. 最小化处理：只做必要的数据接收和标志位设置
	// 4. 数据转移：将复杂逻辑留给主循环处理
	
	/* ========== 步骤 1：判断中断源 ========== */
	// USART_GetITStatus()：检查是否是 RXNE 中断
	// 为什么需要判断？
	//   - USART 有多个中断源（RXNE、TXE、TC、IDLE 等）
	//   - 避免误处理其他中断事件
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		/* ========== 步骤 2：读取接收到的数据 ========== */
		// USART_ReceiveData()：从 USART1->DR 寄存器读取数据
		// 副作用：
		//   - 读取 DR 寄存器会自动清除 RXNE 标志位
		//   - 如果不读取，RXNE 标志会一直保持，导致持续触发中断
		Serial_RxData = USART_ReceiveData(USART1);
		
		/* ========== 步骤 3：置标志位 ========== */
		// 通知主循环"有新数据待处理"
		// 主循环通过 Serial_GetRxFlag() 轮询此标志位
		Serial_RxFlag = 1;
		
		/* ========== 步骤 4：清除中断标志位（可选）========== */
		// USART_ClearITPendingBit()：手动清除 RXNE 标志位
		// 说明：
		//   - 步骤 2 中读取 DR 寄存器已自动清除 RXNE 标志
		//   - 此处显式清除是为了代码规范和保险起见
		//   - 如果不清除，也不会影响功能（已被自动清除）
		// 建议：保留此行，提高代码可读性
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
	
	/* ========== 【重要提示】避免在中断中执行的操作 ========== */
	// ❌ 不要调用：Delay_ms()、Delay_us()
	//    - 会阻塞中断，导致其他中断无法响应
	// ❌ 不要调用：OLED_ShowString()、printf()
	//    - 耗时过长（OLED I2C 通信需 ~5ms）
	//    - 可能导致数据丢失或系统卡死
	// ❌ 不要执行：复杂计算、数据处理
	//    - 中断应快速返回，将处理逻辑留给主循环
	// ✅ 正确做法：
	//    - 中断只做数据接收 + 标志位设置
	//    - 主循环轮询标志位，执行复杂处理
}