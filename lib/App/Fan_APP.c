#include "main.h"
#include "Fan_APP.h"
#include "Config.h"
#include "MAX6650.h"

static Fan_APP_Mode_t fan_mode = FAN_APP_MODE_FORCE_OFF;
static bool fault=false;

void Fan_APP_Init(void)
{
    MAX6650_Init();
}
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