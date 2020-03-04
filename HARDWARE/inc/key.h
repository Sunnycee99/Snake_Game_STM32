#ifndef __KEY_H
#define __KEY_H
#include "stm32f10x.h"

void Key_Init(void);
u8 Key_Scan(u8 continuous_flag);
u8 Key_Scan_2(u8 if_continuous);
void Key_EXTI_Init(void);


#endif

