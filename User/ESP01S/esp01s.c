#include "esp01s.h"
#include "bsp_usart.h"
#include "string.h"
#include "temperature.h"



void Http_Send_Msg(float temp)
{
	char str1[250]="POST /devices/548130051/datapoints HTTP/1.1\n",
			 str2[100]="{\"datastreams\":[{\"id\":\"TEMPERATURE\",\"datapoints\":[{\"value\":";
	
	strcat(str1,"api-key: 0gqdymAG5YQ76y7CUW=FxOrlO=w=\n");
	strcat(str1,"Host:api.heclouds.com\n");
	strcat(str1,"Content-Length:68\n\n");
	strcat(str1,str2);
	memset(str2, 0, sizeof(u8)*100);
	sprintf(str2, "%2.1f", temp);
	//Usart_SendString(USART2, str2);
	strcat(str1,str2);
	strcat(str1,"}]}]}\r\n\r\n");
	
	Usart_SendString(USART2, str1);
	return;
	
}	



