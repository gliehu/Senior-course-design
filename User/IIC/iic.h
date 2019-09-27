#ifndef __IIC_H
#define __IIC_H
#include <stm32f10x.h> 

#define USR_I2C_USED I2C1

#define max10300_WR_address 0xAE

#define INTERRUPT_REG  						0X00
#define TEMPERATURE_REG  					0X16

#define INTERRUPT_REG_A_FULL  			(0X01<<7)
#define INTERRUPT_REG_TEMP_RDY  		(0X01<<6)
#define INTERRUPT_REG_HR_RDY  			(0X01<<5)
#define INTERRUPT_REG_SPO2_RDY  		(0X01<<4)
#define INTERRUPT_REG_PWR_RDY  			(0X01<<0)


void I2C_Conf(void);

void max10300_Bus_Write(u8 Register_Address, u8 Word_Data);
u8 max10300_Bus_Read(u8 Register_Address);
void max10300_FIFO_Read(u8 Register_Address,u16  Word_Data[][2],u8 count);
void max10300_init(void);
void test_bh1750_fun(void);

#endif

