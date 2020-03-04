#ifndef __LCD1602_H
#define __LCD1602_H

#define RS_H GPIOC->BSRR |= 1<<6;
#define RS_L GPIOC->BRR |= 1<<6;
#define RW_L GPIOA->BRR |= 1<<11;
#define RW_H GPIOA->BSRR |= 1<<11;
#define E_L  GPIOB->BRR |= 1<<4;
#define E_H  GPIOB->BSRR |= 1<<4;

void LCD1602_Init(void);
void LCD1602_Write_Byte(unsigned char data);
void LCD1602_Write_Order(unsigned char flag, unsigned char order);
void LCD1602_Write_String(char s[]);
void LCD1602_CGRAM_Write(unsigned char cgram_data[]);
#endif

