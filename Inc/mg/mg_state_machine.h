/** ***************************************************************************
*   \file        mg_state_machine.h
*   \brief       State machine module header file
*
*   \copyright   Copyright (C) : <company name> <creation date YYYY-MM-DD>
*
*   \addtogroup  AddGroupsAsRequiredForTheProject
*   \{
******************************************************************************/
 
#ifndef __MG_STATE_MACHINE_H
#define __MG_STATE_MACHINE_H
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
  
/*****************************************************************************/
// variables
extern uint8_t rtcInterrupt;
  
/*****************************************************************************/
// functions
void mg_state_machine(void);
  
#endif /* __MG_STATE_MACHINE_H */
// close the Doxygen group
/**
\}
*/
  
/* end of file */
