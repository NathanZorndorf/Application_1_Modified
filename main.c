/* Author: Nathan Zorndorf
 * Description: Audio to MIDI converter. 
 */

#include <stdio.h>
#include <math.h> 
#include <tms320.h>
#include <Dsplib.h>
#include <my_types.h>
#include <usbstk5505.h>
#include <My_DMA_Ping_Pong.h>
#include <My_PLL.h>
#include <My_I2C.h> 
#include <My_AIC3204.h>
#include <My_I2S.h>
#include <My_I2S_Register.h>
#include <Audio_Straight_Through.h>
//#include <Audio_Straight_Through_Using_DMA.h>
#include <My_DMA_Ping_Pong_Register.h>
#include <My_UART.h>

#define DMA_IFR     *(ioport volatile unsigned *)0x1C30    // DMA Interrupt Flag Register
#define DMA_MSK     *(ioport volatile unsigned *)0x1C31    // DMA Interrupt Mask Flag Register
#define IER0        *(volatile unsigned *)0x0000
#define IFR0        *(volatile unsigned *)0x0001
#define IER1        *(volatile unsigned *)0x0045
#define IFR1        *(volatile unsigned *)0x0046

int main(void) {
	
	int success,i;
	
	printf("Hello Nathan. Let's get funky. \n");
 	success = My_PLL();
 	IER0 = 0xC100; // DMA, I2S RX/TX Interrupt Enable. 
 	success = My_I2C(); 
 	success = My_AIC3204();

	/*	
 	success = My_I2S_Register(); // - called inside of My_DMA_Ping_Pong_Register 
 	 
	for(i=0; i < 8; i++) // Checking to make sure I2S Receive registers are receiving data properly. 
	{
			printf("I2S2 Receive Left Data 0  (LSW) Register = %x \n", 	I2S2_W0_LSW_R);
	 	 	printf("I2S2 Receive Left Data 1 (MSW) Register = %x \n",  I2S2_W0_MSW_R); 
	    	printf("I2S2 Receive Right Data 0  (LSW) Register = %x \n", 	I2S2_W1_LSW_R);
	 	 	printf("I2S2 Receive Right Data 1 (MSW) Register = %x \n",	I2S2_W1_MSW_R);
	}
	
	Audio_Straight_Through();
	*/
	
	My_DMA_Ping_Pong_Register();

	

 	return(1);
}










