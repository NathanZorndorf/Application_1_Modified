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

#define CSL_DMA_BUFFER_SIZE 1024
#define OFFSET      		CSL_DMA_BUFFER_SIZE

// Some CPU Interrupt Enable/Flag Registers 
#define IER0        			 *(volatile unsigned *)0x0000
#define IFR0        			 *(volatile unsigned *)0x0001
#define IER1        			 *(volatile unsigned *)0x0045
#define IFR1        			 *(volatile unsigned *)0x0046
// Some General DMA Registers 
#define DMA_IFR     			 *(ioport volatile unsigned *)0x1C30    // DMA Interrupt Flag Register
#define DMA_IER     			 *(ioport volatile unsigned *)0x1C31    // DMA Interrupt Enable Register
#define DMA1CESR1          		 *(volatile ioport Uint16*)(0x1C1C) // DMA1 Channel Event Source Register 1
#define DMA1CESR2          		 *(volatile ioport Uint16*)(0x1C1D) // DMA1 Channel Event Source Register 2
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
static long n = 1;
/* Variables for DMA functions */
CSL_DMA_Handle		 hDMA1_CH0;
CSL_DMA_Handle  	 hDMA1_CH1;
CSL_DMA_Handle  	 hDMA1_CH2;
CSL_DMA_Handle  	 hDMA1_CH3;
	
/*  When ping-pong mode is enabled DMA considers the source data buffer as two parts and generates an interrupt
 *  after transferring half of the data. First half of the buffer is Ping data buffer and second half of the buffer
 *  is Pong data buffer. These two buffers should be allocated in contiguous memory locations or one ping buffer of 
 *  size equal to double the data transfer length should be allocated. */
#pragma DATA_ALIGN (InputL, 4)
Int16 InputL[2*CSL_DMA_BUFFER_SIZE];	 
#pragma DATA_ALIGN (InputR, 4)
Int16 InputR[2*CSL_DMA_BUFFER_SIZE];
#pragma DATA_ALIGN (OutputL, 4)
Int16 OutputL[2*CSL_DMA_BUFFER_SIZE];
#pragma DATA_ALIGN (OutputR, 4)
Int16 OutputR[2*CSL_DMA_BUFFER_SIZE];
	
/* Interrupt vector start address */  
extern void VECSTART(void);	 			// WHERE IS THIS DEFINED/DECLARED 

