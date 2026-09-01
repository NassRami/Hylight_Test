#include "main.h"
#include "BMP280.h"
#include "Config.h"
#include <stdbool.h>

static uint32_t sensors_press_val[BMP280_SENSOR_COUNT];
static bool sensors_fault = false;

static int16_t delta_p1 = 0;
static int16_t delta_p2 = 0;
static int16_t delta_p3 = 0;

void  Sensors_App_Init(void)
{
    BMP280_InitALL();
}

void Sensors_App_Process(void)
{
    uint8_t channel;
    int32_t temp = 0;
    sensors_fault = false;
    for (channel = 0U;
         channel < BMP280_SENSOR_COUNT;
         channel++)
    {
        if(!BMP280_Mesure(
                channel,
                &temp,
                &sensors_press_val[channel]))
        {
            sensors_fault = true;
        }
    //Calcul DeltaP
    delta_p1 =(int16_t)((int32_t)sensors_press_val[1] -(int32_t)sensors_press_val[0]);

    delta_p2 =(int16_t)((int32_t)sensors_press_val[3] -(int32_t)sensors_press_val[2]);

    delta_p3 = (int16_t)((int32_t)sensors_press_val[5] -(int32_t)sensors_press_val[4]);
    }
}

bool Sensors_App_Fault(void)
{
    return sensors_fault;
}