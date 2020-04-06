/*
 * eInkGdew0213z16Driver.c
 *
 *  Created on: Apr 5, 2020
 *      Author: Hassa
 */

#include "eInkGdew0213z16Driver.h"
#include "cyhal.h"
#include "cybsp.h"
#include <stdlib.h>
#include "cy_retarget_io.h"
#include "FreeRTOS.h"
#include "task.h"

/*********************************************************************
*
*       Defines: Configuration
*
**********************************************************************
  Needs to be adapted to custom hardware.
*/
static uint32_t timerCount;



/* SPI port to communicate with the eInk display controller */
static cyhal_spi_t SPI;

void EinkGdew0213z16_DriverInit(void)
{
    cy_rslt_t result;

	/* Start the SPI master */
	result = cyhal_spi_init(&SPI, EINK_MOSI, NC, EINK_SCLK, NC, NULL, 8, CYHAL_SPI_MODE_00_MSB, false);
	if(result != CY_RSLT_SUCCESS)
	{
		printf("SPI not initialized - Code: 0x%lX (Type:0x%lX, Module=0x%lX, Code=0x%lX)\r\n", \
				result, CY_RSLT_GET_TYPE(result), CY_RSLT_GET_MODULE(result), CY_RSLT_GET_CODE(result));
	}
	else
	{
		cyhal_spi_set_frequency(&SPI, 4000000);
	}

	cyhal_gpio_init(EINK_DISPCS, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, 0);
	cyhal_gpio_init(EINK_DISPRST, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, 0);
	cyhal_gpio_init(EINK_DISPBUSY, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_NONE, 0);
	cyhal_gpio_init(EINK_DC, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, 0);

	/* Deselect the eInk Display */
	EinkGdew0213z16_DetachSPI();
}

/*******************************************************************************
* Function Name: void Cy_EINK_AttachSPI(void)
********************************************************************************
*
* Summary:
*  Attaches the SPI master to the E-INK display driver.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  None
*******************************************************************************/
void EinkGdew0213z16_AttachSPI(void)
{
    /* Make the chip select HIGH */
	EinkGdew0213z16_CsLow;
}

/*******************************************************************************
* Function Name: void Cy_EINK_DetachSPI(void)
********************************************************************************
*
* Summary:
*  Detaches the SPI master from the E-INK display driver.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  None
*******************************************************************************/
void EinkGdew0213z16_DetachSPI(void)
{
	EinkGdew0213z16_CsHigh;
}

/*******************************************************************************
* Function Name: void Cy_EINK_TimerInit(void)
********************************************************************************
*
* Summary:
*  This function clears and initializes the E-INK Timer
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  None
*******************************************************************************/
void EinkGdew0213z16_TimerInit(void)
{
	timerCount = xTaskGetTickCount();
}

/*******************************************************************************
* Function Name: uint32_t Cy_EINK_GetTimeTick(void)
********************************************************************************
*
* Summary:
*  This function returns the value of current time tick of E-INK Timer.
*
* Parameters:
*  None
*
* Return:
*  uint32_t : current value of time tick
*
* Side Effects:
*  None
*******************************************************************************/
uint32_t EinkGdew0213z16_GetTimeTick(void)
{
    /* Return the current value of time tick */
	return(xTaskGetTickCount()-timerCount);
}

/*******************************************************************************
* Function Name: void Cy_EINK_TimerStop(void)
********************************************************************************
*
* Summary:
*  Stops the E-INK Timer.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  None
*******************************************************************************/
void EinkGdew0213z16_TimerStop(void)
{

}

/*******************************************************************************
* Function Name: void Cy_EINK_WriteSPI(uint8_t data)
********************************************************************************
*
* Summary:
*  Send a byte of data to the E-INK display driver via SPI.
*
* Parameters:
*  uint8_t data : data byte that need to be transmitted
*
* Return:
*  None
*
* Side Effects:
*  None
*******************************************************************************/
void EinkGdew0213z16_WriteDataSPI(uint8_t data)
{
	/* Select the eInk Display */
	EinkGdew0213z16_AttachSPI();
	/* Setup DC pin for data */
	EinkGdew0213z16_Data;
    /* Send one byte of data */
    cyhal_spi_send(&SPI, data);
}

