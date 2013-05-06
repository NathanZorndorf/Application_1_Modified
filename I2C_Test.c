/* Author: Nathan Zorndorf
 * Description: Testing and using the I2S
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

#define SAMPLES_PER_SECOND 48000
#define WINDOW_SAMPLE_SIZE 512

unsigned long int i = 0;
unsigned long int j = 0;

Int16 dummy_counter1 = 0;
Int16 dummy_counter2 = 0;

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
Int16 MIDI_value;

int I2C_Test(void) {

	//--- I2C Variables ---
	CSL_I2cSetup	configI2C;
	volatile Uint16	looper;
	//--- I2S Variables ---
		 int dummy_counter3; 
     I2S_Instance		 i2sInstNum; // open
     I2S_OpMode		     opMode;	 // open
     I2S_ChanType 	     chType;     // open	
     CSL_I2sHandle		 hI2S2;		 // setup
   	 I2S_Config     	 configI2S;  // setup
	 
	//--- AIC Variables ---  
	
	//---------- PLL SETUP BEGIN -----------
	PLL_Obj pllObj;
	PLL_Config pllCfg1;
	PLL_Config pllCfg_100MHz    = {0x82FA, 0x8000, 0x0806, 0x0000};
	PLL_Config *pConfigInfo;
	PLL_Handle hPll;
	CSL_Status status;
	Uint16 test_reg_val;
	int i = 0;
	
    // Enable clocks to all peripherals 
    SYS_PCGCR1 = 0x0000;
    SYS_PCGCR2 = 0x0000;


//---------- I2C SETUP BEGIN -------------
/*	printf("\nI2C SETUP BEGIN\n\n");
// Enable and initalize the I2C module - The I2C clk is set to run at 20 KHz 
       
    I2C_MDR = 0x0400;             // Reset I2C. I2C = Master. The I2C is a master and generates the serial clock on the SCL pin.
    I2C_PSC   = 20;               // I2C clock frequency = I2C input clock frequency/(IPSC + 1) - also creates the value d used below
        				// I2C Serial CLK Frequency = prescaled Module CLK Freq / [(ICCL + d) = (ICCH + d)]
    I2C_CLKL  = 20;               // ICCL
    I2C_CLKH  = 20;               // ICCH 
    I2C_MDR   = 0x0420   ;        // Release from reset; Master, Transmitter, 7-bit address

	USBSTK5505_wait( 100 );  // Wait  
*/

	// ------------------------ I2C Setup ---------------------------- //
	printf("I2C SETUP BEGIN!!\n");
	*(ioport volatile unsigned *) 0x1c00 = 0x2 << 10;	// Select GPIO10
	*(ioport volatile unsigned *) 0x1c06 |= 0x400;		// Set GPIO-10 as output
	for(i=1;i<10;i++ )
		asm("	nop");
	*(ioport volatile unsigned *)0x1c0A |= 0x000;	// Set GPIO-10 = 1
	for(i=1;i<100;i++ )
		asm("	nop");
	*(ioport volatile unsigned *)0x1c0A |= 0x400;	// Set GPIO-10 = 1
	for(i=1;i<10;i++ )
		asm("	nop");
		
	// Initialize I2C module instance //
	status = I2C_init(CSL_I2C0); // CSL_I2C0 means 0 = instance number 
	if(status != CSL_SOK)
	{
		printf("I2C Init Failed!!\n");
		return(status);
	}

	// Setup I2C module //
	configI2C.addrMode    = CSL_I2C_ADDR_7BIT;
	configI2C.bitCount    = CSL_I2C_BC_8BITS;
	configI2C.loopBack    = CSL_I2C_LOOPBACK_DISABLE;
	configI2C.freeMode    = CSL_I2C_FREEMODE_DISABLE;
	configI2C.repeatMode  = CSL_I2C_REPEATMODE_DISABLE;
	configI2C.ownAddr     = 0x2F; // I2C own slave address - don't care if master.
	configI2C.sysInputClk = 100; // Value of system clock in MHz 
	configI2C.i2cBusFreq  = 10; // I2C bus frequency in KHz- a number between 10 and 400. 

	status = I2C_setup(&configI2C);
	if(status != CSL_SOK)
	{
		printf("I2C Setup Failed!!\n");
		return(status);
	}
	printf("I2C SETUP END!!\n\n");
    // --------------------------------------------------------------- //
