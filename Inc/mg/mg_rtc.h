/** ***************************************************************************
*   \file        Filename.h
*   \brief       Brief description of the file
*
*   \copyright   Copyright (C) : <company name> <creation date YYYY-MM-DD>
*
*   \addtogroup  AddGroupsAsRequiredForTheProject
*   \{
******************************************************************************/
 
#ifndef __MG_RTC_H
#define __MG_RTC_H
/*****************************************************************************/
// standard libraries first
#include "stm32l0xx_hal.h"
 
// user headers directly related to this component, ensures no dependency
 
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
void RTC_Set(void);
void RTC_Get(RTC_DateTypeDef *date, RTC_TimeTypeDef *time);
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc);

  
/*****************************************************************************/
// variables
  
/*****************************************************************************/
// functions
  
#endif /* __MG_RTC_H */
// close the Doxygen group
/**
\}
*/
  
/* end of file */
