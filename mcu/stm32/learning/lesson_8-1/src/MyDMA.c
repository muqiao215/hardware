#include "stm32f10x.h"

uint16_t MyDMA_Size;  // 全局变量：保存传输数量，用于重复传输

/**
 * @brief  初始化DMA（内存到内存传输模式）
 * @param  AddrA: 源地址（DataA数组地址）
 * @param  AddrB: 目标地址（DataB数组地址）
 * @param  Size: 传输数量（字节数）
 */
void MyDMA_Init(uint32_t AddrA, uint32_t AddrB, uint16_t Size)
{
	MyDMA_Size = Size;                       // 保存传输数量，供MyDMA_Transfer()使用
	
	/* ========== 第一步：使能DMA时钟 ========== */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  // 使能DMA1时钟（AHB总线）
	
	/* ========== 第二步：配置DMA参数 ========== */
	DMA_InitTypeDef DMA_InitStructure;
	
	/* ========== 源地址配置（Peripheral端，实际是内存） ========== */
	DMA_InitStructure.DMA_PeripheralBaseAddr = AddrA;  // 源地址：DataA数组起始地址
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  // 数据大小：字节（8位）
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;  // 地址递增：每次传输后地址+1
	
	/* ========== 目标地址配置（Memory端） ========== */
	DMA_InitStructure.DMA_MemoryBaseAddr = AddrB;       // 目标地址：DataB数组起始地址
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  // 数据大小：字节（8位）
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  // 地址递增：每次传输后地址+1
	
	/* ========== 传输方向配置 ========== */
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  // 传输方向：外设→内存（实际是内存→内存）
	// 注意：M2M模式下，Peripheral和Memory都是内存，只是命名习惯
	
	/* ========== 传输参数配置 ========== */
	DMA_InitStructure.DMA_BufferSize = Size;           // 传输数量：4个字节
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;       // 传输模式：普通模式（单次传输，完成后停止）
	// Normal模式：传输完成后需要重新使能才能再次传输
	// Circular模式：传输完成后自动重新开始（循环传输）
	
	/* ========== M2M模式配置（关键！） ========== */
	DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;        // 内存到内存模式：使能
	// M2M模式特点：
	// 1. 不需要外设触发，软件直接启动
	// 2. 源和目标都是内存地址
	// 3. 传输速度最快（不受外设时钟限制）
	
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;  // 优先级：中等
	// 优先级选项：Low, Medium, High, VeryHigh
	// 当多个DMA通道同时请求时，高优先级先执行
	
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);       // 应用配置到DMA1通道1
	
	/* ========== 初始化时禁用DMA ========== */
	DMA_Cmd(DMA1_Channel1, DISABLE);                    // 禁用DMA，等待MyDMA_Transfer()触发
}

/**
 * @brief  执行DMA传输
 * @note   阻塞等待传输完成
 */
void MyDMA_Transfer(void)
{
	/* ========== 第一步：禁用DMA（安全措施） ========== */
	DMA_Cmd(DMA1_Channel1, DISABLE);                    // 禁用DMA，准备重新配置
	
	/* ========== 第二步：重新设置传输数量 ========== */
	// 每次传输前需要重新设置计数器，因为Normal模式传输完成后计数器会变为0
	DMA_SetCurrDataCounter(DMA1_Channel1, MyDMA_Size); // 设置传输数量（4个字节）
	
	/* ========== 第三步：使能DMA，开始传输 ========== */
	DMA_Cmd(DMA1_Channel1, ENABLE);                    // 使能DMA，硬件开始自动传输
	// 传输过程：
	// 1. DMA硬件自动从DataA[0]读取 → 写入DataB[0]
	// 2. 地址自动递增：DataA[1] → DataB[1]
	// 3. 继续传输：DataA[2] → DataB[2]
	// 4. 最后传输：DataA[3] → DataB[3]
	// 5. 传输完成，TC1标志位置位
	
	/* ========== 第四步：等待传输完成 ========== */
	// DMA1_FLAG_TC1：DMA1通道1传输完成标志位
	// RESET：标志位未置位（传输未完成）
	// SET：标志位已置位（传输完成）
	while (DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET); // 阻塞等待传输完成
	
	/* ========== 第五步：清除完成标志 ========== */
	DMA_ClearFlag(DMA1_FLAG_TC1);                      // 清除传输完成标志位
	// 清除标志位后，可以准备下次传输
}
