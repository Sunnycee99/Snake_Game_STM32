/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	snake.c
	*
	*	���ߣ� 		Sunnycee
	*
	*	���ڣ� 		2020-03-02
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		̰���߲�����س���
	*
	*	�޸ļ�¼��	
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
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  //����0������ 0x00
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  //����1������ 0x01
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  //����2������ 0x02
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  //����3������ 0x03
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  //����4������ 0x04
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  //����5������ 0x05
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  //����6������ 0x06
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  //����7������ 0x07
};


/*
************************************************************
*	�������ƣ�	Snake_ShapeClear
*
*	�������ܣ�	̰��������Լ�������ǽ
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		���Snake_ShapeCode��������ǽ
************************************************************
*/
void Snake_ShapeClear()
{
	u8 i, j;
	
	memset(Snake_ShapeCode, 0x0, sizeof(u8)*252);
	//�������ұ߽�
	memset(Snake_ShapeCode, 0x10, sizeof(u8)*8);
	memset(Snake_ShapeCode+32, 0x10, sizeof(u8)*8);
	memset(Snake_ShapeCode+24, 0x01, sizeof(u8)*8);
	memset(Snake_ShapeCode+56, 0x01, sizeof(u8)*8);
	
	//�����ϱ߽�
	for(i=0;i<4;i++)
	{
		for(j=0;j<5;j++)
		{
			Snake_ShapeCode[i*8] = 0xFF;
		}
	}
	
	//�����±߽�
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
*	�������ƣ�	Snake_Init
*
*	�������ܣ�	̰������ؽṹ���ʼ��
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		Snake�ṹ���ʼ��
************************************************************
*/
void Snake_Init()
{
	Snake.Area[251] = 0;
	Snake.Col[251] = 1;
	Snake.Row[251] = 1;
	
	//����252�ĸ�ֵ����ֹ�տ�ʼ���ݴ���
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
*	�������ƣ�	Snake_GenerateFood
*
*	�������ܣ�	����ʳ��λ��
*
*	��ڲ�����	update_location��1-����λ�� 0-������λ��
*
*	���ز�����	��
*
*	˵����		��
************************************************************
*/
void Snake_GenerateFood(u8 update_location)
{
	u8 area, col, row;
	u16 random;
	
	if(update_location)
	{
		random = rand();  //����ʳ��λ��
		area = random/100%10%8;
		col = random/10%10%5;
		row = random%10%8;
		Snake.FoodLocation = area*100+col*10+row;
	}
	
	area = Snake.FoodLocation/100;
	col = Snake.FoodLocation/10%10;
	row = Snake.FoodLocation%10;
	
	//���ʳ����̰���ߺ�ǽ��λ�ó�ͻ��������ʳ��
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
*	�������ƣ�	Snake_GenerateShape
*
*	�������ܣ�	����̰���߸���λ��
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����	 �����252
************************************************************
*/
void Snake_GenerateShape()
{
	u8 i;

	for(i=0;i<Snake.Len;i++)
	{
		//�ж���������
		Snake_ShapeCode[Snake.Area[(Snake.Index+i)%252]*8
						+Snake.Row[(Snake.Index+i)%252]] |= 0x10>>Snake.Col[(Snake.Index+i)%252];
	}
}


/*
************************************************************
*	�������ƣ�	Snake_JudgeDeath
*
*	�������ܣ�	�ж�̰�����Ƿ�����
*
*	��ڲ�����	��
*
*	���ز�����	̰����״̬ 1-���� 0-δ����
*
*	˵���� ��
************************************************************
*/
u8 Snake_JudgeDeath()
{
	u8 i;
	
	//��ʱ̰����û��ײǽ���ܣ�ֻ����ҧ���Լ�
	if(Snake.Row[Snake.Index]>0 
			&& Snake.Row[Snake.Index]<7 
			&& Snake.Col[Snake.Index]<4
			&& Snake.Col[Snake.Index]>0)
	{
		for(i=1;i<Snake.Len;i++)
		{
			if(Snake.Area[Snake.Index]!=Snake.Area[(Snake.Index+i)%252])  //����%252��ֹ�������
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
	//����ײǽ
	else
	{
		//������������
		if(Snake.Area[Snake.Index]==1 || Snake.Area[Snake.Index]==2)
		{
			if(Snake.Row[Snake.Index]==0)
				return 1;
		}
		//������������
		else if(Snake.Area[Snake.Index]==5 || Snake.Area[Snake.Index]==6)
		{
			if(Snake.Row[Snake.Index]==7)
				return 1;
		}
		//�������Ͻ�
		else if(Snake.Area[Snake.Index]==0)
		{
			if(Snake.Row[Snake.Index]==0)
				return 1;
			if(Snake.Col[Snake.Index]==0)
				return 1;
		}
		//�������½�
		else if(Snake.Area[Snake.Index]==4)
		{
			if(Snake.Row[Snake.Index]==7)
				return 1;
			if(Snake.Col[Snake.Index]==0)
				return 1;
		}
		//�������Ͻ�
		else if(Snake.Area[Snake.Index]==3)
		{
			if(Snake.Row[Snake.Index]==0)
				return 1;
			if(Snake.Col[Snake.Index]==4)
				return 1;
		}
		//�������½�
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
*	�������ƣ�	Snake_UpCrossArea
*
*	�������ܣ�	���Ͽ�Խ�ַ�����
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵���� ��һ������絽��һ������ʱ����
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
*	�������ƣ�	Snake_DownCrossArea
*
*	�������ܣ�	���¿�Խ�ַ�����
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵���� ��һ������絽��һ������ʱ����
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
*	�������ƣ�	Snake_RightCrossArea
*
*	�������ܣ�	���ҿ�Խ�ַ�����
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵���� ��һ������絽��һ������ʱ����
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
*	�������ƣ�	Snake_LeftCrossArea
*
*	�������ܣ�	�����Խ�ַ�����
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵���� ��һ������絽��һ������ʱ����
************************************************************
*/
void Snake_LeftCrossArea()
{
	Snake.Area[Snake.Index] = Snake.Area[Snake.Index+1]-1;
	Snake.Row[Snake.Index] = Snake.Row[Snake.Index+1];
	Snake.Col[Snake.Index] = 4;
}

