/*  ============================================================================
 *   Author: Nathan Zorndorf 
 *   Description: Testing the use of the DMA in Ping Pong Mode. 
 *   For peripheral to memory transfer: The DMA takes 2N + 4 system clock cycles to complete a burst transfer, 
 *   where N corresponds to the burst size in words.
 */

#include <soc.h>
#include <csl_sar.h>
#include <csl_dma.h>
#include <csl_intc.h>
#include <stdio.h>
#include <usbstk5505.h>
#include <My_I2S.h> 

#define CSL_DMA_BUFFER_SIZE 1024
#define OFFSET      		CSL_DMA_BUFFER_SIZE

// CPU Registers 
#define PRCR       				 *(volatile ioport Uint16*)(0x1C05)
// Some CPU Interrupt Enable/Flag Registers
#define IER0        			 *(volatile unsigned *)0x0000
#define IFR0        			 *(volatile unsigned *)0x0001
#define IER1        			 *(volatile unsigned *)0x0045
#define IFR1        			 *(volatile unsigned *)0x0046
// Some General DMA Registers 
#define DMA_IFR     			 *(ioport volatile unsigned *)0x1C30    // DMA Interrupt Flag Register
#define DMA_IER     			 *(ioport volatile unsigned *)0x1C31    // DMA Interrupt Enable Register
// DMA Controller 1 Channel Event Source Registers
#define DMA1CESR1          		 *(volatile ioport Uint16*)(0x1C1C) // DMA Controller 1 Channel Event Source Register 1
#define DMA1CESR2          		 *(volatile ioport Uint16*)(0x1C1D) // DMA Controller 1 Channel Event Source Register 2
// DMA Controller 1 channel 0 Important Registers 
#define DMA1_DMACH0SSAL          *(volatile ioport Uint16*)(0x0D00) // Channel 0 Source Start Address (Lower Part) Register
#define DMA1_DMACH0SSAU          *(volatile ioport Uint16*)(0x0D01) // Channel 0 Source Start Address (Upper Part) Register
#define DMA1_DMACH0DSAL          *(volatile ioport Uint16*)(0x0D02) // Channel 0 Destination Start Address (Lower Part) Register
#define DMA1_DMACH0DSAU          *(volatile ioport Uint16*)(0x0D03) // Channel 0 Destination Start Address (Upper Part) Register
#define DMA1_DMACH0TCR1          *(volatile ioport Uint16*)(0x0D04) // Channel 0 Transfer Control Register 1
#define DMA1_DMACH0TCR2          *(volatile ioport Uint16*)(0x0D05) // Channel 0 Transfer Control Register 2
// DMA Controller 1 channel 1 Important Registers 
#define DMA1_DMACH1SSAL          *(volatile ioport Uint16*)(0x0D20) // Channel 0 Source Start Address (Lower Part) Register
#define DMA1_DMACH1SSAU          *(volatile ioport Uint16*)(0x0D21) // Channel 0 Source Start Address (Upper Part) Register
#define DMA1_DMACH1DSAL          *(volatile ioport Uint16*)(0x0D22) // Channel 0 Destination Start Address (Lower Part) Register
#define DMA1_DMACH1DSAU          *(volatile ioport Uint16*)(0x0D23) // Channel 0 Destination Start Address (Upper Part) Register
#define DMA1_DMACH1TCR1          *(volatile ioport Uint16*)(0x0D24) // Channel 0 Transfer Control Register 1
#define DMA1_DMACH1TCR2          *(volatile ioport Uint16*)(0x0D25) // Channel 0 Transfer Control Register 2
// DMA Controller 1 channel 2 Important Registers 
#define DMA1_DMACH2SSAL          *(volatile ioport Uint16*)(0x0D40) // Channel 0 Source Start Address (Lower Part) Register
#define DMA1_DMACH2SSAU          *(volatile ioport Uint16*)(0x0D41) // Channel 0 Source Start Address (Upper Part) Register
#define DMA1_DMACH2DSAL          *(volatile ioport Uint16*)(0x0D42) // Channel 0 Destination Start Address (Lower Part) Register
#define DMA1_DMACH2DSAU          *(volatile ioport Uint16*)(0x0D43) // Channel 0 Destination Start Address (Upper Part) Register
#define DMA1_DMACH2TCR1          *(volatile ioport Uint16*)(0x0D44) // Channel 0 Transfer Control Register 1
#define DMA1_DMACH2TCR2          *(volatile ioport Uint16*)(0x0D45) // Channel 0 Transfer Control Register 2
// DMA Controller 1 channel 3 Important Registers 
#define DMA1_DMACH3SSAL          *(volatile ioport Uint16*)(0x0D60) // Channel 0 Source Start Address (Lower Part) Register
#define DMA1_DMACH3SSAU          *(volatile ioport Uint16*)(0x0D61) // Channel 0 Source Start Address (Upper Part) Register
#define DMA1_DMACH3DSAL          *(volatile ioport Uint16*)(0x0D62) // Channel 0 Destination Start Address (Lower Part) Register
#define DMA1_DMACH3DSAU          *(volatile ioport Uint16*)(0x0D63) // Channel 0 Destination Start Address (Upper Part) Register
#define DMA1_DMACH3TCR1          *(volatile ioport Uint16*)(0x0D64) // Channel 0 Transfer Control Register 1
#define DMA1_DMACH3TCR2          *(volatile ioport Uint16*)(0x0D65) // Channel 0 Transfer Control Register 2
// I2S2 Left (LSW/MSW) and Right (LSW/MSW) Data Registers 
#define I2S2_Left_LSW      		 *(volatile ioport Uint16*)(0x2A28)
#define I2S2_Left_MSW      		 *(volatile ioport Uint16*)(0x2A29)
#define I2S2_Right_LSW      	 *(volatile ioport Uint16*)(0x2A2C)
#define I2S2_Right_MSW      	 *(volatile ioport Uint16*)(0x2A2D)
// I2S Register 
#define I2S2_I2SINTFL            *(volatile ioport Uint16*)(0x2A10) // I2S2 Interrupt Flag Register

