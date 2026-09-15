#include "stm32f10x.h"

void AD_Init(void)
{
	/* ========== 第一步：使能时钟 ========== */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);  // 使能ADC1时钟（APB2总线）
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能GPIOA时钟（APB2总线）
	
	/* ========== 第二步：配置ADC时钟 ========== */
	// ADC时钟不能超过14MHz，APB2时钟=72MHz，所以需要分频
	// 72MHz ÷ 6 = 12MHz（在14MHz限制内）
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);                     // 配置ADC时钟：APB2 ÷ 6 = 12MHz
	
	/* ========== 第三步：配置GPIO为模拟输入 ========== */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;         // 模拟输入模式：用于ADC采集
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;             // PA0引脚（ADC1通道0固定映射到PA0）
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // 速度设置（模拟输入模式下此参数无效）
	GPIO_Init(GPIOA, &GPIO_InitStructure);                // 应用配置到GPIOA
	// 注意：模拟输入模式下，引脚内部断开数字电路，只连接ADC
	
	/* ========== 第四步：配置ADC通道 ========== */
	// 参数说明：
	// ADC1：使用ADC1外设
	// ADC_Channel_0：通道0（对应PA0）
	// 1：规则通道序列中的第1个位置（单通道时就是1）
	// ADC_SampleTime_55Cycles5：采样时间55.5个ADC时钟周期
	//   采样时间越长，精度越高，但转换速度越慢
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
	
	/* ========== 第五步：配置ADC工作模式 ========== */
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;              // 独立模式：ADC1独立工作（非双ADC模式）
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;           // 数据右对齐：12位数据存储在低12位
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 外部触发：无（使用软件触发）
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;              // 连续转换：禁用（单次转换模式）
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;                     // 扫描模式：禁用（单通道模式）
	ADC_InitStructure.ADC_NbrOfChannel = 1;                          // 通道数量：1个通道
	ADC_Init(ADC1, &ADC_InitStructure);                               // 应用配置到ADC1
	
	/* ========== 第六步：使能ADC ========== */
	ADC_Cmd(ADC1, ENABLE);                                            // 使能ADC1外设
	
	/* ========== 第七步：ADC校准（提高精度） ========== */
	// 校准过程：ADC内部自动校准，消除偏移误差
	ADC_ResetCalibration(ADC1);                                      // 复位校准寄存器
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);              // 等待复位校准完成
	ADC_StartCalibration(ADC1);                                       // 启动校准
	while (ADC_GetCalibrationStatus(ADC1) == SET);                   // 等待校准完成
	// 校准完成后，ADC才能获得最佳精度
}

/**
 * @brief  获取ADC转换值
 * @return ADC原始值（0-4095，12位分辨率）
 */
uint16_t AD_GetValue(void)
{
	/* ========== 启动软件触发转换 ========== */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);                          // 软件触发ADC转换开始
	
	/* ========== 等待转换完成 ========== */
	// ADC_FLAG_EOC：转换结束标志位
	// RESET：标志位未置位（转换未完成）
	// SET：标志位已置位（转换完成）
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);          // 等待转换完成（阻塞等待）
	
	/* ========== 读取转换结果 ========== */
	return ADC_GetConversionValue(ADC1);                              // 读取ADC数据寄存器（DR）的值
	// 返回值范围：0-4095（12位ADC）
	// 0：对应0V（或VREF-）
	// 4095：对应3.3V（或VREF+）
}
