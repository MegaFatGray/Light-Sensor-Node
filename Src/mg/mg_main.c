/** ***************************************************************************
*   \file        mg_main.c
*   \brief       Top level program
*
*   \copyright   Copyright (C) : <company name> <creation date YYYY-MM-DD>
*
*   \addtogroup  AddGroupsAsRequiredForTheProject
*   \{
******************************************************************************/
 
/*****************************************************************************/
// standard libraries
 
// user headers directly related to this component, ensures no dependency
#include "mg_main.h"
#include "stm32l0xx_hal.h"
#include "string.h"
#include "mg_adc.h"
   
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

void mg_main_Main(void)
{
	char mystring1[] = "TEST PROGRAM";
	HAL_UART_Transmit(&huart1, (uint8_t*)mystring1, strlen(mystring1), 500);
	
	mg_adc_StartReading();
	
	while(1)
	{
		HAL_GPIO_TogglePin(LED_GRN_GPIO_Port, LED_GRN_Pin);
		HAL_Delay(1000);
	}
}


// close the Doxygen group
/**
\}
*/
  
/* end of file */