static int count = 0;
static long isrEntryCount = 0;
/* Variables for DMA functions */
CSL_DMA_Handle		 hDMA1_CH0;
CSL_DMA_Handle  	 hDMA1_CH1;
CSL_DMA_Handle  	 hDMA1_CH2;
CSL_DMA_Handle  	 hDMA1_CH3;
	
/*  When ping-pong mode is enabled DMA considers the source data buffer as two parts and generates an interrupt
 *  after transferring half of the data. First half of the buffer is Ping data buffer and second half of the buffer
 *  is Pong data buffer. These two buffers should be allocated in contiguous memory locations or one ping buffer of 
 *  size equal to double the data transfer length should be allocated. */
//#pragma DATA_ALIGN (InputL, 4)
Int16 InputL[2*CSL_DMA_BUFFER_SIZE];	 
//#pragma DATA_ALIGN (InputR, 4)
Int16 InputR[2*CSL_DMA_BUFFER_SIZE];
//#pragma DATA_ALIGN (OutputL, 4)
Int16 OutputL[2*CSL_DMA_BUFFER_SIZE];
//#pragma DATA_ALIGN (OutputR, 4)
Int16 OutputR[2*CSL_DMA_BUFFER_SIZE];
	
/* Interrupt vector start address */  
extern void VECSTART(void);	 			// WHERE IS THIS DEFINED/DECLARED 

/* prototype declaration for ISR function */
interrupt void DMA_IsR(void); 

