/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	snake.c
	*
	*	作者： 		Sunnycee
	*
	*	日期： 		2020-03-02
	*
	*	版本： 		V1.0
	*
	*	说明： 		贪吃蛇操作相关程序
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/
#include "snake.h"
#include "stdlib.h"
#include "string.h"
#include "at24c02.h"

Snake_Struct Snake;

u8 Snake_ShapeCode[]={ 
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  //区域0，代码 0x00
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  //区域1，代码 0x01
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  //区域2，代码 0x02
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  //区域3，代码 0x03
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  //区域4，代码 0x04
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  //区域5，代码 0x05
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  //区域6，代码 0x06
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  //区域7，代码 0x07
};


/*
************************************************************
*	函数名称：	Snake_ShapeClear
*
*	函数功能：	贪吃蛇清除以及构筑外墙
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		清空Snake_ShapeCode，加上外墙
************************************************************
*/
void Snake_ShapeClear()
{
	u8 i, j;
	
	memset(Snake_ShapeCode, 0x0, sizeof(u8)*252);
	//生成左右边界
	memset(Snake_ShapeCode, 0x10, sizeof(u8)*8);
	memset(Snake_ShapeCode+32, 0x10, sizeof(u8)*8);
	memset(Snake_ShapeCode+24, 0x01, sizeof(u8)*8);
	memset(Snake_ShapeCode+56, 0x01, sizeof(u8)*8);
	
	//生成上边界
	for(i=0;i<4;i++)
	{
		for(j=0;j<5;j++)
		{
			Snake_ShapeCode[i*8] = 0xFF;
		}
	}
	
	//生成下边界
	for(i=0;i<4;i++)
	{
		for(j=0;j<5;j++)
		{
			Snake_ShapeCode[(i+4)*8+7] = 0xFF;
		}
	}
}


/*
************************************************************
*	函数名称：	Snake_Init
*
*	函数功能：	贪吃蛇相关结构体初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		Snake结构体初始化
************************************************************
*/
void Snake_Init()
{
	Snake.Area[251] = 0;
	Snake.Col[251] = 1;
	Snake.Row[251] = 1;
	
	//加入252的赋值，防止刚开始数据错误
	Snake.Area[252] = 0;
	Snake.Col[252] = 1;
	Snake.Row[252] = 1;
	
	Snake.Len = 1;
	Snake.Direction = 1;
	Snake.Index = 251;
	Snake.Best_Score = AT24C02_ReadByte(0x00);
}


/*
************************************************************
*	函数名称：	Snake_GenerateFood
*
*	函数功能：	生成食物位置
*
*	入口参数：	update_location：1-更新位置 0-不更新位置
*
*	返回参数：	无
*
*	说明：		无
************************************************************
*/
void Snake_GenerateFood(u8 update_location)
{
	u8 area, col, row;
	u16 random;
	
	if(update_location)
	{
		random = rand();  //更新食物位置
		area = random/100%10%8;
		col = random/10%10%5;
		row = random%10%8;
		Snake.FoodLocation = area*100+col*10+row;
	}
	
	area = Snake.FoodLocation/100;
	col = Snake.FoodLocation/10%10;
	row = Snake.FoodLocation%10;
	
	//如果食物与贪吃蛇和墙壁位置冲突，再生成食物
	if(Snake_ShapeCode[area*8+row]&(0x10>>col))
		Snake_GenerateFood(1);
	else
	{
		Snake_ShapeCode[area*8+row] += 0x10>>col;
	}
	return ;
}


/*
************************************************************
*	函数名称：	Snake_GenerateShape
*
*	函数功能：	生成贪吃蛇各点位置
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：	 蛇身最长252
************************************************************
*/
void Snake_GenerateShape()
{
	u8 i;

	for(i=0;i<Snake.Len;i++)
	{
		//判断所处区域
		Snake_ShapeCode[Snake.Area[(Snake.Index+i)%252]*8
						+Snake.Row[(Snake.Index+i)%252]] |= 0x10>>Snake.Col[(Snake.Index+i)%252];
	}
}


