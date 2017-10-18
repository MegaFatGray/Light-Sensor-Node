/** ***************************************************************************
*   \file        Filename.h
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
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc);
void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc);
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc);
void mg_adc_GetReading(void);
  
/*****************************************************************************/
// variables
  
/*****************************************************************************/
// functions
  
#endif /* __MG_ADC_H */
// close the Doxygen group
/**
\}
*/
  
/* end of file */
