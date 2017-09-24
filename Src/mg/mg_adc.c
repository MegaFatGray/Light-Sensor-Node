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
#include "mg_global_defs.h"
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
#define VREFINT_CAL_ADDR    0x1FF80078
#define T_SENSE_CAL1_ADDR		0x1FF8007A
#define T_SENSE_CAL2_ADDR		0x1FF8007E
#define ADC_RESOLUTION			4096

/*****************************************************************************/
// macros
  
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
		char HereString[50] 		= "\n\rHAL_ADC_PollForConversion - Not okay";
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

uint32_t mg_adc_GetVbat(void)
{
	// Change ADC channel to internal bandgap reference voltage
	ADC1->CHSELR = ADC_CHSELR_CHSEL17;
	
	// Get reading
	uint32_t AdcReading = mg_adc_GetRawReading();
	
	// Read internal reference voltage cal value
	uint16_t vrefint_cal;
	vrefint_cal = *((uint16_t*)VREFINT_CAL_ADDR);
	
	// Calculate battery voltage
	uint32_t Vbat = (3 * (vrefint_cal*1000 / AdcReading*1000)) / 1000;
	return Vbat;
}

// ADC sampling time must be >10us
// CubeMX configured for synchronous ADC clock with no prescalar
// System clock 16MHz
// Therefore minimum of 160 clock cycles 
uint32_t mg_adc_GetTemp(void)
{
	// Change ADC channel to internal temperature sensor
	ADC1->CHSELR = ADC_CHSELR_CHSEL18;
	
	// Get reading
	uint32_t AdcReading = mg_adc_GetRawReading();
	
	#ifdef DEBUG_ADC
	char AdcReadingString[50];
	sprintf(AdcReadingString, "\n\rReading = %d", AdcReading);
	HAL_UART_Transmit(&huart1, (uint8_t*)AdcReadingString, strlen(AdcReadingString), 500);
	#endif
	
	// Scale reading to cal values (Vdda=3V)
	AdcReading = ( AdcReading * mg_adc_GetVbat() ) / 3;
	AdcReading /= 1000;
	
	#ifdef DEBUG_ADC
	sprintf(AdcReadingString, "\n\rScaled Reading = %d", AdcReading);
	HAL_UART_Transmit(&huart1, (uint8_t*)AdcReadingString, strlen(AdcReadingString), 500);
	#endif
	
	// Read internal temperature cal values
	uint16_t tSenseCal1;
	tSenseCal1 = *((uint16_t*)T_SENSE_CAL1_ADDR);
	
	#ifdef DEBUG_ADC
	sprintf(AdcReadingString, "\n\rtSenseCal1 = %d", tSenseCal1);
	HAL_UART_Transmit(&huart1, (uint8_t*)AdcReadingString, strlen(AdcReadingString), 500);
	#endif
	
	uint16_t tSenseCal2;
	tSenseCal2 = *((uint16_t*)T_SENSE_CAL2_ADDR);
	
	#ifdef DEBUG_ADC
	sprintf(AdcReadingString, "\n\rtSenseCal2 = %d", tSenseCal2);
	HAL_UART_Transmit(&huart1, (uint8_t*)AdcReadingString, strlen(AdcReadingString), 500);
	#endif
	
	// Calculate temperature
	uint32_t tempDegC = ( ( ( (130-30)*1000 ) / (tSenseCal2 - tSenseCal1) ) * (AdcReading - tSenseCal1) / 1000 ) + 30;
	
	#ifdef DEBUG_ADC
	sprintf(AdcReadingString, "\n\rtempDegC = %d", tempDegC);
	HAL_UART_Transmit(&huart1, (uint8_t*)AdcReadingString, strlen(AdcReadingString), 500);
	#endif
	
	return tempDegC;
}

uint32_t mg_adc_GetLight(void)
{
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
