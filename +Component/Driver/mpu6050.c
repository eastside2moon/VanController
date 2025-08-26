/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	MPU6050.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2020-09-24
	*
	*	版本： 		V1.0
	*
	*	说明： 		MPU6050驱动
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f1xx_hal.h"

//OS
#include "cmsis_os.h"

//驱动

//硬件
#include "mpu6050.h"


#define MPU6050_WRITE_ADDR		(MPU6050_ADDRESS << 1)

#define MPU6050_READ_ADDR		((MPU6050_ADDRESS << 1) | 1)


MPU6050_INFO mpu6050_info;


static unsigned short accel_p = 0;

static float gyro_p = 0;


extern I2C_HandleTypeDef hi2c1;


/*
************************************************************
*	函数名称：	MPU6050_WriteByte
*
*	函数功能：	写数据
*
*	入口参数：	reg：寄存器地址
*				data：数据
*
*	返回参数：	0-成功	1-失败
*
*	说明：		
************************************************************
*/
static _Bool MPU6050_WriteByte(unsigned char reg, unsigned char data)
{
	
	if(HAL_I2C_Mem_Write(&hi2c1, MPU6050_WRITE_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 200) == HAL_OK)
		return 0;
	else
		return 1;
	
}

/*
************************************************************
*	函数名称：	MPU6050_ReadByte
*
*	函数功能：	读数据
*
*	入口参数：	reg：寄存器地址
*				data：数据
*
*	返回参数：	读到的数据
*
*	说明：		
************************************************************
*/
static unsigned char MPU6050_ReadByte(unsigned char reg)
{
	
	unsigned char byte = 0;
	
	if(HAL_I2C_Mem_Read(&hi2c1, MPU6050_READ_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &byte, 1, 200) == HAL_OK)
		return byte;
	
	return 0xFF;
	
}

/*
************************************************************
*	函数名称：	MPU6050_WriteBytes
*
*	函数功能：	写数据
*
*	入口参数：	slave_addr：器件地址
*				reg：寄存器地址
*				data：数据地址
*				len：数据长度(字节)
*
*	返回参数：	0-成功	1-失败
*
*	说明：		
************************************************************
*/
unsigned char MPU6050_WriteBytes(unsigned char slave_addr, unsigned char reg, unsigned char len, unsigned char *data)
{
	
	if(HAL_I2C_Mem_Write(&hi2c1, MPU6050_WRITE_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, len, 200) == HAL_OK)
		return 0;
	else
		return 1;
	
}

/*
************************************************************
*	函数名称：	MPU6050_ReadBytes
*
*	函数功能：	读数据
*
*	入口参数：	slave_addr：器件地址
*				reg：寄存器地址
*				data：数据缓存
*				len：数据长度(字节)
*
*	返回参数：	0-成功	1-失败
*
*	说明：		
************************************************************
*/
unsigned char MPU6050_ReadBytes(unsigned char slave_addr, unsigned char reg, unsigned char len, unsigned char *data)
{
	
	if(HAL_I2C_Mem_Read(&hi2c1, MPU6050_READ_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, len, 200) == HAL_OK)
		return 0;
	else
		return 1;
	
}

/*
************************************************************
*	函数名称：	MPU6050_SetGyroFSR
*
*	函数功能：	设置陀螺仪满量程
*
*	入口参数：	fsr：值
*
*	返回参数：	0-成功	1-失败
*
*	说明：		0~3
************************************************************
*/
_Bool MPU6050_SetGyroFSR(unsigned char fsr)
{
	
	_Bool result = 1;
	
	if(fsr <= 3)
	{
		result = MPU6050_WriteByte(MPU_GYRO_CFG_REG, fsr << 3);
		
		switch(fsr)
		{
			case 0:			gyro_p = 131.072;			break;
			case 1:			gyro_p = 65.536;			break;
			case 2:			gyro_p = 32.768;			break;
			case 3:			gyro_p = 16.384;			break;
		}
	}
	
	return result;
	
}

