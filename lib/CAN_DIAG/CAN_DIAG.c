#include "CAN_DIAG.h"
#include "main.h"
#include <stdbool.h>
#include <stdint.h>

static uint32_t CAN_GetId(void)
{
    uint32_t id = 0U;
    
    if (HAL_GPIO_ReadPin(
            CAN_ID0_GPIO_Port,
            CAN_ID0_Pin) == GPIO_PIN_SET)
    {
        id |= (1U << 0U);
    }


    if (HAL_GPIO_ReadPin(
            CAN_ID1_GPIO_Port,
            CAN_ID1_Pin) == GPIO_PIN_SET)
    {
        id |= (1U << 1U);
    }


    if (HAL_GPIO_ReadPin(
            CAN_ID3_GPIO_Port,
            CAN_ID3_Pin) == GPIO_PIN_SET)
    {
        id |= (1U << 2U);
    }


    return id;
}

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