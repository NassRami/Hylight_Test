#include "main.h"
#include "BMP280.h"
#include "Config.h"
#include <stdbool.h>
#include <stdint.h>

static uint32_t sensors_press_val[BMP280_SENSOR_COUNT];
static bool sensors_fault = false;
static int16_t delta_p1 = 0;
static int16_t delta_p2 = 0;
static int16_t delta_p3 = 0;

/* @brief  Initialize the sensors application
 * @retval None
 */
void  Sensors_App_Init(void)
{
    BMP280_InitALL();
}

/* @brief  Process the sensors and update the pressure values
 * @retval None
 */
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

/* @brief  Get the fault status of the sensors
 * @retval true if any sensor has a fault, false otherwise
 */
bool Sensors_App_Fault(void)
{
    return sensors_fault;
}

/* @brief  Get the pressure value from a specific sensor
 * @param  channel: The TCA9548A channel where the BMP280 is connected
 * @retval The pressure value in Pascals, or 0 if the channel is invalid
 */
int16_t Sensors_GetDeltaP1(void)
{
    return delta_p1;
}

/* @brief  Get the pressure value from a specific sensor
 * @param  channel: The TCA9548A channel where the BMP280 is connected
 * @retval The pressure value in Pascals, or 0 if the channel is invalid
 */
int16_t Sensors_GetDeltaP2(void)
{
    return delta_p2;
}

/* @brief  Get the pressure value from a specific sensor
 * @param  channel: The TCA9548A channel where the BMP280 is connected
 * @retval The pressure value in Pascals, or 0 if the channel is invalid
 */
int16_t Sensors_GetDeltaP3(void)
{
    return delta_p3;
}   