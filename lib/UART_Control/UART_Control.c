#include "UART_Control.h"
#include "main.h"
#include "stdint.h"
#include <stdbool.h>
static bool uart_frame_state = false; // Flag to indicate if a complete UART frame has been received
static uint8_t uart_rx_buffer[8u]; // Buffer to store received UART data
static uint8_t uart_buffer_index; // Index to keep track of the position in the buffer
static uint8_t rx_byte = 0U;

void UART_Init(void)
{

    uart_buffer_index = 0U; // Reset the buffer index to 0 at Init
    uart_frame_state = false; // Reset the frame state to false at Init

    /*
     * Start reception of one byte.
     */
    (void)HAL_UART_Receive_IT(
        &huart3,
        &rx_byte,
        1U
    );
}
void HAL_UART_RxCpltCallback(
    UART_HandleTypeDef *huart)
{
        if (rx_byte == 0x00U) // Check for end of frame (0x00)
        {
                uart_frame_state = true; /*Don't Copy the 0x00 to the buffer,
                                          just set the frame state to true */
        }
        else
        {
            // Store the received byte in the buffer 
            if (uart_buffer_index < 8U)
            {
                uart_rx_buffer[uart_buffer_index] = rx_byte;
                uart_buffer_index++;
            }
            else
            {
                uart_buffer_index = 0U; // Buffer is Full, reset index to 0
            }
        }

        (void)HAL_UART_Receive_IT( // Restart reception of one byte
            &huart3,
            &rx_byte,
            1U
        );

}
bool UART_COBSDecode(uint8_t *input,//Rx_buff
                     uint8_t *output,//Buff after decoding
                      uint8_t length)//Length of the input buffer
{
    uint8_t write_index = 0U;
    uint8_t read_index = 0U;
    uint8_t COBS_code = 0U;

    while(read_index < length)
    {
        COBS_code = input[read_index];//by pass the COBS code
        read_index++;

        if (COBS_code == 0U)// Invalid COBS code 0x00, cannot decode
        {
            return false;
        }
        //1 byte COBS + 3 bytes of data
        for (uint8_t i = 1U; i < COBS_code; i++)
        {
            output[write_index] = input[read_index];
            write_index++;
            read_index++;
        }

    }
    return true;
}
void UART_Control_Process(void)
{
    uint8_t decoded_buffer[8u]; // Buffer to store the decoded data
    uint16_t received_CRC = 0U; // Variable to store the received CRC value
    if (!uart_frame_state)// UART Frame is not ready
    {
        return;
    }
    uart_frame_state = false; // Reset the frame ready flag

    /* Process the received UART frame  */
    if(!UART_COBSDecode(uart_rx_buffer, decoded_buffer, uart_buffer_index))
    {
        // Handle COBS decoding error
        uart_buffer_index = 0U; // Reset the buffer index
        return;
    }

    received_CRC = ((uint16_t) decoded_buffer[1] << 8)| (uint16_t)decoded_buffer[2] ; // Extract the received CRC from the decoded buffer

    /* Calculate the CRC */

}