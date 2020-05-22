#include "stdint.h"

#include "arkanoidEngeen.h"

#include "ArkanoidGameSpace.h"

static ArkanoidCB arkanoidCBList;
static ArkanoidConfig arkanoidConfig = {
};
static ArkanoidH arkanoidH;
/*
typedef struct {
    int32_t    x1;
    int32_t y1;
    int32_t x2;
    int32_t y2;
    bool        isPresent;
} ElementDescr;
#define ELEMENTS_QYANTITY 10
#define ELEMENT_HEIGHT    10
#define ELEMENT_WIDTH     20
*/
ElementDescr elementDescr[ELEMENTS_QYANTITY];

void arkGameSpaceButtonAction(void)
{

}

void arkGameSpaceInit(ArkanoidCB arkanoidCB)
{
    arkanoidCBList = arkanoidCB;
    arkanoidH      = arkanoidInit(arkanoidConfig);
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
}

void arkGameSpaceProcessing(void)
{

}
