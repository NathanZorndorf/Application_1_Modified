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
#include <Output_MIDI.h>

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
#pragma DATA_SECTION(PSD_Result_sqrt, "PSD_sqrt");
Int16 PSD_Result_sqrt[FFT_LENGTH];
/* -------------------------------------------*/
/* --- Special buffers required for CFFT ---*/
#pragma DATA_SECTION(complex_data,"cmplxBuf");
LDATA complex_data[2*FFT_LENGTH];
/* -------------------------------------------*/

int Audio_To_MIDI_Using_DMA_and_CFFT(void) {

	int i = 0;
	int j = 0;
	int f = 0;
	DATA Peak_Magnitude_Value = 0;
	DATA Peak_Magnitude_Index = 0;
	int MIDI[256] = {0};
	
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
        
	
		/* Convert real data to "pseudo"-complex data (real, 0) */
		/* Int32 complex = Int16 real (MSBs) + Int16 imag (LSBs) */
		for (i = 0; i < FFT_LENGTH; i++) 
		{
			complex_data[2*i] = BufferR[i]; // place audio data (Real) in each even index of complex_data
			complex_data[2*i+1] = 0;		// place a 0 (Imag) in each odd index of complex_data
		}
		
			
		/* Perform FFT */
		//cfft32(complex_data, HOP_SIZE, SCALE);
		cfft32_SCALE(complex_data, FFT_LENGTH); 

		/* Perform bit-reversing */
		cbrev32(complex_data, complex_data, FFT_LENGTH);


		/* Extract real and imaginary parts */
		for (i = 0; i < FFT_LENGTH; i++) {
			realR[i] = complex_data[2*i];
			imagR[i] = complex_data[2*i+1];
		}
	
		// Find the Power of the audio signal using the cfft results and scale by 1/2 
		for(i = 0; i < FFT_LENGTH; i++) { // square the real vector and the imaginary vector
			realR[i] = realR[i] * realR[i];	
			imagR[i] = imagR[i] * imagR[i];
		}
		
		// ADD 
		for(i = 0; i < FFT_LENGTH; i++) {
			PSD_Result[i] = realR[i] + imagR[i];
		}
		
		
		// Scale result by dividing again, because im not sure if I have the sqrt C library runtime fuction
		
		
		
		// Process freq. bins from 0Hz to Nyquist frequency (for efficiency) 
		Peak_Magnitude_Value = PSD_Result[1]; // start the search at the first value in the Magnitude spectrum
		Peak_Magnitude_Index = 1;
		for( j = 2; j < NUM_BINS; j++ ) // go through useful frequency bins (FFT_LENGTH/2 +1) because the rest are symmetric
		{ 
			if( PSD_Result[j] > Peak_Magnitude_Value ) // Peak search on the magnitude of the FFT to find the fundamental frequency - the frequency bin with the highest power value in it 
			{
				Peak_Magnitude_Value = PSD_Result[j];
				Peak_Magnitude_Index = j;
			}
		}
		
		
		// This huge if-else statement only applies for a sample rate of 8000 samples/sec, and an FFT length of 512.
		f++;
		if ((Peak_Magnitude_Index >= 15) && (Peak_Magnitude_Index < 16)) { 
                MIDI[f] = 59; }
        else if ((Peak_Magnitude_Index >= 16) && (Peak_Magnitude_Index < 17)) {
                MIDI[f] = 60; }
        else if ((Peak_Magnitude_Index >= 17) && (Peak_Magnitude_Index < 18)) {
                MIDI[f] = 61; }
        else if ((Peak_Magnitude_Index >= 18) && (Peak_Magnitude_Index < 19)) {
                MIDI[f] = 62; }
        else if ((Peak_Magnitude_Index >= 19) && (Peak_Magnitude_Index < 21)) { 
                MIDI[f] = 63; }
        else if ((Peak_Magnitude_Index >= 21) && (Peak_Magnitude_Index < 22)) {
                MIDI[f] = 64; }
        else if ((Peak_Magnitude_Index >= 22) && (Peak_Magnitude_Index < 23)) {
                MIDI[f] = 65; }
        else if ((Peak_Magnitude_Index >= 23) && (Peak_Magnitude_Index < 24)) {
                MIDI[f] = 66; }
        else if ((Peak_Magnitude_Index >= 24) && (Peak_Magnitude_Index < 26)) {
                MIDI[f] = 67; }
        else if ((Peak_Magnitude_Index >= 26) && (Peak_Magnitude_Index < 27)) { 
                MIDI[f] = 68; }
        else if ((Peak_Magnitude_Index >= 27) && (Peak_Magnitude_Index < 29)) { 
                MIDI[f] = 69; }
        else if ((Peak_Magnitude_Index >= 29) && (Peak_Magnitude_Index < 31)) {
                MIDI[f] = 70; }
        else if ((Peak_Magnitude_Index >= 31) && (Peak_Magnitude_Index < 33)) { 
                MIDI[f] = 71; }
        else if ((Peak_Magnitude_Index >= 33) && (Peak_Magnitude_Index < 34)) {
                MIDI[f] = 72; }
        else if ((Peak_Magnitude_Index >= 34) && (Peak_Magnitude_Index < 37)) { 
                MIDI[f] = 73; }
        else if ((Peak_Magnitude_Index >= 37) && (Peak_Magnitude_Index < 39)) { 
                MIDI[f] = 74; }
        else if ((Peak_Magnitude_Index >= 39) && (Peak_Magnitude_Index < 41)) {
                MIDI[f] = 75; }

        else if ((Peak_Magnitude_Index >= 41) && (Peak_Magnitude_Index < 43)) {
                MIDI[f] = 76; }
        else if ((Peak_Magnitude_Index >= 43) && (Peak_Magnitude_Index < 46)) { 
                MIDI[f] = 77; }
        else if ((Peak_Magnitude_Index >= 46) && (Peak_Magnitude_Index < 49)) { 
                MIDI[f] = 78; }
        else if ((Peak_Magnitude_Index >= 49) && (Peak_Magnitude_Index < 52)) {
                MIDI[f] = 79; }
        else if ((Peak_Magnitude_Index >= 52) && (Peak_Magnitude_Index < 55)) {
                MIDI[f] = 80; }
        else if ((Peak_Magnitude_Index >= 55) && (Peak_Magnitude_Index < 58)) {
                MIDI[f] = 81; }
        else if ((Peak_Magnitude_Index >= 58) && (Peak_Magnitude_Index < 61)) {
                MIDI[f] = 82; }
        else if ((Peak_Magnitude_Index >= 61) && (Peak_Magnitude_Index < 65)) { 
                MIDI[f] = 83; }
        else if ((Peak_Magnitude_Index >= 65) && (Peak_Magnitude_Index < 69)) {
                MIDI[f] = 84; }
        else if ((Peak_Magnitude_Index >= 69) && (Peak_Magnitude_Index < 73)) { 
                MIDI[f] = 85; }

        else if ((Peak_Magnitude_Index >= 73) && (Peak_Magnitude_Index < 77)) {
                MIDI[f] = 86; }
        else if ((Peak_Magnitude_Index >= 77) && (Peak_Magnitude_Index < 77)) { 
                MIDI[f] = 87; }
        else if ((Peak_Magnitude_Index >= 82) && (Peak_Magnitude_Index < 87)) { 
                MIDI[f] = 88; }
        else if ((Peak_Magnitude_Index >= 87) && (Peak_Magnitude_Index < 92)) {
                MIDI[f] = 89; }
        else if ((Peak_Magnitude_Index >= 92) && (Peak_Magnitude_Index < 98)) {
                MIDI[f] = 90; }
        else if ((Peak_Magnitude_Index >= 98) && (Peak_Magnitude_Index < 103)) {
                MIDI[f] = 91; }
        else {
                MIDI[f] = 0; } 
		
		if(Peak_Magnitude_Value <= POWER_THRESHOLD)
		{
			MIDI[f] = 0;
		}
		
		Output_MIDI(MIDI[f]); // output the MIDI note through UART.

		if(f == 512) {
			f = 0;
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


