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
#include "stm32l0xx_hal.h"
 
// user headers directly related to this component, ensures no dependency
 
// user headers from other components
 
/*****************************************************************************/
// enumerations
  
/*****************************************************************************/
// typedefs
typedef enum
{
	LIGHT_RANGE_HIGH,
	LIGHT_RANGE_LOW
} LightRange_t;
  
/*****************************************************************************/
// structures
  
/*****************************************************************************/
// constants
  
/*****************************************************************************/
// macros
  
/*****************************************************************************/
// function declarations
uint32_t mg_adc_GetRawReading(void);
uint32_t mg_adc_GetVbat(void);
uint32_t mg_adc_ConvertMv(uint32_t reading, uint32_t vBat);
uint32_t mg_adc_ScaleReadingTo3V(uint32_t reading);
uint32_t mg_adc_GetTemp(void);
void mg_adc_SetLightRange(LightRange_t range);
uint32_t mg_adc_ConvertLight(uint32_t reading, LightRange_t range);
uint32_t mg_adc_GetLight(void);
  
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
