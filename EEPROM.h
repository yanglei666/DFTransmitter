#ifndef EEPROM_H
#define EEPROM_H
#include "Common.h"

#define CMD_IDLE    0		 //����ģʽ
#define CMD_READ    1		 //IAP�ֽڶ�����
#define CMD_PROGRAM 2		 //IAP�ֽڱ������
#define CMD_ERASE   3		 //IAP�ֽڲ�������

#define ENABLE_IAP  0X82     //if SYSCLK<20MHz

//���Ե�ַ
#define IAP_ADDRESS 0X0000	 //���ֲ��Ͽ��Կ��� STC15F2K60S2��EEPROM��ַ��0x0000- 0x03ff	������1k�ĵ�ַ�ռ�
 
void IapIdle();
u8 IapReadByte(u16 addr);
void IapProgramByte(u16 addr,u8 dat);
void IapEraseSector(u16 addr);

#endif