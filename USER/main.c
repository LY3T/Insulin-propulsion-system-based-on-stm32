#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "oled.h"
#include "motor.h"
#include "adc.h"
#include "key.h"
#include "beep.h"
#include "rtc.h"
#include "usmart.h"
#include "stmflash.h"

//Ĭ���������ӵ�ע���ٶ�Ϊ40���û����޸�
#define alarm0_speed 40
#define alarm1_speed 40
#define alarm2_speed 40

//�������ֵ���Ƚ�ֵ��
#define emg_temp 2920

//�͵�������ֵ(%)
#define low_power_data 10

//�ߵ�������ֵ(%)
#define high_power_data 50

//Mini STM32�����巶������9
//OLED��ʾ ʵ��
//����ԭ��@ALIENTEK
//������̳:www.openedv.com	

u8 clear_flag=1;//������־ 

u8 interset_flag=0;//�����־

u8 on_off[2][4]={"off"," on"};//�ڶ���ʱ��2�����������м����ַ�����4����ÿ���ַ����ĳ��Ȳ�Ҫ����3,��Ϊ�ַ����ں����Զ�����\0����ַ� 

u8 alarm_flag[3]={1,1,1};//���ӱ�־����Ӧ����0~2

u8 save_key=0;//��������ʱ�ļ�ֵ��ʱ����

u8 timeset=0;//����ʱ��ѭ������

u8 low_power_flag=0;//�����ͱ�־

u8 i;//forѭ������

u8 t=0,m=0;//�룬�ֵ���ʱ����

u16 adcx=4600;

u8 emg_adc=100;

u16 temp=0;//��ֵ����ʱ����

u8 key_stack[4]={0,0,0,0};//��ֵ��ջ
u8 key_temp[4]={0,0,0,0};//��ֵ��ʱ����

//����ʱ����ʾ���棬0~5λʱ�䣬6~8λע��ʱ�䣬9~11λ���ӿ�����־����ͬ
u8 alarm_time[12]={9,30,12,30,18,30,10,10,10,1,1,1};
//����ʱ����ʱ�������
u8 alarm_time_temp[12]={9,30,12,30,18,30,10,10,10,1,1,1};

#define SIZE sizeof(alarm_time_temp)		//���鳤��
#define FLASH_SAVE_ADDR  0X08070000		//����FLASH �����ַ(����Ϊż��������ֵҪ���ڱ�������ռ��FLASH�Ĵ�С+0X08000000)

//OLED���ߺ���
void myOLED_DrawLine(u8 x,u8 y,u8 num)
{
	for(;num>0;num--)
	{
		OLED_DrawPoint(x++,y,1);	
	}	
}





//�������������£�
u8 emg_proc_new(u16 adcx)
{
	u8 emg_data;
	emg_data=(adcx-emg_temp)/10;
	return emg_data;
}

//����������
//mode:1 ����죬delay_time:ÿ�γ�����ʱ�䣬warning_time:��Ĵ���
//mode:0 �����죬delay_time:������ʱ�䣬warning_time:����
void warning_tone(u8 mode,u16 delay_time,u8 warning_time)
{
	if(mode)
	{
		for(;warning_time>0;warning_time--)
		{
			beep=1;
			delay_ms(delay_time);
			beep=0;
		}
	}
	else 
	{
		beep=1;
		delay_ms(delay_time);
		beep=0;		
	}
}

//����ת��
void number_conversion(u8 num,u8 mode)
{
	if(mode==1)
	{
		if(num==0||num==2||num==4)
		{
			if(alarm_time[num]==24)
				alarm_time[num]=0;
		}
		if(num==1||num==3||num==5)
		{	
			if(alarm_time[num]==60)
				alarm_time[num]=0;
		}
		if(num==6||num==7||num==8)
		{	
			if(alarm_time[num]==60)
				alarm_time[num]=0;
		}		
	}
	
	if(mode==0)
	{
		if(num==0||num==2||num==4)
		{
			if(alarm_time[num]==255)
				alarm_time[num]=23;
		}
		if(num==1||num==3||num==5)
		{	
			if(alarm_time[num]==255)
				alarm_time[num]=59;
		}
		if(num==6||num==7||num==8)
		{	
			if(alarm_time[num]==255)
				alarm_time[num]=59;
		}		
	}

	
}


