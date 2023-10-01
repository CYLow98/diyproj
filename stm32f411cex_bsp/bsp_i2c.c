/******************************** include files ********************************/
#include "bsp_i2c.h"
/*******************************************************************************/

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
	SET_BIT(I2C1->CR1,I2C_CR1_PE);
	while(READ_BIT(I2C1->CR1,I2C_CR1_PE)==0u); //I2C_CR1_PE is not set immediately causing I2C_CR1_ACK to not set properly and make I2C not working properly as master receiver
	SET_BIT(I2C1->CR1,I2C_CR1_ACK);
}

void bsp_i2c_master_transfer(uint8_t* buf,uint16_t len,uint8_t addr){
	volatile uint8_t tempreg = 0;
	//RM (pg 479)
	//Set I2C_SR1_Start bit
	SET_BIT(I2C1->CR1,I2C_CR1_START);
	
	//Wait I2C_SR1_SB = 1 (EV5 starts, clock is stretched by master)
	while(READ_BIT(I2C1->SR1,I2C_SR1_SB)==0u);
	
	//Write addr to I2C_DR (set LSB to 0/reset)
	I2C1->DR = (addr<<1);
	
	//Wait I2C_SR1_ADDR = 1 (EV6 starts, clock is stretched by master)
	//Check I2C_SR1_AF bits as well, if AF, reset I2C
	while(READ_BIT(I2C1->SR1,I2C_SR1_ADDR|I2C_SR1_AF)==0u);
	if(READ_BIT(I2C1->SR1,I2C_SR1_AF)){//acknowledge failure, reset i2c and return
			//Generate stop bit
			SET_BIT(I2C1->CR1,I2C_CR1_STOP);
			for(volatile uint16_t i = 0;i<100;i++){
				//do nothing and wait for the stop bit to kick in
			}
			//Reset I2C
			bsp_i2c_init();
			return;
	}
	
	//Read I2C_SR1 then I2C_SR2 (clear ADDR)
	tempreg = I2C1->SR1;
	tempreg = I2C1->SR2;
	
	//Wait until I2C_SR1_TxE = 1 (EV8_1 starts)
	while(READ_BIT(I2C1->SR1,I2C_SR1_TXE)==0u);
	
	//Loop : Write data in current pointer of buf to I2C_DR
	//			 Wait until I2C_SR1_TxE=1(because set by hardware when ACK is get) (EV8 starts)
	//			 Increment buf to move on to next byte
	for (uint16_t i=0;i<len;i++){
		I2C1->DR=(*buf);
		while(READ_BIT(I2C1->SR1,I2C_SR1_TXE)==0u);
		buf++;
	}
	
	//When loop finishes wait I2C_SR1_TxE & I2C_SR1_BTF = 1 (EV8_2 starts, clock is stretched by master)
	while(READ_BIT(I2C1->SR1,I2C_SR1_TXE)==0u || READ_BIT(I2C1->SR1,I2C_SR1_BTF)==0u);
	//Set I2C_CR1_STOP
	SET_BIT(I2C1->CR1,I2C_CR1_STOP);
}

