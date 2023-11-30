#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H

/******************************** include files ********************************/
#include "stm32f4xx.h"                  // Device header
#include "stm32f411xe.h"                  // Device header
/*********************************** CONSTANTS ************************************/
//These are specific to stm32
//RM (Section 8.4.1) include in header files so can be used in other type of bsp ie i2c,spi,uart
#define GPIO_MODER_INPUT 	0x0u 
#define GPIO_MODER_OUTPUT 0x1u
#define GPIO_MODER_AF 		0x2u
#define GPIO_MODER_ANALOG 0x3u

//RM (Section 8.4.2)
#define GPIO_OTYPER_PP 0x0u
#define GPIO_OTYPER_OD 0x1u

//RM (Section 8.4.3)
#define GPIO_OSPEEDR_LS 0x0u
#define GPIO_OSPEEDR_MS 0x1u
#define GPIO_OSPEEDR_FS 0x2u
#define GPIO_OSPEEDR_HS 0x3u

//RM (Section 8.4.4)
#define GPIO_PUPDR_NONE 0x0u
#define GPIO_PUPDR_PU 0x1u
#define GPIO_PUPDR_PD 0x2u

//RM (Section 8.4.9)
#define GPIO_AFRX_AF0  0x0u //System Function
#define GPIO_AFRX_AF1  0x1u
#define GPIO_AFRX_AF2  0x2u
#define GPIO_AFRX_AF3  0x3u
#define GPIO_AFRX_AF4  0x4u
#define GPIO_AFRX_AF5  0x5u
#define GPIO_AFRX_AF6  0x6u
#define GPIO_AFRX_AF7  0x7u
#define GPIO_AFRX_AF8  0x8u
#define GPIO_AFRX_AF9  0x9u
#define GPIO_AFRX_AF10 0xAu
#define GPIO_AFRX_AF11 0xBu
#define GPIO_AFRX_AF12 0xCu
#define GPIO_AFRX_AF13 0xDu
#define GPIO_AFRX_AF14 0xEu
#define GPIO_AFRX_AF15 0xFu //EVENTOUT (FPU)

/**********************************************************************************/

void bsp_gpio_LED_init(void); //PC13
void bsp_gpio_LED_on(void);
void bsp_gpio_LED_off(void);

void bsp_gpio_user_key_init(void);
void bsp_gpio_user_key_it_init(void);
uint8_t bsp_gpio_user_key_get_state(void);

void bsp_gpio_PA7_init(void);
void bsp_gpio_PA7_set(void);
void bsp_gpio_PA7_reset(void);

void bsp_gpio_PA6_init(void);
void bsp_gpio_PA6_set(void);
void bsp_gpio_PA6_reset(void);
uint8_t bsp_gpio_PA6_get_state(void);

void bsp_gpio_PB0_init(void);
void bsp_gpio_PB0_set(void);
void bsp_gpio_PB0_reset(void);
uint8_t bsp_gpio_PB0_get_state(void);

void bsp_gpio_PB1_init(void);
void bsp_gpio_PB1_set(void);
void bsp_gpio_PB1_reset(void);
uint8_t bsp_gpio_PB1_get_state(void);

void bsp_gpio_PB10_init(void);
void bsp_gpio_PB10_set(void);
void bsp_gpio_PB10_reset(void);

#endif
