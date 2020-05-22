#include "stdint.h"
#include "string.h"

#include "frameHall.h"
//#include "displaySsd1306HAL.h"

//#include "symbolsArial11pts.h"
//#include "symbolsArial8pts.h"
//#include "symbolsArial16pts.h"
//#include "symbolsSegoePrint14pts.h"

/*fontDescriptor fields */
#define WIDTH_POS   0
#define HEIGHT_POS  1
#define ADDRESS_POS 2

/*fontInfo fields */
#define FIRST_SYMBOL_POS      1
#define LAST_SYMBOL_POS       2
#define FONT_DESCRIPTIONS_POS 4
#define FONT_BITMAP_POS       5

static const uint32_t *fontInfoList[] =
{
    //[ARIAL_8PTS]  = (uint32_t*)arial_8ptFontInfo,
    //[ARIAL_11PTS] = (uint32_t*)arial_11ptFontInfo,
    //[ARIAL_16PTS] = (uint32_t*)arial_16ptFontInfo,
    //[SEGOEPRINT_14PTS] = (uint32_t*)segoePrint_14ptFontInfo,
};

static const uint8_t* getSymbol(const uint8_t *symbol,
                                          uint8_t *height,
                                          uint8_t *width,
                                          SymbolType symbolType)
{
    const uint32_t *fontInfo            = fontInfoList[symbolType];
    const uint32_t (*fontDescriptor)[3] = (uint32_t(*)[3])fontInfo[FONT_DESCRIPTIONS_POS];
    const uint8_t  *fontBitmap          = (uint8_t*)fontInfo[FONT_BITMAP_POS];

    if((*symbol < fontInfo[FIRST_SYMBOL_POS]) || (*symbol > fontInfo[LAST_SYMBOL_POS])) {
        return NULL;  // symbol out of range
    }

    uint8_t pos = *symbol - fontInfo[FIRST_SYMBOL_POS];

    *height = fontDescriptor[pos][HEIGHT_POS];
    *width  = fontDescriptor[pos][WIDTH_POS];
    return fontBitmap + fontDescriptor[pos][ADDRESS_POS];
}

static void r5G6B5ToColors8(uint16_t r5g6b5, uint8_t *r8, uint8_t *g8, uint8_t *b8)
{
    *b8 = 0b11111000 & (r5g6b5 >> 5);
	*g8 = (0b11100000 & (r5g6b5 >> 8)) | (0b00011100 & (r5g6b5 << 2));
	*r8 = 0b11111000 & r5g6b5;
}

static void colors8ToR5G6B5(uint16_t *r5g6b5, uint8_t r8, uint8_t g8, uint8_t b8)
{
    *r5g6b5  = 0;
	*r5g6b5  = 0b0001111100000000 & (((uint16_t)b8) << 5);
	*r5g6b5 |= 0b0000000011111000 & ((uint16_t)r8);
	*r5g6b5 |= 0b1110000000000111 & ((((uint16_t)g8) << 11) | (((uint16_t)g8) >> 5));
}

static inline uint8_t mixCollor(uint8_t alpha, uint8_t srcCollor, uint8_t dstCollor)
{
    return (srcCollor * alpha + dstCollor * (0xFF - alpha)) >> 8;
}

