/** ***************************************************************************
*   \file        mg_state_machine.c
*   \brief       State machine module
*
*   \copyright   Copyright (C) : <company name> <creation date YYYY-MM-DD>
*
*   \addtogroup  AddGroupsAsRequiredForTheProject
*   \{
******************************************************************************/
 
/*****************************************************************************/
// standard libraries
#include "string.h"
#include <stdbool.h>
 
// user headers directly related to this component, ensures no dependency
#include "mg_state_machine.h"
#include "mg_adc.h"

// user headers from other components
  
/*****************************************************************************/
// enumerations
  
/*****************************************************************************/
// typedefs
typedef enum stateTop {
	INIT,									// Initialise
	AWAKE,								// Awake - checking system status and acting accordingly
	ASLEEP								// Sleeping
} State_t;
  
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
extern bool flagStartConv;
  
/*****************************************************************************/
// functions
void mg_state_machine(void)
{
	static State_t stateTop = INIT;
	static bool firstPass = true;
	
	switch(stateTop)
	{
		case INIT:
		{
			char myString[] = "Light Sensor Node";
			HAL_UART_Transmit(&huart1, (uint8_t*)myString, strlen(myString), 500);

			stateTop = AWAKE;																																// Jump into main program
			break;
		}
		
		case AWAKE:
		{
			if(firstPass)																																// If this is the first pass
			{
				flagStartConv = true;																													// Flag a new conversion to start
				firstPass = false;																														// Reset flag
			}
			
			mg_adc_StateMachine();																													// Kick ADC state machine
			
			break;
		}
		
		case ASLEEP:
		{
			break;
		}
		
		default:
		{
			break;
		}
		
	}
}


// close the Doxygen group
/**
\}
*/
  
/* end of file */
