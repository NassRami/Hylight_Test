#include "UART_Control.h"
#include "main.h"
#include "stdint.h"
#include <stdbool.h>
static bool uart_frame_state = false; // Flag to indicate if a complete UART frame has been received
static uint8_t uart_rx_buffer[8u]; // Buffer to store received UART data
static uint8_t uart_buffer_index; // Index to keep track of the position in the buffer

void UART_Init(void)
{

    uart_buffer_index = 0U; // Reset the buffer index to 0 at Init
    uart_frame_state = false; // Reset the frame state to false at Init

    /*
     * Start reception of one byte.
     */
    (void)HAL_UART_Receive_IT(
        &huart3,
        &uart_rx_byte,
        1U
    );
}
void HAL_UART_RxCpltCallback(
    UART_HandleTypeDef *huart)
{
        if (uart_rx_byte == 0x00U) // Check for end of frame (0x00)
        {
                uart_frame_state = true; /*Don't Copy the 0x00 to the buffer,
                                          just set the frame state to true */
        }
        else
        {
            // Store the received byte in the buffer 
            if (uart_buffer_index < 8U)
            {
                uart_rx_buffer[uart_buffer_index] = uart_rx_byte;
                uart_buffer_index++;
            }
            else
            {
                uart_buffer_index = 0U; // Buffer is Full, reset index to 0
            }
        }

        (void)HAL_UART_Receive_IT( // Restart reception of one byte
            &huart3,
            &uart_rx_byte,
            1U
        );

}
