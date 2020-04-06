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
#include <stdio.h>
#include <stdlib.h>

#include "GUI_Paint.h"
#include "EPD_2in13bc.h"
#include "ImageData.h"

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

    printf("EPD_2IN13BC_test Demo\r\n");
    EPD_2IN13BC_DriverInit();
    printf("e-Paper Init and Clear...\r\n");
    EPD_2IN13BC_Init();
    EPD_2IN13BC_Clear();
    vTaskDelay(10000);

    //Create a new image cache named IMAGE_BW and fill it with white
    uint8_t *BlackImage, *RYImage; // Red or Yellow
    uint16_t Imagesize = ((EPD_2IN13BC_WIDTH % 8 == 0)? (EPD_2IN13BC_WIDTH / 8 ): (EPD_2IN13BC_WIDTH / 8 + 1)) * EPD_2IN13BC_HEIGHT;
    if((BlackImage = (uint8_t *)malloc(Imagesize)) == NULL) {
    	printf("Failed to apply for black memory...\r\n");
    }
    if((RYImage = (uint8_t *)malloc(Imagesize)) == NULL) {
    	printf("Failed to apply for red memory...\r\n");
    }
    printf("NewImage:BlackImage and RYImage\r\n");
    Paint_NewImage(BlackImage, EPD_2IN13BC_WIDTH, EPD_2IN13BC_HEIGHT, 270, WHITE);
    Paint_NewImage(RYImage, EPD_2IN13BC_WIDTH, EPD_2IN13BC_HEIGHT, 270, WHITE);

    //Select Image
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    Paint_SelectImage(RYImage);
    Paint_Clear(WHITE);

    printf("show image for array\r\n");
    EPD_2IN13BC_Display(gImage_2in13c_b, gImage_2in13c_y);
    vTaskDelay(10000);

    /*Horizontal screen*/
    //1.Draw black image
    printf("Draw black image\r\n");
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    Paint_DrawPoint(5, 70, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
    Paint_DrawPoint(5, 80, BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
    Paint_DrawLine(20, 70, 50, 100, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(50, 70, 20, 100, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawRectangle(60, 70, 90, 100, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawCircle(125, 85, 15, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawString_EN(5, 15, "Testing black", &Font12, WHITE, BLACK);

    //2.Draw red image
    printf("Draw red image\r\n");
    Paint_SelectImage(RYImage);
    Paint_Clear(WHITE);
    Paint_DrawPoint(5, 90, RED, DOT_PIXEL_3X3, DOT_STYLE_DFT);
    Paint_DrawPoint(5, 100, RED, DOT_PIXEL_4X4, DOT_STYLE_DFT);
    Paint_DrawLine(125, 70, 125, 100, RED, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(110, 85, 140, 85, RED, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawRectangle(20, 70, 50, 100, RED, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawCircle(165, 85, 15, RED, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawString_EN(5, 0, "This is not easy", &Font12, BLACK, WHITE);
    Paint_DrawNum(5, 50, 1234567890, &Font20, WHITE, RED);

    printf("EPD_Display\r\n");
    EPD_2IN13BC_Display(BlackImage, RYImage);
    vTaskDelay(10000);

    printf("Clear...\r\n");
    EPD_2IN13BC_Clear();

    printf("Goto Sleep...\r\n");
    EPD_2IN13BC_Sleep();
    free(BlackImage);
    free(RYImage);
    BlackImage = NULL;
    RYImage = NULL;

    for(;;)
    {
        cyhal_gpio_toggle( CYBSP_LED_RGB_GREEN);
        vTaskDelay(500);
    }
}
