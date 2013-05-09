/* Author: Nathan Zorndorf
 * Description: Audio to MIDI converter. 
 */

#include <stdio.h>
#include <math.h> 
#include <tms320.h>
#include <Dsplib.h>
#include <my_types.h>
#include <usbstk5505.h>
#include <Application_1_Modified_Registers.h>
#include <My_PLL.h>
#include <My_I2C.h> 
#include <My_AIC3204.h>
#include <My_I2S.h>
#include <My_I2S_Register.h>
//#include <Audio_Straight_Through.h>
//#include <My_DMA_Ping_Pong.h>
//#include <My_DMA_Ping_Pong_Register.h>
#include <Audio_Straight_Through_Using_DMA.h>
#include <My_DMA_Ping_Pong_Register_Setup.h>
#include <My_UART.h>

// Buffers for DMA in/out and overlap-add 
Int16 PingPongFlagInL;
Int16 PingPongFlagInR;
Int16 PingPongFlagOutL;
Int16 PingPongFlagOutR;

Int16 DMA_InpL[PING_PONG_SIZE];
Int16 DMA_InpR[PING_PONG_SIZE];
Int16 DMA_OutL[PING_PONG_SIZE];
Int16 DMA_OutR[PING_PONG_SIZE];

/* Testing Varialbes */ 
Uint32 DMA_ISR_count = 0;
Uint32 DMA_ISR_0_Fill_Ping = 0;
Uint32 DMA_ISR_0_Fill_Pong = 0;
Uint32 DMA_ISR_1_Fill_Ping = 0;
Uint32 DMA_ISR_1_Fill_Pong = 0;
Uint32 DMA_ISR_2_Fill_Ping = 0;
Uint32 DMA_ISR_2_Fill_Pong = 0;
Uint32 DMA_ISR_3_Fill_Ping = 0;
Uint32 DMA_ISR_3_Fill_Pong = 0;

int main(void) {
	
	int i;
	
	printf("Hello Nathan. Let's get funky. \n");
	
 	My_PLL();

 	IER0 = 0xC100; // DMA, I2S RX/TX Interrupt Enable. 
 	
 	My_I2C();
 	 
 	My_AIC3204();


	/*	
	for(i=0; i < 8; i++) // Checking to make sure I2S Receive registers are receiving data properly. 
	{
			printf("I2S2 Receive Left Data 0  (LSW) Register = %x \n", 	I2S2_W0_LSW_R);
	 	 	printf("I2S2 Receive Left Data 1 (MSW) Register = %x \n",  I2S2_W0_MSW_R); 
	    	printf("I2S2 Receive Right Data 0  (LSW) Register = %x \n", 	I2S2_W1_LSW_R);
	 	 	printf("I2S2 Receive Right Data 1 (MSW) Register = %x \n",	I2S2_W1_MSW_R);
	}
	
	Audio_Straight_Through();
	*/

	My_DMA_Ping_Pong_Register_Setup();
	
	My_I2S_Register();
		
	Audio_Straight_Through_Using_DMA();
	

 	return(1);
}










