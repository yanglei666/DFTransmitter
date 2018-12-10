#include"LED.h"
unsigned char code LED[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0x73,0x00};

sbit SER = P1^4;    //p3.4脚控制串行数据输入
sbit SCK = P1^5;    //串行输入时钟
sbit RCK = P4^4;    //锁存输入时钟

void DigLEDInit()
{
   //初始化拉低信号
   SER = 0;    
   SCK = 0;    
   RCK = 0;
}
void WriteLEDData8(unsigned char nData,bit bIsShi)
{
        char i=0;
        //先确定是十位还是个位
		if(bIsShi)
		{
		  SER = 1;               
          SCK = 1;           //上升沿，让串行输入时钟变为高电平，并延时2个时钟周期        
		  _nop_();
          _nop_();
           SCK = 0;          //变为低电平，为下次准备 
		  _nop_();
          _nop_();

		  SER = 0;               
          SCK = 1;          //上升沿，让串行输入时钟变为高电平，并延时2个时钟周期        
		  _nop_();
          _nop_();
           SCK = 0;          //变为低电平，为下次准备 
		  _nop_();
          _nop_();
		}
		else
		{
		  SER = 0;               
          SCK = 1;           //上升沿，让串行输入时钟变为高电平，并延时2个时钟周期        
		  _nop_();
          _nop_();
           SCK = 0;          //变为低电平，为下次准备 
		  _nop_();
          _nop_();

		  SER = 1;               
          SCK = 1;          //上升沿，让串行输入时钟变为高电平，并延时2个时钟周期        
		  _nop_();
          _nop_();
           SCK = 0;          //变为低电平，为下次准备 
		  _nop_();
          _nop_();
		}

		//发送数据
	    for(;i<8;i++)
	   {
	        SER  = nData>>7;         //大家自己考量这2句
	        nData= nData<<1;      
	
	        SCK = 1;          //上升沿，让串行输入时钟变为高电平，并延时2个时钟周期        
			_nop_();
	        _nop_();
	        SCK = 0;          //变为低电平，为下次准备
			_nop_();
	        _nop_();   
	   }  
	   
	   /*位移寄存器数据准备完毕,转移到存储寄存器*/
	   RCK = 1;         //上升沿，让存储寄存器时钟变为高电平，并延时2个时钟周期   
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
