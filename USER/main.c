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

//默认三个闹钟的注射速度为40，用户可修改
#define alarm0_speed 40
#define alarm1_speed 40
#define alarm2_speed 40

//电量最低值（比较值）
#define emg_temp 2920

//低电量定义值(%)
#define low_power_data 10

//高电量定义值(%)
#define high_power_data 50

//Mini STM32开发板范例代码9
//OLED显示 实验
//正点原子@ALIENTEK
//技术论坛:www.openedv.com	

u8 clear_flag=1;//清屏标志 

u8 interset_flag=0;//进入标志

u8 on_off[2][4]={"off"," on"};//在定义时候，2代表数组中有几个字符串，4代表每个字符串的长度不要超过3,因为字符串在后面自动加了\0这个字符 

u8 alarm_flag[3]={1,1,1};//闹钟标志，对应闹钟0~2

u8 save_key=0;//保存设置时的键值临时变量

u8 timeset=0;//设置时的循环变量

u8 low_power_flag=0;//电量低标志

u8 i;//for循环变量

u8 t=0,m=0;//秒，分的临时变量

u16 adcx=4600;

u8 emg_adc=100;

u16 temp=0;//键值的临时变量

u8 key_stack[4]={0,0,0,0};//键值堆栈
u8 key_temp[4]={0,0,0,0};//键值临时数组

//闹钟时间显示缓存，0~5位时间，6~8位注射时间，9~11位闹钟开启标志，下同
u8 alarm_time[12]={9,30,12,30,18,30,10,10,10,1,1,1};
//闹钟时间临时存放数组
u8 alarm_time_temp[12]={9,30,12,30,18,30,10,10,10,1,1,1};

#define SIZE sizeof(alarm_time_temp)		//数组长度
#define FLASH_SAVE_ADDR  0X08070000		//设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)

//OLED画线函数
void myOLED_DrawLine(u8 x,u8 y,u8 num)
{
	for(;num>0;num--)
	{
		OLED_DrawPoint(x++,y,1);	
	}	
}





//电量处理函数（新）
u8 emg_proc_new(u16 adcx)
{
	u8 emg_data;
	emg_data=(adcx-emg_temp)/10;
	return emg_data;
}

//蜂鸣器报警
//mode:1 多次响，delay_time:每次持续的时间，warning_time:响的次数
//mode:0 单次响，delay_time:持续的时间，warning_time:无用
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

//进制转换
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


//闹钟时间临时变量导入
//mode=0,alarm_time导入alarm_time_temp
//mode=1,alarm_time_temp导入alarm_time
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

//开机欢迎页
void page_welcome()
{
	OLED_ShowString(24,16,"Welcome",24);
	OLED_Refresh_Gram();//更新显示到OLED
	delay_ms(1800);
	delay_ms(1800);	
	OLED_Clear();
}


//设置页面
void page_timeset()
{
	OLED_ShowString(0,0,"alarm time set:",16);
	OLED_ShowString(0,16,"1/",16);
	OLED_ShowString(0,32,"2/",16);
	OLED_ShowString(0,48,"3/",16);
	
	//闹钟时间
	OLED_ShowNum(24,16,alarm_time[0],2,16);	
	OLED_ShowNum(48,16,alarm_time[1],2,16);
	OLED_ShowNum(24,32,alarm_time[2],2,16);
	OLED_ShowNum(48,32,alarm_time[3],2,16);
	OLED_ShowNum(24,48,alarm_time[4],2,16);
	OLED_ShowNum(48,48,alarm_time[5],2,16);
	
	//注射时间
	OLED_ShowNum(72,16,alarm_time[6],2,16);
	OLED_ShowNum(72,32,alarm_time[7],2,16);
	OLED_ShowNum(72,48,alarm_time[8],2,16);	
	
	//闹钟开关标志
	OLED_ShowString(104,16,on_off[alarm_time[9]],16);
	OLED_ShowString(104,32,on_off[alarm_time[10]],16);
	OLED_ShowString(104,48,on_off[alarm_time[11]],16);
	
	//冒号
	OLED_ShowString(40,16,":",16);
	OLED_ShowString(40,32,":",16);
	OLED_ShowString(40,48,":",16);
	
	//秒
	OLED_ShowString(88,16,"s",16);
	OLED_ShowString(88,32,"s",16);
	OLED_ShowString(88,48,"s",16);	
	
	//下划线
	switch(timeset)
	{
		//闹钟时间		
		case(0):myOLED_DrawLine(24,32,16);break;//
		case(1):myOLED_DrawLine(48,32,16);break;//  
		case(2):myOLED_DrawLine(24,48,16);break;//
		case(3):myOLED_DrawLine(48,48,16);break;//
		case(4):myOLED_DrawLine(24,63,16);break;//
		case(5):myOLED_DrawLine(48,63,16);break;//
		
		//注射时间
		case(6):myOLED_DrawLine(72,32,16);break;//
		case(7):myOLED_DrawLine(72,48,16);break;//	
		case(8):myOLED_DrawLine(72,63,16);break;//	
		
		//闹钟标志
		case(9):myOLED_DrawLine(104,32,23);break;//
		case(10):myOLED_DrawLine(104,48,23);break;//	
		case(11):myOLED_DrawLine(104,63,23);break;//	
		
	}	
}

