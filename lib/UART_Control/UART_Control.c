#include "UART_Control.h"
#include "main.h"
#include "stdint.h"
#include "Fan_APP.h"
#include <stdbool.h>
static bool uart_frame_state = false; // Flag to indicate if a complete UART frame has been received
static uint8_t uart_rx_buffer[8u]; // Buffer to store received UART data
static uint8_t uart_buffer_index; // Index to keep track of the position in the buffer
static uint8_t rx_byte = 0U;
static uint8_t UART_Control_COBSEncode(
    const uint8_t *input,
    uint8_t input_length,
    uint8_t *output,
    uint8_t output_size)
{
    uint8_t read_index = 0U;
    uint8_t write_index = 1U;

    uint8_t code_index = 0U;
    uint8_t code = 1U;


    while (read_index < input_length)
    {
        if (input[read_index] == 0U)
        {
            output[code_index] = code;

            code = 1U;

            code_index = write_index;

            write_index++;

            if (write_index > output_size)
            {
                return 0U;
            }
        }
        else
        {
            if (write_index >= output_size)
            {
                return 0U;
            }

            output[write_index] =
                input[read_index];

            write_index++;

            code++;


            if (code == 0xFFU)
            {
                output[code_index] = code;

                code = 1U;

                code_index = write_index;

                write_index++;

                if (write_index > output_size)
                {
                    return 0U;
                }
            }
        }


        read_index++;
    }


    output[code_index] = code;


    return write_index;
}
static uint16_t UART_Control_CRC16(const uint8_t *data, uint8_t length)
{
    uint16_t crc = 0xFFFFU;       // Initial value for CRC16
    uint8_t i;
    uint8_t bit;

    for (i = 0U; i < length; i++)
    {
        crc ^= ((uint16_t)data[i] << 8U);

        for (bit = 0U; bit < 8U; bit++)
        {
            if ((crc & 0x8000U) != 0U)
            {
                crc = (uint16_t)((crc << 1U) ^ 0x1021U); //polynomial 0x1021
            }
            else
            {
                crc = (uint16_t)(crc << 1U);
            }
        }
    }

    return crc;                  
}
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
    UART_HandleTypeDef *huart3)
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
            huart3,
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
    uint8_t command;
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



    command = decoded_buffer [0];

    switch (command)
    {
        case 0x01u:
            Fan_APP_SetMode(FAN_APP_MODE_FORCE_ON);
            break;

        case 0x02u:
            Fan_APP_SetMode(FAN_APP_MODE_FORCE_OFF);
            break;   
 
        case 0x03u:
            Fan_APP_SetMode(FAN_APP_MODE_AUTO);
            break;
    }
         


}

/***************************************************************************************
                                UART Transmission
****************************************************************************************/

void UART_Control_TransmitDiagnostic(
    int16_t delta_p1,
    int16_t delta_p2,
    int16_t delta_p3,
    uint16_t fan_rpm,
    uint8_t flags)
{
    uint8_t data[11U];
    uint8_t encoded[13U]; // 11 for (DATA + CRC ), 1 for CODECOBS , 1 for delimiter
    uint8_t length_encode;
    uint16_t crc ;
    data[0] = (uint8_t)(((uint16_t)delta_p1 >> 8U) & 0xFFU);
    data[1] = (uint8_t)((uint16_t)delta_p1 & 0xFFU);

    data[2] = (uint8_t)(((uint16_t)delta_p2 >> 8U) & 0xFFU);
    data[3] = (uint8_t)((uint16_t)delta_p2 & 0xFFU);

    data[4] = (uint8_t)(((uint16_t)delta_p3 >> 8U) & 0xFFU);
    data[5] = (uint8_t)((uint16_t)delta_p3 & 0xFFU);

    data[6] = (uint8_t)((fan_rpm >> 8U) & 0xFFU);
    data[7] = (uint8_t)(fan_rpm & 0xFFU);

    data[8] = flags;

    crc = UART_Control_CRC16(data, 9U);// from byte 0 to byte 8 (9 bytes) 
    




    data[9] = (uint8_t)((crc >> 8U) & 0xFFU);// MSB
    data[10] =(uint8_t)(crc & 0xFFU);// LSB

     length_encode = UART_Control_COBSEncode(
            data,
            11U,
            encoded,
            12U // Data + COBS
        );
    if (length_encode == 0U)
    {
        return;//Error 
    }

    encoded[length_encode] = 0x00U;

    length_encode++;


    (void)HAL_UART_Transmit(
        &huart3,
        encoded,
        length_encode,
        100U
    );
}

