/** ***************************************************************************
*   \file        MCU_Interface.c
*   \brief       Interface between STM32L053R8 MCU and S2LP API
*
*   \copyright   Copyright (C) : <company name> <creation date YYYY-MM-DD>
*
*   \addtogroup  AddGroupsAsRequiredForTheProject
*   \{
******************************************************************************/
 
/*****************************************************************************/
// standard libraries
 
// user headers directly related to this component, ensures no dependency
#include "mg_S2lpMcuInterface.h"
#include "stm32l0xx_hal.h"
#include "main.h"
   
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

// SPI_Private_Defines
#define CS_TO_SCLK_DELAY  0x0001
#define CLK_TO_CS_DELAY   0x0001

// SPI_Headers
#define HEADER_WRITE_MASK     0x00 /*!< Write mask for header byte*/
#define HEADER_READ_MASK      0x01 /*!< Read mask for header byte*/
#define HEADER_ADDRESS_MASK   0x00 /*!< Address mask for header byte*/
#define HEADER_COMMAND_MASK   0x80 /*!< Command mask for header byte*/

#define LINEAR_FIFO_ADDRESS 0xFF  /*!< Linear FIFO address*/

// SPI_Private_Macros
#define BUILT_HEADER(add_comm, w_r) (add_comm | w_r)  /*!< macro to build the header byte*/
#define WRITE_HEADER    BUILT_HEADER(HEADER_ADDRESS_MASK, HEADER_WRITE_MASK) /*!< macro to build the write header byte*/
#define READ_HEADER     BUILT_HEADER(HEADER_ADDRESS_MASK, HEADER_READ_MASK)  /*!< macro to build the read header byte*/
#define COMMAND_HEADER  BUILT_HEADER(HEADER_COMMAND_MASK, HEADER_WRITE_MASK) /*!< macro to build the command header byte*/

//SPI_Private_Variables
#define WAIT_FOR_SPI_TC()               {while(!__HAL_DMA_GET_FLAG(hspi1.hdmarx, __HAL_DMA_GET_TC_FLAG_INDEX(hspi1.hdmarx)));\
                                          HAL_DMA_IRQHandler(hspi1.hdmarx);\
                                          HAL_DMA_IRQHandler(hspi1.hdmatx);\
                                            while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);}

// SPI_Private_FunctionPrototypes
#define SPI_ENTER_CRITICAL()           __disable_irq()
#define SPI_EXIT_CRITICAL()            __enable_irq()
  
/*****************************************************************************/
// static function declarations
  
/*****************************************************************************/
// static variable declarations
static uint8_t tx_buff[128];
static uint8_t rx_buff[128];

/*****************************************************************************/
// variable declarations
extern SPI_HandleTypeDef hspi1;
  
/*****************************************************************************/
// functions

void S2LP_CS_LOW(void)
{
	/* Set low the GPIO connected to CS pin */
  HAL_GPIO_WritePin(nCS_S2LP_GPIO_Port, nCS_S2LP_Pin, GPIO_PIN_RESET);
}

void S2LP_CS_HIGH(void)
{
	/* Set high the GPIO connected to CS pin */
  HAL_GPIO_WritePin(nCS_S2LP_GPIO_Port, nCS_S2LP_Pin, GPIO_PIN_SET);
}

/** ***************************************************************************
*   \brief      Brief Function Description.
*   \details    Full detailed function description.
*   \param      <parameter 1 name>     <parameter 1 description>
*   \param      <parameter N name>     <parameter N description>
*   \return     <return code description>
*   \retval     <optional. explain individual return codes>
******************************************************************************/
void S2LPEnterShutdown(void)
{
	/* Set high the GPIO connected to shutdown pin */
  HAL_GPIO_WritePin(nS2LP_EN_GPIO_Port, nS2LP_EN_Pin, GPIO_PIN_SET);
}

void S2LPExitShutdown(void)
{
	/* Set low the GPIO connected to shutdown pin */
  HAL_GPIO_WritePin(nS2LP_EN_GPIO_Port, nS2LP_EN_Pin, GPIO_PIN_RESET);
}

GPIO_PinState S2LPCheckShutdown(void)
{
  return  HAL_GPIO_ReadPin(nS2LP_EN_GPIO_Port, nS2LP_EN_Pin);
}

// Exported function prototypes

void S2LPSpiInit(void)
{
	// SPI already initiated in Cube generated start up code
}

void S2LPSpiDeinit(void)
{
	// Not required - omitted until required
}

/**
* @brief  Write single or multiple register
* @param  cRegAddress: base register's address to be write
* @param  cNbBytes: number of registers and bytes to be write
* @param  pcBuffer: pointer to the buffer of values have to be written into registers
* @retval Device status
*/
StatusBytes S2LPSpiWriteRegisters(uint8_t cRegAddress, uint8_t cNbBytes, uint8_t *pcBuffer)
{
	//HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout)
	
	tx_buff[0]=WRITE_HEADER;
  tx_buff[1]=cRegAddress;
	
	StatusBytes status;
	
	for(uint32_t i=0;i<cNbBytes;i++)
  {
    tx_buff[i+2]=pcBuffer[i];
  }
	
	SPI_ENTER_CRITICAL();
	
	/* Puts the SPI chip select low to start the transaction */
  S2LP_CS_LOW();
	
	//HAL_SPI_TransmitReceive_DMA(&hspi1, tx_buff, rx_buff, 2+cNbBytes);
	HAL_SPI_TransmitReceive(&hspi1, tx_buff, rx_buff, cNbBytes, 100);
	
	// line below included in example project but not using SPI DMA so not included
	//WAIT_FOR_SPI_TC();
	
	/* Puts the SPI chip select high to end the transaction */
  S2LP_CS_HIGH();
	
	SPI_EXIT_CRITICAL();
	
	((uint8_t*)&status)[1]=rx_buff[0];
  ((uint8_t*)&status)[0]=rx_buff[1];
	
	return status;
}

