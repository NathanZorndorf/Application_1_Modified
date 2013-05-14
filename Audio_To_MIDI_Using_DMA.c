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
#include <Audio_To_MIDI_Using_DMA.h>
#include <hwafft.h>

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

#pragma DATA_SECTION(PSD_Result, "PSD");
Int16 PSD_Result[FFT_LENGTH];

/* --- Special buffers required for HWAFFT ---*/
#pragma DATA_SECTION(complex_buffer, "cmplxBuf");
Int32 complex_buffer[WND_LEN];

#pragma DATA_SECTION(bitreversed_buffer, "brBuf");
#pragma DATA_ALIGN(bitreversed_buffer, 2*FFT_LENGTH);
Int32 bitreversed_buffer[FFT_LENGTH];

#pragma DATA_SECTION(temporary_buffer,"tmpBuf");
Int32 temporary_buffer[FFT_LENGTH];
/* -------------------------------------------*/

void do_fft(Int16 *real_data, Int16 *fft_real, Int16 *fft_imag, Uint16 scale);
void do_ifft(Int16 *fft_real, Int16 *fft_imag, Int16 *ifft_data, Uint16 scale);

int Audio_To_MIDI_Using_DMA(void) {

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
        	//BufferL[i] = _smpy(OverlapInL[i], window[i]);
        	//BufferR[i] = _smpy(OverlapInR[i], window[i]);
        	BufferL[i] = OverlapInL[i];
        	BufferR[i] = OverlapInR[i];
        }
        
        
        // Perform FFT on windowed buffer 
   		//do_fft(BufferL, realL, imagL, 1);
   		do_fft(BufferR, realR, imagR, 1);
        
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
			} // if
			
		} // for
		
		/*
		printf("BufferR[256]= %d \n", BufferR[256]);
		printf("realR[256] 	= %d \n", realR[256]);
		printf("PSD_Result[256] 	 = %d \n", PSD_Result[256]);
		printf("Peak_Magnitude_Value = %d \n", Peak_Magnitude_Value);
		printf("Peak_Magnitude_Index = %d \n\n", Peak_Magnitude_Index);
		*/
		
		/*
		// Complete symmetric frequencies (since audio data are real) 
		for (i = 1; i < FFT_LENGTH/2; i++) {
			realL[FFT_LENGTH - i] = realL[i];
			imagL[FFT_LENGTH - i] = -imagL[i]; // conjugation 
			realR[FFT_LENGTH - i] = realR[i];
			imagR[FFT_LENGTH - i] = -imagR[i]; // conjugation 
		}


		// Perform IFFT 
   		do_ifft(realL, imagL, BufferL, 0);
   		do_ifft(realR, imagR, BufferR, 0);
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



/* ---------------- Wrapper function to implement HWAFFT ---------------- */
/* For more information see: Application Report - SPRABB6A, June 2010     */
/* Note that there are also several discussions in e2e.ti.com forums.     */
/* The appropriate functions are linked via c5515.cmd. See also:          */
/* http://e2e.ti.com/support/dsp/c5000/f/109/p/49635/176118.aspx#176118   */
/*																		  */
/* The function requires pre-allocated arrays for input data (16-bit Q15) */
/* and real and imaginray parts of the FFT.                               */
/* ---------------------------------------------------------------------- */
void do_fft(Int16 *real_data, Int16 *fft_real, Int16 *fft_imag, Uint16 scale)
{
	Uint16 i, data_selection;
	Int32 *complex_data, *bitrev_data, *temp_data, *fft_data;

	/* Initialize relevant pointers */
	bitrev_data  = bitreversed_buffer;
	temp_data    = temporary_buffer;
	complex_data = complex_buffer;

	/* Convert real data to "pseudo"-complex data (real, 0) */
	/* Int32 complex = Int16 real (MSBs) + Int16 imag (LSBs) */
	for (i = 0; i < FFT_LENGTH; i++) {
		*(complex_data + i) = ( (Int32) (*(real_data + i)) ) << 16;
	}

	/* Perform bit-reversing */
	hwafft_br(complex_data, bitrev_data, FFT_LENGTH);

	/* Perform FFT */
	if (scale) {
		data_selection = hwafft_512pts(bitrev_data, temp_data, FFT_FLAG, SCALE_FLAG); // hwafft_#pts, where # = 2*HOP_SIZE 
	} 
	else {
		data_selection = hwafft_512pts(bitrev_data, temp_data, FFT_FLAG, NOSCALE_FLAG);
	}

	/* Return appropriate data pointer */
	if (data_selection == 1) {
		fft_data = temp_data;	// results stored in this scratch vector 
	} 
	else {
		fft_data = bitrev_data; // results stored in this data vector 
	}

	/* Extract real and imaginary parts */
	for (i = 0; i < FFT_LENGTH; i++) {
		*(fft_real + i) = (Int16)((*(fft_data + i)) >> 16);
		*(fft_imag + i) = (Int16)((*(fft_data + i)) & 0x0000FFFF);
	}
}

/* ---------------- Wrapper function to implement HWAIFFT --------------- */
/* For more information see: Application Report - SPRABB6A, June 2010     */
/* Note that there are also several discussions in e2e.ti.com forums.     */
/* The appropriate functions are linked via c5515.cmd. See also:          */
/* http://e2e.ti.com/support/dsp/c5000/f/109/p/49635/176118.aspx#176118   */
/*																		  */
/* The function requires pre-allocated arrays for output data (16-bit Q15)*/
/* of the IFFT.							                                  */
/* ---------------------------------------------------------------------- */
void do_ifft(Int16 *fft_real, Int16 *fft_imag, Int16 *ifft_data, Uint16 scale)
{
	Uint16 i, data_selection;
	Int32 *complex_data, *bitrev_data, *temp_data, *ifft_tmp;

	/* Initialize relevant pointers */
	complex_data = complex_buffer;
	bitrev_data  = bitreversed_buffer;
	temp_data    = temporary_buffer;

	/* Reconstruct complex data from real and imaginary parts */
	for (i = 0; i < FFT_LENGTH; i++) {
		*(complex_data + i) = ((Int32)(*(fft_real + i)) << 16);
		*(complex_data + i) = *(complex_data + i) | ((Int32)(*(fft_imag + i)) & 0x0000FFFF);
	}

	/* Perform bit-reversing */
	hwafft_br(complex_data, bitrev_data, FFT_LENGTH);

	/* Perform IFFT */
	if (scale) {
		data_selection = hwafft_512pts(bitrev_data, temp_data, IFFT_FLAG, SCALE_FLAG);
	} else {
		data_selection = hwafft_512pts(bitrev_data, temp_data, IFFT_FLAG, NOSCALE_FLAG);
	}

	/* Return appropriate data pointer */
	if (data_selection) {
		ifft_tmp = temp_data;
	} else {
		ifft_tmp = bitrev_data;
	}

	/* Return real part of IFFT */
	for (i = 0; i < FFT_LENGTH; i++) {
		*(ifft_data + i) = (Int16)((*(ifft_tmp + i)) >> 16);
	}
}
