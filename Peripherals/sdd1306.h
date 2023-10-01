/******************************** includes ********************************/
#include "bsp_i2c.h"                  // i2c utilities
/**************************************************************************/

//These here so can be used by main
////////////////////////////////////////////////////////////////////////
// Control bytes to start i2c
////////////////////////////////////////////////////////////////////////
#define OLED_CONTROL_COMMAND_BYTE 												0x00u
#define OLED_CONTROL_DATA_BYTE 														0x40u

void sdd1306_init(void);
//horizontal mode, from top left and scan to right and bottom
void sdd1306_draw(uint8_t startpage, uint8_t endpage, uint8_t startcolumn, uint8_t endcolumn, uint8_t *data, uint16_t datalen);