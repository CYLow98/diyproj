/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* RTOS Related define ------------------------------------------------------------*/
#define PRIO_OLED_UPDATE 		 7 //highest priority
#define PRIO_COLOUR_SAMPLING 8
#define PRIO_VEML_LED_TOGGLE 9
#define TASK_STK_SIZE 512

#define PERIOD_OLED_UPDATE		 200 //OLED updates every 200ms
#define PERIOD_COLOUR_SAMPLING 250 //Colour updates every 250ms

/* Application Related define -----------------------------------------------------*/

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


/* Shared Variable ------------------------------------------------------------*/
volatile VEML6040RawData_TypeDef * RGBCount;
volatile bool ChangeMode = false; //when button is pressed clear LED, depending on mode the display will need to be reinitialised
OS_EVENT * i2cMutex;
OS_EVENT * RGBCountMutex;
OS_EVENT * pa6switchSem;

/* Private macro -------------------------------------------------------------*/
//For debugging
#define CE(x) if ((err = x) != OS_ERR_NONE) \
 printf("Runtime error: %d line %d - see ucos_ii.h\n", err, __LINE__);

#define CAL_END_COLUMN(STARTCOL) STARTCOL+DISPLAY_CHAR_OFFSET_X-1

#define CAL_END_PAGE(STARTPAGE) (STARTPAGE*DISPLAY_CHAR_OFFSET_Y)+1

/* Variables ----------------------------------------------------------------*/
OS_STK TaskStk_OLEDUpdate[TASK_STK_SIZE];
OS_STK TaskStk_ColourSampling[TASK_STK_SIZE];
OS_STK TaskStk_Veml_LedToggle[64];
uint8_t err;

