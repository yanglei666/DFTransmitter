#ifndef LED_H
#define KED_H
#include "Common.h"

void DigLEDInit();
void WriteLEDData16(unsigned char nShiData,unsigned char nGeData);
void WriteLEDData8(unsigned char nData,bit bIsShi);

#endif