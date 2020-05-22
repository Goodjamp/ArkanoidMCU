#ifndef __SPI_HAL_H__
#define __SPI_HAL_H__

#include "stdint.h"
#include "stdbool.h"

typedef struct SpiCb {
    void (*spiTxCompliteCb)(void);
    void (*spiRxCompliteCb)(void);
} SpiCb;

void initSPI(SpiCb *spiCbIn);
void spiTx(uint8_t data[], uint32_t size);
void dcControl(bool set);
void resetControl(bool set);

#endif
