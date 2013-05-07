/* ===============================================
 * Author: Nathan Zorndorf
 * Description: Uses DMA Controller 1 Channels 0-3 to take audio in from I2S2 RX, and pass it straight through to I2S2 TX
 */

#include <stdio.h>
#include <math.h> 
#include <tms320.h>
#include <Dsplib.h>
#include <my_types.h>
#include <usbstk5505.h>
#include <Application_1_Modified_Registers.h>
#include <Audio_Straight_Through_Using_DMA.h>

Int16 OverlapInL[WND_LEN];
Int16 OverlapInR[WND_LEN];
Int16 OverlapOutL[OVERLAP_LENGTH];
Int16 OverlapOutR[OVERLAP_LENGTH];

/* Buffers for processing */
#pragma DATA_SECTION(BufferL,"BufL");
Int16 BufferL[FFT_LENGTH];
#pragma DATA_SECTION(BufferR,"BufR");
Int16 BufferR[FFT_LENGTH];
#pragma DATA_SECTION(realL, "rfftL");
Int16 realL[FFT_LENGTH];
#pragma DATA_SECTION(realR, "rfftR");
Int16 realR[FFT_LENGTH];
#pragma DATA_SECTION(imagL, "ifftL");
Int16 imagL[FFT_LENGTH];
#pragma DATA_SECTION(imagR, "ifftR");
Int16 imagR[FFT_LENGTH];

/* --- Special buffers required for HWAFFT ---*/
#pragma DATA_SECTION(complex_buffer, "cmplxBuf");
Int32 complex_buffer[WND_LEN];

#pragma DATA_SECTION(bitreversed_buffer, "brBuf");
#pragma DATA_ALIGN(bitreversed_buffer, 2*FFT_LENGTH);
Int32 bitreversed_buffer[FFT_LENGTH];

#pragma DATA_SECTION(temporary_buffer,"tmpBuf");
Int32 temporary_buffer[FFT_LENGTH];
/* -------------------------------------------*/

int Audio_Straight_Through_Using_DMA(void) {

	int i;
	
	
	/* Initialize buffers */
	for (i = 0; i < WND_LEN; i++) {
		OverlapInL[i] = 0;
		OverlapInR[i] = 0;
	}
	for (i = 0; i < OVERLAP_LENGTH; i++) {
		OverlapOutL[i] = 0;
		OverlapOutR[i] = 0;
	}
	
	/* Begin infinite loop */
	while (1) 
	{
        /* Get new input audio block */
		if (PingPongFlagInL && PingPongFlagInR)  // Last Transfer complete was Pong - Filling Ping
		{
			for (i = 0; i < HOP_SIZE; i++) {
		       	/* Copy previous NEW data to current OLD data */
		       	OverlapInL[i] = OverlapInL[i + HOP_SIZE];
		       	OverlapInR[i] = OverlapInR[i + HOP_SIZE];

		       	/* Update NEW data with current audio in */
		       	OverlapInL[i + HOP_SIZE] = DMA_InpL[i + AUDIO_IO_SIZE]; // CPU Copies Second Half of index values ("Pong"), while DMA fills First Half ("Ping")
		       	OverlapInR[i + HOP_SIZE] = DMA_InpR[i + AUDIO_IO_SIZE];
		    }
		}
		else  									// Last Transfer complete was Ping - Filling Pong
		{
			for (i = 0; i < HOP_SIZE; i++) {
		       	/* Copy previous NEW data to current OLD data */
		       	OverlapInL[i] = OverlapInL[i + HOP_SIZE];
		       	OverlapInR[i] = OverlapInR[i + HOP_SIZE];

		       	/* Update NEW data with current audio in */
		       	OverlapInL[i + HOP_SIZE] = DMA_InpL[i];
		       	OverlapInR[i + HOP_SIZE] = DMA_InpR[i];
		    }
		}

        for (i = 0; i < WND_LEN; i++) 
        {
        	BufferL[i] = OverlapInL[i]; 
        	BufferR[i] = OverlapInR[i];
        }

        if (PingPongFlagOutL && PingPongFlagOutR) // Last Transfer complete was Pong - Filling Ping
        {
        	for (i = 0; i < OVERLAP_LENGTH; i++) 
        	{
        		/* Current output block is previous overlapped block + current processed block */
        		DMA_OutL[i + AUDIO_IO_SIZE] = OverlapOutL[i] + BufferL[i];
        		DMA_OutR[i + AUDIO_IO_SIZE] = OverlapOutR[i] + BufferR[i]; 

				/* Update overlap buffer */
        		OverlapOutL[i] = BufferL[i + HOP_SIZE];
        		OverlapOutR[i] = BufferR[i + HOP_SIZE];
        	}
        } 
        else 									// Last Transfer complete was Ping - Filling Pong
        {
        	for (i = 0; i < OVERLAP_LENGTH; i++) 
        	{
        		/* Current output block is previous overlapped block + current processed block */
        		DMA_OutL[i] = OverlapOutL[i] + BufferL[i];
        		DMA_OutR[i] = OverlapOutR[i] + BufferR[i]; 

        		/* Update overlap buffer */
        		OverlapOutL[i] = BufferL[i + HOP_SIZE];
        		OverlapOutR[i] = BufferR[i + HOP_SIZE];
        	}
        }
	}

}
