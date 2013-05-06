#include "aic_i2c.h"

CSL_Status initializeI2C(void)
{
	CSL_Status      status;
	CSL_I2cSetup	configI2C;
	volatile Uint16	looper, i;

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

	/* Initialize I2C module */
	status = I2C_init(CSL_I2C0);
	if(status != CSL_SOK)
	{
		printf("I2C Init Failed!!\n");
		return(status);
	}

	/* Setup I2C module */
	configI2C.addrMode    = CSL_I2C_ADDR_7BIT;
	configI2C.bitCount    = CSL_I2C_BC_8BITS;
	configI2C.loopBack    = CSL_I2C_LOOPBACK_DISABLE;
	configI2C.freeMode    = CSL_I2C_FREEMODE_DISABLE;
	configI2C.repeatMode  = CSL_I2C_REPEATMODE_DISABLE;
	configI2C.ownAddr     = CSL_I2C_OWN_ADDR;
	configI2C.sysInputClk = CSL_I2C_SYS_CLK;
	configI2C.i2cBusFreq  = CSL_I2C_BUS_FREQ;

	status = I2C_setup(&configI2C);
	if(status != CSL_SOK)
	{
		printf("I2C Setup Failed!!\n");
		return(status);
	}
	
	return(status);
}

CSL_Status AIC_Write(Uint16 regAddr, Uint16 regValue)
{
	CSL_Status  status;
	Uint16      startStop;
	Uint16      write_buffer[2];
	volatile Uint16    looper;

	startStop = ((CSL_I2C_START) | (CSL_I2C_STOP));

	write_buffer[0] = regAddr;
	write_buffer[1] = regValue;

	/* Write data */
	status = I2C_write(write_buffer, 2,
                       CSL_I2C_CODEC_ADDR, TRUE, startStop,
                       CSL_I2C_MAX_TIMEOUT);
    
    /* Give some delay */              	
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++){;}
                      
	return status;
}

Uint16 AIC_Read(Uint16 regAddr)
{
	CSL_Status  status;
	Uint16      startStop;
	Uint16      read_buffer[2];
	volatile Uint16    looper;

	startStop = ((CSL_I2C_START) | (CSL_I2C_STOP));
	
	/* Read data */
	status = I2C_read(read_buffer, 1, CSL_I2C_CODEC_ADDR,
	                  (Uint16 *)&regAddr, 1, TRUE,
	                  startStop, CSL_I2C_MAX_TIMEOUT, FALSE);
	     
    if (status != CSL_SOK) {
    	printf("I2C Read ERROR!\n");
    	return status;
    }
                       
	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++){;}
   
	return read_buffer[0];
}

CSL_Status setupAIC3204(void) {

	CSL_Status status;
	
	status = initializeI2C();
	if (status != CSL_SOK) {
		printf("Could not initialize I2C!\n");
		return status;
	}
	
	AIC_Write(0x00, 0x00); //Select page 0
	AIC_Write(0x01, 0x01); //Software RESET
	
	AIC_Write(0x00, 0x01); //Select page 1
	AIC_Write(0x01, 0x08); //Disable weak connection of AVDD with DVDD
	AIC_Write(0x02, 0x01); //AVDD=DVDD=1.72V, power up AVDD LDO, etc.
	
	AIC_Write(0x00, 0x00); //Select page 0
	AIC_Write(0x1B, 0x0D); //I2S, 16bits
	
	// --------------- PLL & FS Setup --------------- //
	AIC_Write(0x04, 0x03); //Low PLL range, MCLK input to PLL, PLL CLK is CODEC_CLKIN
		
	AIC_Write(0x06, 0x08); //PLL J = 8
	AIC_Write(0x07, 0x07); //PLL D (MSBs)
	AIC_Write(0x08, 0x80); //PLL D (LSBs)
						   //PLL D = 1920
	AIC_Write(0x1E, 0xA0); //BCLK N = 10
	AIC_Write(0x05, 0x91); //PLL Power Up, P=1, R=1
	
	AIC_Write(0x0D, 0x02); //DAC OSR (MSBs)
	AIC_Write(0x0E, 0x00); //DAC OSR (LSBs)
						   //DOSR = 2
	
	AIC_Write(0x14, 0x80); //AOSR = 128
	
	AIC_Write(0x0B, 0x86); //NDAC = 6
	AIC_Write(0x0C, 0x82); //MDAC = 2
	
	AIC_Write(0x12, 0x88); //NADC = 8
	AIC_Write(0x13, 0x86); //MADC = 6
	
	// --------------- DAC & HPL Setup --------------- //
	AIC_Write(0x00, 0x01); //Select page 1
	AIC_Write(0x0C, 0x08); //Left  DAC positive terminal to HPL
	AIC_Write(0x0D, 0x08); //Right DAC positive terminal to HPR
		
	AIC_Write(0x00, 0x00); //Select page 0
	AIC_Write(0x40, 0x02); //DAC unmute, right volume tracks left
	AIC_Write(0x41, 0x00); //DAC Left Volume = 0dB
	AIC_Write(0x3F, 0xD4); //DAC power up
	
	AIC_Write(0x10, 0x00); //HPL unmuted, gain = 0dB
	AIC_Write(0x11, 0x00); //HPR unmuted, gain = 0dB
	AIC_Write(0x09, 0x30); //HPL, HPR power up
	
	// ----------------- ADC Setup ----------------- //
	AIC_Write(0x00, 0x01); //Select page 1
	
	AIC_Write(0x33, 0x00); //MICBIAS power down
	AIC_Write(0x34, 0x30); //IN2L routed to MICPGA (positive terminal) 40kOhm
	AIC_Write(0x37, 0x30); //IN2R routed to MICPGA (positive terminal) 40kOhm
	AIC_Write(0x36, 0x03); //CM2L routed to MICPGA (negative terminal) 40kOhm
	AIC_Write(0x39, 0x03); //CM2R routed to MICPGA (negative terminal) 40kOhm
	AIC_Write(0x3B, 0x00); //MICPGA (left)  enabled, gain = 0dB
	AIC_Write(0x3C, 0x00); //MICPGA (right) enabled, gain = 0dB
	
	AIC_Write(0x00, 0x00); //Select page 0
	AIC_Write(0x51, 0xC0); //Left/right ADC power up
	AIC_Write(0x52, 0x00); //Left/right ADC unmute, fine gain = 0dB
	
	return status;
}