/* prototype declaration for ISR function */
interrupt void DMA_IsR(void); 

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
	// 3 lines below needed???
	int ifrValue;
  	ifrValue = CSL_SYSCTRL_REGS->DMAIFR; // what does "->" do?
	CSL_SYSCTRL_REGS->DMAIFR |= ifrValue;
    
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
void My_DMA_Ping_Pong(void) {
	
	/* Variables for DMA functions */
	Uint32 				 dummy = 0;
	Uint16   			 index;
	CSL_Status       	 status;
    CSL_DMAChanNum       chanNum;
    CSL_DMA_ChannelObj   pDmaChanObj; // includes engineID number 
	CSL_DMA_Config       pConfig;
 	CSL_DMA_Config 		 getdmaConfig;
 	
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
	      
	status = DMA_init(); 
	if(CSL_SOK != status) { printf("DMA_init failed!!\n"); }
	
		// Initialize arrays to 0 
	for(index = 0; index < CSL_DMA_BUFFER_SIZE; index++)
	{
		InputL[index]  = 0x0000;
		InputR[index]  = 0x0000;
		OutputL[index] = 0x0000;
		OutputR[index] = 0x0000;
	}
	
    pConfig.pingPongMode = CSL_DMA_PING_PONG_ENABLE;	// OK
    pConfig.autoMode     = CSL_DMA_AUTORELOAD_ENABLE;   // OK - If 1, then the DMA automatically reinitiates the DMA transfer until either EN or AUTORLD bit is set to 0. In the case that the AUTORLD bit is set to 0, DMA stops data transfer after the Pong buffer is full and the interrupt is generated. 
    pConfig.burstLen     = CSL_DMA_TXBURST_1WORD; 		// OK - Depends on IS2/AIC3204 settings - This represents the number of words to be transferred by a channel before giving the DMA FIFO to next channel. 
    pConfig.trigger      = CSL_DMA_EVENT_TRIGGER;       // OK - DMA transfer can be triggered by software or hardware event. 
    	pConfig.dmaEvt       = CSL_DMA_EVT_I2S2_RX;			// ? - This represents the event by which DMA transfer is being triggered (double check with DMA1CESR1/2)
    pConfig.dmaInt       = CSL_DMA_INTERRUPT_ENABLE;	// OK - This provides the information about DMA Interrupt. After completion of data transfer will be served by interrupt. 
    pConfig.trfType      = CSL_DMA_TRANSFER_IO_MEMORY;	// OK
    	pConfig.dataLen      = CSL_DMA_BUFFER_SIZE * 2;		// ? - 2 OR 4 ????? - Length of data to transfer in # of bytes - When Ping-Pong mode is enabled, this is the size of the Ping and the Pong transfer combined.
    
	// ---------------------- DMA0_CH0 Setup ---------------------- //
	chanNum = CSL_DMA_CHAN4; // Set the controller number and channel number for the DMA controller 
	hDMA1_CH0 = DMA_open(chanNum, &pDmaChanObj, &status);
	if(CSL_SOK != status) { printf("DMA_open failed!!\n"); }
	else
	{
		printf("pDmaChanObj.dmaRegs =	%d\n", 		pDmaChanObj.dmaRegs );
		printf("pDmaChanObj.chanNum = 	%d\n", 		pDmaChanObj.chanNum);
		printf("pDmaChanObj.dmaInstNum =  %d\n", 	pDmaChanObj.dmaInstNum);
		printf("pDmaChanObj.isChanFree =  %d\n", 	pDmaChanObj.isChanFree);
		printf("pDmaChanObj.chanDir =    %d\n", 	pDmaChanObj.chanDir  );
		printf("pDmaChanObj.trigger =   %d\n", 		pDmaChanObj.trigger);
		printf("pDmaChanObj.trfType =   %d\n", 		pDmaChanObj.trfType);
		printf("pDmaChanObj.dmaInt =  %d\n\n", 		pDmaChanObj.dmaInt); 
	}
	
	pConfig.chanDir      = CSL_DMA_READ;				// OK - For READ, src. is fixed and dest. is incremented.
	pConfig.srcAddr      = 0x00002A28;				    // OK - Address of source data buffer - I2S2 Receive Left Data 0 Register (0x2A28)
    pConfig.destAddr     = (Uint32)InputL;				// OK - Address of destination data buffer 
    	printf("pConfig.srcAddr (before DMA_config())  =  0x%X\n", 		pConfig.srcAddr );
		printf("pConfig.destAddr(before DMA_config())  =  0x%X\n", 		pConfig.destAddr );
	
    status = DMA_config(hDMA1_CH0, &pConfig); if(CSL_SOK != status) { printf("DMA_Config failed!!\n"); }
	    printf("FIRST CHECKPOINT DMA1_DMACH0SSAU (source start address MSW) = 0x%X \n", DMA1_DMACH0SSAU);
	 	printf("DMA1_DMACH0SSAL (source start address LSW) = 0x%X \n", DMA1_DMACH0SSAL);
	 	printf("DMA1_DMACH0DSAU (dest.  start address MSW) = 0x%X \n", DMA1_DMACH0DSAU);
	 	printf("DMA1_DMACH0DSAL (dest.  start address LSW) = 0x%X \n", DMA1_DMACH0DSAL);
    status = DMA_getConfig(hDMA1_CH0, &getdmaConfig); if(CSL_SOK != status) { printf("DMA_Config failed!!\n"); }
		printf("pConfig.srcAddr (after DMA_getConfig()) =  0x%X\n", 		getdmaConfig.srcAddr );
		printf("pConfig.destAddr(after DMA_config()) =  0x%X\n", 		getdmaConfig.destAddr );
	    printf("SECOND CHECKPOINT DMA1_DMACH0SSAU (source start address MSW) = 0x%X \n", DMA1_DMACH0SSAU);
	 	printf("DMA1_DMACH0SSAL (source start address LSW) = 0x%X \n", DMA1_DMACH0SSAL);
	 	printf("DMA1_DMACH0DSAU (dest.  start address MSW) = 0x%X \n", DMA1_DMACH0DSAU);
	 	printf("DMA1_DMACH0DSAL (dest.  start address LSW) = 0x%X \n", DMA1_DMACH0DSAL);
    // ------------------------------------------------------------ //
    
    // ---------------------- DMA0_CH1 Setup ---------------------- //
	chanNum = CSL_DMA_CHAN5; // Set the controller number and channel number for the DMA controller (CHAN0-3 = controller 1, CHAN4-7=controller 2, etc...)
	hDMA1_CH1 = DMA_open(chanNum, &pDmaChanObj, &status); if(CSL_SOK != status) { printf("DMA_open failed!!\n"); }
	
	pConfig.chanDir      = CSL_DMA_READ;				// OK - For READ, src. is fixed and dest. is incremented.
	pConfig.srcAddr      = 0x00002A2C;					// OK - Address of source data buffer - I2S2 Receive Right Data 0 Register (0x2A2C)
    pConfig.destAddr     = (Uint32)InputR;				// OK - Address of destination data buffer - why recast as Uint32? 

    status = DMA_config(hDMA1_CH1, &pConfig); if(CSL_SOK != status) { printf("DMA_Config failed!!\n"); }
    // ------------------------------------------------------------ //
    
    pConfig.dmaEvt       = CSL_DMA_EVT_I2S2_TX; // Testing to see if TX DMA should be triggered by I2S2 TX 
    // ---------------------- DMA0_CH2 Setup ---------------------- //
	chanNum = CSL_DMA_CHAN6; // Set the controller number and channel number for the DMA controller (CHAN0-3 = controller 1, CHAN4-7=controller 2, etc...) 
	hDMA1_CH2 = DMA_open(chanNum, &pDmaChanObj, &status); if(CSL_SOK != status) { printf("DMA_open failed!!\n"); }
	
	pConfig.chanDir      = CSL_DMA_WRITE;				// OK - For WRITE, src is incremented and dest. is fixed. -
	pConfig.srcAddr      = (Uint32)OutputL;				// OK - Address of source data buffer 
    pConfig.destAddr     = 0x2A08;						// OK - Address of destination data buffer - I2S2 Transmit Left Data 0 Register

    status = DMA_config(hDMA1_CH2, &pConfig);  if(CSL_SOK != status) { printf("DMA_Config failed!!\n"); }
    // ------------------------------------------------------------ //
    
    // ---------------------- DMA0_CH3 Setup ---------------------- //
	chanNum = CSL_DMA_CHAN7; // Set the controller number and channel number for the DMA controller (CHAN0-3 = controller 1, CHAN4-7=controller 2, etc...) 
	hDMA1_CH3 = DMA_open(chanNum, &pDmaChanObj, &status); if(CSL_SOK != status){printf("DMA_open failed!!\n");}
	
	pConfig.chanDir      = CSL_DMA_WRITE;				// OK - For WRITE, src is incremented and dest. is fixed.
	pConfig.srcAddr      = (Uint32)OutputR;				// OK - Address of source data buffer
    pConfig.destAddr     = 0x2A0C;						// OK - Address of destination data buffer  - I2S2 Transmit Right Data 0 Register

    status = DMA_config(hDMA1_CH3, &pConfig); if(CSL_SOK != status) {printf("DMA_Config failed!!\n");}
    // ------------------------------------------------------------ //
 	
    // ---------------------- DMA Start---------------------------- //
    status = DMA_start (hDMA1_CH0); if(CSL_SOK != status){printf("DMA_start hDMA1_CH0 failed!!\n");}
    status = DMA_start (hDMA1_CH1); if(CSL_SOK != status){printf("DMA_start hDMA1_CH1 failed!!\n");}
    status = DMA_start (hDMA1_CH2); if(CSL_SOK != status){printf("DMA_start hDMA1_CH2 failed!!\n");}
    status = DMA_start (hDMA1_CH3); if(CSL_SOK != status){printf("DMA_start hDMA1_CH3 failed!!\n");}
    // ------------------------------------------------------------ //
    
    printf("\n");
	printf("VECSTART 		= 0x%X \n", VECSTART);
	
	printf("DMA_IER   		= 0x%X \n", DMA_IER);  // 1 = DMA controller n, channel m interrupt is enabled.
 	printf("DMA_IFR   		= 0x%X \n", DMA_IFR); // 1 = DMA controller n, channel m block transfer complete.
 	
 	printf("IER0     		= 0x%X \n", IER0);
 	printf("IER1     		= 0x%X \n", IER1);
 	
 	printf("DMA1CESR1 (CH0/1 Sync Events)   	= 0x%X \n", DMA1CESR1);
 	printf("DMA1CESR2 (CH2/3 Sync Events)    	= 0x%X \n", DMA1CESR2);
 	
 	printf("DMA1_DMACH0TCR1 = 0x%X \n", DMA1_DMACH0TCR1);
 	printf("DMA1_DMACH0TCR2 = 0x%X \n", DMA1_DMACH0TCR2);
 	
 	printf("DMA1_DMACH2TCR1 = 0x%X \n", DMA1_DMACH2TCR1);
 	printf("DMA1_DMACH2TCR2 = 0x%X \n", DMA1_DMACH2TCR2);
 	
    printf("DMA SETUP END!!\n\n");
	
	
	while(1)
	{
		/*
		if(isrEntryCount == 8000)
		{	
			status = DMA_stop (hDMA1_CH0);
			status = DMA_stop (hDMA1_CH1);
		    status = DMA_stop (hDMA1_CH2);
		    status = DMA_stop (hDMA1_CH3);
		    status = DMA_close(hDMA1_CH0);
		    status = DMA_close(hDMA1_CH1);
		    status = DMA_close(hDMA1_CH2);
		    status = DMA_close(hDMA1_CH3);
		    printf("isrEntryCount = %d\n", isrEntryCount);
		    break;
		}
		*/
		if(I2S2_I2SINTFL == 0x0001)
  		{
  			printf("ERROR: The I2S2 data registers were not read from or written to before the receive/transmit buffer was overwritten.\n");
  			printf("I2SINTFL = 0x%X\n\n", I2S2_I2SINTFL);
  		}
  		if(I2S2_I2SINTFL == 0x0002)
  		{
  			printf("ERROR: Frame-synchronization error(s) occurred!!\n");
  			printf("I2SINTFL = 0x%X\n\n", I2S2_I2SINTFL);
  		}
	}
	
    printf("My_Dma_Ping_Pong.c Complete!\n");
}
