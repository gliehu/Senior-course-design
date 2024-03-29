#include "lcd.h"
#include "gui.h"
#include "test.h"
#include "touch.h"
#include "pic.h"
#include <bsp_SysTick.h>
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//测试硬件：单片机STM32F103RBT6,正点原子MiniSTM32开发板,主频72MHZ
//QDtech-TFT液晶驱动 for STM32 IO模拟
//xiao冯@ShenZhen QDtech co.,LTD
//公司网站:www.qdtft.com
//淘宝网站：http://qdtech.taobao.com
//我司提供技术支持，任何技术问题欢迎随时交流学习
//固话(传真) :+86 0755-23594567 
//手机:15989313508（冯工） 
//邮箱:goodtft@126.com 
//Skype:QDtech2008
//技术支持QQ:3002773612
//技术交流QQ群:324828016
//创建日期:2013/5/13
//版本：V2.0
//版权所有，盗版必究。
//Copyright(C) 深圳市全动电子技术有限公司 2013-2019
//All rights reserved
/****************************************************************************************************

//====================================电源接线==========================================//
//VCC        ---接 5V或3.3V电源
//GND        ---接 地

//===============================液晶屏控制线接线======================================//
//CS	       ---接PB11	//液晶屏片选信号
//RESET	     ---接PB12	//液晶屏复位信号
//DC/RS	     ---接PB10	//寄存器/数据选择信号
//SDI(MOSI)	 ---接PB15	//SPI总线写数据
//SCK	       ---接PB13	//SPI总线时钟信号
//LED	       ---接PB9	 //背光控制信号(高电平点亮)如果不需要控制可接VCC常亮
//SDO(MISO)  ---接PB14 //SPI读信号如无需读取可不接

//===============================触摸屏接线========================================//
//不使用触摸或者模块本身不带触摸，则可不连接  
//T_CLK	     ---接PC0	 //触摸SPI总线时钟
//T_CS	     ---接PC13 //触摸IC片选
//T_DIN	     ---接PC3	//触摸SPI总线输入
//T_DO	     ---接PC2	//触摸SPI总线输出
//T_IRQ	     ---接PC1	//触摸屏中断信号

*************************************************************************************************/
//========================variable==========================//
u16 ColorTab[5]={BRED,YELLOW,RED,GREEN,BLUE};//定义颜色数组
//=====================end of variable======================//

//******************************************************************
//函数名：  DrawTestPage
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    绘制测试界面
//输入参数：str :字符串指针
//返回值：  无
//修改记录：
//******************************************************************
void DrawTestPage(u8 *str)
{
//绘制固定栏up
LCD_Fill(0,0,lcddev.width,20,BLUE);
//绘制固定栏down
//LCD_Fill(0,lcddev.height-20,lcddev.width,lcddev.height,BLUE);
POINT_COLOR=WHITE;
Gui_StrCenter(0,2,WHITE,BLUE,str,16,1);//居中显示
//Gui_StrCenter(0,lcddev.height-18,WHITE,BLUE,"QDtech版权所有",16,1);//居中显示
//绘制测试区域
LCD_Fill(0,20,lcddev.width,lcddev.height,WHITE);
}

//******************************************************************
//函数名：  main_test
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    绘制全动电子综合测试程序主界面
//输入参数：无
//返回值：  无
//修改记录：
//******************************************************************
void main_test(void)
{
	DrawTestPage("脉搏波测试程序");
	
	Gui_StrCenter(0,30,RED,BLUE,"全动电子",16,1);//居中显示
	Gui_StrCenter(0,60,RED,BLUE,"综合测试程序",16,1);//居中显示	
	Gui_StrCenter(0,90,YELLOW,BLUE,"2.8' SPI ILI9341 240X320",16,1);//居中显示
	Gui_StrCenter(0,120,BLUE,BLUE,"xiaoFeng@QDtech 2014-02-25",16,1);//居中显示
	Delay_ms(1500);		
	Delay_ms(1500);
}

//******************************************************************
//函数名：  Test_Color
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    颜色填充测试，依次填充白色、黑色、红色、绿色、蓝色
//输入参数：无
//返回值：  无
//修改记录：
//******************************************************************
void Test_Color(void)  
{
	//DrawTestPage("测试1:纯色填充测试");
	LCD_Fill(0,0,lcddev.width,lcddev.height-0,WHITE);
	Show_Str(lcddev.width-50,30,BLUE,YELLOW,"White",16,1);//Delay_ms(500);
	LCD_Fill(0,0,lcddev.width,lcddev.height-0,BLACK);
	Show_Str(lcddev.width-50,30,BLUE,YELLOW,"Black",16,1);//Delay_ms(500);
	LCD_Fill(0,0,lcddev.width,lcddev.height-0,RED);
	Show_Str(lcddev.width-50,30,BLUE,YELLOW,"Red",16,1); //Delay_ms(500);
	LCD_Fill(0,0,lcddev.width,lcddev.height-0,GREEN);
	Show_Str(lcddev.width-50,30,BLUE,YELLOW,"Green",16,1);//Delay_ms(500);
	LCD_Fill(0,0,lcddev.width,lcddev.height-0,BLUE);
	Show_Str(lcddev.width-50,30,WHITE,YELLOW,"Blue",16,1);//Delay_ms(500);

}

