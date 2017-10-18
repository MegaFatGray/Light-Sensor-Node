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
#define VREFINT_CAL_ADDR    			0x1FF80078		// Memory address of the VREFINT calibration value
#define T_SENSE_CAL1_ADDR					0x1FF8007A		// Memory address of the TSENSECAL1 calibration value
#define T_SENSE_CAL2_ADDR					0x1FF8007E		// Memory address of the TSENSECAL2 calibration value
#define ADC_RESOLUTION						4096					// Resolution of the ADC
#define LIGHT_SETTLE_TIME_MS			50						// Time in ms to allow the sensor output to settle
#define LUX_CONV_FACTOR_PA				180						// Conversion factor for convting from pA to lux
#define LIGHT_RANGE_THRESHOLD			5							// If the reading in low range is within this percentage of the battery voltage, use high range instead
																								// If the reading in high range is within this percentage of the battery voltage above zero, use low range instead
#define ADC_AVERAGE_WIDTH					5							// Number of ADC samples to average together for a single reading

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

/* Returns a raw ADC reading */
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
	
	#ifdef DEBUG_ADC
	char AdcReadingString[50];
	sprintf(AdcReadingString, "\n\rRaw Reading = %d", AdcReading);
	HAL_UART_Transmit(&huart1, (uint8_t*)AdcReadingString, strlen(AdcReadingString), 500);
	#endif
	
	return AdcReading;
}

/* Returns an averaged ADC reading */
uint32_t mg_adc_GetReading(void)
{
	uint32_t reading = 0;
	
	for(uint8_t i=0; i<ADC_AVERAGE_WIDTH; i++)
	{
		reading += mg_adc_GetRawReading();
		HAL_Delay(1);
	}
	reading /= ADC_AVERAGE_WIDTH;
	
	#ifdef DEBUG_ADC
	char AdcReadingString[50];
	sprintf(AdcReadingString, "\n\rAveraged reading = %d", reading);
	HAL_UART_Transmit(&huart1, (uint8_t*)AdcReadingString, strlen(AdcReadingString), 500);
	#endif
	
	return reading;
}

/* Returns battery voltage */
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

/* Converts raw ADC reading to mV */
uint32_t mg_adc_ConvertMv(uint32_t reading, uint32_t vBat)
{
	// Convert to mV
	reading = ((reading * ((vBat*1000) / ADC_RESOLUTION)) / 1000);
	return reading;
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

/* Converts mV to lux */
uint32_t mg_adc_ConvertLight(uint32_t reading, LightRange_t range)
{
	#ifdef DEBUG_ADC
	char AdcReadingString[50];
	sprintf(AdcReadingString, "\n\rreading = %d", reading);
	HAL_UART_Transmit(&huart1, (uint8_t*)AdcReadingString, strlen(AdcReadingString), 500);
	#endif
	
	// Convert to pA
	if(range == LIGHT_RANGE_HIGH)
	{
		reading = (reading*1000*1000 / 100);
	}
	else if(range == LIGHT_RANGE_LOW)
	{
		reading = (reading*1000*10 / 100);
	}
	else
	{
		#ifdef DEBUG_ADC
		sprintf(AdcReadingString, "\n\rError: range not valid");
		HAL_UART_Transmit(&huart1, (uint8_t*)AdcReadingString, strlen(AdcReadingString), 500);
		#endif
		while(1);
	}
	#ifdef DEBUG_ADC
	sprintf(AdcReadingString, "\n\rreading_pA = %d", reading);
	HAL_UART_Transmit(&huart1, (uint8_t*)AdcReadingString, strlen(AdcReadingString), 500);
	#endif
	
	// Convert to lux
	reading = reading / LUX_CONV_FACTOR_PA;
	#ifdef DEBUG_ADC
	sprintf(AdcReadingString, "\n\rreading_lux = %d", reading);
	HAL_UART_Transmit(&huart1, (uint8_t*)AdcReadingString, strlen(AdcReadingString), 500);
	#endif
	
	return reading;
}

uint32_t mg_adc_GetLightReading(void)
{
	// Turn on power to ambient light sensor
	HAL_GPIO_WritePin(SENSE_EN_GPIO_Port, SENSE_EN_Pin, GPIO_PIN_SET);
	
	// Record time at which output was enabled
	uint32_t tickStart = HAL_GetTick();
	
	// Change ADC channel to light sensor pin
	ADC1->CHSELR = ADC_CHSELR_CHSEL10;
	
	// Wait for settle time to elapse
	while( (HAL_GetTick() - tickStart) < LIGHT_SETTLE_TIME_MS )
	{		}
	
	// Get reading
	uint32_t reading = mg_adc_GetReading();
	
	// Turn off power to ambient light sensor
	HAL_GPIO_WritePin(SENSE_EN_GPIO_Port, SENSE_EN_Pin, GPIO_PIN_RESET);
	
	return reading;
}

/* Returns a reading in lux */
uint32_t mg_adc_GetLight(void)
{
	static LightRange_t lightRange = LIGHT_RANGE_LOW;

	// Take a reading of battery voltage for use throughout this function
	//uint32_t vBat = mg_adc_GetVbat();
	
	// Set the range
	mg_adc_SetLightRange(lightRange);
	
	// Get ADC reading
	uint32_t reading = mg_adc_GetLightReading();
	
	// Convert to mV
	//reading = mg_adc_ConvertMv(reading, vBat);
	
	/*
	// If in low range and the reading is saturated, go to high range and take another reading
	if( (lightRange == LIGHT_RANGE_LOW) && (reading > ( ( (100 - LIGHT_RANGE_THRESHOLD) * vBat ) / 100 ) ) )
	{
		lightRange = LIGHT_RANGE_HIGH;
		HAL_Delay(LIGHT_SETTLE_TIME_MS);
		reading = mg_adc_GetLightReading();
	}
	// Else if in the high range and the reading is too low, go to low range and take another reading
	else if( (lightRange == LIGHT_RANGE_HIGH) && (reading < (LIGHT_RANGE_THRESHOLD * vBat ) ) )
	{
		lightRange = LIGHT_RANGE_LOW;
		HAL_Delay(LIGHT_SETTLE_TIME_MS);
		reading = mg_adc_GetLightReading();
	}
	// Else the reading is in range so convert to lux
	else
	{
		// Convert to lux
		reading = mg_adc_ConvertLight(reading, lightRange);
	}
	*/
	
	return reading;
}

// close the Doxygen group
/**
\}
*/
  
/* end of file */
