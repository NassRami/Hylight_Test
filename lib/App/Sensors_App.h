#ifndef SENSORS_APP_H
#define SENSORS_APP_H
#include <stdbool.h>
#include <stdint.h>
void  Sensors_App_Init(void);
void Sensors_App_Process(void);
bool Sensors_App_Fault(void);
int16_t Sensors_GetDeltaP1(void);
int16_t Sensors_GetDeltaP2(void);
int16_t Sensors_GetDeltaP3(void);



#endif //SENSORS_APP_H