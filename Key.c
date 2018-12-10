#include"Key.h"

sbit READ_KEY_A = P2^3;
sbit READ_KEY_B = P2^2;

u16 cnt = 0;

//IO��ʼ������
void IOInit(void)
{
   //�����ȫ�������������ģʽ
   P0M1 = 0x00;
   P0M0 = 0xff;
   P1M1 = 0x00;
   P1M0 = 0xff;
   P3M1 = 0x1c;
   P3M0 = 0xe2;
   P4M1 = 0x00;
   P4M0 = 0x10;
   //�ر�P3.2 3.3 3.4 ��˹���ش�����
   P3NCS |= 0x1c;
}

//������ȡ��ֵ
unsigned char Key_Read(void)
{    
    bit A_VAL = 0,B_VAL =0;
	A_VAL = READ_KEY_A;
	B_VAL =	READ_KEY_B;
    if((!A_VAL)&&(!B_VAL))  
		return KEY_AB_PRES;

    if(!A_VAL)  
	    return KEY_A_PRES;

    if(!B_VAL)  
		return KEY_B_PRES;
	
	return KEY_NONE;
}

//Ҫ���ⲿɨ������Ϊ10ms
unsigned char Key_Scan(void)
{
	static unsigned char state = 0;               //������ʼ��״̬
	static unsigned char KEY_LAST=0;	          //��¼���ε�ƽ��״̬
	unsigned char KEY_NOW=0;    
	unsigned char KEY_VALUE=0;
 
	KEY_NOW = Key_Read();              //������ֵ

	switch(state)
	{
		case 0:
		{
			//if(KEY_NOW != KEY_LAST)	           //�а�������
			if((KEY_NOW != 0)&&(KEY_LAST == 0))	   //��һ�εİ��������δ�ɿ�����Ϊ��Ч
			{
			   state = 1;
			   printf("key is press %bx \n",KEY_VALUE);
			}	
		}
		break;
		case 1: 
		{
			if((KEY_NOW == KEY_LAST)&&(KEY_NOW!=0))	  //����֮�󰴼���Ч  ���Ҳ�Ϊ0
			{
			   state = 2;
			} 
			else 
			    state = 0;            //��Ϊ��
		}
		break; 
		case 2:                       //����֮��
		{
			if(KEY_NOW == KEY_LAST)   //���ǰ��µ�״̬ 
			{
			  	state = 3;
			}
			else//�ɿ��ˣ��̰�
			{
			    //����ڻ�δ�ɿ���������ְ�������һ����������ϼ����¼���
			    if(KEY_NOW == KEY_AB_PRES)
				{
				     state = 0;
					 cnt = 0;
					 KEY_LAST = 0;
					 KEY_NOW  = 0;
					 KEY_VALUE = 0;
					 break;
				}
				state = 0; 
				KEY_VALUE = KEY_LAST|KEY_SHORT;  //���ؼ�ֵ�̰�
				//printf("key is press 2 %02x\n",KEY_VALUE);	
			}
		}
		break;
		
		case 3: //�жϳ����̰�
		{
			cnt++;
			printf("keynow is %bd keylast is %bd cnt %d\n",KEY_NOW,KEY_LAST,cnt);
			if(KEY_NOW == KEY_LAST) 
			{
				if(cnt > VLONG_TIME) //7000ms `
				{ 
				     if(KEY_NOW == KEY_AB_PRES)
					 {
					    bInFreq  = 1;     //��������Ƶ�ʵ�����ʾ��־
						bInAlarm = 0;
					 }
					    
				}
				else if((cnt < VLONG_TIME)&&(cnt > LONG_TIME))        //LONG
				{
					if(KEY_NOW == KEY_AB_PRES)
					{
					   bInAlarm = 1;	 //�������뾯ʾ������ʾ��־
					   bInFreq  = 0;
					}	    
				}			  
			}
			else
			{
			    //����ڻ�δ�ɿ���������ְ�������һ����������ϼ����¼���
			    if(KEY_NOW == KEY_AB_PRES)
				{
				     state = 0;
					 cnt = 0;
					 KEY_LAST = 0;
					 KEY_NOW  = 0;
					 KEY_VALUE = 0;
					 break;
				}

				if(cnt > VLONG_TIME) //7000ms
				{ 
					state = 4;
					KEY_VALUE = KEY_LAST|KEY_VLONG; //VLONG
					//printf("key is press 31 %bx cnt %d\n",KEY_VALUE,cnt);
					cnt = 0;
				}
				else if((cnt < VLONG_TIME)&&(cnt > LONG_TIME))        //LONG
				{
					state = 0;
					KEY_VALUE = KEY_LAST|KEY_LONG; //���ؼ�ֵ����
					//printf("key is press 32 %bx cnt %d\n",KEY_VALUE,cnt);
					cnt = 0; 
				}
				else
				{
					state = 0;
				    KEY_VALUE = KEY_LAST|KEY_SHORT; //���ؼ�ֵ�̰�
					//printf("key is press 33 %bx cnt %d\n",KEY_VALUE,cnt);
					cnt = 0;
				}  					
			}
		}
		break;
		case 4://�������ּ��
		{
			if(KEY_NOW != KEY_LAST) 
			{
			   state = 0;
			   //printf("key is press 4\n");
			}		
		}
		break;
	}
	KEY_LAST = KEY_NOW; //����
	return KEY_VALUE;
}

