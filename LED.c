#include"LED.h"
unsigned char code LED[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0x73,0x00};

sbit SER = P1^4;    //p3.4�ſ��ƴ�����������
sbit SCK = P1^5;    //��������ʱ��
sbit RCK = P4^4;    //��������ʱ��

void DigLEDInit()
{
   //��ʼ�������ź�
   SER = 0;    
   SCK = 0;    
   RCK = 0;
}
void WriteLEDData8(unsigned char nData,bit bIsShi)
{
        char i=0;
        //��ȷ����ʮλ���Ǹ�λ
		if(bIsShi)
		{
		  SER = 1;               
          SCK = 1;           //�����أ��ô�������ʱ�ӱ�Ϊ�ߵ�ƽ������ʱ2��ʱ������        
		  _nop_();
          _nop_();
           SCK = 0;          //��Ϊ�͵�ƽ��Ϊ�´�׼�� 
		  _nop_();
          _nop_();

		  SER = 0;               
          SCK = 1;          //�����أ��ô�������ʱ�ӱ�Ϊ�ߵ�ƽ������ʱ2��ʱ������        
		  _nop_();
          _nop_();
           SCK = 0;          //��Ϊ�͵�ƽ��Ϊ�´�׼�� 
		  _nop_();
          _nop_();
		}
		else
		{
		  SER = 0;               
          SCK = 1;           //�����أ��ô�������ʱ�ӱ�Ϊ�ߵ�ƽ������ʱ2��ʱ������        
		  _nop_();
          _nop_();
           SCK = 0;          //��Ϊ�͵�ƽ��Ϊ�´�׼�� 
		  _nop_();
          _nop_();

		  SER = 1;               
          SCK = 1;          //�����أ��ô�������ʱ�ӱ�Ϊ�ߵ�ƽ������ʱ2��ʱ������        
		  _nop_();
          _nop_();
           SCK = 0;          //��Ϊ�͵�ƽ��Ϊ�´�׼�� 
		  _nop_();
          _nop_();
		}

		//��������
	    for(;i<8;i++)
	   {
	        SER  = nData>>7;         //����Լ�������2��
	        nData= nData<<1;      
	
	        SCK = 1;          //�����أ��ô�������ʱ�ӱ�Ϊ�ߵ�ƽ������ʱ2��ʱ������        
			_nop_();
	        _nop_();
	        SCK = 0;          //��Ϊ�͵�ƽ��Ϊ�´�׼��
			_nop_();
	        _nop_();   
	   }  
	   
	   /*λ�ƼĴ�������׼�����,ת�Ƶ��洢�Ĵ���*/
	   RCK = 1;         //�����أ��ô洢�Ĵ���ʱ�ӱ�Ϊ�ߵ�ƽ������ʱ2��ʱ������   
	   _nop_();
	   _nop_();
	   RCK = 0;
	   SER = 0; 
}

void WriteLEDData16(unsigned char nShiData,unsigned char nGeData)
{
	  WriteLEDData8(LED[nShiData],1);
	  delay_ms(1);
	  WriteLEDData8(LED[nGeData],0);
	  delay_ms(1);
}