void bsp_i2c_master_receive(uint8_t* buf,uint16_t len,uint8_t addr){
	volatile uint8_t tempreg = 0;
	
	//Check len to be greater than 0 or return
	if(len<=0){
		return;
	}
	
	//RM (pg 479)
	//Set I2C_SR1_START bit 
	SET_BIT(I2C1->CR1,I2C_CR1_START);
	
	//Wait I2C_SR1_SB = 1 (EV5 starts)
	while(READ_BIT(I2C1->SR1,I2C_SR1_SB)==0u);
	
	//Write addr to I2C_DR (set LSB to 1/set)
	I2C1->DR = (addr<<1)|(1); //read
	
	//Wait I2C_SR1_ADDR = 1 (EV6 starts, clock is stretched by master)
	//Check I2C_SR1_AF bits as well, if AF, reset I2C
	while(READ_BIT(I2C1->SR1,I2C_SR1_ADDR|I2C_SR1_AF)==0u);
	if(READ_BIT(I2C1->SR1,I2C_SR1_AF)){//acknowledge failure, reset i2c and return
			//Generate stop bit
			SET_BIT(I2C1->CR1,I2C_CR1_STOP);
			for(volatile uint16_t i = 0;i<100;i++){
				//do nothing and wait for the stop bit to kick in
			}
			//Reset I2C
			bsp_i2c_init();
			return;
	}
	
	//RM (pg 481)
	//Need to send NACK after last byte received
	//If len == 1, Clear I2C_CR1_ACK bit
	//						 Read I2C_SR1 then I2C_SR2 (clear ADDR, after master stops clock stretched)
	//						 Set I2C_CR1_STOP bit
	//						 Wait until I2C_SR1_RxNE = 1 (EV7 started)
	//						 Read data into buf from I2C_DR
	//						 Set I2C_CR1_ACK bit
	//						 Return
	if(len==1){
		CLEAR_BIT(I2C1->CR1,I2C_CR1_ACK);
		SET_BIT(I2C1->CR1,I2C_CR1_POS);
		tempreg = I2C1->SR1;
		tempreg = I2C1->SR2;
		SET_BIT(I2C1->CR1,I2C_CR1_STOP);
		while(READ_BIT(I2C1->SR1,I2C_SR1_RXNE)==0u);
		(*buf) = I2C1->DR;
		CLEAR_BIT(I2C1->CR1,I2C_CR1_POS);
		SET_BIT(I2C1->CR1,I2C_CR1_ACK);
		return;
	}
	
	
	//RM (pg 482 & 483)
	//Need to send NACK after last byte received
	//If len == 2, Clear I2C_CR1_ACK bit and set I2C_CR1_POS bit
	//						Read I2C_SR1 then I2C_SR2 (clear ADDR, after master stops clock stretched)
	//						Wait until I2C_SR1_BTF = 1 (Data 1 in I2C_DR, Data 2 in shift register, last byte received, also I2C_SR1_RxNE=1, EV7_1 started, master strethes clock)
	//						Set I2C_CR1_STOP bit
	//						Read Data 1 into buf[0] from I2C_DR (Data 2 move from shift register to I2C_DR, NACK will be sent to slave)
	//						Read data 2 into buf[1] from I2C_DR (right after this stop bits (P) will be sent to slave)
	//						Set I2C_CR1_ACK bit, clear I2C_CR1_POS bit
	//			      Return
	if(len==2){
		CLEAR_BIT(I2C1->CR1,I2C_CR1_ACK);
		SET_BIT(I2C1->CR1,I2C_CR1_POS);
		tempreg = I2C1->SR1;
		tempreg = I2C1->SR2;
		while(READ_BIT(I2C1->SR1,I2C_SR1_BTF)==0u);
		SET_BIT(I2C1->CR1,I2C_CR1_STOP);
		buf[0] = I2C1->DR;
		while(READ_BIT(I2C1->SR1,I2C_SR1_RXNE)==0u);
		buf[1] = I2C1->DR;
		CLEAR_BIT(I2C1->CR1,I2C_CR1_POS);
		SET_BIT(I2C1->CR1,I2C_CR1_ACK);
		return;
	}
	//Read I2C_SR1 then I2C_SR2 (clear ADDR)
	//Initiate i as 0
	//Loop : While len > 0
	//			 Need to send NACK after last byte received
	//			 If len == 3, Wait until I2C_SR1_BTF = 1 (Data N-2 in I2C_DR, Data N-1 in shift register, also I2C_SR1_RxNE=1)
	//										Clear I2C_CR1_ACK bit
	//									  Read Data N-2 into buf[i++] from I2C_DR
	//										Wait until I2C_SR1_BTF = 1 (Data N-1 in I2C_DR, Data N in shift register, last byte received, also I2C_SR1_RxNE=1, EV7_1 started, master strethes clock)
	//										Set I2C_CR1_STOP bit
	//                    Read Data N-1 into buf[i++] from I2C_DR (Data N will move from shift register to I2C_DR, NACK will be sent to slave)
	//										Read Data N into buf[i]
	//                    Set I2C_CR1_ACK bit
	//										Return
	//			 Else, Wait RxNE = 1 (ACK sent to slave from master, EV7 started, have data in I2C_DR)
	//		   			 Read from I2C_DR and write to buf[i++]
	//             Decrement len
	//       #because len==1 and len==2 has been check, so I can be sure that this sequence can only return from three possible places
	tempreg = I2C1->SR1;
	tempreg = I2C1->SR2;
	uint16_t i = 0;
	while(len>0){
		if(len==3){
			while(READ_BIT(I2C1->SR1,I2C_SR1_BTF)==0u);
			CLEAR_BIT(I2C1->CR1,I2C_CR1_ACK);
			buf[i++] = I2C1->DR;
			while(READ_BIT(I2C1->SR1,I2C_SR1_BTF)==0u);
			SET_BIT(I2C1->CR1,I2C_CR1_STOP);
			buf[i++] = I2C1->DR;
			while(READ_BIT(I2C1->SR1,I2C_SR1_RXNE)==0u);
			buf[i] = I2C1->DR; //last one
			SET_BIT(I2C1->CR1,I2C_CR1_ACK);
			return;
		}else{
			while(READ_BIT(I2C1->SR1,I2C_SR1_RXNE)==0u);
			buf[i++] = I2C1->DR;
			len--;
		}
	}
}

