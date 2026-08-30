#ifndef __UART_CONTROL_H
#define __UART_CONTROL_H

#include <stdbool.h>
#include "stdint.h"

void UART_Init(void);
bool UART_COBSDecode(uint8_t *input,//Rx_buff
                     uint8_t *output,//Buff after decoding
                      uint8_t length);//Length of the input buffer  
void UART_Control_Process(void);


#endif /* __UART_CONTROL_H */