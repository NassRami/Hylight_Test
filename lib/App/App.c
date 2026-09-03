#include "App.h"
#include "Sensors_App.h"
#include "Fan_APP.h"
#include "main.h"
#include "UART_Control.h"
#include "CAN_DIAG.h"

/* @brief  Initialize the application
 * @param  None
 * @retval None
 */
void App_Init(void)
{
    Sensors_App_Init();
    Fan_APP_Init();
    UART_Init();
    CAN_Init();
}

/* @brief  Application process
 * @param  None
 * @retval None
 */
void App_Process(void)
{
    static uint32_t sensors_tick = 0U;
    static uint32_t fan_tick = 0U;
    static uint32_t can_tick = 0U;
    static uint32_t uart_tick = 0U;

    uint32_t now;
    int16_t delta_p1 ;
    int16_t delta_p2 ;
    int16_t delta_p3 ;
    uint16_t fan_rpm=0 ;


    now = HAL_GetTick();

    UART_Control_Process();

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
    if ((now - can_tick) >= 200U)// CAN tx periodicity 200ms
    {
        can_tick = now;
        delta_p1 = Sensors_GetDeltaP1();
        delta_p2 = Sensors_GetDeltaP2();
        delta_p3 = Sensors_GetDeltaP3();
        if(!Fan_APP_GetRPM(&fan_rpm))
        {
            fan_rpm=0;
        }
        CAN_Transmit(
            delta_p1,
            delta_p2,
            delta_p3,
            fan_rpm);
    }
    if ((now - uart_tick) >= 500U)
    {
    uart_tick = now;

    delta_p1 = Sensors_GetDeltaP1();
    delta_p2 = Sensors_GetDeltaP2();
    delta_p3 = Sensors_GetDeltaP3();

    if (!Fan_APP_GetRPM(&fan_rpm))
    {
        fan_rpm = 0U;
    }

    UART_Control_TransmitDiagnostic(
        delta_p1,
        delta_p2,
        delta_p3,
        fan_rpm,
        App_GetStatusFlags());
    }
}
/* @brief  Get the status flags of the application
 * @param  None
 * @retval Status flags
 */
uint8_t App_GetStatusFlags(void)
{
    uint8_t flags = 0U;

    if (Sensors_App_Fault())
    {
        flags |= STATUS_SENSOR_FAULT;
    }
    if(Fan_APP_Fault())
    {
        flags |= STATUS_FAN_FAULT;
    }

    return flags;
}