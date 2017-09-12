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
uint32_t PollForConversionTimeout = 1000;

/*****************************************************************************/
// variable declarations
extern ADC_HandleTypeDef hadc;
extern UART_HandleTypeDef huart1;
  
/*****************************************************************************/
// functions
void mg_adc_GetReading(void)
{
	AdcStatus = HAL_ADC_Start(&hadc);
	
	char AdcStatusString[50];
	switch(AdcStatus)
	{
		case HAL_OK:
		{
			strcpy(AdcStatusString, "\n\rHAL_ADC_Start HAL_OK");
			break;
		}
		case HAL_ERROR:
		{
			strcpy(AdcStatusString, "\n\rHAL_ADC_Start HAL_ERROR");
			break;
		}
		case HAL_BUSY:
		{
			strcpy(AdcStatusString, "\n\rHAL_ADC_Start HAL_BUSY");
			break;
		}
		case HAL_TIMEOUT:
		{
			strcpy(AdcStatusString, "\n\rHAL_ADC_Start HAL_TIMEOUT");
			break;
		}
		default:
		{
			strcpy(AdcStatusString, "\n\rHAL_ADC_Start defaulted");
			break;
		}
	}
	HAL_UART_Transmit(&huart1, (uint8_t*)AdcStatusString, strlen(AdcStatusString), 500);
	
	AdcStatus = HAL_ADC_PollForConversion(&hadc, PollForConversionTimeout);
	
	switch(AdcStatus)
	{
		case HAL_OK:
		{
			strcpy(AdcStatusString, "\n\rHAL_ADC_PollForConversion HAL_OK");
			break;
		}
		case HAL_ERROR:
		{
			strcpy(AdcStatusString, "\n\rHAL_ADC_PollForConversion HAL_ERROR");
			break;
		}
		case HAL_BUSY:
		{
			strcpy(AdcStatusString, "\n\rHAL_ADC_PollForConversion HAL_BUSY");
			break;
		}
		case HAL_TIMEOUT:
		{
			strcpy(AdcStatusString, "\n\rHAL_ADC_PollForConversion HAL_TIMEOUT");
			break;
		}
		default:
		{
			strcpy(AdcStatusString, "\n\rHAL_ADC_PollForConversion defaulted");
			break;
		}
	}
	HAL_UART_Transmit(&huart1, (uint8_t*)AdcStatusString, strlen(AdcStatusString), 500);
	
	uint32_t AdcReading = HAL_ADC_GetValue(&hadc);
	char AdcReadingString[50];
	sprintf(AdcReadingString, "\n\rADC Reading = %d", AdcReading);
	HAL_UART_Transmit(&huart1, (uint8_t*)AdcReadingString, strlen(AdcReadingString), 500);
}

// close the Doxygen group
/**
\}
*/
  
/* end of file */
