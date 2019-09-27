#include "bsp_adc.h"

__IO uint16_t ADC_ConvertedValue[Sample_Num][macNOFCHANEL]={0};

static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// �� ADC IO�˿�ʱ��
	macADC_GPIO_APBxClock_FUN ( macADC_GPIO_CLK, ENABLE );
	
	// ���� ADC IO ����ģʽ
	// ADC 3��ͨ��
	GPIO_InitStructure.GPIO_Pin = macADC_PIN1;
	GPIO_InitStructure.GPIO_Pin = macADC_PIN2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	
	// ��ʼ�� ADC IO
	GPIO_Init(macADC_PORT, &GPIO_InitStructure);				
}

/**
  * @brief  ����ADC����ģʽ
  * @param  ��
  * @retval ��
  */
static void ADCx_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	// ��DMAʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	// ��ADCʱ��
	macADC_APBxClock_FUN ( macADC_CLK, ENABLE );
	
	// ��λDMA������
	DMA_DeInit(macADC_DMA_CHANNEL);
	
	// ���� DMA ��ʼ���ṹ��
	// �����ַΪ��ADC ���ݼĴ�����ַ
	DMA_InitStructure.DMA_PeripheralBaseAddr = ( u32 ) ( & ( macADCx->DR ) );
	
	// �洢����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ADC_ConvertedValue;
	
	// ����Դ��������
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	
	// ��������С��Ӧ�õ�������Ŀ�ĵصĴ�С
	DMA_InitStructure.DMA_BufferSize = macNOFCHANEL*Sample_Num;
	
	// ����Ĵ���ֻ��һ������ַ���õ���
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

	// �洢����ַ����
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
	
	// �������ݴ�СΪ���֣��������ֽ�
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	
	// �ڴ����ݴ�СҲΪ���֣����������ݴ�С��ͬ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	
	// ѭ������ģʽ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	

	// DMA ����ͨ�����ȼ�Ϊ�ߣ���ʹ��һ��DMAͨ��ʱ�����ȼ����ò�Ӱ��
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	
	// ��ֹ�洢�����洢��ģʽ����Ϊ�Ǵ����赽�洢��
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	// ��ʼ��DMA
	DMA_Init(macADC_DMA_CHANNEL, &DMA_InitStructure);
	
	DMA_ClearFlag(DMA1_FLAG_TC1);
	

	// ʹ�� DMA ͨ��
	DMA_Cmd(macADC_DMA_CHANNEL , ENABLE);
	
	// ADC ģʽ����
	// ֻʹ��һ��ADC�����ڵ�ģʽ
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	
	// ɨ��ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE ; 

	// ����ת��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;

	// �����ⲿ����ת���������������
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;

	// ת������Ҷ���
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	
	// ת��ͨ������
	ADC_InitStructure.ADC_NbrOfChannel = macNOFCHANEL;	
		
	// ��ʼ��ADC
	ADC_Init(macADCx, &ADC_InitStructure);
	
	// ����ADCʱ�ӣΪPCLK2��8��Ƶ����9MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
	
	// ����ADC ͨ����ת��˳��Ͳ���ʱ��
	ADC_RegularChannelConfig(macADCx, macADC_CHANNEL1, 1, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(macADCx, macADC_CHANNEL2, 2, ADC_SampleTime_7Cycles5);
	
	// ʹ��ADC DMA ����
	ADC_DMACmd(macADCx, ENABLE);
	
	// ����ADC ������ʼת��
	ADC_Cmd(macADCx, ENABLE);
	
	// ��ʼ��ADC У׼�Ĵ���  
	ADC_ResetCalibration(macADCx);
	// �ȴ�У׼�Ĵ�����ʼ�����
	while(ADC_GetResetCalibrationStatus(macADCx));
	
	// ADC��ʼУ׼
	ADC_StartCalibration(macADCx);
	// �ȴ�У׼���
	while(ADC_GetCalibrationStatus(macADCx));
	
	// ����û�в����ⲿ����������ʹ���������ADCת�� 
	ADC_SoftwareStartConvCmd(macADCx, ENABLE);
}




//void NVIC_Configuration(void)
//{
//  NVIC_InitTypeDef NVIC_InitStructure;     /* Configure one bit for preemption priority */
//  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);     
//  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;     
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;          
//  NVIC_Init(&NVIC_InitStructure);
//}



