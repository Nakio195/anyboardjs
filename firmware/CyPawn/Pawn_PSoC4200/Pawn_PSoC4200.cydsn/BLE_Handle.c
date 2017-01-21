#include "main.h"


CYBLE_CONN_HANDLE_T  connectionHandle;
uint8_t deviceConnected = 0;
uint8_t busyStatus = 0;

CYBLE_GATT_HANDLE_VALUE_PAIR_T SerialNotificationCCCDhandle;
uint8_t SerialCCCDValue[2];

CYBLE_GATT_HANDLE_VALUE_PAIR_T	BLE_BufferHandle;	
uint8_t BLE_Buffer[20] = {0};
uint8_t BLE_Buffer_Updated = 0;

extern uint8_t NeedProcessing;



void SendDataOverSerialNotification(uint8_t *Buffer)
{
	/* 'CapSensenotificationHandle' stores CapSense notification data parameters */
	CYBLE_GATTS_HANDLE_VALUE_NTF_T		SerialNotificationHandle;	
	
    if(BLE_Buffer_Updated == 0)
        return;
	/* If stack is not busy, then send the notification */
	if(busyStatus == CYBLE_STACK_STATE_FREE)
	{
		/* Update notification handle with CapSense slider data*/
		SerialNotificationHandle.attrHandle = CYBLE_CYPAWN_ANYBOARDJS_SERIAL_CHAR_HANDLE;				
		SerialNotificationHandle.value.val = Buffer;
		SerialNotificationHandle.value.len = 20;
		
		/* Send the updated handle as part of attribute for notifications */
		CyBle_GattsNotification(connectionHandle, &SerialNotificationHandle);
        BLE_Buffer_Updated = 0;
	}
}

