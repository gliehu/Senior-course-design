#include "iic.h"
#include <bsp_usart.h>
#include <stm32f10x.h> 

void I2C1_Configuration(void)
{
	I2C_InitTypeDef  I2C_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure; 

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    

	/* PB6,7 SCL and SDA */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
    I2C_DeInit(I2C1);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x30;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 100000;//100K速度
    
	I2C_Cmd(I2C1, ENABLE);
	I2C_Init(I2C1, &I2C_InitStructure);
	/*允许1字节1应答模式*/
	I2C_AcknowledgeConfig(I2C1, ENABLE);
}


void I2C1_GPIO_Config(void)
{

	GPIO_InitTypeDef GPIO_InitStructure; //GPIO结构体定义
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能I2C的IO口 

		/* 使能与 I2C1 有关的时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	
	/* PB6-I2C1_SCL、PB7-I2C1_SDA*/
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;

	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; // 开漏输出
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化结构体配置

}

/* I2C 工作模式配置 */

void I2C1_Mode_config(void)
{
		/*定义I2C结构体*/
	I2C_InitTypeDef  I2C_InitStructure;



		
	/*配置为I2C模式*/
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	
	/*该参数只有在I2C 工作在快速模式（时钟工作频率高于 100KHz）下才有意义。*/
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	
	/*设置第一个设备自身地址*/
	I2C_InitStructure.I2C_OwnAddress1 =0x30;
	
	/*使能应答*/
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;
	
	/*AT24C02地址为7位所以设置7位就行了*/
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; /*时钟速率，以HZ为单位的，最高为400khz*/
	
	I2C_InitStructure.I2C_ClockSpeed = 20000;

	/* 使能 I2C1 */
	I2C_Cmd(I2C1, ENABLE);
	
	/* I2C1 初始化 */
	I2C_Init(I2C1, &I2C_InitStructure);
	
}

void I2C_Conf(void)
{
	I2C1_Configuration();

  I2C1_GPIO_Config();

  I2C1_Mode_config();

}




