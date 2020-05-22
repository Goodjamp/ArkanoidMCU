#include "stdint.h"
#include "stdbool.h"

#include "buttonsHall.h"

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"

enum {
    SW2,  // right
    SW3,  // down
    SW4,  // sell
    SW5,  // up
    SW6,  // left
};

static const struct {
    uint32_t         pin;
    uint32_t         pinSrc;
    GPIO_TypeDef*    port;
    GPIOMode_TypeDef mode;
    uint32_t         portDef;
    bool             af;
} buttonsPinList[] = {
    [SW2] = {
        .pin     = GPIO_Pin_15,
        .pinSrc  = GPIO_PinSource15,
        .port    = GPIOB,
        .mode    = GPIO_Mode_IN,
        .portDef = RCC_AHB1Periph_GPIOB,
        .af      = false,
        },
    [SW3] = {
        .pin     = GPIO_Pin_14,
        .pinSrc  = GPIO_PinSource14,
        .port    = GPIOB,
        .mode    = GPIO_Mode_IN,
        .portDef = RCC_AHB1Periph_GPIOB,
        .af      = false,
        },
    [SW4] = {
        .pin     = GPIO_Pin_8,
        .pinSrc  = GPIO_PinSource8,
        .port    = GPIOA,
        .mode    = GPIO_Mode_IN,
        .portDef = RCC_AHB1Periph_GPIOA,
        .af      = false,
        },
    [SW5] = {
        .pin     = GPIO_Pin_13,
        .pinSrc  = GPIO_PinSource13,
        .port    = GPIOB,
        .mode    = GPIO_Mode_IN,
        .portDef = RCC_AHB1Periph_GPIOB,
        .af      = false,
        },
    [SW6] = {
        .pin     = GPIO_Pin_12,
        .pinSrc  = GPIO_PinSource12,
        .port    = GPIOB,
        .mode    = GPIO_Mode_IN,
        .portDef = RCC_AHB1Periph_GPIOB,
        .af      = false,
        },
};

static bool isGpioInit = false;

static void initGpio(void) {
    GPIO_InitTypeDef pinInit = {
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_OType = GPIO_OType_PP,
        .GPIO_PuPd  = GPIO_PuPd_UP,
    };
    for(uint32_t k = 0; k < sizeof(buttonsPinList) / sizeof(buttonsPinList[0]); k++) {
        pinInit.GPIO_Pin = buttonsPinList[k].pin;
        pinInit.GPIO_Mode = buttonsPinList[k].mode;
        RCC_AHB1PeriphClockCmd(buttonsPinList[k].portDef, ENABLE);
        GPIO_Init(buttonsPinList[k].port, &pinInit);
        GPIO_SetBits(buttonsPinList[k].port, buttonsPinList[k].pin);
        if(!buttonsPinList[k].af) {
            continue;
        }
    }
    isGpioInit = true;
}

bool readButton(uint16_t buttonNumber, bool *rezRead)
{
    if(buttonNumber >= sizeof(buttonsPinList) / sizeof(buttonsPinList[0])) {
        return false;
    }
    if(!isGpioInit) {
        initGpio();
    }
    *rezRead = !GPIO_ReadInputDataBit(buttonsPinList[buttonNumber].port,
                                     buttonsPinList[buttonNumber].pin);
    return true;
}

