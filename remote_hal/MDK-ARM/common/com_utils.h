#ifndef __COM_UTILS_H
#define __COM_UTILS_H


#define COM_LIMIT_RANGE(x, min, max) ((x) < min ? min : ((x) > max ? max : (x)))
#define COM_LIMIT_JOYSTICK(x) COM_LIMIT_RANGE(x, 0, 1000)


#endif /* __COM_UTILS_H */
