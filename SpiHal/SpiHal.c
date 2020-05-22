
#include "stm32f4xx_conf.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_rcc.h"

#include "SpiHal.h"
// Busy
static volatile bool txInProcess = true;

struct {
    SPI_TypeDef *spi;
    void (*clockEbable)(uint32_t, FunctionalState);
    uint32_t clockSrc;
    uint32_t gpiAf;
} userSpi = {
    .spi         = SPI1,
    .clockEbable = RCC_APB2PeriphClockCmd,
    .clockSrc    = RCC_APB2Periph_SPI1,
    .gpiAf       = GPIO_AF_SPI1,
};
struct {
    DMA_Stream_TypeDef *dmatTx;
    uint32_t    channelTx;
    IRQn_Type   dmaTxIRQ;
    void (*clockEbable)(uint32_t, FunctionalState);
    uint32_t clockSrc;
} userDma = {
    .dmatTx      = DMA2_Stream3,
    .channelTx   = DMA_Channel_3,
    .dmaTxIRQ    = DMA2_Stream3_IRQn,
    .clockEbable = RCC_AHB1PeriphClockCmd,
    .clockSrc    = RCC_AHB1Periph_DMA2,
};

static SpiCb spiCb;

enum {
    SPI_PIN_MOSI,
    SPI_PIN_SCK,
    SPI_PIN_CS,
    PIN_DC,
    PIN_RST,
    PIN_BL
};

static const struct {
    uint32_t         pin;
    uint32_t         pinSrc;
    GPIO_TypeDef*    port;
    GPIOMode_TypeDef mode;
    uint32_t         portDef;
    bool             af;
} displayPin[] = {
    [SPI_PIN_MOSI] = {
        .pin     = GPIO_Pin_7,
        .pinSrc  = GPIO_PinSource7,
        .port    = GPIOA,
        .mode    = GPIO_Mode_AF,
        .portDef = RCC_AHB1Periph_GPIOA,
        .af      = true,
    },
    [SPI_PIN_SCK] = {
        .pin     = GPIO_Pin_5,
        .pinSrc  = GPIO_PinSource5,
        .port    = GPIOA,
        .mode    = GPIO_Mode_AF,
        .portDef = RCC_AHB1Periph_GPIOA,
        .af      = true,
    },
    [SPI_PIN_CS] = {
        .pin     = GPIO_Pin_6,
        .pinSrc  = GPIO_PinSource6,
        .port    = GPIOA,
        .mode    = GPIO_Mode_OUT,
        .portDef = RCC_AHB1Periph_GPIOA,
        .af      = false,
    },
    [PIN_DC] = {
        .pin     = GPIO_Pin_1,
        .pinSrc  = GPIO_PinSource1,
        .port    = GPIOB,
        .mode    = GPIO_Mode_OUT,
        .portDef = RCC_AHB1Periph_GPIOB,
        .af      = false,
    },
    [PIN_RST] = {
        .pin     = GPIO_Pin_4,
        .pinSrc  = GPIO_PinSource4,
        .port    = GPIOA,
        .mode    = GPIO_Mode_OUT,
        .portDef = RCC_AHB1Periph_GPIOA,
        .af      = false,
    },
    [PIN_BL] = {
        .pin     = GPIO_Pin_0,
        .pinSrc  = GPIO_PinSource0,
        .port    = GPIOB,
        .mode    = GPIO_Mode_OUT,
        .portDef = RCC_AHB1Periph_GPIOB,
        .af      = false,
    },
};

static void initSPIGpio(void){
    GPIO_InitTypeDef pinInit = {
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_OType = GPIO_OType_PP,
        .GPIO_PuPd  = GPIO_PuPd_NOPULL,
    };
    for(uint32_t k = 0; k < sizeof(displayPin) / sizeof(displayPin[0]); k++) {
        pinInit.GPIO_Pin = displayPin[k].pin;
        pinInit.GPIO_Mode = displayPin[k].mode;
        RCC_AHB1PeriphClockCmd(displayPin[k].portDef, ENABLE);
        GPIO_Init(displayPin[k].port, &pinInit);
        GPIO_SetBits(displayPin[k].port, displayPin[k].pin);
        if(!displayPin[k].af) {
            continue;
        }
        GPIO_PinAFConfig(displayPin[k].port, displayPin[k].pinSrc, userSpi.gpiAf);
    }
}

