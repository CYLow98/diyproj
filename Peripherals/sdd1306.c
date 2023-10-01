/******************************** includes ********************************/
#include "sdd1306.h"
/**************************************************************************/

////////////////////////////////////////////////////////////////////////
// i2c address
////////////////////////////////////////////////////////////////////////
#define OLED_ADDR 0x3Cu

////////////////////////////////////////////////////////////////////////
// Fundamental Command Table Page 28
////////////////////////////////////////////////////////////////////////
// 0x81 + 0-0xFF Contrast ... reset = 0x7F
#define OLED_SETCONTRAST                              		0x81u //1 data
#define OLED_SETCONTRAST_VAL                            	0x7Fu //1 data

// A4/A5 commands to resume displaying data
// A4 = Resume to RAM content display
#define OLED_DISPLAYALLONRESUME                       		0xA4u //0 data
// A5 = Set entire display on
#define OLED_DISPLAYALLONIGNORE                       		0xA5u //0 data

// 0xA6/A7 Normal 1=white 0=black Inverse 0=white  1=black
#define OLED_DISPLAYNORMAL                            		0xA6u //0 data
#define OLED_DISPLAYINVERT                            		0xA7u //0 data

// 0xAE/AF are a pair to turn screen off/on
#define OLED_DISPLAYOFF                               		0xAEu //0 data
#define OLED_DISPLAYON                                		0xAFu //0 data

////////////////////////////////////////////////////////////////////////
// Scrolling Settings pg 28-30
////////////////////////////////////////////////////////////////////////
#define OLED_DEACTIVATE_SCROLL 														0x2Eu //0 data
#define OLED_ACTIVATE_SCROLL 															0x2Fu //0 data
#define OLED_CONTINUOUS_HORIZONTAL_SCROLL_LEFT						0x27u //6 data
#define OLED_CONTINUOUS_HORIZONTAL_SCROLL_RIGHT						0x26u //6 data
#define OLED_CONTINUOUS_VERTICAL_HORIZONTAL_SCROLL_LEFT		0x2Au //5 data
#define OLED_CONTINUOUS_VERTICAL_HORIZONTAL_SCROLL_RIGHT	0x29u //5 data
#define OLED_CONTINUOUS_VERTICAL_SCROLL_AREA_SET					0xA3u //2 data

////////////////////////////////////////////////////////////////////////
// Address Setting Command Table pg 30-31
////////////////////////////////////////////////////////////////////////
// 00-0F - set lower nibble of page address
// 10-1F - set upper niddle of page address

// 0x20 + 0x00 = horizontal, 0x01 = vertical, 0x02 = page, 0x03 = invalid
#define OLED_SETMEMORYMODE                            		0x20u //1 data
#define OLED_SETMEMORYMODE_HORIZONTAL                 		0x00u
#define OLED_SETMEMORYMODE_VERTICAL                   		0x01u
#define OLED_SETMEMORYMODE_PAGE                       		0x02u

// Only used for horizonal or vertical address modes, valid column ranges 0-127
// 0x21 + startcol[0x00-0x3F] + endcol[0x00-0x3F]
#define OLED_SETCOLUMNADDR                            		0x21u //2 data

// Only used for horizonal or vertical address modes, valid page ranges 0-7
// 0x22 + startpage[0x00-0x07] + endpage[0x00-0x07]
#define OLED_SETPAGEADDR                              		0x22u //2 data

////////////////////////////////////////////////////////////////////////
// Hardware Configuration pg 31
////////////////////////////////////////////////////////////////////////
// 40-7F - set address startline from 0-127 (6-bits)
#define OLED_SETSTARTLINE_ZERO 														0x40u //0 data

// Y Direction 0xA0-idx 0 is SEG0 0xA1-idx 127 is SEG0
#define OLED_SEGREMAPNORMAL    														0xA0u //0 data
#define OLED_SEGREMAPINV       														0xA1u //0 data

// 0xA8, number of rows -1 ... e.g. 0xA8, 63
#define OLED_SETMULTIPLEX      														0xA8u //1 data
#define OLED_SETMULTIPLEX_VAL	 														0x3Fu

// X Direction
#define OLED_COMSCANINC        														0xC0u //0 data
#define OLED_COMSCANDEC        														0xC8u //0 data

// double byte with image wrap ...probably should be 0
#define OLED_SETDISPLAYOFFSET  														0xD3u //1 data
#define OLED_SETDISPLAYOFFSET_VAL													0x00u 

