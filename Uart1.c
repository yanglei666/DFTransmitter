#include "uart1.h"

#define FOSC   11095200
#define BRT    (65535 - FOSC/115200/4)

void Uart1Init()
{
  SCON = 0x50;     //REN=1�����н���״̬�����ڹ���ģʽ2     	                  
  AUXR |= 0X15;	   //����1Tģʽ
  TH2  = BRT>>8;
  TL2  = BRT;      // ���ò�����Ϊ115200                                                                     
  //EA   = 1;        // �����ж� 
  TI   = 1;
}