static bool addImage(FrameDescr *inFrame, const uint8_t *image, uint16_t heigh, uint16_t width, bool withAlpha)
{
#pragma pack(push,1)
union {
    uint32_t colorRowData;
    struct {
        uint8_t alpha;
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    } colorFormatData;
} R8G8B8Format;
#pragma pack(pop)

    uint16_t  (*frameArray)[inFrame->width] = (uint16_t(*)[inFrame->width])inFrame->buff;
    if(withAlpha) {
        uint8_t frameRed;
        uint8_t frameGreen;
        uint8_t frameBlue;
        uint32_t  (*imageArray)[width] = (uint32_t(*)[width])image;
        /*input image with alpha must be A8R8G8B8 format*/
        for(uint16_t  y = 0; y < heigh; y++) {
            for(uint16_t  x = 0; x < width; x++) {
                r5G6B5ToColors8(frameArray[y + inFrame->y][x + inFrame->x],
                                &frameRed,
                                &frameGreen,
                                &frameBlue);
                R8G8B8Format.colorRowData = imageArray[y][x];

                frameRed = mixCollor(R8G8B8Format.colorFormatData.alpha,
                                     R8G8B8Format.colorFormatData.red,
                                     frameRed);
                frameGreen = mixCollor(R8G8B8Format.colorFormatData.alpha,
                                     R8G8B8Format.colorFormatData.green,
                                     frameGreen);
                frameBlue = mixCollor(R8G8B8Format.colorFormatData.alpha,
                                     R8G8B8Format.colorFormatData.blue,
                                     frameBlue);
                colors8ToR5G6B5(&frameArray[y + inFrame->y][x + inFrame->x],
                                frameRed,
                                frameGreen,
                                frameBlue);
            }
        }

    } else {
        /*input image without alpha must be R5G6B5 format*/
        uint16_t  (*imageArray)[width] = (uint16_t(*)[width])image;
        for(uint16_t  y = 0; y < heigh; y++) {
            for(uint16_t  x = 0; x < width; x++) {
                frameArray[y + inFrame->y][x + inFrame->x] = imageArray[y][x];
            }
        }
    }
    return true;
}

void frameInit(FrameHandl inFrame, uint8_t *frameBuff, uint16_t heigh, uint16_t width)
{
    inFrame->buff  = frameBuff;
    inFrame->heigh = heigh;
    inFrame->width = width;
}

bool frameAddString(FrameHandl inFrame, const uint8_t *str, SymbolType symbolType, bool leaveExisting)
{
    return true;
}

bool frameAddImage(FrameHandl inFrame, const uint8_t *image, uint16_t imageHeigh, uint16_t imageWidth, bool withAlpha)
{
    return addImage(inFrame, image, imageHeigh, imageWidth, withAlpha);
}

void frameAddArea(FrameHandl inFrame, Point beginPoint, uint8_t height, uint8_t width)
{
    uint16_t  (*screenBuff)[inFrame->width] = (uint16_t(*)[inFrame->width])inFrame->buff;
}

void frameAddRectangle(FrameHandl inFrame, Point upLeft, Point downRight, uint8_t width)
{
    //up horizontal line
    frameAddArea(inFrame, upLeft, width, downRight.x - upLeft.x);
    //down horizontal line
    frameAddArea(inFrame, (Point){upLeft.x, downRight.y - width + 1}, width, downRight.x - upLeft.x + 1);
    //left vertical line
    frameAddArea(inFrame, upLeft, downRight.y - upLeft.y  , width);
    //left vertical line
    frameAddArea(inFrame, (Point){downRight.x - width + 1, upLeft.y}, downRight.y - upLeft.y + 1 , width);
}

void frameAddHorizontalLine(FrameHandl inFrame, Point startPoint, uint8_t width, uint8_t length)
{
    frameAddArea(inFrame, startPoint, width, length);
}

void framePutPixe(FrameHandl inFrame, Point pixel, Collor565 collor)
{
    uint16_t  (*screenBuff)[inFrame->width] = (uint16_t(*)[inFrame->width])inFrame->buff;
    memcpy(&screenBuff[pixel.y][pixel.x], &collor, sizeof(collor));
}

bool frameClear(FrameHandl inFrame)
{
   memset(inFrame->buff, 0, inFrame->heigh * inFrame->width * sizeof(uint16_t));
   return true;
}

bool frameSetPosition(FrameHandl inFrame, uint16_t inX, uint16_t inY)
{
    inFrame->x = inX;
    inFrame->y = inY;
    return true;
}
