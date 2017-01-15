/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include <main.h>


int main()
{
    extern uint8_t NeedProcessing;
    extern uint8_t deviceConnected;
    extern uint16 TCS_Red;
    extern uint16 TCS_Green;
    extern uint16 TCS_Blue;
    extern CYBLE_GATT_HANDLE_VALUE_PAIR_T SerialNotificationCCCDhandle;
    
    CyGlobalIntEnable;   /* Enable global interrupts */
    LED_R_Write(1);
    LED_G_Write(1);
    LED_B_Write(1);

    TCS_Init();
    CyBle_Start(StackEventHandler);
    
    for(;;)
    {
        CyBle_ProcessEvents();
        
        if(NeedProcessing)
            ProcessingRequest();
        
    	if(SerialNotificationCCCDhandle.value.val[0])
			SendDataOverSerialNotification(BLE_Buffer);
        
        if(BTN_Read() == 0)
        {
            if(!deviceConnected)
            {
                CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
                while(BTN_Read() == 0);
                LED_R_Write(0);
            }  
            
            else
            {
                TCS_ReadRawData();
                BLE_Buffer[ANY_COMMAND] = GET_COLOR;
                BLE_Buffer[PARAM_GET_COLOR__R] = TCS_Red;
                BLE_Buffer[PARAM_GET_COLOR__G] = TCS_Green;
                BLE_Buffer[PARAM_GET_COLOR__B] = TCS_Blue;
                
                BLE_Buffer_Updated = 1;                
            }
            
        }
    }
}



