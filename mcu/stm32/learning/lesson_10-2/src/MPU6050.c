#include "stm32f10x.h"
#include "MPU6050_Reg.h"

#define MPU6050_ADDRESS		0xD0

/**
  * 函    数：MPU6050等待I2C事件
  * 参    数：I2Cx I2C外设
  * 参    数：I2C_EVENT 等待的目标事件
  * 说    明：硬件I2C是异步的，每一步操作（如发送起始位）都需要一定时间。
  *           必须等待硬件标志位（Event）置位后，才能进行下一步。
  *           为了防止硬件卡死导致程序死循环，这里增加了一个简单的超时退出机制。
  */
void MPU6050_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)
{
	uint32_t Timeout;
	Timeout = 10000; 	// 定义超时计数器
	while (I2C_CheckEvent(I2Cx, I2C_EVENT) != SUCCESS)
	{
		Timeout --;
		if (Timeout == 0) break; // 超时跳出，避免死锁
	}
}

/**
  * 函    数：MPU6050写寄存器 (硬件I2C版)
  * 参    数：RegAddress 寄存器地址
  * 参    数：Data 要写入的数据
  * 流程说明：
  * 1. 发送起始位 (Start) -> 等待 EV5 (Master Mode Select)
  * 2. 发送从机地址(写模式) -> 等待 EV6 (Transmitter Mode Selected)
  * 3. 发送寄存器地址 -> 等待 EV8 (Byte Transmitting)
  * 4. 发送数据 -> 等待 EV8_2 (Byte Transmitted)
  * 5. 发送停止位 (Stop)
  */
void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)
{
	I2C_GenerateSTART(I2C2, ENABLE);									//硬件生成起始条件
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);				//等待EV5事件（起始条件已发送）
	
	I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Transmitter);//发送从机地址，方向：发送
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);//等待EV6事件（地址发送完成，主机模式已选定）
	
	I2C_SendData(I2C2, RegAddress);										//发送数据（这里是寄存器地址）
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING);		//等待EV8事件（数据正在发送/移位寄存器为空）
	
	I2C_SendData(I2C2, Data);											//发送数据（这里是真正要写的数据）
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);			//等待EV8_2事件（数据发送完成）
	
	I2C_GenerateSTOP(I2C2, ENABLE);										//硬件生成终止条件
}

/**
  * 函    数：MPU6050读寄存器 (硬件I2C版)
  * 参    数：RegAddress 寄存器地址
  * 返 回 值：读取到的数据
  * 流程说明：
  * 1. 写过程：Start -> EV5 -> Address(Write) -> EV6 -> RegAddress -> EV8
  * 2. 重启过程：Start (Restart) -> EV5 -> Address(Read) -> EV6
  * 3. 接收过程：
  *    - 提前关闭ACK（因为只读一个字节）
  *    - 提前请求Stop
  *    - 等待 EV7 (Byte Received)
  *    - 读取数据
  *    - 恢复ACK
  */
uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	
	/* 第一步：伪写入，目的是为了告诉从机我们要操作哪个寄存器 */
	I2C_GenerateSTART(I2C2, ENABLE);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);
	
	I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Transmitter);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	
	I2C_SendData(I2C2, RegAddress);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
	
	/* 第二步：重复起始条件，切换到读模式 */
	I2C_GenerateSTART(I2C2, ENABLE);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);
	
	I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Receiver);	//注意这里方向是 Receiver
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);
	
	/* 第三步：读取一个字节的特殊处理 */
	/* 因为只读1个字节，所以在读取之前就要告诉硬件：收完这个不要应答(ACK=0)，并且立刻准备停止 */
	I2C_AcknowledgeConfig(I2C2, DISABLE);
	I2C_GenerateSTOP(I2C2, ENABLE);
	
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED);			//等待EV7事件（数据寄存器非空）
	Data = I2C_ReceiveData(I2C2);										//从数据寄存器读出数据
	
	I2C_AcknowledgeConfig(I2C2, ENABLE);								//恢复ACK为使能，方便后续通信
	
	return Data;
}

/**
  * 函    数：MPU6050初始化
  * 说    明：配置硬件I2C2接口
  */
void MPU6050_Init(void)
{
	/* 1. 开启时钟：I2C2 和 GPIOB */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	/* 2. 配置GPIO：必须为 复用开漏输出 (AF_OD) */
	/* 复用：引脚控制权交给片上外设I2C，而不是CPU直接控制ODR */
	/* 开漏：I2C协议物理层要求 */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* 3. 配置I2C外设参数 */
	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_ClockSpeed = 50000;		// 时钟速度 50kHz (标准模式)
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;		// 使能自动应答
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;		// STM32自身的从机地址（做从机时才用，这里随意）
	I2C_Init(I2C2, &I2C_InitStructure);
	
	I2C_Cmd(I2C2, ENABLE); // 使能I2C外设
	
	/* 4. 配置MPU6050内部寄存器 */
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06);
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);
}

uint8_t MPU6050_GetID(void)
{
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}

void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
						int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
	uint8_t DataH, DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
	*AccX = (DataH << 8) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
	*AccY = (DataH << 8) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
	*AccZ = (DataH << 8) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
	*GyroX = (DataH << 8) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
	*GyroY = (DataH << 8) | DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
	*GyroZ = (DataH << 8) | DataL;
}
