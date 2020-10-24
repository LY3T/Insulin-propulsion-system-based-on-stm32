#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK miniSTM32������
//������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   	 


#define KEY0 PCin(0)   	
#define KEY1 PCin(1)	 
#define KEY2 PCin(2)	 
#define KEY3 PCin(3)

//
#define KEY4 PAin(4)
#define KEY5 PAin(5)


#define OK  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)//��ȡ����0
#define ADD  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1)//��ȡ����1
#define SUB  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2)//��ȡ����2 
#define RES  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_3)//��ȡ����2  

//
#define RES_SW_F  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)//ǰ����λ 
#define RES_SW_B  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)//���˸�λ 

 
#define OK_PRES	1		  //OK 
#define ADD_PRES	2		//ADD 
#define SUB_PRES	3		//SUB
#define RES_PRES 4		//RES

//
#define RES_SW_F_PRES 5		//RES_SWB
#define RES_SW_B_PRES 6		//RES_SWF

void KEY_Init(void);//IO��ʼ��
u8 KEY_Scan(u8 mode);  	//����ɨ�躯��					    
#endif
