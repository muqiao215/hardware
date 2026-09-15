#ifndef __MYRTC_H
#define __MYRTC_H

#include <stdint.h>

extern uint16_t MyRTC_Time[];

void MyRTC_Init(void);
void MyRTC_SetTime(void);
void MyRTC_ReadTime(void);
uint8_t MyRTC_IsUsingLSE(void);

#endif

