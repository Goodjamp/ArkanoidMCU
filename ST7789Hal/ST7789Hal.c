#include "stdint.h"
#include "stdbool.h"

#include "ST7789Hal.h"

#define T16_TURN_ENDIANNESS(X) ((0xFF & ((X) >> 8)) | (0xFF00 & ((X) << 8)))

static St7789Cb st7789CbLocal;

enum ST7789Commands{
    SWRESET   = (uint8_t)0x01, // Software reset
    SLPIN     = (uint8_t)0x10, // Sleep in
    SLPOUT    = (uint8_t)0x11, // Sleep out
    PTLON     = (uint8_t)0x12, // Partial mode on
    NORMON    = (uint8_t)0x13, // Partial off (NORMAL)
    INVOFF    = (uint8_t)0x20, // Display Inversion Off
    INVON     = (uint8_t)0x21, // Display Inversion On
    DISPOFF   = (uint8_t)0x28, // Display off
    DISPON    = (uint8_t)0x29, // Display on
    CASET     = (uint8_t)0x2A, // Column address SET
    RASET     = (uint8_t)0x2B, // Row address set
    RAMWR     = (uint8_t)0x2C, // Memory write
    PTLAR     = (uint8_t)0x30, // Partial start/end address set
    MADCTL    = (uint8_t)0x36, // Memory data access control
    IDMOFF    = (uint8_t)0x38, // Idle mode off
    IDMON     = (uint8_t)0x39, // Idle mode oN
    COLMOD    = (uint8_t)0x3A, // Interface pixel  format
    RAMWRC    = (uint8_t)0x3C, // Memory write continue
    WRDISBV   = (uint8_t)0x51, // Write display brightness
    WRCTRLD   = (uint8_t)0x53, // Write CTRL display
} ST7789Commands;

enum COLMODInterface {
    PIXEL_12BIT = (uint8_t)0b011,
    PIXEL_16BIT = (uint8_t)0b101,
    PIXEL_18BIT = (uint8_t)0b110
} COLMODInterface;

#pragma pack(push, 1)
struct PartialStartEnd {
    uint16_t startAddress;
    uint16_t endAddress;
};
union PartialStartEndFormat {
    struct PartialStartEnd format;
    uint8_t buff[sizeof(struct PartialStartEnd)];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct InterfacePixelFormat {
    uint8_t pixelFormat;
};

union InterfacePixelFormatFormat {
    struct InterfacePixelFormat format;
    uint8_t buff[sizeof(struct InterfacePixelFormat)];
};
#pragma pack(pop)


static inline void sendCommand(enum ST7789Commands command)
{
    st7789CbLocal.dcControlCb(false);
    st7789CbLocal.txCb(&command, 1);
    st7789CbLocal.dcControlCb(true);
}

static inline void sendData(uint8_t data[], uint32_t size)
{
    st7789CbLocal.dcControlCb(true);
    st7789CbLocal.txCb(data, size);
}


void st7789SetDefInitStruct(St7789InitStruct *initStruct)
{
    initStruct->rowStartAddress = 0;
    initStruct->rowEndAddress   = 240 - 1;
}


void st7789Init(St7789Cb st7789Cb, St7789InitStruct initStruct)
{
    uint32_t cnt = 0;
    st7789CbLocal = st7789Cb;
    //set D/C pin in data state
    st7789CbLocal.dcControlCb(true);
    /*Initializations and enable display*/
    sendCommand(SWRESET);
    while(cnt < 1600000) {
        cnt++;
    }
    cnt = 0;
    sendCommand(SLPOUT);
    while(cnt < 1600000) {
        cnt++;
    }
    /*
    union PartialStartEndFormat partialSettings =
    {
        .format.startAddress = initStruct.rowStartAddress,
        .format.endAddress   = initStruct.rowEndAddress
    };
    sendCommand(PTLAR);
    sendData(partialSettings.buff, sizeof(partialSettings.buff));
    */

    //sendCommand(PTLON);
    sendCommand(NORMON);
    sendCommand(COLMOD);
    union InterfacePixelFormatFormat pixelFormatSettings =
    {
        .format.pixelFormat = PIXEL_16BIT
    };
    sendData(pixelFormatSettings.buff, sizeof(pixelFormatSettings.buff));
    sendCommand(INVON);
    sendCommand(DISPON);
}

void st7789SetCursor(uint16_t x, uint16_t y)
{

}

void st7789SetArea(uint16_t x, uint16_t y, uint16_t areaWidth, uint16_t areaHeight)
{
#pragma pack(push, 1)
    struct SetAreaCommand {
        uint16_t startCoord;
        uint16_t stopCoord;
    };
    union SetAreaCommandBuff{
        uint8_t buff[sizeof(struct SetAreaCommand)];
        struct SetAreaCommand setAreaCommand;
    };
#pragma pack(pop)
    union  SetAreaCommandBuff setAreaCommandBuff;
    setAreaCommandBuff.setAreaCommand.startCoord = T16_TURN_ENDIANNESS(x);
    setAreaCommandBuff.setAreaCommand.stopCoord  = T16_TURN_ENDIANNESS(x + areaWidth - 1);
    sendCommand(CASET);
    sendData(setAreaCommandBuff.buff, sizeof(setAreaCommandBuff.buff));
    setAreaCommandBuff.setAreaCommand.startCoord = T16_TURN_ENDIANNESS(y);
    setAreaCommandBuff.setAreaCommand.stopCoord  = T16_TURN_ENDIANNESS(y + areaHeight - 1);
    sendCommand(RASET);
    sendData(setAreaCommandBuff.buff, sizeof(setAreaCommandBuff.buff));
}

void st7789WriteFrame(uint8_t frame[], uint32_t size)
{
    sendCommand(RAMWR);
    sendData(frame, size);
}
