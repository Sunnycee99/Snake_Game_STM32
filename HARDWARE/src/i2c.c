/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	i2c.c
	*
	*	作者： 		Sunnycee
	*
	*	日期： 		2020-03-02
	*
	*	版本： 		V1.0
	*
	*	说明： 		I2C驱动程序
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/
#include "i2c.h"
#include "stm32f10x.h"
#include "delay.h"


/*
************************************************************
*	函数名称：	My_I2C_Init
*
*	函数功能：	I2C初始化函数
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：	 PB11-SDA PB10-SCL
************************************************************
*/
void My_I2C_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;  //开漏输出不需要切换输出方向
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10;  //SDA和SCL
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	MY_I2C_SDA_HIGH();
	MY_I2C_SCL_HIGH();
}


/*
************************************************************
*	函数名称：	My_I2C_Start
*
*	函数功能：	I2C Start信号
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：	 无
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
*	函数名称：	My_I2C_Stop
*
*	函数功能：	I2C Stop信号
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：	 无
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
*	函数名称：	My_I2C_Send_ACK
*
*	函数功能：	发送ACK
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：	 无
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
*	函数名称：	My_I2C_Send_NACK
*
*	函数功能：	发送NACK
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：	 无
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
*	函数名称：	My_I2C_WaitACK
*
*	函数功能：	等待从机ACK
*
*	入口参数：	无
*
*	返回参数：	1-发送成功  0-发送失败
*
*	说明：	 无
************************************************************
*/
u8 My_I2C_WaitACK()
{
	u8 flag, timeout=200;
	
	MY_I2C_SDA_HIGH();  //释放SDA
	MY_I2C_SCL_HIGH();
	delay_us(4);
	
	if(timeout--)
	{
		flag = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);  //读取SDA
		
		if(!flag)
		{
			MY_I2C_SCL_LOW();
			return 1;  //发送成功
		}
		else
			delay_ms(1);
	}
	MY_I2C_SCL_LOW();
	
	return 0;  //发送失败
}


/*
************************************************************
*	函数名称：	My_I2C_WriteByte
*
*	函数功能： 写一个字节
*
*	入口参数：	data：需要传输的数据
*
*	返回参数：	无
*
*	说明：	 无
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
*	函数名称：	My_I2C_ReadByte
*
*	函数功能：	Function
*
*	入口参数：	if_ack：0-不发送ACK  1-发送ACK
*
*	返回参数：	读到的字节
*
*	说明：	 无
************************************************************
*/
u8 My_I2C_ReadByte(u8 if_ack)
{
	u8 i, recv=0;
	
	MY_I2C_SDA_HIGH();  //释放SDA
	
	for(i=0;i<8;i++)
	{
		MY_I2C_SCL_HIGH();
		delay_us(4);
		recv += GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);  //读取SDA
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

