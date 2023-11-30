//$file${.::app.h} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: ColourDiffusal.qm
// File:  ${.::app.h}
//
// This code has been generated by QM 5.3.0 <www.state-machine.com/qm>.
// DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// This generated code is open source software: you can redistribute it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation.
//
// This code is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// NOTE:
// Alternatively, this generated code may be distributed under the terms
// of Quantum Leaps commercial licenses, which expressly supersede the GNU
// General Public License and are specifically designed for licensees
// interested in retaining the proprietary status of their code.
//
// Contact information:
// <www.state-machine.com/licensing>
// <info@state-machine.com>
//
//$endhead${.::app.h} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#ifndef APP_H
#define APP_H

/* a very simple Board Support Package (BSP) -------------------------------*/
#define BSP_TICKS_PER_SEC    OS_TICKS_PER_SEC /* number of clock ticks in a second */

/* define the event signals used in the application ------------------------*/
enum CentralSignals {
    UPDATESCREEN_SIG = Q_USER_SIG, /* offset the first signal by Q_USER_SIG */
    SENSOROBTAIN_SIG,
    SWITCHMODE_SIG,
    LEDTOGGLE_SIG,
    PAUSE_SIG,
    MAX_SIG /* keep last (he number of signals) */
};

//LED POSITIONS SETTING
//To get column start use (POS_X_MODE)
//To get column end use (POS_X_MODE)+DISPLAY_CHAR_OFFSET_X-1
//To get page start use DISPLAY_CHAR_OFFSET_Y*(POS_Y_MODE)
//To get page start end DISPLAY_CHAR_OFFSET_Y*(POS_Y_MODE)+1
#define DISPLAY_CHAR_OFFSET_Y       LED_FONT_16_CHAR_HEIGHT
#define DISPLAY_CHAR_OFFSET_X       LED_FONT_16_CHAR_WIDTH

#define R_POS_Y_MODE_0          		0u    //first page
#define R_POS_X_MODE_0          		48u   //starts from 47th column
#define R3_POS_Y_MODE_0         		0u   
#define R3_POS_X_MODE_0         		80u  
#define R2_POS_Y_MODE_0      				0u   
#define R2_POS_X_MODE_0      				96u  
#define R1_POS_Y_MODE_0      				0u   
#define R1_POS_X_MODE_0      				112u 
#define R_COLON_POS_Y_MODE_0				0u
#define R_COLON_POS_X_MODE_0 				64u

#define G_POS_Y_MODE_0              2u   	//third page
#define G_POS_X_MODE_0              48u  	//starts from 47th column
#define G3_POS_Y_MODE_0             2u   
#define G3_POS_X_MODE_0             80u  
#define G2_POS_Y_MODE_0             2u   
#define G2_POS_X_MODE_0             96u  
#define G1_POS_Y_MODE_0             2u  
#define G1_POS_X_MODE_0             112u 
#define G_COLON_POS_Y_MODE_0        2u
#define G_COLON_POS_X_MODE_0        64u

#define B_POS_Y_MODE_0              4u   	//fifth page
#define B_POS_X_MODE_0              48u  	//starts from 47th column
#define B3_POS_Y_MODE_0             4u   
#define B3_POS_X_MODE_0             80u  
#define B2_POS_Y_MODE_0             4u   
#define B2_POS_X_MODE_0             96u  
#define B1_POS_Y_MODE_0             4u   
#define B1_POS_X_MODE_0             112u 
#define B_COLON_POS_Y_MODE_0        4u
#define B_COLON_POS_X_MODE_0        64u

#define R_POS_Y_MODE_1              0u    //first page
#define R_POS_X_MODE_1              16u   //starts from 47th column
#define R5_POS_Y_MODE_1             0u   
#define R5_POS_X_MODE_1             48u  
#define R4_POS_Y_MODE_1             0u   
#define R4_POS_X_MODE_1             64u  
#define R3_POS_Y_MODE_1             0u   
#define R3_POS_X_MODE_1             80u  
#define R2_POS_Y_MODE_1             0u   
#define R2_POS_X_MODE_1             96u  
#define R1_POS_Y_MODE_1             0u   
#define R1_POS_X_MODE_1             112u 
#define R_COLON_POS_Y_MODE_1        0u
#define R_COLON_POS_X_MODE_1        32u

#define G_POS_Y_MODE_1              2u   	//third page
#define G_POS_X_MODE_1              16u  	//starts from 47th column
#define G5_POS_Y_MODE_1             2u   
#define G5_POS_X_MODE_1             48u  
#define G4_POS_Y_MODE_1             2u   
#define G4_POS_X_MODE_1             64u  
#define G3_POS_Y_MODE_1             2u   
#define G3_POS_X_MODE_1             80u  
#define G2_POS_Y_MODE_1             2u   
#define G2_POS_X_MODE_1             96u  
#define G1_POS_Y_MODE_1             2u  
#define G1_POS_X_MODE_1             112u 
#define G_COLON_POS_Y_MODE_1        2u
#define G_COLON_POS_X_MODE_1        32u

#define B_POS_Y_MODE_1       4u   	//fifth page
#define B_POS_X_MODE_1       16u  	//starts from 47th column 
#define B5_POS_Y_MODE_1      4u   
#define B5_POS_X_MODE_1      48u  
#define B4_POS_Y_MODE_1      4u   
#define B4_POS_X_MODE_1      64u 
#define B3_POS_Y_MODE_1      4u   
#define B3_POS_X_MODE_1      80u  
#define B2_POS_Y_MODE_1      4u   
#define B2_POS_X_MODE_1      96u  
#define B1_POS_Y_MODE_1      4u   
#define B1_POS_X_MODE_1      112u 
#define B_COLON_POS_Y_MODE_1 4u
#define B_COLON_POS_X_MODE_1 32u

#define HUEH_POS_Y		       6u   	//seventh page
#define HUEH_POS_X           16u  	//starts from 47th column
#define HUEU_POS_Y           6u   	
#define HUEU_POS_X           32u  	
#define HUEE_POS_Y           6u   
#define HUEE_POS_X           48u  
#define HUE3_POS_Y           6u   
#define HUE3_POS_X           80u  
#define HUE2_POS_Y           6u   
#define HUE2_POS_X           96u  
#define HUE1_POS_Y           6u   
#define HUE1_POS_X           112u 

#define HUE_COLON_POS_Y      6u   
#define HUE_COLON_POS_X      64u 

#define MODE_CLEAR_AREA_X_START 0u
#define MODE_CLEAR_AREA_X_END   127u
#define MODE_CLEAR_AREA_Y_START 0u
#define MODE_CLEAR_AREA_Y_END   5u

#define CAL_END_COLUMN(STARTCOL) STARTCOL+DISPLAY_CHAR_OFFSET_X-1

#define CAL_END_PAGE(STARTPAGE) (STARTPAGE*DISPLAY_CHAR_OFFSET_Y)+1

#endif /* APP_H */