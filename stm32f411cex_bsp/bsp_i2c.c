#include "stm32f4xx.h"                  // Device header
#include "stm32f411xe.h"                  // Device header
#include "bsp_gpio.h"
#include "bsp_i2c.h"

//***********************************CONSTANT******************************//
//RM(Chapter 18.6.8)
#define I2C_CCR_FS_SM 0x0u //standard mode
#define I2C_CCR_FS_FM 0x1u //standard mode

#define I2C_CCR_DUTY_0 0x0u //2
#define I2C_CCR_DUTY_1 0x1u //16/9
//*************************************************************************//

//***********************************VALUE*********************************//
#define SCL_FREQ 384000 //400KHz
#define I2C_CCR_FS_VAL I2C_CCR_FS_FM
#define I2C_CCR_DUTY_VAL I2C_CCR_DUTY_1
//*************************************************************************//
void bsp_i2c_init(void){
	//Set up RCC_AHB1ENR
	SET_BIT(RCC->AHB1ENR,RCC_AHB1ENR_GPIOBEN);
	
	//Set up GPIO PB8(SCL1) and PB9(SDA1) as follows
	//GPIOB_MODER - AF (alternate function)
	MODIFY_REG(GPIOB->MODER,GPIO_MODER_MODE8_Msk|GPIO_MODER_MODE9_Msk,(GPIO_MODER_AF<<GPIO_MODER_MODE8_Pos) | (GPIO_MODER_AF<<GPIO_MODER_MODE9_Pos));
	//GPIOB_OTYPER - OD (open drain)
	MODIFY_REG(GPIOB->OTYPER,GPIO_OTYPER_OT8_Msk|GPIO_OTYPER_OT9_Msk,(GPIO_OTYPER_OD<<GPIO_OTYPER_OT8_Pos)|(GPIO_OTYPER_OD<<GPIO_OTYPER_OT9_Pos));
	//GPIOB_OSPEEDR - HS (High speed)
	MODIFY_REG(GPIOB->OSPEEDR,GPIO_OSPEEDR_OSPEED8_Msk|GPIO_OSPEEDR_OSPEED9_Msk,(GPIO_OSPEEDR_HS<<GPIO_OSPEEDR_OSPEED8_Pos)|(GPIO_OSPEEDR_HS<<GPIO_OSPEEDR_OSPEED9_Pos));
	//GPIOB_PUPDR - NONE
	MODIFY_REG(GPIOB->PUPDR,GPIO_PUPDR_PUPD8_Msk|GPIO_PUPDR_PUPD9_Msk,(GPIO_PUPDR_PU<<GPIO_PUPDR_PUPD8_Pos)|(GPIO_PUPDR_PU<<GPIO_PUPDR_PUPD9_Pos));
	//GPIOB_AFRL - AF4
	MODIFY_REG(GPIOB->AFR[1],GPIO_AFRH_AFSEL8_Msk|GPIO_AFRH_AFSEL9_Msk,(GPIO_AFRX_AF4<<GPIO_AFRH_AFSEL8_Pos)|(GPIO_AFRX_AF4<<GPIO_AFRH_AFSEL9_Pos));
	
	//RM (Chapter 18.3.3 Pg 478)
	//Enable I2C on APB1
	SET_BIT(RCC->APB1ENR,RCC_APB1ENR_I2C1EN);
	
	//Disable I2C ie.. I2C_CR1_PE
	CLEAR_BIT(I2C1->CR1,I2C_CR1_PE);
	while(READ_BIT(I2C1->CR1,I2C_CR1_PE)!=0u);
	//Reset I2C
	SET_BIT(I2C1->CR1,I2C_CR1_SWRST);
	while(READ_BIT(I2C1->SR2,I2C_SR2_BUSY));
	CLEAR_BIT(I2C1->CR1,I2C_CR1_SWRST);
	//Program the peripheral input clock in I2C1_CR2 Register in order to generate correct timings
	//systemcoreclock refers to HCLK, freq refers to (HCLK/(APB1 prescaler))/1000000;
	uint32_t apb1pre = APBPrescTable[(READ_BIT(RCC->CFGR,RCC_CFGR_PPRE1_Msk)>>RCC_CFGR_PPRE1_Pos)]; //Get the bits in RCC_CFGR then use it to look up the division factor
	uint32_t freq = SystemCoreClock>>apb1pre;
	MODIFY_REG(I2C1->CR2,I2C_CR2_FREQ_Msk,(freq/1000000u));
	
	//Configure the clock control register I2C_CCR https://fastbitlab.com/implementation-of-i2c-init-api-part-3/
	MODIFY_REG(I2C1->CCR,
	I2C_CCR_FS_Msk|I2C_CCR_DUTY_Msk,
	(I2C_CCR_FS_VAL<<I2C_CCR_FS_Pos)|
	(I2C_CCR_DUTY_VAL<<I2C_CCR_DUTY_Pos));
	uint32_t ccrVal = 0;
	if (I2C_CCR_FS_VAL == I2C_CCR_FS_SM){ //standard mode
		ccrVal=freq/(SCL_FREQ<<1);
	}else{ //fast mode
		if(I2C_CCR_DUTY_VAL == I2C_CCR_DUTY_0){
			ccrVal=(freq)/(SCL_FREQ*3);
		}else{
			ccrVal=(freq)/(SCL_FREQ*25);
		}
	}
	MODIFY_REG(I2C1->CCR,I2C_CCR_CCR_Msk,ccrVal<<I2C_CCR_CCR_Pos);
	
	//Configure the rise time register I2C_TRISE https://fastbitlab.com/i2c-rise-time-calculation/
  uint32_t trise = 0;
	if (I2C_CCR_FS_VAL == I2C_CCR_FS_SM){
		trise = (freq / 1000000u)+1; //1000ns
	}
	else{
		trise = ((freq * 3)/ 10000000u)+1; //300ns
	}
	MODIFY_REG(I2C1->TRISE,I2C_TRISE_TRISE_Msk,trise<<I2C_TRISE_TRISE_Pos);
	
	//Program the I2C1_CR1 register to enable the peripheral
	SET_BIT(I2C1->CR1,I2C_CR1_ACK);
	SET_BIT(I2C1->CR1,I2C_CR1_PE);
}

