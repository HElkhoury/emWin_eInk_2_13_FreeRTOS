/*****************************************************************************
* | File      	:   EPD_2in13bc.c
* | Author      :   Waveshare team
* | Function    :   2.13inch e-paper b&c
* | Info        :
*----------------
* |	This version:   V3.0
* | Date        :   2019-06-13
* | Info        :
* -----------------------------------------------------------------------------
* V3.0(2019-06-13):
* 1.Change:
*    EPD_Reset() => EPD_2IN13BC_Reset()
*    EPD_SendCommand() => EPD_2IN13BC_SendCommand()
*    EPD_SendData() => EPD_2IN13BC_SendData()
*    EPD_WaitUntilIdle() => EPD_2IN13BC_ReadBusy()
*    EPD_Init() => EPD_2IN13BC_Init()
*    EPD_Clear() => EPD_2IN13BC_Clear()
*    EPD_Display() => EPD_2IN13BC_Display()
*    EPD_Sleep() => EPD_2IN13BC_Sleep()
* 2.remove commands define:
*    #define PANEL_SETTING                               0x00
*    #define POWER_SETTING                               0x01
*    #define POWER_OFF                                   0x02
*    #define POWER_OFF_SEQUENCE_SETTING                  0x03
*    #define POWER_ON                                    0x04
*    #define POWER_ON_MEASURE                            0x05
*    #define BOOSTER_SOFT_START                          0x06
*    #define DEEP_SLEEP                                  0x07
*    #define DATA_START_TRANSMISSION_1                   0x10
*    #define DATA_STOP                                   0x11
*    #define DISPLAY_REFRESH                             0x12
*    #define DATA_START_TRANSMISSION_2                   0x13
*    #define VCOM_LUT                                    0x20
*    #define W2W_LUT                                     0x21
*    #define B2W_LUT                                     0x22
*    #define W2B_LUT                                     0x23
*    #define B2B_LUT                                     0x24
*    #define PLL_CONTROL                                 0x30
*    #define TEMPERATURE_SENSOR_CALIBRATION              0x40
*    #define TEMPERATURE_SENSOR_SELECTION                0x41
*    #define TEMPERATURE_SENSOR_WRITE                    0x42
*    #define TEMPERATURE_SENSOR_READ                     0x43
*    #define VCOM_AND_DATA_INTERVAL_SETTING              0x50
*    #define LOW_POWER_DETECTION                         0x51
*    #define TCON_SETTING                                0x60
*    #define RESOLUTION_SETTING                          0x61
*    #define GET_STATUS                                  0x71
*    #define AUTO_MEASURE_VCOM                           0x80
*    #define READ_VCOM_VALUE                             0x81
*    #define VCM_DC_SETTING                              0x82
*    #define PARTIAL_WINDOW                              0x90
*    #define PARTIAL_IN                                  0x91
*    #define PARTIAL_OUT                                 0x92
*    #define PROGRAM_MODE                                0xA0
*    #define ACTIVE_PROGRAM                              0xA1
*    #define READ_OTP_DATA                               0xA2
*    #define POWER_SAVING                                0xE3
* V2.0(2018-11-13):
* 1.Remove:ImageBuff[EPD_HEIGHT * EPD_WIDTH / 8]
* 2.Change:EPD_Display(UBYTE *Image)
*   Need to pass parameters: pointer to cached data
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "cyhal.h"
#include "cybsp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cy_retarget_io.h"

#include "EPD_2in13bc.h"

/* SPI port to communicate with the eInk display controller */
static cyhal_spi_t SPI;

void EPD_2IN13BC_DriverInit(void)
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
	EPD_2IN13BC_CsHigh;
}

/******************************************************************************
function :	Software reset
parameter:
******************************************************************************/
static void EPD_2IN13BC_Reset(void)
{
	if(cyhal_gpio_read(EINK_DISPRST))
	{
		/* Reset the Driver IC */
		EPD_2IN13BC_RstActive;
		vTaskDelay(200);
		EPD_2IN13BC_RstInactive;
		vTaskDelay(200);
	}
	else
	{
		EPD_2IN13BC_RstInactive;
		vTaskDelay(200);
	}
}

/******************************************************************************
function :	send command
parameter:
     Reg : Command register
******************************************************************************/
static void EPD_2IN13BC_SendCommand(uint8_t Reg)
{
	/* Setup DC pin for command */
	EPD_2IN13BC_Command;
	/* Select the eInk Display */
	EPD_2IN13BC_CsLow;
	/* Send one byte of data */
	cyhal_spi_send(&SPI, Reg);
	/* Deselect the eInk Display */
	EPD_2IN13BC_CsHigh;
}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
******************************************************************************/
static void EPD_2IN13BC_SendData(uint8_t Data)
{
	/* Setup DC pin for command */
	EPD_2IN13BC_Data;
	/* Select the eInk Display */
	EPD_2IN13BC_CsLow;
	/* Send one byte of data */
	cyhal_spi_send(&SPI, Data);
	/* Deselect the eInk Display */
	EPD_2IN13BC_CsHigh;
}

