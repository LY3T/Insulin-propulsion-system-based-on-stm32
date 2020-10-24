#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK miniSTM32开发板
//按键驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   	 


#define KEY0 PCin(0)   	
#define KEY1 PCin(1)	 
#define KEY2 PCin(2)	 
#define KEY3 PCin(3)

//
#define KEY4 PAin(4)
#define KEY5 PAin(5)


#define OK  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)//读取按键0
#define ADD  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1)//读取按键1
#define SUB  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2)//读取按键2 
#define RES  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_3)//读取按键2  

//
#define RES_SW_F  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)//前进复位 
#define RES_SW_B  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)//回退复位 

 
#define OK_PRES	1		  //OK 
#define ADD_PRES	2		//ADD 
#define SUB_PRES	3		//SUB
#define RES_PRES 4		//RES

//
#define RES_SW_F_PRES 5		//RES_SWB
#define RES_SW_B_PRES 6		//RES_SWF

void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8 mode);  	//按键扫描函数					    
#endif
