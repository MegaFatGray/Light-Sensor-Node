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

S2LPIrqs xIrqStatus;
  
/*****************************************************************************/
// structures
  
/*****************************************************************************/
// constants
  
/*****************************************************************************/
// macros

/* Switch to change from Rx to Tx code */
#define RX

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
extern UART_HandleTypeDef huart1;

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
char transmitString[20] = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  
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
void TopLevel()
{
	/* Tx startup string */
	#ifndef RX
		uint8_t mystring[] = "\r\nLight Sensor Node - Tx";
		HAL_UART_Transmit(&huart1, mystring, sizeof(mystring), 500);
	#endif
	
	/* Rx startup string */
	#ifdef RX
		uint8_t mystring[] = "\r\nLight Sensor Node - Rx";
		HAL_UART_Transmit(&huart1, mystring, sizeof(mystring), 500);
	#endif
	
	/* S2LP ON */
  S2LPEnterShutdown();
  S2LPExitShutdown();
	
	/* Allow time for S2LP to power up */
	HAL_Delay(10);
	
	/* S2LP IRQ config */
  S2LPGpioInit(&xGpioIRQ);
	
	/* S2LP Radio config */
  S2LPRadioInit(&xRadioInit);
	
	/* S2LP Radio set power */
  S2LPRadioSetMaxPALevel(S_ENABLE);      // Enable transmission at maximum power
	S2LPRadioSetPALeveldBm(7,POWER_DBM);   // Set output power level for the 7th slot
	S2LPRadioSetPALevelMaxIndex(7);        // Set output power index to 7
	
	/* S2LP Packet config */
  S2LPPktBasicInit(&xBasicInit);
	
	/* Tx initialisation */
	#ifndef RX
		/* S2LP IRQs enable */
		S2LPGpioIrqDeInit(NULL);										// Reset IRQ register bits to 0
		S2LPGpioIrqConfig(TX_DATA_SENT , S_ENABLE);	// Set IRQ to interrupt when data has been transmitted
	#endif
	
	/* Rx initialisation */
	#ifdef RX
		/* S2LP IRQs enable */
		S2LPGpioIrqDeInit(&xIrqStatus);	  					// Reset IRQ register bits to 0
		S2LPGpioIrqConfig(RX_DATA_DISC,S_ENABLE);	  // Set IRQ to interrupt if Rx data has been discarded upon filtering
		S2LPGpioIrqConfig(RX_DATA_READY,S_ENABLE);	// Set IRQ to interrupt if Rx data is ready
		/* RX timeout config */
		S2LPTimerSetRxTimerMs(700.0);
	#endif
	
	/* payload length config */
  S2LPPktBasicSetPayloadLength(20);						// Set the payload length to 20 bytes
	
	/* IRQ registers blanking */
  S2LPGpioIrqClearStatus();
	
	#ifdef RX
		/* RX command */
		S2LPCmdStrobeRx();
	#endif
	
	/* infinite loop */
  while (1)
	{
		/* -------------------- Tx -------------------- */
		#ifndef RX
			/* fit the TX FIFO */
			S2LPCmdStrobeFlushTxFifo();														// Flush Tx FIFO
			S2LPSpiWriteFifo(20, (uint8_t*)transmitString);				// Write to Tx FIFO
		
			/* send the TX command */
			S2LPCmdStrobeTx();
		
			/* wait for TX done */
			while(!xTxDoneFlag);
			xTxDoneFlag = RESET;
		
			/* pause between two transmissions */
			HAL_Delay(500);
		
		#endif
		
		/* -------------------- Rx -------------------- */
		#ifdef RX
			
		#endif
	}		
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	/* -------------------- Tx -------------------- */
	#ifndef RX
		// add check for INT_S2LP_GPIO3
		//xTxDoneFlag = SET;
		
		// from example project...
		if(GPIO_Pin==GPIO_PIN_2)
		{ 
			// Get the IRQ status
			S2LPGpioIrqGetStatus(&xIrqStatus);
			
			// Check the SPIRIT TX_DATA_SENT IRQ flag
			if(xIrqStatus.IRQ_TX_DATA_SENT)
			{
				// set the tx_done_flag to manage the event in the main()
				xTxDoneFlag = SET;
				
				// toggle LED1
				HAL_GPIO_TogglePin(LED_GRN_GPIO_Port, LED_GRN_Pin);
			}
		}
	#endif
	
	/* -------------------- Rx -------------------- */
	#ifdef RX
		HAL_GPIO_TogglePin(LED_GRN_GPIO_Port, LED_GRN_Pin);
	
		if(GPIO_Pin==INT_S2LP_GPIO3_Pin)
		{ 
			/* Get the IRQ status */
			S2LPGpioIrqGetStatus(&xIrqStatus);
			
			/* Check the S2LP RX_DATA_DISC IRQ flag */
			if(xIrqStatus.IRQ_RX_DATA_DISC)
			{
				/* error - data discarded */
				uint8_t debugString[] = {"\r\nRx data discarded"};
				HAL_UART_Transmit(&huart1, debugString, sizeof(debugString), 500);
				
				/* RX command - to ensure the device will be ready for the next reception */
				S2LPCmdStrobeRx();
			}
				
			/* Check the S2LP RX_DATA_READY IRQ flag */
			else if(xIrqStatus.IRQ_RX_DATA_READY)
			{
				/* Get the RX FIFO size */
				cRxData = S2LPFifoReadNumberBytesRxFifo();
				
				/* Read the RX FIFO */
				S2LPSpiReadFifo(cRxData, vectcRxBuff);
				
				/* Flush the RX FIFO */
				S2LPCmdStrobeFlushRxFifo();
				
				/* Output Rx data to UART */
				uint8_t debugString[] = {"\r\nRx data:\r\n"};
				HAL_UART_Transmit(&huart1, debugString, sizeof(debugString), 500);
				HAL_UART_Transmit(&huart1, vectcRxBuff, cRxData, 500);
				
				/* RX command - to ensure the device will be ready for the next reception */
				S2LPCmdStrobeRx();
			}
			
			/* If IRQ status is anything else */
			else
			{
				uint8_t debugString[] = {"\r\nUnexpected IRQ status"};
				HAL_UART_Transmit(&huart1, debugString, sizeof(debugString), 500);
			}
		}
	#endif
}

// close the Doxygen group
/**
\}
*/
  
/* end of file */
