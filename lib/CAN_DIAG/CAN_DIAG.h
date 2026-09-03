#ifndef CAN_DIAG_H
#define CAN_DIAG_H

#include <stdint.h>
#include <stdbool.h>
bool CAN_Transmit(int16_t delta_p1, int16_t delta_p2, int16_t delta_p3, uint16_t fan_rpm);
bool CAN_Init(void);


#endif // CAN_DIAG_H    