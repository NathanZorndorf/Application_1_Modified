/*  ============================================================================
 *   Author: Nathan Zorndorf 
 *   Description: Setting up the I2S and AIC3204 
 * I2S Transmit and receive interrupts/events are continuously generated after every transfer from the transmit
data registers to the transmit buffer register and from the receive buffer register to the receive data
registers respectively. If packed mode is enabled (PACK = 1 in I2SSCTRL), interrupts/events are
generated after all the required number of data words have been transmitted/received (see
Section 10.2.8).
From the tech ref (page 351): "Each I2S bus has access to one of the four DMA peripherals on the DSP"  
					DMA0 <===> I2S0     
					DMA1 <===> I2S2     
					DMA2 <===> I2S3    
					DMA3 <===> I2S1 

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
#include <csl_i2s.h> 
#include <audio_to_midi.h>
#include <My_DMA_Ping_Pong.h>
#include <My_AIC3204.h> 

#define I2S2_I2SINTFL          *(volatile ioport Uint16*)(0x2A10) // I2S2 Interrupt Flag Register

int My_I2S(void) {
	 
//---------- I2C SETUP BEGIN -------------

	 // Variables for I2S functions // 
	 Int16          	 status;
     I2S_Instance		 i2sInstNum; // used in I2S_open
     I2S_OpMode		     opMode;	 // used in I2S_open
     I2S_ChanType 	     chType;     // used in I2S_open	
     CSL_I2sHandle		 hI2S2;		 // used in I2S_setup
   	 I2S_Config     	 configI2S;  // used in I2S_setup
	 
// ------------------------ I2S2 Setup ------------------------ //
     printf("\nI2S SETUP BEGIN\n");
     i2sInstNum = I2S_INSTANCE2; 	// - OK
     opMode 	= DMA_INTERRUPT; 	// - ?
     chType 	= I2S_CHAN_STEREO;  // - OK
     hI2S2 		= I2S_open(i2sInstNum, opMode, chType); // - OK
     
     configI2S.dataType     = I2S_STEREO_ENABLE; // - OK
     configI2S.loopBackMode = I2S_LOOPBACK_DISABLE; // - OK
     configI2S.fsPol 		= I2S_FSPOL_LOW;	// Left chan. is transmitted when fs is LOW/high - OK
     configI2S.clkPol		= I2S_RISING_EDGE;	// TX on FALLING/RISING clock edge - OK 
     configI2S.datadelay	= I2S_DATADELAY_ONEBIT; // - OK
     configI2S.datapack     = I2S_DATAPACK_ENABLE; // - OK
     configI2S.signext		= I2S_SIGNEXT_DISABLE; // Set sign extension of data - OK
     configI2S.wordLen      = I2S_WORDLEN_16; // - OK
     configI2S.i2sMode      = I2S_SLAVE; // I2S is SLAVE - OK 
     configI2S.dataFormat   = I2S_DATAFORMAT_LJUST; // - OK
     configI2S.clkDiv		= I2S_CLKDIV2;	// I2Sn_CLK = SystemClock / I2S_CLKDIV - Data bit rate - OK
	 configI2S.fsDiv		= I2S_FSDIV8;	// I2Sn_FS = I2Sn_CLK / I2S_FSDIV - Data word rate = Sampling Rate -  (2FSDIV+3 >= 2 * WDLNGTH (for stereo mode)) - OK
	 										// CLKDIV and FSDIV are not used when CPU_I2S is slave
	 configI2S.FError		= I2S_FSERROR_ENABLE; // - OK
	 configI2S.OuError		= I2S_OUERROR_ENABLE; // - OK
     
     status = I2S_setup(hI2S2, &configI2S);
     if (status != CSL_SOK) {printf("I2S ERROR: Could not setup I2S.\n");return 0;} 
     	else {printf("I2S configuration successful!\n");}

     status = I2S_transEnable(hI2S2, TRUE);
	 if (status != CSL_SOK) { printf("I2S ERROR: Could not enable data transfer!\n"); return 0; } 
		else {printf("I2S data transfer bit enabled!\n"); }
  

    printf("I2SINTFL = 0x%X\n", I2S2_I2SINTFL);
  	printf("I2SSRATE = %X \n", 	I2S2_SRGR); // Print value of  I2S2_SRGR : I2S2 Sample Rate Generator Register
	printf("I2SSCTRL = %X \n", 	I2S2_CR); // Print value of I2S2_CR : I2S2 Serializer Control Register  
	printf("I2SINTMASK = %X \n", I2S2_ICMR); // Print value of I2S2_ICMR : I2S2 Interrupt Mask Register   
	printf("I2S SETUP END \n\n");
    //--- I2S END

	return(0);
}
