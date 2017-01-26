#include "main.h"

uint8_t NeedProcessing = 0;

void ProcessingRequest()
{
    uint8_t i = 0;
    
    extern uint16_t TCS_Red;
    extern uint16_t TCS_Green;
    extern uint16_t TCS_Blue;
    extern uint16_t TCS_Clear;
    
    extern uint8_t CurrentSector;
    
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
            
                for(i = 0; i < 20; i++)
                {
                    BLE_Buffer[i] = 0;   
                }
                
                BLE_Buffer[ANY_COMMAND] = GET_COLOR;
                /*
                BLE_Buffer[PARAM_GET_COLOR__R_H] = TCS_Red >> 8;
                BLE_Buffer[PARAM_GET_COLOR__R_L] = TCS_Red & 0xFF;
                BLE_Buffer[PARAM_GET_COLOR__G_H] = TCS_Green >> 8;
                BLE_Buffer[PARAM_GET_COLOR__G_L] = TCS_Green & 0xFF;
                BLE_Buffer[PARAM_GET_COLOR__B_H] = TCS_Blue >> 8;
                BLE_Buffer[PARAM_GET_COLOR__B_L] = TCS_Blue & 0xFF;
                
                BLE_Buffer[1] = TCS_Red >> 4;
                BLE_Buffer[2] = TCS_Green >> 4;
                BLE_Buffer[3] = TCS_Blue >> 4;
                BLE_Buffer[4] = TCS_Clear >> 8;
                BLE_Buffer[5] = TCS_Clear & 0xFF;
                */
                BLE_Buffer[1] = CurrentSector;
                
                BLE_Buffer_Updated = 1;
            break;    
                
        case MOVE:
                BLE_Buffer[ANY_COMMAND] = MOVE;
                BLE_Buffer[1] = CurrentSector +0x30;
                
                BLE_Buffer_Updated = 1;
            break;
            
        default:
            BLE_Buffer[ANY_COMMAND] = 0;  //Unable to process request
            break;

    }
    
    NeedProcessing = 0;
    CyDelay(50);
}