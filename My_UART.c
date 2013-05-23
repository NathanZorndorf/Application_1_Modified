/*  ============================================================================
 *   Author: Nathan Zorndorf 
 *   Description: UART Initialization and Test 
 */

#include <stdio.h>
#include <csl_general.h>
#include <csl_uart.h>
#include <My_PLL.h>
#include <usbstk5505.h>
#include <My_UART.h>
#include <Application_1_Modified_Registers.h>

// UART #define's
#define TIMEOUT_VALUE	1000
#define COUNT 			3 
#define ONE_HALF_SECOND	500000 // 500,000 microseconds in 0.5 seconds

// MIDI #define's 
#define NOTE_ON		144  // 0x90 - Note ON  for Channel 0 - This should be the first byte in a Note ON  sequence. 
#define NOTE_OFF 	128  // 0x80 - Note OFF for Channel 0 - This should be the first byte in a Note OFF sequence. 
#define NOTE_VALUE	96   // 0x60 - 
#define NOTE_ON_VELOCITY	100 // 0x64 - Velocity = 100 (Range is 1 - 127) - This should be the third byte in a Note ON  sequence. 
#define NOTE_OFF_VELOCITY	127 // 0x7F - Velocity = 127 (Range is 1 - 127) - This should be the third byte in a Note OFF sequence. 

CSL_UartObj 	uartObj; // Used to create the UART instance handle. 
CSL_UartHandle	hUart;	 // The UART instance handle. 
	
int My_UART(void){
	
	// UART Variable Declaration 
	int j = 0;
	Int16           status;  // For checking if functions ran successfully. 
	CSL_UartConfig	Config;
	char pBuf[3];  // UART TX Buffer - char is 1 byte - ?
	
		CSL_UartSetup uartSetup =
		{
			// Input clock freq in MHz 
		    100000000,
			// Baud rate 
		    31250,
			// Word length of 8 
		    CSL_UART_WORD8,
			// To generate 1 stop bit 
		    0, 
			// Disable the parity 
		    CSL_UART_DISABLE_PARITY,
			// Enable trigger 14 fifo 
			CSL_UART_FIFO_DISABLE, // - ??
			// Loop Back enable 
		    CSL_UART_NO_LOOPBACK,
			// No auto flow control
			CSL_UART_NO_AFE , // - does not matter, choose OFF anyway.
			// No RTS 
			CSL_UART_NO_RTS , // - does not matter because AFE is OFF, choose NO_RST anyway. 
		};
	
	
    Config.DLL = 200;  // Set baud rate
    Config.DLH = 0;
    Config.FCR = 0x0000;            // Clear UART TX & RX FIFOs
    Config.LCR = 0x0003;            // 8-bit words, 1 STOP bit generated, No Parity, No Stick paritiy, No Break control
    Config.MCR = 0x0000;            // RTS & CTS disabled, Loopback mode disabled, Autoflow disabled
	

	 
    // Initialize UART
	status = UART_init(&uartObj, CSL_UART_INST_0, UART_POLLED); 
    	if(CSL_SOK != status) { printf("UART_init failed error code %d\n",status); return(status); }
    	else { printf("UART_init Successful\n"); }
    	
	hUart = (CSL_UartHandle)&uartObj;

    /*
    status = UART_reset(hUart);
    if(CSL_SOK != status) { printf("UART_reset failed! %d\n",status); return(status); }
    	else { printf("UART_reset Successful\n"); }
    */  
    
    
	// UART Setup
    status = UART_setup(hUart, &uartSetup);
    	if(CSL_SOK != status) { printf("UART_setup failed error code %d\n",status); return(status); }
    	else { printf("UART_setup Successful\n"); }
    
    // External Bus Selection Register 
	CPU_EBSR = 0x1000; // Mode 1 (SPI, GPIO, UART, and I2S2) - OK
	
    /*
	// UART Config
	status =  UART_config(hUart, &Config);
    	if(CSL_SOK != status) { printf("UART_config failed error code %d\n",status); return(status); }
    	else { printf("UART_config Successful\n"); }
    */
    
    /*
    status = UART_resetOff(hUart);  
    if(CSL_SOK != status) { printf("UART_resetOff failed! %d\n",status); return(status); }
    	else { printf("UART_resetOff Successful\n"); }
    */
    	
	// Repeatedly output MIDI ON and MIDI OFF 
	
	while(j < 6)
	{
		j++;
		pBuf[0] 	= 0x90;
		pBuf[1]	 	= 0x60;
		pBuf[2] 	= 0x64;
		//status = UART_write(hUart, pBuf, COUNT, TIMEOUT_VALUE);
		//status = UART_fputs(hUart, pBuf, TIMEOUT_VALUE);
		status = UART_fputc(hUart, pBuf[0], TIMEOUT_VALUE);
		status = UART_fputc(hUart, pBuf[1], TIMEOUT_VALUE);
		status = UART_fputc(hUart, pBuf[2], TIMEOUT_VALUE);
		printf("Sent note ON message viat UART.\n\n");
		USBSTK5505_waitusec(ONE_HALF_SECOND); 
				
		pBuf[0] 	= 0x80;
		pBuf[1] 	= 0x60;
		pBuf[2] 	= 0x7F;
		//status = UART_write(hUart, pBuf, COUNT, TIMEOUT_VALUE);
		//status = UART_fputs(hUart, pBuf, TIMEOUT_VALUE);
		status = UART_fputc(hUart, pBuf[0], TIMEOUT_VALUE);
		status = UART_fputc(hUart, pBuf[1], TIMEOUT_VALUE);
		status = UART_fputc(hUart, pBuf[2], TIMEOUT_VALUE);
		printf("Sent note OFF message viat UART.\n\n");
		USBSTK5505_waitusec(ONE_HALF_SECOND);
		
	}
	
	
	printf("My_UART.c complete.\n");
	
	return(0);
	
}
