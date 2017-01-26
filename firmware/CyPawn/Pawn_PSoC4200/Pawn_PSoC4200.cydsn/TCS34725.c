#include "TCS34725.h"

uint32 TCS_Red = 0;
uint32 TCS_Green = 0;
uint32 TCS_Blue = 0;
uint32 TCS_Clear = 0;

uint16 TCS_Red_Buffer[10] = {0};
uint16 TCS_Green_Buffer[10] = {0};
uint16 TCS_Blue_Buffer[10] = {0};
uint16 TCS_Clear_Buffer[10] = {0};

void TCS_Enable()
{
    I2C_I2CMasterSendStart(0x29, 0);
    I2C_I2CMasterWriteByte(0xA0);
    I2C_I2CMasterWriteByte(0x00);
    I2C_I2CMasterSendStop();
    CyDelay(10);
    I2C_I2CMasterSendStart(0x29, 0);
    I2C_I2CMasterWriteByte(0xA0);
    I2C_I2CMasterWriteByte(0x0B);
    I2C_I2CMasterSendStop();
}

void TCS_SetGain(uint8 Gain)
{
    I2C_I2CMasterSendStart(0x29, 0);
    I2C_I2CMasterWriteByte(0xAF);
    I2C_I2CMasterWriteByte(Gain);
    I2C_I2CMasterSendStop();        
}

void TCS_SetApertureTime(uint8 Time)
{
    I2C_I2CMasterSendStart(0x29, 0);
    I2C_I2CMasterWriteByte(0xA1);
    I2C_I2CMasterWriteByte(Time);
    I2C_I2CMasterSendStop();        
}


void TCS_ReadColors(uint16 *Red, uint16 *Green, uint16 *Blue, uint16_t *Clear)
{

    I2C_I2CMasterSendStart(0x29, 0);
    I2C_I2CMasterWriteByte(0xB4);
    I2C_I2CMasterSendRestart(0x29, 1);
    *Clear = I2C_I2CMasterReadByte(0);
    *Clear += I2C_I2CMasterReadByte(0) << 8;    
    *Red = I2C_I2CMasterReadByte(0);
    *Red += I2C_I2CMasterReadByte(0) << 8;
    *Green = I2C_I2CMasterReadByte(0);
    *Green += I2C_I2CMasterReadByte(0) << 8;
    *Blue = I2C_I2CMasterReadByte(0);
    *Blue += I2C_I2CMasterReadByte(1) << 8;
    I2C_I2CMasterSendStop();
    CyDelay(3);
}

uint8_t TCS_DataReady()
{
    uint8 Ready = 0;
    
    I2C_I2CMasterSendStart(0x29, 0);
    I2C_I2CMasterWriteByte(0xB3);
    I2C_I2CMasterSendRestart(0x29, 1);
    Ready = I2C_I2CMasterReadByte(1);
    I2C_I2CMasterSendStop();
    
    if(Ready & 0x01)
        return 1;
    else
        return 0;
}