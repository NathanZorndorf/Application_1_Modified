/* Author: Nathan Zorndorf
 * Description: Testing and using the I2S
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
#include <csl_i2c.h>
#include <csl_i2s.h> 
#include <aic_i2c.h>

int My_I2C(void) {
	
	// Variable declaration 
 	//CSL_Status      status;
	//CSL_I2cSetup	configI2C;

	// ------------------------ I2C Setup ---------------------------- //
	printf("I2C SETUP BEGIN!!\n");

	/*	
	// Initialize I2C module instance //
	status = I2C_init(CSL_I2C0); // CSL_I2C0 means 0 = instance number 
	if(status != CSL_SOK) { printf("I2C Init Failed!!\n"); return(status); }

	// Setup I2C module for use with the audio codec AIC3204 //
	configI2C.addrMode    = CSL_I2C_ADDR_7BIT;
	configI2C.bitCount    = CSL_I2C_BC_8BITS;
	configI2C.loopBack    = CSL_I2C_LOOPBACK_DISABLE;
	configI2C.freeMode    = CSL_I2C_FREEMODE_DISABLE;
	configI2C.repeatMode  = CSL_I2C_REPEATMODE_DISABLE;
	configI2C.ownAddr     = 0x2F; 	// I2C own slave address - don't care if master.
	configI2C.sysInputClk = 100; 	// Value of system clock in MHz 
	configI2C.i2cBusFreq  = 10; 	// I2C bus frequency in KHz- a number between 10 and 400. 

	status = I2C_setup(&configI2C); if(status != CSL_SOK) { printf("I2C Setup Failed!!\n"); return(status); }
	
	asm(" bclr XF"); // - I think it clears the register corresponding to the XF LED on the board.. 
	*/
	
	I2C_MDR = 0x0400;             // Reset I2C
    I2C_PSC   = 7;               // Config prescaler for 12MHz
    I2C_CLKL  = 0x026C;               // Config clk LOW for 20kHz
    I2C_CLKH  = 0x026C;               // Config clk HIGH for 20kHz
    
    I2C_MDR   = 0x0420   ;        // Release from reset; Master, Transmitter, 7-bit address
    
	printf("I2C SETUP END!!\n\n");
    // --------------------------------------------------------------- //
    
    return(0);
    
}
