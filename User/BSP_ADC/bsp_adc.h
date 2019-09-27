#ifndef  _BSP_ADC_H
#define  _BSP_ADC_H

#include <stm32f10x.h>

#define    macADC_APBxClock_FUN             RCC_APB2PeriphClockCmd
#define    macADC_CLK                       RCC_APB2Periph_ADC1

#define    macADC_GPIO_APBxClock_FUN        RCC_APB2PeriphClockCmd
#define    macADC_GPIO_CLK                  RCC_APB2Periph_GPIOA 
#define    macADC_PORT                      GPIOA

#define    macNOFCHANEL											2		// ת��ͨ������
#define 	 Sample_Num  											160 	//����������ƽ���˲�������10��ȡ��ֵ

#define    macADC_PIN1                      GPIO_Pin_0
#define    macADC_CHANNEL1                  ADC_Channel_0

#define    macADC_PIN2                      GPIO_Pin_1
#define    macADC_CHANNEL2                  ADC_Channel_1


// ADC1 ��Ӧ DMA1ͨ��1��ADC3��ӦDMA2ͨ��5��ADC2û��DMA����
#define    macADCx                          ADC1
#define    macADC_DMA_CHANNEL               DMA1_Channel1



void ADCx_Init(void);
//void adc_init(void);
//void ADC_NVIC_Config(void);
//void ADC_Reset(ADC_TypeDef* ADCx);

#endif

