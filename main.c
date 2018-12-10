#include"Key.h"
#include"LED.h"
#include"EEPROM.h"

//默认的模式为发射强度设置模式
unsigned char nState = POWER_SET_STATE;
//默认事件0
unsigned char nEvent = 0;

unsigned char nPowerLevel = 1;  //发射强度等级
unsigned char nFreqLevel  = 1;  //发射频率等级
unsigned char nFreqLevelCur  = 1;  //发射频率等级
unsigned char nAlarmLevel = 1;  //电击强度等级

bit  bInAlarm = 0;      //进入警示档位调节提示标志位
bit  bInFreq  = 0;      //进入频率调节提示标志位
u16  nflagLastTime = 0;	//警示或者频率标志持续的时间检测

sbit Signal     = P3^5;	  //定义P35口是信号发射口
sbit bSigSwitch = P3^7;   //发射信号开关
sbit LED2 =P2^5;	     //LED2灯
sbit LED3 =P2^4;	     //LED3灯
sbit Beep =P1^0;	     //定义P10口是Beep
sbit ProtectA =P3^2;	 //保护监测引脚A
sbit ProtectB =P3^3;	 //保护监测引脚B
sbit ProtectC =P3^4;	 //保护监测引脚C

sbit Protect1 =P1^4;	 //保护监测引脚A
sbit Protect2 =P1^5;	 //保护监测引脚B
sbit Protect3 =P4^4;	 //保护监测引脚C
u16  nErrorLastTime = 0;
	 
bit  bEnable = 1;         //发射信号使能标志
bit  bUserEvent = 0;      //用户事件标志位
bit  bSafeProtect = 1;    //安全标志位
bit  bSendCode  = 0;      //发送特征码的标志位
bit  bSetCode  = 0;       //使能特征码配置标志位


//发射功率对应P0指令表
unsigned char code PowerCodeTb[]={0x00,0x80,0x01,0x81,0x04,0x03,0x05,0x85,
                                  0x08,0x88,0x10,0x90,0x11,0x91,0x14,0x8d,
								  0x20,0xa0,0x18,0xa1,0x40,0xc0,0x25,0xc1,
								  0x44,0xc4,0x45,0xa9,0x31,0x31,0x31,0x31};

//获取EEPROM配置
void LoadConfig()
{
   nPowerLevel = IapReadByte(POWER_SET_ADDR);
   nFreqLevel  = IapReadByte(FREQ_SET_ADDR);
   //对数据的有效性进行判断
   if((nPowerLevel >32)||(nPowerLevel < 0))	
   {
       nPowerLevel = 1;
   }
   if((nFreqLevel >2)||(nFreqLevel <= 0))	
   {
       nFreqLevel = 1;
   }
}

//保存EEPROM配置
void SaveConfig()
{
  IapEraseSector(POWER_SET_ADDR);
  IapProgramByte(POWER_SET_ADDR,nPowerLevel);
  IapProgramByte(FREQ_SET_ADDR,nFreqLevel);
}

//获取按键事件
bit GetUserEvent()
{
	if(0 == bUserEvent)
	   return 0;
	else
	   return 1;
}

//处理功率设置模式的事件
void PowerSetHandle()
{
    unsigned char nKey  = nEvent&0x0f;
	unsigned char nMask = nEvent&0xf0;
	switch(nKey)
	{
	   	case KEY_AB_PRES:	           //AB组合键
		{
		     if(KEY_LONG == nMask)	 //长按
			{
			     printf("to Alarm \n");
				 nState =  ALARM_SET_STATE;   //切换到电击强度设置状态
				 bInAlarm = 0;      //关闭进入警示档位调节提示标志位
				 bInFreq  = 0;      //关闭进入频率调节提示标净
			}
			else if(KEY_VLONG == nMask)	 //超长按
			{
			     printf("to Freq \n");
				 nState =  FREQ_SET_STATE;    //切换到发射频率设置状态
				 bInAlarm = 0;      //关闭进入警示档位调节提示标志位
				 bInFreq  = 0;      //关闭进入频率调节提示标净
			}
		}
		break;
		case KEY_A_PRES: 	          //A键
		{
			if(KEY_SHORT == nMask)	  //短按
			{
			   printf("power ++\n");
		   	   if(nPowerLevel<32)
			   {
			      bEnable = 1;
			      nPowerLevel++;
			      P0 = PowerCodeTb[nPowerLevel-1];
				  SaveConfig();
			   }
			}
		}
		break;
		case KEY_B_PRES: 	          //B键
		{
			if(KEY_SHORT == nMask)	  //短按
			{
			  printf("power --\n");
		   	  if(nPowerLevel>=1)
			  {
			     nPowerLevel--;
				 SaveConfig();
				 if(nPowerLevel == 0) //0档位就直接关闭
				 {
					 bEnable = 0;
				 }
				 else
				 {
				    P0 = PowerCodeTb[nPowerLevel-1];
				 }
			  }	 
			}
		}
		break;
		default:
		break; 
	}
}

