#include "main.h"

uint8_t NeedProcessing = 0;

void ProcessingRequest()
{
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
            
        default:
            BLE_Buffer[ANY_COMMAND] = 0;  //Unable to process request
            break;
            
    }
    
    NeedProcessing = 0;
}