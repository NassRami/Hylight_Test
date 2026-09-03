
# HyLight Technical Test ( Case 1 )

## App Scheduling

The app is based on a the **`HAL_GetTick()'**, no 'HAL_Delay()' has been used to prevent the blocking and to allow the differents APPs Layers to run independtenly<br>

| Task | Period | Description |
|---|---:|---|
| UART RX  | Continuous | Processes received UART command frames |
| Sensor measurement | 100 ms | Reads the six BMP280 sensors, compensate the temp and press value based on the algo defined in the datasheet and computes ΔP1, ΔP2 and ΔP3 |
| CAN diagnostic TX | 200 ms | Sends the latest pressure differences and fan RPM over CAN |
| Fan monitoring | 500 ms | Checks the fan status and detects fan faults(blocked or hardware problem) |
| UART diagnostic TX | 500 ms | Sends diagnostic data(delta_p1,delta_p2,delta_p3 + status flag) using CRC16, COBS encoding and a `0x00` delimiter |


## Status Flags

Two status flags are inserted into the **DIAGNOSTIC PACKET** :  
| Bit | Flag | Description |
|---|---|---|
| 0 | `SENSOR_FAULT` | BMP280 sensor measurement fault |
| 1 | `FAN_FAULT` | fan monitoring fault |

We call `App_GetStatusFlags()` before every UART transmission to generate the flag.


## UART CRC

It is required a **CRC16**calcul for the UART frame but the document doesn't specify the **CRC16** variant<br>
The following CRC param are assumed:<br>
- **Polynomial**: 0x1021<br>
- **Initial value**: 0xFFFF<br>


## CAN TX
We used the FDCAN peripheral  available in the  **STM32G431** in Classic CAN mode for periodic transmission<br>
The 8-byte diagnostic payload is organized as:

| Bytes | Data | Type |
|---|---|---|
| 0-1 | ΔP1 | int16 |
| 2-3 | ΔP2 | int16 |
| 4-5 | ΔP3 | int16 |
| 6-7 | Fan RPM | uint16 |