static void initSpiDma(void)
{
    RCC_ClocksTypeDef rccClock;
    RCC_GetClocksFreq(&rccClock);
    SPI_InitTypeDef spiInit;
    spiInit.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    spiInit.SPI_CPHA              = SPI_CPHA_1Edge;
    spiInit.SPI_CPOL              = SPI_CPOL_Low;
    spiInit.SPI_CRCPolynomial     = 0; //?????
    spiInit.SPI_DataSize          = SPI_DataSize_8b;
    spiInit.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
    spiInit.SPI_FirstBit          = SPI_FirstBit_MSB;
    spiInit.SPI_Mode              = SPI_Mode_Master;
    spiInit.SPI_NSS               = SPI_NSS_Soft;
    userSpi.clockEbable(userSpi.clockSrc, ENABLE);
    SPI_Init(userSpi.spi, &spiInit);
    SPI_I2S_DMACmd(userSpi.spi, (SPI_I2S_DMAReq_Tx), ENABLE);
    SPI_Cmd(userSpi.spi, ENABLE);
    /**Config DMA SPI Tx*/
    DMA_InitTypeDef dmaInit;
    dmaInit.DMA_BufferSize         = 0;
    dmaInit.DMA_Channel            = userDma.channelTx;
    dmaInit.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
    dmaInit.DMA_FIFOMode           = DMA_FIFOMode_Disable;
    dmaInit.DMA_Memory0BaseAddr    = 0;
    dmaInit.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
    dmaInit.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    dmaInit.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    dmaInit.DMA_Mode               = DMA_Mode_Normal;
    dmaInit.DMA_PeripheralBaseAddr = (uint32_t)&userSpi.spi->DR;
    dmaInit.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
    dmaInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dmaInit.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    dmaInit.DMA_Priority           = DMA_Priority_High;
    userDma.clockEbable(userDma.clockSrc, ENABLE);
    DMA_Init(userDma.dmatTx, &dmaInit);
    DMA_ITConfig(userDma.dmatTx, DMA_IT_TC, ENABLE);
    NVIC_EnableIRQ(userDma.dmaTxIRQ);
};

void initSPI(SpiCb *spiCbIn)
{
    spiCb = *spiCbIn;
    initSPIGpio();
    initSpiDma();
}


void DMA2_Stream3_IRQHandler(void)
{
    DMA_ClearITPendingBit(userDma.dmatTx, DMA_IT_FEIF3 | DMA_IT_DMEIF3 |DMA_IT_TEIF3 | DMA_IT_HTIF3 | DMA_IT_TCIF3 );
    //DMA_Cmd(userDma.dmatTx, DISABLE);
    txInProcess = false;
    /*
    if(spiCb.spiTxCompliteCb == NULL) {
        return;
    }
    */
}
#define TX_SIZE_MAX 65535

void spiTx(uint8_t data[], uint32_t size)
{
    volatile uint16_t sizeTemp = 0;
    volatile uint32_t addressTemp = 0;
    GPIO_ResetBits(displayPin[SPI_PIN_CS].port, displayPin[SPI_PIN_CS].pin);
    while(size) {
        sizeTemp = (size > TX_SIZE_MAX) ? (TX_SIZE_MAX) : (size);
        txInProcess = true;
        DMA_SetCurrDataCounter(userDma.dmatTx, sizeTemp);
        DMA_MemoryTargetConfig(userDma.dmatTx, (uint32_t)&data[addressTemp], DMA_Memory_0);
        DMA_ClearITPendingBit(userDma.dmatTx, DMA_IT_FEIF3 | DMA_IT_DMEIF3 |DMA_IT_TEIF3 | DMA_IT_HTIF3 | DMA_IT_TCIF3 );
        DMA_Cmd(userDma.dmatTx, ENABLE);
        while(txInProcess) {}
        while(SPI_I2S_GetFlagStatus(userSpi.spi, SPI_I2S_FLAG_TXE) == RESET) {};
        while(SPI_I2S_GetFlagStatus(userSpi.spi, SPI_I2S_FLAG_BSY)) {};
        addressTemp += sizeTemp;
        size -= sizeTemp;
    }
    GPIO_SetBits(displayPin[SPI_PIN_CS].port, displayPin[SPI_PIN_CS].pin);
}

/*
#define DMA_IT_FEIF3 | DMA_IT_DMEIF3 |DMA_IT_TEIF3 | DMA_IT_HTIF3 | DMA_IT_TCIF3

*/

void spiTx_(uint8_t data[], uint32_t size)
{
    GPIO_ResetBits(displayPin[SPI_PIN_CS].port, displayPin[SPI_PIN_CS].pin);
    DMA_SetCurrDataCounter(userDma.dmatTx, size);
    DMA_MemoryTargetConfig(userDma.dmatTx, (uint32_t)data, DMA_Memory_0);
    txInProcess = true;
    DMA_ClearITPendingBit(userDma.dmatTx, DMA_IT_FEIF3 | DMA_IT_DMEIF3 |DMA_IT_TEIF3 | DMA_IT_HTIF3 | DMA_IT_TCIF3 );
    DMA_Cmd(userDma.dmatTx, ENABLE);
    while(txInProcess) {}
    while(SPI_I2S_GetFlagStatus(userSpi.spi, SPI_I2S_FLAG_BSY)) {};

    GPIO_SetBits(displayPin[SPI_PIN_CS].port, displayPin[SPI_PIN_CS].pin);
}



void dcControl(bool set)
{
    GPIO_WriteBit(displayPin[PIN_DC].port,
                  displayPin[PIN_DC].pin,
                  set ? Bit_SET : Bit_RESET);

}

void resetControl(bool set)
{
    GPIO_WriteBit(displayPin[PIN_RST].port,
                  displayPin[PIN_RST].pin,
                  set ? Bit_SET : Bit_RESET);

}

