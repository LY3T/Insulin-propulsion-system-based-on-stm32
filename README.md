# 基于STM32的胰岛素推进系统

Insulin propulsion system based on stm32

## 1.OLED显示时钟，电量

## 2.带按键程序可调闹钟时间，运行时保存设置，掉电不保存

## 3.数据掉电保存到内部flash，可以进入闹钟程序

## 4.闹钟可调用步进电机，修复进制问题，如时和分的进制

## 5.闹钟时间改为两位，增加两个按键功能，强制电机前进和强制电机回退

## 6.增加闹钟单独开关，可单独控制

## 7.增加ADC电量检测（未测试），强制电机前进和回退会显示相应标志

## 8.增加前进后退的标识符，增加软件步进电机复位，建议不用，已注释，闹钟速度使用宏定义，增加两个复位开关初始化（*注：demo8精简以后版本皆以精简）

## 9.ADC电量测试，增加前进复位

## 10.增加基础注射的前复位，但存在bug,建议修复

## 11.重写复位开关检测函数，将复位开关从按键扫描程序中移除（此后按键扫描只检测四个按键，不检测复位开关），进而将复位开关改为更高响应级别的电平检测，修复“闹钟时间临时变量导入”函数中数组长度与实际的闹钟时间数组长度不一致的bug

## 12.增加闹钟的蜂鸣器提醒，电量低的蜂鸣器提醒（未测试）

## 13.改进电量处理及显示函数（更接近实时电量）

## 14.修改低电量提醒函数，修复强制电机前进及回退时屏幕标识不一致的错误