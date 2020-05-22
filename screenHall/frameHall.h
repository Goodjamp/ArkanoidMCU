#ifndef _SCREEN_HALL_H_
#define  _SCREEN_HALL_H_

#include "stdint.h"
#include "stdbool.h"

typedef enum {
    ARIAL_8PTS,
    ARIAL_11PTS,
    ARIAL_16PTS,
    SEGOEPRINT_14PTS,
}SymbolType;

typedef struct{
    uint8_t *buff;
    uint16_t heigh;
    uint16_t width;
    uint16_t x;
    uint16_t y;
}FrameDescr;

typedef struct Point{
    uint16_t x;
    uint16_t y;
} Point;

typedef struct Collor565{
    uint16_t r:5;
    uint16_t g:5;
    uint16_t b:5;
} Collor565;

typedef FrameDescr *FrameHandl;

void frameInit             (FrameHandl inFrame, uint8_t *frameBuff, uint16_t heigh, uint16_t width);
bool frameClear            (FrameHandl inFrame);
bool frameSetPosition      (FrameHandl inFrame, uint16_t inX, uint16_t inY);
bool frameAddString        (FrameHandl inFrame, const uint8_t *str, SymbolType symbolType, bool leaveExisting);
bool frameAddImage         (FrameHandl inFrame, const uint8_t *image, uint16_t imageHeigh, uint16_t imageWidth, bool withAlpha);
void frameAddArea          (FrameHandl inFrame, Point beginPoint, uint8_t height, uint8_t width);
void frameAddRectangle     (FrameHandl inFrame, Point upLeft, Point downRight, uint8_t width);
void frameAddHorizontalLine(FrameHandl inFrame, Point startPoint, uint8_t width, uint8_t length);
void framePutPixe          (FrameHandl inFrame, Point pixel, Collor565 collor);

#endif
