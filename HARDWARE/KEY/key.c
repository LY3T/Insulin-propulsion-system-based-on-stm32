#include "key.h"
#include "delay.h"

//按键初始化函数 
//PA15和PC5 设置成输入
void KEY_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOA,ENABLE);//使能PORTA,PORTC时钟

//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//关闭jtag，使能SWD，可以用SWD模式调试
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;//PC0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //设置成下拉输入
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC0
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;//PC1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //设置成下拉输入
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC1
 
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;//PC2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //设置成下拉输入	  
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC2
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;//PC3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //设置成下拉输入	  
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC3

//
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;//PC3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //设置成下拉输入	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOC3

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;//PC3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //设置成下拉输入	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOC3


} 
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//返回值：
//0，没有任何按键按下
//KEY0_PRES，KEY0按下
//KEY1_PRES，KEY1按下
//WKUP_PRES，WK_UP按下 
//注意此函数有响应优先级,KEY0>KEY1>WK_UP!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(OK==1||ADD==1||SUB==1||RES==1))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(OK==1)return OK_PRES;
		else if(ADD==1)return ADD_PRES;
		else if(SUB==1)return SUB_PRES;
		else if(RES==1)return RES_PRES;	
	}else if(OK==0&&ADD==0&&SUB==0&&RES==0)key_up=1; 	     
	return 0;// 无按键按下
}