/* ================== Change word address to byte address and add DARAM offset for DMA ================== */
Uint32 convert_address(Int16 *buffer) {
	
	Uint32 dma_address;
	
	dma_address = (Uint32)buffer;
	dma_address = (dma_address<<1) + 0x010000;
	
	return dma_address;
}
/* ================== BufferCopy Function ================== */
void bufferCopy(Int16 *input, Int16 *output, Int16 size, Int16 offset_in, Int16 offset_out)
{
	Int16 i;
	
	for(i = 0; i < size; i++) {
		*(output + i + offset_out) = *(input + i + offset_in);
	}
}
/* ================== DMA ISR ============================== */
interrupt void DMA_IsR(void) {
	
	CSL_Status	status;
	Uint16 register_value1, register_value2;

	// Clear CPU DMA interrupt 
	register_value1 = IFR0;
	IFR0 = register_value1;
	
	// Read DMA interrupt flags 
    register_value1 = DMA_IFR;  
      
	if (DMA_getLastTransferType(hDMA1_CH2, &status)) { // Filling Ping 
		bufferCopy(InputL, OutputL, CSL_DMA_BUFFER_SIZE, OFFSET, OFFSET);
		//printf("Pong Set Transfer Completed. Filling Ping...\n");
	} else { // Filling Pong 
		bufferCopy(InputL, OutputL, CSL_DMA_BUFFER_SIZE, 0, 0);		
		//printf("Ping Set Transfer Completed. Filling Pong...\n");
	}
	
	if (DMA_getLastTransferType(hDMA1_CH3, &status)) {
		bufferCopy(InputR, OutputR, CSL_DMA_BUFFER_SIZE, OFFSET, OFFSET);
		//printf("Pong Set Transfer Completed. Filling Ping...\n");
	} else {
		bufferCopy(InputR, OutputR, CSL_DMA_BUFFER_SIZE, 0, 0);
		//printf("Ping Set Transfer Completed. Filling Pong...\n");
	}
    
	isrEntryCount++;
	/*
	if(isrEntryCount == (10000*n))  
    {
    	printf("ISR entry count/# of times the Ping and Pong Buffers Filled = %d\n", isrEntryCount);
    	n++;
    }
    */
}
/* ========================================================= */
void My_DMA_Ping_Pong_Register(void) {
	
	/* Variables for DMA functions */
	Uint16   			 index = 0;
	Uint32 				 dma_address; 
	 /*	
	Uint16				 InputL_Addr_LSW;
 	Uint16				 InputL_Addr_MSW;
 	Uint16				 InputR_Addr_LSW;
 	Uint16				 InputR_Addr_MSW;
 	Uint16				 OutputL_Addr_LSW;
 	Uint16				 OutputL_Addr_MSW;
 	Uint16				 OutputR_Addr_LSW;
 	Uint16				 OutputR_Addr_MSW;
 	
	InputL_Addr_LSW = (Uint32)InputL & 0x0000FFFF; // mask the LSW
 	InputL_Addr_MSW = ((Uint32)InputL & 0xFFFF0000) >> 16; // Mask the MSW
 	InputR_Addr_LSW =  (Uint32)InputR & 0x0000FFFF; 
 	InputR_Addr_MSW = ((Uint32)InputR & 0xFFFF0000) >> 16; 
 	OutputL_Addr_LSW = (Uint32)OutputL & 0x0000FFFF;
 	OutputL_Addr_MSW = ((Uint32)OutputL & 0xFFFF0000) >> 16; 
 	OutputR_Addr_LSW = (Uint32)OutputR & 0x0000FFFF;
 	OutputR_Addr_MSW = ((Uint32)OutputR & 0xFFFF0000) >> 16;
 	*/ 
 	
 	 printf("DMA SETUP BEGIN!!\n");
     /* --------------------- Interrupts Setup --------------------- */  
     printf("DMA INTERRUPTS SETUP BEGIN!!\n");
     count = 0;
	 isrEntryCount = 0;
	     
     IRQ_globalDisable(); // It disables the interrupt globally by disabling INTM bit and also return the previous mask value for INTM bit
	 IRQ_clearAll(); // This function clears all the interrupts. Both IFR0 and IFR1 are cleared by this function.
	 IRQ_disableAll(); // This function disables all the interrupts avaible on C5505 DSP. Both IER0 and IER1 are cleared by this function
	 IRQ_setVecs((Uint32)(&VECSTART)); // It stores the Interrupt vector table address in Interrupt vector pointer DSP and Interrupt vector pointer host Registers
         	 IRQ_clear(DMA_EVENT); // possibly not necessary and might need to go before(?) or after(?) IRQ_plug()
 	 IRQ_plug(DMA_EVENT, &DMA_IsR); // This function is used to register the ISR routine for the corresponding interrupt event.
     IRQ_enable(DMA_EVENT); // It enables the corresponding interrupt bit in IER Register and also return the previous bit mask value
	 IRQ_globalEnable(); // It enables the interrupt globally by enabling INTM bit and also return the previous mask value for INTM bit
	 printf("DMA INTERRUPTS SETUP END!!\n");
	 /* ------------------------------------------------------------ */
	 
	// Initialize arrays to 0 
	for(index = 0; index < CSL_DMA_BUFFER_SIZE; index++)
	{
		InputL[index]  = 0x0000;
		InputR[index]  = 0x0000;
		OutputL[index] = 0x0000;
		OutputR[index] = 0x0000;
	}    
	      
	// ---------------------- DMA Initialization ---------------------- //
	// Step 1 of 3.2.13 in C5535 Technical Reference
	// while((PRCR & 0x0010));
	
	// Step 2
	SYS_PCGCR1 = 0x0000; // - OK
    SYS_PCGCR2 = 0x0000; // - OK
    
    // step 3
    DMA_IFR = 0xFFFF; //  Enable Interrupts for Controller 1, Channels 0-3
    IFR0 = 0x0100;    /* Clear DMA CPU interrupt flag */
 	printf("DMA_IFR   		= 0x%X \n", DMA_IFR); // 1 = DMA controller n, channel m block transfer complete.
 	printf("IER0     		= 0x%X \n", IER0);
 	printf("IER1     		= 0x%X \n", IER1);
 	
 	// Step 4
 	DMA_IER = 0x00F0; //  Enable Interrupts for Controller 1, Channels 0-3
 	printf("DMA_IER   		= 0x%X \n", DMA_IER);  // 1 = DMA controller n, channel m interrupt is enabled.
 	DMA1_DMACH0TCR2 =  0x2000;
 	DMA1_DMACH1TCR2 =  0x2000;
 	DMA1_DMACH2TCR2 =  0x2000;
 	DMA1_DMACH3TCR2 =  0x2000; // Enable Bit 13 TCR2 Register for each channel, which is the CPU Interrupt Enable Bit.
 	
 	// Step 5 - Set the Synchronizing Hardware Event to I2S2 RX for CH0 and CH1, and TX for CH2 and CH3
 	DMA1CESR1  = 0x0202;  // Set CH1, CH0 sync event to I2S2 RX 
	DMA1CESR2  = 0x0101;  // Set CH2, CH3 sync event to I2S2 TX 
	
	// Step 6 - ??? Not sure about Address on Buffers 
 	DMA1_DMACH0SSAL      = 0x2A28;				    // OK - Address of source data buffer - I2S2 RX Left Data 0 Register (0x2A28)
    DMA1_DMACH0SSAU		 = 0;
    DMA1_DMACH1SSAL      = 0x2A2C;				    // OK - Address of source data buffer - I2S2 RX Right Data 0 Register (0x2A2C)
    DMA1_DMACH1SSAU		 = 0;
    
	dma_address = convert_address(InputL);
	DMA1_DMACH0DSAL = (Uint16)dma_address;
	DMA1_DMACH0DSAU = 0xFFFF & (dma_address >> 16);
	dma_address = convert_address(InputR);
	DMA1_DMACH1DSAL = (Uint16)dma_address;
	DMA1_DMACH1DSAL = 0xFFFF & (dma_address >> 16);
		
	// Step 7  - ????
	DMA1_DMACH2DSAL      = 0x2A08;				// OK - Address of destination data buffer - I2S2 TX Right LSW
    DMA1_DMACH2DSAU      = 0;
    DMA1_DMACH3DSAL      = 0x2A0C;				// OK - Address of destination data buffer  -  I2S2 TX Right LSW
    DMA1_DMACH3DSAU      = 0;	
    
	dma_address = convert_address(OutputL);
	DMA1_DMACH2SSAL = (Uint16)dma_address;
	DMA1_DMACH2SSAU = 0xFFFF & (dma_address >> 16);
	dma_address = convert_address(OutputR);
	DMA1_DMACH3SSAL = (Uint16)dma_address;
	DMA1_DMACH3SSAL = 0xFFFF & (dma_address >> 16);
	
	// Step 8 
	DMA1_DMACH0TCR1 |= CSL_DMA_BUFFER_SIZE*4;
	DMA1_DMACH1TCR1 |= CSL_DMA_BUFFER_SIZE*4;
	DMA1_DMACH2TCR1 |= CSL_DMA_BUFFER_SIZE*4;
	DMA1_DMACH3TCR1 |= CSL_DMA_BUFFER_SIZE*4;
	
	// Step 9 - EN and SYNCMODE must be 0 in this step 
	DMA1_DMACH0TCR2 |= 0x3081;
	DMA1_DMACH1TCR2 |= 0x3081;
	DMA1_DMACH2TCR2 |= 0x3201;
	DMA1_DMACH3TCR2 |= 0x3201;
	
	// Step 10 
	// Step 11 - Write a 1 to EN and SYNCMODE bits  
	DMA1_DMACH0TCR2 |= 0x8004;
	DMA1_DMACH1TCR2 |= 0x8004;
	DMA1_DMACH2TCR2 |= 0x8004;
	DMA1_DMACH3TCR2 |= 0x8004;
	
	// Step 12
	My_I2S();
    
    printf("\n");
	printf("VECSTART 		= 0x%X \n\n", VECSTART);
	
	printf("DMA_IER   		= 0x%X \n", DMA_IER);  // 1 = DMA controller n, channel m interrupt is enabled.
 	printf("DMA_IFR   		= 0x%X \n\n", DMA_IFR); // 1 = DMA controller n, channel m block transfer complete.
 	
 	printf("IER0     		= 0x%X \n", IER0);
 	printf("IER1     		= 0x%X \n\n", IER1);
 	
 	printf("DMA1CESR1 (CH0/1 Sync Events)   	= 0x%X \n", DMA1CESR1);
 	printf("DMA1CESR2 (CH2/3 Sync Events)    	= 0x%X \n\n", DMA1CESR2);
 	
 	printf("DMA1_DMACH0SSAU (source start address MSW) = 0x%X \n", DMA1_DMACH0SSAU);
	printf("DMA1_DMACH0SSAL (source start address LSW) = 0x%X \n", DMA1_DMACH0SSAL);
	printf("DMA1_DMACH0DSAU (dest.  start address MSW) = 0x%X \n", DMA1_DMACH0DSAU);
	printf("DMA1_DMACH0DSAL (dest.  start address LSW) = 0x%X \n\n", DMA1_DMACH0DSAL);

 	printf("DMA1_DMACH1SSAU (source start address MSW) = 0x%X \n", DMA1_DMACH1SSAU);
	printf("DMA1_DMACH1SSAL (source start address LSW) = 0x%X \n", DMA1_DMACH1SSAL);
	printf("DMA1_DMACH1DSAU (dest.  start address MSW) = 0x%X \n", DMA1_DMACH1DSAU);
	printf("DMA1_DMACH1DSAL (dest.  start address LSW) = 0x%X \n\n", DMA1_DMACH1DSAL);

 	printf("DMA1_DMACH2SSAU (source start address MSW) = 0x%X \n", DMA1_DMACH2SSAU);
	printf("DMA1_DMACH2SSAL (source start address LSW) = 0x%X \n", DMA1_DMACH2SSAL);
	printf("DMA1_DMACH2DSAU (dest.  start address MSW) = 0x%X \n", DMA1_DMACH2DSAU);
	printf("DMA1_DMACH2DSAL (dest.  start address LSW) = 0x%X \n\n", DMA1_DMACH2DSAL);
	
 	printf("DMA1_DMACH3SSAU (source start address MSW) = 0x%X \n", DMA1_DMACH3SSAU);
	printf("DMA1_DMACH3SSAL (source start address LSW) = 0x%X \n", DMA1_DMACH3SSAL);
	printf("DMA1_DMACH3DSAU (dest.  start address MSW) = 0x%X \n", DMA1_DMACH3DSAU);
	printf("DMA1_DMACH3DSAL (dest.  start address LSW) = 0x%X \n\n", DMA1_DMACH3DSAL);
	
 	printf("DMA1_DMACH0TCR1 = 0x%X \n", DMA1_DMACH0TCR1);
 	printf("DMA1_DMACH0TCR2 = 0x%X \n\n", DMA1_DMACH0TCR2);
 	printf("DMA1_DMACH1TCR1 = 0x%X \n", DMA1_DMACH1TCR1);
 	printf("DMA1_DMACH1TCR2 = 0x%X \n\n", DMA1_DMACH1TCR2);
 	printf("DMA1_DMACH2TCR1 = 0x%X \n", DMA1_DMACH2TCR1);
 	printf("DMA1_DMACH2TCR2 = 0x%X \n\n", DMA1_DMACH2TCR2);
 	printf("DMA1_DMACH3TCR1 = 0x%X \n", DMA1_DMACH3TCR1);
 	printf("DMA1_DMACH3TCR2 = 0x%X \n\n", DMA1_DMACH3TCR2);
 	
    printf("DMA SETUP END!!\n\n");
	
	while(1)
	{
		if(I2S2_I2SINTFL == 0x0001)
  		{
  			printf("ERROR: The data registers were not read from or written to before the receive/transmit buffer was overwritten.\n");
  			printf("I2SINTFL = 0x%X\n\n", I2S2_I2SINTFL);
  			break;
  		}
  		if(I2S2_I2SINTFL == 0x0002)
  		{
  			printf("ERROR: Frame-synchronization error(s) occurred!!\n");
  			printf("I2SINTFL = 0x%X\n\n", I2S2_I2SINTFL);
  			break;
  		}
	}
	
    printf("My_Dma_Ping_Pong.c Complete!\n");
}




