#include "com_utils.h"


uint16_t com_limit_speed_change(uint16_t cur_speed, uint16_t *last_speed, uint16_t max_change)
{
    int16_t diff = cur_speed - (*last_speed);

    if (diff > max_change)
    {
        diff = max_change;
    }

    int16_t new_speed = *last_speed + diff;
    if (new_speed < 0)
    {
        new_speed = 0;
    }
    *last_speed = new_speed;

    return *last_speed;
}