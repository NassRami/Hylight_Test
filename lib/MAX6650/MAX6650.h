#ifndef MAX6650_H
#define MAX6650_H
#include <stdbool.h>

#include "Config.h"
void MAX6650_Init(void);
bool MAX6650_SetMode(MAX6650_Mode_t mode);
bool MAX6650_SetClosedLoop(uint8_t speed_value);
MAX6650_Status_t MAX6650_CheckFan(void);


#endif  // MAX6650_H