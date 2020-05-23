#include "stdint.h"
#include "stdlib.h"

#include "stm32f4xx_conf.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#include "SpiHal.h"
#include "ST7789Hal.h"
#include "frameHall.h"
#include "buttons.h"
#include "buttonsHall.h"

#include "ArkanoidGameSpace.h"
#include "images.h"

#define GAME_SPACE_W 120
#define GAME_SPACE_H 180

uint8_t gameSpaceFrame[AREA_WIDTH * AREA_HEIGHT * 2];
FrameDescr gameSpaceFrameDesc;
void buttonShortCb0(void);
void buttonShortCb1(void);
void buttonShortCb2(void);
void buttonShortCb3(void);
void buttonShortCb4(void);

static const ButtonActionDescription buttonActionDescription[] = {
    [0] = {
        .buttonActionCb = buttonShortCb0,
        .buttonNumber   = 0,
        .pressType      = PRESS_SHORT,
    },
    [1] = {
        .buttonActionCb = buttonShortCb1,
        .buttonNumber   = 1,
        .pressType      = PRESS_SHORT,
    },
    [2] = {
        .buttonActionCb = buttonShortCb2,
        .buttonNumber   = 2,
        .pressType      = PRESS_SHORT,
    },
    [3] = {
        .buttonActionCb = buttonShortCb3,
        .buttonNumber   = 3,
        .pressType      = PRESS_SHORT,
    },
    [4] = {
        .buttonActionCb = buttonShortCb4,
        .buttonNumber   = 4,
        .pressType      = PRESS_SHORT,
    }
};


const St7789Cb st7789Cb = {
    .dcControlCb = dcControl,
    .txCb        = spiTx,
};
const St7789InitStruct st7789InitStruct = {
    .rowStartAddress = 0,
    .rowEndAddress   = 240 - 1,
};

void buttonShortCb4(void) // Left
{
    arkGameSpaceButtonAction(ARKANOID_BUTTON_LEFT);
}

void buttonShortCb0(void) // Right
{
    arkGameSpaceButtonAction(ARKANOID_BUTTON_RIGHT);
}

void buttonShortCb2(void) // Sell
{
    arkGameSpaceButtonAction(ARKANOID_BUTTON_SELL);
}

void buttonShortCb1(void)
{
    //uint8_t event = 1;
}


void buttonShortCb3(void)
{
    //uint8_t event = 1;
}


void initL2(void)
{
    GPIO_InitTypeDef pinInit = {
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_OType = GPIO_OType_PP,
        .GPIO_PuPd  = GPIO_PuPd_NOPULL,
        .GPIO_Pin   = GPIO_Pin_1,
        .GPIO_Mode  = GPIO_Mode_OUT,
    };
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_Init(GPIOA, &pinInit);
}

void buttonsConfig(void)
{
    buttonsInitButton(buttonActionDescription,
                      sizeof(buttonActionDescription) / sizeof(buttonActionDescription[0]),
                      readButton);
}

void addImage(uint16_t x, uint16_t y, const uint8_t image[], uint32_t imageWidth, uint32_t imageHeight)
{
    frameSetPosition(&gameSpaceFrameDesc, x, y);
    frameAddImage(&gameSpaceFrameDesc, image, imageHeight, imageWidth, true);
}

void clearFrame(void) {
    //frameClear(&gameSpaceFrameDesc);
    frameSetPosition(&gameSpaceFrameDesc, 0, 0);
    frameAddImage(&gameSpaceFrameDesc, gameArea, AREA_HEIGHT, AREA_WIDTH, false);
}

void showFrame(void)
{
    st7789WriteFrame(gameSpaceFrame, sizeof(gameSpaceFrame));
}

static const ArkanoidCB arkanoidCB = {
    .ClearSpace = clearFrame,
    .AddImage   = addImage,
    .ShowFrame  = showFrame,
};

int main(void)
{
    uint32_t cnt = 0;
    initL2();
    initSPI(NULL);
    buttonsConfig();
    st7789Init(st7789Cb, st7789InitStruct);
    st7789WriteFrame((uint8_t*)imBackground, sizeof(imBackground));
    st7789SetArea(20, 20, AREA_WIDTH, AREA_HEIGHT);
    arkGameSpaceInit(arkanoidCB);
    /*Init screen */
    frameInit(&gameSpaceFrameDesc, gameSpaceFrame, GAME_SPACE_H, GAME_SPACE_W);
    /*
    frameAddImage(&gameSpaceFrameDesc, gameArea, GAME_SPACE_H, GAME_SPACE_W, false);

    frameSetPosition(&gameSpaceFrameDesc, 40, 40);
    frameAddImage(&gameSpaceFrameDesc, ball6_6, 8, 8, true);

    st7789WriteFrame(gameSpaceFrame, sizeof(gameSpaceFrame));
    frameSetPosition(&gameSpaceFrameDesc, 0, 0);

    for(uint16_t y = 0; y < 60; y += ELEMENT_H) {
        for(uint16_t x = 0; x < (GAME_SPACE_W - 1); x += ELEMENT_W) {
            frameSetPosition(&gameSpaceFrameDesc, x, y);
            frameAddImage(&gameSpaceFrameDesc, element, ELEMENT_H, ELEMENT_W, false);
        }
    }

    st7789WriteFrame(gameSpaceFrame, sizeof(gameSpaceFrame));
    uint16_t yMax = ELEMENT_H;
    */
    while(1)
    {
         while(cnt < 128) {
             cnt++;
         }
        buttonsProcessing();
        arkGameSpaceProcessing();
        /*
        buttonsProcessing();
        yMax += ELEMENT_H;
        if(yMax >= (GAME_SPACE_H - 1)) {
            yMax = ELEMENT_H;
        }
        frameSetPosition(&gameSpaceFrameDesc, 0, 0);
        frameAddImage(&gameSpaceFrameDesc, gameArea, GAME_SPACE_H, GAME_SPACE_W, false);
        for(uint16_t y = 0; y < yMax; y += ELEMENT_H) {
            for(uint16_t x = 0; x < (GAME_SPACE_W - 1); x += ELEMENT_W) {
               frameSetPosition(&gameSpaceFrameDesc, x, y);
               frameAddImage(&gameSpaceFrameDesc, element, ELEMENT_H, ELEMENT_W, false);
            }
        }
        st7789WriteFrame(gameSpaceFrame, sizeof(gameSpaceFrame));


         cnt = 0;
         while(cnt < 12800) {
             cnt++;
         }
         switchScreenCnt++;
         if(switchScreenCnt > 2) {
             switchScreenCnt = 0;
         }
         if(isEnable) {
             GPIO_ResetBits(GPIOA, GPIO_Pin_1);
             isEnable = false;
         } else {
             GPIO_SetBits(GPIOA, GPIO_Pin_1);
             isEnable = true;
         }
         */
    }
}
