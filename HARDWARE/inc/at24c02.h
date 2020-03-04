#ifndef __AT24C02_H
#define __AT24C02_H
#include "stm32f10x.h"

#define DEVICE_ADDR 0xA0

void AT24C02_Init(void);
void AT24C02_WriteByte(u8 word_add, u8 data);
u8 AT24C02_ReadByte(u8 word_add);

#endif

