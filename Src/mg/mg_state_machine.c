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
#include "global_defs.h"
#include "mg_state_machine.h"
#include "mg_adc.h"
#include "mg_stopmode.h"


// user headers from other components
  
/*****************************************************************************/
// enumerations
  
/*****************************************************************************/
// typedefs
typedef enum stateTop {
	TOP_STATE_AWAKE,							// Awake - checking system status and acting accordingly
	TOP_STATE_ASLEEP,							// Sleeping in stop mode
	TOP_STATE_ERROR,							// Error state
	
	TOP_STATE_NUM_STATES 					// The number of states
} StateTop_t;
  
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
bool 												firstPass 	= true;
StateTop_t 									stateTop 		= TOP_STATE_AWAKE;
AdcData_t adcData =
	{
		.readingLight		= 0,
		.readingTemp		= 0,
		.readingBat			= 0
	};
	
uint8_t rtcInterrupt;
  
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
		case TOP_STATE_AWAKE:
		{
			if(firstPass)																																// If this is the first pass
			{
				adcControlFlags.getLight 	= true;																							// Set the light conversion request flag
				adcControlFlags.getTemp 	= true;																							// Clear the temperature conversion request flag
				adcControlFlags.getBat 		= true;																							// Clear the battery conversion request flag
				adcControlFlags.start 		= true;																							// Set the start conversion flag
				adcControlFlags.reset 		= false;																						// Clear the reset flag
				firstPass = false;																														// Reset first pass flag
				
				#ifdef DEBUG_TOPSM
				char debugString[50];
				sprintf(debugString, "\n\rTOP_STATE_AWAKE");
				HAL_UART_Transmit(&huart1, (uint8_t*)debugString, strlen(debugString), 500);
				#endif
			}
			
			AdcStatusFlags_t adcStatusFlags = mg_adc_StateMachine(adcControlFlags, &adcData);		// Kick ADC state machine
			
			if(adcStatusFlags.flagComplete)									 														// If the data is ready
			{
				#ifdef DEBUG_TOPSM
				char debugString[50];
				sprintf(debugString, "\n\rLight = %d", adcData.readingLight);
				HAL_UART_Transmit(&huart1, (uint8_t*)debugString, strlen(debugString), 500);
				sprintf(debugString, "\n\rTemp = %d", adcData.readingTemp);
				HAL_UART_Transmit(&huart1, (uint8_t*)debugString, strlen(debugString), 500);
				sprintf(debugString, "\n\rBat = %d", adcData.readingBat);
				HAL_UART_Transmit(&huart1, (uint8_t*)debugString, strlen(debugString), 500);
				#endif
				
				adcControlFlags.getLight 	= false;																						// Clear the light conversion request flag
				adcControlFlags.getTemp 	= false;																						// Clear the temperature conversion request flag
				adcControlFlags.getBat 		= false;																						// Clear the battery conversion request flag
				adcControlFlags.start 		= false;																						// Clear the start conversion flag
				adcControlFlags.reset 		= true;																							// Set the reset flag
				
				adcStatusFlags = mg_adc_StateMachine(adcControlFlags, &adcData);							// Reset ADC state machine
				mg_state_machine_ChangeState(TOP_STATE_ASLEEP);																// And go to sleep state
			}
			
			break;
		}
		
		case TOP_STATE_ASLEEP:
		{
			if(firstPass)
			{
				firstPass = 0;
				
				#ifdef DEBUG_TOPSM
				char debugString[50];
				sprintf(debugString, "\n\rTOP_STATE_ASLEEP");
				HAL_UART_Transmit(&huart1, (uint8_t*)debugString, strlen(debugString), 500);
				#endif
			}
			
			mg_stopmode_Stop();																													// Go to sleep until next wake cycle
			
			/* SLEEPING */
			
			mg_state_machine_ChangeState(TOP_STATE_AWAKE);															// When woken up go to awake state
			
			
			break;
		}
		
		case TOP_STATE_ERROR:
		{
			if(firstPass)
			{
				firstPass = 0;
				
				#ifdef DEBUG_TOPSM
				char debugString[50];
				sprintf(debugString, "\n\rTOP_STATE_ERROR");
				HAL_UART_Transmit(&huart1, (uint8_t*)debugString, strlen(debugString), 500);
				#endif
			}
			// Intentional fall-through
		}
		
		case TOP_STATE_NUM_STATES:
		{
			if(firstPass)
			{
				firstPass = 0;
				
				#ifdef DEBUG_TOPSM
				char debugString[50];
				sprintf(debugString, "\n\rTOP_STATE_NUM_STATES");
				HAL_UART_Transmit(&huart1, (uint8_t*)debugString, strlen(debugString), 500);
				#endif
			}
			// Intentional fall-through
		}
		
		default:
		{
			if(firstPass)
			{
				firstPass = 0;
				
				#ifdef DEBUG_TOPSM
				char debugString[50];
				sprintf(debugString, "\n\rError: Top state machine");
				HAL_UART_Transmit(&huart1, (uint8_t*)debugString, strlen(debugString), 500);
				#endif
			}
			while(1);
		}
		
	}
}


// close the Doxygen group
/**
\}
*/
  
/* end of file */
