#ifndef __SNAKE_H
#define __SNAKE_H
#include "stm32f10x.h"

typedef struct
{
	u8 Area[255];  //各点所处区域
	u8 Row[255];  //各点所处的行
	u8 Col[255];  //各点所处的列
	u8 Len;  //贪吃蛇长度
	u8 Direction;  //贪吃蛇行进方向 0-向上 1-向右 2-向下 3-向左
	u16 Index;  //贪吃蛇每次移动后偏移量减1，Index为蛇头所处数组位置
	u16 FoodLocation;  //食物坐标，第一位：食物所处区域号 第二位：食物所处区域的列号 第三位：食物所处区域的行号
	u8 Best_Score;  //历史最高分
}Snake_Struct;

void Snake_Init(void);
void Snake_GenerateFood(u8 update_location);
void Snake_GenerateShape(void);
u8 Snake_JudgeDeath(void);
void Snake_UpCrossArea(void);
void Snake_DownCrossArea(void);
void Snake_RightCrossArea(void);
void Snake_LeftCrossArea(void);
void Snake_ShapeClear(void);

#endif

