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
extern UART_HandleTypeDef 	huart1;
  
/*****************************************************************************/
// functions

void mg_main_Main(void)
{
	#ifdef DEBUG_TOPSM
	char debugString[50];
	sprintf(debugString, "\n\rLight Sensor Node");
	HAL_UART_Transmit(&huart1, (uint8_t*)debugString, strlen(debugString), 500);
	#endif
	
	
	
	while(1)
	{
		//HAL_GPIO_TogglePin(LED_GRN_GPIO_Port, LED_GRN_Pin);
		//RTC_Get(&rtcDate, &rtcTime);
		//HAL_Delay(1000);
		
	}
}


// close the Doxygen group
/**
\}
*/
  
/* end of file */
