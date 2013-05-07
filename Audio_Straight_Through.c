/* Author: Nathan Zorndorf
 * Description: Takes in Audio input from the ADC as stereo, creates a mono buffer out of them, and passes that through to the DAC
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
#include <audio_to_midi.h>
#include <csl_i2c.h>
#include <csl_i2s.h> 
#include <aic_i2c.h>

#define WINDOW_SAMPLE_SIZE 512
unsigned long int i = 0;
unsigned long int j = 0;

Int16 dummy_counter1 = 0;
Int16 dummy_counter2 = 0;
Int16 dummy_counter3 = 0;

volatile Int16 dummy;
signed long temp;

Int16 left_input;
Int16 left_input_2;
Int16 right_input;
Int16 right_input_2;
Int16 left_output;
Int16 left_output_2;
Int16 right_output;
Int16 right_output_2;
Int16 mono_input;

complexNum audio_buffer[WINDOW_SAMPLE_SIZE];

void Audio_Straight_Through(void) {
	//while(j < 10000)
	while(1)
	{  
	 	for ( i = 0  ; i < WINDOW_SAMPLE_SIZE   ;i++  )
		 	{
		 		
			/* Read Digital audio inputs from Codec */
		    while(!(I2S2_IR & RcvR) ) // ! = NO Stereo receive interrupt pending. DO NOT Read Receive Left and Right data 0 and 1 registers.
		    {
		    	dummy_counter1++; // Wait for receive interrupt 
		  		//printf("stuck in while(!(I2S2_IR & RcvR)\n");
		    }	
		  	
		    //printf("RcvR Interrupt occurred!)\n");
		    left_input = I2S2_W0_MSW_R;        		// Read Most Significant Word of first channel
		    left_input_2 = I2S2_W0_LSW_R;           // Read Least Significant Word (ignore) 
		    right_input = I2S2_W1_MSW_R;        	// Read Most Significant Word of second channel
		    right_input_2 = I2S2_W1_LSW_R;          // Read Least Significant Word of second channel (ignore)
		 	
		 	dummy_counter3++;
		 	if(dummy_counter3 == 48000) 
		 	{
		 	printf("# of RcvR interrupts = %d \n",  dummy_counter1);	
			printf("I2S2 Receive Left Data 0  (LSW) Register = %x \n", 	I2S2_W0_LSW_R);
	 	 	printf("I2S2 Receive Left Data 1 (MSW) Register = %x \n",  I2S2_W0_MSW_R); 
	    	printf("I2S2 Receive Right Data 0  (LSW) Register = %x \n", 	I2S2_W1_LSW_R);
	 	 	printf("I2S2 Receive Right Data 1 (MSW) Register = %x \n",	I2S2_W1_MSW_R);
		 	}
			 /* Take average of left and right channels. */
			 temp = (signed long) left_input + (signed long) right_input;
			 temp >>= 1;    /* Divide by 2 to prevent overload at output */
			 
			 /* assign averaged and divided stero input to mono input */
			 mono_input = temp;	 
			 
			 audio_buffer[i].real = mono_input; 	
			 audio_buffer[i].imag = 0; 
			 
			 
		     left_output =  left_input;            // Very simple inter-sample processing. Directly connect inputs to outputs. Replace w/ code!
		     right_output = right_input;         
	
			
		    while( !(I2S2_IR & XmitR) ) // ! = NO Stereo transmit interrupt pending. DO NOT Write new data value to I2S Transmit Left and Right data 0 and 1 registers.
		    {
		   		dummy_counter2++; // Wait for transmit interrupt
		    }	
			
			I2S2_W0_MSW_W = left_output;         // Left output       
		    I2S2_W0_LSW_W = left_output_2;
		    I2S2_W1_MSW_W = right_output;        // Right output
		    I2S2_W1_LSW_W = right_output_2;
		    
		    if((I2S2_IR & OUERRFL) == 1)
		    {
		    	printf("ERROR: The data registers were not read from or written to before the receive/transmit buffer was overwritten!!!\n");
		    }
		    if((I2S2_IR & FERRFL) == 1)
		    {
		    	printf("ERROR: Frame-synchronization error(s) occurred!!!\n");
		    }
		    
		    j++;
		    
		 } // End for loop  
		//---------- AUDIO SAMPLING ENDS HERE -----------
	
	} // End while loop
} //Audio_Straight_Through.c