// legal values 0x02, 0x12, 0x22, 0x32
#define OLED_SETCOMPINS        														0xDAu //1 data
#define OLED_SETCOMPINS_VAL    														0x12u //1 data

////////////////////////////////////////////////////////////////////////
// Timing and Driving Scheme Settings pg 32
////////////////////////////////////////////////////////////////////////
//Alter framerate, use 0x80 for best performance
#define OLED_SETDISPLAYCLOCKDIV 													0xD5u //1 data
#define OLED_SETDISPLAYCLOCKDIV_VAL 											0x80u

#define OLED_SETPRECHARGE       													0xD9u //1 data
#define OLED_SETPRECHARGE_VAL    													0x22u 

#define OLED_SETVCOMDESELECT    													0xDBu //1 data
#define OLED_SETVCOMDESELECT_VAL    											0x20u //default

#define OLED_NOP                													0xE3u

#define OLED_CHARGEPUMP         													0x8Du //1 data
#define OLED_CHARGEPUMP_ON      													0x14u
#define OLED_CHARGEPUMP_OFF     													0x10u

void sdd1306_init(void){
	//by default the addressing mode is horizontal
	uint8_t initializeCmds[]={
				OLED_CONTROL_COMMAND_BYTE,//0x00
        //////// Fundamental Commands
        OLED_DISPLAYOFF,          
        OLED_SETCONTRAST,         
        OLED_SETCONTRAST_VAL,    
        OLED_DISPLAYNORMAL,       
        //////// Scrolling Commands
        OLED_DEACTIVATE_SCROLL,   
        //////// Addressing Commands
        OLED_SETMEMORYMODE,       
        OLED_SETMEMORYMODE_PAGE,  
        //////// Hardware Configuration Commands
        OLED_SEGREMAPNORMAL,       
        OLED_SETMULTIPLEX,       
        OLED_SETMULTIPLEX_VAL,    
        OLED_COMSCANINC,          
        OLED_SETDISPLAYOFFSET,    
        OLED_SETDISPLAYOFFSET_VAL,
        OLED_SETCOMPINS,          
        OLED_SETCOMPINS_VAL,      
   
        //////// Timing and Driving Settings
        OLED_SETDISPLAYCLOCKDIV,
        OLED_SETDISPLAYCLOCKDIV_VAL,
        OLED_SETPRECHARGE,      
        OLED_SETPRECHARGE_VAL, 
        OLED_SETVCOMDESELECT,  
        OLED_SETVCOMDESELECT_VAL,                    
        //////// Charge pump regulator
        OLED_CHARGEPUMP,          
        OLED_CHARGEPUMP_ON, 
        // Turn the screen back on...       
        OLED_DISPLAYALLONRESUME, 
        OLED_DISPLAYON, 
    };
	bsp_i2c_master_transfer(initializeCmds,sizeof(initializeCmds),OLED_ADDR);
		
	//clear the screen
		uint8_t temp[]= {
						OLED_CONTROL_COMMAND_BYTE,
            OLED_DISPLAYOFF,
            OLED_SETMEMORYMODE, 
            OLED_SETMEMORYMODE_HORIZONTAL, // Horizontal
            OLED_DISPLAYALLONRESUME,
            OLED_DISPLAYON,
            OLED_SETPAGEADDR, //Set page address range
            0x00u,
            0x07u,
            OLED_SETCOLUMNADDR, // column start and end address
            0x00u, //0
            0x7Fu, //127
    };
		bsp_i2c_master_transfer(temp, sizeof(temp), OLED_ADDR);
		
		    // Write twelve bytes onto screen with 0b10101010
		uint8_t dat[1025] = {0x00u};
		dat[0] = 0x40u;
		for (int i = 1;i<1025;i++){
			dat[i] = 0x00u;
		}
		bsp_i2c_master_transfer(dat,sizeof(dat),OLED_ADDR);
}

void sdd1306_draw(uint8_t startpage, uint8_t endpage, uint8_t startcolumn, uint8_t endcolumn, uint8_t *data, uint16_t datalen){
	//here it is assumed that the addressing mode is horizontal or vertical
	uint8_t cmd[] = {
		OLED_CONTROL_COMMAND_BYTE,
		OLED_SETCOLUMNADDR, //must set column first then page only for it to work
		startcolumn, 
		endcolumn,
		OLED_SETPAGEADDR,
		startpage,
		endpage
	};
	bsp_i2c_master_transfer(cmd,sizeof(cmd),OLED_ADDR);
	bsp_i2c_master_transfer(data,datalen,OLED_ADDR);
}