/******************************************************************************
function :	Wait until the busy_pin goes LOW
parameter:
******************************************************************************/
void EPD_2IN13BC_ReadBusy(void)
{
    printf("e-Paper busy\r\n");
    while(cyhal_gpio_read(EINK_DISPBUSY))
    {
        vTaskDelay(100);
    }
    printf("e-Paper busy release\r\n");
}

/******************************************************************************
function :	Turn On Display
parameter:
******************************************************************************/
static void EPD_2IN13BC_TurnOnDisplay(void)
{
    EPD_2IN13BC_SendCommand(0x12);		 //DISPLAY REFRESH
    vTaskDelay(10);

    EPD_2IN13BC_ReadBusy();
}

/******************************************************************************
function :	Initialize the e-Paper register
parameter:
******************************************************************************/
void EPD_2IN13BC_Init(void)
{
    EPD_2IN13BC_Reset();

    EPD_2IN13BC_SendCommand(0x06); // BOOSTER_SOFT_START
    EPD_2IN13BC_SendData(0x17);
    EPD_2IN13BC_SendData(0x17);
    EPD_2IN13BC_SendData(0x17);

    EPD_2IN13BC_SendCommand(0x04); // POWER_ON
    EPD_2IN13BC_ReadBusy();

    EPD_2IN13BC_SendCommand(0x00); // PANEL_SETTING
    EPD_2IN13BC_SendData(0x8F);

    EPD_2IN13BC_SendCommand(0x50); // VCOM_AND_DATA_INTERVAL_SETTING
    EPD_2IN13BC_SendData(0xF0);
    EPD_2IN13BC_SendCommand(0x61); // RESOLUTION_SETTING
    EPD_2IN13BC_SendData(EPD_2IN13BC_WIDTH); // width: 104
    EPD_2IN13BC_SendData(EPD_2IN13BC_HEIGHT >> 8); // height: 212
    EPD_2IN13BC_SendData(EPD_2IN13BC_HEIGHT & 0xFF);
}

/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void EPD_2IN13BC_Clear(void)
{
    uint16_t Width = (EPD_2IN13BC_WIDTH % 8 == 0)? (EPD_2IN13BC_WIDTH / 8 ): (EPD_2IN13BC_WIDTH / 8 + 1);
    uint16_t Height = EPD_2IN13BC_HEIGHT;

    //send black data
    EPD_2IN13BC_SendCommand(0x10);
    for (uint16_t j = 0; j < Height; j++) {
        for (uint16_t i = 0; i < Width; i++) {
            EPD_2IN13BC_SendData(0xFF);
        }
    }
    EPD_2IN13BC_SendCommand(0x92);

    //send red data
    EPD_2IN13BC_SendCommand(0x13);
    for (uint16_t j = 0; j < Height; j++) {
        for (uint16_t i = 0; i < Width; i++) {
            EPD_2IN13BC_SendData(0xFF);
        }
    }
    EPD_2IN13BC_SendCommand(0x92);

    EPD_2IN13BC_TurnOnDisplay();
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays
parameter:
******************************************************************************/
void EPD_2IN13BC_Display(const uint8_t *blackimage, const uint8_t *ryimage)
{
	uint16_t Width, Height;
    Width = (EPD_2IN13BC_WIDTH % 8 == 0)? (EPD_2IN13BC_WIDTH / 8 ): (EPD_2IN13BC_WIDTH / 8 + 1);
    Height = EPD_2IN13BC_HEIGHT;

    EPD_2IN13BC_SendCommand(0x10);
    for (uint16_t j = 0; j < Height; j++) {
        for (uint16_t i = 0; i < Width; i++) {
            EPD_2IN13BC_SendData(blackimage[i + j * Width]);
        }
    }
    EPD_2IN13BC_SendCommand(0x92);

    EPD_2IN13BC_SendCommand(0x13);
    for (uint16_t j = 0; j < Height; j++) {
        for (uint16_t i = 0; i < Width; i++) {
            EPD_2IN13BC_SendData(ryimage[i + j * Width]);
        }
    }
    EPD_2IN13BC_SendCommand(0x92);

    EPD_2IN13BC_TurnOnDisplay();
}

/******************************************************************************
function :	Enter sleep mode
parameter:
******************************************************************************/
void EPD_2IN13BC_Sleep(void)
{
    EPD_2IN13BC_SendCommand(0x02); // POWER_OFF
    EPD_2IN13BC_ReadBusy();
    EPD_2IN13BC_SendCommand(0x07); // DEEP_SLEEP
    EPD_2IN13BC_SendData(0xA5); // check code

    /* Reset the Driver IC */
    EPD_2IN13BC_RstActive;
}
