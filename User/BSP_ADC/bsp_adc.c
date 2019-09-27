#include "bsp_adc.h"

__IO uint16_t ADC_ConvertedValue[Sample_Num][macNOFCHANEL]={0};

static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// 打开 ADC IO端口时钟
	macADC_GPIO_APBxClock_FUN ( macADC_GPIO_CLK, ENABLE );
	
	// 配置 ADC IO 引脚模式
	// ADC 3个通道
	GPIO_InitStructure.GPIO_Pin = macADC_PIN1;
	GPIO_InitStructure.GPIO_Pin = macADC_PIN2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	
	// 初始化 ADC IO
	GPIO_Init(macADC_PORT, &GPIO_InitStructure);				
}

/**
  * @brief  配置ADC工作模式
  * @param  无
  * @retval 无
  */
static void ADCx_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	// 打开DMA时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	// 打开ADC时钟
	macADC_APBxClock_FUN ( macADC_CLK, ENABLE );
	
	// 复位DMA控制器
	DMA_DeInit(macADC_DMA_CHANNEL);
	
	// 配置 DMA 初始化结构体
	// 外设基址为：ADC 数据寄存器地址
	DMA_InitStructure.DMA_PeripheralBaseAddr = ( u32 ) ( & ( macADCx->DR ) );
	
	// 存储器地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ADC_ConvertedValue;
	
	// 数据源来自外设
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	
	// 缓冲区大小，应该等于数据目的地的大小
	DMA_InitStructure.DMA_BufferSize = macNOFCHANEL*Sample_Num;
	
	// 外设寄存器只有一个，地址不用递增
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

	// 存储器地址递增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
	
	// 外设数据大小为半字，即两个字节
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	
	// 内存数据大小也为半字，跟外设数据大小相同
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	
	// 循环传输模式
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	

	// DMA 传输通道优先级为高，当使用一个DMA通道时，优先级设置不影响
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	
	// 禁止存储器到存储器模式，因为是从外设到存储器
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	// 初始化DMA
	DMA_Init(macADC_DMA_CHANNEL, &DMA_InitStructure);
	
	DMA_ClearFlag(DMA1_FLAG_TC1);
	

	// 使能 DMA 通道
	DMA_Cmd(macADC_DMA_CHANNEL , ENABLE);
	
	// ADC 模式配置
	// 只使用一个ADC，属于单模式
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	
	// 扫描模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE ; 

	// 连续转换模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;

	// 不用外部触发转换，软件开启即可
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;

	// 转换结果右对齐
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	
	// 转换通道个数
	ADC_InitStructure.ADC_NbrOfChannel = macNOFCHANEL;	
		
	// 初始化ADC
	ADC_Init(macADCx, &ADC_InitStructure);
	
	// 配置ADC时钟Ｎ狿CLK2的8分频，即9MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
	
	// 配置ADC 通道的转换顺序和采样时间
	ADC_RegularChannelConfig(macADCx, macADC_CHANNEL1, 1, ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(macADCx, macADC_CHANNEL2, 2, ADC_SampleTime_7Cycles5);
	
	// 使能ADC DMA 请求
	ADC_DMACmd(macADCx, ENABLE);
	
	// 开启ADC ，并开始转换
	ADC_Cmd(macADCx, ENABLE);
	
	// 初始化ADC 校准寄存器  
	ADC_ResetCalibration(macADCx);
	// 等待校准寄存器初始化完成
	while(ADC_GetResetCalibrationStatus(macADCx));
	
	// ADC开始校准
	ADC_StartCalibration(macADCx);
	// 等待校准完成
	while(ADC_GetCalibrationStatus(macADCx));
	
	// 由于没有采用外部触发，所以使用软件触发ADC转换 
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
  * @brief  ADC初始化
  * @param  无
  * @retval 无
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
//	//复位校准寄存器             寄存器置1
//	
//	while(ADC_GetResetCalibrationStatus(ADCx));                                          
//	//等待校准寄存器复位完成      寄存器置0
// 
//  ADC_StartCalibration(ADCx);                                                           
//	//ADC校准                    寄存器置1
//	
//  while(ADC_GetCalibrationStatus(ADCx));                                                
//	//等待校准完成                寄存器置0
//	
//}

//void ADC_NVIC_Config(void)
//{
//	NVIC_InitTypeDef NVIC_InitStructure;
//	
//		  /* 嵌套向量中断控制器组选择 */
//  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//  
//  /* 配置USART为中断源 */
//  NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
//  /* 抢断优先级*/
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  /* 子优先级 */
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//  /* 使能中断 */
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  /* 初始化配置NVIC */
//  NVIC_Init(&NVIC_InitStructure);
//	
//}


//void adc_init(void)
//{
//	
//	ADC_InitTypeDef ADC_InitStructure;                                                    
//	//结构体_ADC基础-声明   
//	
//	GPIO_InitTypeDef GPIO_InitStructure;                                                  
//	//结构体_引脚基础-声明   
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);           
//	//时钟开启_GPIOA,ADC1
//	
//	
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;	
//	
//	//结构体_引脚-PA1
//	
//	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                     
//	//结构体_引脚-引脚频率_50Mhz
//	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;                                         
//	//结构体_引脚-引脚模式_模拟输入
//	
//	GPIO_Init(GPIOA, &GPIO_InitStructure);                                                
//	//结构体_引脚_结束配置
//	
//	
//	
//	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                                    
//	//结构体_ADC-总模式_独立模式
// 
//	ADC_InitStructure.ADC_ScanConvMode = ENABLE;                                         
//	//结构体_ADC-是否扫描_单通道
//	
//	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                                   
//	//结构体_ADC-是否连续_单次
//	
//	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;                   
//	//结构体_ADC-触发方式_软件触发
//	
//	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                                
//	//结构体_ADC-对齐方式_右对齐
//	
//	ADC_InitStructure.ADC_NbrOfChannel = 2;                                               
//	//结构体_ADC-通道个数_单通道
//	
//	ADC_Init(ADC1, &ADC_InitStructure);                                                   
//	//结构体_ADC_结束配置
//	
//	
//	
//	RCC_ADCCLKConfig(RCC_PCLK2_Div8);                                                 
//	//ADC1-时钟分频配置       PS:原始输入的ADC输入时钟是1/2的系统时钟
//	
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_0 , 1, ADC_SampleTime_1Cycles5);            
//	//配置内容和ADC外设的具体对接函数   (ADC端口 ,ADC通道 ,转换序号-第几个转换 ,转换的周期)     
// 	ADC_RegularChannelConfig(ADC1, ADC_Channel_1 , 2, ADC_SampleTime_239Cycles5);            

//	ADC_Cmd(ADC1,ENABLE);  	
//	//开关_ADC-总开关   
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
//	//ADC校准函数                                               
//	                                                                                                                 
// 
//}
