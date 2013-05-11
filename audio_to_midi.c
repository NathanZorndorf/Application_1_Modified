/* Author: Nathan Zorndorf
 * Description: The main DSP function that uses audio data, takes the FFT and peak searches 
 * to find the waveforms current fundamental frequency. 
 */
 
#include <stdio.h> 
#include <my_types.h>
#include <math.h> 
#include <tms320.h>
#include <Dsplib.h>

#define SAMPLE_RATE 48000
#define WINDOW_SAMPLE_SIZE 512
#define NEXT_POW_TWO 10 // 2^(NEXT_POW_TWO) = WINDOW_SAMPLE_SIZE

 int audio_to_midi(complexNum audio_buffer[]) {
 	
 	int i = 0;
 	int j = 0;
 	long NFFT = 0;
 	long PSD_Result[WINDOW_SAMPLE_SIZE];
 	int Peak_Magnitude_Value = 0;
 	int Peak_Magnitude_Index;
 	int Peak_Found = 0;
 	int MIDI_value = 0; 
	 
	 // FFT
	cfft((DATA*) audio_buffer, WINDOW_SAMPLE_SIZE, SCALE);
	
	// Complex bit-reverse the FFT result for a linear-order array 
	cbrev((DATA*) audio_buffer, (DATA*) audio_buffer, WINDOW_SAMPLE_SIZE);
	
	// Scale FFT result based on WINDOW_SAMPLE_SIZE
	for( i = 0; i < WINDOW_SAMPLE_SIZE; i++ )
	{
		audio_buffer[i].real = audio_buffer[i].real/(WINDOW_SAMPLE_SIZE);
		audio_buffer[i].real = audio_buffer[i].real/(WINDOW_SAMPLE_SIZE);
	}
	
	// Find magnitude of FFT result for each index 
	for( i = 0; i < WINDOW_SAMPLE_SIZE; i++ )
	{
		PSD_Result[i] = sqrt((audio_buffer[i].real)^2 + (audio_buffer[i].imag)^2);
	}
	
	// Peak search on the magnitude of the FFT to find the fundamental frequency  
	Peak_Magnitude_Value = PSD_Result[0];
	
	for ( j = 1; j < WINDOW_SAMPLE_SIZE; j++ ) {
		if( PSD_Result[j] > Peak_Magnitude_Value )
		{
			Peak_Magnitude_Value = PSD_Result[j];
			Peak_Magnitude_Index = j;
		} // if
	} // for 

	printf("\nMaximum element is present at location %d and it's value is %d.\n\n", Peak_Magnitude_Index, Peak_Magnitude_Value);
	
	//free(audio_buffer_copy);
 	return(MIDI_value);
 	
 }
 
 
 





 