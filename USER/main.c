/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	main.c
	*
	*	作者： 		Sunnycee
	*
	*	日期： 		2020-03-02
	*
	*	版本： 		V1.0
	*
	*	说明： 		主程序
	*
	*	修改记录：	
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

u8 SPEED_SWITCH = 0, If_Run = 1, If_Die = 0, Key_EN = 1;  //{if_run: 暂停-0 运行-1}  {if_die: 死亡-1 未死亡-0}
u8 Xword[]={ 
    0x18,0x18,0x07,0x08,0x08,0x08,0x07,0x00,  //℃，代码 0x00
    0x00,0x00,0x00,0x00,0xff,0x00,0x00,0x00,  //一，代码 0x01
    0x00,0x00,0x00,0x0e,0x00,0xff,0x00,0x00,  //二，代码 0x02
    0x00,0x00,0xff,0x00,0x0e,0x00,0xff,0x00,  //三，代码 0x03
    0x00,0x00,0xff,0xf5,0xfb,0xf1,0xff,0x00,  //四，代码 0x04
    0x00,0xfe,0x08,0xfe,0x0a,0x0a,0xff,0x00,  //五，代码 0x05
    0x00,0x04,0x00,0xff,0x00,0x0a,0x11,0x00,  //六，代码 0x06
    0x00,0x1f,0x11,0x1f,0x11,0x11,0x1f,0x00,  //日，代码 0x07
};

u16 Speed[] = {1000, 500, 250, 100};  //贪吃蛇移动速度


/*
************************************************************
*	函数名称：	Hardware_Init
*
*	函数功能：	硬件初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：	 单片机外设初始化
************************************************************
*/
void Hardware_Init(void)
{
	delay_init();
	LCD1602_Init();
	AT24C02_Init();
	Snake_Init();
	ADXL362_Init();										//三轴加速计初始化
	Key_EXTI_Init();
	Beep_Init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}


/*
************************************************************
*	函数名称：	main
*
*	函数功能：	主函数
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：	 无
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
					//向上移动
					case 0:
					{
						for(i=0;i<Snake.Len;i++)
						{
							//是否跨越区域
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
					
					//向右移动
					case 1:
					{
						//是否跨越区域
						if(Snake.Col[Snake.Index+1]==4)
							Snake_RightCrossArea();
						else
						{
							Snake.Area[Snake.Index] = Snake.Area[Snake.Index+1];
							Snake.Row[Snake.Index] = Snake.Row[Snake.Index+1];
							Snake.Col[Snake.Index] = Snake.Col[Snake.Index+1]+1;
						}
					}break;
					
					//向下移动
					case 2:
					{
						//是否跨越区域
						if(Snake.Row[Snake.Index+1]==7)
							Snake_DownCrossArea();
						else
						{
							Snake.Area[Snake.Index] = Snake.Area[Snake.Index+1];
							Snake.Row[Snake.Index] = Snake.Row[Snake.Index+1]+1;
							Snake.Col[Snake.Index] = Snake.Col[Snake.Index+1];
						}
					}break;
					
					//向左移动
					case 3:
					{
						//是否跨越区域
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
				//贪吃蛇没有死亡
				if(Snake_JudgeDeath()==0)
				{
					Snake_GenerateShape();
					
					//吃到食物
					if(Snake.FoodLocation%1000==(u16)(Snake.Area[Snake.Index]*100
																						+Snake.Col[Snake.Index]*10
																						+Snake.Row[Snake.Index]))
					{
						Snake.Len++;
						Snake_GenerateFood(1);
						Snake_GenerateShape();
					}
					else
						Snake_GenerateFood(0);  //显示以前的食物位置
					
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
				//贪吃蛇已死亡
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
		
		//游戏暂停
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
*	函数名称：	EXTI15_10_IRQHandler
*
*	函数功能：	按键外部中断服务函数
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：	 Key1（短按）：重新开始  Key1（长按）：重新开始并清除最高记录
				Key2：改变速度  Key3：启动/暂停
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
			SPEED_SWITCH = (SPEED_SWITCH+1)%4;  //改变速度
			
			LCD1602_Write_Order(0,0x80+0x4B );
			LCD1602_Write_Byte(SPEED_SWITCH+1+'0');
		}
		
		if(Key_EN&&(GPIOC->IDR&(1<<12))==0)
		{
			If_Run = !If_Run;  //暂停、启动
		}
		
		if((GPIOC->IDR&(1<<13))==0)
		{
			while((GPIOC->IDR&(1<<13))==0)
			{
				delay_ms(10);
				delay_time++;
			}
			//重新开始游戏
			if(delay_time<200)
			{
				Snake_Init();
				If_Run = 1;
				If_Die = 0;
				Key_EN = 1;
				SPEED_SWITCH = 0;
			}
			//重新开始游戏并清除排行榜
			else
			{
				AT24C02_WriteByte(0x00, 0x00);
				delay_ms(2);  //延时防止读写出错
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

