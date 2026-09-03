#include "main.h"
#include "Fan_APP.h"
#include "Config.h"
#include "MAX6650.h"
#include <stdbool.h>

static Fan_APP_Mode_t fan_mode = FAN_APP_MODE_FORCE_OFF;
static bool fault=false;

/* @brief  Initialize the fan application
 * @retval None
 */
void Fan_APP_Init(void)
{
    MAX6650_Init();
}

/* @brief  Set the fan mode
 * @param  mode: The desired fan mode
 * @retval None
 */
void Fan_APP_SetMode(Fan_APP_Mode_t mode)
{
    fan_mode = mode;
    switch (fan_mode)
    {
        case FAN_APP_MODE_FORCE_ON:
            if(!MAX6650_SetMode(MAX6650_FAN_FULL_ON))
            {
                fault = true ;
            }
            break;

        case FAN_APP_MODE_FORCE_OFF:
            if(!MAX6650_SetMode(MAX6650_MODE_FULL_OFF))
            {
                fault = true ;
            }
            break;

        case FAN_APP_MODE_AUTO:
            if(!MAX6650_SetClosedLoop(64U))// the vitesse was taken arbirtary 
            {
               fault = true ; 
            }
            break;

        default:
            break;
    }
}

/* @brief  Process the fan application and check for faults
 * @retval None
 */
void Fan_APP_Process(void)
{
    MAX6650_Status_t status;
    if (fan_mode == FAN_APP_MODE_FORCE_OFF)// no error bc the fan is OFF
    {
        fault = false;
        return;
    }
    status = MAX6650_CheckFan();

    if( status == MAX6650_STATUS_OK)
    {
        fault = false;
    }
    else{
        fault = true; // hardware
    }
}

/* @brief  Get the current fan mode
 * @retval The current fan mode
 */
bool Fan_APP_Fault(void)
{
    return fault;
}

/* @brief  Get the current fan speed in RPM
 * @param  rpm: Pointer to a variable where the RPM value will be stored
 * @retval true if the RPM was successfully retrieved, false otherwise
 */
bool Fan_APP_GetRPM(uint16_t *rpm)
{
    return MAX6650_GetRPM(rpm);
}