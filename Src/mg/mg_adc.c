/** ***************************************************************************
*   \file        mg_adc.c
*   \brief       ADC functions
*
*   \copyright   Copyright (C) : <company name> <creation date YYYY-MM-DD>
*
*   \addtogroup  AddGroupsAsRequiredForTheProject
*   \{
******************************************************************************/
 
/*****************************************************************************/
// standard libraries
#include <stdbool.h>
 
// user headers directly related to this component, ensures no dependency
#include "mg_adc.h"
#include "stm32l0xx_hal.h"
#include "string.h"
   
// user headers from other components
  
/*****************************************************************************/
// enumerations
  
/*****************************************************************************/
// typedefs
HAL_StatusTypeDef AdcStatus;

typedef enum stateADC {
	ADC_STATE_IDLE,									// Not doing anything
	ADC_STATE_CONVERTING,						// Conversion in progress
	ADC_STATE_CONVERTED,						// Conversion ready
	ADC_STATE_ERROR									// ADC error state
} StateADC_t;
  
/*****************************************************************************/
// structures
  
/*****************************************************************************/
// constants
#define CONVERSION_AVG_WIDTH	3

/*****************************************************************************/
// macros
  
/*****************************************************************************/
// static function declarations
  
/*****************************************************************************/
// static variable declarations
StateADC_t stateADC = ADC_STATE_IDLE;			// ADC state machine state variable
bool flagStartConv 		= false;						// Flag to indicate if a new conversion should start
bool flagConvComplete = false;						// Flag to indicate conversion is complete
bool flagConvDone 		= false;						// Flag to indicate conversion is recorded
uint8_t avgIndex;													// Counts the number of readings taken for averaging							

/*****************************************************************************/
// variable declarations
extern ADC_HandleTypeDef hadc;
extern UART_HandleTypeDef huart1;
  
/*****************************************************************************/
// functions
/* ADC system state machine */
void mg_adc_StateMachine(void)
{
	switch(stateADC)
	{
		case ADC_STATE_IDLE:
		{
			if(flagStartConv)										// If a new conversion has been requested
			{
				flagConvDone 	= false;							// Then clear data ready flag (if not already clear)
				flagStartConv = false;							// Clear start conversion flag
				HAL_ADC_Start_IT(&hadc);						// Then start the ADC
				avgIndex = 0;												// Reset the average index
				stateADC = ADC_STATE_CONVERTING;		// And go to converting state
			}
			break;
		}
		
		case ADC_STATE_CONVERTING:
		{
			if(flagConvComplete)								// If the conversion is complete
			{
				HAL_ADC_Stop_IT(&hadc);							// Stop the ADC
				flagConvComplete = false;						// Reset flag
				stateADC = ADC_STATE_CONVERTED;			// And go to converted state
			}
			break;
		}
		
		case ADC_STATE_CONVERTED:
		{
			static uint32_t readingArray[CONVERSION_AVG_WIDTH];		// Array to store readings for averaging
			
			uint32_t reading = HAL_ADC_GetValue(&hadc);						// Read the conversion result
			
			char debugString[50];
			sprintf(debugString, "\n\rReading = %d", reading);
			HAL_UART_Transmit(&huart1, (uint8_t*)debugString, strlen(debugString), 500);
			
			readingArray[avgIndex] = reading;											// Store the reading for averaging later
			avgIndex++;
			
			if(avgIndex >= (CONVERSION_AVG_WIDTH))						// If all the readings have been taken
			{
				uint32_t total = 0;
				for(uint8_t i=0; i<CONVERSION_AVG_WIDTH; i++)				// Sum the readings
				{
					total += readingArray[i];
				}
				reading = total / CONVERSION_AVG_WIDTH;							// Average the readings
				flagConvDone = true;																// Flag that data is ready
				stateADC = ADC_STATE_IDLE;													// And go back to idle state
				
				char debugString[50];
				sprintf(debugString, "\n\rAveraged reading = %d", reading);
				HAL_UART_Transmit(&huart1, (uint8_t*)debugString, strlen(debugString), 500);
			}
			else																							// Else if there are more readings to take
			{
				HAL_ADC_Start_IT(&hadc);														// Then start the ADC again
				stateADC = ADC_STATE_CONVERTING;										// And go back to converting state
			}
			
			
			break;
		}
		
		case ADC_STATE_ERROR:
		{
			// intentional fall through
		}
		
		default:
		{
			char debugString[50] 		= "\n\rADC state machine error";
			HAL_UART_Transmit(&huart1, (uint8_t*)debugString, strlen(debugString), 500);
			while(1);
		}
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	flagConvComplete = true;
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
	HAL_ADC_Stop_IT(hadc);
	stateADC = ADC_STATE_ERROR;
}
	
void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc)
{
	HAL_ADC_Stop_IT(hadc);
	stateADC = ADC_STATE_ERROR;
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
	HAL_ADC_Stop_IT(hadc);
	stateADC = ADC_STATE_ERROR;
}


// close the Doxygen group
/**
\}
*/
  
/* end of file */