/**
  * @brief  ADC��ʼ��
  * @param  ��
  * @retval ��
  */
void ADCx_Init(void)
{
	ADCx_GPIO_Config();
	ADCx_Mode_Config();
	//NVIC_Configuration();
}



/*---------------------------------------------------------*/
//void ADC_Reset(ADC_TypeDef* ADCx)
//{
//		
//	ADC_ResetCalibration(ADCx);                                                           
//	//��λУ׼�Ĵ���             �Ĵ�����1
//	
//	while(ADC_GetResetCalibrationStatus(ADCx));                                          
//	//�ȴ�У׼�Ĵ�����λ���      �Ĵ�����0
// 
//  ADC_StartCalibration(ADCx);                                                           
//	//ADCУ׼                    �Ĵ�����1
//	
//  while(ADC_GetCalibrationStatus(ADCx));                                                
//	//�ȴ�У׼���                �Ĵ�����0
//	
//}

//void ADC_NVIC_Config(void)
//{
//	NVIC_InitTypeDef NVIC_InitStructure;
//	
//		  /* Ƕ�������жϿ�������ѡ�� */
//  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//  
//  /* ����USARTΪ�ж�Դ */
//  NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
//  /* �������ȼ�*/
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  /* �����ȼ� */
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//  /* ʹ���ж� */
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  /* ��ʼ������NVIC */
//  NVIC_Init(&NVIC_InitStructure);
//	
//}


//void adc_init(void)
//{
//	
//	ADC_InitTypeDef ADC_InitStructure;                                                    
//	//�ṹ��_ADC����-����   
//	
//	GPIO_InitTypeDef GPIO_InitStructure;                                                  
//	//�ṹ��_���Ż���-����   
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);           
//	//ʱ�ӿ���_GPIOA,ADC1
//	
//	
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	
//	
//	//�ṹ��_����-PA1
//	
//	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                     
//	//�ṹ��_����-����Ƶ��_50Mhz
//	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;                                         
//	//�ṹ��_����-����ģʽ_ģ������
//	
//	GPIO_Init(GPIOA, &GPIO_InitStructure);                                                
//	//�ṹ��_����_��������
//	
//	
//	
//	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                                    
//	//�ṹ��_ADC-��ģʽ_����ģʽ
// 
//	ADC_InitStructure.ADC_ScanConvMode = ENABLE;                                         
//	//�ṹ��_ADC-�Ƿ�ɨ��_��ͨ��
//	
//	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                                   
//	//�ṹ��_ADC-�Ƿ�����_����
//	
//	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;                   
//	//�ṹ��_ADC-������ʽ_�������
//	
//	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                                
//	//�ṹ��_ADC-���뷽ʽ_�Ҷ���
//	
//	ADC_InitStructure.ADC_NbrOfChannel = 2;                                               
//	//�ṹ��_ADC-ͨ������_��ͨ��
//	
//	ADC_Init(ADC1, &ADC_InitStructure);                                                   
//	//�ṹ��_ADC_��������
//	
//	
//	
//	RCC_ADCCLKConfig(RCC_PCLK2_Div8);                                                 
//	//ADC1-ʱ�ӷ�Ƶ����       PS:ԭʼ�����ADC����ʱ����1/2��ϵͳʱ��
//	
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_0 , 1, ADC_SampleTime_1Cycles5);            
//	//�������ݺ�ADC����ľ���ԽӺ���   (ADC�˿� ,ADCͨ�� ,ת�����-�ڼ���ת�� ,ת��������)     
// 	ADC_RegularChannelConfig(ADC1, ADC_Channel_1 , 2, ADC_SampleTime_239Cycles5);            

//	ADC_Cmd(ADC1,ENABLE);  	
//	//����_ADC-�ܿ���   
//	

//	
//	//ADC_ClearITPendingBit(ADC1,ADC_IT_EOC);
//	//clear eoc it flag
//	
//	ADC_ITConfig(ADC1,ADC_IT_EOC,ENABLE);
//	//open the ADC_IT_EOC interrupt
//	
//		ADC_NVIC_Config();
//	
//	
// 	ADC_Reset(ADC1);                                                                      
//	//ADCУ׼����                                               
//	                                                                                                                 
// 
//}
