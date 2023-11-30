
/******************************** include files ********************************/
#include "bsp_spi.h"
#include "stdlib.h"
#include "stm32f4xx.h"                  // Device header
#include "stm32f411xe.h"                  // Device header
#include "bsp_gpio.h"
#include <stdbool.h>
//#include <stdio.h>
/*******************************************************************************/

typedef struct{
	bool busy;
	void (*transfer_complete_cb_)(void);
	void (*transfer_error_cb_)(void);
}SPIUtilStruct;

volatile SPIUtilStruct *SPIUtil;

#define SPI_CR1_BR_VAL 4 //device by 32
#define SPI_CR1_CPOL_VAL 1 //0: CK to 0 when idle
#define SPI_CR1_CPHA_VAL 0 //0: The first clock transition is the first data capture edge
#define SPI_CR1_DFF_VAL 0 //0: 8-bit data frame format is selected for transmission/reception
#define SPI_CR1_LSBFIRST_VAL 0 //0: MSB transmitted first

//Init and configure DMA, GPIO and SPI
//Need to also initialise SPIUtilStruct
//TODO: should be device dependant
void bsp_spi_init(){
	//init SPIUtil
	SPIUtil = (SPIUtilStruct*)malloc(sizeof(SPIUtilStruct));
	SPIUtil->busy = false;
	SPIUtil->transfer_complete_cb_ = ((void*)0);
	SPIUtil->transfer_error_cb_ = ((void*)0);
	
	//Enable bus
	SET_BIT(RCC->APB2ENR,RCC_APB2ENR_SPI1EN);
	SET_BIT(RCC->AHB1ENR,RCC_AHB1ENR_DMA2EN);
	
	//Configure GPIO
	//GPIOA_MODER - AF (alternate function)
	MODIFY_REG(GPIOA->MODER,GPIO_MODER_MODE5_Msk|GPIO_MODER_MODE6_Msk|GPIO_MODER_MODE7_Msk,(GPIO_MODER_AF<<GPIO_MODER_MODE5_Pos) | (GPIO_MODER_AF<<GPIO_MODER_MODE6_Pos) | (GPIO_MODER_AF<<GPIO_MODER_MODE7_Pos));
	//GPIOB_OTYPER - OD (open drain)
	MODIFY_REG(GPIOA->OTYPER,GPIO_OTYPER_OT5_Msk|GPIO_OTYPER_OT6_Msk|GPIO_OTYPER_OT7_Msk,(GPIO_OTYPER_PP<<GPIO_OTYPER_OT5_Pos)|(GPIO_OTYPER_PP<<GPIO_OTYPER_OT6_Pos)|(GPIO_OTYPER_PP<<GPIO_OTYPER_OT7_Pos));
	//GPIOB_OSPEEDR - HS (High speed)
	MODIFY_REG(GPIOA->OSPEEDR,GPIO_OSPEEDR_OSPEED5_Msk|GPIO_OSPEEDR_OSPEED6_Msk|GPIO_OSPEEDR_OSPEED7_Msk,(GPIO_OSPEEDR_HS<<GPIO_OSPEEDR_OSPEED5_Pos)|(GPIO_OSPEEDR_HS<<GPIO_OSPEEDR_OSPEED6_Pos)|(GPIO_OSPEEDR_HS<<GPIO_OSPEEDR_OSPEED7_Pos));
	//GPIOB_PUPDR - NONE
	MODIFY_REG(GPIOA->PUPDR,GPIO_PUPDR_PUPD5_Msk|GPIO_PUPDR_PUPD6_Msk|GPIO_PUPDR_PUPD7_Msk,(GPIO_PUPDR_NONE<<GPIO_PUPDR_PUPD5_Pos)|(GPIO_PUPDR_PU<<GPIO_PUPDR_PUPD6_Pos)|(GPIO_PUPDR_NONE<<GPIO_PUPDR_PUPD7_Pos));
	//GPIOB_AFRL - AF5
	MODIFY_REG(GPIOA->AFR[0],GPIO_AFRL_AFSEL5_Msk|GPIO_AFRL_AFSEL6_Msk|GPIO_AFRL_AFSEL7_Msk,(GPIO_AFRX_AF5<<GPIO_AFRL_AFSEL5_Pos)|(GPIO_AFRX_AF5<<GPIO_AFRL_AFSEL6_Pos)|(GPIO_AFRX_AF5<<GPIO_AFRL_AFSEL7_Pos));
	
	
	//RM (20.3.3)
	//1. Select the BR[2:0] bits to define the serial clock baud rate (see SPI_CR1 register)'
	//2. Select the CPOL and CPHA bits to define one of the four relationships between the data transfer and the serial clock
	//3. Set the DFF bit to define 8- or 16-bit data frame format
	//4. Configure the LSBFIRST bit in the SPI_CR1 register to define the frame format. This step is not required when the TI mode is selected.
	//5. Set the FRF bit in SPI_CR2 to select the TI protocol for serial communications
	CLEAR_BIT(SPI1->CR1,SPI_CR1_SPE);
	while(READ_BIT(SPI1->CR1,SPI_CR1_SPE)); //disable SPI
	
	MODIFY_REG(SPI1->CR1,SPI_CR1_BR_Msk,(SPI_CR1_BR_VAL<<SPI_CR1_BR_Pos)); //baud rate
	MODIFY_REG(SPI1->CR1,SPI_CR1_CPOL_Msk,(SPI_CR1_CPOL_VAL<<SPI_CR1_CPOL_Pos)); //clock setting
	MODIFY_REG(SPI1->CR1,SPI_CR1_CPHA_Msk,(SPI_CR1_CPHA_VAL<<SPI_CR1_CPHA_Pos)); //clock setting
	MODIFY_REG(SPI1->CR1,SPI_CR1_DFF_Msk,(SPI_CR1_DFF_VAL<<SPI_CR1_CPHA_Pos)); //dataframe 8 bits or 16 bits
	MODIFY_REG(SPI1->CR1,SPI_CR1_LSBFIRST_Msk,(SPI_CR1_LSBFIRST_VAL<<SPI_CR1_LSBFIRST_Pos)); //endianness
	MODIFY_REG(SPI1->CR1,SPI_CR1_MSTR_Msk,(1<<SPI_CR1_MSTR_Pos)); //Master or slave
	MODIFY_REG(SPI1->CR1,SPI_CR1_SSM_Msk,(1<<SPI_CR1_SSM_Pos)); //software slave management
	CLEAR_BIT(SPI1->CR2,SPI_CR2_FRF);
	CLEAR_BIT(SPI1->CR1,SPI_CR1_CRCEN);
	
	//RM (9.3.17)
	//If the stream is enabled, disable it by resetting the EN bit in the DMA_SxCR register
	//Select the DMA channel (request) using CHSEL[2:0] in the DMA_SxCR register
	//Reset the PFCTRL bit in the DMA_SxCR register.
	//Configure the stream priority using the PL[1:0] bits in the DMA_SxCR register
	//Configure the FIFO usage (enable or disable, threshold in transmission and reception)
	// Configure the data transfer direction, peripheral and memory incremented/fixed mode, 
	// single or burst transactions, peripheral and memory data widths, Circular mode, 
	// Double buffer mode and interrupts after half and/or full transfer, 
	// and/or errors in the DMA_SxCR register.
	
	//DMA2_Stream3(Tx)
	CLEAR_BIT(DMA2_Stream3->CR,DMA_SxCR_EN);
	while(READ_BIT(DMA2_Stream3->CR,DMA_SxCR_EN)); //when EN bits is cleared, DMA is ready to be configured
	
	MODIFY_REG(DMA2_Stream3->CR,DMA_SxCR_CHSEL_Msk,(3<<DMA_SxCR_CHSEL_Pos)); //Channel 3
	CLEAR_BIT(DMA2_Stream3->CR,DMA_SxCR_PFCTRL); //DMA controller as controller
	MODIFY_REG(DMA2_Stream3->CR,DMA_SxCR_PL_Msk,(3<<DMA_SxCR_PL_Pos)); //Very High Priority
	MODIFY_REG(DMA2_Stream3->CR,DMA_SxCR_MBURST_Msk,(0<<DMA_SxCR_MBURST_Pos)); //Single Transfer for memory
	MODIFY_REG(DMA2_Stream3->CR,DMA_SxCR_PBURST_Msk,(0<<DMA_SxCR_PBURST_Pos)); //Single Transfer for peripheral
	MODIFY_REG(DMA2_Stream3->CR,DMA_SxCR_DBM_Msk,(0<<DMA_SxCR_DBM_Pos)); //Double buffer off
	MODIFY_REG(DMA2_Stream3->CR,DMA_SxCR_PINCOS_Msk,(0<<DMA_SxCR_PINCOS_Pos)); //No offset for peripheral increase
	MODIFY_REG(DMA2_Stream3->CR,DMA_SxCR_MSIZE_Msk,(0<<DMA_SxCR_MSIZE_Pos)); //Byte size memory for memory
	MODIFY_REG(DMA2_Stream3->CR,DMA_SxCR_PSIZE_Msk,(0<<DMA_SxCR_PSIZE_Pos)); //Byte size memory for peripheral
	MODIFY_REG(DMA2_Stream3->CR,DMA_SxCR_MINC_Msk,(1<<DMA_SxCR_MINC_Pos)); //Memory increment on for memory
	MODIFY_REG(DMA2_Stream3->CR,DMA_SxCR_PINC_Msk,(0<<DMA_SxCR_PINC_Pos)); //Peripheral increment off for peripheral
	MODIFY_REG(DMA2_Stream3->CR,DMA_SxCR_CIRC_Msk,(0<<DMA_SxCR_CIRC_Pos)); //Memory circular buffer off
	MODIFY_REG(DMA2_Stream3->CR,DMA_SxCR_DIR_Msk,(1<<DMA_SxCR_DIR_Pos)); //Memory to peripheral
	SET_BIT(DMA2_Stream3->CR,DMA_SxCR_TCIE); //Enable interrupt raise when transfer completed
	SET_BIT(DMA2_Stream3->CR,DMA_SxCR_TEIE); //Enable interrupt raise when transfer error
	SET_BIT(DMA2_Stream3->CR,DMA_SxCR_DMEIE); //Enable interrupt raise when direct mode error
	
	MODIFY_REG(DMA2_Stream3->FCR,DMA_SxFCR_DMDIS_Msk,(1<<DMA_SxFCR_DMDIS_Pos)); //Enable FIFO
	MODIFY_REG(DMA2_Stream3->FCR,DMA_SxFCR_FTH_Msk,(0<<DMA_SxFCR_FTH_Pos)); //1/4 FIFO Threshold
	
	
	//DMA2_Stream2(Rx)
	CLEAR_BIT(DMA2_Stream2->CR,DMA_SxCR_EN);
	
	while(READ_BIT(DMA2_Stream2->CR,DMA_SxCR_EN)); //when EN bits is cleared, DMA is ready to be configured
	
	MODIFY_REG(DMA2_Stream2->CR,DMA_SxCR_CHSEL_Msk,(3<<DMA_SxCR_CHSEL_Pos)); //Channel 3
	CLEAR_BIT(DMA2_Stream2->CR,DMA_SxCR_PFCTRL); //DMA controller as controller
	MODIFY_REG(DMA2_Stream2->CR,DMA_SxCR_PL_Msk,(3<<DMA_SxCR_PL_Pos)); //Very High Priority
	MODIFY_REG(DMA2_Stream2->CR,DMA_SxCR_MBURST_Msk,(0<<DMA_SxCR_MBURST_Pos)); //Single Transfer for memory
	MODIFY_REG(DMA2_Stream2->CR,DMA_SxCR_PBURST_Msk,(0<<DMA_SxCR_PBURST_Pos)); //Single Transfer for peripheral
	MODIFY_REG(DMA2_Stream2->CR,DMA_SxCR_DBM_Msk,(0<<DMA_SxCR_DBM_Pos)); //Double buffer off
	MODIFY_REG(DMA2_Stream2->CR,DMA_SxCR_PINCOS_Msk,(0<<DMA_SxCR_PINCOS_Pos)); //No offset for peripheral increase
	MODIFY_REG(DMA2_Stream2->CR,DMA_SxCR_MSIZE_Msk,(0<<DMA_SxCR_MSIZE_Pos)); //Byte size memory for memory
	MODIFY_REG(DMA2_Stream2->CR,DMA_SxCR_PSIZE_Msk,(0<<DMA_SxCR_PSIZE_Pos)); //Byte size memory for peripheral
	MODIFY_REG(DMA2_Stream2->CR,DMA_SxCR_MINC_Msk,(1<<DMA_SxCR_MINC_Pos)); //Memory increment on for memory
	MODIFY_REG(DMA2_Stream2->CR,DMA_SxCR_PINC_Msk,(0<<DMA_SxCR_PINC_Pos)); //Peripheral increment off for peripheral
	MODIFY_REG(DMA2_Stream2->CR,DMA_SxCR_CIRC_Msk,(0<<DMA_SxCR_CIRC_Pos)); //Memory circular buffer off
	MODIFY_REG(DMA2_Stream2->CR,DMA_SxCR_DIR_Msk,(0<<DMA_SxCR_DIR_Pos)); //Peripheral to memory
	SET_BIT(DMA2_Stream2->CR,DMA_SxCR_TEIE); //Enable interrupt raise when transfer error
	SET_BIT(DMA2_Stream2->CR,DMA_SxCR_DMEIE); //Enable interrupt raise when direct mode error
	
	MODIFY_REG(DMA2_Stream2->FCR,DMA_SxFCR_DMDIS_Msk,(1<<DMA_SxFCR_DMDIS_Pos)); //Enable FIFO
	MODIFY_REG(DMA2_Stream2->FCR,DMA_SxFCR_FTH_Msk,(0<<DMA_SxFCR_FTH_Pos)); //1/4 FIFO Threshold
	
	//Enable interrupt for DMA2_Stream3(Tx) and DMA2_Stream2(Rx)
	NVIC_EnableIRQ(DMA2_Stream3_IRQn);
	NVIC_EnableIRQ(DMA2_Stream2_IRQn);
}

