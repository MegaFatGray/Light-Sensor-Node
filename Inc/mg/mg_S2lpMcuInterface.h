/** ***************************************************************************
*   \file        MCU_Interface.h
*   \brief       Interface between STM32L053R8 MCU and S2LP API
*
*   \copyright   Copyright (C) : <company name> <creation date YYYY-MM-DD>
*
*   \addtogroup  AddGroupsAsRequiredForTheProject
*   \{
******************************************************************************/
 
#ifndef MG_S2LPMCUINTERFACE_H
#define MG_S2LPMCUINTERFACE_H
/*****************************************************************************/
// standard libraries first
 
// user headers directly related to this component, ensures no dependency
#include "S2LP_Config.h"
#include "stm32l0xx_hal.h"
 
// user headers from other components

#ifdef __cplusplus
extern "C" {
#endif
 
/*****************************************************************************/
// enumerations
  
/*****************************************************************************/
// typedefs
typedef S2LPStatus StatusBytes;
	
/*****************************************************************************/
// structures
  
/*****************************************************************************/
// constants
  
/*****************************************************************************/
// macros
  
/*****************************************************************************/
// function declarations
void S2LPEnterShutdown(void);
void S2LPExitShutdown(void);
GPIO_PinState S2LPCheckShutdown(void);
 
void S2LPSpiInit(void);
void S2LPSpiDeinit(void);
StatusBytes S2LPSpiWriteRegisters(uint8_t cRegAddress, uint8_t cNbBytes, uint8_t *pcBuffer);
StatusBytes S2LPSpiReadRegisters(uint8_t cRegAddress, uint8_t cNbBytes, uint8_t *pcBuffer);
StatusBytes S2LPSpiCommandStrobes(uint8_t cCommandCode);
StatusBytes S2LPSpiWriteFifo(uint8_t cNbBytes, uint8_t *pcBuffer);
StatusBytes S2LPSpiReadFifo(uint8_t cNbBytes, uint8_t *pcBuffer);

/*
void SdkEvalSpiInit(void);
void SdkEvalSpiDeinit(void);
StatusBytes SdkEvalSpiWriteRegisters(uint8_t cRegAddress, uint8_t cNbBytes, uint8_t* pcBuffer);
StatusBytes SdkEvalSpiReadRegisters(uint8_t cRegAddress, uint8_t cNbBytes, uint8_t* pcBuffer);
StatusBytes SdkEvalSpiCommandStrobes(uint8_t cCommandCode);
StatusBytes SdkEvalSpiWriteFifo(uint8_t cNbBytes, uint8_t* pcBuffer);
StatusBytes SdkEvalSpiReadFifo(uint8_t cNbBytes, uint8_t* pcBuffer);

void SdkEvalEnterShutdown(void);
void SdkEvalExitShutdown(void);
SFlagStatus SdkEvalCheckShutdown(void);
*/

/**
 * @}
 */
  

/** @defgroup SPI_Exported_Macros       SPI Exported Macros
 * @{
 */



/**
 * @}
 */
	
/*****************************************************************************/
// variables
  
/*****************************************************************************/
// functions
  
	
#ifdef __cplusplus
}
#endif

#endif //MG_S2LPMCUINTERFACE_H
// close the Doxygen group
/**
\}
*/
  
/* end of file */
