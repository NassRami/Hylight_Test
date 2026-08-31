#include "App.h"
#include "Sensors_App.h"

void App_Init(void)
{
    Sensors_App_Init(void);
}

void App_Process(void)
{
    static uint32_t sensors_tick = 0U;
    uint32_t now;

    now = HAL_GetTick();
    /*Schedule the sensors for a measurement each 100 ms*/
    if ((now - sensors_tick) >= 100U)
    {
        sensors_tick = now;
        Sensors_App_Process();
    }
}