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
    extern CYBLE_GATT_HANDLE_VALUE_PAIR_T SerialNotificationCCCDhandle;
    
    CyGlobalIntEnable;   /* Enable global interrupts */
    LED_R_Write(1);
    LED_G_Write(1);
    LED_B_Write(1);

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
            
        }
    }
}



