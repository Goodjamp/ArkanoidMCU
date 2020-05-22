#ifndef __ST7789HAL_H__
#define __ST7789HAL_H__

#include "stdint.h"
#include "stdbool.h"

typedef struct St7789Cb {
    void (*dcControlCb)(bool set);
    void (*txCb)(uint8_t buff[], uint32_t size);
} St7789Cb;

typedef struct St7789InitStruct {
    uint16_t rowStartAddress;
    uint16_t rowEndAddress;
} St7789InitStruct;

void st7789Init(St7789Cb st7789Cb, St7789InitStruct initStruct);
void st7789SetDefInitStruct(St7789InitStruct *initStruct);
void st7789SetCursor(uint16_t x, uint16_t y);
void st7789SetArea(uint16_t x, uint16_t y, uint16_t areaWidth, uint16_t areaHeight);
void st7789WriteFrame(uint8_t frame[], uint32_t size);

#endif
