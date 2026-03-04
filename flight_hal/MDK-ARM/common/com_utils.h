#ifndef __COM_UTILS_H
#define __COM_UTILS_H


#include "main.h"


#define COM_LIMIT_RANGE(x, min, max) ((x) < min ? min : ((x) > max ? max : (x)))
#define COM_LIMIT_THROTTLE(x) COM_LIMIT_RANGE(x, 0, 600)


uint16_t com_limit_speed_change(uint16_t cur_speed, uint16_t *last_speed, uint16_t max_change);

#endif /* __COM_UTILS_H */
