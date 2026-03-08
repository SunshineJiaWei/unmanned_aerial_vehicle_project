#ifndef __COM_PROTOCOL_H
#define __COM_PROTOCOL_H


#include <stdint.h>


typedef struct
{
    char vbat[8];
    uint8_t unlocked;
    uint8_t reserved[10];
} back_data_t;


#endif /* __COM_PROTOCOL_H */
