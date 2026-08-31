#include "BMP280.h"
#include "TCA9548A.h"
#include "Config.h"
#include "main.h"
#include <stdbool.h>


/******************************************
        Private variables
*******************************************/
typedef struct
{
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;

    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;

} BMP280_Calibration_t;
static BMP280_Calibration_t bmp280_calibration[BMP280_SENSOR_COUNT];
static int32_t bmp280_t_fine[BMP280_SENSOR_COUNT];
/******************************************
        Private functions
*******************************************/

static bool BMP280_ReadRegister(uint8_t channel, uint8_t reg, uint8_t *data, uint16_t length);
static bool BMP280_WriteRegister(uint8_t channel,uint8_t reg, uint8_t *data, uint16_t length);
static bool BMP280_ReadChipID(uint8_t channel, uint8_t *chip_id);
static bool BMP280_ReadCalibration(uint8_t channel);


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
    if (!TCA9548A_ActiveChannel(channel))
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
static bool BMP280_ReadChipID(uint8_t channel, uint8_t *chip_id)
{
    if (channel >= TCA9548A_CHANNEL_COUNT || chip_id == NULL)
    {
        return false; // Invalid channel or null pointer
    }
    return BMP280_ReadRegister(channel, BMP280_CHIP_ID_REG, chip_id, 1U);
}

static bool BMP280_ReadCalibration(uint8_t channel)
{
    uint8_t data[24]; // BMP280 calibration data is 24 bytes
    BMP280_Calibration_t *calibration = NULL;
    if (channel >= TCA9548A_CHANNEL_COUNT )
    {
        return false; // Invalid channel or null pointer
    }
    calibration = &bmp280_calibration[channel];
    // Read the calibration data from the BMP280
    if (BMP280_ReadRegister(channel, BMP280_START_ADDR_CALIBRATION, data, sizeof(data)) == false)
    {
        return false;
    }
    // Parse the calibration data
    calibration->dig_T1 = (uint16_t)(data[1] << 8 | data[0]);
    calibration->dig_T2 = (int16_t)(data[3] << 8 | data[2]);
    calibration->dig_T3 = (int16_t)(data[5] << 8 | data[4]);
    calibration->dig_P1 = (uint16_t)(data[7] << 8 | data[6]);
    calibration->dig_P2 = (int16_t)(data[9] << 8 | data[8]);
    calibration->dig_P3 = (int16_t)(data[11] << 8 | data[10]);
    calibration->dig_P4 = (int16_t)(data[13] << 8 | data[12]);
    calibration->dig_P5 = (int16_t)(data[15] << 8 | data[14]);
    calibration->dig_P6 = (int16_t)(data[17] << 8 | data[16]);
    calibration->dig_P7 = (int16_t)(data[19] << 8 | data[18]);
    calibration->dig_P8 = (int16_t)(data[21] << 8 | data[20]);
    calibration->dig_P9 = (int16_t)(data[23] << 8 | data[22]);
    
    return true; // Calibration data read successfully
}
static int32_t BMP280_CompensateTemperature(uint8_t channel,int32_t adc_temp)
{
    int32_t var1;
    int32_t var2;
    int32_t temp;

    BMP280_Calibration_t *calibration;

    calibration = &bmp280_calibration[channel];

    var1 = (adc_temp >> 3) -((int32_t)calibration->dig_T1 << 1);
    var1 = (var1 * (int32_t)calibration->dig_T2) >> 11;

    var2 = (adc_temp >> 4) - (int32_t)calibration->dig_T1;
    var2 = (var2 * var2) >> 12;
    var2 = (var2 * (int32_t)calibration->dig_T3) >> 14;

    bmp280_t_fine[channel] = var1 + var2;

    temp = (bmp280_t_fine[channel] * 5 + 128) >> 8;

    return temp;
}
static uint32_t BMP280_CompensatePressure(
    uint8_t channel,
    int32_t adc_press)
{
    int64_t var1;
    int64_t var2;
    int64_t pressure;

    BMP280_Calibration_t *calibration;

    calibration = &bmp280_calibration[channel];

    var1 =
        ((int64_t)bmp280_t_fine[channel]) -
        128000;

    var2 =
        var1 *
        var1 *
        (int64_t)calibration->dig_P6;

    var2 =
        var2 +
        ((var1 *
        (int64_t)calibration->dig_P5) << 17);

    var2 =
        var2 +
        ((int64_t)calibration->dig_P4 << 35);

    var1 =
        ((var1 *
        var1 *
        (int64_t)calibration->dig_P3) >> 8) +
        ((var1 *
        (int64_t)calibration->dig_P2) << 12);

    var1 =
        (((((int64_t)1 << 47) + var1) *
        (int64_t)calibration->dig_P1) >> 33);

    if (var1 == 0)
    {
        return 0U;
    }

    pressure =
        1048576 - adc_press;

    pressure =
        (((pressure << 31) - var2) *
        3125) /
        var1;

    var1 =
        (((int64_t)calibration->dig_P9 *
        (pressure >> 13) *
        (pressure >> 13)) >> 25);

    var2 =
        (((int64_t)calibration->dig_P8 *
        pressure) >> 19);

    pressure =
        ((pressure + var1 + var2) >> 8) +
        ((int64_t)calibration->dig_P7 << 4);

    return (uint32_t)(pressure >> 8);//Convert to Pa
}

