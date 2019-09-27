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
    I2C_InitStructure.I2C_ClockSpeed = 100000;//100K�ٶ�
    
	I2C_Cmd(I2C1, ENABLE);
	I2C_Init(I2C1, &I2C_InitStructure);
	/*����1�ֽ�1Ӧ��ģʽ*/
	I2C_AcknowledgeConfig(I2C1, ENABLE);
}


void I2C1_GPIO_Config(void)
{

	GPIO_InitTypeDef GPIO_InitStructure; //GPIO�ṹ�嶨��
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//ʹ��I2C��IO�� 

		/* ʹ���� I2C1 �йص�ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	
	/* PB6-I2C1_SCL��PB7-I2C1_SDA*/
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;

	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; // ��©���
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ���ṹ������

}

/* I2C ����ģʽ���� */

void I2C1_Mode_config(void)
{
		/*����I2C�ṹ��*/
	I2C_InitTypeDef  I2C_InitStructure;



		
	/*����ΪI2Cģʽ*/
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	
	/*�ò���ֻ����I2C �����ڿ���ģʽ��ʱ�ӹ���Ƶ�ʸ��� 100KHz���²������塣*/
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	
	/*���õ�һ���豸�����ַ*/
	I2C_InitStructure.I2C_OwnAddress1 =0x30;
	
	/*ʹ��Ӧ��*/
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;
	
	/*AT24C02��ַΪ7λ��������7λ������*/
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; /*ʱ�����ʣ���HZΪ��λ�ģ����Ϊ400khz*/
	
	I2C_InitStructure.I2C_ClockSpeed = 20000;

	/* ʹ�� I2C1 */
	I2C_Cmd(I2C1, ENABLE);
	
	/* I2C1 ��ʼ�� */
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
		; //���ÿ⺯�����I2C�����Ƿ���BUSY״̬
	I2C_GenerateSTART(USR_I2C_USED, ENABLE);

	 while(!I2C_CheckEvent(USR_I2C_USED, I2C_EVENT_MASTER_MODE_SELECT))
	 	; //���EV5
	 	
	I2C_Send7bitAddress(USR_I2C_USED, max10300_WR_address, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(USR_I2C_USED,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		; //ADDR=1�����EV6  

	I2C_SendData(USR_I2C_USED, Register_Address);
	 while(! I2C_CheckEvent(USR_I2C_USED, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	 	;//��λ�Ĵ����ǿգ����ݼĴ����Ѿ��գ�����EV8���������ݵ�DR�ȿ�������¼�
	
		
	I2C_SendData(USR_I2C_USED, Word_Data);
	 while(! I2C_CheckEvent(USR_I2C_USED, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	 	;//��λ�Ĵ����ǿգ����ݼĴ����Ѿ��գ�����EV8���������ݵ�DR�ȿ�������¼�
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
		; //���ÿ⺯�����I2C�����Ƿ���BUSY״̬
	I2C_GenerateSTART(USR_I2C_USED, ENABLE);
	 while(!I2C_CheckEvent(USR_I2C_USED, I2C_EVENT_MASTER_MODE_SELECT))
	 	; //���EV5
	 	
	I2C_Send7bitAddress(USR_I2C_USED, max10300_WR_address, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(USR_I2C_USED,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		; //ADDR=1�����EV6  
		
	I2C_SendData(USR_I2C_USED, Register_Address);
	 while(! I2C_CheckEvent(USR_I2C_USED, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	 	;//��λ�Ĵ����ǿգ����ݼĴ����Ѿ��գ�����EV8���������ݵ�DR�ȿ�������¼�

	I2C_GenerateSTART(USR_I2C_USED, ENABLE);
	 while(!I2C_CheckEvent(USR_I2C_USED, I2C_EVENT_MASTER_MODE_SELECT))
	 	; //���EV5
	 	
	I2C_Send7bitAddress(USR_I2C_USED, max10300_WR_address, I2C_Direction_Receiver);

	while(!I2C_CheckEvent(USR_I2C_USED, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	I2C_AcknowledgeConfig(USR_I2C_USED, DISABLE);	//���һλ��Ҫ�ر�Ӧ���
    I2C_GenerateSTOP(USR_I2C_USED, ENABLE);			//����ֹͣλ
	
	
		while(!(I2C_CheckEvent(USR_I2C_USED, I2C_EVENT_MASTER_BYTE_RECEIVED)))
		; //��ȡ����
		data1 = I2C_ReceiveData(USR_I2C_USED);
	

	I2C_AcknowledgeConfig(USR_I2C_USED, ENABLE);//��Ӧ��λʹ�ܻ�ȥ���ȴ��´�ͨ��
	
	return (data1);
#endif
}

static void i2c_Delay(void)
{
	uint8_t i;

	/*��
		CPU��Ƶ168MHzʱ�����ڲ�Flash����, MDK���̲��Ż�����̨ʽʾ�����۲Ⲩ�Ρ�
		ѭ������Ϊ5ʱ��SCLƵ�� = 1.78MHz (����ʱ: 92ms, ��д������������ʾ����̽ͷ���ϾͶ�дʧ�ܡ�ʱ��ӽ��ٽ�)
		ѭ������Ϊ10ʱ��SCLƵ�� = 1.1MHz (����ʱ: 138ms, ���ٶ�: 118724B/s)
		ѭ������Ϊ30ʱ��SCLƵ�� = 440KHz�� SCL�ߵ�ƽʱ��1.0us��SCL�͵�ƽʱ��1.2us

		��������ѡ��2.2Kŷʱ��SCL������ʱ��Լ0.5us�����ѡ4.7Kŷ����������Լ1us

		ʵ��Ӧ��ѡ��400KHz���ҵ����ʼ���
	*/
	for (i = 0; i < 30; i++);
}




void max10300_FIFO_Read(u8 Register_Address,u16  Word_Data[][2],u8 count)
{
	u8 i=0;
	u8 no = count;
	u8 data1, data2;
	
	
	while(I2C_GetFlagStatus(USR_I2C_USED, I2C_FLAG_BUSY))
		; //���ÿ⺯�����I2C�����Ƿ���BUSY״̬
		

	I2C_AcknowledgeConfig(USR_I2C_USED, ENABLE);   /*����1�ֽ�1Ӧ��ģʽ*/


	I2C_GenerateSTART(USR_I2C_USED, ENABLE);
	 while(!I2C_CheckEvent(USR_I2C_USED, I2C_EVENT_MASTER_MODE_SELECT))
	 	; //���EV5
	
	I2C_Send7bitAddress(USR_I2C_USED, max10300_WR_address, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(USR_I2C_USED,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		; //ADDR=1�����EV6 
		
	I2C_SendData(USR_I2C_USED, Register_Address);
	 while(! I2C_CheckEvent(USR_I2C_USED, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	 	;//��λ�Ĵ����ǿգ����ݼĴ����Ѿ��գ�����EV8���������ݵ�DR�ȿ�������¼�

	I2C_GenerateSTART(USR_I2C_USED, ENABLE);
	 while(!I2C_CheckEvent(USR_I2C_USED, I2C_EVENT_MASTER_MODE_SELECT))
	 	; //���EV5
	
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
     		I2C_AcknowledgeConfig(I2C1, DISABLE);	//���һλ��Ҫ�ر�Ӧ���
    		I2C_GenerateSTOP(I2C1, ENABLE);			//����ֹͣλ
		
		}

		while(!I2C_CheckEvent(USR_I2C_USED, I2C_EVENT_MASTER_BYTE_RECEIVED)); // EV7
		data2 = I2C_ReceiveData(USR_I2C_USED);

		Word_Data[i][1] = (((u16)data1 << 8) | data2); 
		i++;

		
	    /* Decrement the read bytes counter */
	    no--;
    }

	
	I2C_AcknowledgeConfig(USR_I2C_USED, ENABLE);//��Ӧ��λʹ�ܻ�ȥ���ȴ��´�ͨ��
	I2C_GenerateSTOP(I2C1, ENABLE);			//����ֹͣλ
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



