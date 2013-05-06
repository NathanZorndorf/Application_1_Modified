/*  ============================================================================
 *   Author: Nathan Zorndorf 
 *   Description: Setting up I2C and AIC3204 Audio Codec
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
#include <csl_i2c.h>
#include <csl_i2s.h> 
#include <aic_i2c.h>

int My_AIC3204(void) {
	
	Uint16 test_reg_val;
	int i;
	
    printf("AIC3204 SETUP BEGIN\n");
    // Configure Parallel Port 
    SYS_EXBUSSEL = 0x1000;  // Configure Parallel Port mode = 1 for I2S2 (SPI, GPIO, UART, and I2S2).
    // Define Starting Point   - OK
    AIC3204_rset( 0, 0);      // Select PAGE 0	   - OK
    AIC3204_rset(0x01, 0x01);      // Software Reset codec - OK
    // Clock Settings 		   - ?
    printf("PLL and Clocks config and Power Up...\n");
    AIC3204_rset( 0, 0);  		// Select PAGE 0	   - OK
    AIC3204_rset(0x1B, 0x0D);  // Audio Interface Setting Register 1 (P0_R27) - OK 
		// I2S, 16bit word length, BCLK and WCLK is set as o/p from AIC3204, DOUT will be high impedance after data has been transferred
    AIC3204_rset(0x1C, 0x00);  // Data offset = 0 - OK
    AIC3204_rset(0x04, 0x43);  // MCLK = PLL_CLKIN => *R*J.D/p => PLL_CLK => PLLCLK => CODEC_CLKIN and HIGH PLL clock range  - OK 
    AIC3204_rset(0x05, 0x91);  // Power up PLL, P=1, R=1 - OK
   	AIC3204_rset(0x06, 0x07);  // J = 7 - OK
    AIC3204_rset(0x07, 0x06);  // D (MSBits) D=1680 - OK
   	AIC3204_rset(0x08, 0x90);  // D (LSBits)  		- OK
    AIC3204_rset(0x1E, 0x88);  // WCLK source, BCLK Power UP/down, and BLCK divider value = 8 - BCLK is output to I2S2_CLK on CPU
                               // BCLK=DAC_CLK/N =(12288000/8) = 1.536MHz, which must be > 32*(FS/WCLK/ADC_FS) because that way  
                               // 32 bits can be transferred per sample 
                               // this setup -> DAC_CLK = WCLK, and BCLK = DAC_CLK/BCLKDividervalue
    AIC3204_rset(0x0B, 0x87);  	   // NDAC = 7   - OK
    AIC3204_rset(0x0C, 0x82);  	   // MDAC = 2   - OK       
    AIC3204_rset(0x0D, 0x00);      // DOSR(MSbits) = 0					   - OK
    AIC3204_rset(0x0E, 0x80);      // DOSR(LSbits) = 128 decimal or 0x0080 - OK 
    AIC3204_rset(0x12, 0x87);  	   // NADC = 7   - OK 
	AIC3204_rset(0x13, 0x82); 	   // MADC = 2   - OK
	AIC3204_rset(0x14, 0x80);	   // AOSR = 128 - OK
	AIC3204_rset(0x3D, 0x01);	   // ADC PRB_R1 - OK
	// Program Analog Blocks   - OK
    printf("Program Analog Blocks...\n");
    AIC3204_rset( 0, 1);      // Select PAGE 1	   - OK
	AIC3204_rset(0x01, 0x08);	   // Disable Internal Crude AVdd - OK
	AIC3204_rset(0x02, 0x01);	   // Enable Master Analog Power Control - AVDD LDO o/p = DVDD LDO o/p = 1.72V,power up AVDD LDOEnable Analog Blocks, use LDO power - ?
	//AIC3204_rset(0x0A, 0x00);	   // Set the input common mode to 0.9V - ?
	//AIC3204_rset(0x3D, 0x00);	   // Select ADC PTM_R4 (for power savings) - OK
	//AIC3204_rset(0x47, 0x32);	   // Set MicPGA startup delay to 3.1ms - OK
	//AIC3204_rset(0x7B, 0x01);	   // Set the REF charging time to 40ms - OK  
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
    AIC3204_rset(0x09, 0x30);    	// Power up HPL,HPR		- OK
	// ADC Routing		 		- OK 
	printf("ADC Routing...\n"); 
	AIC3204_rset( 0, 1 ); 	   //Select  PAGE 1	   - OK
	AIC3204_rset(0x33, 0x00); 	   // MICBIAS power down - MCBIAS is for "electret-condenser microphones," we are using a dynamic mic - OK
	AIC3204_rset(0x34, 0x30); 	   // IN2L routed to Left MICPGA (positive terminal) via 40kOhm = -12 dB gain	- OK
	AIC3204_rset(0x37, 0x30); 	   // IN2R routed to Right MICPGA (positive terminal)via 40kOhm = -12 dB gain	- OK	
	AIC3204_rset(0x36, 0x03); 	   // CM2L routed to left MICPGA (negative terminal) via  40kOhm 				- OK
	AIC3204_rset(0x39, 0x03); 	   // CM2R routed to right MICPGA (negative terminal)via 40kOhm				- OK
	AIC3204_rset(0x3B, 12); 	   // MICPGA (left)  enabled, register 59, value 0x28(40d) = +20 dB gain 	- OK
	AIC3204_rset(0x3C, 12); 	   // MICPGA (right) enabled, register 60, value 0x28(40d) = +20 dB gain 	- OK
	// ADC Power Up				- OK 
	printf("ADC Power Up...\n"); 
	AIC3204_rset( 0, 0 );	   // Select page 0													- OK	
	AIC3204_rset(0x51, 0xC0); 	   // Left and Right channel ADC power up							- OK
	AIC3204_rset(0x52, 0x00); 	   // Left and Right ADC unmuted, fine gain on both channels = 0dB	- OK
	
    USBSTK5505_wait( 100 );  // Wait
    printf("AIC3204 SETUP END\n\n");
    /*
	AIC3204_rset( 0, 0 );	   // Select PAGE 0		
    for(i=0; i < 128; i++)
    {
    	AIC3204_rget(  i , &test_reg_val );
    	printf("AIC3204 Page 0 Register 0x%X = 0x%X.\n", i, test_reg_val);
    	test_reg_val++;
    }
    AIC3204_rset( 0, 1);	// Select PAGE 1
    for(i=0; i < 128; i++)
    {
    	AIC3204_rget(  i , &test_reg_val );
    	printf("AIC3204 Page 1 Register 0x%X = 0x%X.\n", i, test_reg_val);
    	test_reg_val++;
    }
    */
	return(0);
}
