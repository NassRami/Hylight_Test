#include "BMP280.h"
#include "TCA9548A.h"
#include "Config.h"
#include "main.h"
#include <stdbool.h>

/******************************************
        Private functions
*******************************************/

static bool BMP280_ReadRegister(uint8_t channel, uint8_t reg, uint8_t *data, uint16_t length);
static bool BMP280_WriteRegister(uint8_t channel,uint8_t reg, uint8_t *data, uint16_t length);


static bool BMP280_ReadRegister(uint8_t channel, uint8_t reg, uint8_t *data, uint16_t length)
{
    HAL_StatusTypeDef hal_status;
    bool status = false;
    // Check if the channel is valid
    if (channel >= TCA9548A_CHANNEL_COUNT)
    {
        return false; // Invalid channel
    }
    // Enable the specified channel on the TCA9548A
    if (!TCA9548A_ActiveChannel(channel))
    {
        return false; // Failed to enable the channel
    }

    hal_status = HAL_I2C_Mem_Read(
        &hi2c2,
        (uint16_t)(BMP280_ADDRESS << 1U),
        reg,
        I2C_MEMADD_SIZE_8BIT,
        data,
        length,
        I2C_TIMEOUT
    );

    if (hal_status == HAL_OK)
    {
        status = true;
    }

    return status;
}

static bool BMP280_WriteRegister(uint8_t channel,uint8_t reg, uint8_t *data, uint16_t length)
{
    HAL_StatusTypeDef hal_status;
    bool status = false;
    // Check if the channel is valid
    if (channel >= TCA9548A_CHANNEL_COUNT)
    {
        return false; // Invalid channel
    }
    // Enable the specified channel on the TCA9548A
    if (!TCA9548A_EnableChannel(channel))
    {
        return false; // Failed to enable the channel
    }
    hal_status = HAL_I2C_Mem_Write(
        &hi2c2,
        (uint16_t)(BMP280_ADDRESS << 1U),
        reg,
        I2C_MEMADD_SIZE_8BIT,
        data,
        length,
        I2C_TIMEOUT
    );

    if (hal_status == HAL_OK)
    {
        status = true;
    }

    return status;
}