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

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	char HereString[50] 		= "\n\rcallback - conversion complete";
	HAL_UART_Transmit(&huart1, (uint8_t*)HereString, strlen(HereString), 500);
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
	char HereString[50] 		= "\n\rcallback - error";
	HAL_UART_Transmit(&huart1, (uint8_t*)HereString, strlen(HereString), 500);
}
	
void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc)
{
	char HereString[50] 		= "\n\rcallback - level out of window";
	HAL_UART_Transmit(&huart1, (uint8_t*)HereString, strlen(HereString), 500);
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
	char HereString[50] 		= "\n\rcallback - half complete";
	HAL_UART_Transmit(&huart1, (uint8_t*)HereString, strlen(HereString), 500);
}


void mg_adc_GetReading(void)
{
	HAL_Delay(1);
	
	AdcStatus = HAL_ADC_Start(&hadc);
	//HAL_UART_Transmit(&huart1, (uint8_t)AdcStatus, sizeof(AdcStatus), 500);
	char AdcStatusString[20];
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
	
	/*
	while(1)
	{
	
	AdcStatus = HAL_ADC_PollForConversion(&hadc, PollForConversionTimeout);
	
	switch(AdcStatus)
	{
		char AdcStatusString[20];
			
		case HAL_OK:
		{
			strcpy(AdcStatusString, "\n\rHAL_ADC_PollForConversion HAL_OK");
		}
		case HAL_ERROR:
		{
			strcpy(AdcStatusString, "\n\rHAL_ADC_PollForConversion HAL_ERROR");
		}
		case HAL_BUSY:
		{
			strcpy(AdcStatusString, "\n\rHAL_ADC_PollForConversion HAL_BUSY");
		}
		case HAL_TIMEOUT:
		{
			strcpy(AdcStatusString, "\n\rHAL_ADC_PollForConversion HAL_TIMEOUT");
		}
		
		HAL_UART_Transmit(&huart1, (uint8_t*)AdcStatusString, strlen(AdcStatusString), 500);
		
	}
	
	}
	*/
	
}

// close the Doxygen group
/**
\}
*/
  
/* end of file */
