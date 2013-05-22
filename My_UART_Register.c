/*  ============================================================================
 *   Author: Nathan Zorndorf 
 *   Description: UART Initialization and Test 
 */

#include <stdio.h>
#include <csl_uart.h>
#include <Application_1_Modified_Registers.h>
#include <My_UART_Register.h>
#include <usbstk5505.h>

// UART #define's
#define TIMEOUT_VALUE	1000
#define COUNT 			3 
#define ONE_HALF_SECOND	500000 // 500,000 microseconds in 0.5 seconds

int My_UART_Register(void){

	int i;
	 Uint16 register_value;
	 	
	 	/*
	CSL_UartConfig	uartConfig = 
		{
		   // DLL holds least significant bits of the divisor
		   200,
		   // DLH holds most significant bits of the divisor
		   0,
		   // LCR controls word length,parity selection,stop bit generation
		   0x0003,
		   // FCR controls fifo enable/disable,trigger level selection,dma selection
		   0x0000, 
		   // MCR controls autoflow selection,loopback feature,RTS selection   
		   0x0000,
		};
		
	
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
	*/
	
	
	// Initialize 
	// Step 1 - Perform the necessary device pin multiplexing setup
	// already done 
	// Step 2 - Ensure the UART is out of reset
	while ( CPU_PRCR & 0x0080) {
		printf("Still in reset. PRCR = 0x%X\n", CPU_PRCR);
	}
	
	// Step 3 - Enable the UART input clock by setting UARTCG to 1(TYPO !?!? - Methinks should be 0.) in (PCGCR1). 
	register_value = SYS_PCGCR1;
	SYS_PCGCR1 = register_value | 0; 
	
	// Step 4 - Place the UART transmitter and receiver in reset by setting UTRST and URRST to 0 in (PWREMU_MGMT)
	//register_value = UART_PWREMU_MGMT;
	//UART_PWREMU_MGMT = register_value | 0x0000;
	// Step 5 - Set the desired baud rate by writing the appropriate clock divisor values to (DLL and DLH).
	UART_DLL = 200;
	UART_DLH = 0;
	// Step 6 - See tech ref. - Select the desired trigger level/ enable the FIFOs by writing to (FCR) if the FIFOs are used.
	//UART_FCR = 0x0000;
	// Step 7 - Choose the desired protocol settings by writing to (LCR).
	UART_LCR = 0x0003;
	// Step 8 - Write appropriate values to the modem control register (MCR) if autoflow control is desired.
	// Step 9 - Choose the desired response to emulation suspend events by configuring the FREE bit and enable the
	// UART by setting the UTRST and URRST bits in the power and emulation management register (PWREMU_MGMT).
	register_value = UART_PWREMU_MGMT;
	UART_PWREMU_MGMT = register_value | 0x6000;
	
	
	while(1)
	{
		/*
		//status = UART_write(hUart, pBuf, COUNT, TIMEOUT_VALUE);
		//status = UART_fputs(hUart, pBuf, TIMEOUT_VALUE);
		UART_THR = 0x90;
		UART_THR = 0x60;
		UART_THR = 0x64;
		printf("Sent note ON message viat UART.\n\n");
		USBSTK5505_waitusec(ONE_HALF_SECOND); 
				
		//status = UART_write(hUart, pBuf, COUNT, TIMEOUT_VALUE);
		//status = UART_fputs(hUart, pBuf, TIMEOUT_VALUE);
		UART_THR = 0x80;
		UART_THR = 0x60;
		UART_THR = 0x7F;
		printf("Sent note OFF message viat UART.\n\n");
		USBSTK5505_waitusec(ONE_HALF_SECOND);
		*/
		i++;
		if ( i == 500)
		{
			i = 0;
		}
	}
	
	
	printf("My_UART.c complete.\n");
	
	return(0);
	
}
