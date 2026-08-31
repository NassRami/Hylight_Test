#include "App.h"
#include "Sensors_App.h"
#include "Fan_APP.h"
#include "main.h"

void App_Init(void)
{
    Sensors_App_Init();
    Fan_APP_Init();
}

void App_Process(void)
{
    static uint32_t sensors_tick = 0U;
    static uint32_t fan_tick = 0U;
    uint32_t now;

    now = HAL_GetTick();
    /*Schedule the sensors for a measurement each 100 ms*/
    if ((now - sensors_tick) >= 100U)
    {
        sensors_tick = now;
        Sensors_App_Process();
    }
    /* SCHEDULE THE FAN CHECK EACH 500MS*/
    if ((now - fan_tick) >= 500)
    {
        fan_tick = now;

        Fan_APP_Process();
    }
}