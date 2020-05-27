#ifndef __ARKANOID_GAME_SPACE_H__
#define __ARKANOID_GAME_SPACE_H__


/**Game area size**/
#define AREA_WIDTH        120
#define AREA_HEIGHT       180
/**Element size **/
#define ELEMENT_WIDTH     20
#define ELEMENT_HEIGHT    15
/**Platform size**/
#define PLATFORM_WIDTH    30
#define PLATFORM_HEIGHT   8
/**Ball size**/
#define BALL_RADIUS  3
#define BALL_WIDTH    7
#define BALL_HEIGHT   7
/**General game parameters**/
#define POINTS_PER_FRAME  4
#define ELEMENTS_QYANTITY 20
#define PLATFORM_SPEED    8


typedef enum ArkanoidButtons {
    ARKANOID_BUTTON_LEFT,
    ARKANOID_BUTTON_RIGHT,
    ARKANOID_BUTTON_SELL,
} ArkanoidButtons;

typedef struct {
    void (*ClearSpace)(void);
    void (*ShowFrame)(void);
    void (*AddImage)(uint16_t x, uint16_t y, const uint8_t image[], uint32_t imageWidth, uint32_t imageHeight);
} ArkanoidCB;

void arkGameSpaceInit(ArkanoidCB arkanoidCB);
void arkGameSpaceProcessing(void);
void arkGameSpaceButtonAction(ArkanoidButtons button);

#endif
