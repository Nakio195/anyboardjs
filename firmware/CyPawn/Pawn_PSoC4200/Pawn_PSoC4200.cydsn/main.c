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


uint8_t CurrentSector = 0;
    
int main()
{
    extern uint8_t NeedProcessing;
    extern uint8_t deviceConnected;

    extern uint32 TCS_Red;
    extern uint32 TCS_Green;
    extern uint32 TCS_Blue;
    extern uint32 TCS_Clear;
    
    extern uint16 TCS_Red_Buffer[10];
    extern uint16 TCS_Green_Buffer[10];
    extern uint16 TCS_Blue_Buffer[10];
    extern uint16 TCS_Clear_Buffer[10];
    
    uint8_t LastCurrentSector = 0;
    uint8_t TCS_UpdateGain = 1;
    uint8_t TCS_CurrentGain = 1;
    
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t PawnMoved = 0;
    
    uint8_t Tempo_BeforeSendingMOVE = 0;
    
    uint8_t TCS_DataAcquired = 0;
    
    uint16_t BatteryLevel = 0;
    
    extern CYBLE_GATT_HANDLE_VALUE_PAIR_T SerialNotificationCCCDhandle;
   
    
    CyGlobalIntEnable;   /* Enable global interrupts */
    

    I2C_Start();
    CyDelay(20);
    TCS_Enable();
    TCS_SetApertureTime(0xC0);
    TCS_SetGain(2);

    LED_W_Write(1);

    if(BTN_Read() == 0)
        BatteryLevel = 0;
    else
        BatteryLevel = 1;
    
    
    while(BTN_Read() != 1);
    
    if(BatteryLevel == 0)
    {
        LED_R_1_Write(0);
        LED_G_1_Write(0);
        LED_B_1_Write(1);   
        CyDelay(500);
        TCS_SetGain(3);
    }
    
    if(BatteryLevel == 1)
    {
        LED_R_1_Write(1);
        LED_G_1_Write(0);
        LED_B_1_Write(1);   
        CyDelay(500);
    }
    
    LED_R_1_Write(1);
    LED_G_1_Write(1);
    LED_B_1_Write(1); 
    
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
                
                BLE_Buffer[ANY_COMMAND] = MOVE;
                NeedProcessing = 1;
                while(BTN_Read() == 0);             
            }
            
        }
        
        if(TCS_DataReady())
        {
            TCS_ReadColors(&TCS_Red_Buffer[i], &TCS_Green_Buffer[i], &TCS_Blue_Buffer[i], &TCS_Clear_Buffer[i]);
            
            for(j = 0; j < 10; j++)
            {
                TCS_Red += TCS_Red_Buffer[j];
                TCS_Green += TCS_Green_Buffer[j];
                TCS_Blue += TCS_Blue_Buffer[j];
                TCS_Clear += TCS_Clear_Buffer[j];
            }
            
            TCS_Red /= 10;
            TCS_Green /= 10;
            TCS_Blue /= 10;
            TCS_Clear /= 10;
            
            i++;
            
            if(i == 10)
            {
                i = 0;
                TCS_DataAcquired = 1;
            }

            CyDelay(50);

            if (TCS_Clear > 0x1800)
                CurrentSector = 1;
            else if (TCS_Clear > 0x11A0 && TCS_Clear < 0x1800 && (TCS_Red >> 4) > 0x90)
                CurrentSector = 2;
            else if (TCS_Clear > 0x0F00 && TCS_Clear < 0x11A0)
                CurrentSector = 3;

            else if (TCS_Clear < 0x0F00)
                CurrentSector = 5;

            // Sends sectors ID of the sector that has been left and the sector that has been reached
            if (CurrentSector != LastCurrentSector && TCS_DataAcquired)
            {
                TCS_DataAcquired = 0;
                // Update sector_ID variables
                LastCurrentSector = CurrentSector;
                PawnMoved = 1;
                Tempo_BeforeSendingMOVE = 0;
            }
            
            if(PawnMoved)
            {
                if(Tempo_BeforeSendingMOVE == 0)
                {
                    BLE_Buffer[0] = MOVE;
                    BLE_Buffer[1] = CurrentSector;

                    //NeedProcessing = 1;
                    BLE_Buffer_Updated = 1;
                    PawnMoved = 0;
                }
                
                else
                    Tempo_BeforeSendingMOVE --;
            }
           
        }
    }
}



