#ifndef __BOOT_PICTURE_
#define __BOOT_PICTURE_

#include <stm32f10x.h>


extern const unsigned char gImage_64[8200] ;

void Gui_Draw_Boot_Picture(u16 x,u16 y,const unsigned char *p);


#endif //__BOOT_PICTURE_