//保存提示弹窗
void page_save_ornot()
{
	if(clear_flag)
	{
		clear_flag=0;
		OLED_Clear();//清屏
	}
	OLED_ShowString(8,0,"Do you want to ",16);
	OLED_ShowString(0,16,"save the change?",16);
	OLED_ShowString(0,48,"no           yes",16);
	OLED_Refresh_Gram();//更新显示到OLED
}

//闹钟函数，可控制步进电机
//m:闹钟标识0~2
//time:持续时间(s)
//speed:电机速度
void page_alarm_time_up(u8 m,u8 time,u8 speed)
{
	u8 k;
	
	//蜂鸣器提醒
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
			OLED_Clear();//清屏
		}
		
		OLED_ShowString(16,16,"injecting...",16);		
		
		if(t!=calendar.sec)
		{
			t=calendar.sec;
			
			OLED_ShowString(20,36,"   :   :   ",16);			
			
			OLED_ShowNum(20,32,calendar.hour,2,24);
			OLED_ShowNum(52,32,calendar.min,2,24);
			OLED_ShowNum(84,32,calendar.sec,2,24);
			
			OLED_Refresh_Gram();//更新显示到OLED			
		}
		run_forward(speed);
		if(((calendar.sec-k)>=time)||RES_SW_F==1)
		{
			if(RES_SW_F)
			{
				beep=1;
				stop();
				for(i=15;i>0;i--)//触发复位后回退
				{
					run_back(10);//快退
				}
				beep=0;
			}			
			stop();
			//alarm_time[m+9]=0;//alarm_time_temp[m+9]=0;
			clear_flag=1;
			break;
		}					
	}
	
	//蜂鸣器提醒
	beep=1;
	delay_ms(100);
	beep=0;
	delay_ms(100);	
	beep=1;
	delay_ms(100);
	beep=0;	
	
	OLED_Clear();
}

/*闹钟判断*/
void alarm_cmp()
{
		//闹钟判断程序
		//闹钟一
		if((alarm_time_temp[0]==calendar.hour)&&
			(alarm_time_temp[1]==calendar.min)&&
		((calendar.sec==0)||(calendar.sec==1))&&
		(alarm_time_temp[9]))
		{
			page_alarm_time_up(0,alarm_time_temp[6],alarm0_speed);
		}

		//闹钟二
		if((alarm_time_temp[2]==calendar.hour)&&
			(alarm_time_temp[3]==calendar.min)&&
		((calendar.sec==0)||(calendar.sec==1))&&
		(alarm_time_temp[10]))
		{
			page_alarm_time_up(1,alarm_time_temp[7],alarm1_speed);
		}		

		//闹钟三
		if((alarm_time_temp[4]==calendar.hour)&&
			(alarm_time_temp[5]==calendar.min)&&
		((calendar.sec==0)||(calendar.sec==1))&&
		(alarm_time_temp[11]))
		{
			page_alarm_time_up(2,alarm_time_temp[8],alarm2_speed);
		}
						
		
		//闹钟日重置
//		if((calendar.hour==0)&&(calendar.min==0)&&
//		((calendar.sec==0)||(calendar.sec==1)||(calendar.sec==2)))
//			alarm_time_set(1);	
}

/*主显示函数*/
void display()
{
		//时钟及电量显示
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

		//电量显示
//  u8 emg_proc_new(u16 adcx)
		OLED_ShowNum(96,0,emg_adc,3,16);
		OLED_ShowString(120,0,"%",16);

		
		OLED_Refresh_Gram();//更新显示到OLED
		}	
}

/*按键扫描*/
void scan()
{
		temp=KEY_Scan(0);
		key_stack[temp-1]=1;	
}

