/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	main.c
	*
	*	���ߣ� 		Sunnycee
	*
	*	���ڣ� 		2020-03-02
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		������
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/
#include "stm32f10x.h"
#include "delay.h"
#include "lcd1602.h"
#include "key.h"
#include "beep.h"
#include "snake.h"
#include "spi.h"
#include "adxl362.h"
#include "at24c02.h"

#define ADXL_SENS 0.2

extern Snake_Struct Snake;
extern u8 Snake_ShapeCode[];

u8 SPEED_SWITCH = 0, If_Run = 1, If_Die = 0, Key_EN = 1;  //{if_run: ��ͣ-0 ����-1}  {if_die: ����-1 δ����-0}
u8 Xword[]={ 
    0x18,0x18,0x07,0x08,0x08,0x08,0x07,0x00,  //�棬���� 0x00
    0x00,0x00,0x00,0x00,0xff,0x00,0x00,0x00,  //һ������ 0x01
    0x00,0x00,0x00,0x0e,0x00,0xff,0x00,0x00,  //�������� 0x02
    0x00,0x00,0xff,0x00,0x0e,0x00,0xff,0x00,  //�������� 0x03
    0x00,0x00,0xff,0xf5,0xfb,0xf1,0xff,0x00,  //�ģ����� 0x04
    0x00,0xfe,0x08,0xfe,0x0a,0x0a,0xff,0x00,  //�壬���� 0x05
    0x00,0x04,0x00,0xff,0x00,0x0a,0x11,0x00,  //�������� 0x06
    0x00,0x1f,0x11,0x1f,0x11,0x11,0x1f,0x00,  //�գ����� 0x07
};

u16 Speed[] = {1000, 500, 250, 100};  //̰�����ƶ��ٶ�


/*
************************************************************
*	�������ƣ�	Hardware_Init
*
*	�������ܣ�	Ӳ����ʼ��
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����	 ��Ƭ�������ʼ��
************************************************************
*/
void Hardware_Init(void)
{
	delay_init();
	LCD1602_Init();
	AT24C02_Init();
	Snake_Init();
	ADXL362_Init();										//������ټƳ�ʼ��
	Key_EXTI_Init();
	Beep_Init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}


