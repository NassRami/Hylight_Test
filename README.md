<<<<<<< HEAD
# HyLight Technical Test ( Case 1 )
=======
# HyLight Technical Test

>>>>>>> 9f7651e (Update README.md)
## Status Flags

Two status flags are inserted into the **DIAGNOSTIC PACKET** :  
**bit 0** = Sensor fault // Detected during Meausrement.<br>
**bit 1** = Fan fault

## UART CRC

It is required a **CRC16**calcul for the UART frame but the document doesn't specify the **CRC16** variant<br>
The following CRC param are assumed:<br>
- **Polynomial**: 0x1021<br>
- **Initial value**: 0xFFFF<br>