//Configure SPI and DMA for transmission to write content in txBuf array to peripheral
void bsp_spi_transmit_dma(const uint8_t *txBuf,uint16_t length){	
	
	//RM (9.3.17)
	//Set the peripheral port register address in the DMA_SxPAR register as SPI
	//Set the memory address in the DMA_SxMA0R register as txBuf
	//Set the total number of data items to be transferred in the DMA_SxNDTR register as length
	//Activate the stream by setting the EN bit in the DMA_SxCR register
	
	//RM (20.3.9)
	
	if(!SPIUtil->busy){
		SET_BIT(SPI1->CR1,SPI_CR1_SSI);
		SPIUtil->busy = true;
		CLEAR_BIT(DMA2_Stream3->CR,DMA_SxCR_EN);
		while(READ_BIT(DMA2_Stream3->CR,DMA_SxCR_EN)); //when EN bits is cleared, DMA is ready to be configured
		CLEAR_BIT(SPI1->CR1,SPI_CR1_SPE);
		while(READ_BIT(SPI1->CR1,SPI_CR1_SPE)); //disable SPI
		DMA2_Stream3->PAR = (uint32_t)&SPI1->DR;
		DMA2_Stream3->M0AR = (uint32_t)txBuf;
		DMA2_Stream3->NDTR = length;
		
		SET_BIT(SPI1->CR2,SPI_CR2_TXDMAEN);
		
		//Enable DMA and SPI
		SET_BIT(SPI1->CR1,SPI_CR1_SPE);
		SET_BIT(DMA2_Stream3->CR,DMA_SxCR_EN);
	}

}

