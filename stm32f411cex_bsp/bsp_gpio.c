
/******************************** includes ********************************/
#include "stm32f4xx.h"                  // Device header
#include "stm32f411xe.h"                  // Device header
#include "bsp_gpio.h"
/*************************************************************************/

/****************************LED(PC13)***************************/
//PC13
void bsp_gpio_LED_init(void){
	//RM (Section 8.3 & 8.4)
	//Set up 
	//1) RCC_AHB1ENR
	SET_BIT(RCC->AHB1ENR,RCC_AHB1ENR_GPIOCEN);
	//2) GPIOC_MODER bit 13 Output
	MODIFY_REG(GPIOC->MODER,GPIO_MODER_MODE13_Msk,GPIO_MODER_OUTPUT<<GPIO_MODER_MODE13_Pos);
	//3) GPIOC_OTYPER bit 13 Push-pull
	MODIFY_REG(GPIOC->OTYPER,GPIO_OTYPER_OT13_Msk,GPIO_OTYPER_PP<<GPIO_OTYPER_OT13_Pos);
	//4) GPIOC_OSPEEDR bit 13 Low speed
	MODIFY_REG(GPIOC->OSPEEDR,GPIO_OSPEEDR_OSPEED13_Msk,GPIO_OSPEEDR_LS<<GPIO_OSPEEDR_OSPEED13_Pos);
	//5) GPIOC_PUPDR bit 13 None
	MODIFY_REG(GPIOC->PUPDR,GPIO_PUPDR_PUPD13_Msk,GPIO_PUPDR_NONE<<GPIO_PUPDR_PUPD13_Pos);
	
} 
void bsp_gpio_LED_on(void){
	//RM (Section 8.3.5)
	//Since it is atomic write operation, there is no need to disable interrupt to protect against race condition
	SET_BIT(GPIOC->BSRR,GPIO_BSRR_BR13); //Set the 13th pin on the GPIOC_BSRR reg //Since GPIO is acting as sink, going low will turn LED on
}

void bsp_gpio_LED_off(void){
	//RM (Section 8.3.5)
	//Since it is atomic write operation, there is no need to disable interrupt to protect against race condition
	SET_BIT(GPIOC->BSRR,GPIO_BSRR_BS13); //set the 29th pin on the GPIOC_BSRR reg //Since GPIO is acting as sink, going high will turn LED off
}
/****************************User Key(PA0)***************************/
void bsp_gpio_user_key_init(void){
	
	//Pin as GPIO input
	//1) RCC_AHB1ENR
	SET_BIT(RCC->AHB1ENR,RCC_AHB1ENR_GPIOAEN);
	//2) GPIOA_MODER bit 0 Input
	MODIFY_REG(GPIOA->MODER,GPIO_MODER_MODE0_Msk,GPIO_MODER_INPUT<<GPIO_MODER_MODE0_Pos);
	//3) GPIOA_PUPDR bit 0 PU
	MODIFY_REG(GPIOA->PUPDR,GPIO_PUPDR_PUPD0_Msk,GPIO_PUPDR_PU<<GPIO_PUPDR_PUPD0_Pos);
}

void bsp_gpio_user_key_it_init(void){
	//Interrupt
	//RM (Section 6.3.12, 7.2 and 10.3)
	//Set up
	//1) RCC_APB2ENR enable SYSCFG
	SET_BIT(RCC->APB2ENR,RCC_APB2ENR_SYSCFGEN);
	//2) SYSCFG_EXTICR1 EXTI0[3:0] 
	SET_BIT(SYSCFG->EXTICR[0],SYSCFG_EXTICR1_EXTI0_PA);
	//3) EXTI_IMR bit 0 set not masked
	SET_BIT(EXTI->IMR,EXTI_IMR_IM0);
	//4) EXTI_FTSR bit 0 enable
	SET_BIT(EXTI->FTSR,EXTI_FTSR_TR0);
	
	//PM (Section 4.3.1)
	//5) NVIC_SetPriority
	NVIC_SetPriority(EXTI0_IRQn,4u);
	//6) NVIC_EnableIRQ
	NVIC_EnableIRQ(EXTI0_IRQn);
}

//1 or 0
int bsp_gpio_user_key_get_state(){
	return (READ_BIT(GPIOA->IDR,GPIO_IDR_ID0_Msk)>>GPIO_IDR_ID0_Pos);
}
/********************************************************************/

/****************************GPIO Generic(PA7)***************************/
void bsp_gpio_PA7_init(void){
	SET_BIT(RCC->AHB1ENR,RCC_AHB1ENR_GPIOAEN);
	MODIFY_REG(GPIOA->MODER,GPIO_MODER_MODE7_Msk,GPIO_MODER_OUTPUT<<GPIO_MODER_MODE7_Pos);
	MODIFY_REG(GPIOA->OTYPER,GPIO_OTYPER_OT7_Msk,GPIO_OTYPER_PP<<GPIO_OTYPER_OT7_Pos);
	MODIFY_REG(GPIOA->OSPEEDR,GPIO_OSPEEDR_OSPEED7_Msk,GPIO_OSPEEDR_LS<<GPIO_OSPEEDR_OSPEED7_Pos);
	MODIFY_REG(GPIOA->PUPDR,GPIO_PUPDR_PUPD7_Msk,GPIO_PUPDR_NONE<<GPIO_PUPDR_PUPD7_Pos);
}

void bsp_gpio_PA7_set(void){
	SET_BIT(GPIOA->BSRR,GPIO_BSRR_BS7);
}

void bsp_gpio_PA7_reset(void){
	SET_BIT(GPIOA->BSRR,GPIO_BSRR_BR7);
}
/********************************************************************/

/****************************GPIO Generic(PA6)***************************/
void bsp_gpio_PA6_init(void){
	SET_BIT(RCC->AHB1ENR,RCC_AHB1ENR_GPIOAEN);
	MODIFY_REG(GPIOA->MODER,GPIO_MODER_MODE6_Msk,GPIO_MODER_OUTPUT<<GPIO_MODER_MODE6_Pos);
	MODIFY_REG(GPIOA->OTYPER,GPIO_OTYPER_OT6_Msk,GPIO_OTYPER_PP<<GPIO_OTYPER_OT6_Pos);
	MODIFY_REG(GPIOA->OSPEEDR,GPIO_OSPEEDR_OSPEED6_Msk,GPIO_OSPEEDR_LS<<GPIO_OSPEEDR_OSPEED6_Pos);
	MODIFY_REG(GPIOA->PUPDR,GPIO_PUPDR_PUPD6_Msk,GPIO_PUPDR_NONE<<GPIO_PUPDR_PUPD6_Pos);
}

void bsp_gpio_PA6_set(void){
	SET_BIT(GPIOA->BSRR,GPIO_BSRR_BS6);
}

void bsp_gpio_PA6_reset(void){
	SET_BIT(GPIOA->BSRR,GPIO_BSRR_BR6);
}
/********************************************************************/