/* uCosII task prototypes ---------------------------------------------------*/
void Task_OLEDUpdate(void *pdata);
void Task_ColourSampling(void *pdata);
void Task_Veml_LedToggle(void *pdata);
/* uCosII task definition ---------------------------------------------------*/
void Task_OLEDUpdate(void *pdata){
	uint16_t R=0;
	uint16_t G=0;
	uint16_t B=0;
	uint8_t Mode = 0; //0 - showing percentage, 1 - showing count
	uint8_t digitR[5] = {0}; // To store the individual digits to be displayed
	uint8_t digitG[5] = {0}; // To store the individual digits to be displayed
	uint8_t digitB[5] = {0}; // To store the individual digits to be displayed
	uint8_t digitHue[3] = {0}; // To store the individual digits to be displayed
	//wake up immediately copy RGB value over, then use them to do calculation and display
	
	OSMutexPend(i2cMutex,0,&err);CE(err);
	sdd1306_draw(R_POS_Y_MODE_0,CAL_END_PAGE(R_POS_Y_MODE_0),R_POS_X_MODE_0,CAL_END_COLUMN(R_POS_X_MODE_0),font16x16[15],LED_CHAR_16_SIZE);
	sdd1306_draw(R_COLON_POS_Y_MODE_0,CAL_END_PAGE(R_COLON_POS_Y_MODE_0),R_COLON_POS_X_MODE_0,CAL_END_COLUMN(R_COLON_POS_X_MODE_0),font16x16[10],LED_CHAR_16_SIZE);
	sdd1306_draw(G_POS_Y_MODE_0,CAL_END_PAGE(G_POS_Y_MODE_0),G_POS_X_MODE_0,CAL_END_COLUMN(G_POS_X_MODE_0),font16x16[14],LED_CHAR_16_SIZE);
	sdd1306_draw(G_COLON_POS_Y_MODE_0,CAL_END_PAGE(G_COLON_POS_Y_MODE_0),G_COLON_POS_X_MODE_0,CAL_END_COLUMN(G_COLON_POS_X_MODE_0),font16x16[10],LED_CHAR_16_SIZE);
	sdd1306_draw(B_POS_Y_MODE_0,CAL_END_PAGE(B_POS_Y_MODE_0),B_POS_X_MODE_0,CAL_END_COLUMN(B_POS_X_MODE_0),font16x16[11],LED_CHAR_16_SIZE);
	sdd1306_draw(B_COLON_POS_Y_MODE_0,CAL_END_PAGE(B_COLON_POS_Y_MODE_0),B_COLON_POS_X_MODE_0,CAL_END_COLUMN(B_COLON_POS_X_MODE_0),font16x16[10],LED_CHAR_16_SIZE);
	
	sdd1306_draw(HUEH_POS_Y,CAL_END_PAGE(HUEH_POS_Y),HUEH_POS_X,CAL_END_COLUMN(HUEH_POS_X),font16x16[13],LED_CHAR_16_SIZE);
	sdd1306_draw(HUEU_POS_Y,CAL_END_PAGE(HUEU_POS_Y),HUEU_POS_X,CAL_END_COLUMN(HUEU_POS_X),font16x16[16],LED_CHAR_16_SIZE);
	sdd1306_draw(HUEE_POS_Y,CAL_END_PAGE(HUEE_POS_Y),HUEE_POS_X,CAL_END_COLUMN(HUEE_POS_X),font16x16[12],LED_CHAR_16_SIZE);
	sdd1306_draw(HUE_COLON_POS_Y,CAL_END_PAGE(HUE_COLON_POS_Y),HUE_COLON_POS_X,CAL_END_COLUMN(HUE_COLON_POS_X),font16x16[10],LED_CHAR_16_SIZE);
	OSMutexPost(i2cMutex);
	
	while (1){
		OSMutexPend(RGBCountMutex,0,&err);CE(err);
		R = RGBCount->RCount;
		G = RGBCount->GCount;
		B = RGBCount->BCount;
		OSMutexPost(RGBCountMutex);
		
		if(ChangeMode){
			//Clear RGB and redraw RGB
			OSMutexPend(i2cMutex,0,&err);CE(err);
			sdd1306_clear_area(MODE_CLEAR_AREA_Y_START,MODE_CLEAR_AREA_Y_END,MODE_CLEAR_AREA_X_START,MODE_CLEAR_AREA_X_END);
			if(Mode == 0){
				Mode = 1;
				sdd1306_draw(R_POS_Y_MODE_1,CAL_END_PAGE(R_POS_Y_MODE_1),R_POS_X_MODE_1,CAL_END_COLUMN(R_POS_X_MODE_1),font16x16[15],LED_CHAR_16_SIZE);
				sdd1306_draw(R_COLON_POS_Y_MODE_1,CAL_END_PAGE(R_COLON_POS_Y_MODE_1),R_COLON_POS_X_MODE_1,CAL_END_COLUMN(R_COLON_POS_X_MODE_1),font16x16[10],LED_CHAR_16_SIZE);
				sdd1306_draw(G_POS_Y_MODE_1,CAL_END_PAGE(G_POS_Y_MODE_1),G_POS_X_MODE_1,CAL_END_COLUMN(G_POS_X_MODE_1),font16x16[14],LED_CHAR_16_SIZE);
				sdd1306_draw(G_COLON_POS_Y_MODE_1,CAL_END_PAGE(G_COLON_POS_Y_MODE_1),G_COLON_POS_X_MODE_1,CAL_END_COLUMN(G_COLON_POS_X_MODE_1),font16x16[10],LED_CHAR_16_SIZE);
				sdd1306_draw(B_POS_Y_MODE_1,CAL_END_PAGE(B_POS_Y_MODE_1),B_POS_X_MODE_1,CAL_END_COLUMN(B_POS_X_MODE_1),font16x16[11],LED_CHAR_16_SIZE);
				sdd1306_draw(B_COLON_POS_Y_MODE_1,CAL_END_PAGE(B_COLON_POS_Y_MODE_1),B_COLON_POS_X_MODE_1,CAL_END_COLUMN(B_COLON_POS_X_MODE_1),font16x16[10],LED_CHAR_16_SIZE);
			}else{
				Mode = 0;
				sdd1306_draw(R_POS_Y_MODE_0,CAL_END_PAGE(R_POS_Y_MODE_0),R_POS_X_MODE_0,CAL_END_COLUMN(R_POS_X_MODE_0),font16x16[15],LED_CHAR_16_SIZE);
				sdd1306_draw(R_COLON_POS_Y_MODE_0,CAL_END_PAGE(R_COLON_POS_Y_MODE_0),R_COLON_POS_X_MODE_0,CAL_END_COLUMN(R_COLON_POS_X_MODE_0),font16x16[10],LED_CHAR_16_SIZE);
				sdd1306_draw(G_POS_Y_MODE_0,CAL_END_PAGE(G_POS_Y_MODE_0),G_POS_X_MODE_0,CAL_END_COLUMN(G_POS_X_MODE_0),font16x16[14],LED_CHAR_16_SIZE);
				sdd1306_draw(G_COLON_POS_Y_MODE_0,CAL_END_PAGE(G_COLON_POS_Y_MODE_0),G_COLON_POS_X_MODE_0,CAL_END_COLUMN(G_COLON_POS_X_MODE_0),font16x16[10],LED_CHAR_16_SIZE);
				sdd1306_draw(B_POS_Y_MODE_0,CAL_END_PAGE(B_POS_Y_MODE_0),B_POS_X_MODE_0,CAL_END_COLUMN(B_POS_X_MODE_0),font16x16[11],LED_CHAR_16_SIZE);
				sdd1306_draw(B_COLON_POS_Y_MODE_0,CAL_END_PAGE(B_COLON_POS_Y_MODE_0),B_COLON_POS_X_MODE_0,CAL_END_COLUMN(B_COLON_POS_X_MODE_0),font16x16[10],LED_CHAR_16_SIZE);
			}
			OSMutexPost(i2cMutex); //put it here just in case the screen could go flickering
			
			ChangeMode = false;
		}
		
		if(Mode==0){
			uint32_t total = R+B+G;
			uint8_t percR = (R*100+(total>>1))/total;
			uint8_t percG = (G*100+(total>>1))/total;
			uint8_t percB = (B*100+(total>>1))/total;
			
			digitR[0] =  percR % 10;	// Ones place
			digitR[1] = (percR / 10) % 10; // Tens place 
			digitR[2] = (percR / 100) % 10; // Hundreds place
			
			digitG[0] =  percG % 10;	// Ones place
			digitG[1] = (percG / 10) % 10; // Tens place 
			digitG[2] = (percG / 100) % 10; // Hundreds place 
			
			digitB[0] =  percB % 10;	// Ones place 
			digitB[1] = (percB / 10) % 10; // Tens place 
			digitB[2] = (percB / 100) % 10; // Hundreds place 
			OSMutexPend(i2cMutex,0,&err);CE(err);
			sdd1306_draw(R1_POS_Y_MODE_0,CAL_END_PAGE(R1_POS_Y_MODE_0),R1_POS_X_MODE_0,CAL_END_COLUMN(R1_POS_X_MODE_0),font16x16[digitR[0]],LED_CHAR_16_SIZE);
			sdd1306_draw(R2_POS_Y_MODE_0,CAL_END_PAGE(R2_POS_Y_MODE_0),R2_POS_X_MODE_0,CAL_END_COLUMN(R2_POS_X_MODE_0),font16x16[digitR[1]],LED_CHAR_16_SIZE);
			sdd1306_draw(R3_POS_Y_MODE_0,CAL_END_PAGE(R3_POS_Y_MODE_0),R3_POS_X_MODE_0,CAL_END_COLUMN(R3_POS_X_MODE_0),font16x16[digitR[2]],LED_CHAR_16_SIZE);
			
			sdd1306_draw(G1_POS_Y_MODE_0,CAL_END_PAGE(G1_POS_Y_MODE_0),G1_POS_X_MODE_0,CAL_END_COLUMN(G1_POS_X_MODE_0),font16x16[digitG[0]],LED_CHAR_16_SIZE);
			sdd1306_draw(G2_POS_Y_MODE_0,CAL_END_PAGE(G2_POS_Y_MODE_0),G2_POS_X_MODE_0,CAL_END_COLUMN(G2_POS_X_MODE_0),font16x16[digitG[1]],LED_CHAR_16_SIZE);
			sdd1306_draw(G3_POS_Y_MODE_0,CAL_END_PAGE(G3_POS_Y_MODE_0),G3_POS_X_MODE_0,CAL_END_COLUMN(G3_POS_X_MODE_0),font16x16[digitG[2]],LED_CHAR_16_SIZE);
		
			sdd1306_draw(B1_POS_Y_MODE_0,CAL_END_PAGE(B1_POS_Y_MODE_0),B1_POS_X_MODE_0,CAL_END_COLUMN(B1_POS_X_MODE_0),font16x16[digitB[0]],LED_CHAR_16_SIZE);
			sdd1306_draw(B2_POS_Y_MODE_0,CAL_END_PAGE(B2_POS_Y_MODE_0),B2_POS_X_MODE_0,CAL_END_COLUMN(B2_POS_X_MODE_0),font16x16[digitB[1]],LED_CHAR_16_SIZE);
			sdd1306_draw(B3_POS_Y_MODE_0,CAL_END_PAGE(B3_POS_Y_MODE_0),B3_POS_X_MODE_0,CAL_END_COLUMN(B3_POS_X_MODE_0),font16x16[digitB[2]],LED_CHAR_16_SIZE);
			OSMutexPost(i2cMutex);
		}else{
			digitR[0] =  R % 10;	// Ones placeX
			digitR[1] = (R / 10) % 10; // Tens place 
			digitR[2] = (R / 100) % 10; // Hundreds place 
			digitR[3] = (R / 1000) % 10; // Thousands place 
			digitR[4] =  R / 10000; // Ten-Thousand place 
			
			digitG[0] =  G % 10;	// Ones place 
			digitG[1] = (G / 10) % 10; // Tens place 
			digitG[2] = (G / 100) % 10; // Hundreds place 
			digitG[3] = (G / 1000) % 10; // Thousands place 
			digitG[4] =  G / 10000; // Ten-Thousand place 
			
			digitB[0] =  B % 10;	// Ones place 
			digitB[1] = (B / 10) % 10; // Tens place 
			digitB[2] = (B / 100) % 10; // Hundreds place 
			digitB[3] = (B / 1000) % 10; // Thousands place 
			digitB[4] =  B / 10000; // Ten-Thousand place 
			
			OSMutexPend(i2cMutex,0,&err);CE(err);
			sdd1306_draw(R1_POS_Y_MODE_1,CAL_END_PAGE(R1_POS_Y_MODE_1),R1_POS_X_MODE_1,CAL_END_COLUMN(R1_POS_X_MODE_1),font16x16[digitR[0]],LED_CHAR_16_SIZE);
			sdd1306_draw(R2_POS_Y_MODE_1,CAL_END_PAGE(R2_POS_Y_MODE_1),R2_POS_X_MODE_1,CAL_END_COLUMN(R2_POS_X_MODE_1),font16x16[digitR[1]],LED_CHAR_16_SIZE);
			sdd1306_draw(R3_POS_Y_MODE_1,CAL_END_PAGE(R3_POS_Y_MODE_1),R3_POS_X_MODE_1,CAL_END_COLUMN(R3_POS_X_MODE_1),font16x16[digitR[2]],LED_CHAR_16_SIZE);
			sdd1306_draw(R4_POS_Y_MODE_1,CAL_END_PAGE(R4_POS_Y_MODE_1),R4_POS_X_MODE_1,CAL_END_COLUMN(R4_POS_X_MODE_1),font16x16[digitR[3]],LED_CHAR_16_SIZE);
			sdd1306_draw(R5_POS_Y_MODE_1,CAL_END_PAGE(R5_POS_Y_MODE_1),R5_POS_X_MODE_1,CAL_END_COLUMN(R5_POS_X_MODE_1),font16x16[digitR[4]],LED_CHAR_16_SIZE);

			sdd1306_draw(G1_POS_Y_MODE_1,CAL_END_PAGE(G1_POS_Y_MODE_1),G1_POS_X_MODE_1,CAL_END_COLUMN(G1_POS_X_MODE_1),font16x16[digitG[0]],LED_CHAR_16_SIZE);
			sdd1306_draw(G2_POS_Y_MODE_1,CAL_END_PAGE(G2_POS_Y_MODE_1),G2_POS_X_MODE_1,CAL_END_COLUMN(G2_POS_X_MODE_1),font16x16[digitG[1]],LED_CHAR_16_SIZE);
			sdd1306_draw(G3_POS_Y_MODE_1,CAL_END_PAGE(G3_POS_Y_MODE_1),G3_POS_X_MODE_1,CAL_END_COLUMN(G3_POS_X_MODE_1),font16x16[digitG[2]],LED_CHAR_16_SIZE);
			sdd1306_draw(G4_POS_Y_MODE_1,CAL_END_PAGE(G4_POS_Y_MODE_1),G4_POS_X_MODE_1,CAL_END_COLUMN(G4_POS_X_MODE_1),font16x16[digitG[3]],LED_CHAR_16_SIZE);
			sdd1306_draw(G5_POS_Y_MODE_1,CAL_END_PAGE(G5_POS_Y_MODE_1),G5_POS_X_MODE_1,CAL_END_COLUMN(G5_POS_X_MODE_1),font16x16[digitG[4]],LED_CHAR_16_SIZE);
		
			sdd1306_draw(B1_POS_Y_MODE_1,CAL_END_PAGE(B1_POS_Y_MODE_1),B1_POS_X_MODE_1,CAL_END_COLUMN(B1_POS_X_MODE_1),font16x16[digitB[0]],LED_CHAR_16_SIZE);
			sdd1306_draw(B2_POS_Y_MODE_1,CAL_END_PAGE(B2_POS_Y_MODE_1),B2_POS_X_MODE_1,CAL_END_COLUMN(B2_POS_X_MODE_1),font16x16[digitB[1]],LED_CHAR_16_SIZE);
			sdd1306_draw(B3_POS_Y_MODE_1,CAL_END_PAGE(B3_POS_Y_MODE_1),B3_POS_X_MODE_1,CAL_END_COLUMN(B3_POS_X_MODE_1),font16x16[digitB[2]],LED_CHAR_16_SIZE);
			sdd1306_draw(B4_POS_Y_MODE_1,CAL_END_PAGE(B4_POS_Y_MODE_1),B4_POS_X_MODE_1,CAL_END_COLUMN(B4_POS_X_MODE_1),font16x16[digitB[3]],LED_CHAR_16_SIZE);
			sdd1306_draw(B5_POS_Y_MODE_1,CAL_END_PAGE(B5_POS_Y_MODE_1),B5_POS_X_MODE_1,CAL_END_COLUMN(B5_POS_X_MODE_1),font16x16[digitB[4]],LED_CHAR_16_SIZE);
			OSMutexPost(i2cMutex);
		}
		
		//https://changingminds.org/explanations/perception/visual/hsl.html
		int32_t RNorm = (R<<12)/65535; //0.12
    int32_t GNorm = (G<<12)/65535; //0.12
    int32_t BNorm = (B<<12)/65535; //0.12
		
		uint8_t MaxRGB = 0;
		uint32_t Cmin = RNorm; //0.12
    uint32_t Cmax = RNorm; //0.12
		if(GNorm<Cmin){
			Cmin=GNorm;
		}else if(GNorm>Cmax){
			Cmax=GNorm;
			MaxRGB=1;
		}
		
		if(BNorm<Cmin){
			Cmin=BNorm;
		}else if(BNorm>Cmax){
			Cmax=BNorm;
			MaxRGB=2;
		}
		int32_t delta = Cmax-Cmin; //0.12
		int32_t hue = 0;
		if(MaxRGB==0){ //red is max
				hue = 60*(((((GNorm<<8)-(BNorm<<8))%(6<<20))+(delta>>1))/delta); //intermediate cast to prevent conversion rule messing up the result
		}else if(MaxRGB==1){ //green is max
				hue = 60*((2<<8)+(((BNorm<<8)-(RNorm<<8))+(delta>>1))/delta);
		}else{ //blue is max
				hue = 60*((4<<8)+(((RNorm<<8)-(GNorm<<8))+(delta>>1))/delta);
		}
		//hue is 0.8
		if(hue<0){
				hue = hue +(360<<8);
		}
		hue = hue >> 8;
		
		digitHue[0] =  hue % 10;	// Ones place
		digitHue[1] = (hue / 10) % 10; // Tens place 
		digitHue[2] = (hue / 100) % 10; // Hundreds place
		
		OSMutexPend(i2cMutex,0,&err);CE(err);
		sdd1306_draw(HUE1_POS_Y,CAL_END_PAGE(HUE1_POS_Y),HUE1_POS_X,CAL_END_COLUMN(HUE1_POS_X),font16x16[digitHue[0]],LED_CHAR_16_SIZE);
		sdd1306_draw(HUE2_POS_Y,CAL_END_PAGE(HUE2_POS_Y),HUE2_POS_X,CAL_END_COLUMN(HUE2_POS_X),font16x16[digitHue[1]],LED_CHAR_16_SIZE);
		sdd1306_draw(HUE3_POS_Y,CAL_END_PAGE(HUE3_POS_Y),HUE3_POS_X,CAL_END_COLUMN(HUE3_POS_X),font16x16[digitHue[2]],LED_CHAR_16_SIZE);
		OSMutexPost(i2cMutex);
	
		OSTimeDly(PERIOD_OLED_UPDATE);
	}
}

