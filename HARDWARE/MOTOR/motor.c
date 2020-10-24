#include <motor.h>
#include "delay.h"

void motor_Init(void)
{

 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PA,PD端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 //LED0-->PA.8 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.8
 GPIO_SetBits(GPIOB,GPIO_Pin_12);
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	    		 //LED1-->PD.2 端口配置, 推挽输出
 GPIO_Init(GPIOB, &GPIO_InitStructure);
 GPIO_SetBits(GPIOB,GPIO_Pin_13);
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;	    		 //LED1-->PD.2 端口配置, 推挽输出
 GPIO_Init(GPIOB, &GPIO_InitStructure);
 GPIO_SetBits(GPIOB,GPIO_Pin_14);
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;	    		 //LED1-->PD.2 端口配置, 推挽输出
 GPIO_Init(GPIOB, &GPIO_InitStructure);
 GPIO_SetBits(GPIOB,GPIO_Pin_15);


}

void run_forward(u16 time)
{
	//step1
	AIN1_1=0;
	AIN1_2=1;
	BIN1_1=0;
	BIN1_2=0;
	delay_ms(time);
	//step2
	AIN1_1=0;
	AIN1_2=1;
	BIN1_1=0;
	BIN1_2=1;
	delay_ms(time);
	//step3
	AIN1_1=0;
	AIN1_2=0;
	BIN1_1=0;
	BIN1_2=1;
	delay_ms(time);
	//step4
	AIN1_1=1;
	AIN1_2=0;
	BIN1_1=0;
	BIN1_2=1;
	delay_ms(time);
	//step5
	AIN1_1=1;
	AIN1_2=0;
	BIN1_1=0;
	BIN1_2=0;
	delay_ms(time);
	//step6
	AIN1_1=1;
	AIN1_2=0;
	BIN1_1=1;
	BIN1_2=0;
	delay_ms(time);
	//step7
	AIN1_1=0;
	AIN1_2=0;
	BIN1_1=1;
	BIN1_2=0;
	delay_ms(time);
	//step8
	AIN1_1=0;
	AIN1_2=1;
	BIN1_1=1;
	BIN1_2=0;
	delay_ms(time);
}

void run_back(u16 time)
{
	//step1
	AIN1_1=0;
	AIN1_2=1;
	BIN1_1=1;
	BIN1_2=0;
	delay_ms(time);
	//step2
	AIN1_1=0;
	AIN1_2=0;
	BIN1_1=1;
	BIN1_2=0;
	delay_ms(time);
	//step3
	AIN1_1=1;
	AIN1_2=0;
	BIN1_1=1;
	BIN1_2=0;
	delay_ms(time);
	//step4
	AIN1_1=1;
	AIN1_2=0;
	BIN1_1=0;
	BIN1_2=0;
	delay_ms(time);
	//step5
	AIN1_1=1;
	AIN1_2=0;
	BIN1_1=0;
	BIN1_2=1;
	delay_ms(time);
	//step6
	AIN1_1=0;
	AIN1_2=0;
	BIN1_1=0;
	BIN1_2=1;
	delay_ms(time);
	//step7
	AIN1_1=0;
	AIN1_2=1;
	BIN1_1=0;
	BIN1_2=1;
	delay_ms(time);
	//step8
	AIN1_1=0;
	AIN1_2=1;
	BIN1_1=0;
	BIN1_2=0;
	delay_ms(time);
}

void stop(void)
{
	AIN1_1=0;
	AIN1_2=0;
	BIN1_1=0;
	BIN1_2=0;
}
