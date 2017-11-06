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
	ADC_STATE_ERROR,								// ADC error state
	
	ADC_STATE_NUM_STATES						// Number of states
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
uint8_t avgIndex;													// Counts the number of readings taken for averaging							
StateADC_t stateADC = ADC_STATE_IDLE;			// ADC state machine state variable
AdcExtFlags_t adcExtFlags;

/* Light sensor range */
typedef enum
{
	LIGHT_RANGE_HIGH,
	LIGHT_RANGE_LOW
} LightRange_t;

/* Flags for use within ADC state machine */
typedef union {
    struct
    {
        uint8_t flagConvComplete 	: 1;			// Flag to indicate conversion is complete
    };
    uint8_t adcIntFlags;
} AdcIntFlags_t;
AdcIntFlags_t adcIntFlags;

/*****************************************************************************/
// variable declarations
extern ADC_HandleTypeDef hadc;
extern UART_HandleTypeDef huart1;
  
/*****************************************************************************/
// functions
/* Sets the light range on the ambient light sensor amplifier */
void mg_adc_SetLightRange(LightRange_t range)
{
	switch (range)
	{
		case LIGHT_RANGE_HIGH:
		{
			// Set the range high
			HAL_GPIO_WritePin(RANGE_GPIO_Port, RANGE_Pin, GPIO_PIN_SET);
			break;
		}
		
		case LIGHT_RANGE_LOW:
		{
			// Set the range low
			HAL_GPIO_WritePin(RANGE_GPIO_Port, RANGE_Pin, GPIO_PIN_RESET);
			break;
		}
		
		default:
		{
			#ifdef DEBUG_ADC
			char AdcReadingString[50];
			sprintf(AdcReadingString, "Error: range not valid");
			HAL_UART_Transmit(&huart1, (uint8_t*)AdcReadingString, strlen(AdcReadingString), 500);
			#endif
			while(1);
		}
	}
}

/* ADC system state machine */
void mg_adc_StateMachine(void)
{
	LightRange_t LightRange = LIGHT_RANGE_LOW;															// Set range low
	mg_adc_SetLightRange(LightRange);
	HAL_GPIO_WritePin(SENSE_EN_GPIO_Port, SENSE_EN_Pin, GPIO_PIN_SET);			// Turn on power to ambient light sensor
	ADC1->CHSELR = ADC_CHSELR_CHSEL10;																			// Change ADC channel to light sensor pin
	
	switch(stateADC)
	{
		case ADC_STATE_IDLE:
		{
			if(adcExtFlags.flagStartConv)										// If a new conversion has been requested
			{
				adcExtFlags.flagConvDone 	= false;							// Then clear data ready flag (if not already clear)
				adcExtFlags.flagStartConv = false;							// Clear start conversion flag
				HAL_ADC_Start_IT(&hadc);												// Then start the ADC
				avgIndex = 0;																		// Reset the average index
				stateADC = ADC_STATE_CONVERTING;								// And go to converting state
			}
			break;
		}
		
		case ADC_STATE_CONVERTING:
		{
			if(adcIntFlags.flagConvComplete)								// If the conversion is complete
			{
				HAL_ADC_Stop_IT(&hadc);													// Stop the ADC
				adcIntFlags.flagConvComplete = false;						// Reset flag
				stateADC = ADC_STATE_CONVERTED;									// And go to converted state
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
				adcExtFlags.flagConvDone = true;										// Flag that data is ready
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
	adcIntFlags.flagConvComplete = true;
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
