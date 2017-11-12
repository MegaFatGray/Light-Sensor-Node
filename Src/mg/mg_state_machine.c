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
} StateTop_t;
  
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
extern UART_HandleTypeDef 	huart1;
bool 												firstPass 	= true;
StateTop_t 									stateTop 		= TOP_STATE_INIT;
  
/*****************************************************************************/
// functions

/* Function to change state */
void mg_state_machine_ChangeState(StateTop_t newState)
{
	firstPass	= true;
	stateTop  = newState;
}

/* Top level state machine */
void mg_state_machine(void)
{
	static 				AdcControlFlags_t adcControlFlags = 								
								{
									.start		= 0,
									.getLight = 0,
									.getTemp	= 0,
									.getBat		= 0,
									.reset		= 0
								};
	
	switch(stateTop)
	{
		case TOP_STATE_INIT:
		{
			char myString[] = "Light Sensor Node";
			HAL_UART_Transmit(&huart1, (uint8_t*)myString, strlen(myString), 500);
			mg_state_machine_ChangeState(TOP_STATE_AWAKE);															// Jump into main program
			break;
		}
		
		case TOP_STATE_AWAKE:
		{
			if(firstPass)																																// If this is the first pass
			{
				adcControlFlags.getLight 	= true;																							// Set the light conversion request flag
				adcControlFlags.getTemp 	= false;																						// Clear the temperature conversion request flag
				adcControlFlags.getBat 		= false;																						// Clear the battery conversion request flag
				adcControlFlags.start 		= true;																							// Set the start conversion flag
				adcControlFlags.reset 		= false;																						// Clear the reset flag
				firstPass = false;																														// Reset first pass flag
			}
			
			AdcStatusFlags_t adcStatusFlags = mg_adc_StateMachine(adcControlFlags);			// Kick ADC state machine
			
			if(adcStatusFlags.flagComplete)									 														// If the data is ready
			{
				adcControlFlags.getLight 	= false;																						// Clear the light conversion request flag
				adcControlFlags.getTemp 	= false;																						// Clear the temperature conversion request flag
				adcControlFlags.getBat 		= false;																						// Clear the battery conversion request flag
				adcControlFlags.start 		= false;																						// Clear the start conversion flag
				adcControlFlags.reset 		= true;																							// Set the reset flag
				
				adcStatusFlags = mg_adc_StateMachine(adcControlFlags);												// Reset ADC state machine
				
				mg_state_machine_ChangeState(TOP_STATE_ASLEEP);																// And go to sleep
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
				mg_state_machine_ChangeState(TOP_STATE_AWAKE);																// Wake up
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