/*按键事件*/
void key_event()
{
		while(key_stack[0])	//检测OK键
		{
			//清屏
			if(clear_flag)
			{
				clear_flag=0;
				OLED_Clear();		
				//导入显示缓存
				alarm_time_set(1);
			}
			
			//布局设置界面
			page_timeset();
			//更新OLED
			OLED_Refresh_Gram();//更新显示到OLED

			//预处理，取键值堆栈到键值缓存堆栈
			for(i=0;i<4;i++)
			{
				key_temp[i]=key_stack[i];
			}
			//按键扫描
			temp=KEY_Scan(0);
			key_stack[temp-1]++;
			
			//退出判断，键值堆栈清零
			if(temp==RES_PRES)
			{
				for(i=0;i<4;i++)
					key_stack[i]=0;
				clear_flag=1;
				interset_flag=1;				
				OLED_Clear();
				break;
			}
			
			//时间值遍历
			if((key_stack[0]-key_temp[0])>=1)
			{
				timeset++;
				if(timeset==12)
					timeset=0;
			}
			//加
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
			//减
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
		//是否保存
		while(interset_flag)
		{
			page_save_ornot();
			save_key=KEY_Scan(0);
			if(save_key==OK_PRES)
			{
				interset_flag=0;
				clear_flag=1;
				alarm_time_set(0);
				//保存到flash
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
		//步进电机后退
		while(key_stack[2])
		{
			if(t!=calendar.sec)
			{
				t=calendar.sec;
				OLED_ShowNum(20,32,calendar.hour,2,24);
				OLED_ShowNum(52,32,calendar.min,2,24);
				OLED_ShowNum(84,32,calendar.sec,2,24);
				OLED_ShowString(116,32,">",24);				
				OLED_Refresh_Gram();//更新显示到OLED
			}
			run_back(10);//快退
			if((KEY_Scan(0)==SUB_PRES)||(RES_SW_B==1))
			{				
				if(RES_SW_B==1)
				{
					beep=1;
					stop();
					for(i=13;i>0;i--)//触发复位后回退
					{
						run_forward(10);//快进
					}
					beep=0;
				}
				stop();
				key_stack[2]=0;
				OLED_ShowString(116,32," ",24);				
				OLED_Refresh_Gram();//更新显示到OLED				
				break;
			}
		}
		//步进电机前进		
		while(key_stack[1])
		{
			if(t!=calendar.sec)
			{
				t=calendar.sec;
				OLED_ShowNum(20,32,calendar.hour,2,24);
				OLED_ShowNum(52,32,calendar.min,2,24);
				OLED_ShowNum(84,32,calendar.sec,2,24);
				OLED_ShowString(0,32,"<",24);			
				OLED_Refresh_Gram();//更新显示到OLED
			}
			run_forward(10);//快进
			if((KEY_Scan(0)==ADD_PRES)||(RES_SW_F==1))
			{
				if(RES_SW_F)
				{
					beep=1;
					stop();
					for(i=15;i>0;i--)//触发复位后回退
					{
						run_back(10);//快退
					}
					beep=0;
				}
				stop();
				key_stack[1]=0;
				OLED_ShowString(0,32," ",24);
				OLED_Refresh_Gram();//更新显示到OLED				
				break;
			}
		}	
}

/*分钟事件(基础注射及电量检测)*/
void minute_event()
{
		//模拟基础注射量及电量检测（一分钟执行一次）
		if(m!=calendar.min)
		{
			u8 temp;
			m=calendar.min;
			if(RES_SW_F==1)
			{
				beep=1;
				for(i=15;i>0;i--)//触发复位后回退
				{
					run_back(10);//快退
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
			//电量低报警
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
			//退出电量低状态
			if((low_power_flag==1)&&(emg_adc>high_power_data))
				low_power_flag=0;

			printf("%d\n",adcx);
		}	
}

/*调试代码*/
void text()
{
		//手动触发复位（仅测试用）
		if(PAin(4))
		{
			stop();
			beep=1;
			for(i=15;i>0;i--)//触发复位后回退
			{
				run_back(10);//快退
			}
			beep=0;
		}

		//手动触发复位（仅测试用）
		if(PAin(5))
		{
			stop();
			beep=1;
			for(i=15;i>0;i--)//触发复位后前进
			{
				run_forward(10);//快进
			}
			beep=0;
		}	
}


int main(void)
{	
//	motor_step=0;
	delay_init();	        //延时初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
 	LED_Init();				    //初始化IO口
	KEY_Init();				    //初始化按键
	beep_Init();			    //初始化蜂鸣器
	Adc_Init();           //ADC初始化
  OLED_Init();			    //初始化OLED 
	motor_Init();//初始化步进电机
	uart_init(115200);    //串口初始化为115200	 
	usmart_dev.init(SystemCoreClock/1000000);	//初始化USMART	
	RTC_Init();	  			  //RTC初始化	 
	OLED_Clear(); 

	beep=0;
	
	//将flash数据导入到alarm_time_temp，开机后只导入一次 
	STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)alarm_time_temp,SIZE);			
	
	//将alarm_time_temp导入到alarm_time
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



