#include "stm32f10x.h"
#include "MyI2C.h"
#include "MPU6050_Reg.h"

#define MPU6050_ADDRESS		0xD0		//MPU6050的I2C从机地址（AD0接GND时为0xD0，接VCC时为0xD2）

/**
  * 函    数：MPU6050写寄存器
  * 参    数：RegAddress 寄存器地址
  * 参    数：Data 要写入的数据
  * 返 回 值：无
  */
void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)
{
	MyI2C_Start();					//起始信号
	MyI2C_SendByte(MPU6050_ADDRESS);//发送从机地址，写模式
	MyI2C_ReceiveAck();				//接收应答
	MyI2C_SendByte(RegAddress);		//发送寄存器地址
	MyI2C_ReceiveAck();				//接收应答
	MyI2C_SendByte(Data);			//发送数据
	MyI2C_ReceiveAck();				//接收应答
	MyI2C_Stop();					//停止信号
}

/**
  * 函    数：MPU6050读寄存器
  * 参    数：RegAddress 寄存器地址
  * 返 回 值：读取到的数据
  */
uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	
	MyI2C_Start();					//起始信号
	MyI2C_SendByte(MPU6050_ADDRESS);//发送从机地址，写模式
	MyI2C_ReceiveAck();				//接收应答
	MyI2C_SendByte(RegAddress);		//发送寄存器地址（指定要读哪个寄存器）
	MyI2C_ReceiveAck();				//接收应答
	
	MyI2C_Start();					//重新起始（Restart）
	MyI2C_SendByte(MPU6050_ADDRESS | 0x01);	//发送从机地址，读模式（最后一位为1）
	MyI2C_ReceiveAck();				//接收应答
	Data = MyI2C_ReceiveByte();		//接收数据
	MyI2C_SendAck(1);				//发送非应答（NAK），告诉从机别发了，我要停了
	MyI2C_Stop();					//停止信号
	
	return Data;
}

/**
  * 函    数：MPU6050初始化
  * 参    数：无
  * 返 回 值：无
  */
void MPU6050_Init(void)
{
	MyI2C_Init();	//底层I2C初始化
	
	/*配置MPU6050寄存器*/
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);		//电源管理1：不复位，解除睡眠，时钟源选择X轴陀螺仪
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);		//电源管理2：6轴均不待机
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);		//采样率分频：10分频
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06);			//配置寄存器：DLPF（低通滤波）配置为最平滑模式
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);	//陀螺仪配置：满量程范围±2000°/s
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);	//加速度计配置：满量程范围±16g
}

/**
  * 函    数：获取MPU6050的ID
  * 参    数：无
  * 返 回 值：MPU6050的ID号（默认0x68）
  */
uint8_t MPU6050_GetID(void)
{
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);		//读取WHO_AM_I寄存器
}

/**
  * 函    数：获取MPU6050的数据
  * 参    数：AccX, AccY, AccZ 加速度计X、Y、Z轴数据
  * 参    数：GyroX, GyroY, GyroZ 陀螺仪X、Y、Z轴数据
  * 返 回 值：无
  * 说    明：使用指针返回多个数据
  */
void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
						int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
	uint8_t DataH, DataL;
	
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);	//读取加速度X轴高8位
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);	//读取加速度X轴低8位
	*AccX = (DataH << 8) | DataL;					//拼接为16位数据
	
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
