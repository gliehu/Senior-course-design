#ifndef __TEMPERATURE_H
#define __TEMPERATURE_H		

#include <stm32f10x.h>

//const u16 tempRes_buf[];


float calcuTem(u16 ad_value);

u16 ReadADCAverageValue(uint16_t Channel);

#endif
