#ifndef APP_H
#define APP_H

#include <stdint.h>
#include <stdbool.h>

#define STATUS_SENSOR_FAULT    0x01U
#define STATUS_FAN_FAULT       0x02U

void App_Process(void);
void App_Init(void);
uint8_t App_GetStatusFlags(void);

#endif //APP_H