void bsp_i2c_master_transfer(uint8_t* buf,uint16_t len,uint8_t addr){
	volatile uint8_t tempreg = 0;
	//RM (pg 479)
	//Set Start bit in I2C_SR1
	SET_BIT(I2C1->CR1,I2C_CR1_START);
	
	//Wait SB = 1 in I2C_SR1 (EV5 starts)
	while(READ_BIT(I2C1->SR1,I2C_SR1_SB)==0u);
	
	//Write addr to I2C_DR (set LSB to 0/reset)
	I2C1->DR = (addr<<1);
	
	//Wait ADDR = 1 in I2C_SR1 (EV6 starts)
	//Todo check AF bits as well, if AF, reset I2C
	while(READ_BIT(I2C1->SR1,I2C_SR1_ADDR|I2C_SR1_AF)==0u);
	if(READ_BIT(I2C1->SR1,I2C_SR1_AF)){//acknowledge failure, reset i2c and return
			//Generate stop bit
			//SET_BIT(I2C1->CR1,I2C_CR1_STOP);
			//Reset I2C
			bsp_i2c_init();
			return;
	}
	
	//Read I2C_SR1 then I2C_SR2 (clear ADDR)
	tempreg = I2C1->SR1;
	tempreg = I2C1->SR2;
	
	//Wait until TxE=1 in I2C_SR1(EV8_1 starts)
	while(READ_BIT(I2C1->SR1,I2C_SR1_TXE)==0u);
	
	//Loop : Write data in current pointer of buf to I2C_DR
	//			 Wait until TxE=1(because set by hardware when ACK is get) (EV8 starts)
	//			 Increment buf to move on to next byte
	for (uint16_t i=0;i<len;i++){
		I2C1->DR=(*buf);
		while(READ_BIT(I2C1->SR1,I2C_SR1_TXE)==0u);
		buf++;
	}
	
	//When loop finishes wait TxE & BTF = 1 (EV8_2 starts)
	while(READ_BIT(I2C1->SR1,I2C_SR1_TXE)==0u || READ_BIT(I2C1->SR1,I2C_SR1_BTF)==0u);
	//Set stop bit
	SET_BIT(I2C1->CR1,I2C_CR1_STOP);
}