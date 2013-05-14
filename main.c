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
#include <My_DMA_Ping_Pong_Register_Setup.h>
// #include <Audio_Straight_Through_Using_DMA.h>
#include <Audio_To_MIDI_Using_DMA.h>

// Buffers for DMA in/out and overlap-add 
Int16 PingPongFlagInL;
Int16 PingPongFlagInR;
Int16 PingPongFlagOutL;
Int16 PingPongFlagOutR;

Int16 DMA_InpL[PING_PONG_SIZE];
Int16 DMA_InpR[PING_PONG_SIZE];
Int16 DMA_OutL[PING_PONG_SIZE];
Int16 DMA_OutR[PING_PONG_SIZE];

int main(void) {
	
	printf("Hello Nathan. Let's get funky. \n");
	
 	My_PLL();

 	IER0 = 0xC100; // DMA, I2S RX/TX Interrupt Enable. 
 	
 	My_I2C();
 	 
 	My_AIC3204();

	My_DMA_Ping_Pong_Register_Setup();
	
	My_I2S_Register();
		
	// Audio_Straight_Through_Using_DMA();
	
	Audio_To_MIDI_Using_DMA();
	

 	return(1);
}