//Configure SPI and DMA for transmission and receive and write content to rxBuf
void bsp_spi_transmit_receive_dma(const uint8_t *txBuf,uint8_t * rxBuf, uint16_t length){
	
	//Enable interrupt for DMA2_Stream3(Tx) and DMA2_Stream2(Rx)
		if(!SPIUtil->busy){
			SET_BIT(SPI1->CR1,SPI_CR1_SSI);
			SPIUtil->busy = true;
			CLEAR_BIT(DMA2_Stream3->CR,DMA_SxCR_EN);
			while(READ_BIT(DMA2_Stream3->CR,DMA_SxCR_EN)); //when EN bits is cleared, DMA is ready to be configured
			CLEAR_BIT(DMA2_Stream2->CR,DMA_SxCR_EN);
			while(READ_BIT(DMA2_Stream2->CR,DMA_SxCR_EN)); //when EN bits is cleared, DMA is ready to be configured
			CLEAR_BIT(SPI1->CR1,SPI_CR1_SPE);
			while(READ_BIT(SPI1->CR1,SPI_CR1_SPE)); //disable SPI
			DMA2_Stream3->PAR = (uint32_t)&SPI1->DR;
			DMA2_Stream3->M0AR = (uint32_t)txBuf;
			DMA2_Stream3->NDTR = length;
			
			while(READ_BIT(SPI1->SR,SPI_SR_RXNE)){
				uint8_t temp = SPI1->DR;
			}
			DMA2_Stream2->PAR = (uint32_t)&SPI1->DR;
			DMA2_Stream2->M0AR = (uint32_t)rxBuf;
			DMA2_Stream2->NDTR = length;
			
			SET_BIT(SPI1->CR2,SPI_CR2_TXDMAEN);
			SET_BIT(SPI1->CR2,SPI_CR2_RXDMAEN);
			
			//Enable DMA and SPI
			SET_BIT(SPI1->CR1,SPI_CR1_SPE);
			SET_BIT(DMA2_Stream3->CR,DMA_SxCR_EN);
			SET_BIT(DMA2_Stream2->CR,DMA_SxCR_EN);
	}

}

