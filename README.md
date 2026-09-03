
# HyLight Technical Test ( Case 1 )

## Status Flags

Two status flags are inserted into the **DIAGNOSTIC PACKET** :  
**bit 0** = Sensor fault // Detected during Meausrement.<br>
**bit 1** = Fan fault

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



