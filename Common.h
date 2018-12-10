#ifndef COMMON_H
#define COMMON_H
#include "STC\stc8.h"
#include "stdio.h"
#include <intrins.h>
//״̬(ģʽ)����
#define POWER_SET_STATE	   0x01 //����ǿ������ģʽ
#define FREQ_SET_STATE	   0x02 //����Ƶ������ģʽ
#define ALARM_SET_STATE	   0x04 //���ǿ������ģʽ
#define ERROR_STATE	       0x08 //�쳣ģʽ

#define POWER_SET_ADDR	   0x0000 //����ǿ������EEPROM��ַ
#define FREQ_SET_ADDR	   0x0001 //����Ƶ������EEPROM��ַ

//����Ƶ�ʵȼ�
#define BB_FREQ (103*11059200/1000000)
#define CC_FREQ (66*11059200/1000000)
#define ER_FREQ (89*11059200/1000000)

typedef unsigned int  u16;	  //���������ͽ�����������
typedef unsigned char u8;

void delay_ms(unsigned int n);
void delay_us(unsigned int n);
#endif