/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	key.c
	*
	*	���ߣ� 		Sunnycee
	*
	*	���ڣ� 		2020-03-02
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		��������
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/
#include "key.h"
#include "delay.h"


/*
************************************************************
*	�������ƣ�	Key_EXTI_Init
*
*	�������ܣ�	������ʼ��
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����	 ��
************************************************************
*/
void Key_EXTI_Init()
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	/* Enable GPIOC clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  
  /* Configure PC.11��12��13 pin as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Enable AFIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

  /* Connect EXTI0 Line to PA.11��12��13 pin */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource11);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource12);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13);

  /* Configure EXTI11��12��13 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line11 | EXTI_Line12 | EXTI_Line13;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI0 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

