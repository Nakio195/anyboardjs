#include "TCS34725.h"

uint16 TCS_Red = 0;
uint16 TCS_Green = 0;
uint16 TCS_Blue = 0;

void TCS_Init()
{
    I2C_Start();
    
    I2C_I2CMasterSendStart(TCS34725_ADDRESS, 0);
    I2C_I2CMasterWriteByte(0xA0);
    I2C_I2CMasterWriteByte(0x03);
    I2C_I2CMasterWriteByte(0xFF);
    I2C_I2CMasterSendStop();
    
    I2C_I2CMasterSendStart(TCS34725_ADDRESS, 0);
    I2C_I2CMasterWriteByte(0xAF);
    I2C_I2CMasterWriteByte(0x02);
    I2C_I2CMasterSendStop();
    
    I2C_I2CMasterSendStart(TCS34725_ATIME, TCS34725_INTEGRATIONTIME_154MS);
    I2C_I2CMasterWriteByte(0xAF);
    I2C_I2CMasterWriteByte(0x02);
    I2C_I2CMasterSendStop();

}

void TCS_ReadRawData()
{
    uint8_t R_L, R_H, G_L, G_H, B_L, B_H;
    
    I2C_I2CMasterSendStart(TCS34725_ADDRESS, 0);
    I2C_I2CMasterWriteByte(0xB6);
    I2C_I2CMasterSendRestart(TCS34725_ADDRESS, 1);

    R_L = I2C_I2CMasterReadByte(1);
    CyDelay(1);
    R_H = I2C_I2CMasterReadByte(1);
    CyDelay(1);
    G_L = I2C_I2CMasterReadByte(1);
    CyDelay(1);
    G_H = I2C_I2CMasterReadByte(1);
    CyDelay(1);
    B_L = I2C_I2CMasterReadByte(1);
    CyDelay(1);
    B_H = I2C_I2CMasterReadByte(1);
    I2C_I2CMasterSendStop();
    CyDelay(154);
    
    TCS_Red = R_L + (R_H << 8);
    TCS_Green = G_L + (G_H << 8);
    TCS_Blue = B_L + (B_H << 8);
    
    return;
}