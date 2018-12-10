#include"Key.h"
#include"LED.h"
#include"EEPROM.h"

//Ĭ�ϵ�ģʽΪ����ǿ������ģʽ
unsigned char nState = POWER_SET_STATE;
//Ĭ���¼�0
unsigned char nEvent = 0;

unsigned char nPowerLevel = 1;  //����ǿ�ȵȼ�
unsigned char nFreqLevel  = 1;  //����Ƶ�ʵȼ�
unsigned char nFreqLevelCur  = 1;  //����Ƶ�ʵȼ�
unsigned char nAlarmLevel = 1;  //���ǿ�ȵȼ�

bit  bInAlarm = 0;      //���뾯ʾ��λ������ʾ��־λ
bit  bInFreq  = 0;      //����Ƶ�ʵ�����ʾ��־λ
u16  nflagLastTime = 0;	//��ʾ����Ƶ�ʱ�־������ʱ����

sbit Signal     = P3^5;	  //����P35�����źŷ����
sbit bSigSwitch = P3^7;   //�����źſ���
sbit LED2 =P2^5;	     //LED2��
sbit LED3 =P2^4;	     //LED3��
sbit Beep =P1^0;	     //����P10����Beep
sbit ProtectA =P3^2;	 //�����������A
sbit ProtectB =P3^3;	 //�����������B
sbit ProtectC =P3^4;	 //�����������C

sbit Protect1 =P1^4;	 //�����������A
sbit Protect2 =P1^5;	 //�����������B
sbit Protect3 =P4^4;	 //�����������C
u16  nErrorLastTime = 0;
	 
bit  bEnable = 1;         //�����ź�ʹ�ܱ�־
bit  bUserEvent = 0;      //�û��¼���־λ
bit  bSafeProtect = 1;    //��ȫ��־λ
bit  bSendCode  = 0;      //����������ı�־λ
bit  bSetCode  = 0;       //ʹ�����������ñ�־λ


//���书�ʶ�ӦP0ָ���
unsigned char code PowerCodeTb[]={0x00,0x80,0x01,0x81,0x04,0x03,0x05,0x85,
                                  0x08,0x88,0x10,0x90,0x11,0x91,0x14,0x8d,
								  0x20,0xa0,0x18,0xa1,0x40,0xc0,0x25,0xc1,
								  0x44,0xc4,0x45,0xa9,0x31,0x31,0x31,0x31};

//��ȡEEPROM����
void LoadConfig()
{
   nPowerLevel = IapReadByte(POWER_SET_ADDR);
   nFreqLevel  = IapReadByte(FREQ_SET_ADDR);
   //�����ݵ���Ч�Խ����ж�
   if((nPowerLevel >32)||(nPowerLevel < 0))	
   {
       nPowerLevel = 1;
   }
   if((nFreqLevel >2)||(nFreqLevel <= 0))	
   {
       nFreqLevel = 1;
   }
}

//����EEPROM����
void SaveConfig()
{
  IapEraseSector(POWER_SET_ADDR);
  IapProgramByte(POWER_SET_ADDR,nPowerLevel);
  IapProgramByte(FREQ_SET_ADDR,nFreqLevel);
}

//��ȡ�����¼�
bit GetUserEvent()
{
	if(0 == bUserEvent)
	   return 0;
	else
	   return 1;
}

