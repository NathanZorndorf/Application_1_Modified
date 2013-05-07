/*  ============================================================================
 *   Author: Nathan Zorndorf 
 *   Description: Setting up the DMA in Ping Pong mode by writing directly to registers. 
 */
 
#include <soc.h>
#include <csl_sar.h>
#include <csl_dma.h>
#include <csl_intc.h>
#include <stdio.h>
#include <usbstk5505.h>
#include <Application_1_Modified_Registers.h>
#include <My_DMA_Ping_Pong_Register_Setup.h>

/* Interrupt vector start address */  
extern void VECSTART(void);	 			// WHERE IS THIS DEFINED/DECLARED 
 
 int My_DMA_Ping_Pong_Register_Setup(void) {
 	
 	Uint16 register_value;
	Uint32 dma_address; 
	 
     /* --------------------- Interrupts Setup --------------------- */  
     printf("DMA INTERRUPTS SETUP BEGIN!!\n");

     IRQ_globalDisable(); // It disables the interrupt globally by disabling INTM bit and also return the previous mask value for INTM bit
	 IRQ_clearAll(); // This function clears all the interrupts. Both IFR0 and IFR1 are cleared by this function.
	 IRQ_disableAll(); // This function disables all the interrupts avaible on C5505 DSP. Both IER0 and IER1 are cleared by this function
	 IRQ_setVecs((Uint32)(&VECSTART)); // It stores the Interrupt vector table address in Interrupt vector pointer DSP and Interrupt vector pointer host Registers
         	 IRQ_clear(DMA_EVENT); // possibly not necessary and might need to go before(?) or after(?) IRQ_plug()
 	 IRQ_plug(DMA_EVENT, &DMA_ISR); // This function is used to register the ISR routine for the corresponding interrupt event.
     IRQ_enable(DMA_EVENT); // It enables the corresponding interrupt bit in IER Register and also return the previous bit mask value
	 IRQ_globalEnable(); // It enables the interrupt globally by enabling INTM bit and also return the previous mask value for INTM bit
	 printf("DMA INTERRUPTS SETUP END!!\n");
	 /* ------------------------------------------------------------ */
 	
 	printf("DMA SETUP BEGIN!!\n");
 		 
 	// step 3
    DMA_IFR = 0xFFFF; //  Enable Interrupts for Controller 1, Channels 0-3
 	printf("DMA_IFR   		= 0x%X \n", DMA_IFR); // 1 = DMA controller n, channel m block transfer complete.
 	printf("IER0     		= 0x%X \n", IER0);
 	printf("IER1     		= 0x%X \n", IER1);
 	
 	
 	// Step 4 - Enable interrupts 
    DMA_IER = 0x00F0; // Enable interrupts for DMA1 CH0-CH3 
    
    // Step 5 - Setup sync event 
    DMA1_CESR1 = 0x0202; /* Set CH1, CH0 sync event to I2S0 transmit */
    DMA1_CESR2 = 0x0101; /* Set CH3, CH2 sync event to I2S0 receive*/
    	
    // Step 6 - Channel Source Address
    DMA1_CH0_SSAL = 0x2A28; // I2S Receive Left Data 0 Register
	DMA1_CH0_SSAU = 0x0000;
	
	DMA1_CH1_SSAL = 0x2A2C; // I2S Receive Right Data 0 Register
	DMA1_CH1_SSAU = 0x0000;
    
    dma_address = convert_address(DMA_OutL);       // convert address 
	DMA1_CH2_SSAL = (Uint16)dma_address;          // keep LSBs 
	DMA1_CH2_SSAU = 0xFFFF & (dma_address >> 16); // keep MSBs 
	
	dma_address = convert_address(DMA_OutR);   
	DMA1_CH3_SSAL = (Uint16)dma_address;
	DMA1_CH3_SSAU = 0xFFFF & (dma_address >> 16);

    // Step 7 - Channel Destination Address 
    dma_address = convert_address(DMA_InpL);
	DMA1_CH0_DSAL = (Uint16)dma_address;
	DMA1_CH0_DSAU = 0xFFFF & (dma_address >> 16);
	
	dma_address = convert_address(DMA_InpR);
	DMA1_CH1_DSAL = (Uint16)dma_address;
	DMA1_CH1_DSAU = 0xFFFF & (dma_address >> 16);
	
	DMA1_CH2_DSAL = 0x2A08;	// I2S2 Transmit Left Data 0 Register
	DMA1_CH2_DSAU = 0x0000;
	
	DMA1_CH3_DSAL = 0x2A0C;	// I2S2 Transmit Right Data 0 Register
	DMA1_CH3_DSAU = 0x0000;
	
	/* Step 8 - DMA Transfer Length */
	/* Note the following (Sections 2.9, 4.3):                          */
	/* - DMA transfer length is specified in bytes.                     */
	/* - DMA transfer length in ping/pong mode is half the length of    */
	/*   the TCR1 register.                                             */
	/* - Ping/pong buffers should occupy contiguous memory spaces and   */
	/*   it is recommended to setup a buffer double the size of the     */
	/*   intended one to ensure this.                                   */
	/* In order to transfer _AUDIO_IO_SIZE_ 16-bit samples in ping/pong */
	/* mode, we specify the transfer length as 2*2*AUDIO_IO_SIZE.       */
	DMA1_CH0_TCR1 = 2*PING_PONG_SIZE;
    DMA1_CH1_TCR1 = 2*PING_PONG_SIZE;
    DMA1_CH2_TCR1 = 2*PING_PONG_SIZE;
    DMA1_CH3_TCR1 = 2*PING_PONG_SIZE;
    
    // Step 9 - Configure options 
    DMA1_CH0_TCR2 = 0x3081; 
    DMA1_CH1_TCR2 = 0x3081;
    DMA1_CH2_TCR2 = 0x3201;
    DMA1_CH3_TCR2 = 0x3201;
    
    // Step 10 - Enable DMA Controller 0 channel 0-3
    DMA1_CH0_TCR2 = 0x8004;
    DMA1_CH1_TCR2 = 0x8004;
    DMA1_CH2_TCR2 = 0x8004;
    DMA1_CH3_TCR2 = 0x8004;
    
 	return(0);
 	
 }
 
 interrupt void DMA_ISR(void)
{
	Uint16 register_value1, register_value2;
	
	/* Clear CPU DMA interrupt */
	register_value1 = IFR0;
	IFR0 = register_value1;
	   
	/* Read DMA interrupt flags */
    register_value1 = DMA_IFR;
    
    /* Channels 0-1, input */
    if (register_value1 & 0x0030) // if DMA 1 channel 0 and 1 interrupts are flagged  
    {
    	register_value2 = DMA1_CH0_TCR2;
    	if (register_value2 & 0x0002) { 
    		PingPongFlagInL = 1; // Last Transfer complete was Pong - Filling Ping
    	} else {
    		PingPongFlagInL = 0; // Last Transfer complete was Ping - Filling Pong
    	}
    	
    	register_value2 = DMA1_CH1_TCR2;
    	if (register_value2 & 0x0002) {
    		PingPongFlagInR = 1; // Last Transfer complete was Pong - Filling Ping
    	} else {
    		PingPongFlagInR = 0; // Last Transfer complete was Ping - Filling Pong
    	}
    		    
    	/* Clear CH0-1 interrupts */
    	DMA_IFR = 0x0030; 
    }
    
    /* Channels 2-3, output */
    if (register_value1 & 0x00C0)  // if DMA 1 channel 2-3 interrupts are flagged 
    { 
    	register_value2 = DMA1_CH2_TCR2;
    	if (register_value2 & 0x0002) {
    		PingPongFlagOutL = 1;
    	} else {
    		PingPongFlagOutL = 0;
    	}
    	
    	register_value2 = DMA1_CH3_TCR2;
    	if (register_value2 & 0x0002) {
    		PingPongFlagOutR = 1;
    	} else {
    		PingPongFlagOutR = 0;
    	}
    	  	
    	/* Clear CH2-3 interrupts */
    	DMA_IFR = 0x00C0; 
    }

}

 
/* Change word address to byte address and add DARAM offset for DMA */
Uint32 convert_address(Int16 *buffer) {
	
	Uint32 dma_address;
	
	dma_address = (Uint32)buffer;
	dma_address = (dma_address<<1) + 0x010000;
	
	return dma_address;
}
