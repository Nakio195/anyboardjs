/*******************************************************************************
* File Name: Comp_0.c
* Version 2.20
*
* Description:
*  This file provides the source code to the API for the Low Power Comparator
*  component.
*
* Note:
*  None
*
********************************************************************************
* Copyright 2013-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "Comp_0.h"
#include "CyLib.h"
#include "cyPm.h"

uint8 Comp_0_initVar = 0u;
static uint32 Comp_0_internalIntr = 0u;
#if(Comp_0_CY_LPCOMP_V2)
    #if(CY_IP_SRSSV2)
        /* This variable saves INTR_MASK register since it get reset after wakeup 
        * from Hibernate. Attribute CY_NOINIT puts SRAM variable in memory section 
        * which is retained in low power modes 
        */
        CY_NOINIT static uint32 Comp_0_intrMaskRegState;
    #endif /* (CY_IP_SRSSV2) */
#endif /* (Comp_0_CY_LPCOMP_V2) */


/*******************************************************************************
* Function Name: Comp_0_Start
********************************************************************************
*
* Summary:
*  Performs all of the required initialization for the component and enables
*  power to the block. The first time the routine is executed, the component is
*  initialized to the configuration from the customizer. When called to restart
*  the comparator following a Comp_0_Stop() call, the current
*  component parameter settings are retained.
*
* Parameters:
*   None
*
* Return:
*  None
*
* Global variables:
*  Comp_0_initVar: Is modified when this function is called for the
*   first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void Comp_0_Start(void)
{
    if(0u == Comp_0_initVar)
    {
        Comp_0_Init();
        Comp_0_initVar = 1u;
    }
    Comp_0_Enable();
}


/*******************************************************************************
* Function Name: Comp_0_Init
********************************************************************************
*
* Summary:
*  Initializes or restores the component according to the customizer settings.
*  It is not necessary to call Comp_0_Init() because the
*  Comp_0_Start() API calls this function and is the preferred method
*  to begin component operation.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Comp_0_Init(void)
{
    uint8 enableInterrupts;

    enableInterrupts = CyEnterCriticalSection();
    Comp_0_CONFIG_REG |= Comp_0_CONFIG_REG_DEFAULT;
    Comp_0_internalIntr = Comp_0_GET_CONFIG_INTR(Comp_0_INTERRUPT);
    
    #if (Comp_0_CY_LPCOMP_V2)
        #if(CY_IP_SRSSV2)
            if (CySysPmGetResetReason() == CY_PM_RESET_REASON_WAKEUP_HIB)
            {
                /* Restore the INTR_MASK register state since it get reset after 
                * wakeup from Hibernate.  
                */
                Comp_0_INTR_MASK_REG |= Comp_0_intrMaskRegState;  
            }
            else
            {
                Comp_0_INTR_MASK_REG |= Comp_0_INTR_MASK_REG_DEFAULT;    
                Comp_0_intrMaskRegState = Comp_0_INTR_MASK_REG;
            }
        #else
            Comp_0_INTR_MASK_REG |= Comp_0_INTR_MASK_REG_DEFAULT;
        #endif /* (Comp_0_CY_LPCOMP_V2) */
    #endif /* (Comp_0_CY_LPCOMP_V2) */
    
    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: Comp_0_Enable
