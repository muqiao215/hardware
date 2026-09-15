#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "MyDMA.h"

uint8_t DataA[] = {0x01, 0x02, 0x03, 0x04};  // 源数组：DMA传输的源数据
uint8_t DataB[] = {0, 0, 0, 0};              // 目标数组：DMA传输的目标位置

int main(void)
{
	OLED_Init();                              // 初始化OLED显示屏（PB8=SCL, PB9=SDA）
	
	/* ========== 初始化DMA：配置内存到内存传输 ========== */
	// 参数说明：
	// (uint32_t)DataA：源地址（DataA数组的起始地址）
	// (uint32_t)DataB：目标地址（DataB数组的起始地址）
	// 4：传输数量（传输4个字节）
	MyDMA_Init((uint32_t)DataA, (uint32_t)DataB, 4);
	
	OLED_ShowString(1, 1, "DataA");          // 第1行显示"DataA"标签
	OLED_ShowString(3, 1, "DataB");          // 第3行显示"DataB"标签
	
	OLED_ShowHexNum(1, 8, (uint32_t)DataA, 8);  // 显示DataA数组的地址（16进制）
	OLED_ShowHexNum(3, 8, (uint32_t)DataB, 8);  // 显示DataB数组的地址（16进制）
		
	/* ========== 主循环：演示DMA传输过程 ========== */
	while (1)
	{
		/* ========== 第一步：修改源数组DataA ========== */
		DataA[0] ++;                          // DataA[0]自增：0x01 → 0x02
		DataA[1] ++;                          // DataA[1]自增：0x02 → 0x03
		DataA[2] ++;                          // DataA[2]自增：0x03 → 0x04
		DataA[3] ++;                          // DataA[3]自增：0x04 → 0x05
		
		/* ========== 第二步：显示修改后的DataA和DataB ========== */
		OLED_ShowHexNum(2, 1, DataA[0], 2);  // 第2行显示DataA[0]（2位16进制）
		OLED_ShowHexNum(2, 4, DataA[1], 2);   // 第2行显示DataA[1]
		OLED_ShowHexNum(2, 7, DataA[2], 2);   // 第2行显示DataA[2]
		OLED_ShowHexNum(2, 10, DataA[3], 2);  // 第2行显示DataA[3]
		OLED_ShowHexNum(4, 1, DataB[0], 2);   // 第4行显示DataB[0]（此时还未传输，仍为0）
		OLED_ShowHexNum(4, 4, DataB[1], 2);   // 第4行显示DataB[1]
		OLED_ShowHexNum(4, 7, DataB[2], 2);   // 第4行显示DataB[2]
		OLED_ShowHexNum(4, 10, DataB[3], 2);  // 第4行显示DataB[3]
		
		Delay_ms(1000);                       // 延时1秒，观察修改后的状态
		
		/* ========== 第三步：触发DMA传输 ========== */
		// DMA硬件自动将DataA的4个字节传输到DataB
		// 传输过程：CPU不参与，硬件自动完成
		MyDMA_Transfer();                     // 触发DMA传输：DataA → DataB
		
		/* ========== 第四步：显示传输后的结果 ========== */
		OLED_ShowHexNum(2, 1, DataA[0], 2);  // 显示DataA[0]（未改变）
		OLED_ShowHexNum(2, 4, DataA[1], 2);   // 显示DataA[1]
		OLED_ShowHexNum(2, 7, DataA[2], 2);   // 显示DataA[2]
		OLED_ShowHexNum(2, 10, DataA[3], 2);  // 显示DataA[3]
		OLED_ShowHexNum(4, 1, DataB[0], 2);   // 显示DataB[0]（已等于DataA[0]）
		OLED_ShowHexNum(4, 4, DataB[1], 2);   // 显示DataB[1]（已等于DataA[1]）
		OLED_ShowHexNum(4, 7, DataB[2], 2);   // 显示DataB[2]（已等于DataA[2]）
		OLED_ShowHexNum(4, 10, DataB[3], 2);  // 显示DataB[3]（已等于DataA[3]）
		// 此时DataB应该等于DataA，证明DMA传输成功

		Delay_ms(1000);                       // 延时1秒，观察传输结果
		// 循环继续，DataA继续自增，然后再次传输
	}
}
