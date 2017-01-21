#ifndef ANYBOARDJS_HANDLE_H_PROTECTION
#define ANYBOARDJS_HANDLE_H_PROTECTION
   
#include <project.h>
    
    void TCS_Init();
    void TCS_Enable();
    void TCS_SetGain(uint8 Gain);
    void TCS_SetApertureTime(uint8 Time);
    void TCS_ReadColors(uint16 *Red, uint16 *Green, uint16 *Blue);
    uint8 TCS_DataReady();
    
#endif
    