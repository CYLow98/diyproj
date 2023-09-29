#ifndef __BSP_I2C_H
#define __BSP_I2C_H

#include "stm32f4xx.h"                  // Device header
#include "stm32f411xe.h"                  // Device header

void bsp_i2c_init(void);
void bsp_i2c_master_transfer(uint8_t* buf,uint16_t len,uint8_t addr);

#endif