/*
************************************************************
*	函数名称：	MPU6050_SetAccelFSR
*
*	函数功能：	设置加速计满量程
*
*	入口参数：	fsr：值
*
*	返回参数：	0-成功	1-失败
*
*	说明：		0~3
************************************************************
*/
_Bool MPU6050_SetAccelFSR(unsigned char fsr)
{
	
	_Bool result = 1;
	
	if(fsr <= 3)
	{
		result = MPU6050_WriteByte(MPU_ACCEL_CFG_REG, fsr << 3);
		
		switch(fsr)
		{
			case 0:			accel_p = 16384;			break;
			case 1:			accel_p = 8192;			break;
			case 2:			accel_p = 4096;			break;
			case 3:			accel_p = 2048;			break;
		}
	}
	
	return result;
	
}

/*
************************************************************
*	函数名称：	MPU6050_SetLPF
*
*	函数功能：	设置MPU6050的数字低通滤波器
*
*	入口参数：	lpf：数字低通滤波频率(Hz)
*
*	返回参数：	0-成功	1-失败
*
*	说明：		
************************************************************
*/
unsigned char MPU6050_SetLPF(unsigned short lpf)
{
	
	unsigned char data = 0;
	
	if(lpf >= 188)				data = 1;
	else if(lpf >= 98)			data = 2;
	else if(lpf >= 42)			data = 3;
	else if(lpf >= 20)			data = 4;
	else if(lpf >= 10)			data = 5;
	else						data = 6;
	
	return MPU6050_WriteByte(MPU_CFG_REG, data);
	
}

/*
************************************************************
*	函数名称：	MPU6050_SetRate
*
*	函数功能：	设置MPU6050的采样率(假定Fs=1KHz)
*
*	入口参数：	rate：设置MPU6050的采样率(假定Fs=1KHz)
*
*	返回参数：	0-成功	1-失败
*
*	说明：		
************************************************************
*/
unsigned char MPU6050_SetRate(unsigned short rate)
{
	
	unsigned char data = 0;

	if(rate > 1000)
		rate = 1000;

	if(rate < 4)
		rate = 4;

	data = 1000 / rate - 1;
	data = MPU6050_WriteByte(MPU_SAMPLE_RATE_REG, data);	//设置数字低通滤波器
	
	return MPU6050_SetLPF(rate / 2);						//自动设置LPF为采样率的一半
	
}

/*
************************************************************
*	函数名称：	MPU6050_Exist
*
*	函数功能：	MPU6050设备在线检查
*
*	入口参数：	无
*
*	返回参数：	0-未检测到	1-检测到
*
*	说明：		
************************************************************
*/
_Bool MPU6050_Exist(void)
{
	
	//检测MPU6050
	if(MPU6050_ReadByte(MPU_DEVICE_ID_REG) == 0x68)			//读取用户寄存器
		mpu6050_info.device_ok = 1;
	
	return mpu6050_info.device_ok;

}

/*
************************************************************
*	函数名称：	MPU6050_Init
*
*	函数功能：	初始化MPU6050
*
*	入口参数：	无
*
*	返回参数：	0-成功	1-失败
*
*	说明：		
************************************************************
*/
_Bool MPU6050_Init(void)
{
	
	_Bool result = 1;
	
	if(mpu6050_info.device_ok == 1)
	{
		MPU6050_WriteByte(MPU_PWR_MGMT1_REG, 0x80);			//复位
		osDelay(100);
		MPU6050_WriteByte(MPU_PWR_MGMT1_REG, 0x00);			//结束复位
		
		MPU6050_SetGyroFSR(3);								//设置陀螺仪满量程
		MPU6050_SetAccelFSR(0);								//设置加速计满量程
		osDelay(250);										//设置采样率
		MPU6050_WriteByte(MPU_USER_CTRL_REG, 0x00);
		MPU6050_WriteByte(MPU_INT_EN_REG, 0x00);			//关闭所有中断
		MPU6050_WriteByte(MPU_FIFO_EN_REG, 0X00);			//关闭FIFO
		MPU6050_WriteByte(MPU_INTBP_CFG_REG, 0X80);			//INT引脚低电平有效
		MPU6050_WriteByte(MPU_PWR_MGMT1_REG, 0x01);			//设置CLKSEL,PLL X轴为参考
		MPU6050_WriteByte(MPU_PWR_MGMT2_REG, 0x00);			//全部正常工作
		
		result = 0;
	}
	
	return result;
	
}

