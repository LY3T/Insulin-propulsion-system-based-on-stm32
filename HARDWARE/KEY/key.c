#include "key.h"
#include "delay.h"

//������ʼ������ 
//PA15��PC5 ���ó�����
void KEY_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOA,ENABLE);//ʹ��PORTA,PORTCʱ��

//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//�ر�jtag��ʹ��SWD��������SWDģʽ����
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;//PC0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //���ó���������
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC0
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;//PC1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //���ó���������
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC1
 
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;//PC2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //���ó���������	  
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC2
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;//PC3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //���ó���������	  
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC3

//
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;//PC3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //���ó���������	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOC3

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;//PC3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //���ó���������	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOC3


} 
//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//����ֵ��
//0��û���κΰ�������
//KEY0_PRES��KEY0����
//KEY1_PRES��KEY1����
//WKUP_PRES��WK_UP���� 
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>WK_UP!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(key_up&&(OK==1||ADD==1||SUB==1||RES==1))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(OK==1)return OK_PRES;
		else if(ADD==1)return ADD_PRES;
		else if(SUB==1)return SUB_PRES;
		else if(RES==1)return RES_PRES;	
	}else if(OK==0&&ADD==0&&SUB==0&&RES==0)key_up=1; 	     
	return 0;// �ް�������
}
