/******************************** include files ********************************/
#include "bsp_system.h"
/*******************************************************************************/

/******************************** Global Variables ********************************/
volatile uint32_t tmpreg = 0x00U;
/**********************************************************************************/

/******************************** Constants ********************************/
//Oscillator
#define HSI_VALUE 16000000 //16MHz
#define HSE_VALUE 25000000 //16MHz

//RCC parameters
//RM (Section 6.3)
#define RCC_PLLCFGR_PLLQ_VAL 0x4u //0100: PLLQ = 4
#define RCC_PLLCFGR_PLLP_VAL 0x0u //00: PLLP = 2
#define RCC_PLLCFGR_PLLN_VAL 0xC0u //011000000 (0xC0): PLLN = 192
#define RCC_PLLCFGR_PLLM_VAL 0x19u //011001 (0x19) : PLLM = 25

#define RCC_CFGR_MCO2_VAL 0x0u // 00: System clock (SYSCLK) selected
#define RCC_CFGR_MCO2PRE_VAL 0x0u // 0xx: no division
#define RCC_CFGR_MCO1PRE_VAL 0x0u // 0xx: no division
#define RCC_CFGR_I2SSRC_VAL 0x0u // 0: PLLI2S clock used as I2S clock source
#define RCC_CFGR_MCO1_VAL 0x0u // 00: HSI clock selected
#define RCC_CFGR_RTCPRE_VAL 0x2u // 00010: HSE/2
#define RCC_CFGR_PPRE2_VAL 0x0u // 0xx: AHB clock not divided
#define RCC_CFGR_PPRE1_VAL 0x4u // 100: AHB clock divided by 2
#define RCC_CFGR_HPRE_VAL 0x0u // 0xxx: system clock not divided
#define RCC_CFGR_SW_VAL RCC_CFGR_SW_PLL // 10: PLL selected as system clock
#define RCC_CFGR_SWS_VAL RCC_CFGR_SWS_PLL // 10: PLL selected as system clock

//SysTick parameters
//RM (Chapter 10.1.2)
#define SYSTICK_FREQ 1000u //1kHz, happens every 1ms
/***************************************************************************/

/******************************** MACROS ********************************/
//RM (pg 108)
#define UNUSED(X) (void)X      /* To avoid gcc/g++ warnings */
#define __GET_SYSCLK_SOURCE() (RCC->CFGR & RCC_CFGR_SWS)
#define _MODIFY_REG(REG, CLEARMASK, SETMASK) do{ \
	MODIFY_REG(REG,CLEARMASK,SETMASK); \
	tmpreg = READ_BIT(REG,SETMASK); \
}while(0u)

#define _SET_BIT(REG, BIT) do{ \
	SET_BIT(REG,BIT); \
	tmpreg = READ_BIT(REG,BIT); \
}while(0u)

#define _CLEAR_BIT(REG, BIT) do{ \
	CLEAR_BIT(REG,BIT); \
	tmpreg = READ_BIT(REG,BIT); \
}while(0u)
/************************************************************************/

/******************************** Functions Declarations ********************************/
void bsp_flash_init(void);
static void bsp_rcc_clock_init(void);
static void bsp_systick_init(void);

/******************************** Functions Definitions ********************************/
//RM (Section 3 pg 45)
void bsp_system_init(void){
	//Init flash
	bsp_flash_init();
	
	//Init clocks
	bsp_rcc_clock_init();
	
	//Init systick
	bsp_systick_init();
}

static void bsp_systick_init(void){	
	//PM (Section 4.5)
	SysTick->LOAD = (SystemCoreClock/SYSTICK_FREQ-1); //Might need to by 8 to fit the register (LOAD VAL < 16MHz(max))
  SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
  _SET_BIT(SysTick->CTRL,
	SysTick_CTRL_CLKSOURCE_Msk |
  SysTick_CTRL_TICKINT_Msk   |
  SysTick_CTRL_ENABLE_Msk);                         /* Enable SysTick IRQ and SysTick Timer */
	//_CLEAR_BIT(SysTick->CTRL,
  //SysTick_CTRL_CLKSOURCE_Msk);                         /* Set source clock to Sysclk/8 */
}

