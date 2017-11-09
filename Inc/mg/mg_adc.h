/** ***************************************************************************
*   \file        mg_adc.h
*   \brief       Brief description of the file
*
*   \copyright   Copyright (C) : <company name> <creation date YYYY-MM-DD>
*
*   \addtogroup  AddGroupsAsRequiredForTheProject
*   \{
******************************************************************************/
 
#ifndef __MG_ADC_H
#define __MG_ADC_H
/*****************************************************************************/
// standard libraries first
 
// user headers directly related to this component, ensures no dependency
#include "stm32l0xx_hal.h"

#pragma anon_unions
 
// user headers from other components
 
/*****************************************************************************/
// enumerations
  
/*****************************************************************************/
// typedefs
  
/*****************************************************************************/
// structures
  
/*****************************************************************************/
// constants
  
/*****************************************************************************/
// macros
  
/*****************************************************************************/
// function declarations
  
/*****************************************************************************/
// variables
/* Flags for external control of ADC state machine */
typedef union {
    struct
    {
			uint8_t getLight 						: 1;		// Flag to indicate a light sensor conversion is requested
			uint8_t getTemp							: 1;		// Flag to indicate a temperature sensor conversion is requested
			uint8_t getBat							: 1;		// Flag to indicate a battery voltage conversion is required
    };
    uint8_t adcControlFlags;
} AdcControlFlags_t;

extern AdcControlFlags_t adcControlFlags;

/* Flags for status of ADC state machine */
typedef union {
    struct
    {
			uint8_t flagIdle			 			: 1;		// Flag to indicate ADC state machine is idle
      uint8_t flagConverting			: 1;		// Flag to indicate conversion is in progress
      uint8_t flagComplete 				: 1;		// Flag to indicate conversion is recorded	
    };
    uint8_t adcStatusFlags;
} AdcStatusFlags_t;

extern AdcStatusFlags_t adcStatusFlags;
  
/*****************************************************************************/
// functions
AdcStatusFlags_t mg_adc_StateMachine(AdcControlFlags_t adcControlFlags);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc);
void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc);
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc);
void mg_adc_StartReading(void);
  
#endif /* __MG_ADC_H */
// close the Doxygen group
/**
\}
*/
  
/* end of file */
