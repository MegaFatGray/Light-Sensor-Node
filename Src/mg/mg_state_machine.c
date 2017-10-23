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
#define SLEEP_PERIOD_MS 1000

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

			stateTop = AWAKE;																														// Jump into main program
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
			
			if(flagConvDone)																														// If the data is ready
			{
				flagConvDone = false;																													// Clear the flag
				firstPass = true;																															// Set flag
				stateTop = ASLEEP;																														// And go to sleep
			}
			
			break;
		}
		
		case ASLEEP:
		{
			static uint32_t lastTick;
			if(firstPass)
			{
				lastTick = HAL_GetTick();
				firstPass = false;
			}

			if( (HAL_GetTick() - lastTick) > SLEEP_PERIOD_MS )													// If the sleep period has elapsed
			{
				firstPass = true;																															// Set flag
				stateTop = AWAKE;																															// And wake up
			}
			
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
