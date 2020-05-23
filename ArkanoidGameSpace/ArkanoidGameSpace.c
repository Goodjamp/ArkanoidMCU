#include "stdint.h"

#include "arkanoidEngeen.h"
#include "images.h"
#include "ArkanoidGameSpace.h"

static ArkanoidCB arkanoidCBList;
static ArkanoidConfig arkanoidConfig;
static ArkanoidH arkanoidH;

const uint32_t areaX = AREA_WIDTH - 1;
const uint32_t areaY = AREA_HEIGHT - 1;

ElementDescr elementDescr[ELEMENTS_QYANTITY];

void arkGameSpaceInit(ArkanoidCB arkanoidCB)
{
    arkanoidCBList = arkanoidCB;
    uint32_t columnsPerRow = (AREA_WIDTH - 2) / ELEMENT_WIDTH;
    uint32_t currentRow = 0;
    uint32_t currentColumn = 0;
    for(uint32_t k = 0; k < ELEMENTS_QYANTITY; k++) {
        currentRow    = columnsPerRow / k;
        currentColumn = k - currentRow * columnsPerRow;
        elementDescr[k].x1 = 1 + currentColumn * ELEMENT_WIDTH;
        elementDescr[k].x2 = 1 + elementDescr[k].x1 + ELEMENT_WIDTH - 1;
        elementDescr[k].y1 = 1 + currentRow * ELEMENT_HEIGHT;
        elementDescr[k].y2 = 1 + elementDescr[k].y1 + ELEMENT_HEIGHT - 1;
        elementDescr[k].isPresent = true;
    }
    arkanoidConfig.elementList     = elementDescr;
    arkanoidConfig.elementQuantity = ELEMENTS_QYANTITY;
    arkanoidConfig.platformHeight  = PLATFORM_HEIGHT;
    arkanoidConfig.platformWidth   = PLATFORM_WIDTH;
    arkanoidConfig.platformX       = 1;
    arkanoidConfig.platformY       = AREA_HEIGHT - PLATFORM_HEIGHT;
    arkanoidConfig.areaHeight      = AREA_HEIGHT;
    arkanoidConfig.areaWidth       = AREA_WIDTH;
    arkanoidConfig.pointsPerFrame  = POINTS_PER_FRAME;

    arkanoidH      = arkanoidInit(arkanoidConfig);

    uint32_t x = AREA_WIDTH/2 - PLATFORM_WIDTH / 2;
    uint32_t y = AREA_HEIGHT - PLATFORM_HEIGHT - 2;

    arkanoidSetPlatformPos(arkanoidH, x, y);
}

void arkGameSpaceProcessing(void)
{
/*platform8_30*/
    int32_t x;
    int32_t y;
    arkanoidGetPlatformPos(arkanoidH, &x, &y);
    arkanoidCBList.ClearSpace();
    arkanoidCBList.AddImage(x, y, platform8_30, 30, 8);
    arkanoidCBList.AddImage(60, 90, ball9_9, 9, 9);
    arkanoidCBList.ShowFrame();
}

void arkGameSpaceButtonAction(ArkanoidButtons button)
{
    int32_t x;
    int32_t y;
    arkanoidGetPlatformPos(arkanoidH, &x, &y);
    switch (button) {
    case ARKANOID_BUTTON_LEFT:
        if(x <= PLATFORM_SPEED) {
            x = 1;
        } else {
            x -= PLATFORM_SPEED;
        }
        break;
    case ARKANOID_BUTTON_RIGHT:
        if((x + PLATFORM_SPEED + PLATFORM_WIDTH) >= areaX) {
            x = areaX - PLATFORM_WIDTH;
        } else {
            x += PLATFORM_SPEED;
        }
        break;
    case ARKANOID_BUTTON_SELL: break;
    }
    arkanoidSetPlatformPos(arkanoidH, x, y);
}
