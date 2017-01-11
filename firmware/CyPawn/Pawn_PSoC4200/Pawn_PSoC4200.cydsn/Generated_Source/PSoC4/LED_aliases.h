/*******************************************************************************
* File Name: LED.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_LED_ALIASES_H) /* Pins LED_ALIASES_H */
#define CY_PINS_LED_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define LED_0			(LED__0__PC)
#define LED_0_PS		(LED__0__PS)
#define LED_0_PC		(LED__0__PC)
#define LED_0_DR		(LED__0__DR)
#define LED_0_SHIFT	(LED__0__SHIFT)
#define LED_0_INTR	((uint16)((uint16)0x0003u << (LED__0__SHIFT*2u)))

#define LED_1			(LED__1__PC)
#define LED_1_PS		(LED__1__PS)
#define LED_1_PC		(LED__1__PC)
#define LED_1_DR		(LED__1__DR)
#define LED_1_SHIFT	(LED__1__SHIFT)
#define LED_1_INTR	((uint16)((uint16)0x0003u << (LED__1__SHIFT*2u)))

#define LED_2			(LED__2__PC)
#define LED_2_PS		(LED__2__PS)
#define LED_2_PC		(LED__2__PC)
#define LED_2_DR		(LED__2__DR)
#define LED_2_SHIFT	(LED__2__SHIFT)
#define LED_2_INTR	((uint16)((uint16)0x0003u << (LED__2__SHIFT*2u)))

#define LED_INTR_ALL	 ((uint16)(LED_0_INTR| LED_1_INTR| LED_2_INTR))


#endif /* End Pins LED_ALIASES_H */


/* [] END OF FILE */
