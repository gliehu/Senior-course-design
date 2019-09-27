#include <stm32f10x.h>   
#include <bsp_usart.h>
#include <bsp_SysTick.h>
#include <stm32f10x_it.h>
#include <bsp_adc.h>
#include <bsp_TiMbase.h>
#include <lcd.h>
#include "iic.h"
#include "touch.h"
#include "delay.h"
#include "test.h"
#include "temperature.h"
#include "ui.h"
#include "key.h"
#include "esp01s.h"
#include "max30102.h" 
#include "myiic.h"
#include "algorithm.h"

#include <gui.h>
#include <stdio.h>

extern __IO uint16_t ADC_ConvertedValue[Sample_Num][macNOFCHANEL];
extern  u8 	delay_temp_flag, delay_pulse_flag, blood_oxyge_flag;
extern 	u8  up_down_counter, sure_key, back_key;


/*--------------------------------------------------------------------*/
uint32_t aun_ir_buffer[500]; //IR LED sensor data
int32_t n_ir_buffer_length;    //data length
uint32_t aun_red_buffer[500];    //Red LED sensor data
int32_t n_sp02; //SPO2 value
int8_t ch_spo2_valid;   //indicator to show if the SP02 calculation is valid
 int32_t n_heart_rate;   //heart rate value
 int8_t  ch_hr_valid;    //indicator to show if the heart rate calculation is valid
 uint8_t uch_dummy;

#define MAX_BRIGHTNESS 255
/*-------------------------------------------------------------------*/


static void Secondary_Pages_Init(void)
{
	up_down_counter=0;
	sure_key=0;
	back_key=0;
	LCD_Fill(0, 0, lcddev.width, lcddev.height, WHITE);
}


void DrawPage_One(void)
{
		LCD_Fill(0, 0, lcddev.width, lcddev.height, WHITE);
		DrawItem(0, 0, lcddev.width, 59, YELLOW, WHITE, "血氧和体所");
		DrawItem(0, 60, lcddev.width, 119, BLUE, WHITE, "脉搏波");
		DrawItem(0, 120, lcddev.width, 179, BLUE, WHITE, "设备状态");
		LCD_Fill(0,180,lcddev.width,lcddev.height,WHITE);	
		//while(1);
}


void DrawItem(u16 x1_rec, u16 y1_rec, u16 x2_rec, u16 y2_rec, u16 bk_color, u16 str_color, u8 *str)
{
//绘制固定栏up
LCD_Fill(x1_rec, y1_rec, x2_rec, y2_rec, bk_color);

POINT_COLOR=str_color;
Gui_StrCenter(0,y1_rec+20,str_color,bk_color,str,16,1);//居中显示

}


