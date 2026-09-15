#ifndef __SERVO_H
#define __SERVO_H

#include <stdint.h>

void Servo_Init(void);
void Servo_SetAngle(uint8_t channel, float angle_deg);

#endif