//处理频率设置模式的事件
void FreqSetHandle()
{
    unsigned char nKey  = nEvent&0x0f;
	unsigned char nMask = nEvent&0xf0;
	switch(nKey)
	{
		case KEY_A_PRES: 	   //A键
		{
			if(KEY_SHORT == nMask)	  //短按
			{
			      printf("freq ++\n");
			   	  if(nFreqLevel == 1)
				  {
				     nFreqLevel = 2;
					 SaveConfig();
				  }
				  else
				  {
				     nFreqLevel = 1;
					 SaveConfig();
				  }
			}
		}
		break;
		case KEY_B_PRES: 	   //B键
		{
			if(KEY_SHORT == nMask)	  //短按
			{
			    printf("to Normal \n");
			   	nState =  POWER_SET_STATE;   //切换到发射功率设置状态
			}
		}
		break;
		default:
		break; 
	}
}

//处理警示强度设置模式的事件
void AlarmSetHandle()
{
    unsigned char nKey  = nEvent&0x0f;
	unsigned char nMask = nEvent&0xf0;
	switch(nKey)
	{
		case KEY_A_PRES: 	          //A键
		{
			if(KEY_SHORT == nMask)	  //短按
			{
			   	 bSendCode = 1;       //发送特征码标志位置位
				 printf("Send a Code Signal \n");
			}
		}
		break;
		case KEY_B_PRES: 	          //B键
		{
			if(KEY_SHORT == nMask)	  //短按
			{
			   	nState =  POWER_SET_STATE;   //切换到发射功率设置状态
				printf("to Normal \n");
			}
		}
		break;
		default:
		break; 
	}
}

//用户交互处理任务
void UserTask()
{
	 switch(nState)
	 {
		case POWER_SET_STATE:
		{
			PowerSetHandle();  //处理功率设置模式的事件
		}
		break;
		case FREQ_SET_STATE: 
		{
			FreqSetHandle();   //处理频率设置模式的事件
		}
		break;
		case ALARM_SET_STATE: 
		{
			AlarmSetHandle();  //处理警示强度设置模式的事件
		}
		break;
		default:
		break;  
	 }
}

//系统任务
void SystemTask()
{
	  if(!bSafeProtect)	 //看看是否有异常系统事件发生
	  {
	      nState =  ERROR_STATE;   //切换到Error状态
	  }
	  else
	  {
	      //如果异常解除了，则恢复到默认的发射功率设置模式
	      if(nState == ERROR_STATE)
	          nState = 	POWER_SET_STATE;
	  }
}

//状态指示任务
void StatusTask()
{
	 switch(nState)
	 {
		case POWER_SET_STATE:
		{
		    StopT3();
			LED2 = 1;
			LED3 = 1;
		}
		break;
		case FREQ_SET_STATE: 
		{
		    StopT3();
			LED2 = 1;
			LED3 = 0;
		}
		break;
		case ALARM_SET_STATE: 
		{
		    StopT3();
			LED2 = 0;
			LED3 = 1;
		}
		break;
		case ERROR_STATE:
		{
		    StartT3();
			LED2 = 0;
			LED3 = 0;
		}
		break;
		default:
		break;  
	 }
}
//数码管显示任务
void DigLEDTask()
{
	 switch(nState)
	 {
		case POWER_SET_STATE:
		{
		     nflagLastTime++;
			 if(bInAlarm)      //如果检测到警示标志
			 {
				 if(nflagLastTime <= 100)	  //数码管00和关闭交替出现
				 {
				    WriteLEDData16(0,0);
				 }
			     else
				 {
				    WriteLEDData16(17,17);
				 }
			 }
			 else if(bInFreq)  //如果检测到频率标志
			 {
			     if(nflagLastTime <= 100)	  //数码管01和关闭交替出现
				 {
				    WriteLEDData16(0,1);
				 }
			     else
				 {
				    WriteLEDData16(17,17);
				 }
			 }
			 else
			 {
			      WriteLEDData16(nPowerLevel/10,nPowerLevel%10);
			 }
			 
			 if(nflagLastTime == 200)
			     nflagLastTime = 0; 	 
		}
		break;
		case FREQ_SET_STATE: 
		{
			 WriteLEDData16(0,nFreqLevel);
		}
		break;
		case ALARM_SET_STATE: 
		{
			 WriteLEDData16(16,16);	  //数码管显示PP
		}
		break;
		case ERROR_STATE:
		{
		     nErrorLastTime++;
			 if(nErrorLastTime <= 100)	  //异常状态数码管88和关闭交替出现
			 {
			    WriteLEDData16(8,8);
			 }
		     else
			 {
			    WriteLEDData16(17,17);
			 }

			 if(nErrorLastTime == 200)
			     nErrorLastTime = 0; 
		}
		break;
		default:
		break;  
	 }
}
 
