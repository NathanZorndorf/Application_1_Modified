/*  ============================================================================
 *   Author: Nathan Zorndorf 
 *  Description: Testing the use of the DMA in Ping Pong Mode. 
 */

#include <csl_sar.h>
#include <csl_dma.h>
#include <csl_intc.h>
#include <stdio.h>

#define CSL_DMA_BUFFER_SIZE 512

/* Reference the start of the interrupt vector table */
extern void VECSTART(void);

/* prototype declaration for ISR function */
/*  \brief  DMA Interrupt Service routine
 *  \param  none
 *  \return none */
interrupt void dma_isr(void);

#pragma DATA_ALIGN (dmaPingDstBuf, 4)
Uint16 dmaPingDstBuf[CSL_DMA_BUFFER_SIZE];
#pragma DATA_ALIGN (dmaPongDstBuf, 4)
Uint16 dmaPongDstBuf[CSL_DMA_BUFFER_SIZE];

/* Declaration of the buffer */
#pragma DATA_ALIGN (dmaPingSrcBuf, 4)
Uint16 dmaPingSrcBuf[CSL_DMA_BUFFER_SIZE];

#pragma DATA_ALIGN (dmaPongSrcBuf, 4)
Uint16 dmaPongSrcBuf[CSL_DMA_BUFFER_SIZE];

static int count ;
static int isrEntryCount = 0;

CSL_DMA_Handle 		dmaHandle;
CSL_DMA_Config 		dmaConfig;
CSL_DMA_Config 		getdmaConfig;

CSL_Status 			status;

/*  \brief  Tests DMA Ping-Pong Mode transfers
 *  \param  none
 *  \return none */
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 1 --   
   /////  Define PaSs_StAtE variable for catching errors as program executes.
   /////  Define PaSs flag for holding final pass/fail result at program completion.
        volatile Int16 PaSs_StAtE = 0x0001; // Init to 1. Reset to 0 at any monitored execution error.
        volatile Int16 PaSs = 0x0000; // Init to 0.  Updated later with PaSs_StAtE when and if
   /////                                  program flow reaches expected exit point(s).

