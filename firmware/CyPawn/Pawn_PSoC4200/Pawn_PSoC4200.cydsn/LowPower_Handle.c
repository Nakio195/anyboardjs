#include <main.h>

extern uint8 shut_down_led;


void HandleLowPowerMode(void)
{
	#ifdef ENABLE_LOW_POWER_MODE
		/* Local variable to store the status of BLESS Hardware block */
		CYBLE_LP_MODE_T sleepMode;
		CYBLE_BLESS_STATE_T blessState;
		
		#ifdef CAPSENSE_ENABLED
		/* Put CapSense to Sleep*/
		CapSense_Sleep();
		#endif
		
		/* Put BLESS into Deep Sleep and check the return status */
		sleepMode = CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP);
		
		/* Disable global interrupt to prevent changes from any other interrupt ISR */
		CyGlobalIntDisable;
	
		/* Check the Status of BLESS */
		blessState = CyBle_GetBleSsState();

		if(sleepMode == CYBLE_BLESS_DEEPSLEEP)
		{
		    /* If the ECO has started or the BLESS can go to Deep Sleep, then place CPU 
			* to Deep Sleep */
			if(blessState == CYBLE_BLESS_STATE_ECO_ON || blessState == CYBLE_BLESS_STATE_DEEPSLEEP)
		    {
				if(shut_down_led)
				{
					/* Put PrISM modules to sleep */
					PrISM_1_Sleep();
					PrISM_2_Sleep();
					
					/* Place CPU to Deep sleep only when the RGB PrISM module is not 
					* active (indicated by flag 'shut_down_led'). 
					* If RGB PrISM is active, then the CPU should only be placed in 
					* Sleep to allow the PrISM to function and control the color 
					* and Intensity */
			        CySysPmDeepSleep();
					
					/* After system wakes up, wake up the PrISM modules*/
					PrISM_1_Wakeup();
					PrISM_2_Wakeup();
				}
				else
				{
					/* If the system is controlling RGB LED, then PrISM needs to be running. 
					* Put CPU to sleep only */
					CySysClkWriteImoFreq(IMO_FREQUENCY_3MHZ);
					CySysPmSleep();
					CySysClkWriteImoFreq(IMO_FREQUENCY_12MHZ);

				}
		 	}
		}
		else
		{
		    if(blessState != CYBLE_BLESS_STATE_EVENT_CLOSE)
		    {
				/* If the BLESS hardware block cannot go to Deep Sleep and BLE Event has not 
				* closed yet, then place CPU to Sleep */
		        CySysPmSleep();
		    }
		}
		
		/* Re-enable global interrupt mask after wakeup */
		CyGlobalIntEnable;
		
		#ifdef CAPSENSE_ENABLED
		/* Wakeup CapSense Block */
		CapSense_Wakeup();
		#endif
		
	#endif
}


void BTN_ISR_Interrupt_InterruptCallback()
{
	BTN_ISR_ClearPending();
}