//---------- I2C SETUP END ---------------

    printf("AIC3204 SETUP BEGIN\n");
    // Configure Parallel Port 
    SYS_EXBUSSEL = 0x1000;  // Configure Parallel Port mode = 1 for I2S2 (SPI, GPIO, UART, and I2S2).
    // Define Starting Point   - OK
    AIC3204_rset( 0, 0);      // Select PAGE 0	   - OK
    AIC3204_rset(0x01, 0x01);      // Software Reset codec - OK
    // Clock Settings 		   - ?
    printf("PLL and Clocks config and Power Up...\n");
    AIC3204_rset( 0, 0);  // Select PAGE 0	   - OK
    AIC3204_rset(0x1B, 0x0D);  // Audio Interface Setting Register 1 (P0_R27) - OK 
		// I2S, 16bit word length, BCLK and WCLK is set as o/p from AIC3204, DOUT will be high impedance after data has been transferred
    		//AIC3204_rset(0x1B, 0x1D); // (aic3204_init says word = 20 bits (0x1D)), WCLK and BCLK output 
    AIC3204_rset(0x1C, 0x00);  // Data offset = 0 - OK
    AIC3204_rset(0x04, 0x43);  // MCLK = PLL_CLKIN => *R*J.D/p => PLL_CLK => PLLCLK => CODEC_CLKIN and HIGH PLL clock range  - OK 
    		//AIC3204_rset(0x04, 0x03 ); // (they say 0x03 = low PLL clock range)
    AIC3204_rset(0x05, 0x91);  // Power up PLL, P=1, R=1 - OK
   			//AIC3204_rset(0x05, 0x91 );   //PLL setting: Power up PLL, P=1 and R=1 
   	AIC3204_rset(0x06, 0x07);  // J = 7 - OK
    	    //AIC3204_rset(0x06, 0x07 );      // PLL setting: J=7
    AIC3204_rset(0x07, 0x06);  // D (MSBits) D=1680 - OK
   		    //AIC3204_rset(0x07, 0x06 );   // PLL setting: HI_BYTE(D)
   	AIC3204_rset(0x08, 0x90);  // D (LSBits)  		- OK
        	//AIC3204_rset(0x08, 0x90 );   // PLL setting: LO_BYTE(D)
    AIC3204_rset(0x1E, 0x88);  // BCLK Power UP/down, and BLCK divider value = 8 - BCLK is output to I2S2_CLK on CPU
                               // BCLK=DAC_CLK/N =(12288000/8) = 1.536MHz = 32*fs - 
    AIC3204_rset(0x0B, 0x87);  	   // NDAC = 7   - ?
    AIC3204_rset(0x0C, 0x82);  	   // MDAC = 2   - ?        
    AIC3204_rset(0x0D, 0x00);      // DOSR(MSbits) = 0					   - OK
    AIC3204_rset(0x0E, 0x80);      // DOSR(LSbits) = 128 decimal or 0x0080 - OK 
    AIC3204_rset(0x12, 0x87);  	   // NADC = 7   - OK 
	AIC3204_rset(0x13, 0x82); 	   // MADC = 2   - OK
	AIC3204_rset(0x14, 0x80);	   // AOSR = 128 - OK
	AIC3204_rset(0x3D, 0x01);	   // ADC PRB_R1 - OK
	// Program Analog Blocks   - ? 
    printf("Program Analog Blocks...\n");
    AIC3204_rset( 0, 1);      // Select PAGE 1	   - OK
	AIC3204_rset(0x01, 0x08);	   // Disable Internal Crude AVdd - OK
	AIC3204_rset(0x02, 0x01);	   // Enable Master Analog Power Control - AVDD LDO o/p = DVDD LDO o/p = 1.72V,power up AVDD LDOEnable Analog Blocks, use LDO power - ?
	//AIC3204_rset(0x0A, 0x00);	   // Set the input common mode to 0.9V - ?
		// Nothing 
	//AIC3204_rset(0x3D, 0x00);	   // Select ADC PTM_R4 (for power savings) - OK
		// Nothing 
	//AIC3204_rset(0x47, 0x32);	   // Set MicPGA startup delay to 3.1ms - OK
		// Nothing 
	//AIC3204_rset(0x7B, 0x01);	   // Set the REF charging time to 40ms - OK  
		// Nothing 
	// DAC Routing and Power Up - OK
	printf("DAC ROUTING and Power Up... \n"); 
	AIC3204_rset( 0, 1); 	   //Select  PAGE 1	   - OK
	AIC3204_rset(0x0C, 0x08); 	   //Left Channel DAC reconstruction filter's positive terminal is routed to HPL 	- OK
	AIC3204_rset(0x0D, 0x08); 	   //Right Channel DAC reconstruction filter's positive terminal is routed to HPL	- OK
	AIC3204_rset( 0, 0); 	   //Select PAGE 0	   - OK
	AIC3204_rset(0x40, 0x02); 	   //DAC unmute, right volume tracks left	- OK
	AIC3204_rset(0x41, 0x00); 	   //DAC Left Volume = 0dB	- OK
	AIC3204_rset(0x3F, 0xD4); 	   // - OK
			//R&L DAC data = R&L Channel Audio Interface Data, Soft-Stepping is 1 step per 1 DAC Word Clock, R&L DAC Power Up
	AIC3204_rset( 0, 1);      	 // Select PAGE 1 			- OK
    AIC3204_rset(0x10, 0x3A );  	 // Unmute HPL , -6 dB gain	- OK
    AIC3204_rset(0x11, 0x3A );  	 // Unmute HPR , -6 dB gain	- OK
    AIC3204_rset(0x09, 0x30);    // Power up HPL,HPR		- OK
	// ADC Routing		 		- OK 
	printf("ADC Routing...\n"); 
	AIC3204_rset( 0, 1 ); 	   //Select  PAGE 1	   - OK
	AIC3204_rset(0x33, 0x00); 	   // MICBIAS power down - MCBIAS is for "electret-condenser microphones," we are using a dynamic mic - OK
			//AIC3204_rset(0x33, 0x48);  // power up MICBIAS with AVDD (0x40)or LDOIN (0x48)
	AIC3204_rset(0x34, 0x30); 	   // IN2L routed to Left MICPGA (positive terminal) via 40kOhm = -12 dB gain	- OK
	AIC3204_rset(0x37, 0x30); 	   // IN2R routed to Right MICPGA (positive terminal)via 40kOhm = -12 dB gain	- OK	
	    	//AIC3204_rset(0x34, 0x10 );// STEREO 1 Jack - IN2_L to LADC_P through 10 kohm
    		//AIC3204_rset(0x37, 0x10 );// 			   - IN2_R to RADC_P through 10 kohmm
	AIC3204_rset(0x36, 0x03); 	   // CM2L routed to left MICPGA (negative terminal) via  40kOhm 				- OK
	AIC3204_rset(0x39, 0x03); 	   // CM2R routed to right MICPGA (negative terminal)via 40kOhm				- OK
	   	 	//AIC3204_rset(0x36, 0x01 ); // CM_1 (common mode) to LADC_M through 10 kohm
    		//AIC3204_rset(0x39, 0x40 ); // CM_1 (common mode) to RADC_M through 10 kohm
	AIC3204_rset(0x3B, 12); 	   // MICPGA (left)  enabled, register 59, value 0x28(40d) = +20 dB gain 	- OK
	AIC3204_rset(0x3C, 12); 	   // MICPGA (right) enabled, register 60, value 0x28(40d) = +20 dB gain 	- OK
	// ADC Power Up				- OK 
	printf("ADC Power Up...\n"); 
	AIC3204_rset( 0, 0 );	   // Select page 0													- OK	
	AIC3204_rset(0x51, 0xC0); 	   // Left and Right channel ADC power up							- OK
	AIC3204_rset(0x52, 0x00); 	   // Left and Right ADC unmuted, fine gain on both channels = 0dB	- OK
	
    USBSTK5505_wait( 100 );  // Wait
    
