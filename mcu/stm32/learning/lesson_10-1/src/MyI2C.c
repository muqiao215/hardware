#include "stm32f10x.h"
#include "Delay.h"

/**
  * 函    数：I2C写SCL引脚电平
  * 参    数：BitValue 协议层传入的位值（0或1）
  * 返 回 值：无
  */
void MyI2C_W_SCL(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_10, (BitAction)BitValue);
	Delay_us(10);		//延时，确保电平保持时间符合I2C标准
}

/**
  * 函    数：I2C写SDA引脚电平
  * 参    数：BitValue 协议层传入的位值（0或1）
  * 返 回 值：无
  */
void MyI2C_W_SDA(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_11, (BitAction)BitValue);
	Delay_us(10);		//延时，确保电平保持时间符合I2C标准
}

/**
  * 函    数：I2C读SDA引脚电平
  * 参    数：无
  * 返 回 值：协议层读取的位值（0或1）
  */
uint8_t MyI2C_R_SDA(void)
{
	uint8_t BitValue;
	BitValue = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);
	Delay_us(10);		//延时
	return BitValue;
}

/**
  * 函    数：I2C初始化
  * 参    数：无
  * 返 回 值：无
  * 说    明：配置PB10(SCL)和PB11(SDA)为开漏输出模式
  *           开漏模式下，输出1为高阻态（靠外部上拉电阻拉高），输出0为低电平（强下拉）
  *           这种模式既能输出也能输入，且避免了多主机时的短路风险
  */
void MyI2C_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//开启GPIOB时钟
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;		//核心配置：开漏输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_10 | GPIO_Pin_11);			//默认拉高SCL和SDA，处于空闲状态
}

/**
  * 函    数：I2C起始信号
  * 参    数：无
  * 返 回 值：无
  * 说    明：SCL高电平期间，SDA从高电平切换到低电平
  */
void MyI2C_Start(void)
{
	MyI2C_W_SDA(1);	//确保SDA为高
	MyI2C_W_SCL(1);	//确保SCL为高
	MyI2C_W_SDA(0);	//SDA拉低，产生下降沿（起始信号）
	MyI2C_W_SCL(0);	//SCL拉低，占用总线，准备发送数据
}

/**
  * 函    数：I2C终止信号
  * 参    数：无
  * 返 回 值：无
  * 说    明：SCL高电平期间，SDA从低电平切换到高电平
  */
void MyI2C_Stop(void)
{
	MyI2C_W_SDA(0);	//确保SDA为低
	MyI2C_W_SCL(1);	//SCL拉高
	MyI2C_W_SDA(1);	//SDA拉高，产生上升沿（终止信号）
}

/**
  * 函    数：I2C发送一个字节
  * 参    数：Byte 要发送的一个字节
  * 返 回 值：无
  * 说    明：SCL低电平期间，主机将数据位依次放到SDA线上（高位先行）
  *           SCL高电平期间，从机读取数据位
  */
void MyI2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i ++)
	{
		MyI2C_W_SDA(!!(Byte & (0x80 >> i)));	//掩码提取每一位，并写入SDA
		MyI2C_W_SCL(1);							//SCL拉高，通知从机读取
		MyI2C_W_SCL(0);							//SCL拉低，准备下一位
	}
}

/**
  * 函    数：I2C接收一个字节
  * 参    数：无
  * 返 回 值：接收到的一个字节数据
  * 说    明：SCL低电平期间，从机将数据位放到SDA线上
  *           SCL高电平期间，主机读取数据位
  */
uint8_t MyI2C_ReceiveByte(void)
{
	uint8_t i, Byte = 0x00;
	MyI2C_W_SDA(1);		//主机释放SDA线（开漏输出写1即释放），交由从机控制
	for (i = 0; i < 8; i ++)
	{
		MyI2C_W_SCL(1);							//SCL拉高，读取从机数据
		if (MyI2C_R_SDA()){Byte |= (0x80 >> i);}//读取SDA电平，并存入Byte
		MyI2C_W_SCL(0);							//SCL拉低，准备下一位
	}
	return Byte;
}

/**
  * 函    数：I2C发送应答
  * 参    数：AckBit 应答位，0为应答，1为非应答
  * 返 回 值：无
  */
void MyI2C_SendAck(uint8_t AckBit)
{
	MyI2C_W_SDA(AckBit);	//主机发送应答位
	MyI2C_W_SCL(1);			//SCL拉高，从机读取应答
	MyI2C_W_SCL(0);			//SCL拉低
}

/**
  * 函    数：I2C接收应答
  * 参    数：无
  * 返 回 值：接收到的应答位，0为应答，1为非应答
  */
uint8_t MyI2C_ReceiveAck(void)
{
	uint8_t AckBit;
	MyI2C_W_SDA(1);			//主机释放SDA线
	MyI2C_W_SCL(1);			//SCL拉高，读取从机应答
	AckBit = MyI2C_R_SDA();	//读取SDA电平
	MyI2C_W_SCL(0);			//SCL拉低
	return AckBit;
}
