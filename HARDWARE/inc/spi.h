#ifndef __SPI_H
#define __SPI_H

#include "stm32f10x.h"

void SPI1_Init(void);
void SPI1_SetSpeed(u8 speed);
u8 SPI1_ReadWriteByte(u8 data);

#endif

