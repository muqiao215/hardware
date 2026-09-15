#ifndef __STORE_H
#define __STORE_H

#define STORE_START_ADDRESS     0x0800FC00
#define STORE_COUNT             512

extern uint16_t Store_Data[STORE_COUNT];

void Store_Init(void);
void Store_Save(void);
void Store_Clear(void);

#endif