/*
************************************************************
*	�������ƣ�	main
*
*	�������ܣ�	������
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����	 ��
************************************************************
*/
int main(void)
{
	u8 i;
	
	Hardware_Init();
	Snake_GenerateFood(1);
	Snake_ShapeClear();
	while(1)
	{
		if(If_Run)
		{
			if(!If_Die)
			{
				ADXL362_GetValue();
				
				switch(Snake.Direction)
				{
					case 0:
					{
						if(adxl362Info.y>ADXL_SENS || adxl362Info.y<-ADXL_SENS)
							Snake.Direction = 0;
						else
						{
							if(adxl362Info.x<-ADXL_SENS)
								Snake.Direction = 3;
							else if(adxl362Info.x>ADXL_SENS)
								Snake.Direction = 1;
						}
					}break;
					
					case 1:
					{
						if(adxl362Info.x>ADXL_SENS || adxl362Info.x<-ADXL_SENS)
							Snake.Direction = 1;
						else
						{
							if(adxl362Info.y<-ADXL_SENS)
								Snake.Direction = 2;
							else if(adxl362Info.y>0.3)
								Snake.Direction = 0;
						}
					}break;
					
					case 2:
					{
						if(adxl362Info.y>ADXL_SENS || adxl362Info.y<-ADXL_SENS)
							Snake.Direction = 2;
						else
						{
							if(adxl362Info.x<-ADXL_SENS)
								Snake.Direction = 3;
							else if(adxl362Info.x>0.3)
								Snake.Direction = 1;
						}
					}break;
					
					case 3:
					{
						if(adxl362Info.x>ADXL_SENS || adxl362Info.x<-ADXL_SENS)
							Snake.Direction = 3;
						else
						{
							if(adxl362Info.y<-ADXL_SENS)
								Snake.Direction = 2;
							else if(adxl362Info.y>ADXL_SENS)
								Snake.Direction = 0;
						}
					}break;
				}

				switch(Snake.Direction)
				{
					//�����ƶ�
					case 0:
					{
						for(i=0;i<Snake.Len;i++)
						{
							//�Ƿ��Խ����
							if(Snake.Row[Snake.Index+1]==0)
								Snake_UpCrossArea();
							else
							{
								Snake.Area[Snake.Index] = Snake.Area[Snake.Index+1];
								Snake.Row[Snake.Index] = Snake.Row[Snake.Index+1]-1;
								Snake.Col[Snake.Index] = Snake.Col[Snake.Index+1];
							}
						}
					}break;
					
					//�����ƶ�
					case 1:
					{
						//�Ƿ��Խ����
						if(Snake.Col[Snake.Index+1]==4)
							Snake_RightCrossArea();
						else
						{
							Snake.Area[Snake.Index] = Snake.Area[Snake.Index+1];
							Snake.Row[Snake.Index] = Snake.Row[Snake.Index+1];
							Snake.Col[Snake.Index] = Snake.Col[Snake.Index+1]+1;
						}
					}break;
					
					//�����ƶ�
					case 2:
					{
						//�Ƿ��Խ����
						if(Snake.Row[Snake.Index+1]==7)
							Snake_DownCrossArea();
						else
						{
							Snake.Area[Snake.Index] = Snake.Area[Snake.Index+1];
							Snake.Row[Snake.Index] = Snake.Row[Snake.Index+1]+1;
							Snake.Col[Snake.Index] = Snake.Col[Snake.Index+1];
						}
					}break;
					
					//�����ƶ�
					case 3:
					{
						//�Ƿ��Խ����
						if(Snake.Col[Snake.Index+1]==0)
							Snake_LeftCrossArea();
						else
						{
							Snake.Area[Snake.Index] = Snake.Area[Snake.Index+1];
							Snake.Row[Snake.Index] = Snake.Row[Snake.Index+1];
							Snake.Col[Snake.Index] = Snake.Col[Snake.Index+1]-1;
						}
					}break;
				}
				//̰����û������
				if(Snake_JudgeDeath()==0)
				{
					Snake_GenerateShape();
					
					//�Ե�ʳ��
					if(Snake.FoodLocation%1000==(u16)(Snake.Area[Snake.Index]*100
																						+Snake.Col[Snake.Index]*10
																						+Snake.Row[Snake.Index]))
					{
						Snake.Len++;
						Snake_GenerateFood(1);
						Snake_GenerateShape();
					}
					else
						Snake_GenerateFood(0);  //��ʾ��ǰ��ʳ��λ��
					
					LCD1602_CGRAM_Write(Snake_ShapeCode);
					LCD1602_Write_Order(0,0x80);
					LCD1602_Write_Byte(0x00);
					LCD1602_Write_Byte(0x01);
					LCD1602_Write_Byte(0x02);
					LCD1602_Write_Byte(0x03);
					
					LCD1602_Write_Order(0,0x80+0x06);
					LCD1602_Write_Byte('L');
					LCD1602_Write_Byte('e');
					LCD1602_Write_Byte('n');
					LCD1602_Write_Byte(':');
					LCD1602_Write_Byte(Snake.Len/100+'0');
					LCD1602_Write_Byte(Snake.Len/10%10+'0');
					LCD1602_Write_Byte(Snake.Len%10+'0');
					
					LCD1602_Write_Order(0,0x80+0x40);
					LCD1602_Write_Byte(0x04);
					LCD1602_Write_Byte(0x05);
					LCD1602_Write_Byte(0x06);
					LCD1602_Write_Byte(0x07);
					
					LCD1602_Write_Order(0,0x80+0x46);
					LCD1602_Write_Byte('L');
					LCD1602_Write_Byte('v');
					LCD1602_Write_Byte(':');
					LCD1602_Write_Byte(SPEED_SWITCH+1+'0');
					
					LCD1602_Write_Order(0,0x80+0x4B);
					LCD1602_Write_Byte('B');
					LCD1602_Write_Byte(':');
					LCD1602_Write_Byte(Snake.Best_Score/100+'0');
					LCD1602_Write_Byte(Snake.Best_Score/10%10+'0');
					LCD1602_Write_Byte(Snake.Best_Score%10+'0');
				}
				//̰����������
				else
				{
					if(Snake.Len>Snake.Best_Score)
						AT24C02_WriteByte(0x00, Snake.Len);
					
					LCD1602_Write_Order(0,0x80);
					LCD1602_Write_Byte('G');
					LCD1602_Write_Byte('a');
					LCD1602_Write_Byte('m');
					LCD1602_Write_Byte('e');
					LCD1602_Write_Order(0,0x80+0x40);
					LCD1602_Write_Byte('O');
					LCD1602_Write_Byte('v');
					LCD1602_Write_Byte('e');
					LCD1602_Write_Byte('r');
					
					If_Die = 1;
					Key_EN = 0;
					BEEP_ON;
					delay_ms(500);
					BEEP_OFF;
				}
				
				if(Snake.Index==0)
					Snake.Index = 252;
				Snake.Index--;
				Snake_ShapeClear();
				delay_ms(Speed[SPEED_SWITCH]);
			}
		}
		
		//��Ϸ��ͣ
		else
		{
			LCD1602_Write_Order(0,0x80); 
			LCD1602_Write_Byte('P');
			LCD1602_Write_Byte('A');
			LCD1602_Write_Byte('U');
			LCD1602_Write_Byte('S');
			
			LCD1602_Write_Order(0,0x80+0x40);
			LCD1602_Write_Byte('E');
			LCD1602_Write_Byte(0xff);
			LCD1602_Write_Byte(0xff);
			LCD1602_Write_Byte(0xff);
			while(!If_Run)
			{
				LCD1602_Write_Order(0,0x80); 
				LCD1602_Write_Byte('P');
				LCD1602_Write_Byte('A');
				LCD1602_Write_Byte('U');
				LCD1602_Write_Byte('S');
				
				LCD1602_Write_Order(0,0x80+0x40);
				LCD1602_Write_Byte('E');
				LCD1602_Write_Byte(0xff);
				LCD1602_Write_Byte(0xff);
				LCD1602_Write_Byte(0xff);
				
				delay_ms(1000);
				
				LCD1602_Write_Order(0,0x80);
				LCD1602_Write_Byte(0x00);
				LCD1602_Write_Byte(0x01);
				LCD1602_Write_Byte(0x02);
				LCD1602_Write_Byte(0x03);
				
				LCD1602_Write_Order(0,0x80+0x40);
				LCD1602_Write_Byte(0x04);
				LCD1602_Write_Byte(0x05);
				LCD1602_Write_Byte(0x06);
				LCD1602_Write_Byte(0x07);
				
				delay_ms(1000);
			}
		}
	}
}