/*
************************************************************
*	函数名称：	Snake_JudgeDeath
*
*	函数功能：	判断贪吃蛇是否死亡
*
*	入口参数：	无
*
*	返回参数：	贪吃蛇状态 1-死亡 0-未死亡
*
*	说明： 无
************************************************************
*/
u8 Snake_JudgeDeath()
{
	u8 i;
	
	//此时贪吃蛇没有撞墙可能，只可能咬到自己
	if(Snake.Row[Snake.Index]>0 
			&& Snake.Row[Snake.Index]<7 
			&& Snake.Col[Snake.Index]<4
			&& Snake.Col[Snake.Index]>0)
	{
		for(i=1;i<Snake.Len;i++)
		{
			if(Snake.Area[Snake.Index]!=Snake.Area[(Snake.Index+i)%252])  //加上%252防止溢出数组
				continue;
			else
			{
				if(Snake.Row[Snake.Index]!=Snake.Row[(Snake.Index+i)%252])
					continue;
				else
				{
					if(Snake.Col[Snake.Index]!=Snake.Col[(Snake.Index+i)%252])
						continue;
					else
						return 1;
				}
			}
		}
		return 0;
	}
	//可能撞墙
	else
	{
		//处于上中两格
		if(Snake.Area[Snake.Index]==1 || Snake.Area[Snake.Index]==2)
		{
			if(Snake.Row[Snake.Index]==0)
				return 1;
		}
		//处于下中两格
		else if(Snake.Area[Snake.Index]==5 || Snake.Area[Snake.Index]==6)
		{
			if(Snake.Row[Snake.Index]==7)
				return 1;
		}
		//处于左上角
		else if(Snake.Area[Snake.Index]==0)
		{
			if(Snake.Row[Snake.Index]==0)
				return 1;
			if(Snake.Col[Snake.Index]==0)
				return 1;
		}
		//处于左下角
		else if(Snake.Area[Snake.Index]==4)
		{
			if(Snake.Row[Snake.Index]==7)
				return 1;
			if(Snake.Col[Snake.Index]==0)
				return 1;
		}
		//处于右上角
		else if(Snake.Area[Snake.Index]==3)
		{
			if(Snake.Row[Snake.Index]==0)
				return 1;
			if(Snake.Col[Snake.Index]==4)
				return 1;
		}
		//处于右下角
		else if(Snake.Area[Snake.Index]==7)
		{
			if(Snake.Row[Snake.Index]==7)
				return 1;
			if(Snake.Col[Snake.Index]==4)
				return 1;
		}
	}
		
	return 0;
}


/*
************************************************************
*	函数名称：	Snake_UpCrossArea
*
*	函数功能：	向上跨越字符区域
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明： 从一个区域跨到另一个区域时调用
************************************************************
*/
void Snake_UpCrossArea()
{
	Snake.Area[Snake.Index] = Snake.Area[Snake.Index+1]-4;
	Snake.Row[Snake.Index] = 7;
	Snake.Col[Snake.Index] = Snake.Col[Snake.Index+1];
}


/*
************************************************************
*	函数名称：	Snake_DownCrossArea
*
*	函数功能：	向下跨越字符区域
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明： 从一个区域跨到另一个区域时调用
************************************************************
*/
void Snake_DownCrossArea()
{
	Snake.Area[Snake.Index] = Snake.Area[Snake.Index+1]+4;
	Snake.Row[Snake.Index] = 0;
	Snake.Col[Snake.Index] = Snake.Col[Snake.Index+1];
}


/*
************************************************************
*	函数名称：	Snake_RightCrossArea
*
*	函数功能：	向右跨越字符区域
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明： 从一个区域跨到另一个区域时调用
************************************************************
*/
void Snake_RightCrossArea()
{
	Snake.Area[Snake.Index] = Snake.Area[Snake.Index+1]+1;
	Snake.Row[Snake.Index] = Snake.Row[Snake.Index+1];
	Snake.Col[Snake.Index] = 0;
}


/*
************************************************************
*	函数名称：	Snake_LeftCrossArea
*
*	函数功能：	向左跨越字符区域
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明： 从一个区域跨到另一个区域时调用
************************************************************
*/
void Snake_LeftCrossArea()
{
	Snake.Area[Snake.Index] = Snake.Area[Snake.Index+1]-1;
	Snake.Row[Snake.Index] = Snake.Row[Snake.Index+1];
	Snake.Col[Snake.Index] = 4;
}

