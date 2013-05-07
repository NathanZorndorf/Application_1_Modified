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

#define I2S2_I2SSCTRL            *(volatile ioport Uint16*)(0x2A00) // I2S2 Serializer Control Register
#define I2S2_I2SSRATE            *(volatile ioport Uint16*)(0x2A04) // I2S2 Sample Rate Generator Register
#define I2S2_I2SINTFL            *(volatile ioport Uint16*)(0x2A10) // I2S2 Interrupt Flag Register
#define I2S2_I2SINTMASK          *(volatile ioport Uint16*)(0x2A14) // I2S2 Interrupt Mask Register

int My_I2S_Register(void) {

// ------------------------ I2S2 Setup Begin ------------------------ //
    printf("\nI2S SETUP BEGIN!!!\n");
     
    //I2S2_I2SINTMASK = 0x0000; // For use with Audio_Straight_Through_Using_DMA.c
    I2S2_I2SINTMASK = 0x002B; // For use with Audio_Straight_Through.c
    
    I2S2_I2SSCTRL   = 0x8090; /* Set I2S0 in stereo mode, 16-bit, with data packing, slave */
	
    printf("I2S2_I2SINTMASK = 0x%X\n", I2S2_I2SINTMASK);
    printf("I2S2_I2SINTFL 	= 0x%X\n", I2S2_I2SINTFL);
  	printf("I2S2_SRGR 		= 0x%X \n", 	I2S2_SRGR); // Print value of  I2S2_SRGR : I2S2 Sample Rate Generator Register
	printf("I2S2_I2SSCTRL 	= 0x%X \n", 	I2S2_I2SSCTRL); // Print value of I2S2_I2SSCTRL : I2S2 Serializer Control Register  
	printf("I2S SETUP END!!! \n\n");
// ------------------------  I2S2 Setup End  ------------------------ //

	return(0);
}
