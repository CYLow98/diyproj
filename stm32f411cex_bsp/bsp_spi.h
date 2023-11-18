#ifndef __BSP_SPI_H
#define __BSP_SPI_H

/******************************** include files ********************************/#include "stm32f4xx.h"                  // Device header
#include "stdint.h"                  // Device header
/*******************************************************************************/

void bsp_spi_init();

//#Chip select will be handled by peripheral drivers
void bsp_spi_transmit_dma(const uint8_t *txBuf,uint16_t length);

void bsp_spi_transmit_receive_dma(const uint8_t *txBuf,uint8_t * rxBuf, uint16_t length);

void bsp_spi_register_transfer_complete_callback(void (*cb)(void));

void bsp_spi_register_transfer_error_callback(void (*cb)(void));

#endif