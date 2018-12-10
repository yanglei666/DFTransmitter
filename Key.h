#ifndef KEY_H
#define KEY_H
#include "Common.h"
//#define READ_KEY_A  P2^3
//#define READ_KEY_B  P2^2
 
#define KEY_A_PRES  0x01
#define KEY_B_PRES  0x02
#define KEY_AB_PRES 0x04
 
#define KEY_SHORT   0x20
#define KEY_LONG    0x40
#define KEY_VLONG   0x80
#define KEY_NONE      0

#define VLONG_TIME  500
#define LONG_TIME   200

extern bit  bInAlarm;      //进入警示档位调节提示标志位
extern bit  bInFreq;      //进入频率调节提示标志位

//按键初始化操作
void IOInit(void);
//按键读取码值
unsigned char Key_Read(void);
//要求外部扫描周期为10ms
unsigned char Key_Scan(void);

#endif

 