//Definition for SPI1 TRANSFER IRQ handler
//Need to check errors or completed, and then service interrupt(reset DMA and SPI for next SPI request, set busy to false) and then call callback
//If completed, need to wait until TXE and BSY flags to reset before servicing and calling back
void DMA2_Stream3_IRQHandler(void){
	//https://www.jamessjackson.com/stm32,/f4,/spi,/dma,/c++,/embedded,/interrupts/2017/10/13/Interrupt-Driven-SPI-with-DMA-for-STM32F4/
	//check error
	//Clear interrupt
	//Call transfer_error_cb_ if not null
	//printf("enter\n");
	//Set transfer_error_cb_ to null
	if(READ_BIT(DMA2->LISR,DMA_LISR_TEIF3|DMA_LISR_DMEIF3)){
		SET_BIT(DMA2->LIFCR,DMA_LIFCR_CTEIF3|DMA_LIFCR_CDMEIF3);
		if((SPIUtil->transfer_error_cb_)!=(void*)0){
			SPIUtil->transfer_error_cb_();
		}
		SPIUtil->transfer_error_cb_ = (void*)0;
	}
	
	//complete(TCIF flag set)
	//Clear interrupt
	//RM (20.3.8 Disabling the SPI)
	//1. Wait until SPI_SR_TXE=1
	//2. Then wait until BSY=0
	//Call transfer_complete_cb_ if not null
	//Set transfer_complete_cb_ to null
	if(READ_BIT(DMA2->LISR,DMA_LISR_TCIF3)){ //transfer completed
		while(!READ_BIT(SPI1->SR,SPI_SR_TXE));
		while(READ_BIT(SPI1->SR,SPI_SR_BSY));
		if((SPIUtil->transfer_complete_cb_)!=(void*)0){
			SPIUtil->transfer_complete_cb_();
		}
		SPIUtil->transfer_complete_cb_ = (void*)0;
		SET_BIT(DMA2->LIFCR,DMA_LIFCR_CTCIF3);
		SET_BIT(DMA2->LIFCR,DMA_LIFCR_CTCIF2); //clear TCIF2 as well, if not DMA_Stream2(Rx) will not activate correctly the next time
	}
	
	//Disable DMA Stream
	//Disable the SPI (SPE=0) and, eventually, enter the Halt mode (or disable the peripheral clock)
	//Reset SPI_CR2_TXDMAEN
	//Reset SPI_CR2_RXDMAEN
	//Set busy to false
	CLEAR_BIT(SPI1->CR1,SPI_CR1_SPE);
	CLEAR_BIT(DMA2_Stream3->CR,DMA_SxCR_EN);
	CLEAR_BIT(DMA2_Stream2->CR,DMA_SxCR_EN);
	while(READ_BIT(SPI1->CR1,SPI_CR1_SPE)); //disable SPI
	while(READ_BIT(DMA2_Stream3->CR,DMA_SxCR_EN)); //when EN bits is cleared, DMA is ready to be configured
	while(READ_BIT(DMA2_Stream2->CR,DMA_SxCR_EN)); //when EN bits is cleared, DMA is ready to be configured
	
	CLEAR_BIT(SPI1->CR2,SPI_CR2_TXDMAEN);
	CLEAR_BIT(SPI1->CR2,SPI_CR2_RXDMAEN);
	
	SPIUtil->busy=false;
	CLEAR_BIT(SPI1->CR1,SPI_CR1_SSI);
}

