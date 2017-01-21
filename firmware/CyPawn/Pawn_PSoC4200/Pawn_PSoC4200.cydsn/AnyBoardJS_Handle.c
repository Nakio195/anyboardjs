#include "main.h"

uint8_t NeedProcessing = 0;

void ProcessingRequest()
{
    extern uint16 TCS_Red;
    extern uint16 TCS_Green;
    extern uint16 TCS_Blue;
    
    switch(BLE_Buffer[ANY_COMMAND])
    {
        case LED_ON :
            //Processing request
            if(BLE_Buffer[PARAM_LED_ON__R])
                LED_R_Write(0);
            else
                LED_R_Write(1);
                
            if(BLE_Buffer[PARAM_LED_ON__G])
                LED_G_Write(0);
            else
                LED_G_Write(1);
                
            if(BLE_Buffer[PARAM_LED_ON__B])
                LED_B_Write(0);
            else
                LED_B_Write(1);
                
            //Creating answer
            BLE_Buffer[ANY_COMMAND] = LED_ON;
            
            //Answer ready to be sent
            BLE_Buffer_Updated = 1;
            break;
        
        case GET_COLOR:
                BLE_Buffer[ANY_COMMAND] = GET_COLOR;
                BLE_Buffer[PARAM_GET_COLOR__R_H] = TCS_Red >> 8;
                BLE_Buffer[PARAM_GET_COLOR__R_L] = TCS_Red & 0xFF;
                BLE_Buffer[PARAM_GET_COLOR__G_H] = TCS_Green >> 8;
                BLE_Buffer[PARAM_GET_COLOR__G_L] = TCS_Green & 0xFF;
                BLE_Buffer[PARAM_GET_COLOR__B_H] = TCS_Blue >> 8;
                BLE_Buffer[PARAM_GET_COLOR__B_L] = TCS_Blue & 0xFF;
                
                BLE_Buffer_Updated = 1;
            break;
            
        default:
            BLE_Buffer[ANY_COMMAND] = 0;  //Unable to process request
            break;
            
    }
    
    NeedProcessing = 0;
}