/*
************************************************************
*	函数名称：	MPU6050_ReadTemperature
*
*	函数功能：	读取温度
*
*	入口参数：	无
*
*	返回参数：	温度
*
*	说明：		
************************************************************
*/
float MPU6050_ReadTemperature(void)
{
	
	short tmp = 0;
	unsigned char tempT[2] = {0, 0};
	
	float temperature = 0;

	if(MPU6050_ReadBytes(MPU6050_ADDRESS, MPU_TEMP_OUTH_REG, 2, tempT) == 0)
	{
		tmp = ((unsigned short)tempT[0] << 8) | tempT[1];
		temperature = (double)tmp / 340 + 36.53;
		
		mpu6050_info.temperature = temperature;
	}
	
	return temperature;
	
}

/*
************************************************************
*	函数名称：	MPU6050_ReadGyro
*
*	函数功能：	读取角速度
*
*	入口参数：	无
*
*	返回参数：	0-成功		1-失败
*
*	说明：		
************************************************************
*/
_Bool MPU6050_ReadGyro(void)
{
	
	unsigned char cyro_t[6];
	_Bool result = 1;

	if(MPU6050_ReadBytes(MPU6050_ADDRESS, MPU_GYRO_XOUTH_REG, 6, cyro_t) == 0)
	{
		mpu6050_info.gyro_original[0] = (short)cyro_t[0] << 8 | cyro_t[1];
		mpu6050_info.gyro_original[1] = (short)cyro_t[2] << 8 | cyro_t[3];
		mpu6050_info.gyro_original[2] = (short)cyro_t[4] << 8 | cyro_t[5];
		
		result = 0;
	}
	
	return result;
	
}

/*
************************************************************
*	函数名称：	MPU6050_ReadAccel
*
*	函数功能：	读取加速度
*
*	入口参数：	无
*
*	返回参数：	0-成功		1-失败
*
*	说明：		
************************************************************
*/
_Bool MPU6050_ReadAccel(void)
{
	
	unsigned char accel_t[6];
	_Bool result = 1;

	if(MPU6050_ReadBytes(MPU6050_ADDRESS, MPU_ACCEL_XOUTH_REG, 6, accel_t) == 0)
	{
		mpu6050_info.accel_original[0] = (short)accel_t[0] << 8 | accel_t[1];
		mpu6050_info.accel_original[1] = (short)accel_t[2] << 8 | accel_t[3];
		mpu6050_info.accel_original[2] = (short)accel_t[4] << 8 | accel_t[5];
		
		result = 0;
	}
	
	return result;
}

/*
************************************************************
*	函数名称：	MPU6050_Read_Accel_Gyro
*
*	函数功能：	读取数据并计算
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void MPU6050_Read_Accel_Gyro(void)
{

	MPU6050_ReadGyro();
	osDelay(5);
	MPU6050_ReadAccel();
	
	mpu6050_info.accel[0] = (float)mpu6050_info.accel_original[0] / accel_p;
	mpu6050_info.accel[1] = (float)mpu6050_info.accel_original[1] / accel_p;
	mpu6050_info.accel[2] = (float)mpu6050_info.accel_original[2] / accel_p;
	
	mpu6050_info.gyro[0] = (float)mpu6050_info.gyro_original[0] / gyro_p;
	mpu6050_info.gyro[1] = (float)mpu6050_info.gyro_original[1] / gyro_p;
	mpu6050_info.gyro[2] = (float)mpu6050_info.gyro_original[2] / gyro_p;

}
