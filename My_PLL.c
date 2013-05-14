/*  ============================================================================
 *   Author: Nathan Zorndorf 
 *   Description: Setting up the PLL 
 */

#include <stdio.h>
#include <my_types.h>
#include <usbstk5505.h>
#include <aic3204.h>
#include <PLL.h>
#include <stereo.h>
#include <usbstk5505_gpio.h>
#include <usbstk5505_i2c.h>
#include <csl_general.h>
#include <csl_pll.h>
#include <csl_pllAux.h>
#include <Application_1_Modified_Registers.h>

#define PLL_CNTL1        *(ioport volatile unsigned *)0x1C20    // PLL Control Register #1
#define PLL_CNTL2        *(ioport volatile unsigned *)0x1C21 
#define PLL_CNTL3        *(ioport volatile unsigned *)0x1C22 
#define PLL_CNTL4        *(ioport volatile unsigned *)0x1C23

int My_PLL(void) {

	Uint16	i=0;
	PLL_Obj pllObj;
	PLL_Config pllCfg_100MHz    = {0x8BE8, 0x8000, 0x0806, 0x0000}; 
	/* 1st Hex Value : CGCR1 = b12 = PLL power UP/DOWN. b11-0 = M. PLL Multiplier value = M + 4.
	 * 2nd Hex Value : CGCR2 - b15 = Reference divider ENABLED/BYPASS. b11-0 = DRATIO => Divider Value = DRATIO + 4.
	 * 3rd Hex Value : CGCR3 - Must be 0x0806 during initialization for proper user. 
	 * 4th Hex Value : CGCR4 -  b9 = Output divider BYPASSED/ENABLED. b7-0 = ODRATIO  => Divider Value = ODRATIO + 1.  */ 
	PLL_Config *pConfigInfo;
	PLL_Handle hPll;
	CSL_Status status;
	
//---------- PLL SETUP BEGIN -----------
    printf("PLL SETUP BEGIN! \n");
   
    // Enable clocks to all peripherals 
    SYS_PCGCR1 = 0x0000;
    SYS_PCGCR2 = 0x0000;
    
    // External Bus Selection Register 
	CPU_EBSR = 0x1000; // Mode 1 (SPI, GPIO, UART, and I2S2) - ?
	
    /* CGCR1 - Section 1.4.4.1 */
    /* PLL power up. PLL Multiplier M = 1000 */
    PLL_CNTL1 = 0x8BE8; //PG1.4: 0x82FA;
	/* CGCR2 - Section 1.4.4.2 */
    PLL_CNTL2 = 0x8000; /* Bypass reference divider */
    /* CGCR3 - Section 1.4.4.3 */
    PLL_CNTL3 = 0x0806; /* initialization bits */
    /* CGCR4 - Section 1.4.4.4 */
    PLL_CNTL4 = 0x0000; /* Bypass output divider */
    
	// Peripheral Reset
	CPU_PSRCR = 0x0008; // Software reset signals asserted after 2 clock cycles
	CPU_PRCR = 0x00FF; // Reset all peripherals. 
    for (i=0; i< 0xFFFF; i++); // Wait in for loop to give time for peripherals to reset. 
    /*
    status = PLL_init(&pllObj, CSL_PLL_INST_0); 
    
	hPll = (PLL_Handle)(&pllObj); 
	
	PLL_reset(hPll); 

    pConfigInfo = &pllCfg_100MHz; 

    status = PLL_config (hPll, pConfigInfo);
    if(CSL_SOK != status) { printf("PLL config failed\n"); return(status); }

    status = PLL_enable(hPll); 
    if(CSL_SOK != status) { printf("PLL enable failed:%d\n",CSL_ESYS_BADHANDLE); return(status); } 
  */
//---------- PLL SETUP END -------------
    printf("PLL SETUP END \n");
    printf("REGISTER          --- CONFIG VALUES\n");
	printf("PLL_CGCR1 		  --- 0x%X \n", PLL_CGCR1); 
	printf("PLL_CGCR2 		  --- 0x%X \n", PLL_CGCR2); 
	printf("PLL_CGCR3 		  --- 0x%X \n", PLL_CGCR3);    
	printf("PLL_CGCR4 		  --- 0x%X \n", PLL_CGCR4); 
	printf("PLL_CCR2  		  --- 0x%X \n", PLL_CCR2); 		
	
	return(0);
}
