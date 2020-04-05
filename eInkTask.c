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
    //uint8_t pageNumber = 0;

    /* Configure Switch and LEDs*/
    cyhal_gpio_init( CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_PULLUP, 
                     CYBSP_BTN_OFF);
    cyhal_gpio_init( CYBSP_LED_RGB_RED, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, 
                     CYBSP_LED_STATE_OFF);
    cyhal_gpio_init( CYBSP_LED_RGB_GREEN, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, 
                     CYBSP_LED_STATE_OFF);
    
    /* Initialize EmWin driver*/
    //GUI_Init();
    
    /* Start the eInk display interface and turn on the display power */
    //Cy_EINK_Start(20);
    //Cy_EINK_Power(1);

    /* Show the startup screen */
    //ShowStartupScreen();
    //vTaskDelay(2000);

    /* Show the instructions screen */
    //ShowInstructionsScreen();
    //WaitforSwitchPressAndRelease();

    for(;;)
    {
        cyhal_gpio_write( CYBSP_LED_RGB_GREEN, CYBSP_LED_STATE_ON);

        /* Using pageNumber as index, update the display with a demo screen
            Following are the functions that are called in sequence
                ShowFontSizesNormal()
                ShowFontSizesBold()
                ShowTextModes()
                ShowTextWrapAndOrientation()
                Show2DGraphics1()
                Show2DGraphics2()
        */
        //(*demoPageArray[pageNumber])();

        vTaskDelay(500);
        cyhal_gpio_write( CYBSP_LED_RGB_GREEN, CYBSP_LED_STATE_OFF);

        /* Wait for a switch press event */
        //WaitforSwitchPressAndRelease();

        /* Cycle through demo pages */
        //pageNumber = (pageNumber+1) % NUMBER_OF_DEMO_PAGES;
        vTaskDelay(1000);
    }
}
