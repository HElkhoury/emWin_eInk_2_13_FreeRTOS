/******************************************************************************
* 
* File Name: eInkTask.c
*
*******************************************************************************
* (c) 2019-2020, Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************
* This software, including source code, documentation and related materials
* ("Software"), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries ("Cypress") and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software ("EULA").
*
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress's integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer of such
* system or application assumes all risk of such use and in doing so agrees to
* indemnify Cypress against all liability.
*******************************************************************************/
/******************************************************************************
* This file contains the code of E-Ink that demonstrates controlling a EInk
* display using the EmWin Graphics Library. The project displays a start up
* screen with Cypress logo and text "CYPRESS EMWIN GRAPHICS DEMO EINK DISPLAY".
* The project then displays the following screens in a loop
*
*   1. A screen showing various text alignments, styles and modes
*   2. A screen showing normal fonts
*   3. A screen showing bold fonts
*   4. A screen showing 2D graphics with horizontal lines, vertical lines
*       arcs and filled rounded rectangle
*   5. A screen showing 2D graphics with concentric circles and ellipses
*   6. A screen showing a text box with wrapped text
*
 *******************************************************************************/
#include "cyhal.h"
#include "cybsp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cy_retarget_io.h"

#include "eInkGdew0213z16Driver.h"

#define GxEPD_WHITE 0x00
#define GxEPD_BLACK 0x01
#define GxEPD_RED   0x02
#define GxEPD_RED   0x02

void fillScreen(uint16_t color)
{
	uint8_t black = 0x00;
	uint8_t red = 0x00;
	if (color == GxEPD_WHITE){}
	else if (color == GxEPD_BLACK) black = 0xFF;
	else if (color == GxEPD_RED) red = 0xFF;
	else if ((color & 0xF100) > (0xF100 / 2))  red = 0xFF;
	else if ((((color & 0xF100) >> 11) + ((color & 0x07E0) >> 5) + (color & 0x001F)) < 3 * 255 / 2) black = 0xFF;
	for (uint16_t x = 0; x < sizeof(_black_buffer); x++)
	{
		_black_buffer[x] = black;
		_red_buffer[x] = red;
	}
}

void update(void)
{
	EinkGdew0213z16_WriteCommandSPI(0x10);
	for (uint32_t i = 0; i < GxGDEW0213Z16_BUFFER_SIZE; i++)
	{
		EinkGdew0213z16_WriteDataSPI((i < sizeof(_black_buffer)) ? ~_black_buffer[i] : 0xFF);
	}
	EinkGdew0213z16_WriteCommandSPI(0x13);
	for (uint32_t i = 0; i < GxGDEW0213Z16_BUFFER_SIZE; i++)
	{
		EinkGdew0213z16_WriteDataSPI((i < sizeof(_red_buffer)) ? ~_red_buffer[i] : 0xFF);
	}
	EinkGdew0213z16_WriteCommandSPI(0x12); //display refresh
	while(EinkGdew0213z16_IsBusy());
}

void sleep(void)
{
	EinkGdew0213z16_WriteCommandSPI(0x02);      //power off
	while(EinkGdew0213z16_IsBusy());
	EinkGdew0213z16_WriteCommandSPI(0x07);     //deep sleep
	EinkGdew0213z16_WriteDataSPI(0xA5);
}

/*******************************************************************************
* Function Name: void eInkTask(void *arg)
********************************************************************************
*
* Summary: Following functions are performed
*           1. Initialize the EmWin library
*           2. Display the startup screen for 2 seconds
*           3. Display the instruction screen and wait for key press and release
*           4. Inside a while loop scroll through the 6 demo pages on every
*               key press and release
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void eInkTask(void *arg)
{
    /* Configure Switch and LEDs*/
    cyhal_gpio_init( CYBSP_LED_RGB_RED, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, 
                     CYBSP_LED_STATE_OFF);
    cyhal_gpio_init( CYBSP_LED_RGB_GREEN, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, 
                     CYBSP_LED_STATE_OFF);

    EinkGdew0213z16_DriverInit();

    if(cyhal_gpio_read(EINK_DISPRST))
    {
    	/* Reset the Driver IC */
    	EinkGdew0213z16_RstActive;
    	vTaskDelay(1);
    	EinkGdew0213z16_RstInactive;
    	vTaskDelay(1);
    }
    else
    {
    	EinkGdew0213z16_RstInactive;
    	vTaskDelay(1);
    }

	/* Configure booster Soft Start */
	EinkGdew0213z16_WriteCommandSPI(0x06);
	EinkGdew0213z16_WriteDataSPI(0x17);
	EinkGdew0213z16_WriteDataSPI(0x17);
	EinkGdew0213z16_WriteDataSPI(0x17);
	/* Power On */
	EinkGdew0213z16_WriteCommandSPI(0x04);
	/* Check busy signal */
	while(EinkGdew0213z16_IsBusy());
	/* Panel setting */
	EinkGdew0213z16_WriteCommandSPI(0x00);
	EinkGdew0213z16_WriteDataSPI(0x0F);
	/* Resolution setting */
	EinkGdew0213z16_WriteCommandSPI(0x61);
	EinkGdew0213z16_WriteDataSPI(0x68);
	EinkGdew0213z16_WriteDataSPI(0x00);
	EinkGdew0213z16_WriteDataSPI(0xD4);
	/* Vcom and data interval setting */
	EinkGdew0213z16_WriteCommandSPI(0x50);
	EinkGdew0213z16_WriteDataSPI(0x37);

	fillScreen(GxEPD_RED);

	update();

	//sleep();

    for(;;)
    {
        cyhal_gpio_toggle( CYBSP_LED_RGB_GREEN);
        vTaskDelay(500);
    }
}