********************************************************************************
*
* Summary:
*  Activates the hardware and begins component operation. It is not necessary
*  to call Comp_0_Enable() because the Comp_0_Start() API
*  calls this function, which is the preferred method to begin component
*  operation.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Comp_0_Enable(void)
{
    uint8 enableInterrupts;
    
    enableInterrupts = CyEnterCriticalSection();
    Comp_0_CONFIG_REG |= Comp_0_CONFIG_EN;
    
    /*******************************************************
    * When the Enable() API is called the CONFIG_EN bit is set. 
    * This can cause fake interrupt because of the output delay 
    * of the analog. This requires setting the CONFIG_INTR bits 
    * after the CONFIG_EN bit is set.
    *******************************************************/
    Comp_0_CONFIG_REG |= Comp_0_internalIntr;
    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: Comp_0_Stop
********************************************************************************
*
* Summary:
*  Turns off the LP Comparator.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  The function doesn't change component Speed settings.
*
*******************************************************************************/
void Comp_0_Stop(void)
{
    uint8 enableInterrupts;
    
    enableInterrupts = CyEnterCriticalSection();
    #if (Comp_0_CY_LPCOMP_V0)
        /*******************************************************
        * When the Stop() API is called the CONFIG_EN bit is 
        * cleared. This causes the output of the comparator to go 
        * low. If the comparator output was high and the Interrupt 
        * Configuration is set for Falling edge disabling the 
        * comparator will cause an fake interrupt. This requires 
        * to clear the CONFIG_INTR bits before the CONFIG_EN bit 
        * is cleared.
        *******************************************************/
        Comp_0_CONFIG_REG &= (uint32)~Comp_0_CONFIG_INTR_MASK;
    #endif /* (Comp_0_CY_LPCOMP_V0) */

    Comp_0_CONFIG_REG &= (uint32)~Comp_0_CONFIG_EN;
    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: Comp_0_SetSpeed
*
* Summary:
*  Sets the drive power and speed to one of three settings.
*
* Parameters:
*  uint32 speed: Sets operation mode of the component:
*   Comp_0_LOW_SPEED  - Slow/ultra low 
*   Comp_0_MED_SPEED  - Medium/low 
*   Comp_0_HIGH_SPEED - Fast/normal 
*
* Return:
*  None
*
*******************************************************************************/
void Comp_0_SetSpeed(uint32 speed)
{
    uint32 config;
    uint8 enableInterrupts;
    
    enableInterrupts = CyEnterCriticalSection();
    config = Comp_0_CONFIG_REG & (uint32)~Comp_0_CONFIG_SPEED_MODE_MASK;
    Comp_0_CONFIG_REG = config | Comp_0_GET_CONFIG_SPEED_MODE(speed);
    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: Comp_0_SetInterruptMode
********************************************************************************
*
* Summary:
*  Sets the interrupt edge detect mode. This also controls the value provided
*  on the output.
*
* Parameters:
*  uint32 mode: Enumerated edge detect mode value:
*  Comp_0_INTR_DISABLE - Disable
*  Comp_0_INTR_RISING  - Rising edge detect
*  Comp_0_INTR_FALLING - Falling edge detect
*  Comp_0_INTR_BOTH    - Detect both edges
*
* Return:
*  None
*
*******************************************************************************/
void Comp_0_SetInterruptMode(uint32 mode)
{
    uint32 config;
    uint8 enableInterrupts;
    
    Comp_0_internalIntr = Comp_0_GET_CONFIG_INTR(mode);

    enableInterrupts = CyEnterCriticalSection();
    config = Comp_0_CONFIG_REG & (uint32)~Comp_0_CONFIG_INTR_MASK;
    Comp_0_CONFIG_REG = config | Comp_0_internalIntr;
    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: Comp_0_GetInterruptSource
********************************************************************************
*
* Summary:
*  Gets the interrupt requests. This function is for use when using the combined
*  interrupt signal from the global signal reference. This function from either
*  component instance can be used to determine the interrupt source for both the
*  interrupts combined.
*
* Parameters:
*  None
*
* Return:
*  uint32: Interrupt source. Each component instance has a mask value:
*  Comp_0_INTR.
*
*******************************************************************************/
uint32 Comp_0_GetInterruptSource(void)
{
    return (Comp_0_INTR_REG); 
}


/*******************************************************************************
* Function Name: Comp_0_ClearInterrupt
********************************************************************************
*
* Summary:
*  Clears the interrupt request. This function is for use when using the
*  combined interrupt signal from the global signal reference. This function
*  from either component instance can be used to clear either or both
*  interrupts.
*
* Parameters:
*  uint32 interruptMask: Mask of interrupts to clear. Each component instance
*  has a mask value: Comp_0_INTR.
*
* Return:
*  None
*
*******************************************************************************/
void Comp_0_ClearInterrupt(uint32 interruptMask)
{   
    Comp_0_INTR_REG = interruptMask;
}


/*******************************************************************************
* Function Name: Comp_0_SetInterrupt
********************************************************************************
*
* Summary:
*  Sets a software interrupt request. This function is for use when using the
*  combined interrupt signal from the global signal reference. This function
*  from either component instance can be used to trigger either or both software
*  interrupts.
*
* Parameters:
*  uint32 interruptMask: Mask of interrupts to set. Each component instance has
*  a mask value: Comp_0_INTR_SET.
*
* Return:
*  None
*
*******************************************************************************/
void Comp_0_SetInterrupt(uint32 interruptMask)
{
    uint8 enableInterrupts;
    
    enableInterrupts = CyEnterCriticalSection();
    Comp_0_INTR_SET_REG |= interruptMask;
    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: Comp_0_SetHysteresis
********************************************************************************
*
* Summary:
*  Enables or disables the hysteresis.
*
* Parameters:
*  hysteresis: (uint32) Enables or disables the hysteresis setting.
*  Comp_0_HYST_ENABLE     - Enable hysteresis
*  Comp_0_HYST_DISABLE    - Disable hysteresis
*
* Return:
*  None
*
*******************************************************************************/
void Comp_0_SetHysteresis(uint32 hysteresis)
{
    uint8 enableInterrupts;
    
    enableInterrupts = CyEnterCriticalSection();
    if(0u != hysteresis)
    {
        Comp_0_CONFIG_REG |= Comp_0_CONFIG_HYST;
    }
    else
    {
        Comp_0_CONFIG_REG &= (uint32)~Comp_0_CONFIG_HYST;
    }
    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: Comp_0_SetFilter
********************************************************************************
*
* Summary:
*  Enables or disables the digital filter. This still exists for saving backward 
*  compatibility, but not recommended for using.
*  This API is DEPRECATED and should not be used in new projects.
*
* Parameters:
*  uint32 filter: filter enable.
*  Comp_0_FILT_ENABLE  - Enable filter
*  Comp_0_FILT_DISABLE - Disable filter
*
* Return:
*  None
*
*******************************************************************************/
void Comp_0_SetFilter(uint32 filter)
{
    uint8 enableInterrupts;
    
    enableInterrupts = CyEnterCriticalSection();
    if( 0u != filter)
    {
        Comp_0_CONFIG_REG |= Comp_0_CONFIG_FILT;
    }
    else
    {
        Comp_0_CONFIG_REG &= (uint32)~Comp_0_CONFIG_FILT;
    }
    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: Comp_0_GetCompare
********************************************************************************
*
* Summary:
*  This function returns a nonzero value when the voltage connected to the
*  positive input is greater than the negative input voltage.
*  This function reads the direct (unflopped) comparator output which can also be 
*  metastable (since it may result in incorrect data).
*
* Parameters:
*  None
*
* Return:
*  (uint32) Comparator output state. This value is not impacted by the interrupt
*  edge detect setting:
*   0 - if positive input is less than negative one.
*   1 - if positive input greater than negative one.
*
*******************************************************************************/
uint32 Comp_0_GetCompare(void)
{
    return((uint32)((0u == (Comp_0_CONFIG_REG & Comp_0_CONFIG_OUT)) ? 0u : 1u));
}


/*******************************************************************************
* Function Name: Comp_0_ZeroCal
********************************************************************************
*
* Summary:
*  Performs custom calibration of the input offset to minimize error for a
*  specific set of conditions: comparator reference voltage, supply voltage,
*  and operating temperature. A reference voltage in the range at which the
*  comparator will be used must be applied to one of the inputs. The two inputs
*  will be shorted internally to perform the offset calibration.
*
* Parameters:
*  None
*
* Return:
*  The value from the comparator trim register after the offset calibration is
*  complete. This value has the same format as the input parameter for the
*  Comp_0_LoadTrim() API routine.
*
*******************************************************************************/
uint32 Comp_0_ZeroCal(void)
{
    uint32 cmpOut;
    uint32 i;

    Comp_0_DFT_REG |= Comp_0_DFT_CAL_EN;
    Comp_0_TRIMA_REG = 0u;
    Comp_0_TRIMB_REG = 0u;

    CyDelayUs(1u);

    cmpOut = Comp_0_GetCompare();

    if(0u != cmpOut)
    {
        Comp_0_TRIMA_REG = ((uint32) 0x01u << Comp_0_TRIMA_SIGNBIT);
    }

    for(i = (Comp_0_TRIMA_MAX_VALUE +1u ); i != 0u; i--)
    {
        Comp_0_TRIMA_REG++;
        CyDelayUs(1u);
        if(cmpOut != Comp_0_GetCompare())
        {
            break;
        }
    }

    if(((uint32)(Comp_0_CONFIG_REG >> Comp_0_CONFIG_REG_SHIFT) &
        Comp_0_SPEED_PARAM_MASK) == Comp_0_MED_SPEED)
    {
        cmpOut = Comp_0_GetCompare();

        if(0u == cmpOut)
        {
            Comp_0_TRIMB_REG = ((uint32)1u << Comp_0_TRIMB_SIGNBIT);
        }

        for(i = (Comp_0_TRIMB_MAX_VALUE +1u ); i != 0u; i--)
        {
            Comp_0_TRIMB_REG++;
            CyDelayUs(1u);
            if(cmpOut != Comp_0_GetCompare())
            {
                break;
            }
        }
    }

    Comp_0_DFT_REG &= ((uint32)~Comp_0_DFT_CAL_EN);

    return (Comp_0_TRIMA_REG | ((uint32)Comp_0_TRIMB_REG << Comp_0_TRIMB_SHIFT));
}


/*******************************************************************************
* Function Name: Comp_0_LoadTrim
********************************************************************************
*
* Summary:
*  This function writes a value into the comparator offset trim register.
*
* Parameters:
*  trimVal: Value to be stored in the comparator offset trim register. This
*  value has the same format as the parameter returned by the
*  Comp_0_ZeroCal() API routine.
*
* Return:
*  None
*
*******************************************************************************/
void Comp_0_LoadTrim(uint32 trimVal)
{
    Comp_0_TRIMA_REG = (trimVal & Comp_0_TRIMA_MASK);
    Comp_0_TRIMB_REG = ((trimVal >> Comp_0_TRIMB_SHIFT) & Comp_0_TRIMB_MASK);
}


#if (Comp_0_CY_LPCOMP_V2)
    
/*******************************************************************************
* Function Name: Comp_0_SetOutputMode
********************************************************************************
*
* Summary:
*  Set comparator output mode. 
*
* Parameters:
*  uint32 mode: Comparator output mode value
*  Comp_0_OUT_DIRECT - Direct output
*  Comp_0_OUT_SYNC   - Synchronized output
*  Comp_0_OUT_PULSE  - Pulse output
*
* Return:
*  None
*
*******************************************************************************/
void Comp_0_SetOutputMode(uint32 mode)
{
    uint32 config;
    uint8 enableInterrupts;
    
    enableInterrupts = CyEnterCriticalSection();
    config = Comp_0_CONFIG_REG & ((uint32)~(Comp_0_CONFIG_DSI_BYPASS | \
                                                     Comp_0_CONFIG_DSI_LEVEL));
    Comp_0_CONFIG_REG = config | Comp_0_GET_CONFIG_DSI_BYPASS(mode) | \
                                           Comp_0_GET_CONFIG_DSI_LEVEL(mode);
    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: Comp_0_SetInterruptMask
********************************************************************************
*
* Summary:
*  Configures which bits of interrupt request register will trigger an interrupt 
*  event.
*
* Parameters:
*  uint32 interruptMask: Mask of interrupts to set. Each component instance has
*  a mask value: Comp_0_INTR_MASK.
*
* Return:
*  None
*
*******************************************************************************/
void Comp_0_SetInterruptMask(uint32 interruptMask)
{
    #if(CY_IP_SRSSV2)
        Comp_0_intrMaskRegState = interruptMask;
    #endif /* (CY_IP_SRSSV2) */
    
    Comp_0_INTR_MASK_REG = interruptMask;
}


/*******************************************************************************
* Function Name: Comp_0_GetInterruptMask
********************************************************************************
*
* Summary:
*  Returns interrupt mask.
*
* Parameters:
*  None
*
* Return:
*  uint32:  Mask of enabled interrupt source. Each component instance 
*  has a mask value: Comp_0_INTR_MASK.
*
*******************************************************************************/
uint32 Comp_0_GetInterruptMask(void)
{
    return (Comp_0_INTR_MASK_REG);
}


/*******************************************************************************
* Function Name: Comp_0_GetInterruptSourceMasked
********************************************************************************
*
* Summary:
*  Returns interrupt request register masked by interrupt mask. Returns the result 
*  of bitwise AND operation between corresponding interrupt request and mask bits.
*
* Parameters:
*  None
*
* Return:
*  uint32: Status of enabled interrupt source. Each component instance 
*  has a mask value: Comp_0_INTR_MASKED.
*
*******************************************************************************/
uint32 Comp_0_GetInterruptSourceMasked(void)
{
    return (Comp_0_INTR_MASKED_REG);
}

#endif /* (Comp_0_CY_LPCOMP_V2) */


/* [] END OF FILE */