//---------- AUDIO SAMPLING BEGINS HERE ---------
    printf("\n\nRunning Getting Started Project\n");
    printf( "<-> Audio Loopback from Stereo IN --> to HP/Lineout\n" );
	
	// Setup sampling frequency and 30dB gain for microphone //
    //set_sampling_frequency_and_gain(SAMPLES_PER_SECOND, 0);	
    // I2S settings //
    //I2S2_SRGR = 0x0;      // No effect when I2S is in Slave Mode. otherwise sets FSDIV (0=>8) and CLKDIV(0=>2). 
    //I2S2_CR = 0x8010;     // Address = 0x2A00
    						// I2S/LJustified, I2S = Slave (I2Sn_CLK and I2Sn_FS pins = inputs) The bitclock
							// and frame-synchronization signals are derived from an external source and are provided
							// directly to the I2S synchronizer without being further divided.)
							// 16-bit data word
							// Sign Extension Disabled 
							// Data Pack mode Disabled 
							// 1 Bit delay 
							// Clock Polarity => RX data sampled on rising (leading?) edge & TX data shifted on falling (trailing?) edge of the bit clock. 
							// FS Polarity    => Frame-synchronization pulse is low for left word and high for right word - OK!!
							// Loopback Disabled 
							// Stereo Mode 
							// I2S Enabled 
    //I2S2_ICMR = 0x3f;    // Register 0x2A14 - Enable interrupts
	//I2S2_ICMR = 0x2B;      // Register 0x2A14 - Enable interrupts for TX/RX stereo, and enable FERR and OUERR flags 
	