void Task_ColourSampling(void *pdata){
	//periodically call VEML6040 utilities functions to get latest colour sensor data
	while (1){
		OSMutexPend(RGBCountMutex,0,&err);CE(err)
		OSMutexPend(i2cMutex,0,&err);CE(err);
		veml6040_get_data_raw(RGBCount);
		OSMutexPost(i2cMutex);
		OSMutexPost(RGBCountMutex);
		OSTimeDly(PERIOD_COLOUR_SAMPLING);
	}
}

void Task_Veml_LedToggle(void *pdata){
	uint8_t state = 1; //0 - off, 1 - on
	bsp_gpio_PA7_set();
	while(1){
		OSSemPend(pa6switchSem,0,&err);
		if(state==1){
			bsp_gpio_PA7_reset();
			state=0;
		}else{
			bsp_gpio_PA7_set();
			state=1;
		}
	}
	
}
int main(void){
	bsp_system_init();
	bsp_gpio_user_key_init();
	bsp_gpio_PA7_init();
	bsp_gpio_PA6_init();
	bsp_i2c_init();
	
	veml6040_init();
	
	sdd1306_init();
	
	OSInit();
	//Initialise variables
	i2cMutex = OSMutexCreate(4,&err); CE(err);
	RGBCountMutex = OSMutexCreate(5,&err); CE(err);
	pa6switchSem = OSSemCreate(0);
	RGBCount = (VEML6040RawData_TypeDef*)malloc(sizeof(VEML6040RawData_TypeDef));
	RGBCount->RCount = 0;
	RGBCount->GCount = 0;
	RGBCount->BCount = 0;
	
	//Create tasks
	OSTaskCreateExt(&Task_OLEDUpdate,
	NULL,
	&TaskStk_OLEDUpdate[TASK_STK_SIZE-1],
	PRIO_OLED_UPDATE,
	PRIO_OLED_UPDATE,
	TaskStk_OLEDUpdate,
	TASK_STK_SIZE,
	NULL,
	NULL);
	
	OSTaskCreateExt(&Task_ColourSampling,
	NULL,
	&TaskStk_ColourSampling[TASK_STK_SIZE-1],
	PRIO_COLOUR_SAMPLING,
	PRIO_COLOUR_SAMPLING,
	TaskStk_ColourSampling,
	TASK_STK_SIZE,
	NULL,
	NULL);
	
	OSTaskCreateExt(&Task_Veml_LedToggle,
	NULL,
	&TaskStk_Veml_LedToggle[63],
	PRIO_VEML_LED_TOGGLE,
	PRIO_VEML_LED_TOGGLE,
	TaskStk_Veml_LedToggle,
	63,
	NULL,
	NULL);
	
	//Start OS
	OSStart(); 
}

bool DebounceSwitch()
{
    static uint16_t State = 0; // Current debounce status
    State=(State<<1) | !bsp_gpio_user_key_get_state() | 0xe000;
    if(State==0xf000)return true;
    return false;
}

bool DebounceSwitch2(){
	  static uint16_t State = 0; // Current debounce status
    State=(State<<1) | !bsp_gpio_PA6_get_state() | 0xe000;
    if(State==0xf000)return true;
    return false;
}

#if OS_TIME_TICK_HOOK_EN > 0
void  App_TimeTickHook (void)
{
	if(DebounceSwitch()){
		ChangeMode = true;
	}
	if(DebounceSwitch2()){
		OSSemPost(pa6switchSem);
	}
}
#endif



void Error_Handler(void){
	while (1)
  {
  }
}