//����ʱ����ʱ��������
//mode=0,alarm_time����alarm_time_temp
//mode=1,alarm_time_temp����alarm_time
void alarm_time_set(u8 mode)
{
	if(mode==0)
	{
		for(i=0;i<12;i++)
		{
			alarm_time_temp[i]=alarm_time[i];
		}
	}
	if(mode==1)
	{
		for(i=0;i<12;i++)
		{
			alarm_time[i]=alarm_time_temp[i];
		}
	}		
}

//������ӭҳ
void page_welcome()
{
	OLED_ShowString(24,16,"Welcome",24);
	OLED_Refresh_Gram();//������ʾ��OLED
	delay_ms(1800);
	delay_ms(1800);	
	OLED_Clear();
}


//����ҳ��
void page_timeset()
{
	OLED_ShowString(0,0,"alarm time set:",16);
	OLED_ShowString(0,16,"1/",16);
	OLED_ShowString(0,32,"2/",16);
	OLED_ShowString(0,48,"3/",16);
	
	//����ʱ��
	OLED_ShowNum(24,16,alarm_time[0],2,16);	
	OLED_ShowNum(48,16,alarm_time[1],2,16);
	OLED_ShowNum(24,32,alarm_time[2],2,16);
	OLED_ShowNum(48,32,alarm_time[3],2,16);
	OLED_ShowNum(24,48,alarm_time[4],2,16);
	OLED_ShowNum(48,48,alarm_time[5],2,16);
	
	//ע��ʱ��
	OLED_ShowNum(72,16,alarm_time[6],2,16);
	OLED_ShowNum(72,32,alarm_time[7],2,16);
	OLED_ShowNum(72,48,alarm_time[8],2,16);	
	
	//���ӿ��ر�־
	OLED_ShowString(104,16,on_off[alarm_time[9]],16);
	OLED_ShowString(104,32,on_off[alarm_time[10]],16);
	OLED_ShowString(104,48,on_off[alarm_time[11]],16);
	
	//ð��
	OLED_ShowString(40,16,":",16);
	OLED_ShowString(40,32,":",16);
	OLED_ShowString(40,48,":",16);
	
	//��
	OLED_ShowString(88,16,"s",16);
	OLED_ShowString(88,32,"s",16);
	OLED_ShowString(88,48,"s",16);	
	
	//�»���
	switch(timeset)
	{
		//����ʱ��		
		case(0):myOLED_DrawLine(24,32,16);break;//
		case(1):myOLED_DrawLine(48,32,16);break;//  
		case(2):myOLED_DrawLine(24,48,16);break;//
		case(3):myOLED_DrawLine(48,48,16);break;//
		case(4):myOLED_DrawLine(24,63,16);break;//
		case(5):myOLED_DrawLine(48,63,16);break;//
		
		//ע��ʱ��
		case(6):myOLED_DrawLine(72,32,16);break;//
		case(7):myOLED_DrawLine(72,48,16);break;//	
		case(8):myOLED_DrawLine(72,63,16);break;//	
		
		//���ӱ�־
		case(9):myOLED_DrawLine(104,32,23);break;//
		case(10):myOLED_DrawLine(104,48,23);break;//	
		case(11):myOLED_DrawLine(104,63,23);break;//	
		
	}	
}

//������ʾ����
void page_save_ornot()
{
	if(clear_flag)
	{
		clear_flag=0;
		OLED_Clear();//����
	}
	OLED_ShowString(8,0,"Do you want to ",16);
	OLED_ShowString(0,16,"save the change?",16);
	OLED_ShowString(0,48,"no           yes",16);
	OLED_Refresh_Gram();//������ʾ��OLED
}