/*
************************************************************
*	�������ƣ�	EXTI15_10_IRQHandler
*
*	�������ܣ�	�����ⲿ�жϷ�����
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����	 Key1���̰��������¿�ʼ  Key1�������������¿�ʼ�������߼�¼
				Key2���ı��ٶ�  Key3������/��ͣ
************************************************************
*/
void EXTI15_10_IRQHandler(void)
{
	u16 delay_time = 0;
	delay_ms(10);
	if(EXTI_GetITStatus(EXTI_Line11)==SET || EXTI_GetITStatus(EXTI_Line12)==SET || EXTI_GetITStatus(EXTI_Line13)==SET)
	{
		if(Key_EN&&(GPIOC->IDR&(1<<11))==0)
		{
			SPEED_SWITCH = (SPEED_SWITCH+1)%4;  //�ı��ٶ�
			
			LCD1602_Write_Order(0,0x80+0x4B );
			LCD1602_Write_Byte(SPEED_SWITCH+1+'0');
		}
		
		if(Key_EN&&(GPIOC->IDR&(1<<12))==0)
		{
			If_Run = !If_Run;  //��ͣ������
		}
		
		if((GPIOC->IDR&(1<<13))==0)
		{
			while((GPIOC->IDR&(1<<13))==0)
			{
				delay_ms(10);
				delay_time++;
			}
			//���¿�ʼ��Ϸ
			if(delay_time<200)
			{
				Snake_Init();
				If_Run = 1;
				If_Die = 0;
				Key_EN = 1;
				SPEED_SWITCH = 0;
			}
			//���¿�ʼ��Ϸ��������а�
			else
			{
				AT24C02_WriteByte(0x00, 0x00);
				delay_ms(2);  //��ʱ��ֹ��д����
				Snake_Init();
				If_Run = 1;
				If_Die = 0;
				Key_EN = 1;
				SPEED_SWITCH = 0;
			}
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line11 | EXTI_Line12 | EXTI_Line13);
}

