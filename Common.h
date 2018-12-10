#ifndef COMMON_H
#define COMMON_H
#include "STC\stc8.h"
#include "stdio.h"
#include <intrins.h>
//状态(模式)定义
#define POWER_SET_STATE	   0x01 //发射强度设置模式
#define FREQ_SET_STATE	   0x02 //发射频率设置模式
#define ALARM_SET_STATE	   0x04 //电击强度设置模式
#define ERROR_STATE	       0x08 //异常模式

#define POWER_SET_ADDR	   0x0000 //发射强度设置EEPROM地址
#define FREQ_SET_ADDR	   0x0001 //发射频率设置EEPROM地址

//发射频率等级
#define BB_FREQ (103*11059200/1000000)
#define CC_FREQ (66*11059200/1000000)
#define ER_FREQ (89*11059200/1000000)

typedef unsigned int  u16;	  //对数据类型进行声明定义
typedef unsigned char u8;

void delay_ms(unsigned int n);
void delay_us(unsigned int n);
#endif