void bsp_i2c_master_receive_reg(uint8_t* buf,uint16_t len,uint8_t addr,uint8_t* regs,uint16_t reglen){
	volatile uint8_t tempreg = 0;
	
	//With restart
	//Set I2C_SR1_Start bit
	SET_BIT(I2C1->CR1,I2C_CR1_START);
	
	//Wait I2C_SR1_SB = 1 (EV5 starts)
	while(READ_BIT(I2C1->SR1,I2C_SR1_SB)==0u);
	
	//Write addr to I2C_DR (set LSB to 0/reset)
	I2C1->DR = (addr<<1);
	
	//Wait I2C_SR1_ADDR = 1 (EV6 starts)
	//Check I2C_SR1_AF bits as well, if AF, reset I2C
	while(READ_BIT(I2C1->SR1,I2C_SR1_ADDR|I2C_SR1_AF)==0u);
	if(READ_BIT(I2C1->SR1,I2C_SR1_AF)){//acknowledge failure, reset i2c and return
			//Generate stop bit
			SET_BIT(I2C1->CR1,I2C_CR1_STOP);
			for(volatile uint16_t i = 0;i<100;i++){
				//do nothing and wait for the stop bit to kick in
			}
			//Reset I2C
			bsp_i2c_init();
			return;
	}
	
	//Read I2C_SR1 then I2C_SR2 (clear ADDR)
	tempreg = I2C1->SR1;
	tempreg = I2C1->SR2;
	
	//Wait until I2C_SR1_TxE = 1 (EV8_1 starts)
	while(READ_BIT(I2C1->SR1,I2C_SR1_TXE)==0u);
	
	//Loop : For 0 to reglen as i 
	//       Write data in regs[i] to I2C_DR
	//			 Wait until I2C_SR1_TxE=1(because set by hardware when ACK is get) (EV8 starts)
	for (uint16_t i=0;i<reglen;i++){
		I2C1->DR=regs[i];
		while(READ_BIT(I2C1->SR1,I2C_SR1_TXE)==0u);
	}
	
	//When loop finishes wait I2C_SR1_TxE & I2C_SR1_BTF = 1 (EV8_2 starts, clock is stretched by master)
	while(READ_BIT(I2C1->SR1,I2C_SR1_TXE)==0u || READ_BIT(I2C1->SR1,I2C_SR1_BTF)==0u);
	
	//call bsp_i2c_master_receive(), restart will happen when I2C_SR1_Start bit is set in the beginning of the function
	bsp_i2c_master_receive(buf,len,addr);
}