/*******************************************************************************
* Function Name: void Cy_EINK_WriteSPI(uint8_t data)
********************************************************************************
*
* Summary:
*  Send a byte of data to the E-INK display driver via SPI.
*
* Parameters:
*  uint8_t data : data byte that need to be transmitted
*
* Return:
*  None
*
* Side Effects:
*  None
*******************************************************************************/
void EinkGdew0213z16_WriteCommandSPI(uint8_t data)
{
	/* Select the eInk Display */
	EinkGdew0213z16_AttachSPI();
	/* Setup DC pin for command */
	EinkGdew0213z16_Command;
    /* Send one byte of data */
    cyhal_spi_send(&SPI, data);
}

/*******************************************************************************
* Function Name: Cy_EINK_ReadSPI(void)
********************************************************************************
*
* Summary:
*  Read a byte of data from the E-INK display driver via SPI.
*
* Parameters:
*  None
*
* Return:
*  uint8_t : received data
*
* Side Effects:
*  None
*******************************************************************************/
void EinkGdew0213z16_ReadDataSPI(void)
{
	CY_ASSERT(0); /// This should never happen
	/* The EinkGdew0213z16 is not readable through SPI. Using cache instead (LCDConf.c, GUIDRV_SPAGE_1C1)*/
}

/*******************************************************************************
* Function Name: void Cy_EINK_WriteSPIBuffer(uint8_t* data, uint16 dataLength)
********************************************************************************
*
* Summary:
*  Send a multiple bytes of data to the E-INK display driver via SPI.
*
* Parameters:
*  uint8_t* data : pointer to the buffer that holds the data
*  uint16 dataLength : number of bytes to be transmitted
*
* Return:
*  int : Number of bytes written to SPI
*
* Side Effects:
*  None
*******************************************************************************/
void EinkGdew0213z16_WriteDataSPIBuffer(unsigned char * pData, int numBytes)
{
	CY_ASSERT(pData);
	CY_ASSERT(numBytes>0);

	/* Select the eInk Display */
	EinkGdew0213z16_AttachSPI();
	/* Setup DC pin for data */
	EinkGdew0213z16_Data;

	uint8_t* buff = (uint8_t*)malloc(numBytes);

	/* Copy data bytes to buffer array */
	memcpy(&buff[0], pData, numBytes);

    cyhal_spi_transfer(&SPI, buff, numBytes, NULL, 0, 0);
    free(buff);
}

/*******************************************************************************
* Function Name: void Cy_EINK_WriteSPIBuffer(uint8_t* data, uint16 dataLength)
********************************************************************************
*
* Summary:
*  Send a multiple bytes of data to the E-INK display driver via SPI.
*
* Parameters:
*  uint8_t* data : pointer to the buffer that holds the data
*  uint16 dataLength : number of bytes to be transmitted
*
* Return:
*  int : Number of bytes written to SPI
*
* Side Effects:
*  None
*******************************************************************************/
void EinkGdew0213z16_WriteCommandSPIBuffer(unsigned char * pData, int numBytes)
{
	CY_ASSERT(pData);
	CY_ASSERT(numBytes>0);

	/* Select the eInk Display */
	EinkGdew0213z16_AttachSPI();
	/* Setup DC pin for data */
	EinkGdew0213z16_Data;

	uint8_t* buff = (uint8_t*)malloc(numBytes);

	/* Copy data bytes to buffer array */
	memcpy(&buff[0], pData, numBytes);

    cyhal_spi_transfer(&SPI, buff, numBytes, NULL, 0, 0);
    free(buff);
}

/*******************************************************************************
* Function Name: bool CY_EINK_IsBusy(void)
********************************************************************************
*
* Summary:
*  Check if the E-INK display is busy.
*
* Parameters:
*  None
*
* Return:
*  bool : True if the E-INK display is buy, False otherwise
*
* Side Effects:
*  None
*******************************************************************************/
bool EinkGdew0213z16_IsBusy(void)
{
    /* Return the status of  pin */
    return cyhal_gpio_read(EINK_DISPBUSY);
}

/*************************** End of file ****************************/
