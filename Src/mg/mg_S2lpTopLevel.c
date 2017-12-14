/** ***************************************************************************
*   \file        S2LP_TopLevel.c
*   \brief       Top level user code. TopLevel() called from main() and contains user program.
*
*   \copyright   Copyright (C) : <company name> <creation date YYYY-MM-DD>
*
*   \addtogroup  AddGroupsAsRequiredForTheProject
*   \{
******************************************************************************/
 
/*****************************************************************************/
// standard libraries
 
// user headers directly related to this component, ensures no dependency
#include "mg_S2lpTopLevel.h"
#include "stm32l0xx_hal.h"
#include "S2LP_Config.h"
   
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
#define BASE_FREQUENCY              868.0e6
#define MODULATION_SELECT           MOD_2FSK
#define DATARATE                    38400
#define FREQ_DEVIATION              20e3
#define BANDWIDTH                   100E3
#define POWER_DBM                   12.0



/*  Packet configuration parameters  */
#define PREAMBLE_BYTE(v)        (4*v)
#define SYNC_BYTE(v)            (8*v)

#define PREAMBLE_LENGTH             PREAMBLE_BYTE(4)
#define SYNC_LENGTH                 SYNC_BYTE(4)
#define SYNC_WORD                   0x88888888
#define VARIABLE_LENGTH             S_ENABLE
#define EXTENDED_LENGTH_FIELD       S_DISABLE
#define CRC_MODE                    PKT_CRC_MODE_8BITS
#define EN_ADDRESS                  S_DISABLE
#define EN_FEC                      S_DISABLE
#define EN_WHITENING                S_ENABLE

  
/*****************************************************************************/
// static function declarations
  
/*****************************************************************************/
// static variable declarations

/**
* @brief Radio structure fitting
*/
SRadioInit xRadioInit = {
  BASE_FREQUENCY,
  MODULATION_SELECT,
  DATARATE,
  FREQ_DEVIATION,
  BANDWIDTH
};

/**
* @brief Packet Basic structure fitting
*/
PktBasicInit xBasicInit={
  PREAMBLE_LENGTH,
  SYNC_LENGTH,
  SYNC_WORD,
  VARIABLE_LENGTH,
  EXTENDED_LENGTH_FIELD,
  CRC_MODE,
  EN_ADDRESS,
  EN_FEC,
  EN_WHITENING
};

/**
* @brief GPIO structure fitting
*/
SGpioInit xGpioIRQ={
  S2LP_GPIO_3,
  S2LP_GPIO_MODE_DIGITAL_OUTPUT_LP,
  S2LP_GPIO_DIG_OUT_IRQ
};

/**
 * @brief IRQ status struct declaration
 */
S2LPIrqs xIrqStatus;

/**
 * @brief Rx buffer declaration: how to store the received data
 */
uint8_t vectcRxBuff[128], cRxData;

/**
* @brief Declare the Tx done flag
*/
volatile FlagStatus xTxDoneFlag = RESET;

/**
* @brief Tx buffer declaration: data to transmit
*/
uint8_t vectcTxBuff[20]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
  
/*****************************************************************************/
// functions

	/** ***************************************************************************
*   \brief      Brief Function Description.
*   \details    Full detailed function description.
*   \param      <parameter 1 name>     <parameter 1 description>
*   \param      <parameter N name>     <parameter N description>
*   \return     <return code description>
*   \retval     <optional. explain individual return codes>
******************************************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin==INT_S2LP_GPIO3_Pin)
  { 
		/* Get the IRQ status */
		S2LPGpioIrqGetStatus(&xIrqStatus);
		
		/* Check the S2LP RX_DATA_DISC IRQ flag */
    if(xIrqStatus.IRQ_RX_DATA_DISC)
		{
			/* SEND SOMETHING OVER DEBUG UART... (NEED TO IMPLEMENT) */
      
			/* RX command - to ensure the device will be ready for the next reception */
			S2LPCmdStrobeRx();
		}
			
		/* Check the S2LP RX_DATA_READY IRQ flag */
    if(xIrqStatus.IRQ_RX_DATA_READY)
		{
      /* Get the RX FIFO size */
      cRxData = S2LPFifoReadNumberBytesRxFifo();
      
      /* Read the RX FIFO */
      S2LPSpiReadFifo(cRxData, vectcRxBuff);
      
      /* Flush the RX FIFO */
      S2LPCmdStrobeFlushRxFifo();
			
			/*  A simple way to check if the received data sequence is correct (in this case LED5 will toggle) */
      {
        SBool xCorrect=S_TRUE;
        
        for(uint8_t i=0 ; i<cRxData ; i++)
          if(vectcRxBuff[i] != i+1)
            xCorrect=S_FALSE;
        
        if(xCorrect)
				{
					/* SEND SOMETHING OVER DEBUG UART... (NEED TO IMPLEMENT) */
        }
      }
			
			/* RX command - to ensure the device will be ready for the next reception */
      S2LPCmdStrobeRx();
		}
	}
}
	
/** ***************************************************************************
*   \brief      Brief Function Description.
*   \details    Full detailed function description.
*   \param      <parameter 1 name>     <parameter 1 description>
*   \param      <parameter N name>     <parameter N description>
*   \return     <return code description>
*   \retval     <optional. explain individual return codes>
******************************************************************************/
void TopLevel()
{
	/* S2LP ON */
  S2LPEnterShutdown();
  S2LPExitShutdown();
	
	/* S2LP IRQ config */
  S2LPGpioInit(&xGpioIRQ);
	
	/* S2LP Radio config */
  S2LPRadioInit(&xRadioInit);
	
	/* S2LP Radio set power */
  S2LPRadioSetMaxPALevel(S_DISABLE);     // Disable transmission at maximum power
	S2LPRadioSetPALeveldBm(7,POWER_DBM);   // Set output power level for the 7th slot
	S2LPRadioSetPALevelMaxIndex(7);        // Set output power index to 7
	
	/* S2LP Packet config */
  S2LPPktBasicInit(&xBasicInit);
	
	/* S2LP IRQs enable */
  S2LPGpioIrqDeInit(&xIrqStatus);	  					// Reset IRQ register bits to 0
  S2LPGpioIrqConfig(RX_DATA_DISC,S_ENABLE);	  // Set IRQ to interrupt if Rx data has been discarded upon filtering
	S2LPGpioIrqConfig(RX_DATA_READY,S_ENABLE);	// Set IRQ to interrupt if Rx data is ready
	
	/* payload length config */
  S2LPPktBasicSetPayloadLength(20);						// Set the payload length to 20 bytes
	
	/* RX timeout config */
  S2LPTimerSetRxTimerMs(700.0);
	//SET_INFINITE_RX_TIMEOUT();
	
	/* IRQ registers blanking */
  S2LPGpioIrqClearStatus();
	
	/* RX command */
  S2LPCmdStrobeRx();
	
	/* infinite loop */
  while (1)
	{
		
	}		
}

// close the Doxygen group
/**
\}
*/
  
/* end of file */