//����������ģʽ���¼�
void PowerSetHandle()
{
    unsigned char nKey  = nEvent&0x0f;
	unsigned char nMask = nEvent&0xf0;
	switch(nKey)
	{
	   	case KEY_AB_PRES:	           //AB��ϼ�
		{
		     if(KEY_LONG == nMask)	 //����
			{
			     printf("to Alarm \n");
				 nState =  ALARM_SET_STATE;   //�л������ǿ������״̬
				 bInAlarm = 0;      //�رս��뾯ʾ��λ������ʾ��־λ
				 bInFreq  = 0;      //�رս���Ƶ�ʵ�����ʾ�꾻
			}
			else if(KEY_VLONG == nMask)	 //������
			{
			     printf("to Freq \n");
				 nState =  FREQ_SET_STATE;    //�л�������Ƶ������״̬
				 bInAlarm = 0;      //�رս��뾯ʾ��λ������ʾ��־λ
				 bInFreq  = 0;      //�رս���Ƶ�ʵ�����ʾ�꾻
			}
		}
		break;
		case KEY_A_PRES: 	          //A��
		{
			if(KEY_SHORT == nMask)	  //�̰�
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
		case KEY_B_PRES: 	          //B��
		{
			if(KEY_SHORT == nMask)	  //�̰�
			{
			  printf("power --\n");
		   	  if(nPowerLevel>=1)
			  {
			     nPowerLevel--;
				 SaveConfig();
				 if(nPowerLevel == 0) //0��λ��ֱ�ӹر�
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

//����Ƶ������ģʽ���¼�
void FreqSetHandle()
{
    unsigned char nKey  = nEvent&0x0f;
	unsigned char nMask = nEvent&0xf0;
	switch(nKey)
	{
		case KEY_A_PRES: 	   //A��
		{
			if(KEY_SHORT == nMask)	  //�̰�
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
		case KEY_B_PRES: 	   //B��
		{
			if(KEY_SHORT == nMask)	  //�̰�
			{
			    printf("to Normal \n");
			   	nState =  POWER_SET_STATE;   //�л������书������״̬
			}
		}
		break;
		default:
		break; 
	}
}

//����ʾǿ������ģʽ���¼�
void AlarmSetHandle()
{
    unsigned char nKey  = nEvent&0x0f;
	unsigned char nMask = nEvent&0xf0;
	switch(nKey)
	{
		case KEY_A_PRES: 	          //A��
		{
			if(KEY_SHORT == nMask)	  //�̰�
			{
			   	 bSendCode = 1;       //�����������־λ��λ
				 printf("Send a Code Signal \n");
			}
		}
		break;
		case KEY_B_PRES: 	          //B��
		{
			if(KEY_SHORT == nMask)	  //�̰�
			{
			   	nState =  POWER_SET_STATE;   //�л������书������״̬
				printf("to Normal \n");
			}
		}
		break;
		default:
		break; 
	}
}

//�û�������������
void UserTask()
{
	 switch(nState)
	 {
		case POWER_SET_STATE:
		{
			PowerSetHandle();  //����������ģʽ���¼�
		}
		break;
		case FREQ_SET_STATE: 
		{
			FreqSetHandle();   //����Ƶ������ģʽ���¼�
		}
		break;
		case ALARM_SET_STATE: 
		{
			AlarmSetHandle();  //����ʾǿ������ģʽ���¼�
		}
		break;
		default:
		break;  
	 }
}

//ϵͳ����
void SystemTask()
{
	  if(!bSafeProtect)	 //�����Ƿ����쳣ϵͳ�¼�����
	  {
	      nState =  ERROR_STATE;   //�л���Error״̬
	  }
	  else
	  {
	      //����쳣����ˣ���ָ���Ĭ�ϵķ��书������ģʽ
	      if(nState == ERROR_STATE)
	          nState = 	POWER_SET_STATE;
	  }
}

//״ָ̬ʾ����
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
//�������ʾ����
void DigLEDTask()
{
	 switch(nState)
	 {
		case POWER_SET_STATE:
		{
		     nflagLastTime++;
			 if(bInAlarm)      //�����⵽��ʾ��־
			 {
				 if(nflagLastTime <= 100)	  //�����00�͹رս������
				 {
				    WriteLEDData16(0,0);
				 }
			     else
				 {
				    WriteLEDData16(17,17);
				 }
			 }
			 else if(bInFreq)  //�����⵽Ƶ�ʱ�־
			 {
			     if(nflagLastTime <= 100)	  //�����01�͹رս������
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
			 WriteLEDData16(16,16);	  //�������ʾPP
		}
		break;
		case ERROR_STATE:
		{
		     nErrorLastTime++;
			 if(nErrorLastTime <= 100)	  //�쳣״̬�����88�͹رս������
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
   //ϵͳʱ��2��Ƶ
   P_SW2 = 0x80;
   CLKDIV = 0x02;
   P_SW2 = 0x00;

   //��ȡEEPROM����
   LoadConfig();
   //��ʼ����ʱ��
   Timer0Init();
   Timer1Init();
   Timer3Init();
   //��ʼ��IO
   IOInit();
   //��ʼ�������
   DigLEDInit();
   //��ʼ������1
   Uart1Init();
   //�����ж�
   EA = 1;

   while(1)
   {
      //�����û������¼�
	  if(GetUserEvent())  
	  {
	     printf("get a event \n");
		 UserTask();
		 bUserEvent = 0;  //����¼����ݲ��Ҹ�λ��־λ
		 nEvent = 0;
	  }
	  //����ϵͳ�¼�
	  SystemTask();
	  //�����豸��ʾ
	  DigLEDTask();	  //�������ʾ����
	  StatusTask();	  //��ʾ��ǰ�豸״̬��LED,Beep�ȣ�
	  //printf("nPowerLevel %bd nFreqLevel %bd\n",nPowerLevel,nFreqLevel);
	  //delay_ms(10);
   }
}

/*******************************************************************************
* �� �� ��         : void Timer0() interrupt 1
* ��������		   : ��ʱ��0�жϺ���
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void Timer0() interrupt 1  using 1
{
	static u16 iCnt =0;	    //��������ָ��
	//static u16 iCntCode  =0;	//����������ָ��
	static u8  iSendStep =0;	//����������ָ��
	u8 SetTL0 = 0,SetTH0 = 0;
	u16 PWMOnCnt = 0,PWMOffCnt = 0,CheckSafePos = 0,PWMLength =0;
	u8  CodeLength = 66;   //������ļ���  6ms/(181.6us/2)
	u16 StopLength = 330;  //������ļ���  30ms/(181.6us/2)
	//����һ���������֮�� �������õ�Ƶ�ʵ�λ���ڶ�ʱ����ֵ
	if(nFreqLevelCur != nFreqLevel)
	{
	    if(iCnt == 0)
		{
		   nFreqLevelCur = nFreqLevel;
		}
	}

    //ѡ��ʱ������
	if(!bSetCode)
	{
		 if(nFreqLevelCur == 1)
		{
		   	//TL0 = 0x6C;		//���ö�ʱ��ֵ 106us
	        //TH0 = 0xFB;		//���ö�ʱ��ֵ
			//TL0 = 0x8D;		//���ö�ʱ��ֵ 106us
	        //TH0 = 0xFB;		//���ö�ʱ��ֵ
		    TL0 = (65535 - BB_FREQ)%256;		//���ö�ʱ��ֵ 106us
	        TH0 = (65535 - BB_FREQ)/256;		//���ö�ʱ��ֵ
			PWMOnCnt  = 30;
			PWMOffCnt = 314;
			CheckSafePos = 200;
			PWMLength = 1887;
		}
		else
		{
			//TL0 = 0x0A;		//���ö�ʱ��ֵ 68.5us
	        //TH0 = 0xFD;		//���ö�ʱ��ֵ
            //TL0 = 0x26;		//���ö�ʱ��ֵ 68.5us
            //TH0 = 0xFD;		//���ö�ʱ��ֵ
			TL0 = (65535 - CC_FREQ)%256;		//���ö�ʱ��ֵ 106us
	        TH0 = (65535 - CC_FREQ)/256;		//���ö�ʱ��ֵ
			PWMOnCnt  = 46;
			PWMOffCnt = 476;
			CheckSafePos = 300;
			PWMLength = 2899;
		}
	}
	else
	{
	     //TL0 = 0x12;		//���ö�ʱ��ֵ 91us
	     //TH0 = 0xFC;		//���ö�ʱ��ֵ
		 TL0 = (65535 - ER_FREQ)%256;		//���ö�ʱ��ֵ 106us
	     TH0 = (65535 - ER_FREQ)/256;		//���ö�ʱ��ֵ
	}
	
	if(bEnable)   //��ʹ������������
	{
		if(nState != ALARM_SET_STATE)	 //��ͨѶģʽ
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
			//��ȫ��������
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
		     //�ж��Ƿ��з��������������	 
		     if(bSendCode)
			 {
				//�ֲ��������
				switch(iSendStep)
				{
				    case 0:	//��һ�����ȴ�����
					{
						bSigSwitch = 1;
						iSendStep  = 1;   //��ת�����ʾ�ź�
					}
				    case 1: //�ڶ��������200ms�ľ�ʾ�ź�
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
							bSigSwitch = 0;	  //�򿪿��� ׼�����6ms������
							iSendStep  = 2;   //��ת����һ������������
							bSetCode   = 1;   //ʹ���������������ñ�־λ
						}
					}
					break;
					case 2:	//����������ʼ���30ms��ֹͣ�ź�
					{
					    bSigSwitch = 0;	     //�򿪿��� ׼�����6ms������
						iSendStep  = 3;      //��ת�����ʾ�ź�
						iCnt = 0;            //���¿�ʼ����
					}
					break;
				    case 3:	//���Ĳ���ֹͣ���30ms��ֹͣ�ź�
					{
						if(iCnt == StopLength)
						{
						    //��������
							iSendStep  = 4;
						}
					}
					break;
					case 4:	//���Ĳ�����ʼ���6ms��������
					{
					    bSigSwitch = 1;	     //�򿪿��� ׼�����6ms������
						iSendStep  = 5;      //��ת�����ʾ�ź�
						iCnt = 0;            //���¿�ʼ����
					}
					break;
				    case 5:	//���岽��ֹͣ���6ms��������
					{
						if(iCnt == CodeLength)
						{
						    //��������
							iCnt = 0;
							//iCntCode = 0;
							bSigSwitch = 0;
							bSendCode  = 0;	 //�����������������
							iSendStep  = 0;
							bSetCode   = 0;  //�ر��������������ñ�־λ 
						}
					}
					break;
				}
				iCnt++;
			    //iCntCode++;
			 }
			 else
			 {
			    //û�з������󣬹رշ��� ,��־λ��0
			    bSigSwitch = 0;
				iCnt = 0;
				//iCntCode = 0;
				iSendStep = 0;
			 }   
		}
	}	
	else
	{
	    bSigSwitch = 0;  //δʹ�ܣ��ر����
		iCnt = 0;
	}
}


/*******************************************************************************
* �� �� ��         : void Timer1() interrupt 3
* ��������		   : ��ʱ��1�жϺ���
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void Timer1() interrupt 3  using 1
{
	//static u16 j =0;
	u8 	nUserEventTmp = 0;		
	//j++;

	//printf("Timer1 is running\n");
	//����ɨ��
	nUserEventTmp = Key_Scan();
	if((nUserEventTmp != 0)&&(!bUserEvent))
	{
	   //printf("send a event \n");
	   nEvent = nUserEventTmp;	//�������ݲ�����λ��־λ
	   bUserEvent = 1;  
	}

	//Protect1 = ~Protect1;	 //�����������A
	//Protect2 = ~Protect2;	 //�����������B
	//Protect3 = ~Protect3;	 //�����������C
}

void Timer3() interrupt 19  using 1
{
	Beep   = ~Beep;
}