void DMA_Ping_Pong(void)
{
#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined(CHIP_C5517))

	CSL_DMA_ChannelObj  dmaObj;
	Uint16   			chanNumber;
	Uint16   			index;

	printf("\nDMA PING-PONG MODE TEST!\n");

	// The original configuration settings 
	dmaConfig.pingPongMode = CSL_DMA_PING_PONG_ENABLE;
	dmaConfig.autoMode     = CSL_DMA_AUTORELOAD_DISABLE; // The auto-initialization feature can only be used when event synchronization (hardware_trigger) is used
	dmaConfig.burstLen     = CSL_DMA_TXBURST_4WORD;
	dmaConfig.trigger      = CSL_DMA_SOFTWARE_TRIGGER;
	dmaConfig.dmaEvt       = CSL_DMA_EVT_NONE;
	dmaConfig.dmaInt       = CSL_DMA_INTERRUPT_ENABLE;
	dmaConfig.chanDir      = CSL_DMA_READ;
	dmaConfig.trfType      = CSL_DMA_TRANSFER_MEMORY;
	dmaConfig.dataLen      = CSL_DMA_BUFFER_SIZE * 4;
	dmaConfig.srcAddr      = (Uint32)dmaPingSrcBuf;
	dmaConfig.destAddr     = (Uint32)dmaPingDstBuf;
	
	/*
	// My own configuration settings - Not finalized as of April 14, 2013
	dmaConfig.pingPongMode = CSL_DMA_PING_PONG_ENABLE;  // OK 
	dmaConfig.autoMode     = CSL_DMA_AUTORELOAD_ENABLE; // OK - If 1, then the DMA automatically reinitiates the DMA transfer until either EN or AUTORLD bit is set to 0. In the case that the AUTORLD bit is set to 0, DMA stops data transfer after the Pong buffer is full and the interrupt is generated. 
	dmaConfig.burstLen     = CSL_DMA_TXBURST_1WORD;		// OK? - Depends on IS2/AIC3204 settings - This represents the number of words to be transferred by a channel before giving the DMA FIFO to next channel. 
	dmaConfig.trigger      = CSL_DMA_EVENT_TRIGGER ;    // ? - DMA transfer can be triggered by software or hardware event. 
	dmaConfig.dmaEvt       = CSL_DMA_EVT_I2S2_RX ;		// OK - This represents the event by which DMA transfer is being triggered. 
	dmaConfig.dmaInt       = CSL_DMA_INTERRUPT_ENABLE;	// ? - This provides the information about DMA Interrupt. After completion of data transfer will be served by interrupt. 
	dmaConfig.chanDir      = CSL_DMA_READ;				// OK 
	dmaConfig.trfType      = CSL_DMA_TRANSFER_IO_MEMORY;// OK
	dmaConfig.dataLen      = CSL_DMA_BUFFER_SIZE * 4;	// ? - Length of data to transfer in # of bytes 
	dmaConfig.srcAddr      = (Uint32)dmaPingSrcBuf;		// ? - Address of source data buffer 
	dmaConfig.destAddr     = (Uint32)dmaPingDstBuf;		// ? - Address of destination data buffer 
	*/
	
    IRQ_globalDisable();

	IRQ_clearAll();

	IRQ_disableAll();

	IRQ_setVecs((Uint32)&VECSTART);
	IRQ_clear(DMA_EVENT);

	IRQ_plug (DMA_EVENT, &dma_isr);

	IRQ_enable(DMA_EVENT);
	IRQ_globalEnable();

    status = DMA_init();
    if (status != CSL_SOK)
    {
        printf("DMA_init() Failed \n"); /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --   Reseting PaSs_StAtE to 0 if error detected here.
        PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
    }
    
	for( chanNumber = 0; chanNumber < CSL_DMA_CHAN_MAX; chanNumber++)
	{
	    count = 0;
	    printf("\nTest for DMA Channel Number: %d\n", chanNumber);

		for(index = 0; index < CSL_DMA_BUFFER_SIZE; index++)
		{
			dmaPingSrcBuf[index]  = index;
			dmaPongSrcBuf[index]  = 2*index;

			dmaPingDstBuf[index] = 0x0000;
			dmaPongDstBuf[index] = 0x0000;
		}

		dmaHandle = DMA_open((CSL_DMAChanNum)chanNumber,&dmaObj, &status);
        if (dmaHandle == NULL)
        {
            printf("DMA_open() Failed \n");
            break;
        }

		status = DMA_config(dmaHandle, &dmaConfig);
        if (status != CSL_SOK)
        {
            printf("DMA_config() Failed \n");
            break;
        }

		status = DMA_start(dmaHandle);
        if (status != CSL_SOK)
        {
            printf("DMA_start() Failed \n");
            break;
        }
        /* I think that once DMA_start has been called (assuming the trigger event is software, and there is no trigger event), the ISR keeps being called 
         * From the tech ref: In non-synchronized transfers (SYNCMODE = 0), the channel sends an access request to the 
         * source address as soon as the channel is enabled (EN = 1 in DMACHmTCR2). The channel will transfer data from
         * the source address to the FIFO, and then to the destination address until the entire block transfer has been 
         * completed or until the user program disables the channel.
         */ 
        

 	    while(count != 2);

		status = DMA_close(dmaHandle);
        if (status != CSL_SOK)
        {
            printf("DMA_close() Failed \n");
            break;
        }

        status = DMA_reset(dmaHandle);
        if (status != CSL_SOK)
        {
            printf("DMA_reset() Failed \n");
            break;
        }

		for(index = 0; index < CSL_DMA_BUFFER_SIZE; index++)
		{
			if(dmaPingSrcBuf[index] != dmaPingDstBuf[index])
			{
				printf("Ping Buffer Miss Matched at Position %d\n", index);
				break;
			}

			if(dmaPongSrcBuf[index] != dmaPongDstBuf[index])
			{
				printf("Pong Buffer Miss Matched at Position %d\n", index);
				break;
			}
		}

		if(index == CSL_DMA_BUFFER_SIZE)
		{
			printf("Test Successful\n");
		}
	}

	IRQ_clearAll();
	IRQ_disableAll();
	IRQ_globalDisable();

	if(isrEntryCount == 32)
	{
		printf("\n\nDMA PING-PONG MODE TEST PASSED!!\n");
	}
	else
	{
		printf("\n\nDMA PING-PONG MODE TEST FAILED!!\n");
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --   
   /////  Reseting PaSs_StAtE to 0 if error detected here.
        PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
	}
#else
	printf("\n\nINVALID TEST FOR THE CHIP VERSION!!\n");
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --   
   /////  Reseting PaSs_StAtE to 0 if error detected here.
        PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
#endif
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 3 -- 
   /////  At program exit, copy "PaSs_StAtE" into "PaSs".
        PaSs = PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with 
   /////                   // pass/fail value determined during program execution.
   /////  Note:  Program should next exit to C$$EXIT and halt, where DSS, under
   /////   control of a host PC script, will read and record the PaSs' value.  
}

/** \brief  DMA Interrupt Service routine - param none - return none */
interrupt void dma_isr(void)
{
    int ifrValue;

  	ifrValue = CSL_SYSCTRL_REGS->DMAIFR;
	CSL_SYSCTRL_REGS->DMAIFR |= ifrValue;

#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined(CHIP_C5517))

	if ((DMA_getLastTransferType (dmaHandle, &status)) == 1)
	{
		printf("Pong Set Transfer Completed\n");
	}
	else
	{
		printf("Ping Set Transfer Completed\n");
	}

#endif

	++count;
	++isrEntryCount;
}


