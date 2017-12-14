/** ***************************************************************************
*   \file        FileName.h
*   \brief       Brief description of the file
*
*   \copyright   Copyright (C) : <company name> <creation date YYYY-MM-DD>
*
*   \addtogroup  AddGroupsAsRequiredForTheProject
*   \{
******************************************************************************/
 
#ifndef __MG_S2LPTOPLEVEL_H
#define __MG_S2LPTOPLEVEL_H
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
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void TopLevel(void);
  
/*****************************************************************************/
// variables
  
/*****************************************************************************/
// functions
  
#endif /* __MG_S2LPTOPLEVEL_H */
// close the Doxygen group
/**
\}
*/
  
/* end of file */
