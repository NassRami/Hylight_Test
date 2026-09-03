#include "CAN_DIAG.h"
#include "main.h"
#include <stdbool.h>
#include <stdint.h>


static uint32_t can_id = 0U;

/* @brief  Get the CAN ID based on the state of the GPIO pins
 * @retval The CAN ID as a 32-bit unsigned integer
 */
static uint32_t CAN_GetId(void)
{
    
    if (HAL_GPIO_ReadPin(
            CAN_ID0_GPIO_Port,
            CAN_ID0_Pin) == GPIO_PIN_SET)
    {
        can_id |= (1U << 0U);
    }


    if (HAL_GPIO_ReadPin(
            CAN_ID1_GPIO_Port,
            CAN_ID1_Pin) == GPIO_PIN_SET)
    {
        can_id |= (1U << 1U);
    }


    if (HAL_GPIO_ReadPin(
            CAN_ID3_GPIO_Port,
            CAN_ID3_Pin) == GPIO_PIN_SET)
    {
        can_id |= (1U << 2U);
    }


    return can_id;
}

/* @brief  Initialize the CAN peripheral
 * @retval true if the initialization was successful, false otherwise
 */
bool CAN_Init(void)
{
    uint32_t id = CAN_GetId();
    // Configure the FDCAN peripheral with the obtained ID
    if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
    {
        return false;
    }
    return true;
}

/* @brief  Transmit a CAN message with the given data
 * @param  delta_p1: First pressure delta value
 * @param  delta_p2: Second pressure delta value
 * @param  delta_p3: Third pressure delta value
 * @param  fan_rpm: Fan speed in RPM
 * @retval true if the message was successfully transmitted, false otherwise
 */
bool CAN_Transmit(int16_t delta_p1, int16_t delta_p2, int16_t delta_p3, uint16_t fan_rpm)
{
    FDCAN_TxHeaderTypeDef tx_header;
    uint8_t data[8];

    tx_header.Identifier = CAN_GetId();
    tx_header.IdType = FDCAN_STANDARD_ID;
    tx_header.TxFrameType = FDCAN_DATA_FRAME;
    tx_header.DataLength = FDCAN_DLC_BYTES_8;
    tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    tx_header.BitRateSwitch = FDCAN_BRS_OFF;
    tx_header.FDFormat = FDCAN_CLASSIC_CAN;
    tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;

    data[0] = (uint8_t)(delta_p1 >> 8);
    data[1] = (uint8_t)(delta_p1 & 0xFF);
    data[2] = (uint8_t)(delta_p2 >> 8);
    data[3] = (uint8_t)(delta_p2 & 0xFF);
    data[4] = (uint8_t)(delta_p3 >> 8);
    data[5] = (uint8_t)(delta_p3 & 0xFF);
    data[6] = (uint8_t)(fan_rpm >> 8);
    data[7] = (uint8_t)(fan_rpm & 0xFF);

    if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &tx_header, data) != HAL_OK)
    {
        return false; // Transmission failed
    }

    return true; // Transmission successful
}