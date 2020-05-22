#ifndef __ARKANOID_GAME_SPACE_H__
#define __ARKANOID_GAME_SPACE_H__

#define PLATFORM_HEIGHT   5
#define PLATFORM_WIDTH    30
#define AREA_WIDTH        120
#define AREA_HEIGHT       180
#define POINTS_PER_FRAME  5
#define ELEMENTS_QYANTITY 10
#define ELEMENT_HEIGHT    10
#define ELEMENT_WIDTH     20
#define PLATFORM_SPEED    5

typedef enum ArkanoidButtons {
    ARKANOID_BUTTON_LEFT,
    ARKANOID_BUTTON_RIGHT,
    ARKANOID_BUTTON_SELL,
} ArkanoidButtons;

typedef struct {
    void (*ClearSpace)(void);
    void (*AddImage)(uint16_t x, uint16_t y, uint8_t image[], uint32_t imageSize);
} ArkanoidCB;

void arkGameSpaceInit(ArkanoidCB arkanoidCB);
void arkGameSpaceProcessing(void);

#endif
