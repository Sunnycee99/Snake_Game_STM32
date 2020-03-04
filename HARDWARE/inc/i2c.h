#ifndef __I2C_H
#define __I2C_H
#include "stm32f10x.h"

#define MY_I2C_SDA_LOW() (GPIOB->BRR=1<<11)  //(GPIO_ResetBits(GPIOB, GPIO_Pin_11))
#define MY_I2C_SDA_HIGH() (GPIOB->BSRR=1<<11)

#define MY_I2C_SCL_LOW() (GPIOB->BRR=1<<10)
#define MY_I2C_SCL_HIGH() (GPIOB->BSRR=1<<10)

void My_I2C_Init(void);
void My_I2C_Start(void);
void My_I2C_Stop(void);
void My_I2C_Send_ACK(void);
void My_I2C_Send_NACK(void);
u8 My_I2C_WaitACK(void);
void My_I2C_WriteByte(u8 data);
u8 My_I2C_ReadByte(u8 if_ack);

#endif

