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

    I2C_Start();
    
    TCS_Enable();
    TCS_SetApertureTime(0xFF);
    TCS_SetGain(0x02);
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
                //TCS_ReadColors(&TCS_Red, &TCS_Green, &TCS_Blue);
                BLE_Buffer[ANY_COMMAND] = GET_COLOR;
                BLE_Buffer[PARAM_GET_COLOR__R_H] = TCS_Red >> 8;
                BLE_Buffer[PARAM_GET_COLOR__R_L] = TCS_Red & 0xFF;
                BLE_Buffer[PARAM_GET_COLOR__G_H] = TCS_Green >> 8;
                BLE_Buffer[PARAM_GET_COLOR__G_L] = TCS_Green & 0xFF;
                BLE_Buffer[PARAM_GET_COLOR__B_H] = TCS_Blue >> 8;
                BLE_Buffer[PARAM_GET_COLOR__B_L] = TCS_Blue & 0xFF;
                
                while(BTN_Read() == 0);
                BLE_Buffer_Updated = 1;                
            }
            
        }
        
        if(TCS_DataReady())
        {
            TCS_ReadColors(&TCS_Red, &TCS_Green, &TCS_Blue);
        }
    }
}