void StackEventHandler( uint32 eventCode, void *eventParam )
{
    volatile CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam;
    char i = 0;
    
    switch(eventCode)
    {
        case CYBLE_EVT_STACK_ON:
			/* This event is received when component is Started */
			
			/* Set restartAdvertisement flag to allow calling Advertisement 
			* API from main function */
			
			break;
			
		case CYBLE_EVT_TIMEOUT:
			/* Event Handling for Timeout  */
	
			break;
        
		/**********************************************************
        *                       GAP Events
        ***********************************************************/
		case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
			
			break;
			
        
		/**********************************************************
        *                       GATT Events
        ***********************************************************/
        case CYBLE_EVT_GATT_CONNECT_IND:
			/* This event is received when device is connected over GATT level */
			
            connectionHandle = *(CYBLE_CONN_HANDLE_T  *)eventParam;
            
            deviceConnected = 1;
            
            LED_R_1_Write(1);
            LED_G_1_Write(0);
            LED_B_1_Write(1);
			break;
        
        case CYBLE_EVT_GATT_DISCONNECT_IND:
			/* This event is received when device is disconnected */

			
			break;
			
		case CYBLE_EVT_L2CAP_CONN_PARAM_UPDATE_RSP:
				/* If L2CAP connection parameter update response received, reset application flag */

            break;
			
		case CYBLE_EVT_STACK_BUSY_STATUS:
			/* This event is generated when the internal stack buffer is full and no more
			* data can be accepted or the stack has buffer available and can accept data.
			* This event is used by application to prevent pushing lot of data to stack. */
			busyStatus = * (uint8*)eventParam;

            break;
            
		case CYBLE_EVT_GAP_DEVICE_CONNECTED:

        break;

        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            
            deviceConnected = 0;
            
            LED_R_1_Write(0);
            LED_G_1_Write(1);
            LED_B_1_Write(1);
        break;


        case CYBLE_EVT_GATTS_WRITE_REQ:
            wrReqParam = (CYBLE_GATTS_WRITE_REQ_PARAM_T *) eventParam;
            
            LED_R_1_Write(1);
            LED_G_1_Write(1);
            LED_B_1_Write(0);
            
            CyDelay(50);
            
            if(CYBLE_CYPAWN_ANYBOARDJS_SERIAL_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE == wrReqParam->handleValPair.attrHandle)
            {
                
                SerialCCCDValue[0] = wrReqParam->handleValPair.value.val[CYBLE_CYPAWN_ANYBOARDJS_SERIAL_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX];
                SerialCCCDValue[1] = 0;
                
                SerialNotificationCCCDhandle.attrHandle = CYBLE_CYPAWN_ANYBOARDJS_SERIAL_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
                SerialNotificationCCCDhandle.value.val = SerialCCCDValue;
                SerialNotificationCCCDhandle.value.len = 2;
                
                CyBle_GattsWriteAttributeValue(&SerialNotificationCCCDhandle, 0, &connectionHandle, CYBLE_GATT_DB_PEER_INITIATED);
                
            }
            			/* This event is received when Central device sends a Write command on an Attribute */
             
            if(CYBLE_CYPAWN_ANYBOARDJS_SERIAL_CHAR_HANDLE == wrReqParam->handleValPair.attrHandle)
            {
                for(i = 0; i < wrReqParam->handleValPair.value.len; i++)
                {
                    BLE_Buffer[i] = (uint8_t)wrReqParam->handleValPair.value.val[i];
                }
                
                BLE_BufferHandle.attrHandle = CYBLE_CYPAWN_ANYBOARDJS_SERIAL_CHAR_HANDLE;
                BLE_BufferHandle.value.val = BLE_Buffer;
                BLE_BufferHandle.value.len = 20;
                
                
                CyBle_GattsWriteAttributeValue(&BLE_BufferHandle, 0, &connectionHandle, CYBLE_GATT_DB_PEER_INITIATED);
                
                NeedProcessing = 1;
            }
            
            
            CyBle_GattsWriteRsp(connectionHandle);
            
            LED_R_1_Write(1);
            LED_G_1_Write(0);
            LED_B_1_Write(1);
            
            break;
            
//           if(CYBLE_CAPSENSE_CAPSENSE_SLIDER_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE == wrReqParam->handleValPair.attrHandle)
//			{
//				/* Extract the Write value sent by the Client for CapSense Slider CCCD */
//                if(wrReqParam->handleValPair.value.val[CYBLE_CAPSENSE_CAPSENSE_SLIDER_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX] == TRUE)
//                {
//                    sendCapSenseSliderNotifications = TRUE;
//                }
//                else if(wrReqParam->handleValPair.value.val[CYBLE_CAPSENSE_CAPSENSE_SLIDER_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX] == FALSE)
//                {
//                    sendCapSenseSliderNotifications = FALSE;
//                }
//				
//        		/* Write the present CapSense notification status to the local variable */
//        		CapSenseCCCDvalue[0] = sendCapSenseSliderNotifications;
//        		CapSenseCCCDvalue[1] = 0x00;
//        		
//        		/* Update CCCD handle with notification status data*/
//        		CapSenseNotificationCCCDhandle.attrHandle = CYBLE_CAPSENSE_CAPSENSE_SLIDER_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
//        		CapSenseNotificationCCCDhandle.value.val = CapSenseCCCDvalue;
//        		CapSenseNotificationCCCDhandle.value.len = CCCD_DATA_LEN;
//        		
//        		/* Report data to BLE component for sending data when read by Central device */
//        		CyBle_GattsWriteAttributeValue(&CapSenseNotificationCCCDhandle, ZERO, &connectionHandle, CYBLE_GATT_DB_PEER_INITIATED);
//            }
//			
//			if(CYBLE_RGB_LED_RGB_LED_CONTROL_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE == wrReqParam->handleValPair.attrHandle)
//            {
//				/* Extract the Write value sent by the Client for RGB LED CCCD */
//                if(wrReqParam->handleValPair.value.val[CYBLE_RGB_LED_RGB_LED_CONTROL_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX] == TRUE)
//                {
//                    rgbledNotifications = TRUE;
//                }
//                else if(wrReqParam->handleValPair.value.val[CYBLE_RGB_LED_RGB_LED_CONTROL_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX] == FALSE)
//                {
//                    rgbledNotifications = FALSE;
//                }
//				
//        		/* Write the present RGB notification status to the local variable */
//        		RGBCCCDvalue[0] = rgbledNotifications;
//        		RGBCCCDvalue[1] = 0x00;
//        		
//        		/* Update CCCD handle with notification status data*/
//        		RGBNotificationCCCDhandle.attrHandle = CYBLE_RGB_LED_RGB_LED_CONTROL_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
//        		RGBNotificationCCCDhandle.value.val = RGBCCCDvalue;
//        		RGBNotificationCCCDhandle.value.len = CCCD_DATA_LEN;
//        		
//        		/* Report data to BLE component for sending data when read by Central device */
//        		CyBle_GattsWriteAttributeValue(&RGBNotificationCCCDhandle, ZERO, &connectionHandle, CYBLE_GATT_DB_PEER_INITIATED);
//				
//				/* Update the RGB LED Notification attribute with new color coordinates */
//				SendDataOverRGBledNotification(RGBledData, RGB_CHAR_DATA_LEN);
//            }
//			
//			/* Check if the returned handle is matching to RGB LED Control Write Attribute and extract the RGB data*/
//            if(CYBLE_RGB_LED_RGB_LED_CONTROL_CHAR_HANDLE == wrReqParam->handleValPair.attrHandle)
//            {
//				/* Extract the Write value sent by the Client for RGB LED Color characteristic */
//                RGBledData[RED_INDEX] = wrReqParam->handleValPair.value.val[RED_INDEX];
//                RGBledData[GREEN_INDEX] = wrReqParam->handleValPair.value.val[GREEN_INDEX];
//                RGBledData[BLUE_INDEX] = wrReqParam->handleValPair.value.val[BLUE_INDEX];
//                RGBledData[INTENSITY_INDEX] = wrReqParam->handleValPair.value.val[INTENSITY_INDEX];
//                
//				/* Update the PrISM components and the attribute for RGB LED read characteristics*/
//                UpdateRGBled();
//            }
//			
//			/* Send the response to the write request received. */
//			CyBle_GattsWriteRsp(connectionHandle);
//            CyBle_GattsWriteRsp(connectionHandle);
//        break;
      
        default:

       	 	break;
    }   	/* switch(event) */



}