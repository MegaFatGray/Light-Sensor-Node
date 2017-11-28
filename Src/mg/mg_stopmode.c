/** ***************************************************************************
*   \file        mg_stopmode.c
*   \brief       Low power functions
*
*   \copyright   Copyright (C) : <company name> <creation date YYYY-MM-DD>
*
*   \addtogroup  AddGroupsAsRequiredForTheProject
*   \{
******************************************************************************/
 
/*****************************************************************************/
// standard libraries
 
// user headers directly related to this component, ensures no dependency
#include "mg_stopmode.h"
#include "global_defs.h"
#include "stm32l0xx_hal.h"
#include "string.h"
   
// user headers from other components
  
/*****************************************************************************/
// enumerations
  
/*****************************************************************************/
// typedefs
  
/*****************************************************************************/
// structures
  
/*****************************************************************************/
// constants

/*****************************************************************************/
// macros
  
/*****************************************************************************/
// static function declarations
  
/*****************************************************************************/
// static variable declarations			

/*****************************************************************************/
// variable declarations
extern UART_HandleTypeDef huart1;

/*****************************************************************************/
// functions
void mg_stopmode_Stop(void)
{
	#ifdef DEBUG_STOPMODE
	char debugString[50];
	sprintf(debugString, "\n\rStop mode");
	HAL_UART_Transmit(&huart1, (uint8_t*)debugString, strlen(debugString), 500);
	#endif
	
	/* Prepare to enter stop mode */
  PWR->CR |= PWR_CR_CWUF; // clear the WUF flag after 2 clock cycles
  PWR->CR &= ~( PWR_CR_PDDS ); // Enter stop mode when the CPU enters deepsleep
  RCC->CFGR &= ~( RCC_CFGR_STOPWUCK ); // MSI oscillator is wake-up from stop clock
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk; // low-power mode = stop mode
  __WFI(); // enter low-power mode
}


// close the Doxygen group
/**
\}
*/
  
/* end of file */