/**
* @brief  Read single or multiple register
* @param  cRegAddress: base register's address to be read
* @param  cNbBytes: number of registers and bytes to be read
* @param  pcBuffer: pointer to the buffer of registers' values read
* @retval Device status
*/
StatusBytes S2LPSpiReadRegisters(uint8_t cRegAddress, uint8_t cNbBytes, uint8_t *pcBuffer)
{
	tx_buff[0]=READ_HEADER;
  tx_buff[1]=cRegAddress;
  
  StatusBytes status;
  
  SPI_ENTER_CRITICAL();
  S2LP_CS_LOW();
  
  //HAL_SPI_TransmitReceive_DMA(&hspi1, tx_buff, rx_buff, 2+cNbBytes);
	HAL_SPI_TransmitReceive(&hspi1, tx_buff, rx_buff, 2+cNbBytes, 100);
	
	// line below included in example project but not using SPI DMA so not included
  //WAIT_FOR_SPI_TC();
  
  S2LP_CS_HIGH();
  SPI_EXIT_CRITICAL();
  
  for(uint32_t i=0;i<cNbBytes;i++)
  {
    pcBuffer[i]=rx_buff[i+2];
  }
  
  ((uint8_t*)&status)[1]=rx_buff[0];
  ((uint8_t*)&status)[0]=rx_buff[1];  
  
  return status;
}

/**
* @brief  Send a command
* @param  cCommandCode: command code to be sent
* @retval Device status
*/
StatusBytes S2LPSpiCommandStrobes(uint8_t cCommandCode)
{
	tx_buff[0]=COMMAND_HEADER;
  tx_buff[1]=cCommandCode;
  
  
  StatusBytes status;
  
  SPI_ENTER_CRITICAL();
  S2LP_CS_LOW();
  
  //HAL_SPI_TransmitReceive_DMA(&hspi1, tx_buff, rx_buff, 2);
	HAL_SPI_TransmitReceive(&hspi1, tx_buff, rx_buff, 2, 100);
	
	// line below included in example project but not using SPI DMA so not included
  //WAIT_FOR_SPI_TC();
  
  S2LP_CS_HIGH();
  SPI_EXIT_CRITICAL();
  
  ((uint8_t*)&status)[1]=rx_buff[0];
  ((uint8_t*)&status)[0]=rx_buff[1];
  
  return status;
}

/**
* @brief  Write data into TX FIFO
* @param  cNbBytes: number of bytes to be written into TX FIFO
* @param  pcBuffer: pointer to data to write
* @retval Device status
*/
StatusBytes S2LPSpiWriteFifo(uint8_t cNbBytes, uint8_t *pcBuffer)
{
	tx_buff[0]=WRITE_HEADER;
  tx_buff[1]=LINEAR_FIFO_ADDRESS;
  
  StatusBytes status;
  
  for(uint32_t i=0;i<cNbBytes;i++)
  {
    tx_buff[i+2]=pcBuffer[i];
  }
  
  SPI_ENTER_CRITICAL();
  S2LP_CS_LOW();
  
  //HAL_SPI_TransmitReceive_DMA(&hspi1, tx_buff, rx_buff, 2+cNbBytes);
	HAL_SPI_TransmitReceive(&hspi1, tx_buff, rx_buff, 2+cNbBytes, 100);
	
	// line below included in example project but not using SPI DMA so not included
  //WAIT_FOR_SPI_TC();
  
  S2LP_CS_HIGH();
  SPI_EXIT_CRITICAL();
  
  ((uint8_t*)&status)[1]=rx_buff[0];
  ((uint8_t*)&status)[0]=rx_buff[1];
  
  return status;
}

/**
* @brief  Read data from RX FIFO
* @param  cNbBytes: number of bytes to read from RX FIFO
* @param  pcBuffer: pointer to data read from RX FIFO
* @retval Device status
*/
StatusBytes S2LPSpiReadFifo(uint8_t cNbBytes, uint8_t *pcBuffer)
{
	tx_buff[0]=READ_HEADER;
  tx_buff[1]=LINEAR_FIFO_ADDRESS;
  
  StatusBytes status;
  
  SPI_ENTER_CRITICAL();
  S2LP_CS_LOW();
  
  //HAL_SPI_TransmitReceive_DMA(&hspi1, tx_buff, rx_buff, 2+cNbBytes);
	HAL_SPI_TransmitReceive(&hspi1, tx_buff, rx_buff, 2+cNbBytes, 100);
	
	// line below included in example project but not using SPI DMA so not included
  //WAIT_FOR_SPI_TC();
  
  S2LP_CS_HIGH();
  SPI_EXIT_CRITICAL();
  
  for(uint32_t i=0;i<cNbBytes;i++)
  {
    pcBuffer[i]=rx_buff[i+2];
  }
  ((uint8_t*)&status)[1]=rx_buff[0];
  ((uint8_t*)&status)[0]=rx_buff[1];
  
  
  return status;
}




// close the Doxygen group
/**
\}
*/
  
/* end of file */
