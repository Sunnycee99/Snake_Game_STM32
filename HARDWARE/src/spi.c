/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	spi.c
	*
	*	���ߣ� 		Sunnycee
	*
	*	���ڣ� 		2020-03-02
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		SPI��������
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/
#include "spi.h"

/*
************************************************************
*	�������ƣ�	SPI1_Init
*
*	�������ܣ�	SPI1��ʼ��
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����	 SPI1��ADXL362ͨ��
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
	
	SPI1_ReadWriteByte(0xff);										//��������
}


/*
************************************************************
*	�������ƣ�	SPI1_SetSpeed
*
*	�������ܣ�	SPI1�����ٶ�����
*
*	��ڲ�����	speed�������ٶ�
*
*	���ز�����	��
*
*	˵����	 speed����ΪSPI_BaudRatePrescaler֧�ָ�ʽ
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
*	�������ƣ�	SPI1_ReadWriteByte
*
*	�������ܣ�	SPI1��д����
*
*	��ڲ�����	data����Ҫ���������
*
*	���ز�����	��
*
*	˵����	 ��
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

