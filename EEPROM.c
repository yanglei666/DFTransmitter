#include"EEPROM.h"
//�ر�IAP
void IapIdle()
{
	  IAP_CONTR=0;		 //�ر�IAP����
	  IAP_CMD  =0;		 //�������Ĵ���
	  IAP_TRIG =0;		 //��������Ĵ���
	  IAP_ADDRH=0X80;	 //����ַ���õ���IAP����
	  IAP_ADDRL=0;
}

//��ISP/IAP/EEPROM�����ȡһ���ֽ�
u8 IapReadByte(u16 addr)
{
	 u8 dat;				 //���ݻ�����
	 IAP_CONTR=ENABLE_IAP;	 //ʹ��IAP	   ͬʱ���õȴ�ʱ��
	 IAP_CMD=CMD_READ;		 //����IAP����
	 IAP_ADDRL=addr;		 //����IAP�͵�ַ
	 IAP_ADDRH=addr>>8;		 //����IAP�ߵ�ַ
	 IAP_TRIG=0X5a;			 //д�������0x5a��	 д���������Ժ�  ����Ż���Ч �ֲ�713ҳ
	 IAP_TRIG=0Xa5;			 //д�������0xa5��
	 delay_us(10);				 //�ȴ�ISP/IAP/EEPROM�������
	 dat=IAP_DATA;			 //��ISP/IAP/EEPROM����
	 IapIdle();				 //�ر�IAP����
	
	 return dat;			 //����
}

//дһ���ֽ����ݵ�ISP/IAP/EEPROM����
void IapProgramByte(u16 addr,u8 dat)
{
	 IAP_CONTR=ENABLE_IAP;	 //ʹ��IAP	   ͬʱ���õȴ�ʱ��
	 IAP_CMD=CMD_PROGRAM;	 //����IAP����
	 IAP_ADDRL=addr;		 //����IAP�͵�ַ
	 IAP_ADDRH=addr>>8;		 //����IAP�ߵ�ַ
	 IAP_DATA=dat;			 //дISP/IAP/EEPROM����
	 IAP_TRIG=0X5a;			 //д�������0x5a��    д���������Ժ�  ����Ż���Ч �ֲ�713ҳ
	 IAP_TRIG=0Xa5;			 //д�������0xa5��
	 delay_us(10);				 //�ȴ�ISP/IAP/EEPROM�������
	 IapIdle();				 //�ر�IAP����  
}

//��������
void IapEraseSector(u16 addr)
{
	 IAP_CONTR=ENABLE_IAP;	 //ʹ��IAP	 ͬʱ���õȴ�ʱ��
	 IAP_CMD=CMD_ERASE;	     //����IAP����
	 IAP_ADDRL=addr;		 //����IAP�͵�ַ
	 IAP_ADDRH=addr>>8;		 //����IAP�ߵ�ַ
	 IAP_TRIG=0X5a;			 //д�������0x5a��	 д���������Ժ�  ����Ż���Ч �ֲ�713ҳ
	 IAP_TRIG=0Xa5;			 //д�������0xa5��
	 delay_us(10);				 //�ȴ�ISP/IAP/EEPROM�������
	 IapIdle();				 //�ر�IAP���� 
}