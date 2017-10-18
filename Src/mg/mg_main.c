/** ***************************************************************************
*   \file        mg_main.c
*   \brief       Top level program
*
*   \copyright   Copyright (C) : <company name> <creation date YYYY-MM-DD>
*
*   \addtogroup  AddGroupsAsRequiredForTheProject
*   \{
******************************************************************************/
 
/*****************************************************************************/
// standard libraries
 
// user headers directly related to this component, ensures no dependency
#include "mg_main.h"
#include "mg_state_machine.h"
   
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
// static function declarations
  
/*****************************************************************************/
// static variable declarations

/*****************************************************************************/
// variable declarations
  
/*****************************************************************************/
// functions

void mg_main_Main(void)
{
	while(1)
	{
		mg_state_machine();
	}
}


// close the Doxygen group
/**
\}
*/
  
/* end of file */