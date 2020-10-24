#ifndef __MOTOR_H
#define __MOTOR_H	 
#include "sys.h"

#define AIN1_1 PBout(12)	// PB12,A-	
#define AIN1_2 PBout(13)	// PB13,A+
#define BIN1_1 PBout(14)	// PB14,B-
#define BIN1_2 PBout(15)	// PB15,B+	

//extern u16 motor_step;

void motor_Init(void);//≥ı ºªØ
void run_forward(u16 time);
void run_back(u16 time);
void stop(void);


#endif