//���Ӻ������ɿ��Ʋ������
//m:���ӱ�ʶ0~2
//time:����ʱ��(s)
//speed:����ٶ�
void page_alarm_time_up(u8 m,u8 time,u8 speed)
{
	u8 k;
	
	//����������
	beep=1;
	delay_ms(100);
	beep=0;
	delay_ms(100);	
	beep=1;
	delay_ms(100);
	beep=0;	
	
	k=calendar.sec;
	while(1)
	{
		if(clear_flag)
		{
			clear_flag=0;
			OLED_Clear();//����
		}
		
		OLED_ShowString(16,16,"injecting...",16);		
		
		if(t!=calendar.sec)
		{
			t=calendar.sec;
			
			OLED_ShowString(20,36,"   :   :   ",16);			
			
			OLED_ShowNum(20,32,calendar.hour,2,24);
			OLED_ShowNum(52,32,calendar.min,2,24);
			OLED_ShowNum(84,32,calendar.sec,2,24);
			
			OLED_Refresh_Gram();//������ʾ��OLED			
		}
		run_forward(speed);
		if(((calendar.sec-k)>=time)||RES_SW_F==1)
		{
			if(RES_SW_F)
			{
				beep=1;
				stop();
				for(i=15;i>0;i--)//������λ�����
				{
					run_back(10);//����
				}
				beep=0;
			}			
			stop();
			//alarm_time[m+9]=0;//alarm_time_temp[m+9]=0;
			clear_flag=1;
			break;
		}					
	}
	
	//����������
	beep=1;
	delay_ms(100);
	beep=0;
	delay_ms(100);	
	beep=1;
	delay_ms(100);
	beep=0;	
	
	OLED_Clear();
}

/*�����ж�*/
void alarm_cmp()
{
		//�����жϳ���
		//����һ
		if((alarm_time_temp[0]==calendar.hour)&&
			(alarm_time_temp[1]==calendar.min)&&
		((calendar.sec==0)||(calendar.sec==1))&&
		(alarm_time_temp[9]))
		{
			page_alarm_time_up(0,alarm_time_temp[6],alarm0_speed);
		}

		//���Ӷ�
		if((alarm_time_temp[2]==calendar.hour)&&
			(alarm_time_temp[3]==calendar.min)&&
		((calendar.sec==0)||(calendar.sec==1))&&
		(alarm_time_temp[10]))
		{
			page_alarm_time_up(1,alarm_time_temp[7],alarm1_speed);
		}		

		//������
		if((alarm_time_temp[4]==calendar.hour)&&
			(alarm_time_temp[5]==calendar.min)&&
		((calendar.sec==0)||(calendar.sec==1))&&
		(alarm_time_temp[11]))
		{
			page_alarm_time_up(2,alarm_time_temp[8],alarm2_speed);
		}
						
		
		//����������
//		if((calendar.hour==0)&&(calendar.min==0)&&
//		((calendar.sec==0)||(calendar.sec==1)||(calendar.sec==2)))
//			alarm_time_set(1);	
}

