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
    

    I2C_Start();
    CyDelay(20);
    TCS_Enable();
    TCS_SetApertureTime(0xC0);
    TCS_SetGain(0x02);

    LED_W_Write(1);
    
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
                LED_R_1_Write(0);
            }  
            
            else
            {
                //TCS_ReadColors(&TCS_Red, &TCS_Green, &TCS_Blue);

                
                while(BTN_Read() == 0);             
            }
            
        }
        
        if(TCS_DataReady())
        {
            TCS_ReadColors(&TCS_Red, &TCS_Green, &TCS_Blue);
        }
    }
}



