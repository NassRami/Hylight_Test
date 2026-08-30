#include "TCA9548A.h"
#include "main.h"
#include "Config.h"
#include <stdbool.h>

/******************************************
        Private functions 
*******************************************/

static bool TCA9548A_WriteControl(uint8_t value);

/*
@brief  Writes control value to TCA9548A
@param  value: Control value to write
@return true if successful, false otherwise
*/
static bool TCA9548A_WriteControl(uint8_t value)
{
    HAL_StatusTypeDef hal_status;
    bool status = false;

    hal_status = HAL_I2C_Master_Transmit(
        &hi2c2,
        (uint16_t)(TCA9548A_ADDRESS << 1U),
        &value,
        1U,
        I2C_TIMEOUT
    );

    if (hal_status == HAL_OK)
    {
        status = true;
    }

    return status;
}
/*
@brief  Activates a specific channel on the TCA9548A
@param  channel: Channel number to activate (0-7)
@return true if successful, false otherwise 
*/

bool TCA9548A_ActiveChannel(uint8_t channel)
{
    if (channel >= TCA9548A_CHANNEL_COUNT)
    {
        return false; // Invalid channel
    }

    uint8_t channel_num = (1U << channel);
    return TCA9548A_WriteControl(channel_num);
}