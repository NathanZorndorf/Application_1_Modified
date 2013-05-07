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

/* ------------------------------------------------------------------------ *
 *  PLL                                                           *
 * ------------------------------------------------------------------------ */
#define PLL_CCR2  *(volatile ioport Uint16*)(0x1C1F)
#define PLL_CGCR1 *(volatile ioport Uint16*)(0x1C20)
#define PLL_CGCR2 *(volatile ioport Uint16*)(0x1C21)
#define PLL_CGCR3 *(volatile ioport Uint16*)(0x1C22)
#define PLL_CGCR4 *(volatile ioport Uint16*)(0x1C23)

// CPU Registers
#define CPU_EBSR          	   *(volatile ioport Uint16*)(0x1C00) // External Bus Selection Register
#define CPU_PSRCR			   *(volatile ioport Uint16*)(0x1C04) // Peripheral Software Reset Counter Register
#define CPU_PRCR			   *(volatile ioport Uint16*)(0x1C05) // Peripheral Software Reset Counter Register

int My_PLL(void) {


	PLL_Obj pllObj;
	PLL_Config pllCfg1;
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
    SYS_PCGCR1 = 0x0000; // - OK
    SYS_PCGCR2 = 0x0000; // - OK
    
    // External Bus Selection Register 
	CPU_EBSR = 0x1000; // Mode 1 (SPI, GPIO, UART, and I2S2) - ?
	printf("EBSR = 0x%X\n",CPU_EBSR);
	
	// Peripheral Reset
	CPU_PSRCR = 0x0008; 
	CPU_PRCR = 0x00FF; 
	
    status = PLL_init(&pllObj, CSL_PLL_INST_0); // - OK
    
	hPll = (PLL_Handle)(&pllObj); // - OK
	
	PLL_reset(hPll); // - OK

    pConfigInfo = &pllCfg_100MHz; // - OK
    printf("\nPLL frequency struct set.\n");    

    // PLL Frequency Setup 
    status = PLL_config (hPll, pConfigInfo);
    if(CSL_SOK != status) { printf("PLL config failed\n"); return(status); }

	status = PLL_getConfig(hPll, &pllCfg1);

    printf("REGISTER --- CONFIG VALUES\n");
    printf("PLL_CNTRL1   %04x --- %04x\n",pllCfg1.PLLCNTL1,hPll->pllConfig->PLLCNTL1);
    printf("PLL_CNTRL2   %04x --- %04x Test Lock Mon will get set after PLL is up\n", pllCfg1.PLLCNTL2,hPll->pllConfig->PLLCNTL2);
    printf("PLL_CNTRL3   %04x --- %04x\n",pllCfg1.PLLINCNTL,hPll->pllConfig->PLLINCNTL);
    printf("PLL_CNTRL4   %04x --- %04x\n",pllCfg1.PLLOUTCNTL,hPll->pllConfig->PLLOUTCNTL);

    status = PLL_enable(hPll); // - OK
    if(CSL_SOK != status) { printf("PLL enable failed:%d\n",CSL_ESYS_BADHANDLE); return(status); } // - OK
    
//---------- PLL SETUP END -------------
    printf("PLL SETUP END \n");
	printf("PLL_CGCR1 = %X \n", PLL_CGCR1); 
	printf("PLL_CGCR2 = %X \n", PLL_CGCR2); 
	printf("PLL_CGCR3 = %X \n", PLL_CGCR3);    
	printf("PLL_CGCR4 = %X \n", PLL_CGCR4); 
	printf("PLL_CCR2 = %X \n", 	PLL_CCR2); 		
	
	return(0);
}
