#ifndef __APP_DISPLAY_H__
#define __APP_DISPLAY_H__


#include "Inf_OLED.h"
#include "int_si24r1.h"
#include "app_process_data.h"
#include "com_protocol.h"


#define LINE1_BEGIN 28
#define LINE2_BEGIN 25
#define LINE3_BEGIN 5
#define LINE4_BEGIN 5
#define BAR1_BEGING 35
#define BAR2_BEGING 47

#define LINE3_BEGIN2 65
#define BAR1_BEGING2 95
#define BAR2_BEGING2 107

#define LINE4_BEGIN2 65

#define Y0 0
#define Y1 14
#define Y2 26
#define Y3 38


void app_display_init(void);

void app_display_show(void);


#endif /* __APP_DISPLAY_H__ */
