#ifndef __SNAKE_H
#define __SNAKE_H
#include "stm32f10x.h"

typedef struct
{
	u8 Area[255];  //������������
	u8 Row[255];  //������������
	u8 Col[255];  //������������
	u8 Len;  //̰���߳���
	u8 Direction;  //̰�����н����� 0-���� 1-���� 2-���� 3-����
	u16 Index;  //̰����ÿ���ƶ���ƫ������1��IndexΪ��ͷ��������λ��
	u16 FoodLocation;  //ʳ�����꣬��һλ��ʳ����������� �ڶ�λ��ʳ������������к� ����λ��ʳ������������к�
	u8 Best_Score;  //��ʷ��߷�
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

