#ifndef FAN_APP_H
#define FAN_APP_H

typedef enum
{
    FAN_APP_MODE_FORCE_ON = 0x01U,
    FAN_APP_MODE_FORCE_OFF = 0x02U ,
    FAN_APP_MODE_AUTO = 0x03U

} Fan_APP_Mode_t;


void Fan_APP_SetMode(Fan_APP_Mode_t mode);
void Fan_APP_Init(void);
void Fan_APP_Process(void);

#endif //FAN_APP_H