void max10300_Bus_Write(u8 Register_Address, u8 Word_Data)
{
#if 0
	i2c_snd(max10300_W_address, TRUE, FALSE);
	i2c_snd(Register_Address, FALSE, FALSE);
	i2c_snd(Word_Data, FALSE, TRUE);
#else
	while(I2C_GetFlagStatus(USR_I2C_USED, I2C_FLAG_BUSY))
		; //调用库函数检测I2C器件是否处于BUSY状态
	I2C_GenerateSTART(USR_I2C_USED, ENABLE);

	 while(!I2C_CheckEvent(USR_I2C_USED, I2C_EVENT_MASTER_MODE_SELECT))
	 	; //清除EV5
	 	
	I2C_Send7bitAddress(USR_I2C_USED, max10300_WR_address, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(USR_I2C_USED,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		; //ADDR=1，清除EV6  

	I2C_SendData(USR_I2C_USED, Register_Address);
	 while(! I2C_CheckEvent(USR_I2C_USED, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	 	;//移位寄存器非空，数据寄存器已经空，产生EV8，发送数据到DR既可清除该事件
	
		
	I2C_SendData(USR_I2C_USED, Word_Data);
	 while(! I2C_CheckEvent(USR_I2C_USED, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	 	;//移位寄存器非空，数据寄存器已经空，产生EV8，发送数据到DR既可清除该事件
	 I2C_GenerateSTOP(USR_I2C_USED, ENABLE);	
	 
#endif

}


u8 max10300_Bus_Read(u8 Register_Address)
{
#if 0
	uint8 data1;
	i2c_snd(max10300_W_address, TRUE, FALSE);
	i2c_snd(Register_Address, FALSE, FALSE);
	i2c_snd(max10300_R_address, TRUE, FALSE);
	i2c_rcv(&data1, TRUE);
#else
	u8 data1;
	while(I2C_GetFlagStatus(USR_I2C_USED, I2C_FLAG_BUSY))
		; //调用库函数检测I2C器件是否处于BUSY状态
	I2C_GenerateSTART(USR_I2C_USED, ENABLE);
	 while(!I2C_CheckEvent(USR_I2C_USED, I2C_EVENT_MASTER_MODE_SELECT))
	 	; //清除EV5
	 	
	I2C_Send7bitAddress(USR_I2C_USED, max10300_WR_address, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(USR_I2C_USED,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		; //ADDR=1，清除EV6  
		
	I2C_SendData(USR_I2C_USED, Register_Address);
	 while(! I2C_CheckEvent(USR_I2C_USED, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	 	;//移位寄存器非空，数据寄存器已经空，产生EV8，发送数据到DR既可清除该事件

	I2C_GenerateSTART(USR_I2C_USED, ENABLE);
	 while(!I2C_CheckEvent(USR_I2C_USED, I2C_EVENT_MASTER_MODE_SELECT))
	 	; //清除EV5
	 	
	I2C_Send7bitAddress(USR_I2C_USED, max10300_WR_address, I2C_Direction_Receiver);

	while(!I2C_CheckEvent(USR_I2C_USED, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	I2C_AcknowledgeConfig(USR_I2C_USED, DISABLE);	//最后一位后要关闭应答的
    I2C_GenerateSTOP(USR_I2C_USED, ENABLE);			//发送停止位
	
	
		while(!(I2C_CheckEvent(USR_I2C_USED, I2C_EVENT_MASTER_BYTE_RECEIVED)))
		; //读取数据
		data1 = I2C_ReceiveData(USR_I2C_USED);
	

	I2C_AcknowledgeConfig(USR_I2C_USED, ENABLE);//将应答位使能回去，等待下次通信
	
	return (data1);
#endif
}

static void i2c_Delay(void)
{
	uint8_t i;

	/*　
		CPU主频168MHz时，在内部Flash运行, MDK工程不优化。用台式示波器观测波形。
		循环次数为5时，SCL频率 = 1.78MHz (读耗时: 92ms, 读写正常，但是用示波器探头碰上就读写失败。时序接近临界)
		循环次数为10时，SCL频率 = 1.1MHz (读耗时: 138ms, 读速度: 118724B/s)
		循环次数为30时，SCL频率 = 440KHz， SCL高电平时间1.0us，SCL低电平时间1.2us

		上拉电阻选择2.2K欧时，SCL上升沿时间约0.5us，如果选4.7K欧，则上升沿约1us

		实际应用选择400KHz左右的速率即可
	*/
	for (i = 0; i < 30; i++);
}




void max10300_FIFO_Read(u8 Register_Address,u16  Word_Data[][2],u8 count)
{
	u8 i=0;
	u8 no = count;
	u8 data1, data2;
	
	
	while(I2C_GetFlagStatus(USR_I2C_USED, I2C_FLAG_BUSY))
		; //调用库函数检测I2C器件是否处于BUSY状态
		

	I2C_AcknowledgeConfig(USR_I2C_USED, ENABLE);   /*允许1字节1应答模式*/


	I2C_GenerateSTART(USR_I2C_USED, ENABLE);
	 while(!I2C_CheckEvent(USR_I2C_USED, I2C_EVENT_MASTER_MODE_SELECT))
	 	; //清除EV5
	
	I2C_Send7bitAddress(USR_I2C_USED, max10300_WR_address, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(USR_I2C_USED,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		; //ADDR=1，清除EV6 
		
	I2C_SendData(USR_I2C_USED, Register_Address);
	 while(! I2C_CheckEvent(USR_I2C_USED, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	 	;//移位寄存器非空，数据寄存器已经空，产生EV8，发送数据到DR既可清除该事件

	I2C_GenerateSTART(USR_I2C_USED, ENABLE);
	 while(!I2C_CheckEvent(USR_I2C_USED, I2C_EVENT_MASTER_MODE_SELECT))
	 	; //清除EV5
	
	I2C_Send7bitAddress(USR_I2C_USED, max10300_WR_address, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(USR_I2C_USED, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	

 	while (no)
    {	
	
		while(!I2C_CheckEvent(USR_I2C_USED, I2C_EVENT_MASTER_BYTE_RECEIVED)); // EV7 
	    data1 = I2C_ReceiveData(USR_I2C_USED);

		while(!I2C_CheckEvent(USR_I2C_USED, I2C_EVENT_MASTER_BYTE_RECEIVED)); // EV7
	    data2 = I2C_ReceiveData(USR_I2C_USED);

	    Word_Data[i][0] = (((u16)data1 << 8) | data2);  //



       while(!I2C_CheckEvent(USR_I2C_USED, I2C_EVENT_MASTER_BYTE_RECEIVED)); // EV7
		data1 = I2C_ReceiveData(USR_I2C_USED);

	    if(no==1)
		{
     		I2C_AcknowledgeConfig(I2C1, DISABLE);	//最后一位后要关闭应答的
    		I2C_GenerateSTOP(I2C1, ENABLE);			//发送停止位
		
		}

		while(!I2C_CheckEvent(USR_I2C_USED, I2C_EVENT_MASTER_BYTE_RECEIVED)); // EV7
		data2 = I2C_ReceiveData(USR_I2C_USED);

		Word_Data[i][1] = (((u16)data1 << 8) | data2); 
		i++;

		
	    /* Decrement the read bytes counter */
	    no--;
    }

	
	I2C_AcknowledgeConfig(USR_I2C_USED, ENABLE);//将应答位使能回去，等待下次通信
	I2C_GenerateSTOP(I2C1, ENABLE);			//发送停止位
}

void max10300_init(void)
{
	max10300_Bus_Write(0x06, 0x0b);
	max10300_Bus_Write(0x01, 0xF0); // open all of interrupt
	max10300_Bus_Write(0x09, 0x33); // r_pa=3,ir_pa=3
//	max10300_Bus_Write(0x07, 0x47); // SPO2_SR[4:2]=001  100 per second    LED_PW[1:0]=11  16BITS
	max10300_Bus_Write(0x07, 0x43); // SPO2_SR[4:2]=000   50 per second    LED_PW[1:0]=11  16BITS

	max10300_Bus_Write(0x02, 0x00);   // set FIFO write Pointer reg = 0x00 for clear it
	max10300_Bus_Write(0x03, 0x00);	// set Over Flow Counter  reg = 0x00 for clear it
	max10300_Bus_Write(0x04, 0x00);	// set FIFO Read Pointer  reg = 0x00 for clear it 
}


	

void test_bh1750_fun(void)
{
	u16 temp_num=0;
	u8  temperature=0;
	u16 fifo_word_buff[15][2];
	u16 i =0;
	u16 Heart_Rate=0;
	while(1)
	{

		temp_num = max10300_Bus_Read(INTERRUPT_REG);
		temperature = max10300_Bus_Read(TEMPERATURE_REG);
		
		//if( (INTERRUPT_REG_HR_RDY&temp_num) && (INTERRUPT_REG_SPO2_RDY&temp_num) )
		if( INTERRUPT_REG_A_FULL&temp_num )
		{
			max10300_FIFO_Read(0x05,fifo_word_buff,15);
			max10300_Bus_Write(0x02, 0x00);   //set FIFO write Pointer reg = 0x00 for clear it
			max10300_Bus_Write(0x03, 0x00);	//set Over Flow Counter  reg = 0x00 for clear it
			max10300_Bus_Write(0x04, 0x00);	//set FIFO Read Pointer  reg = 0x00 for clear it 
		}
						
							
	}
}



