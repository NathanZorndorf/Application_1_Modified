/* ===============================================
 * Author: Nathan Zorndorf
 * Description: Uses DMA Controller 1 Channels 0-3 to take audio in from I2S2 RX, do processing and return the fundamental frequency of the input audio signal. 
 */

#include <stdio.h>
#include <math.h> 
#include <tms320.h>
#include <Dsplib.h>
#include <usbstk5505.h>
#include <Application_1_Modified_Registers.h>
#include <Audio_To_MIDI_Using_DMA_and_CFFT.h>
#include <hwafft.h>

Int16 OverlapInL[WND_LEN];
Int16 OverlapInR[WND_LEN];
Int16 OverlapOutL[OVERLAP_LENGTH];
Int16 OverlapOutR[OVERLAP_LENGTH];

/* --- buffers required for processing  ----*/
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
#pragma DATA_SECTION(PSD_Result, "PSD");
Int16 PSD_Result[FFT_LENGTH];
/* -------------------------------------------*/
/* --- Special buffers required for HWAFFT ---*/
/*
#pragma DATA_SECTION(complex_buffer, "cmplxBuf");
LDATA complex_buffer[WND_LEN];
#pragma DATA_SECTION(bitreversed_buffer, "brBuf");
#pragma DATA_ALIGN(bitreversed_buffer, 2*FFT_LENGTH);
LDATA bitreversed_buffer[FFT_LENGTH];
#pragma DATA_SECTION(temporary_buffer,"tmpBuf");
LDATA temporary_buffer[FFT_LENGTH];
*/
/* -------------------------------------------*/
/* --- Special buffers required for CFFT ---*/
#pragma DATA_SECTION(complex_data,"cmplxBuf");
LDATA complex_data[FFT_LENGTH];
/* -------------------------------------------*/

int Audio_To_MIDI_Using_DMA_and_CFFT(void) {

	int i = 0;
	int j = 0;
	int f = 0;
	int Peak_Magnitude_Value = 0;
	int Peak_Magnitude_Index = 0;

		
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

		
		/* Create windowed/not windowed buffer for processing */
        for (i = 0; i < WND_LEN; i++) {
        	BufferL[i] = OverlapInL[i];
        	BufferR[i] = OverlapInR[i];
        }
        
	    
		/* Initialize relevant pointers */
		//bitrev_data  = bitreversed_buffer;
		//complex_data = complex_buffer;
	
		/* Convert real data to "pseudo"-complex data (real, 0) */
		/* Int32 complex = Int16 real (MSBs) + Int16 imag (LSBs) */
		/*
		for (i = 0; i < FFT_LENGTH; i++) 
		{
			*(complex_data + i) = ( (Int32) (*(BufferR + i)) ) << 16; // Shift Left 16 Bits. 
		}
		*/
		/*
		for (i = 0; i < FFT_LENGTH; i++) 
		{
			complex_data[i] = BufferR[i];
		}
		*/
		
		for (i = 0; i < HOP_SIZE; i++) 
		{
			complex_data[2*i] = BufferR[i]; // place audio data (Real) in each even index of complex_data
			complex_data[2*i+1] = 0;		// place a 0 (Imag) in each odd index of complex_data
		}
		
			
		/* Perform FFT */
		//cfft32(complex_data, HOP_SIZE, SCALE);
		cfft32_SCALE(complex_data, HOP_SIZE); 

		/* Perform bit-reversing */
		cbrev32(complex_data, complex_data, HOP_SIZE);


		/* Extract real and imaginary parts */
		for (i = 0; i < FFT_LENGTH; i++) {
			*(realR + i) = (Int16)((*(complex_data + i)) >> 16);
			*(imagR + i) = (Int16)((*(complex_data + i)) & 0x0000FFFF);
		}
	
	
	        // Process freq. bins from 0Hz to Nyquist frequency  
			// Perform spectral processing here 
			Peak_Magnitude_Value = sqrt((realR[0])^2 + (imagR[0])^2); // start the search at the first value in the Magnitude plot
			
			for( j = 1; j < NUM_BINS; j++ )
			{
				PSD_Result[j] = sqrt((realR[j])^2 + (imagR[j])^2); // Convert FFT to magnitude spectrum. Basically Find magnitude of FFT result for each index 
				
				if( PSD_Result[j] > Peak_Magnitude_Value ) // Peak search on the magnitude of the FFT to find the fundamental frequency  
				{
					Peak_Magnitude_Value = PSD_Result[f];
					Peak_Magnitude_Index = f;
				}
			}
		
		/*
		printf("BufferR[256]= %d \n", BufferR[256]);
		printf("realR[256] 	= %d \n", realR[256]);
		printf("PSD_Result[256] 	 = %d \n", PSD_Result[256]);
		printf("Peak_Magnitude_Value = %d \n", Peak_Magnitude_Value);
		printf("Peak_Magnitude_Index = %d \n\n", Peak_Magnitude_Index);
		*/
		

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


