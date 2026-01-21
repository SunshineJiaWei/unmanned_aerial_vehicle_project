#ifndef __COM_CONFIG_H__
#define __COM_CONFIG_H__


typedef enum
{
    REMOTE_STATE_CONNECTED,
    REMOTE_STATE_DISCONNECT
} remote_state_t;

typedef enum
{
    FLIGHT_STATE_IDLE,
    FLIGHT_STATE_NORMAL,
    FLIGHT_STATE_FIX_HIGH,
    FLIGHT_STATE_FAULT
} flight_state_t;


#endif /* __COM_CONFIG_H */