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
#include "boot_picture.h"
#include "esp01s.h"
#include "max30102.h" 
#include "myiic.h"
#include "algorithm.h"
#include <gui.h>
#include <stdio.h>

uint16_t adcdata;  //set a int for adc value
float ADC_result;
u8 	delay_temp_flag=0, delay_pulse_flag=0, blood_oxyge_flag=0;
/*按键变量*/
u8  up_down_counter=0, sure_key=0, back_key=0, Change_Item_Colour=0;
// ADC1转换的电压值通过MDA方式传到SRAM
extern __IO uint16_t ADC_ConvertedValue[Sample_Num][macNOFCHANEL];

// u16 x_pix_base = 20;
//float ADC_ConvertedValueLocal[Sample_Num]; 


/*--------------------------------------------------------------------*/
extern uint32_t aun_ir_buffer[500]; //IR LED sensor data
extern int32_t n_ir_buffer_length;    //data length
extern uint32_t aun_red_buffer[500];    //Red LED sensor data
extern int32_t n_sp02; //SPO2 value
extern int8_t ch_spo2_valid;   //indicator to show if the SP02 calculation is valid
extern  int32_t n_heart_rate;   //heart rate value
extern  int8_t  ch_hr_valid;    //indicator to show if the heart rate calculation is valid
extern  uint8_t uch_dummy;

#define MAX_BRIGHTNESS 255
/*-------------------------------------------------------------------*/

int main(void)
{
//	u16 x_pix_base = 20;
	u16 temp_adc_num=0, heart_num=0,temp_show_led=0;
//	u8 temp_array[20];
//	u16 raise_2, raise_1;
//	// 局部变量，用于保存转换计算后的电压值 	 
//	float ADC_ConvertedValueLocal[Sample_Num]; 
	
	/*-----------------MAX30102变量---------------------*/
	//variables to calculate the on-board LED brightness that reflects the heartbeats
	uint32_t un_min, un_max, un_prev_data;  
	int i;
	int32_t n_brightness;
	float f_temp;
	u8 temp_num=0;
	u8 temp[6];
	u8 str[100];
	u8 dis_hr=0,dis_spo2=0;
	
	/*---------------------------------------------------*/
	
	
	SysTick_Init();	
	USART1_Config();   //begin serial port 1  used for debug
	USART2_Config();	 //begin serial port 2  used for esp-01s
	ADCx_Init();
	EXTI_Pxy_Config();

	delay_init(72);
	LCD_Init();
	//TP_Init();
	BASIC_TIM_Init();	
	max30102_init();	//调用模拟IIC
	
	LCD_Fill(0, 0, lcddev.width, lcddev.height, WHITE);
	Gui_Draw_Boot_Picture(120,70,gImage_64);
	delay_ms(5000);
	LCD_Fill(0, 0, lcddev.width, lcddev.height, WHITE);
	DrawPage_One();
	
	
			un_min=0x3FFFF;
			un_max=0;	
			n_ir_buffer_length=500; //缓冲区长度为100，存储5秒样本以100sps运行

	//读取前500个样本，并确定信号范围
   for(i=0;i<n_ir_buffer_length;i++)
			{
        while(MAX30102_INT==1)	printf("died...");   //wait until the interrupt pin asserts
        
				max30102_FIFO_ReadBytes(REG_FIFO_DATA,temp);
				aun_red_buffer[i] =  (long)((long)((long)temp[0]&0x03)<<16) | (long)temp[1]<<8 | (long)temp[2];    // Combine values to get the actual number
				aun_ir_buffer[i] = (long)((long)((long)temp[3] & 0x03)<<16) |(long)temp[4]<<8 | (long)temp[5];   // Combine values to get the actual number
            
        if(un_min>aun_red_buffer[i])
            un_min=aun_red_buffer[i];    //update signal min
        if(un_max<aun_red_buffer[i])
            un_max=aun_red_buffer[i];    //update signal max
			}
		un_prev_data=aun_red_buffer[i];
	//在前500个样本（前5秒样本）后计算心率和SpO2
    maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid); 
	
	while(1)
	{

		
		if(sure_key==1)
		{
			switch(up_down_counter % 3)
			{
				case 0 : function_one();	break;
				
				case 1 : function_two();	break;
				
				case 2 : function_three();	break;
			
			}		
			up_down_counter=0;
			sure_key=0;
			back_key=0;
			DrawPage_One();
		}
		
		if(Change_Item_Colour !=  0 )
			{
				Change_Item_Colour = 0;
				switch(up_down_counter % 3)
				{
					case 0 :	DrawItem(0, 0, lcddev.width, 59, YELLOW, WHITE, "血氧 体所");
										DrawItem(0, 60, lcddev.width, 119, BLUE, WHITE, "脉搏波");
										DrawItem(0, 120, lcddev.width, 179, BLUE, WHITE, "设备状态");
										LCD_Fill(0,180,lcddev.width,lcddev.height,WHITE);		break;
				
					case 1 : 	DrawItem(0, 0, lcddev.width, 59, BLUE, WHITE, "血氧 体所");
										DrawItem(0, 60, lcddev.width, 119, YELLOW, WHITE, "脉搏波");
										DrawItem(0, 120, lcddev.width, 179, BLUE, WHITE, "设备状态");
										LCD_Fill(0,180,lcddev.width,lcddev.height,WHITE);		break;
				
					case 2 : 	DrawItem(0, 0, lcddev.width, 59, BLUE, WHITE, "血氧 体所");
										DrawItem(0, 60, lcddev.width, 119, BLUE, WHITE, "脉搏波");
										DrawItem(0, 120, lcddev.width, 179, YELLOW, WHITE, "设备状态");
										LCD_Fill(0,180,lcddev.width,lcddev.height,WHITE);	break;
				}
			}

		if(delay_temp_flag)
		{
			delay_temp_flag=0;
			temp_adc_num = ReadADCAverageValue(1);
			
			Http_Send_Msg( calcuTem(temp_adc_num) );
			
		}

	}


}


//void ADC1_2_IRQHandler(void)
//{
//	if(ADC_GetITStatus(ADC1,ADC_IT_EOC) != RESET )
//	{
//		ADC_ClearITPendingBit(ADC1,ADC_IT_EOC);
//		adcdata = ADC_GetConversionValue(ADC1);
//		ADC_result = (float)(adcdata/4096.000000)*3.3;
//		//delay_pulse_flag=1;
//		//printf("the adc conversion is %f mv	", ADC_result);
//	}

//}

void TIM2_IRQHandler(void)
{
	static int time = 100;
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
		time++;
	}
	if(time % 10 == 0)  //every 10 ms we open adc to get data
	{
		ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	}
	if(time==1200)
	{
		delay_temp_flag=1;
	}
	if(time == 2100)
	{
		time = 100;
		blood_oxyge_flag=1;
		printf("time update \r\n");
	}
}

