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
	ADC_STATE_IDLE,									// Ready
	ADC_STATE_CONVERTING,						// Between conversions
	ADC_STATE_CONVERTING_LIGHT,			// Light sensor conversion in progress
	ADC_STATE_CONVERTING_TEMP,			// Tempature sensor conversion in progress
	ADC_STATE_CONVERTING_BAT,				// Battery votage conversion in progress
	ADC_STATE_CONVERTED,						// Conversion ready
	ADC_STATE_ERROR,								// ADC error state
	
	ADC_STATE_NUM_STATES						// Number of states
} StateADC_t;
  
/*****************************************************************************/
// structures
  
/*****************************************************************************/
// constants
#define DEBUG_ADC

#define CONVERSION_AVG_WIDTH			3
#define VREFINT_CAL_ADDR    			0x1FF80078		// Memory address of the VREFINT calibration value
#define ADC_RESOLUTION						4096					// Resolution of the ADC
#define LIGHT_LOW_RANGE_GAIN			101						// Amplifier gain when in low range
#define LIGHT_HIGH_RANGE_GAIN			2							// Amplifier gain when in high range
#define LUX_CONV_FACTOR_PA				180						// Conversion factor for convting from pA to lux; output linear from 100,000 lux (18mA) to 1 lux (180pA)

/*****************************************************************************/
// macros
  
/*****************************************************************************/
// static function declarations
  
/*****************************************************************************/
// static variable declarations			
static StateADC_t stateADC = ADC_STATE_IDLE;			// ADC state machine state variable
static uint8_t firstPass = 1;

/* Light sensor range */
typedef enum
{
	LIGHT_RANGE_HIGH,
	LIGHT_RANGE_LOW
} LightRange_t;

/* Conversion status */
typedef enum
{
	CONV_IDLE,
	CONV_WAITING,
	CONV_DONE
} ConvStatus_t;

/* Reading status */
typedef enum
{
	READ_IDLE,
	READ_WAITING,
	READ_DONE
} ReadStatus_t;

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
uint32_t vRef;

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

/* Function to take a raw reading */
ConvStatus_t mg_adc_Convert(uint32_t *reading)
{
	static ConvStatus_t status;
	
	if(status == CONV_IDLE)											// If a new conversion is requested
	{
		HAL_ADC_Start_IT(&hadc);											// Then start the ADC
		status = CONV_WAITING;												// And set the status to waiting
	}
	else if(status == CONV_DONE)								// Else if the conversion is complete
	{
		status = CONV_IDLE;												// Then set the status to idle (this status allows the function to be run once after completion without starting a new conversion)
	}
	
	if(status == CONV_WAITING)									// If we are waiting for a conversion
	{
		if(adcIntFlags.flagConvComplete)							// If the conversion is complete
		{
			HAL_ADC_Stop_IT(&hadc);													// Stop the ADC
			*reading = HAL_ADC_GetValue(&hadc);							// Read the conversion result
			adcIntFlags.flagConvComplete = false;						// Reset flag
			status = CONV_DONE;															// And set the status to done
			
			char debugString[50];
			sprintf(debugString, "\n\rConversion = %d", *reading);
			HAL_UART_Transmit(&huart1, (uint8_t*)debugString, strlen(debugString), 500);
		}
	}
	return status;
}

/* Function to acquire raw readings and calculate average */
ReadStatus_t mg_adc_GetReading(uint32_t *reading)
{
	static ReadStatus_t status;
	static uint32_t convArray[CONVERSION_AVG_WIDTH];
	static uint8_t avgIndex;
	uint32_t conversion;
	
	if(status == READ_IDLE)																			// If a new reading is requested
	{
		memset(&convArray[0], 0, sizeof(convArray));									// Clear the array contents
		avgIndex = 0;																									// Reset the average index
		status = READ_WAITING;																				// And set the status
	}
	else if(status == READ_DONE)																// Else if the conversion is complete
	{
		status = READ_IDLE;																						// Then set the status to idle (this status allows the function to be run once after completion without starting a new reading)
	}
	
	if(status == READ_WAITING)																	// If waiting for a conversion
	{
		ConvStatus_t convStatus = mg_adc_Convert(&conversion);				// Get conversion status
		if(convStatus == CONV_DONE)																		// If conversion is complete
		{
			convArray[avgIndex] = conversion;																// Store the conversion
			avgIndex++;																											// Increment array index
			
			if(avgIndex >= CONVERSION_AVG_WIDTH)														// If all the conversions have been taken
			{
				uint32_t conversionAvg;
				uint8_t convArraySize = ( sizeof(convArray) / sizeof(convArray[0]) );
				uint32_t total = 0;
				
				for(uint8_t i=0; i<convArraySize; i++)														// Sum the conversions
				{
					total += convArray[i];
				}
				conversionAvg = total / convArraySize;														// Average the conversions
				*reading = conversionAvg;																					// Copy the result into the argument pointer
				status = READ_DONE;																								// And set the status
				
				char debugString[50];
				sprintf(debugString, "\n\rAveraged reading = %d", *reading);
				HAL_UART_Transmit(&huart1, (uint8_t*)debugString, strlen(debugString), 500);
			}
		}
	}
	return status;
}

