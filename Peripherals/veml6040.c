/******************************** includes ********************************/
#include "veml6040.h"
/**************************************************************************/

////////////////////////////////////////////////////////////////////////
// i2c address
////////////////////////////////////////////////////////////////////////
#define VEML6040_ADDR 									0x10u

///////////////////////////////
//Command Code
//////////////////////////////
#define VEML6040_COMMAND_CONF   				0x00u
#define VEML6040_COMMAND_RED    				0x08u
#define VEML6040_COMMAND_GREEN 				  0x09u
#define VEML6040_COMMAND_BLUE   				0x0Au
#define VEML6040_COMMAND_WHITE  				0x0Bu

///////////////////////////////
//Config
//////////////////////////////
//Integration time, 0x00u-40ms
//									0x01u-80ms
//									0x02u-160ms
//									0x03u-320ms
//									0x04u-640ms
//									0x05u-1280ms
#define VEML6040_CONF_INTEGRATION_TIME  0x01u //higher, better sensitity but need to decrease sample rate
#define VEML6040_CONF_MODE							0x00u //auto
#define VEML6040_CONF_ENABLE						0x00u //enable
#define VEML6040_DUMMY									0x00u //Pass as upper data byte when configuring

void veml6040_init(void){
	
	uint8_t cmd[3] = {
	VEML6040_COMMAND_CONF,
	(VEML6040_CONF_INTEGRATION_TIME<<4)|(VEML6040_CONF_MODE<<1)|VEML6040_CONF_ENABLE,
	VEML6040_DUMMY
	};
	bsp_i2c_master_transfer(cmd,sizeof(cmd),VEML6040_ADDR);
	
}

void veml6040_get_data_raw(VEML6040RawData_TypeDef *datastruct){
	uint8_t bufR[2] = {0x00u,0x00u};
	uint8_t bufG[2] = {0x00u,0x00u};
	uint8_t bufB[2] = {0x00u,0x00u};
	
	uint8_t cmd[1] = {VEML6040_COMMAND_RED};
	bsp_i2c_master_receive_reg(bufR,sizeof(bufR),VEML6040_ADDR,cmd,sizeof(cmd));
	
	cmd[0] = VEML6040_COMMAND_GREEN;
	bsp_i2c_master_receive_reg(bufG,sizeof(bufG),VEML6040_ADDR,cmd,sizeof(cmd));
	
	cmd[0] = VEML6040_COMMAND_BLUE;
	bsp_i2c_master_receive_reg(bufB,sizeof(bufB),VEML6040_ADDR,cmd,sizeof(cmd));
	
	datastruct->RCount = (bufR[1]<<8)|bufR[0];
	datastruct->GCount = (bufG[1]<<8)|bufG[0];
	datastruct->BCount = (bufB[1]<<8)|bufB[0];
}