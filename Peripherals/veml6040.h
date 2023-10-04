#ifndef __VEML6040_H
#define __VEML6040_H

/******************************** includes ********************************/
#include "bsp_i2c.h"                  // i2c utilities
/**************************************************************************/

typedef struct{
	volatile uint16_t RCount;
	volatile uint16_t GCount;
	volatile uint16_t BCount;
	volatile uint16_t WCount;
}VEML6040RawData_TypeDef;

typedef struct{
	volatile uint16_t RLux;
	volatile uint16_t GLux;
	volatile uint16_t BLux;
	volatile uint16_t WLux;
}VEML6040LuxData_TypeDef;

void veml6040_init(void);
void veml6040_get_data_raw(volatile VEML6040RawData_TypeDef *datastruct);
#endif