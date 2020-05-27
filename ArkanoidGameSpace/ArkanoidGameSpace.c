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

static void arkGameReInit(void)
{

    for(uint32_t k = 0; k < ELEMENTS_QYANTITY; k++) {
        elementDescr[k].isPresent = true;
    }
    arkanoidReInit(arkanoidH, arkanoidConfig);

    uint32_t x = AREA_WIDTH/2 - PLATFORM_WIDTH / 2;
    uint32_t y = AREA_HEIGHT - PLATFORM_HEIGHT - 2;

    arkanoidSetPlatformPos(arkanoidH, x, y);
    arkanoidSetBallPos(arkanoidH, x + PLATFORM_WIDTH / 2, y-1);
    arkanoidSetDirection(arkanoidH, 30, 50);
}


void arkGameSpaceInit(ArkanoidCB arkanoidCB)
{
    arkanoidCBList = arkanoidCB;
    uint32_t columnsPerRow = (AREA_WIDTH - 2) / ELEMENT_WIDTH;
    uint32_t currentRow = 0;
    uint32_t currentColumn = 0;
    for(uint32_t k = 0; k < ELEMENTS_QYANTITY; k++) {
        currentRow    = k / columnsPerRow;
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
    arkanoidSetBallPos(arkanoidH, x + PLATFORM_WIDTH / 2, y-1);
    arkanoidSetDirection(arkanoidH, 30, -50);
}

void arkGameSpaceProcessing(void)
{
    int32_t x;
    int32_t y;
    arkanoidCBList.ClearSpace();
    if(arkanoidUpdate(arkanoidH) != STATUS_GAME) {
        arkanoidCBList.AddImage(10, 30 ,
                            loosPic100_115,
                            100,
                            115);
        arkanoidCBList.ShowFrame();
        return;
    }
    for(uint32_t k = 0; k < ELEMENTS_QYANTITY; k++) {
        if(!elementDescr[k].isPresent) {
            continue;
        }
        arkanoidCBList.AddImage(elementDescr[k].x1 + BALL_RADIUS,
                                elementDescr[k].y1 + BALL_RADIUS,
                                element20_15,
                                ELEMENT_WIDTH,
                                ELEMENT_HEIGHT);
    }

    arkanoidGetPlatformPos(arkanoidH, &x, &y);
    arkanoidCBList.AddImage(x + BALL_RADIUS,
                            y + BALL_RADIUS,
                            platform8_30,
                            PLATFORM_WIDTH,
                            PLATFORM_HEIGHT);
    arkanoidGetBallPos(arkanoidH, &x, &y);
    arkanoidCBList.AddImage(x, y, ball7_7, BALL_WIDTH, BALL_HEIGHT);
    arkanoidCBList.ShowFrame();
}
/*loosPic100_115*/
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
        arkanoidSetPlatformPos(arkanoidH, x, y);
        break;
    case ARKANOID_BUTTON_RIGHT:
        if((x + PLATFORM_SPEED + PLATFORM_WIDTH) >= areaX) {
            x = areaX - PLATFORM_WIDTH;
        } else {
            x += PLATFORM_SPEED;
        }
        arkanoidSetPlatformPos(arkanoidH, x, y);
        break;
    case ARKANOID_BUTTON_SELL:
        arkGameReInit();
        break;
    }

}
