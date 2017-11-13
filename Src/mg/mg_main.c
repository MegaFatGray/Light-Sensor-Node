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
#include "global_defs.h"
#include "mg_main.h"
#include "mg_state_machine.h"
#include "mg_rtc.h"
   
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
  
/*****************************************************************************/
// functions

void mg_main_Main(void)
{
	RTC_DateTypeDef rtcDate;
	RTC_TimeTypeDef rtcTime;
	RTC_Set();
	
	while(1)
	{
		//mg_state_machine();
		HAL_GPIO_TogglePin(LED_GRN_GPIO_Port, LED_GRN_Pin);
		RTC_Get(&rtcDate, &rtcTime);
		HAL_Delay(1000);
		
	}
}


// close the Doxygen group
/**
\}
*/
  
/* end of file */