//to test, start the program as usual and check if it reaches main, then go into reset
//verify RCC_CR, RCC_CFGR, RCC_PLLCFGR, RCC_CIR
static void bsp_rcc_clock_init(void){
	
	//Set up power and voltage regulator scaling
	
	//RM (pg 119)
	_SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN);
	
	//RM (pg 86)
	_MODIFY_REG(PWR->CR, PWR_CR_VOS, PWR_CR_VOS);
	
	//RM (Section 6)
	//activate and check HSE is ready (RCC_CR)
	_SET_BIT(RCC->CR, RCC_CR_HSEON);
	while(!(READ_BIT(RCC->CR,RCC_CR_HSERDY))){} //block until bit HSERDY in RCC_CR on
	
	//Disable PLL, PLLI2SON (RCC_CR)
	_CLEAR_BIT(RCC->CR, RCC_CR_PLLON | RCC_CR_PLLI2SON);
	while(READ_BIT(RCC->CR,RCC_CR_PLLON|RCC_CR_PLLI2SON)){} //block until bit PLLON and PLLI2SON in RCC_CR off
	
	//Configurate RCC_PLLCFGR //Use HSE
	_MODIFY_REG(RCC->PLLCFGR,
	RCC_PLLCFGR_PLLQ_Msk | 
	RCC_PLLCFGR_PLLSRC_Msk |
	RCC_PLLCFGR_PLLP_Msk |
	RCC_PLLCFGR_PLLN_Msk |
	RCC_PLLCFGR_PLLM_Msk,
	RCC_PLLCFGR_PLLQ_VAL<<RCC_PLLCFGR_PLLQ_Pos |
	RCC_PLLCFGR_PLLSRC_HSE |
	RCC_PLLCFGR_PLLP_VAL<<RCC_PLLCFGR_PLLP_Pos |
	RCC_PLLCFGR_PLLN_VAL<<RCC_PLLCFGR_PLLN_Pos |
	RCC_PLLCFGR_PLLM_VAL<<RCC_PLLCFGR_PLLM_Pos);
	
	//Configurate RCC_CFGR
	_MODIFY_REG(RCC->CFGR,
	RCC_CFGR_MCO2_Msk |
	RCC_CFGR_MCO2PRE_Msk |
	RCC_CFGR_MCO1PRE_Msk |
	RCC_CFGR_I2SSRC_Msk | 
	RCC_CFGR_MCO1_Msk |
	RCC_CFGR_RTCPRE_Msk |
	RCC_CFGR_PPRE2_Msk |
	RCC_CFGR_PPRE1_Msk |
	RCC_CFGR_HPRE_Msk |
	RCC_CFGR_SW_Msk,
	RCC_CFGR_MCO2_VAL << RCC_CFGR_MCO2_Pos |
	RCC_CFGR_MCO2PRE_VAL << RCC_CFGR_MCO2PRE_Pos |
	RCC_CFGR_MCO1PRE_VAL << RCC_CFGR_MCO1PRE_Pos |
	RCC_CFGR_I2SSRC_VAL << RCC_CFGR_I2SSRC_Pos |
	RCC_CFGR_MCO1_VAL << RCC_CFGR_MCO1_Pos |
	RCC_CFGR_RTCPRE_VAL << RCC_CFGR_RTCPRE_Pos | 
	RCC_CFGR_PPRE2_VAL << RCC_CFGR_PPRE2_Pos |
	RCC_CFGR_PPRE1_VAL << RCC_CFGR_PPRE1_Pos |
	RCC_CFGR_HPRE_VAL << RCC_CFGR_HPRE_Pos);
	
	//Enable PLL (RCC_CR)
	_SET_BIT(RCC->CR,RCC_CR_PLLON);
	//check PLL is ready (RCC_CR) !!! It is a must to turn on PLL before setting RCC_CFGR_SW
	while(!(READ_BIT(RCC->CR,RCC_CR_PLLRDY))){}
	
	//check SWS (RCC_CFGR)
	_SET_BIT(RCC->CFGR,RCC_CFGR_SW_VAL);
	while(!(READ_BIT(RCC->CFGR,RCC_CFGR_SWS_VAL))){} //block until SWS is read as RCC_CFGR_SWS_PLL 
	
	//Configurate RCC_CIR enable clock security
	_SET_BIT(RCC->CR,RCC_CR_CSSON);
	_SET_BIT(RCC->CIR,RCC_CIR_CSSF);
	
	SystemCoreClockUpdate();
	
}

//RM (Section 3)
void bsp_flash_init(void){
	_SET_BIT(FLASH->ACR,FLASH_ACR_ICEN | FLASH_ACR_PRFTEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_3WS);
}
