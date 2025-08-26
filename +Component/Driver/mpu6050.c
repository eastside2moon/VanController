/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	MPU6050.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2020-09-24
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		MPU6050����
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f1xx_hal.h"

//OS
#include "cmsis_os.h"

//����

//Ӳ��
#include "mpu6050.h"


#define MPU6050_WRITE_ADDR		(MPU6050_ADDRESS << 1)

#define MPU6050_READ_ADDR		((MPU6050_ADDRESS << 1) | 1)


MPU6050_INFO mpu6050_info;


static unsigned short accel_p = 0;

static float gyro_p = 0;


extern I2C_HandleTypeDef hi2c1;


/*
************************************************************
*	�������ƣ�	MPU6050_WriteByte
*
*	�������ܣ�	д����
*
*	��ڲ�����	reg���Ĵ�����ַ
*				data������
*
*	���ز�����	0-�ɹ�	1-ʧ��
*
*	˵����		
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
*	�������ƣ�	MPU6050_ReadByte
*
*	�������ܣ�	������
*
*	��ڲ�����	reg���Ĵ�����ַ
*				data������
*
*	���ز�����	����������
*
*	˵����		
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
*	�������ƣ�	MPU6050_WriteBytes
*
*	�������ܣ�	д����
*
*	��ڲ�����	slave_addr��������ַ
*				reg���Ĵ�����ַ
*				data�����ݵ�ַ
*				len�����ݳ���(�ֽ�)
*
*	���ز�����	0-�ɹ�	1-ʧ��
*
*	˵����		
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
*	�������ƣ�	MPU6050_ReadBytes
*
*	�������ܣ�	������
*
*	��ڲ�����	slave_addr��������ַ
*				reg���Ĵ�����ַ
*				data�����ݻ���
*				len�����ݳ���(�ֽ�)
*
*	���ز�����	0-�ɹ�	1-ʧ��
*
*	˵����		
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
*	�������ƣ�	MPU6050_SetGyroFSR
*
*	�������ܣ�	����������������
*
*	��ڲ�����	fsr��ֵ
*
*	���ز�����	0-�ɹ�	1-ʧ��
*
*	˵����		0~3
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
*	�������ƣ�	MPU6050_SetAccelFSR
*
*	�������ܣ�	���ü��ټ�������
*
*	��ڲ�����	fsr��ֵ
*
*	���ز�����	0-�ɹ�	1-ʧ��
*
*	˵����		0~3
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
*	�������ƣ�	MPU6050_SetLPF
*
*	�������ܣ�	����MPU6050�����ֵ�ͨ�˲���
*
*	��ڲ�����	lpf�����ֵ�ͨ�˲�Ƶ��(Hz)
*
*	���ز�����	0-�ɹ�	1-ʧ��
*
*	˵����		
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
*	�������ƣ�	MPU6050_SetRate
*
*	�������ܣ�	����MPU6050�Ĳ�����(�ٶ�Fs=1KHz)
*
*	��ڲ�����	rate������MPU6050�Ĳ�����(�ٶ�Fs=1KHz)
*
*	���ز�����	0-�ɹ�	1-ʧ��
*
*	˵����		
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
	data = MPU6050_WriteByte(MPU_SAMPLE_RATE_REG, data);	//�������ֵ�ͨ�˲���
	
	return MPU6050_SetLPF(rate / 2);						//�Զ�����LPFΪ�����ʵ�һ��
	
}

/*
************************************************************
*	�������ƣ�	MPU6050_Exist
*
*	�������ܣ�	MPU6050�豸���߼��
*
*	��ڲ�����	��
*
*	���ز�����	0-δ��⵽	1-��⵽
*
*	˵����		
************************************************************
*/
_Bool MPU6050_Exist(void)
{
	
	//���MPU6050
	if(MPU6050_ReadByte(MPU_DEVICE_ID_REG) == 0x68)			//��ȡ�û��Ĵ���
		mpu6050_info.device_ok = 1;
	
	return mpu6050_info.device_ok;

}

/*
************************************************************
*	�������ƣ�	MPU6050_Init
*
*	�������ܣ�	��ʼ��MPU6050
*
*	��ڲ�����	��
*
*	���ز�����	0-�ɹ�	1-ʧ��
*
*	˵����		
************************************************************
*/
_Bool MPU6050_Init(void)
{
	
	_Bool result = 1;
	
	if(mpu6050_info.device_ok == 1)
	{
		MPU6050_WriteByte(MPU_PWR_MGMT1_REG, 0x80);			//��λ
		osDelay(100);
		MPU6050_WriteByte(MPU_PWR_MGMT1_REG, 0x00);			//������λ
		
		MPU6050_SetGyroFSR(3);								//����������������
		MPU6050_SetAccelFSR(0);								//���ü��ټ�������
		osDelay(250);										//���ò�����
		MPU6050_WriteByte(MPU_USER_CTRL_REG, 0x00);
		MPU6050_WriteByte(MPU_INT_EN_REG, 0x00);			//�ر������ж�
		MPU6050_WriteByte(MPU_FIFO_EN_REG, 0X00);			//�ر�FIFO
		MPU6050_WriteByte(MPU_INTBP_CFG_REG, 0X80);			//INT���ŵ͵�ƽ��Ч
		MPU6050_WriteByte(MPU_PWR_MGMT1_REG, 0x01);			//����CLKSEL,PLL X��Ϊ�ο�
		MPU6050_WriteByte(MPU_PWR_MGMT2_REG, 0x00);			//ȫ����������
		
		result = 0;
	}
	
	return result;
	
}

/*
************************************************************
*	�������ƣ�	MPU6050_ReadTemperature
*
*	�������ܣ�	��ȡ�¶�
*
*	��ڲ�����	��
*
*	���ز�����	�¶�
*
*	˵����		
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
*	�������ƣ�	MPU6050_ReadGyro
*
*	�������ܣ�	��ȡ���ٶ�
*
*	��ڲ�����	��
*
*	���ز�����	0-�ɹ�		1-ʧ��
*
*	˵����		
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
*	�������ƣ�	MPU6050_ReadAccel
*
*	�������ܣ�	��ȡ���ٶ�
*
*	��ڲ�����	��
*
*	���ز�����	0-�ɹ�		1-ʧ��
*
*	˵����		
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
*	�������ƣ�	MPU6050_Read_Accel_Gyro
*
*	�������ܣ�	��ȡ���ݲ�����
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
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