/* Function to calculate battery voltage from the internal bandgap reference voltage */
uint32_t mg_adc_GetBatVoltage(uint32_t *bandgapReading)
{
	uint16_t vrefint_cal = *((uint16_t*)VREFINT_CAL_ADDR);															// Read internal reference voltage cal value
	uint32_t Vbat = (3 * (vrefint_cal*1000 / *bandgapReading*1000)) / 1000;							// Calculate battery voltage
	
	char AdcReadingString[50];
	sprintf(AdcReadingString, "\n\rVbat = %d", Vbat);
	HAL_UART_Transmit(&huart1, (uint8_t*)AdcReadingString, strlen(AdcReadingString), 500);
	
	return Vbat;
}

/* Converts raw ADC reading to mV */
uint32_t mg_adc_ConvertMv(uint32_t reading, uint32_t vBat)
{
	reading = ((reading * ((vBat*1000) / ADC_RESOLUTION)) / 1000);											// Convert to mV
	
	char AdcReadingString[50];
	sprintf(AdcReadingString, "\n\rmg_adc_ConvertMv = %d", reading);
	HAL_UART_Transmit(&huart1, (uint8_t*)AdcReadingString, strlen(AdcReadingString), 500);
	
	return reading;
}

/* Converts mV to lux */
uint32_t mg_adc_ConvertLight(uint32_t reading, LightRange_t range)
{
	#ifdef DEBUG_ADC
	char AdcReadingString[50];
	sprintf(AdcReadingString, "\n\rmg_adc_ConvertLight mV = %d", reading);
	HAL_UART_Transmit(&huart1, (uint8_t*)AdcReadingString, strlen(AdcReadingString), 500);
	#endif
	
	// Convert mV to uV
	reading *= 1000;
	
	// Convert uV to pA (100R current sense resistor): (uV/100 = uA) -> ((uV*10^3)/100 = pA) -> (mV*10 = pA) 
	if(range == LIGHT_RANGE_HIGH)
	{
		// Divide by amplifier gain
		reading /= LIGHT_HIGH_RANGE_GAIN;
		// Then convert to pA
		reading *= 10;
	}
	else if(range == LIGHT_RANGE_LOW)
	{
		// Divide by amplifier gain
		reading /= LIGHT_LOW_RANGE_GAIN;
		// Then convert to pA
		reading *= 10;
	}
	else
	{
		#ifdef DEBUG_ADC
		char AdcReadingString[50];
		sprintf(AdcReadingString, "\n\rmg_adc_ConvertLight Error: range not valid");
		HAL_UART_Transmit(&huart1, (uint8_t*)AdcReadingString, strlen(AdcReadingString), 500);
		#endif
		while(1);
	}
	#ifdef DEBUG_ADC
	sprintf(AdcReadingString, "\n\rmg_adc_ConvertLight pA = %d", reading);
	HAL_UART_Transmit(&huart1, (uint8_t*)AdcReadingString, strlen(AdcReadingString), 500);
	#endif
	
	// Convert to lux
	reading = reading / LUX_CONV_FACTOR_PA;  // 180
	#ifdef DEBUG_ADC
	sprintf(AdcReadingString, "\n\rmg_adc_ConvertLight Lux = %d", reading);
	HAL_UART_Transmit(&huart1, (uint8_t*)AdcReadingString, strlen(AdcReadingString), 500);
	#endif
	
	return reading;
}

/* Function to change state */
void mg_adc_ChangeState(StateADC_t newState)
{
	firstPass	= 1;
	stateADC  = newState;
}

