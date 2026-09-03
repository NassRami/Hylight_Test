#ifndef __UART_CONTROL_H
#define __UART_CONTROL_H

#include <stdbool.h>
#include "stdint.h"

void UART_Init(void);
bool UART_COBSDecode(uint8_t *input,//Rx_buff
                     uint8_t *output,//Buff after decoding
                      uint8_t length);//Length of the input buffer  
void UART_Control_Process(void);

void UART_Control_TransmitDiagnostic(
    int16_t delta_p1,
    int16_t delta_p2,
    int16_t delta_p3,
    uint16_t fan_rpm,
    uint8_t flags);


#endif /* __UART_CONTROL_H */