bool BMP280_Init(uint8_t channel)
{
    uint8_t chip_id = 0U;

    if (channel >= TCA9548A_CHANNEL_COUNT)
    {
        return false; // Invalid channel
    }

    // Read the chip ID
    if (!BMP280_ReadChipID(channel, &chip_id))
    {
        return false; // Failed to read chip ID
    }

    // Check if the chip ID matches the expected value
    if (chip_id != BMP280_CHIP_ID)
    {
        return false; // Chip ID does not match
    }
    // Check for the calibration data is read successfully
    if (!BMP280_ReadCalibration(channel))
    {
        return false; // Failed to read calibration data
    }

    return true; // Initialization successful
}
bool BMP280_Configure(uint8_t channel)
{
     uint8_t ctrl_meas ;
     uint8_t filter = BMP280_FILTER_OFF;
    if (channel >= TCA9548A_CHANNEL_COUNT)
    {
        return false; // Invalid channel
    }
    /* Configure the BMP280 sensor Oversampling & Mode*/
    ctrl_meas = ((BMP280_OVERSAMPLING_TEMP_X1 << 5) |
                (BMP280_OVERSAMPLING_PRESSURE_X1 << 2) |
                (BMP280_MODE_SLEEP));
    if(BMP280_WriteRegister(channel, BMP280_CONFIG_REG, &ctrl_meas, 1U) == false)
    {
        return false; // Failed to write configuration
    }

    /* Configure the BMP280 sensor Filter */
    if(BMP280_WriteRegister(channel, BMP280_FILTER_REG, &filter, 1U) == false)
    {
        return false; // Failed to write filter configuration
    }
    return true; // Configuration successful
    
}
void BMP280_InitALL(void)
{
    for (uint8_t channel = 0; channel < BMP280_SENSOR_COUNT; channel++)
    {
        if (!BMP280_Init(channel))
        {
            return ; // Initialization failed for this channel
        }
        if (!BMP280_Configure(channel))
        {
            return ; // Configuration failed for this channel
        }
    }
}
bool BMP280_ReadRawData(uint8_t channel, uint32_t *raw_temperature, uint32_t *raw_pressure)
{
    uint8_t data[BMP280_RAW_DATA_SIZE];

    if (channel >= BMP280_SENSOR_COUNT || raw_temperature == NULL || raw_pressure == NULL)
    {
        return false; // Invalid parameters
    }

    // Burst read of six bytes:
    // 0xF7 pressure MSB
    // 0xF8 pressure LSB
    // 0xF9 pressure XLSB
    // 0xFA temperature MSB
    // 0xFB temperature LSB
    // 0xFC temperature XLSB

    if (!BMP280_ReadRegister(channel, BMP280_MESURE_START_REG, data, BMP280_RAW_DATA_SIZE))
    {
        return false; // Failed to read raw data
    }

    // Parse the raw pressure and temperature values
    *raw_pressure =
        ((uint32_t)data[0] << 12U) |
        ((uint32_t)data[1] << 4U) |
        ((uint32_t)data[2] >> 4U);

    *raw_temperature =
        ((uint32_t)data[3] << 12U) |
        ((uint32_t)data[4] << 4U) |
        ((uint32_t)data[5] >> 4U);

    return true; // Successfully read raw data
}
bool BMP280_StartMeasurement(uint8_t channel)
{
    uint8_t ctrl_meas;

    if (channel >= BMP280_SENSOR_COUNT)
    {
        return false;
    }
    ctrl_meas =
        (uint8_t)(
            (BMP280_OVERSAMPLING_TEMP_X1 << 5U) |
            (BMP280_OVERSAMPLING_PRESSURE_X1 << 2U) |
            BMP280_MODE_FORCED
        );

    /* Writing forced mode starts the measurement. */
    return BMP280_WriteRegister(channel, BMP280_CONFIG_REG, &ctrl_meas, 1U);
}
bool BMP280_IsMeasuring(uint8_t channel)
{
    uint8_t status;

    if (channel >= BMP280_SENSOR_COUNT)
    {
        return false;
    }

    if (!BMP280_ReadRegister(channel, BMP280_STATUS_REG, &status, 1U))
    {
        return false; // Failed to read status register
    }
    /*
     * STATUS bit 3:
     *
     * 1 -> measurement running
     * 0 -> measurement finished
     */
    if((status & 0x08) != 0)
    {
        return true; // Measurement is still running
    }
    else
    {
        return false; // Measurement is finished
    }
}
bool BMP280_Mesure(uint8_t channel, int32_t *temp, uint32_t *press)
{
    uint32_t raw_temperature = 0;
    uint32_t raw_pressure = 0;

    if (channel >= BMP280_SENSOR_COUNT )
    {
        return false; // Invalid parameters
    }

    // Start a forced measurement
    if (!BMP280_StartMeasurement(channel))
    {
        return false; // Failed to start measurement
    }
    while (BMP280_IsMeasuring(channel))
    {
        HAL_Delay(10); // Wait for measurement to complete and restart the check
    }
    if(!BMP280_ReadRawData(channel, &raw_temperature, &raw_pressure))
    {
        return false; // Failed to read raw data
    }
    *temp = BMP280_CompensateTemperature(channel,(int32_t)raw_temperature); //Compensate Temp
    *press = BMP280_CompensatePressure(channel,(int32_t)raw_pressure); // compensate Pressure
    
    return true; // Measurement successful


}