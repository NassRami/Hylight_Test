#include "main.h"
#include "MAX6650.h"
#include <stdbool.h>
#include <stdint.h>
#include "Config.h"





static bool MAX6650_ReadRegister(uint8_t reg, uint8_t *data);
static bool MAX6650_WriteRegister(uint8_t reg, uint8_t data);

static bool MAX6650_ReadRegister(uint8_t reg, uint8_t *data)
{
    HAL_StatusTypeDef hal_status;

    if (data == NULL)
    {
        return false;
    }

    hal_status = HAL_I2C_Mem_Read(
        &hi2c2,
        (uint16_t)(MAX6650_ADDRESS << 1U),
        reg,
        I2C_MEMADD_SIZE_8BIT,
        data,
        1U,
        I2C_TIMEOUT
    );

    return (hal_status == HAL_OK);
}
static bool MAX6650_WriteRegister(uint8_t reg, uint8_t data)
{
    HAL_StatusTypeDef hal_status;

    hal_status = HAL_I2C_Mem_Write(
        &hi2c2,
        (uint16_t)(MAX6650_ADDRESS << 1U),
        reg,
        I2C_MEMADD_SIZE_8BIT,
        &data,
        1U,
        I2C_TIMEOUT
    );

    return (hal_status == HAL_OK);
}
static bool MAX6650_WriteMode(uint8_t mode)
{
    uint8_t config;

    config =
        mode |
        8 | //FAN_12V 
        2 ; // DIV 4

    return MAX6650_WriteRegister(
        MAX6650_CONFIG_REG,
        config
    );
}
bool MAX6650_SetMode(MAX6650_Mode_t mode)
{
    bool status;

    switch (mode)
    {
        case MAX6650_FAN_FULL_ON:

            status = MAX6650_WriteMode(
                MAX6650_MODE_FULL_ON
            );

            break;


        case MAX6650_FAN_FULL_OFF:

            status = MAX6650_WriteMode(
                MAX6650_MODE_FULL_OFF
            );

            break;


        case MAX6650_FAN_CLOSED_LOOP:

            status = MAX6650_WriteMode(
                MAX6650_MODE_CLOSED_LOOP
            );

            break;


        default:

            return false;
    }

    if (!status)
    {
        return false;
    }

    return true;
}
void MAX6650_Init(void)
{
    /*
     * Configure tachometer
     */
    if (!MAX6650_WriteRegister(
            MAX6650_COUNT_REG,
            MAX6650_COUNT_1S))
    {
        return ;
    }

    /*
        Set the fan mode to full off. This ensures that the fan is not running at start.
     */
    if (!MAX6650_SetMode(
            MAX6650_FAN_FULL_OFF))
    {
        return ;
    }
}

bool MAX6650_SetClosedLoop(uint8_t speed_value)
{
    /* Pass to full ON first*/
    if (!MAX6650_SetMode(MAX6650_FAN_FULL_ON))
    {
        return false;
    }

    /* Delay for the fan to start */
    HAL_Delay(500U);

    if (!MAX6650_WriteRegister(MAX6650_SPEED_REG, speed_value))
    {
        return false;
    }
    if (!MAX6650_SetMode(MAX6650_MODE_CLOSED_LOOP))
    {
        return false;
    }

    return true;
}


bool MAX6650_GetRPM(uint16_t *rpm)
{
    uint8_t tach_count = 0U;

    if (rpm == NULL)
    {
        return false;
    }

    if (!MAX6650_ReadRegister(
            MAX6650_TACH0_REG,
            &tach_count))
    {
        return false;
    }

    /*
        We configured the MAX6650 to use a 1 second tachometer CT.
        the tach_count value is equal to the number of pulses
        received in 1 second the fan produces 2 pulses per revolution,
        the RPM can be calculated as follows:
        RPM = (tach_count / 2) * 60
            = tach_count * 30
     */

    *rpm =
        (uint16_t)tach_count * 30U;

    return true;
}
MAX6650_Status_t MAX6650_CheckFan(void)
{
    uint16_t rpm = 0U;
    
    if (!MAX6650_GetRPM(&rpm))
    {
        return MAX6650_STATUS_I2C_ERROR;
    }

    /*
     * Fan should run but RPM = 0.
     */
    if (rpm == 0U)
    {
        return MAX6650_STATUS_FAN_ERROR; //Disconnected or Hardware Problem
    }

    return MAX6650_STATUS_OK;
}
