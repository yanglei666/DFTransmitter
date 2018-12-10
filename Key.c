#include"Key.h"

sbit READ_KEY_A = P2^3;
sbit READ_KEY_B = P2^2;

u16 cnt = 0;

//IO初始化操作
void IOInit(void)
{
   //输出口全部设置推挽输出模式
   P0M1 = 0x00;
   P0M0 = 0xff;
   P1M1 = 0x00;
   P1M0 = 0xff;
   P3M1 = 0x1c;
   P3M0 = 0xe2;
   P4M1 = 0x00;
   P4M0 = 0x10;
   //关闭P3.2 3.3 3.4 的斯密特触发器
   P3NCS |= 0x1c;
}

//按键读取码值
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

//要求外部扫描周期为10ms
unsigned char Key_Scan(void)
{
	static unsigned char state = 0;               //按键初始化状态
	static unsigned char KEY_LAST=0;	          //记录两次电平的状态
	unsigned char KEY_NOW=0;    
	unsigned char KEY_VALUE=0;
 
	KEY_NOW = Key_Read();              //读按键值

	switch(state)
	{
		case 0:
		{
			//if(KEY_NOW != KEY_LAST)	           //有按键按下
			if((KEY_NOW != 0)&&(KEY_LAST == 0))	   //上一次的按键如果还未松开则认为无效
			{
			   state = 1;
			   printf("key is press %bx \n",KEY_VALUE);
			}	
		}
		break;
		case 1: 
		{
			if((KEY_NOW == KEY_LAST)&&(KEY_NOW!=0))	  //消斗之后按键有效  并且不为0
			{
			   state = 2;
			} 
			else 
			    state = 0;            //认为误触
		}
		break; 
		case 2:                       //消斗之后
		{
			if(KEY_NOW == KEY_LAST)   //还是按下的状态 
			{
			  	state = 3;
			}
			else//松开了，短按
			{
			    //如果在还未松开的情况下又按了另外一个键，则按组合键重新计数
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
				KEY_VALUE = KEY_LAST|KEY_SHORT;  //返回键值短按
				//printf("key is press 2 %02x\n",KEY_VALUE);	
			}
		}
		break;
		
		case 3: //判断长按短按
		{
			cnt++;
			printf("keynow is %bd keylast is %bd cnt %d\n",KEY_NOW,KEY_LAST,cnt);
			if(KEY_NOW == KEY_LAST) 
			{
				if(cnt > VLONG_TIME) //7000ms `
				{ 
				     if(KEY_NOW == KEY_AB_PRES)
					 {
					    bInFreq  = 1;     //开启进入频率调节提示标志
						bInAlarm = 0;
					 }
					    
				}
				else if((cnt < VLONG_TIME)&&(cnt > LONG_TIME))        //LONG
				{
					if(KEY_NOW == KEY_AB_PRES)
					{
					   bInAlarm = 1;	 //开启进入警示调节提示标志
					   bInFreq  = 0;
					}	    
				}			  
			}
			else
			{
			    //如果在还未松开的情况下又按了另外一个键，则按组合键重新计数
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
					KEY_VALUE = KEY_LAST|KEY_LONG; //返回键值长按
					//printf("key is press 32 %bx cnt %d\n",KEY_VALUE,cnt);
					cnt = 0; 
				}
				else
				{
					state = 0;
				    KEY_VALUE = KEY_LAST|KEY_SHORT; //返回键值短按
					//printf("key is press 33 %bx cnt %d\n",KEY_VALUE,cnt);
					cnt = 0;
				}  					
			}
		}
		break;
		case 4://长按松手检测
		{
			if(KEY_NOW != KEY_LAST) 
			{
			   state = 0;
			   //printf("key is press 4\n");
			}		
		}
		break;
	}
	KEY_LAST = KEY_NOW; //更新
	return KEY_VALUE;
}

