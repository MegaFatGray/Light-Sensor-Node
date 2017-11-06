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
void mg_adc_StateMachine(void);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc);
void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc);
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc);
void mg_adc_StartReading(void);
  
/*****************************************************************************/
// variables
extern bool flagStartConv;						// Flag to indicate if a new conversion should start
extern bool flagConvDone; 						// Flag to indicate conversion is recorded
/* Flags for external control of ADC state machine */
typedef union {
    struct
    {
        uint8_t flagIdle			 			: 1;		// Flag to indicate ADC state machine is idle
				uint8_t flagStartConv 			: 1;		// Flag to indicate a new conversion should start
        uint8_t flagConvInProgress	: 1;		// Flag to indicate conversion is in progress
        uint8_t flagConvDone 				: 1;		// Flag to indicate conversion is recorded
    };
    uint8_t adcExtFlags;
} AdcExtFlags_t;
extern AdcExtFlags_t adcExtFlags;
  
/*****************************************************************************/
// functions
  
#endif /* __MG_ADC_H */
// close the Doxygen group
/**
\}
*/
  
/* end of file */
