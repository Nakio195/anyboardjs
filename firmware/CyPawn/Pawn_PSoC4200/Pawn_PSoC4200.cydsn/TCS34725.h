#ifndef TCS34725_HANDLE_H_PROTECTION
#define TCS34725_HANDLE_H_PROTECTION
   
#include <project.h>
    
    void TCS_Init();
    void TCS_Enable();
    void TCS_SetGain(uint8 Gain);
    void TCS_SetApertureTime(uint8 Time);
    void TCS_ReadColors(uint16 *Red, uint16 *Green, uint16 *Blue, uint16_t *Clear);
    uint8_t TCS_DataReady();
    
#endif
    