void function_one(void)
{
	u8 temp_array[20];
	u16 temp_adc_num=0;
	
	/*-----------------MAX30102变量---------------------*/
	//variables to calculate the on-board LED brightness that reflects the heartbeats
	uint32_t un_min, un_max, un_prev_data;  
	int i;
	int32_t n_brightness;
	float f_temp;
//	u8 temp_num=0;
	u8 temp[6];
	//u8 str[100];
	//u8 dis_hr=0,dis_spo2=0;
	
	/*---------------------------------------------------*/
	
	Secondary_Pages_Init();
	
	while(!back_key)
	{
		//取采样均值求出温度
		if(delay_temp_flag)
		{
			delay_temp_flag=0;
			DrawItem(0, 0, lcddev.width, 60, BLUE, WHITE, "体所 :");
			temp_adc_num = ReadADCAverageValue(1);
			sprintf(temp_array, "%2.1f",calcuTem(temp_adc_num));	
			Http_Send_Msg( calcuTem(temp_adc_num) );
			POINT_COLOR = GREEN;
			LCD_Fill(200,20,320,60,BLUE);
			LCD_ShowString(200,20,16,temp_array,1);				
		}
		
		/*--------------------------------------------------------------*/
		if(blood_oxyge_flag == 1)
		{
			blood_oxyge_flag = 0;
			i=0;
      un_min=0x3FFFF;
      un_max=0;
		
			//dumping the first 100 sets of samples in the memory and shift the last 400 sets of samples to the top
      for(i=100;i<500;i++)
        {
            aun_red_buffer[i-100]=aun_red_buffer[i];
            aun_ir_buffer[i-100]=aun_ir_buffer[i];
            
            //update the signal min and max
            if(un_min>aun_red_buffer[i])
            un_min=aun_red_buffer[i];
            if(un_max<aun_red_buffer[i])
            un_max=aun_red_buffer[i];
        }
				//take 100 sets of samples before calculating the heart rate.
					
				
      for(i=400;i<500;i++)
        {
            un_prev_data=aun_red_buffer[i-1];
            while(MAX30102_INT==1);
            max30102_FIFO_ReadBytes(REG_FIFO_DATA,temp);
						aun_red_buffer[i] =  (long)((long)((long)temp[0]&0x03)<<16) | (long)temp[1]<<8 | (long)temp[2];    // Combine values to get the actual number
						aun_ir_buffer[i] = (long)((long)((long)temp[3] & 0x03)<<16) |(long)temp[4]<<8 | (long)temp[5];   // Combine values to get the actual number
        
            if(aun_red_buffer[i]>un_prev_data)
            {
                f_temp=aun_red_buffer[i]-un_prev_data;
                f_temp/=(un_max-un_min);
                f_temp*=MAX_BRIGHTNESS;
                n_brightness-=(int)f_temp;
                if(n_brightness<0)
                    n_brightness=0;
            }
            else
            {
                f_temp=un_prev_data-aun_red_buffer[i];
                f_temp/=(un_max-un_min);
                f_temp*=MAX_BRIGHTNESS;
                n_brightness+=(int)f_temp;
                if(n_brightness>MAX_BRIGHTNESS)
                    n_brightness=MAX_BRIGHTNESS;
            }
					//send samples and calculation result to terminal program through UART
				}	
		
        maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);

				DrawItem(0, 60, lcddev.width, 119, YELLOW, WHITE, "血氧 :");
				sprintf(temp_array, "%i%%",n_sp02);				
				POINT_COLOR = BLUE;
				LCD_Fill(200,80,159, 80,YELLOW);
				LCD_ShowString(200,80,16,temp_array,1);	
			
		}

	}
	
//	return;
}	

void function_two(void)
{
	u8 temp_array[20];
	int x_pix_base = 0;
	u16 temp_adc_num=0;
	//u16 raise_2, raise_1;
	// 局部变量，用于保存转换计算后的电压值 	 
	float ADC_ConvertedValueLocal[Sample_Num]; 
	
	Secondary_Pages_Init();
	Draw_Coordinate(20, BLACK);
	while(!back_key)
	{
		if( DMA_GetFlagStatus(DMA1_FLAG_TC1) == SET )
		{
			DMA_ClearFlag(DMA1_FLAG_TC1);
			for(u8 t=1; t<Sample_Num; t++)
				{
					ADC_ConvertedValueLocal[t] =(float)( ADC_ConvertedValue[t][0]/4096.00)*200;	
				//	printf("ADC_ConvertedValueLocal = %2.2f \r\n",ADC_ConvertedValueLocal[t]);
					delay_ms(10);	
					//Carry_Out_Judge();
					POINT_COLOR = RED;					
				//	LCD_Fill(x_pix_base+t+1, 100, x_pix_base+t+1, 240, WHITE);
				//	if((x_pix_base+t+1)%20 ==0) {POINT_COLOR = BLACK;LCD_DrawLine(x_pix_base+t+1, 100, x_pix_base+t+1, 240);}
				
					
					Draw_Wave_Line(x_pix_base+t, 280-(u16)ADC_ConvertedValueLocal[t-1], 280-(u16)ADC_ConvertedValueLocal[t]);

				}

				x_pix_base += Sample_Num;
				if(x_pix_base>=320) {x_pix_base =0;	LCD_Fill(x_pix_base,100,lcddev.width,lcddev.height,WHITE);}

				
				Draw_Coordinate(20, BLACK);
		}		
		
		 
		Carry_Out_Judge();

		
	}
	
//	return;

}	

