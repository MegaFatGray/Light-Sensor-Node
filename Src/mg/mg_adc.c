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
#define VREFINT_CAL_ADDR    0x1FF80078
  
/*****************************************************************************/
// static function declarations
  
/*****************************************************************************/
// static variable declarations
uint32_t PollForConversionTimeout = 1;

/*****************************************************************************/
// variable declarations
extern ADC_HandleTypeDef hadc;
extern UART_HandleTypeDef huart1;
  
/*****************************************************************************/
// functions
uint32_t mg_adc_GetRawReading(void)
{
	// Start ADC reading
	if(HAL_ADC_Start(&hadc) != HAL_OK)
	{
		char ErrorString[50];
		strcpy(ErrorString, "\n\rHAL_ADC_Start - Failed");
		while(1);
	}
	
	// Wait for conversion to complete
	while(HAL_ADC_PollForConversion(&hadc, PollForConversionTimeout) != HAL_OK)
	{
		char HereString[20] 		= "\n\rhere";
		HAL_UART_Transmit(&huart1, (uint8_t*)HereString, strlen(HereString), 500);
	}
	
	// Stop ADC
	if(HAL_ADC_Stop(&hadc) != HAL_OK)
	{
		char ErrorString[50];
		strcpy(ErrorString, "\n\rHAL_ADC_Stop - Failed");
		while(1);
	}
	
	// Get reading
	uint32_t AdcReading = HAL_ADC_GetValue(&hadc);
	return AdcReading;
}

uint32_t mg_adc_Cal(void)
{
	// TypeDef for configuring ADC channel
	//ADC_ChannelConfTypeDef sConfig;
	// Set to internal temperature sensor channel
	
	//sConfig.Channel = ADC_CHANNEL_VREFINT;
	// Configure ADC
  //if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  //{
  // _Error_Handler(__FILE__, __LINE__);
  //}
	
	// Change ADC channel to internal bandgap reference voltage
	ADC1->CHSELR = ADC_CHSELR_CHSEL17;
	
	// Get reading
	uint32_t AdcReading = mg_adc_GetRawReading();
	char AdcReadingString[50];
	sprintf(AdcReadingString, "\n\rADC Cal Raw = %d", AdcReading);
	HAL_UART_Transmit(&huart1, (uint8_t*)AdcReadingString, strlen(AdcReadingString), 500);
	
	
	uint16_t vrefint_cal;                         // VREFINT calibration value
	vrefint_cal = *((uint16_t*)VREFINT_CAL_ADDR); // read VREFINT_CAL_ADDR memory location
	sprintf(AdcReadingString, "\n\rADC Cal Value = %d", vrefint_cal);
	HAL_UART_Transmit(&huart1, (uint8_t*)AdcReadingString, strlen(AdcReadingString), 500);
	
	
	
	uint32_t Vdda = 3 * (vrefint_cal / AdcReading);
	return Vdda;
	
}

uint32_t mg_adc_GetTemp(void)
{
	// TypeDef for configuring ADC channel
	//ADC_ChannelConfTypeDef sConfig;
	// Set to internal temperature sensor channel
	//sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
	// Configure ADC
  //if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  //{
  //  _Error_Handler(__FILE__, __LINE__);
  //}
	
	// Change ADC channel to internal temperature sensor
	ADC1->CHSELR = ADC_CHSELR_CHSEL18;
	
	// Get reading
	uint32_t AdcReading = mg_adc_GetRawReading();
	return AdcReading;
}

uint32_t mg_adc_GetLight(void)
{
	// TypeDef for configuring ADC channel
	//ADC_ChannelConfTypeDef sConfig;
	// Set to internal temperature sensor channel
	//sConfig.Channel = ADC_CHANNEL_10;
	// Configure ADC
  //if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  //{
  //  _Error_Handler(__FILE__, __LINE__);
  //}
	
	// Change ADC channel to light sensor pin
	ADC1->CHSELR = ADC_CHSELR_CHSEL10;
	
	// Get reading
	uint32_t AdcReading = mg_adc_GetRawReading();
	return AdcReading;
}

// close the Doxygen group
/**
\}
*/
  
/* end of file */
