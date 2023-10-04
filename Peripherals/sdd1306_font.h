#ifndef __SDD1306_FONT_H
#define __SDD1306_FONT_H

#include <stdint.h>
#include "sdd1306.h"

#define LED_FONT_16_CHAR_HEIGHT  2    //number of pages
#define LED_FONT_16_CHAR_WIDTH   16   //number of pixels

#define LED_CHAR_16_SIZE LED_FONT_16_CHAR_HEIGHT*LED_FONT_16_CHAR_WIDTH+1 //+1 because also need to include control data byte

const static uint8_t font16x16[][33] = {
	{ //0 //0
	SDD1306_CONTROL_DATA_BYTE, 
	0x00u, 0x00u, 0x00u, 0xE0u, 0xF8u, 0xF8u, 0x1Cu, 0x0Cu, 0x0Cu, 0x1Cu, 0xFCu, 0xF8u, 0xE0u, 0x00u, 0x00u, 0x00u,
	0x00u, 0x00u, 0x00u, 0x03u, 0x0Fu, 0x0Fu, 0x1Cu, 0x18u, 0x18u, 0x1Cu, 0x0Fu, 0x0Fu, 0x03u, 0x00u, 0x00u, 0x00u
	},
	{ //1 //1
	SDD1306_CONTROL_DATA_BYTE,
	0x00u, 0x00u, 0x00u, 0x30u, 0x30u, 0x38u, 0xFCu, 0xFCu, 0xFCu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
	0x00u, 0x00u, 0x00u, 0x18u, 0x18u, 0x18u, 0x1Fu, 0x1Fu, 0x1Fu, 0x18u, 0x18u, 0x18u, 0x00u, 0x00u, 0x00u, 0x00u
	},
	{ //2 //2
	SDD1306_CONTROL_DATA_BYTE,
	0x00u, 0x00u, 0x00u, 0x10u, 0x18u, 0x18u, 0x8Cu, 0x8Cu, 0x8Cu, 0xCCu, 0xFCu, 0x78u, 0x38u, 0x00u, 0x00u, 0x00u,
	0x00u, 0x00u, 0x00u, 0x1Cu, 0x1Eu, 0x1Fu, 0x1Bu, 0x19u, 0x19u, 0x18u, 0x18u, 0x18u, 0x18u, 0x00u, 0x00u, 0x00u,
	},
	{ //3 //3
	SDD1306_CONTROL_DATA_BYTE,
	0x00u, 0x00u, 0x00u, 0x00u, 0x18u, 0x18u, 0xCCu, 0xCCu, 0xCCu, 0xECu, 0xFCu, 0x78u, 0x70u, 0x00u, 0x00u, 0x00u,
	0x00u, 0x00u, 0x00u, 0x00u, 0x18u, 0x18u, 0x30u, 0x30u, 0x30u, 0x39u, 0x3Fu, 0x1Fu, 0x0Eu, 0x00u, 0x00u, 0x00u,
	},
	{ //4 //4
	SDD1306_CONTROL_DATA_BYTE,
	0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u, 0xC0u, 0x60u, 0x30u, 0x18u, 0xFCu, 0xFCu, 0xFCu, 0x00u, 0x00u, 0x00u,
	0x00u, 0x00u, 0x00u, 0x03u, 0x03u, 0x03u, 0x03u, 0x03u, 0x03u, 0x03u, 0x1Fu, 0x1Fu, 0x1Fu, 0x03u, 0x03u, 0x00u,
	},
	{ //5 //5
	SDD1306_CONTROL_DATA_BYTE,
	0x00u, 0x00u, 0x00u, 0xFCu, 0xFCu, 0xFCu, 0xECu, 0x6Cu, 0x6Cu, 0x6Cu, 0xECu, 0xCCu, 0x8Cu, 0x00u, 0x00u, 0x00u,
	0x00u, 0x00u, 0x00u, 0x19u, 0x19u, 0x31u, 0x30u, 0x30u, 0x30u, 0x38u, 0x1Fu, 0x1Fu, 0x07u, 0x00u, 0x00u, 0x00u
	},
	{ //6 //6
	SDD1306_CONTROL_DATA_BYTE,
	0x00u, 0x00u, 0x00u, 0x80u, 0xE0u, 0xF0u, 0xF8u, 0xDCu, 0xDCu, 0xCCu, 0xCCu, 0x80u, 0x00u, 0x00u, 0x00u, 0x00u,
	0x00u, 0x00u, 0x00u, 0x0Fu, 0x1Fu, 0x3Fu, 0x38u, 0x30u, 0x30u, 0x38u, 0x1Fu, 0x1Fu, 0x0Fu, 0x00u, 0x00u, 0x00u
	},
	{ //7 //7
	SDD1306_CONTROL_DATA_BYTE,
	0x00u, 0x00u, 0x0Cu, 0x0Cu, 0x0Cu, 0x0Cu, 0x8Cu, 0xCCu, 0xFCu, 0xFCu, 0x3Cu, 0x1Cu, 0x0Cu, 0x00u, 0x00u, 0x00u,
	0x00u, 0x00u, 0x00u, 0x00u, 0x18u, 0x1Eu, 0x1Fu, 0x0Fu, 0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
	},
	{ //8 //8
	SDD1306_CONTROL_DATA_BYTE,
	0x00u, 0x00u, 0x00u, 0x00u, 0xF0u, 0xF8u, 0xFCu, 0xCCu, 0xCCu, 0xFCu, 0xFCu, 0xF8u, 0x00u, 0x00u, 0x00u, 0x00u,
	0x00u, 0x00u, 0x00u, 0x0Fu, 0x1Fu, 0x3Fu, 0x31u, 0x30u, 0x30u, 0x31u, 0x3Fu, 0x1Fu, 0x0Fu, 0x00u, 0x00u, 0x00u
	},
	{ //9 //9
	SDD1306_CONTROL_DATA_BYTE,
	0x00u, 0x00u, 0x00u, 0x70u, 0xF8u, 0xFCu, 0x8Cu, 0x8Cu, 0x8Cu, 0x9Cu, 0xFCu, 0xF8u, 0xF0u, 0x00u, 0x00u, 0x00u,
	0x00u, 0x00u, 0x00u, 0x00u, 0x18u, 0x19u, 0x19u, 0x0Du, 0x0Du, 0x0Fu, 0x07u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u
	},
	{ //: //10
	SDD1306_CONTROL_DATA_BYTE,
	0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x60u, 0x60u, 0x60u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
	0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x06u, 0x06u, 0x06u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u
	},
	{ //B //11
	SDD1306_CONTROL_DATA_BYTE,
	0x00u, 0x00u, 0xFCu, 0xFCu, 0xFCu, 0x8Cu, 0x8Cu, 0x8Cu, 0xDCu, 0xF8u, 0xF8u, 0x70u, 0x00u, 0x00u, 0x00u, 0x00u,
	0x00u, 0x00u, 0x1Fu, 0x1Fu, 0x1Fu, 0x19u, 0x19u, 0x19u, 0x19u, 0x0Fu, 0x0Fu, 0x0Fu, 0x06u, 0x00u, 0x00u, 0x00u
	},
	{ //E //12
	SDD1306_CONTROL_DATA_BYTE,
	0x00u, 0x00u, 0x00u, 0xFCu, 0xFCu, 0xCCu, 0xCCu, 0xCCu, 0xCCu, 0xCCu, 0xC4u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
	0x00u, 0x00u, 0x00u, 0x0Fu, 0x1Fu, 0x18u, 0x18u, 0x18u, 0x18u, 0x18u, 0x08u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u
	},
	{ //H //13
	SDD1306_CONTROL_DATA_BYTE,
	0x00u, 0x00u, 0xFCu, 0xFCu, 0xC0u, 0xC0u, 0xC0u, 0xC0u, 0xC0u, 0xC0u, 0xC0u, 0xFCu, 0xFCu, 0x00u, 0x00u, 0x00u,
	0x00u, 0x00u, 0x1Fu, 0x1Fu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x1Fu, 0x1Fu, 0x00u, 0x00u, 0x00u
	},
	{ //G //14
	SDD1306_CONTROL_DATA_BYTE,
	0x00u, 0x80u, 0xE0u, 0xF0u, 0x78u, 0x38u, 0x9Cu, 0x8Cu, 0x8Cu, 0x8Cu, 0x9Cu, 0x98u, 0x98u, 0x80u, 0x00u, 0x00u,
	0x00u, 0x07u, 0x0Fu, 0x1Fu, 0x18u, 0x19u, 0x19u, 0x19u, 0x1Du, 0x0Du, 0x0Fu, 0x07u, 0x03u, 0x00u, 0x00u, 0x00u
	},
	{ //R //15
	SDD1306_CONTROL_DATA_BYTE,
	0x00u, 0x00u, 0x00u, 0xFCu, 0xFCu, 0x0Cu, 0x0Cu, 0x0Cu, 0x18u, 0xB8u, 0xF0u, 0xE0u, 0x00u, 0x00u, 0x00u, 0x00u,
	0x00u, 0x00u, 0x00u, 0x1Fu, 0x1Fu, 0x03u, 0x07u, 0x07u, 0x0Fu, 0x1Du, 0x1Du, 0x18u, 0x18u, 0x00u, 0x00u, 0x00u
	},
	{ //U //16
	SDD1306_CONTROL_DATA_BYTE,
	0x00u, 0x00u, 0xFCu, 0xFCu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xFCu, 0xFCu, 0x00u, 0x00u, 0x00u,
	0x00u, 0x00u, 0x03u, 0x0Fu, 0x1Cu, 0x18u, 0x18u, 0x18u, 0x18u, 0x0Cu, 0x0Fu, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u
	},
	{ //space //17
	SDD1306_CONTROL_DATA_BYTE,
	0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
	0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u
	}
};

#endif