//******************************************************************
//函数名：  Test_FillRec
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    矩形框显示和填充测试，依次显示粉红色、黄色、红色、绿色、蓝色矩形框，
//       	延时1500毫秒后，依次按照粉红色、黄色、红色、绿色、蓝色填充矩形框 
//输入参数：无
//返回值：  无
//修改记录：
//******************************************************************
void Test_FillRec(void)
{
	u8 i=0;
	DrawTestPage("测试2:GUI矩形填充测试");
	LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE);
	for (i=0; i<5; i++) 
	{
		LCD_DrawRectangle(lcddev.width/2-80+(i*15),lcddev.height/2-80+(i*15),lcddev.width/2-80+(i*15)+60,lcddev.height/2-80+(i*15)+60); 
		POINT_COLOR=ColorTab[i];
	}
	Delay_ms(1500);	
	LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE); 
	for (i=0; i<5; i++) 
	{
		LCD_DrawFillRectangle(lcddev.width/2-80+(i*15),lcddev.height/2-80+(i*15),lcddev.width/2-80+(i*15)+60,lcddev.height/2-80+(i*15)+60); 
		POINT_COLOR=ColorTab[i];
	}
	Delay_ms(1500);
}

//******************************************************************
//函数名：  Test_FillRec
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    圆形框显示和填充测试，依次显示粉红色、黄色、红色、绿色、蓝色圆形框，
//       	延时1500毫秒后，依次按照粉红色、黄色、红色、绿色、蓝色填充圆形框 
//输入参数：无
//返回值：  无
//修改记录：
//******************************************************************
void Test_Circle(void)
{
	u8 i=0;
	DrawTestPage("测试3:GUI画圆填充测试");
	LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE);
	for (i=0; i<5; i++)  
		gui_circle(lcddev.width/2-80+(i*25),lcddev.height/2-50+(i*25),ColorTab[i],30,0);
	Delay_ms(1500);	
	LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE); 
	for (i=0; i<5; i++) 
	  	gui_circle(lcddev.width/2-80+(i*25),lcddev.height/2-50+(i*25),ColorTab[i],30,1);
	Delay_ms(1500);
}

//******************************************************************
//函数名：  English_Font_test
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    英文显示测试 
//输入参数：无
//返回值：  无
//修改记录：
//******************************************************************
void English_Font_test(void)
{
	DrawTestPage("测试4:英文显示测试");
	POINT_COLOR=RED;
	BACK_COLOR=BLUE;
	LCD_ShowString(10,30,12,"6X12:abcdefghijklmnopqrstuvwxyz0123456789",0);
	LCD_ShowString(10,45,12,"6X12:ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789",1);
	LCD_ShowString(10,60,12,"6X12:~!@#$%^&*()_+{}:<>?/|-+.",0);
	LCD_ShowString(10,80,16,"8X16:abcdefghijklmnopqrstuvwxyz0123456789",0);
	LCD_ShowString(10,100,16,"8X16:ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789",1);
	LCD_ShowString(10,120,16,"8X16:~!@#$%^&*()_+{}:<>?/|-+.",0); 
	Delay_ms(1200);
}

//******************************************************************
//函数名：  Chinese_Font_test
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    中文显示测试
//输入参数：无
//返回值：  无
//修改记录：
//******************************************************************
void Chinese_Font_test(void)
{	
	DrawTestPage("测试5:中文显示测试");
	Show_Str(10,30,BLUE,YELLOW,"16X16:全动电子技术有限公司欢迎您",16,0);
	Show_Str(10,50,BLUE,YELLOW,"16X16:Welcome全动电子",16,1);
	Show_Str(10,70,BLUE,YELLOW,"24X24:深圳市中文测试",24,1);
	Show_Str(10,100,BLUE,YELLOW,"32X32:字体测试",32,1);
	Delay_ms(1200);
}

//******************************************************************
//函数名：  Pic_test
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    图片显示测试，依次显示三幅40X40 QQ图像
//输入参数：无
//返回值：  无
//修改记录：
//******************************************************************
void Pic_test(void)
{
	DrawTestPage("测试6:图片显示测试");
	LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE);
	Gui_Drawbmp16(30,30,gImage_qq);
	Show_Str(30+12,75,BLUE,YELLOW,"QQ",16,1);
	Gui_Drawbmp16(90,30,gImage_qq);
	Show_Str(90+12,75,BLUE,YELLOW,"QQ",16,1);
	Gui_Drawbmp16(150,30,gImage_qq);
	Show_Str(150+12,75,BLUE,YELLOW,"QQ",16,1);
	Delay_ms(1200);
}

