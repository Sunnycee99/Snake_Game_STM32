/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	spi.c
	*
	*	作者： 		Sunnycee
	*
	*	日期： 		2020-03-02
	*
	*	版本： 		V1.0
	*
	*	说明： 		SPI驱动程序
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/
#include "spi.h"

/*
************************************************************
*	函数名称：	SPI1_Init
*
*	函数功能：	SPI1初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：	 SPI1与ADXL362通信
************************************************************
*/
void SPI1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef   SPI_InitStructure;
	
	RCC_PCLK2Config(RCC_HCLK_Div2);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA, ENABLE);
	
	  /* Configure SPIy pins: SCK, MISO and MOSI ---------------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;;
	/* Configure MISO pin as Input Floating  */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	  /* SPIy Config -------------------------------------------------------------*/
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);
	
  SPI_Cmd(SPI1, ENABLE);
	
	SPI1_ReadWriteByte(0xff);										//启动传输
}


/*
************************************************************
*	函数名称：	SPI1_SetSpeed
*
*	函数功能：	SPI1传输速度设置
*
*	入口参数：	speed：传输速度
*
*	返回参数：	无
*
*	说明：	 speed必须为SPI_BaudRatePrescaler支持格式
************************************************************
*/
void SPI1_SetSpeed(u8 speed)
{
	SPI_InitTypeDef   SPI_InitStructure;
	
	SPI_InitStructure.SPI_BaudRatePrescaler = speed;
	SPI_Init(SPI1, &SPI_InitStructure);
	
	SPI_Cmd(SPI1, ENABLE);
}


/*
************************************************************
*	函数名称：	SPI1_ReadWriteByte
*
*	函数功能：	SPI1读写数据
*
*	入口参数：	data：需要传输的数据
*
*	返回参数：	无
*
*	说明：	 无
************************************************************
*/
u8 SPI1_ReadWriteByte(u8 data)
{
	u8 retry=0;
	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
	{
		retry++;
		if(retry>200)
			return 0;
	}
	
	retry = 0;
	
	SPI_I2S_SendData(SPI1, data);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
	{
		retry++;
		if(retry>200)
			return 0;
	}
	return SPI_I2S_ReceiveData(SPI1);
}

