#include "temperature.h"
#include "bsp_adc.h"

extern uint16_t adcdata;
extern uint16_t ADC_ConvertedValue[Sample_Num][macNOFCHANEL];

const u16 tempRes_buf[181] = {3876, 3863, 3849, 3835, 3819, 3804, 3787, 
3769, 3751, 3732, 3712, 3692, 3670, 3648, 3625, 3601, 3576, 3550, 3523, 3496,   
3467, 3438, 3408, 3377, 3344, 3312, 3278, 3243, 3208, 3171, 3134, 3096, 3057, 
3018, 2978, 2937, 2896, 2854, 2811, 2768,   2725, 2681, 2637, 2592, 2547, 2502, 
2457, 2411, 2366, 2320, 2274, 2229, 2183, 2138, 2092, 2048, 2003, 1958, 1914, 
1871,   1827, 1784, 1742, 1700, 1659, 1618, 1578, 1538, 1499, 1461, 1423, 1386, 
1350, 1314, 1279, 1245, 1211, 1179, 1146, 1115,   1084, 1054, 1025, 996, 968, 941, 
915, 889, 864, 839, 815, 792, 769, 747, 726, 705, 685, 665, 646, 628, 610, 592, 
575,   558, 542, 527, 512, 497, 483, 469, 456, 443, 430, 418, 406, 395, 383, 373, 
362, 352, 342, 333, 323, 314, 306, 297,   289, 281, 273, 266, 259, 252, 245, 238, 
232, 225, 219, 214, 208, 202, 197, 192, 187, 182, 177, 173, 168, 164, 160,   155, 
152, 148, 144, 140, 137, 133, 130, 127, 124, 121, 118, 115, 112, 109, 107, 104, 
102, 99, 97, 94, 92, 90, 88,  86, 84, 82, 80, 78, 76, 75, 73};


/**
  * @brief 通过查表法，根据AD值计算温度
  * @param AD值
  * @Note  温度范围是-30~150度 如果超过范围，返回32767 ℃。10K上拉，3950 10K热敏电阻
  * @retval 温度值
  */  
float calcuTem(u16 adcdata)
{
  u16 tempValue= 0x7fff;
	float temp_fix;
  if ((adcdata < 3877)&&(adcdata > 72))
  {
    for (short i = 0 ; i < 181 ; i++)
    {
      if (adcdata > tempRes_buf[i])
      {
 //       tempValue = i-30;
				//printf("adcdata = %d\r\n",adcdata);
				temp_fix = ((adcdata-tempRes_buf[i])/((tempRes_buf[i-1]-tempRes_buf[i])*1.00)) +i-30;
        break;
      }
    }
  }
  //else err return 0x7fff
	printf("当前温度 = %2.2f\r\n",temp_fix);
  return temp_fix;
}

u16 ReadADCAverageValue(uint16_t Channel)
{
    uint8_t i;
    uint32_t sum = 0;
    for(i=0; i<Sample_Num; i+=10)
    {
     sum+=ADC_ConvertedValue[i][Channel];
    }
    return (sum/Sample_Num)*10;
}