// ------------------------ I2S2 Setup ------------------------ //
     printf("\nI2C SETUP BEGIN\n");
     i2sInstNum = I2S_INSTANCE2; 	// - OK
     opMode 	= DMA_INTERRUPT; 	// - ?
     chType 	= I2S_CHAN_STEREO;  // - OK
     hI2S2 		= I2S_open(i2sInstNum, opMode, chType); // - OK
     
     configI2S.dataType     = I2S_STEREO_ENABLE; // - OK
     configI2S.loopBackMode = I2S_LOOPBACK_DISABLE; // - OK
     configI2S.fsPol 		= I2S_FSPOL_LOW;	// Left chan. is transmitted when fs is LOW/high - OK
     configI2S.clkPol		= I2S_RISING_EDGE;	// TX on FALLING/RISING clock edge - ?
     configI2S.datadelay	= I2S_DATADELAY_ONEBIT; // - OK
     configI2S.datapack     = I2S_DATAPACK_DISABLE; // - OK
     configI2S.signext		= I2S_SIGNEXT_DISABLE; // Set sign extension of data - OK
     configI2S.wordLen      = I2S_WORDLEN_16; // - OK
     configI2S.i2sMode      = I2S_SLAVE; // I2S is SLAVE - OK 
     configI2S.dataFormat   = I2S_DATAFORMAT_LJUST; // - OK
     	configI2S.clkDiv		= I2S_CLKDIV2;	// I2Sn_CLK = SystemClock / I2S_CLKDIV - Data bit rate - ?
	 	configI2S.fsDiv			= I2S_FSDIV8;	// I2Sn_FS = I2Sn_CLK / I2S_FSDIV     - Data word rate = Sampling Rate -  ? (2FSDIV+3 >= 2 * WDLNGTH (for stereo mode))
	 configI2S.FError		= I2S_FSERROR_ENABLE; // - OK
	 configI2S.OuError		= I2S_OUERROR_ENABLE; // - OK
     
     status = I2S_setup(hI2S2, &configI2S);
     if (status != CSL_SOK) {
     	printf("I2S ERROR: Could not setup I2S.\n");
     	return 0;
     } else {
     	printf("I2S configuration successful!\n");
     }
     
	printf("I2SSRATE = %X \n", 	I2S2_SRGR); // Print value of  I2S2_SRGR : I2S2 Sample Rate Generator Register
	printf("I2SSCTRL = %X \n", 	I2S2_CR); // Print value of I2S2_CR : I2S2 Serializer Control Register
   
     status = I2S_transEnable(hI2S2, TRUE);
	 if (status != CSL_SOK) {
	 	printf("I2S ERROR: Could not enable data transfer!\n");
	 	return 0;
	 } else {
	 	printf("I2S data transfer bit enabled!\n");
	 }

	printf("I2SSRATE = %X \n", 	I2S2_SRGR); // Print value of  I2S2_SRGR : I2S2 Sample Rate Generator Register
	printf("I2SSCTRL = %X \n", 	I2S2_CR); // Print value of I2S2_CR : I2S2 Serializer Control Register  
	printf("I2SINTMASK = %X \n", I2S2_ICMR); // Print value of I2S2_ICMR : I2S2 Interrupt Mask Register     
	printf("I2S SETUP END \n\n");
    //--- I2S END

  
    asm(" bclr XF"); // WTF THIS DO??? - I think it clears the register corresponding to the XF testpoint pin on the dev board. 
   
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
		 	
			//printf("I2S2 Receive Left Data 0  (LSW) Register = %x \n", 	I2S2_W0_LSW_R);
	 	 	//printf("I2S2 Receive Left Data 1 (MSW) Register = %x \n",  I2S2_W0_MSW_R); 
	    	//printf("I2S2 Receive Right Data 0  (LSW) Register = %x \n", 	I2S2_W1_LSW_R);
	 	 	//printf("I2S2 Receive Right Data 1 (MSW) Register = %x \n",	I2S2_W1_MSW_R);
	 	 	
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
		    
		 } // End for loop  
		//---------- AUDIO SAMPLING ENDS HERE -----------
	
		/*-------------- DSP BEGINS HERE --------------*/
		
 	 			
		//MIDI_value = audio_to_midi(audio_buffer);

		/*--------------  DSP ENDS HERE  ---------------*/	
	
	} // End while loop
	
} //I2C_Test.c 

