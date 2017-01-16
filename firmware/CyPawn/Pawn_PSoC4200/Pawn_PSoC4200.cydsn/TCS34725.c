#include "TCS34725.h"

uint16 TCS_Red = 0;
uint16 TCS_Green = 0;
uint16 TCS_Blue = 0;

void TCS_Enable()
{
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


void TCS_ReadColors(uint16 *Red, uint16 *Green, uint16 *Blue)
{
    I2C_I2CMasterSendStart(0x29, 0);
    I2C_I2CMasterWriteByte(0xB6);
    I2C_I2CMasterSendStop();
    I2C_I2CMasterSendStart(0x29, 1);
    *Red = I2C_I2CMasterReadByte(1);
    *Red += I2C_I2CMasterReadByte(1) << 8;
    *Green = I2C_I2CMasterReadByte(1);
    *Green += I2C_I2CMasterReadByte(1) << 8;
    *Blue = I2C_I2CMasterReadByte(1);
    *Blue += I2C_I2CMasterReadByte(0) << 8;
    I2C_I2CMasterSendStop();
    CyDelay(3);
}

uint8 TCS_DataReady()
{
    uint8 Ready = 0;
    
    I2C_I2CMasterSendStart(0x29, 0);
    I2C_I2CMasterWriteByte(0xB3); 
    I2C_I2CMasterSendRestart(0x29, I2C_I2C_READ_XFER_MODE);
    I2C_I2CMasterReadBuf(0x29, &Ready, 1, I2C_I2C_MODE_COMPLETE_XFER);
    //Ready = I2C_I2CMasterReadByte(1);
    I2C_I2CMasterSendStop();
    
    if(Ready & 0x01)
        return 1;
    else
        return 0;
}