#ifndef CONFIG_H
#define CONFIG_H


/* TCA9548A Address */
#define TCA9548A_ADDRESS 0x70

/* TCA9548A Channel Count */
#define TCA9548A_CHANNEL_COUNT 8 // Total number of channels

/* BMP280 Address */
#define BMP280_ADDRESS 0x76
#define BMP280_CHIP_ID 0x58
#define BMP280_START_ADDR_CALIBRATION           (0x88U)

/* BMP280 Calibration Data Size & Data size */
#define BMP280_CALIB_SIZE                (24U)
#define BMP280_RAW_DATA_SIZE             (6U)


#define BMP280_SENSOR_COUNT 6 // Total number of BMP280 sensors

/* ============================================================
 * OVERSAMPLING
 *
 * ctrl_meas:
 *
 * bit 7:5 = temperature oversampling
 * bit 4:2 = pressure oversampling
 * bit 1:0 = mode
 * ============================================================ */
 /* Oversampling settings for BMP280 */
#define BMP280_OVERSAMPLING_TEMP_X1 0x01
#define BMP280_OVERSAMPLING_PRESSURE_X1 0x01


/* BMP280 Modes*/
#define BMP280_MODE_NORMAL 0x03 // Normal mode
#define BMP280_MODE_FORCED 0x02 // Forced mode
#define BMP280_MODE_SLEEP 0x00 // Sleep mode


/* BMP280 Filter Settings */
#define BMP280_FILTER_OFF 0x00 // Filter off

/* BMP280 Register Addresses */
#define BMP280_CHIP_ID_REG 0xD0 // Register address for the chip ID
#define BMP280_CONFIG_REG 0xF4 // Register address for configuration
#define BMP280_FILTER_REG 0xF5 // Register address for filter settings
#define BMP280_MESURE_START_REG 0xF7 // Register address for pressure MSB




/* I2C Timeout */
#define I2C_TIMEOUT 1000

#endif /* CONFIG_H */