/*����ʾ����*/
void display()
{
		//ʱ�Ӽ�������ʾ
		if(t!=calendar.sec)
		{
			t=calendar.sec;
			
			OLED_ShowString(8,0,"    /  /  ",16);	   
			OLED_ShowString(20,36,"   :   :   ",16);	
			
			OLED_ShowNum(8,0,calendar.w_year,4,16);									  
			OLED_ShowNum(48,0,calendar.w_month,2,16);									  
			OLED_ShowNum(72,0,calendar.w_date,2,16);			
		
		switch(calendar.week)
		{
			case 0:
				OLED_ShowString(32,16," Sunday ",16);
				break;
			case 1:
				OLED_ShowString(32,16," Monday ",16);
				break;
			case 2:
				OLED_ShowString(32,16,"Tuesday ",16);
				break;
			case 3:
				OLED_ShowString(32,16,"Wednesday",16);
				break;
			case 4:
				OLED_ShowString(32,16,"Thursday ",16);
				break;
			case 5:
				OLED_ShowString(32,16," Friday ",16);
				break;
			case 6:
				OLED_ShowString(32,16,"Saturday",16);
				break;  
		}
			
		OLED_ShowNum(20,32,calendar.hour,2,24);
		OLED_ShowNum(52,32,calendar.min,2,24);
		OLED_ShowNum(84,32,calendar.sec,2,24);	
		
//		OLED_ShowNum(96,16,adcx,4,16);

		//������ʾ
//  u8 emg_proc_new(u16 adcx)
		OLED_ShowNum(96,0,emg_adc,3,16);
		OLED_ShowString(120,0,"%",16);

		
		OLED_Refresh_Gram();//������ʾ��OLED
		}	
}

/*����ɨ��*/
void scan()
{
		temp=KEY_Scan(0);
		key_stack[temp-1]=1;	
}

/*�����¼�*/
void key_event()
{
		while(key_stack[0])	//���OK��
		{
			//����
			if(clear_flag)
			{
				clear_flag=0;
				OLED_Clear();		
				//������ʾ����
				alarm_time_set(1);
			}
			
			//�������ý���
			page_timeset();
			//����OLED
			OLED_Refresh_Gram();//������ʾ��OLED

			//Ԥ����ȡ��ֵ��ջ����ֵ�����ջ
			for(i=0;i<4;i++)
			{
				key_temp[i]=key_stack[i];
			}
			//����ɨ��
			temp=KEY_Scan(0);
			key_stack[temp-1]++;
			
			//�˳��жϣ���ֵ��ջ����
			if(temp==RES_PRES)
			{
				for(i=0;i<4;i++)
					key_stack[i]=0;
				clear_flag=1;
				interset_flag=1;				
				OLED_Clear();
				break;
			}
			
			//ʱ��ֵ����
			if((key_stack[0]-key_temp[0])>=1)
			{
				timeset++;
				if(timeset==12)
					timeset=0;
			}
			//��
			if((key_stack[1]-key_temp[1])>=1)
			{
				if(timeset==9||timeset==10||timeset==11)
					alarm_time[timeset]=1;
				else 
				{
					alarm_time[timeset]++;
					number_conversion(timeset,1);
				}
			}
			//��
			if((key_stack[2]-key_temp[2])>=1)
			{
				if(timeset==9||timeset==10||timeset==11)
					alarm_time[timeset]=0;
				else 
				{
					alarm_time[timeset]--;
					number_conversion(timeset,0);
				}					
			}	
		}	
		//�Ƿ񱣴�
		while(interset_flag)
		{
			page_save_ornot();
			save_key=KEY_Scan(0);
			if(save_key==OK_PRES)
			{
				interset_flag=0;
				clear_flag=1;
				alarm_time_set(0);
				//���浽flash
				STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)alarm_time_temp,SIZE);
				
				OLED_Clear();
			}
			if(save_key==RES_PRES)
			{
				interset_flag=0;
				clear_flag=1;
				OLED_Clear();
			}									
		}
		//�����������
		while(key_stack[2])
		{
			if(t!=calendar.sec)
			{
				t=calendar.sec;
				OLED_ShowNum(20,32,calendar.hour,2,24);
				OLED_ShowNum(52,32,calendar.min,2,24);
				OLED_ShowNum(84,32,calendar.sec,2,24);
				OLED_ShowString(116,32,">",24);				
				OLED_Refresh_Gram();//������ʾ��OLED
			}
			run_back(10);//����
			if((KEY_Scan(0)==SUB_PRES)||(RES_SW_B==1))
			{				
				if(RES_SW_B==1)
				{
					beep=1;
					stop();
					for(i=13;i>0;i--)//������λ�����
					{
						run_forward(10);//���
					}
					beep=0;
				}
				stop();
				key_stack[2]=0;
				OLED_ShowString(116,32," ",24);				
				OLED_Refresh_Gram();//������ʾ��OLED				
				break;
			}
		}
		//�������ǰ��		
		while(key_stack[1])
		{
			if(t!=calendar.sec)
			{
				t=calendar.sec;
				OLED_ShowNum(20,32,calendar.hour,2,24);
				OLED_ShowNum(52,32,calendar.min,2,24);
				OLED_ShowNum(84,32,calendar.sec,2,24);
				OLED_ShowString(0,32,"<",24);			
				OLED_Refresh_Gram();//������ʾ��OLED
			}
			run_forward(10);//���
			if((KEY_Scan(0)==ADD_PRES)||(RES_SW_F==1))
			{
				if(RES_SW_F)
				{
					beep=1;
					stop();
					for(i=15;i>0;i--)//������λ�����
					{
						run_back(10);//����
					}
					beep=0;
				}
				stop();
				key_stack[1]=0;
				OLED_ShowString(0,32," ",24);
				OLED_Refresh_Gram();//������ʾ��OLED				
				break;
			}
		}	
}

