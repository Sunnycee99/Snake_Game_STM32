#ifndef __BEEP_H
#define __BEEP_H

#define BEEP_ON (GPIOB->BSRR = 1<<3)
#define BEEP_OFF (GPIOB->BRR = 1<<3)
void Beep_Init(void);


#endif

