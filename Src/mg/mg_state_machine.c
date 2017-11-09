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
	TOP_STATE_INIT,								// Initialise
	TOP_STATE_AWAKE,							// Awake - checking system status and acting accordingly
	TOP_STATE_ASLEEP,							// Sleeping
	TOP_STATE_ERROR,							// Error state
	
	TOP_STATE_NUM_STATES 					// The number of states
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
	static bool 	firstPass = true;
	static 				State_t stateTop = TOP_STATE_INIT;
	
	static 				AdcControlFlags_t adcControlFlags = 								
								{
									.getLight = 1,
									.getTemp	= 0,
									.getBat		= 0
								};
	
	switch(stateTop)
	{
		case TOP_STATE_INIT:
		{
			char myString[] = "Light Sensor Node";
			HAL_UART_Transmit(&huart1, (uint8_t*)myString, strlen(myString), 500);
			stateTop = TOP_STATE_AWAKE;																									// Jump into main program
			break;
		}
		
		case TOP_STATE_AWAKE:
		{
			if(firstPass)																																// If this is the first pass
			{
				adcControlFlags.getLight = true;																							// Flag a new conversion to start
				firstPass = false;																														// Reset flag
			}
			
			mg_adc_StateMachine(adcControlFlags);																				// Kick ADC state machine
			
			if(adcStatusFlags.flagComplete)									 														// If the data is ready
			{
				adcStatusFlags.flagComplete = false;																					// Clear the flag
				firstPass = true;																															// Set flag
				stateTop = TOP_STATE_ASLEEP;																									// And go to sleep
			}
			
			break;
		}
		
		case TOP_STATE_ASLEEP:
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
				stateTop = TOP_STATE_AWAKE;																										// And wake up
			}
			
			break;
		}
		
		case TOP_STATE_ERROR:
		{
			// Intentional fall-through
		}
		
		case TOP_STATE_NUM_STATES:
		{
			// Intentional fall-through
		}
		
		default:
		{
			char myString[] = "Error: Top state machine";
			HAL_UART_Transmit(&huart1, (uint8_t*)myString, strlen(myString), 500);
			while(1);
		}
		
	}
}


// close the Doxygen group
/**
\}
*/
  
/* end of file */