/*�����¼�(����ע�估�������)*/
void minute_event()
{
		//ģ�����ע������������⣨һ����ִ��һ�Σ�
		if(m!=calendar.min)
		{
			u8 temp;
			m=calendar.min;
			if(RES_SW_F==1)
			{
				beep=1;
				for(i=15;i>0;i--)//������λ�����
				{
					run_back(10);//����
				}
				stop();
				beep=0;
			}
			if(PAin(4)==0)
				run_forward(20);
			stop();
			adcx=Get_Adc_Average(ADC_Channel_1,10);//
			temp=emg_proc_new(adcx);
			if(temp<emg_adc)
			{
				emg_adc=temp;
			}
			//�����ͱ���
			if((low_power_flag==0)&&(emg_adc<low_power_data))
			{
				for(i=0;i<4;i++)//
				{
					beep=1;
					delay_ms(100);
					beep=0;
				}
				low_power_flag=1;
			}
			//�˳�������״̬
			if((low_power_flag==1)&&(emg_adc>high_power_data))
				low_power_flag=0;

			printf("%d\n",adcx);
		}	
}

/*���Դ���*/
void text()
{
		//�ֶ�������λ���������ã�
		if(PAin(4))
		{
			stop();
			beep=1;
			for(i=15;i>0;i--)//������λ�����
			{
				run_back(10);//����
			}
			beep=0;
		}

		//�ֶ�������λ���������ã�
		if(PAin(5))
		{
			stop();
			beep=1;
			for(i=15;i>0;i--)//������λ��ǰ��
			{
				run_forward(10);//���
			}
			beep=0;
		}	
}


int main(void)
{	
//	motor_step=0;
	delay_init();	        //��ʱ��ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
 	LED_Init();				    //��ʼ��IO��
	KEY_Init();				    //��ʼ������
	beep_Init();			    //��ʼ��������
	Adc_Init();           //ADC��ʼ��
  OLED_Init();			    //��ʼ��OLED 
	motor_Init();//��ʼ���������
	uart_init(115200);    //���ڳ�ʼ��Ϊ115200	 
	usmart_dev.init(SystemCoreClock/1000000);	//��ʼ��USMART	
	RTC_Init();	  			  //RTC��ʼ��	 
	OLED_Clear(); 

	beep=0;
	
	//��flash���ݵ��뵽alarm_time_temp��������ֻ����һ�� 
	STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)alarm_time_temp,SIZE);			
	
	//��alarm_time_temp���뵽alarm_time
	alarm_time_set(1);
	
	page_welcome();
	
	while(1) 
	{	
		alarm_cmp();
		display();
		scan();
		key_event();
		minute_event();
		text();
	}		
}	  