//Definition for SPI1 RECEIVE IRQ handler
//Need to check errors, service interrupt(reset DMA and SPI for next SPI request)
//Error trigger DMAx_Streamy_IRQHandler is caused by DMA and not SPI
void DMA2_Stream2_IRQHandler(void){
	//check error
	//Disable the SPI (SPE=0)
	//Call transfer_error_cb_ if not null
	//Set transfer_error_cb_ to null
	if(READ_BIT(DMA2->LISR,DMA_LISR_TEIF2|DMA_LISR_DMEIF2)){
		SET_BIT(DMA2->LIFCR,DMA_LIFCR_CTEIF2|DMA_LIFCR_CDMEIF2);
		if((SPIUtil->transfer_error_cb_)!=(void*)0){
			SPIUtil->transfer_error_cb_();
		}
		SPIUtil->transfer_error_cb_ = (void*)0;
	}
	//Disable DMA Stream
	//Disable the SPI (SPE=0) and, eventually, enter the Halt mode (or disable the peripheral clock)
	//Reset SPI_CR2_RXDMAEN
	//Set busy to false
	CLEAR_BIT(SPI1->CR1,SPI_CR1_SPE);
	CLEAR_BIT(DMA2_Stream3->CR,DMA_SxCR_EN);
	CLEAR_BIT(DMA2_Stream2->CR,DMA_SxCR_EN);
	while(READ_BIT(SPI1->CR1,SPI_CR1_SPE)); //disable SPI
	while(READ_BIT(DMA2_Stream3->CR,DMA_SxCR_EN)); //when EN bits is cleared, DMA is ready to be configured
	while(READ_BIT(DMA2_Stream2->CR,DMA_SxCR_EN)); //when EN bits is cleared, DMA is ready to be configured
	
	CLEAR_BIT(SPI1->CR2,SPI_CR2_TXDMAEN);
	CLEAR_BIT(SPI1->CR2,SPI_CR2_RXDMAEN);
	
	SPIUtil->busy=false;
	CLEAR_BIT(SPI1->CR1,SPI_CR1_SSI);
}

void bsp_spi_register_transfer_complete_callback(void (*cb)(void)){
	SPIUtil->transfer_complete_cb_ = cb;
}

void bsp_spi_register_transfer_error_callback(void (*cb)(void)){
	SPIUtil->transfer_error_cb_ = cb;
}