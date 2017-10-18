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
	HAL_ADC_Stop_IT(hadc);
	char debugString[50] 		= "\n\rHAL_ADC_ConvCpltCallback";
	HAL_UART_Transmit(&huart1, (uint8_t*)debugString, strlen(debugString), 500);
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
	HAL_ADC_Stop_IT(hadc);
	char debugString[50] 		= "\n\rHAL_ADC_ErrorCallback";
	HAL_UART_Transmit(&huart1, (uint8_t*)debugString, strlen(debugString), 500);
}
	
void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc)
{
	HAL_ADC_Stop_IT(hadc);
	char debugString[50] 		= "\n\rHAL_ADC_LevelOutOfWindowCallback";
	HAL_UART_Transmit(&huart1, (uint8_t*)debugString, strlen(debugString), 500);
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
	HAL_ADC_Stop_IT(hadc);
	char debugString[50] 		= "\n\rHAL_ADC_ConvHalfCpltCallback";
	HAL_UART_Transmit(&huart1, (uint8_t*)debugString, strlen(debugString), 500);
}




void mg_adc_GetReading(void)
{
	HAL_Delay(1);
}

// close the Doxygen group
/**
\}
*/
  
/* end of file */
