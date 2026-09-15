#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "AD.h"

uint16_t ADValue;    // ADC原始值：0-4095（12位ADC，2^12=4096）
float Voltage;       // 转换后的电压值：0.0-3.3V

int main(void)
{
	OLED_Init();     // 初始化OLED显示屏（PB8=SCL, PB9=SDA）
	AD_Init();       // 初始化ADC1：配置PA0作为模拟输入通道
	
	OLED_ShowString(1, 1, "ADValue:");      // 第1行显示"ADValue:"标签
	OLED_ShowString(2, 1, "Voltage:0.00V"); // 第2行显示"Voltage:"标签
	
	/* ========== 主循环：持续采集并显示ADC值 ========== */
	while (1)
	{
		ADValue = AD_GetValue();                    // 读取ADC原始值（0-4095）
		Voltage = (float)ADValue / 4095 * 3.3;      // 转换为电压值
		// 公式说明：12位ADC分辨率 = 4096级（0-4095）
		// 参考电压 = 3.3V
		// 电压 = (ADC值 / 4095) × 3.3V
		// 例如：ADValue=2048 → Voltage = 2048/4095 × 3.3 = 1.65V
		
		OLED_ShowNum(1, 9, ADValue, 4);            // 第1行第9列显示ADC原始值（4位数字）
		OLED_ShowNum(2, 9, Voltage, 1);            // 第2行第9列显示电压整数部分（1位）
		OLED_ShowNum(2, 11, (uint16_t)(Voltage * 100) % 100, 2); // 第2行第11列显示电压小数部分（2位）
		// 小数部分计算：(Voltage × 100) % 100
		// 例如：Voltage=1.65 → 165 % 100 = 65 → 显示"65"
		
		Delay_ms(100);                             // 延时100ms，每100ms采集一次
	}
}
