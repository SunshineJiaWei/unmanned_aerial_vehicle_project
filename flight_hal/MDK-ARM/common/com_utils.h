#ifndef __COM_UTILS_H
#define __COM_UTILS_H


#define COM_LIMIT_RANGE(x, min, max) ((x) < min ? min : ((x) > max ? max : (x)))
#define COM_LIMIT_THROTTLE(x) COM_LIMIT_RANGE(x, 0, 600)


#endif /* __COM_UTILS_H */
