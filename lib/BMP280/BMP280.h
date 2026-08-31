#ifndef BMP280_H
#define BMP280_H

#include <stdint.h>
#include <stdbool.h>

void BMP280_InitALL(void);
bool BMP280_Mesure(uint8_t channel, int32_t *temp, uint32_t *press);
#endif // BMP280_H