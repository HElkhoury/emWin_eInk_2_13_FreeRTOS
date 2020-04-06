/*
 * eInkGdew0213z16Driver.h
 *
 *  Created on: Apr 5, 2020
 *      Author: Hassa
 */

#ifndef EINKGDEW0213Z16DRIVER_H
#define EINKGDEW0213Z16DRIVER_H

#include "cyhal.h"
#include "cybsp.h"

#define GxGDEW0213Z16_WIDTH  104
#define GxGDEW0213Z16_HEIGHT 212

#define GxGDEW0213Z16_BUFFER_SIZE ((uint32_t)(GxGDEW0213Z16_WIDTH) * (uint32_t)(GxGDEW0213Z16_HEIGHT) / 8)

#define EINK_MOSI       CYBSP_D11
#define EINK_MISO       CYBSP_D12
#define EINK_SCLK       CYBSP_D13
#define EINK_DISPCS     CYBSP_D10   /* SPI CS output pin */
#define EINK_DISPRST    CYBSP_D2    /* Display reset output pin */
#define EINK_DISPBUSY   CYBSP_D3    /* Display busy input pin */
#define EINK_DC			CYBSP_D5    /* Display Command/Data output pin */


/* Definitions of pin sates */
#define EINK_PIN_LOW        (uint8_t)(0x00u)
#define EINK_PIN_HIGH       (uint8_t)(0x01u)

/* Push the chip select pin to logic HIGH */
#define EinkGdew0213z16_CsHigh         cyhal_gpio_write(EINK_DISPCS, EINK_PIN_HIGH)

/* Pull the chip select pin to logic LOW */
#define EinkGdew0213z16_CsLow          cyhal_gpio_write(EINK_DISPCS, EINK_PIN_LOW)

/* Push the reset pin to logic HIGH */
#define EinkGdew0213z16_RstInactive    cyhal_gpio_write(EINK_DISPRST, EINK_PIN_HIGH)

/* Pull the reset pin to logic LOW */
#define EinkGdew0213z16_RstActive      cyhal_gpio_write(EINK_DISPRST, EINK_PIN_LOW)

/* Push the DC pin to logic HIGH for Data */
#define EinkGdew0213z16_Data  			cyhal_gpio_write(EINK_DC, EINK_PIN_HIGH)

/* Pull the DC pin to logic LOW for Command */
#define EinkGdew0213z16_Command   		cyhal_gpio_write(EINK_DC, EINK_PIN_LOW)

uint8_t _black_buffer[GxGDEW0213Z16_BUFFER_SIZE];
uint8_t _red_buffer[GxGDEW0213Z16_BUFFER_SIZE];

/* Functions used for E-INK driver communication */
void EinkGdew0213z16_DriverInit(void);
void EinkGdew0213z16_AttachSPI(void);
void EinkGdew0213z16_DetachSPI(void);
void EinkGdew0213z16_WriteDataSPI(uint8_t data);
void EinkGdew0213z16_WriteCommandSPI(uint8_t data);
void EinkGdew0213z16_ReadDataSPI(void);
void EinkGdew0213z16_WriteDataSPIBuffer(unsigned char * pData, int numBytes);
void EinkGdew0213z16_WriteCommandSPIBuffer(unsigned char * pData, int numBytes);
bool EinkGdew0213z16_IsBusy(void);

#endif /* CONFIG_EINKGDEW0213Z16DRIVER_H */
