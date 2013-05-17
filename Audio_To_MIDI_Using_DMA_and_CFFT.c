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
#pragma DATA_SECTION(PSD_Result_sqrt, "PSD_sqrt");
Int16 PSD_Result_sqrt[FFT_LENGTH];
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
LDATA complex_data[2*FFT_LENGTH];
/* -------------------------------------------*/

int Audio_To_MIDI_Using_DMA_and_CFFT(void) {

	int i = 0;
	int j = 0;
	int f = 0;
	DATA Peak_Magnitude_Value = 0;
	DATA Peak_Magnitude_Index = 0;
	DATA Peak_Magnitude_Value_Array[FFT_LENGTH] = {0};
	DATA Peak_Magnitude_Index_Array[FFT_LENGTH] = {0};
	unsigned short oflag = 0; // overflow flag for power function 
	
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
		
		for(i = 0; i < FFT_LENGTH; i++) { 
			PSD_Result_sqrt[i] = PSD_Result[i];
		}
		
		

		
		// Find value and index of maximum value of vector x
		//maxvec(PSD_Result, FFT_LENGTH, DATA *r_val, DATA *r_idx)
		// Initialize index and value variables 
		//Peak_Magnitude_Index = 0; 
		//Peak_Magnitude_Value = 0;
		//maxvec(PSD_Result, FFT_LENGTH, &Peak_Magnitude_Value, &Peak_Magnitude_Index);
		
		// Process freq. bins from 0Hz to Nyquist frequency (for efficiency) 
		Peak_Magnitude_Value = PSD_Result_sqrt[1]; // start the search at the first value in the Magnitude spectrum
		Peak_Magnitude_Index = 1;
		for( j = 2; j < NUM_BINS; j++ ) // go through useful frequency bins (FFT_LENGTH/2 +1) because the rest are symmetric
		{ 
			if( PSD_Result_sqrt[j] > Peak_Magnitude_Value ) // Peak search on the magnitude of the FFT to find the fundamental frequency - the frequency bin with the highest power value in it 
			{
				Peak_Magnitude_Value = PSD_Result[j];
				Peak_Magnitude_Index = j;
			}
		}
		
		// Store the peak search results in an array, for testing purposes 
		/*
		Peak_Magnitude_Index_Array[f] = Peak_Magnitude_Index;
		Peak_Magnitude_Value_Array[f] = Peak_Magnitude_Value;
		f = f + 1;
		if(f == FFT_LENGTH) {
			f = 0;
		}
		*/
		
		/*
				f++;
		if ((Peak_Magnitude_Index >= 28.315) && (Peak_Magnitude_Index < 30.001)) {
                MIDI[f] = 22; }
        else if ((Peak_Magnitude_Index >= 30.001) && (Peak_Magnitude_Index < 31.785)) {
                MIDI[f] = 23; }
        else if ((Peak_Magnitude_Index >= 31.785) && (Peak_Magnitude_Index < 33.675)) {
                MIDI[f] = 24; }
        else if ((Peak_Magnitude_Index >= 33.675) && (Peak_Magnitude_Index < 35.677)) {
                MIDI[f] = 25; }
        else if ((Peak_Magnitude_Index >= 35.677) && (Peak_Magnitude_Index < 37.799)) { 
                MIDI[f] = 26; }
        else if ((Peak_Magnitude_Index >= 37.799) && (Peak_Magnitude_Index < 40.046)) {
                MIDI[f] = 27; }
        else if ((Peak_Magnitude_Index >= 40.046) && (Peak_Magnitude_Index < 42.428)) {
                MIDI[f] = 28; }
        else if ((Peak_Magnitude_Index >= 42.428) && (Peak_Magnitude_Index < 44.94)) {
                MIDI[f] = 29; }
        else if ((Peak_Magnitude_Index >= 44.94) && (Peak_Magnitude_Index < 47.624)) {
                MIDI[f] = 30; }
        else if ((Peak_Magnitude_Index >= 47.624) && (Peak_Magnitude_Index < 50.456)) { 
                MIDI[f] = 31; }
        else if ((Peak_Magnitude_Index >= 50.456) && (Peak_Magnitude_Index < 53.456)) { 
                MIDI[f] = 32; }
        else if ((Peak_Magnitude_Index >= 53.456) && (Peak_Magnitude_Index < 56.635)) {
                MIDI[f] = 33; }
        else if ((Peak_Magnitude_Index >= 56.635) && (Peak_Magnitude_Index < 60.002)) {
                MIDI[f] = 34; }
        else if ((Peak_Magnitude_Index >= 60.002) && (Peak_Magnitude_Index < 63.567)) {
                MIDI[f] = 35; }
        else if ((Peak_Magnitude_Index >= 63.567) && (Peak_Magnitude_Index < 67.35)) {
                MIDI[f] = 36; }
        else if ((Peak_Magnitude_Index >= 67.35) && (Peak_Magnitude_Index < 71.355)) { 
                MIDI[f] = 37; }
        else if ((Peak_Magnitude_Index >= 71.355) && (Peak_Magnitude_Index < 75.598)) {
                MIDI[f] = 38; }
        else if ((Peak_Magnitude_Index >= 42.428) && (Peak_Magnitude_Index < 44.94)) {
                MIDI[f] = 29; }
        else if ((Peak_Magnitude_Index >= 44.94) && (Peak_Magnitude_Index < 47.624)) {
                MIDI[f] = 30; }
        else if ((Peak_Magnitude_Index >= 47.624) && (Peak_Magnitude_Index < 50.456)) { 
                MIDI[f] = 31; }
        else if ((Peak_Magnitude_Index >= 50.456) && (Peak_Magnitude_Index < 53.456)) { 
                MIDI[f] = 32; }
        else if ((Peak_Magnitude_Index >= 53.456) && (Peak_Magnitude_Index < 56.635)) {
                MIDI[f] = 33; }
        else if ((Peak_Magnitude_Index >= 56.635) && (Peak_Magnitude_Index < 60.002)) {
                MIDI[f] = 34; }
        else if ((Peak_Magnitude_Index >= 60.002) && (Peak_Magnitude_Index < 63.567)) {
                MIDI[f] = 35; }
        else if ((Peak_Magnitude_Index >= 63.567) && (Peak_Magnitude_Index < 67.35)) {
                MIDI[f] = 36; }
        else if ((Peak_Magnitude_Index >= 67.35) && (Peak_Magnitude_Index < 71.355)) { 
                MIDI[f] = 37; }
        else if ((Peak_Magnitude_Index >= 71.355) && (Peak_Magnitude_Index < 75.598)) {
                MIDI[f] = 38; }

        else if ((Peak_Magnitude_Index >= 75.598) && (Peak_Magnitude_Index < 80.093)) {
                MIDI[f] = 39; }
        else if ((Peak_Magnitude_Index >= 80.093) && (Peak_Magnitude_Index < 84.856)) { 
                MIDI[f] = 40; }
        else if ((Peak_Magnitude_Index >= 84.856) && (Peak_Magnitude_Index < 89.902)) { 
                MIDI[f] = 41; }
        else if ((Peak_Magnitude_Index >= 89.902) && (Peak_Magnitude_Index < 95.248)) {
                MIDI[f] = 42; }
        else if ((Peak_Magnitude_Index >= 95.248) && (Peak_Magnitude_Index < 100.912)) {
                MIDI[f] = 43; }
        else if ((Peak_Magnitude_Index >= 100.912) && (Peak_Magnitude_Index < 106.913)) {
                MIDI[f] = 44; }
        else if ((Peak_Magnitude_Index >= 106.913) && (Peak_Magnitude_Index < 113.27)) {
                MIDI[f] = 45; }
        else if ((Peak_Magnitude_Index >= 113.27) && (Peak_Magnitude_Index < 120.005)) { 
                MIDI[f] = 46; }
        else if ((Peak_Magnitude_Index >= 120.005) && (Peak_Magnitude_Index < 127.141)) {
                MIDI[f] = 47; }
        else if ((Peak_Magnitude_Index >= 127.141) && (Peak_Magnitude_Index < 134.702)) {
                MIDI[f] = 48; }

        else if ((Peak_Magnitude_Index >= 134.702) && (Peak_Magnitude_Index < 142.712)) {
                MIDI[f] = 49; }
        else if ((Peak_Magnitude_Index >= 142.712) && (Peak_Magnitude_Index < 151.198)) { 
                MIDI[f] = 50; }
        else if ((Peak_Magnitude_Index >= 151.198) && (Peak_Magnitude_Index < 160.188)) { 
                MIDI[f] = 51; }
        else if ((Peak_Magnitude_Index >= 160.188) && (Peak_Magnitude_Index < 169.714)) {
                MIDI[f] = 52; }
        else if ((Peak_Magnitude_Index >= 169.714) && (Peak_Magnitude_Index < 179.806)) {
                MIDI[f] = 53; }
        else if ((Peak_Magnitude_Index >= 179.806) && (Peak_Magnitude_Index < 190.497)) {
                MIDI[f] = 54; }
        else if ((Peak_Magnitude_Index >= 190.497) && (Peak_Magnitude_Index < 201.825)) {
                MIDI[f] = 55; }
        else if ((Peak_Magnitude_Index >= 201.825) && (Peak_Magnitude_Index < 213.826)) { 
                MIDI[f] = 56; }
        else if ((Peak_Magnitude_Index >= 213.826) && (Peak_Magnitude_Index < 226.541)) {
                MIDI[f] = 57; }
        else if ((Peak_Magnitude_Index >= 226.541) && (Peak_Magnitude_Index < 240.011)) {
                MIDI[f] = 58; }


        else if ((Peak_Magnitude_Index >= 240.011) && (Peak_Magnitude_Index < 254.283)) {
                MIDI[f] = 59; }
        else if ((Peak_Magnitude_Index >= 254.283) && (Peak_Magnitude_Index < 269.404)) { 
                MIDI[f] = 60; }
        else if ((Peak_Magnitude_Index >= 269.404) && (Peak_Magnitude_Index < 285.423)) { 
                MIDI[f] = 61; }
        else if ((Peak_Magnitude_Index >= 285.423) && (Peak_Magnitude_Index < 302.395)) {
                MIDI[f] = 62; }
        else if ((Peak_Magnitude_Index >= 302.395) && (Peak_Magnitude_Index < 320.377)) {
                MIDI[f] = 63; }
        else if ((Peak_Magnitude_Index >= 320.377) && (Peak_Magnitude_Index < 339.428)) {
                MIDI[f] = 64; }
        else if ((Peak_Magnitude_Index >= 339.428) && (Peak_Magnitude_Index < 359.611)) {
                MIDI[f] = 65; }
        else if ((Peak_Magnitude_Index >= 359.611) && (Peak_Magnitude_Index < 380.995)) { 
                MIDI[f] = 66; }
        else if ((Peak_Magnitude_Index >= 380.995) && (Peak_Magnitude_Index < 403.65)) {
                MIDI[f] = 67; }
        else if ((Peak_Magnitude_Index >= 403.65) && (Peak_Magnitude_Index < 427.652)) {
                MIDI[f] = 68; }

        else if ((Peak_Magnitude_Index >= 427.652) && (Peak_Magnitude_Index < 453.082)) {
                MIDI[f] = 69; }
        else if ((Peak_Magnitude_Index >= 453.082) && (Peak_Magnitude_Index < 480.023)) { 
                MIDI[f] = 70; }
        else if ((Peak_Magnitude_Index >= 480.023) && (Peak_Magnitude_Index < 508.567)) { 
                MIDI[f] = 71; }
        else if ((Peak_Magnitude_Index >= 508.567) && (Peak_Magnitude_Index < 538.808)) {
                MIDI[f] = 72; }
        else if ((Peak_Magnitude_Index >= 538.808) && (Peak_Magnitude_Index < 570.847)) {
                MIDI[f] = 73; }
        else if ((Peak_Magnitude_Index >= 570.847) && (Peak_Magnitude_Index < 604.791)) {
                MIDI[f] = 74; }
        else if ((Peak_Magnitude_Index >= 604.791) && (Peak_Magnitude_Index < 640.754)) {
                MIDI[f] = 75; }
        else if ((Peak_Magnitude_Index >= 640.754) && (Peak_Magnitude_Index < 678.856)) { 
                MIDI[f] = 76; }
        else if ((Peak_Magnitude_Index >= 678.856) && (Peak_Magnitude_Index < 719.222)) {
                MIDI[f] = 77; }
        else if ((Peak_Magnitude_Index >= 719.222) && (Peak_Magnitude_Index < 761.989)) {
                MIDI[f] = 78; }

        else if ((Peak_Magnitude_Index >= 761.989) && (Peak_Magnitude_Index < 807.3)) {
                MIDI[f] = 79; }
        else if ((Peak_Magnitude_Index >= 807.3) && (Peak_Magnitude_Index < 855.3)) {
                MIDI[f] = 80; }
        else if ((Peak_Magnitude_Index >= 855.3) && (Peak_Magnitude_Index < 906.164)) { 
                MIDI[f] = 81; }
        else if ((Peak_Magnitude_Index >= 906.164) && (Peak_Magnitude_Index < 960.047)) { 
                MIDI[f] = 82; }
        else if ((Peak_Magnitude_Index >= 960.047) && (Peak_Magnitude_Index < 1016.8)) {
                MIDI[f] = 83; }
        else if ((Peak_Magnitude_Index >= 1016.8) && (Peak_Magnitude_Index < 1077)) {
                MIDI[f] = 84; }
        else if ((Peak_Magnitude_Index >= 1077) && (Peak_Magnitude_Index < 1141)) {
                MIDI[f] = 85; }
        else if ((Peak_Magnitude_Index >= 1141) && (Peak_Magnitude_Index < 1209)) {
                MIDI[f] = 86; }
        else if ((Peak_Magnitude_Index >= 1209) && (Peak_Magnitude_Index < 1281)) {
                MIDI[f] = 87; }
        else if ((Peak_Magnitude_Index >= 1281) && (Peak_Magnitude_Index < 1357)) {
                MIDI[f] = 88; }

        else if ((Peak_Magnitude_Index >= 1357) && (Peak_Magnitude_Index < 1438.44)) { 
                MIDI[f] = 89; }
        else if ((Peak_Magnitude_Index >= 1438.44) && (Peak_Magnitude_Index < 1523.98)) {
                MIDI[f] = 90; }
        else if ((Peak_Magnitude_Index >= 1523.98) && (Peak_Magnitude_Index < 1614.6)) { 
                MIDI[f] = 91; }
        else if ((Peak_Magnitude_Index >= 1614.6) && (Peak_Magnitude_Index < 1710.61)) { 
                MIDI[f] = 92; }
        else if ((Peak_Magnitude_Index >= 1710.61) && (Peak_Magnitude_Index < 1812.33)) {
                MIDI[f] = 93; }
        else if ((Peak_Magnitude_Index >= 1812.33) && (Peak_Magnitude_Index < 1920.09)) {
                MIDI[f] = 94; }
        else if ((Peak_Magnitude_Index >= 1920.09) && (Peak_Magnitude_Index < 2032.27)) {
                MIDI[f] = 95; }
        else if ((Peak_Magnitude_Index >= 2032.27) && (Peak_Magnitude_Index < 2155.23)) {
                MIDI[f] = 96; }
        else if ((Peak_Magnitude_Index >= 2155.23) && (Peak_Magnitude_Index < 2283)) {
                MIDI[f] = 97; }
        else if ((Peak_Magnitude_Index >= 2283) && (Peak_Magnitude_Index < 2419)) {
                MIDI[f] = 98; }

        else if ((Peak_Magnitude_Index >= 2419) && (Peak_Magnitude_Index < 2563)) { 
                MIDI[f] = 99; }
        else if ((Peak_Magnitude_Index >= 2563) && (Peak_Magnitude_Index < 2715)) {
                MIDI[f] = 100; }
        else if ((Peak_Magnitude_Index >= 2715) && (Peak_Magnitude_Index < 2876)) { 
                MIDI[f] = 101; }
        else if ((Peak_Magnitude_Index >= 2876) && (Peak_Magnitude_Index < 3047)) { 
                MIDI[f] = 102; }
        else if ((Peak_Magnitude_Index >= 3047) && (Peak_Magnitude_Index < 3228)) {
                MIDI[f] = 103; }
        else if ((Peak_Magnitude_Index >= 3228) && (Peak_Magnitude_Index < 3421)) {
                MIDI[f] = 104; }
        else if ((Peak_Magnitude_Index >= 3421) && (Peak_Magnitude_Index < 3624)) {
                MIDI[f] = 105; }
        else if ((Peak_Magnitude_Index >= 3624) && (Peak_Magnitude_Index < 3840)) {
                MIDI[f] = 106; }
        else if ((Peak_Magnitude_Index >= 3840) && (Peak_Magnitude_Index < 4068)) {
                MIDI[f] = 107; }
        else {
                MIDI[f] = 0; }
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