/* ADC system state machine */
AdcStatusFlags_t mg_adc_StateMachine(AdcControlFlags_t adcControlFlags)
{
	AdcControlFlags_t adcControlFlagsLocal;
	
	AdcStatusFlags_t adcStatusFlags = 
	{
		.flagIdle					= 1,
		.flagConverting		= 0,
		.flagComplete			= 0
	};
	
	static uint32_t reading;
	
	switch(stateADC)
	{
		case ADC_STATE_IDLE:
		{
			if(adcControlFlags.start)													// If a new conversion should start
			{
				adcStatusFlags.flagIdle = 0;												// Then set the status flags to busy
				adcStatusFlags.flagConverting = 1;
				adcStatusFlags.flagComplete = 0;
				adcControlFlagsLocal = adcControlFlags;							// Copy the control flags locally
				mg_adc_ChangeState(ADC_STATE_CONVERTING_BAT);				// Always start by converting battery voltage (need Vref for all other sensors)
			}
			break;
		}
		
		case ADC_STATE_CONVERTING:
		{
			if(adcControlFlagsLocal.getLight)									// If a new light sensor conversion has been requested
			{
				mg_adc_ChangeState(ADC_STATE_CONVERTING_LIGHT);			// Go to converting state
			}
			else if(adcControlFlagsLocal.getTemp)							// If a new temperature sensor conversion has been requested
			{
				mg_adc_ChangeState(ADC_STATE_CONVERTING_TEMP);			// Go to converting state
			}
			else																							// Else if all flags are cleared
			{
				adcStatusFlags.flagIdle = 0;												// Then set the status flags to complete
				adcStatusFlags.flagConverting = 0;									
				adcStatusFlags.flagComplete = 1;
				mg_adc_ChangeState(ADC_STATE_CONVERTED);						// And go to converted state
			}
			break;
		}
		
		case ADC_STATE_CONVERTING_LIGHT:
		{
			static LightRange_t lightRange;
			if(firstPass)																													// If this is the first pass through
			{
				lightRange = LIGHT_RANGE_LOW;																						// Set range low
				mg_adc_SetLightRange(lightRange);
				HAL_GPIO_WritePin(SENSE_EN_GPIO_Port, SENSE_EN_Pin, GPIO_PIN_SET);			// Turn on power to ambient light sensor
				ADC1->CHSELR = ADC_CHSELR_CHSEL10;																			// Change ADC channel to light sensor pin
				firstPass = 0;																													// Clear firstPass flag
			}
			
			ReadStatus_t readStatus = mg_adc_GetReading(&reading);								// Kick the reading state machine
			
			if(readStatus == READ_DONE)																						// If the reading is complete
			{
				uint32_t reading_mV  = mg_adc_ConvertMv(reading, vRef);									// Convert reading to mV
				uint32_t reading_lux = mg_adc_ConvertLight(reading_mV, lightRange);			// Convert to lux
				adcControlFlagsLocal.getLight = false;																	// Clear light conversion flag
				mg_adc_ChangeState(ADC_STATE_CONVERTING);																// And go back to converting state
			}
			break;
		}
		
		case ADC_STATE_CONVERTING_TEMP:
		{
			if(firstPass)																													// If this is the first pass through
			{
				ADC1->CHSELR = ADC_CHSELR_CHSEL18;																			// Change ADC channel to internal temperature sensor
				firstPass = 0;																													// Clear firstPass flag
			}
			
			ReadStatus_t readStatus = mg_adc_GetReading(&reading);								// Kick the reading state machine
			
			if(readStatus == READ_DONE)																						// If the reading is complete
			{
				adcControlFlagsLocal.getTemp = false;																		// Clear temperature conversion flag
				mg_adc_ChangeState(ADC_STATE_CONVERTING);																// And go back to converting state
			}
			break;
		}
		
		case ADC_STATE_CONVERTING_BAT:
		{
			if(firstPass)																													// If this is the first pass through
			{
				ADC1->CHSELR = ADC_CHSELR_CHSEL17;																			// Change ADC channel to internal bandgap reference voltage
				firstPass = 0;																													// Clear firstPass flag
			}
			
			ReadStatus_t readStatus = mg_adc_GetReading(&reading);								// Kick the reading state machine
			
			if(readStatus == READ_DONE)																						// If the reading is complete
			{
				vRef = mg_adc_GetBatVoltage(&reading);																	// Find battery voltage
				adcControlFlagsLocal.getBat = false;																		// Clear internal bandgap voltage conversion flag
				mg_adc_ChangeState(ADC_STATE_CONVERTING);																// And go back to converting state
			}
			break;
		}
		
		case ADC_STATE_CONVERTED:
		{
			if(adcControlFlags.reset)																							// If a reset has been requested
			{
				mg_adc_ChangeState(ADC_STATE_IDLE);																			// Go to idle state
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
	return adcStatusFlags;
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