void main()
{
   //系统时钟2分频
   P_SW2 = 0x80;
   CLKDIV = 0x02;
   P_SW2 = 0x00;

   //获取EEPROM配置
   LoadConfig();
   //初始化定时器
   Timer0Init();
   Timer1Init();
   Timer3Init();
   //初始化IO
   IOInit();
   //初始化数码管
   DigLEDInit();
   //初始化串口1
   Uart1Init();
   //打开总中断
   EA = 1;

   while(1)
   {
      //处理用户操作事件
	  if(GetUserEvent())  
	  {
	     printf("get a event \n");
		 UserTask();
		 bUserEvent = 0;  //清空事件数据并且复位标志位
		 nEvent = 0;
	  }
	  //处理系统事件
	  SystemTask();
	  //处部设备显示
	  DigLEDTask();	  //数码管显示任务
	  StatusTask();	  //显示当前设备状态（LED,Beep等）
	  //printf("nPowerLevel %bd nFreqLevel %bd\n",nPowerLevel,nFreqLevel);
	  //delay_ms(10);
   }
}

/*******************************************************************************
* 函 数 名         : void Timer0() interrupt 1
* 函数功能		   : 定时器0中断函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Timer0() interrupt 1  using 1
{
	static u16 iCnt =0;	    //发射周期指针
	//static u16 iCntCode  =0;	//发送特征码指针
	static u8  iSendStep =0;	//发送特征码指针
	u8 SetTL0 = 0,SetTH0 = 0;
	u16 PWMOnCnt = 0,PWMOffCnt = 0,CheckSafePos = 0,PWMLength =0;
	u8  CodeLength = 66;   //特征码的计数  6ms/(181.6us/2)
	u16 StopLength = 330;  //特征码的计数  30ms/(181.6us/2)
	//在上一个周期完成之后 根据设置的频率档位调节定时器初值
	if(nFreqLevelCur != nFreqLevel)
	{
	    if(iCnt == 0)
		{
		   nFreqLevelCur = nFreqLevel;
		}
	}

    //选择定时器配置
	if(!bSetCode)
	{
		 if(nFreqLevelCur == 1)
		{
		   	//TL0 = 0x6C;		//设置定时初值 106us
	        //TH0 = 0xFB;		//设置定时初值
			//TL0 = 0x8D;		//设置定时初值 106us
	        //TH0 = 0xFB;		//设置定时初值
		    TL0 = (65535 - BB_FREQ)%256;		//设置定时初值 106us
	        TH0 = (65535 - BB_FREQ)/256;		//设置定时初值
			PWMOnCnt  = 30;
			PWMOffCnt = 314;
			CheckSafePos = 200;
			PWMLength = 1887;
		}
		else
		{
			//TL0 = 0x0A;		//设置定时初值 68.5us
	        //TH0 = 0xFD;		//设置定时初值
            //TL0 = 0x26;		//设置定时初值 68.5us
            //TH0 = 0xFD;		//设置定时初值
			TL0 = (65535 - CC_FREQ)%256;		//设置定时初值 106us
	        TH0 = (65535 - CC_FREQ)/256;		//设置定时初值
			PWMOnCnt  = 46;
			PWMOffCnt = 476;
			CheckSafePos = 300;
			PWMLength = 2899;
		}
	}
	else
	{
	     //TL0 = 0x12;		//设置定时初值 91us
	     //TH0 = 0xFC;		//设置定时初值
		 TL0 = (65535 - ER_FREQ)%256;		//设置定时初值 106us
	     TH0 = (65535 - ER_FREQ)/256;		//设置定时初值
	}
	
	if(bEnable)   //在使能输出的情况下
	{
		if(nState != ALARM_SET_STATE)	 //非通讯模式
		{
		    iCnt++;
			if(iCnt == PWMOnCnt)
			{
				bSigSwitch = 0;
			}
			if(iCnt == PWMOffCnt)
			{
				iCnt = 0;
				bSigSwitch = 1;
			}
			//安全保护功能
			if(iCnt == CheckSafePos)
			{
				 if(ProtectA == 0)
				 {
				    bSafeProtect = 0;
				 } 	
				 else
				 {
				    bSafeProtect = 1;
				 }	    
			}	
		}
		else
		{
		     //判断是否有发送特征码的请求	 
		     if(bSendCode)
			 {
				//分步输出脉冲
				switch(iSendStep)
				{
				    case 0:	//第一步，先打开与门
					{
						bSigSwitch = 1;
						iSendStep  = 1;   //跳转输出警示信号
					}
				    case 1: //第二步，输出200ms的警示信号
					{
						/*if(iCnt == PWMOnCnt)
						{
							bSigSwitch = 0;
						}
						if(iCnt == PWMOffCnt)
						{
							iCnt = 0;
							bSigSwitch = 1;
						}*/
						if(iCnt == PWMLength)
						{
						    //printf("iCntCode %d PWMLength %d \n",iCntCode,PWMLength);
							iCnt = 0;
							//iCntCode = 0;
							bSigSwitch = 0;	  //打开开关 准备输出6ms特征码
							iSendStep  = 2;   //跳转到下一步发送特征码
							bSetCode   = 1;   //使能特征码周期配置标志位
						}
					}
					break;
					case 2:	//第三步，开始输出30ms的停止信号
					{
					    bSigSwitch = 0;	     //打开开关 准备输出6ms特征码
						iSendStep  = 3;      //跳转输出警示信号
						iCnt = 0;            //重新开始计数
					}
					break;
				    case 3:	//第四步，停止输出30ms的停止信号
					{
						if(iCnt == StopLength)
						{
						    //结束发送
							iSendStep  = 4;
						}
					}
					break;
					case 4:	//第四步，开始输出6ms的特征码
					{
					    bSigSwitch = 1;	     //打开开关 准备输出6ms特征码
						iSendStep  = 5;      //跳转输出警示信号
						iCnt = 0;            //重新开始计数
					}
					break;
				    case 5:	//第五步，停止输出6ms的特征码
					{
						if(iCnt == CodeLength)
						{
						    //结束发送
							iCnt = 0;
							//iCntCode = 0;
							bSigSwitch = 0;
							bSendCode  = 0;	 //清除发送特征码请求
							iSendStep  = 0;
							bSetCode   = 0;  //关闭特征码周期配置标志位 
						}
					}
					break;
				}
				iCnt++;
			    //iCntCode++;
			 }
			 else
			 {
			    //没有发送请求，关闭发射 ,标志位清0
			    bSigSwitch = 0;
				iCnt = 0;
				//iCntCode = 0;
				iSendStep = 0;
			 }   
		}
	}	
	else
	{
	    bSigSwitch = 0;  //未使能，关闭输出
		iCnt = 0;
	}
}


/*******************************************************************************
* 函 数 名         : void Timer1() interrupt 3
* 函数功能		   : 定时器1中断函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Timer1() interrupt 3  using 1
{
	//static u16 j =0;
	u8 	nUserEventTmp = 0;		
	//j++;

	//printf("Timer1 is running\n");
	//按键扫描
	nUserEventTmp = Key_Scan();
	if((nUserEventTmp != 0)&&(!bUserEvent))
	{
	   //printf("send a event \n");
	   nEvent = nUserEventTmp;	//传递数据并且置位标志位
	   bUserEvent = 1;  
	}

	//Protect1 = ~Protect1;	 //保护监测引脚A
	//Protect2 = ~Protect2;	 //保护监测引脚B
	//Protect3 = ~Protect3;	 //保护监测引脚C
}

void Timer3() interrupt 19  using 1
{
	Beep   = ~Beep;
}