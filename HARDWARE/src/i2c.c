/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	i2c.c
	*
	*	���ߣ� 		Sunnycee
	*
	*	���ڣ� 		2020-03-02
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		I2C��������
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/
#include "i2c.h"
#include "stm32f10x.h"
#include "delay.h"


/*
************************************************************
*	�������ƣ�	My_I2C_Init
*
*	�������ܣ�	I2C��ʼ������
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����	 PB11-SDA PB10-SCL
************************************************************
*/
void My_I2C_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;  //��©�������Ҫ�л��������
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10;  //SDA��SCL
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	MY_I2C_SDA_HIGH();
	MY_I2C_SCL_HIGH();
}


/*
************************************************************
*	�������ƣ�	My_I2C_Start
*
*	�������ܣ�	I2C Start�ź�
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����	 ��
************************************************************
*/
void My_I2C_Start()
{
	MY_I2C_SDA_HIGH();
	MY_I2C_SCL_HIGH();
	
	delay_us(4);
	MY_I2C_SDA_LOW();
	delay_us(4);
	MY_I2C_SCL_LOW();
}


/*
************************************************************
*	�������ƣ�	My_I2C_Stop
*
*	�������ܣ�	I2C Stop�ź�
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����	 ��
************************************************************
*/
void My_I2C_Stop()
{
	MY_I2C_SDA_LOW();
	MY_I2C_SCL_HIGH();
	
	delay_us(4);
	MY_I2C_SDA_HIGH();
	delay_us(4);
	MY_I2C_SCL_LOW();
}


/*
************************************************************
*	�������ƣ�	My_I2C_Send_ACK
*
*	�������ܣ�	����ACK
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����	 ��
************************************************************
*/
void My_I2C_Send_ACK()
{
	MY_I2C_SCL_LOW();
	MY_I2C_SDA_LOW();
	delay_us(4);
	MY_I2C_SCL_HIGH();
	delay_us(4);
	MY_I2C_SCL_LOW();
}


/*
************************************************************
*	�������ƣ�	My_I2C_Send_NACK
*
*	�������ܣ�	����NACK
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����	 ��
************************************************************
*/
void My_I2C_Send_NACK()
{
	MY_I2C_SCL_LOW();
	MY_I2C_SDA_HIGH();
	delay_us(4);
	MY_I2C_SCL_HIGH();
	delay_us(4);
	MY_I2C_SCL_LOW();
}


/*
************************************************************
*	�������ƣ�	My_I2C_WaitACK
*
*	�������ܣ�	�ȴ��ӻ�ACK
*
*	��ڲ�����	��
*
*	���ز�����	1-���ͳɹ�  0-����ʧ��
*
*	˵����	 ��
************************************************************
*/
u8 My_I2C_WaitACK()
{
	u8 flag, timeout=200;
	
	MY_I2C_SDA_HIGH();  //�ͷ�SDA
	MY_I2C_SCL_HIGH();
	delay_us(4);
	
	if(timeout--)
	{
		flag = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);  //��ȡSDA
		
		if(!flag)
		{
			MY_I2C_SCL_LOW();
			return 1;  //���ͳɹ�
		}
		else
			delay_ms(1);
	}
	MY_I2C_SCL_LOW();
	
	return 0;  //����ʧ��
}


/*
************************************************************
*	�������ƣ�	My_I2C_WriteByte
*
*	�������ܣ� дһ���ֽ�
*
*	��ڲ�����	data����Ҫ���������
*
*	���ز�����	��
*
*	˵����	 ��
************************************************************
*/
void My_I2C_WriteByte(u8 data)
{
	u8 i;
	
//	My_I2C_Start();
	MY_I2C_SCL_LOW();
	for(i=0;i<8;i++)
	{
		if((data&0x80)>>7)
			MY_I2C_SDA_HIGH();
		else
			MY_I2C_SDA_LOW();
		delay_us(4);
		MY_I2C_SCL_HIGH();
		delay_us(4);
		
		MY_I2C_SCL_LOW();
		
		data <<= 1;
	}
}


/*
************************************************************
*	�������ƣ�	My_I2C_ReadByte
*
*	�������ܣ�	Function
*
*	��ڲ�����	if_ack��0-������ACK  1-����ACK
*
*	���ز�����	�������ֽ�
*
*	˵����	 ��
************************************************************
*/
u8 My_I2C_ReadByte(u8 if_ack)
{
	u8 i, recv=0;
	
	MY_I2C_SDA_HIGH();  //�ͷ�SDA
	
	for(i=0;i<8;i++)
	{
		MY_I2C_SCL_HIGH();
		delay_us(4);
		recv += GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);  //��ȡSDA
		MY_I2C_SCL_LOW();
		delay_us(4);
		
		if(i!=7)
			recv <<= 1;
	}
	if(!if_ack)
	{
		My_I2C_Send_NACK();
	}
	else
	{
		My_I2C_Send_ACK();
	}
	
	return recv;
}