void function_three(void)
{
	u16 temp_adc_num;
	Secondary_Pages_Init();
	
	temp_adc_num = ReadADCAverageValue(1);

		if( 4050 <= temp_adc_num )
			{
				DrawItem(0, 0, lcddev.width, 59, RED, WHITE, "TEMPERATURE CONNECTED ERROR !");
			}
		else DrawItem(0, 0, lcddev.width, 59, BLUE, WHITE, "TEMPERATURE CONNECTED SUCCESS !");
	
		printf("temp_adc_num = %d \r\n", temp_adc_num);		
			
		temp_adc_num = ReadADCAverageValue(0);
		printf("temp_adc_num***** = %d \r\n", temp_adc_num);	
		if( 2100 <= temp_adc_num || temp_adc_num <= 1500)
			{
				DrawItem(0, 60, lcddev.width, 119, RED, WHITE, "PULSE CONNECTED ERROR !");
			}
		else DrawItem(0, 60, lcddev.width, 119, BLUE, WHITE, "PULSE CONNECTED SUCCESS !");	
			
			
	while(!back_key);
	
//	return;
}	


void Carry_Out_Judge(void)
{
	u8 temp_array[20];
//	int x_pix_base = 0;
	u16 temp_adc_num=0;
		/*-----------------MAX30102变量---------------------*/
	//variables to calculate the on-board LED brightness that reflects the heartbeats
	uint32_t un_min, un_max, un_prev_data;  
	int i;
	int32_t n_brightness;
	float f_temp;
//	u8 temp_num=0;
	u8 temp[6];
	//u8 str[100];
//	u8 dis_hr=0,dis_spo2=0;
	
	/*---------------------------------------------------*/
			//取采样均值求出温度
		if(delay_temp_flag)
		{
			delay_temp_flag=0;
			temp_adc_num = ReadADCAverageValue(1);
			sprintf(temp_array, ": %2.1f",calcuTem(temp_adc_num));	
			Http_Send_Msg( calcuTem(temp_adc_num) );
			POINT_COLOR = BLUE;
			LCD_Fill(160,20,320,40,WHITE);
			
			LCD_ShowString(200,20,16,temp_array,1);
			Show_Str(160,20,BLUE,GREEN,"体所",16,1);
		}
		
		
		if(blood_oxyge_flag == 1)
		{
			blood_oxyge_flag = 0;
			i=0;
      un_min=0x3FFFF;
      un_max=0;
		
			//dumping the first 100 sets of samples in the memory and shift the last 400 sets of samples to the top
      for(i=100;i<500;i++)
        {
            aun_red_buffer[i-100]=aun_red_buffer[i];
            aun_ir_buffer[i-100]=aun_ir_buffer[i];
            
            //update the signal min and max
            if(un_min>aun_red_buffer[i])
            un_min=aun_red_buffer[i];
            if(un_max<aun_red_buffer[i])
            un_max=aun_red_buffer[i];
        }
				//take 100 sets of samples before calculating the heart rate.
					
				
      for(i=400;i<500;i++)
        {
            un_prev_data=aun_red_buffer[i-1];
            while(MAX30102_INT==1);
            max30102_FIFO_ReadBytes(REG_FIFO_DATA,temp);
						aun_red_buffer[i] =  (long)((long)((long)temp[0]&0x03)<<16) | (long)temp[1]<<8 | (long)temp[2];    // Combine values to get the actual number
						aun_ir_buffer[i] = (long)((long)((long)temp[3] & 0x03)<<16) |(long)temp[4]<<8 | (long)temp[5];   // Combine values to get the actual number
        
            if(aun_red_buffer[i]>un_prev_data)
            {
                f_temp=aun_red_buffer[i]-un_prev_data;
                f_temp/=(un_max-un_min);
                f_temp*=MAX_BRIGHTNESS;
                n_brightness-=(int)f_temp;
                if(n_brightness<0)
                    n_brightness=0;
            }
            else
            {
                f_temp=un_prev_data-aun_red_buffer[i];
                f_temp/=(un_max-un_min);
                f_temp*=MAX_BRIGHTNESS;
                n_brightness+=(int)f_temp;
                if(n_brightness>MAX_BRIGHTNESS)
                    n_brightness=MAX_BRIGHTNESS;
            }
					//send samples and calculation result to terminal program through UART

				}	
		
        maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);

				sprintf(temp_array, ": %i%% ",n_sp02);				
				POINT_COLOR = BLUE;
				LCD_Fill(20,20,159, 60,WHITE);
				LCD_ShowString(60,20,16,temp_array,1);
				Show_Str(20,20,BLUE,GREEN,"血氧",16,1);
			
		}

}


