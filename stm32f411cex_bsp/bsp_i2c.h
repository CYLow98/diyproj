#ifndef __BSP_I2C_H
#define __BSP_I2C_H

/******************************** include files ********************************/
#include "stm32f4xx.h"                  // Device header
#include "stm32f411xe.h"                  // Device header
#include "bsp_gpio.h"
/*******************************************************************************/

void bsp_i2c_init(void);
void bsp_i2c_master_transfer(const uint8_t* buf,uint16_t len,uint8_t addr);
//for sequence start bit -> address(r) -> ack from slave -> data from slave -> ack from master --> repeat for len-1 --> stop
void bsp_i2c_master_receive(uint8_t* buf,uint16_t len,uint8_t addr);
//for sequence start bit -> address(w) -> ack from slave -> reg -> ack from slave --> ...repeat for reglen-1 --> restart -> address(r) -> data from slave -> ack from master --> repeat for len-1 --> stop
void bsp_i2c_master_receive_reg(uint8_t* buf,uint16_t len,uint8_t addr,uint8_t* regs,uint16_t reglen);

#endif
