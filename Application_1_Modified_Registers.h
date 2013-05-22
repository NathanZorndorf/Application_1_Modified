#ifndef APPLICATION_1_MODIFIED_REGISTERS_H_
#define APPLICATION_1_MODIFIED_REGISTERS_H_

// main.c
#define DMA_IFR     *(ioport volatile unsigned *)0x1C30    // DMA Interrupt Flag Register
#define DMA_MSK     *(ioport volatile unsigned *)0x1C31    // DMA Interrupt Mask Flag Register
#define IER0        *(volatile unsigned *)0x0000
#define IFR0        *(volatile unsigned *)0x0001
#define IER1        *(volatile unsigned *)0x0045
#define IFR1        *(volatile unsigned *)0x0046
// NOTE: The following definitions assume 50% overlap 
#define HOP_SIZE 256            		// Hop size of overlap-add
#define AUDIO_IO_SIZE  HOP_SIZE 		// DMA transfer size 
#define PING_PONG_SIZE (2 * AUDIO_IO_SIZE)
#define WND_LEN     (2 * HOP_SIZE) 		// Window length 
#define FFT_LENGTH  WND_LEN        		// FFT length equal to WND_LEN - CAUTION: if you change HOP_SIZE and hence WND_LEN and FFT_LENGTH, you must also manually change the corresponding hwafft functions in do_fft(), do_ifft() 
#define NUM_BINS	(FFT_LENGTH/2 + 1) 	// Useful freq. bins, there rest are symmetric  
#define OVERLAP_LENGTH (WND_LEN - HOP_SIZE)

// My_PLL.c
#define PLL_CCR2  *(volatile ioport Uint16*)(0x1C1F)
#define PLL_CGCR1 *(volatile ioport Uint16*)(0x1C20)
#define PLL_CGCR2 *(volatile ioport Uint16*)(0x1C21)
#define PLL_CGCR3 *(volatile ioport Uint16*)(0x1C22)
#define PLL_CGCR4 *(volatile ioport Uint16*)(0x1C23)

#define CPU_EBSR          	   *(volatile ioport Uint16*)(0x1C00) // External Bus Selection Register
#define CPU_PSRCR			   *(volatile ioport Uint16*)(0x1C04) // Peripheral Software Reset Counter Register
#define CPU_PRCR			   *(volatile ioport Uint16*)(0x1C05) // Peripheral Software Reset Counter Register
#define CPU_CLKSTOP			   *(volatile ioport Uint16*)(0x1C3A)

// My_DMA
/* ==================== Some CPU Registers ==================== */
#define IER0        			 *(volatile unsigned *)0x0000
#define IFR0        			 *(volatile unsigned *)0x0001
#define IER1        			 *(volatile unsigned *)0x0045
#define IFR1        			 *(volatile unsigned *)0x0046
#define PRCR       				 *(volatile ioport Uint16*)(0x1C05)

/* ==================== General DMA Registers ==================== */
#define DMA_IFR     			 *(ioport volatile unsigned *)0x1C30    // DMA Interrupt Flag Register
#define DMA_IER     			 *(ioport volatile unsigned *)0x1C31    // DMA Interrupt Enable Register

/* ==================== DMA Controller 1 ==================== */
#define DMA1_CESR1 *(ioport volatile unsigned *)0x1C1C //DMA1 Channel Event Source Register 1
#define DMA1_CESR2 *(ioport volatile unsigned *)0x1C1D //DMA1 Channel Event Source Register 2
/* ------------------------ Channel 0 ----------------------- */
#define DMA1_CH0_SSAL *(ioport volatile unsigned *)0x0D00 //Channel 0 Source Start Address (Lower Part) Register
#define DMA1_CH0_SSAU *(ioport volatile unsigned *)0x0D01 //Channel 0 Source Start Address (Upper Part) Register
#define DMA1_CH0_DSAL *(ioport volatile unsigned *)0x0D02 //Channel 0 Destination Start Address (Lower Part) Register
#define DMA1_CH0_DSAU *(ioport volatile unsigned *)0x0D03 //Channel 0 Destination Start Address (Upper Part) Register
#define DMA1_CH0_TCR1 *(ioport volatile unsigned *)0x0D04 //Channel 0 Transfer Control Register 1
#define DMA1_CH0_TCR2 *(ioport volatile unsigned *)0x0D05 //Channel 0 Transfer Control Register 2
/* ---------------------------------------------------------- */

/* ------------------------ Channel 1 ----------------------- */
#define DMA1_CH1_SSAL *(ioport volatile unsigned *)0x0D20 //Channel 1 Source Start Address (Lower Part) Register
#define DMA1_CH1_SSAU *(ioport volatile unsigned *)0x0D21 //Channel 1 Source Start Address (Upper Part) Register
#define DMA1_CH1_DSAL *(ioport volatile unsigned *)0x0D22 //Channel 1 Destination Start Address (Lower Part) Register
#define DMA1_CH1_DSAU *(ioport volatile unsigned *)0x0D23 //Channel 1 Destination Start Address (Upper Part) Register
#define DMA1_CH1_TCR1 *(ioport volatile unsigned *)0x0D24 //Channel 1 Transfer Control Register 1
#define DMA1_CH1_TCR2 *(ioport volatile unsigned *)0x0D25 //Channel 1 Transfer Control Register 2
/* ---------------------------------------------------------- */

/* ------------------------ Channel 2 ----------------------- */
#define DMA1_CH2_SSAL *(ioport volatile unsigned *)0x0D40 //Channel 2 Source Start Address (Lower Part) Register
#define DMA1_CH2_SSAU *(ioport volatile unsigned *)0x0D41 //Channel 2 Source Start Address (Upper Part) Register
#define DMA1_CH2_DSAL *(ioport volatile unsigned *)0x0D42 //Channel 2 Destination Start Address (Lower Part) Register
#define DMA1_CH2_DSAU *(ioport volatile unsigned *)0x0D43 //Channel 2 Destination Start Address (Upper Part) Register
#define DMA1_CH2_TCR1 *(ioport volatile unsigned *)0x0D44 //Channel 2 Transfer Control Register 1
#define DMA1_CH2_TCR2 *(ioport volatile unsigned *)0x0D45 //Channel 2 Transfer Control Register 2
/* ---------------------------------------------------------- */

/* ------------------------ Channel 3 ----------------------- */
#define DMA1_CH3_SSAL *(ioport volatile unsigned *)0x0D60 //Channel 3 Source Start Address (Lower Part) Register
#define DMA1_CH3_SSAU *(ioport volatile unsigned *)0x0D61 //Channel 3 Source Start Address (Upper Part) Register
#define DMA1_CH3_DSAL *(ioport volatile unsigned *)0x0D62 //Channel 3 Destination Start Address (Lower Part) Register
#define DMA1_CH3_DSAU *(ioport volatile unsigned *)0x0D63 //Channel 3 Destination Start Address (Upper Part) Register
#define DMA1_CH3_TCR1 *(ioport volatile unsigned *)0x0D64 //Channel 3 Transfer Control Register 1
#define DMA1_CH3_TCR2 *(ioport volatile unsigned *)0x0D65 //Channel 3 Transfer Control Register 2
/* ---------------------------------------------------------- */

/* ==================== I2S2 Registers  ==================== */
#define I2S2_I2SSCTRL            *(volatile ioport Uint16*)(0x2A00) // I2S2 Serializer Control Register
#define I2S2_I2SSRATE            *(volatile ioport Uint16*)(0x2A04) // I2S2 Sample Rate Generator Register
#define I2S2_I2SINTFL            *(volatile ioport Uint16*)(0x2A10) // I2S2 Interrupt Flag Register
#define I2S2_I2SINTMASK          *(volatile ioport Uint16*)(0x2A14) // I2S2 Interrupt Mask Register

#define I2S2_Left_RX_LSW      		 *(volatile ioport Uint16*)(0x2A28)
#define I2S2_Left_RX_MSW      		 *(volatile ioport Uint16*)(0x2A29)
#define I2S2_Right_RX_LSW      	 	 *(volatile ioport Uint16*)(0x2A2C)
#define I2S2_Right_RX_MSW      	 	 *(volatile ioport Uint16*)(0x2A2D)

#define I2S2_Left_TX_LSW      		 *(volatile ioport Uint16*)(0x2A08)
#define I2S2_Left_TX_MSW      		 *(volatile ioport Uint16*)(0x2A09)
#define I2S2_Right_TX_LSW      	 	 *(volatile ioport Uint16*)(0x2A0C)
#define I2S2_Right_TX_MSW      	 	 *(volatile ioport Uint16*)(0x2A0D)


/* from usbstk5505.h */
/* ------------------------------------------------------------------------ *
 *  System Module                                                           *
 * ------------------------------------------------------------------------ */
#define SYS_EXBUSSEL       *(volatile ioport Uint16*)(0x1c00)
#define SYS_PCGCR1         *(volatile ioport Uint16*)(0x1c02)
#define SYS_PCGCR2         *(volatile ioport Uint16*)(0x1c03)
#define SYS_PRCNTR         *(volatile ioport Uint16*)(0x1c04)
#define SYS_PRCNTRLR       *(volatile ioport Uint16*)(0x1c05)
#define SYS_GPIO_DIR0      *(volatile ioport Uint16*)(0x1c06)
#define SYS_GPIO_DIR1      *(volatile ioport Uint16*)(0x1c07)
#define SYS_GPIO_DATAIN0   *(volatile ioport Uint16*)(0x1c08)
#define SYS_GPIO_DATAIN1   *(volatile ioport Uint16*)(0x1c09)
#define SYS_GPIO_DATAOUT0  *(volatile ioport Uint16*)(0x1c0a)
#define SYS_GPIO_DATAOUT1  *(volatile ioport Uint16*)(0x1c0b)
#define SYS_OUTDRSTR       *(volatile ioport Uint16*)(0x1c16)
#define SYS_SPPDIR         *(volatile ioport Uint16*)(0x1c17)


/* ------------------------------------------------------------------------ *
 *  I2C Module                                                              *
 * ------------------------------------------------------------------------ */
 
#define I2C_IER    	       *(volatile ioport Uint16*)(0x1A04)
#define I2C_STR    	       *(volatile ioport Uint16*)(0x1A08)
#define I2C_CLKL           *(volatile ioport Uint16*)(0x1A0C)
#define I2C_CLKH           *(volatile ioport Uint16*)(0x1A10)
#define I2C_CNT    		   *(volatile ioport Uint16*)(0x1A14)
#define I2C_DRR    		   *(volatile ioport Uint16*)(0x1A18)
#define I2C_SAR    	       *(volatile ioport Uint16*)(0x1A1C)
#define I2C_DXR    	       *(volatile ioport Uint16*)(0x1A20)
#define I2C_MDR            *(volatile ioport Uint16*)(0x1A24)
#define I2C_EDR    	       *(volatile ioport Uint16*)(0x1A2C)
#define I2C_PSC    	       *(volatile ioport Uint16*)(0x1A30)
/* ------------------------------------------------------------------------ *
 *  I2S Module                                                              *
 * ------------------------------------------------------------------------ */
 /*
#define I2S0_CR            *(volatile ioport Uint16*)(0x2800)
#define I2S0_SRGR          *(volatile ioport Uint16*)(0x2804)
#define I2S0_W0_LSW_W      *(volatile ioport Uint16*)(0x2808)
#define I2S0_W0_MSW_W      *(volatile ioport Uint16*)(0x2809)
#define I2S0_W1_LSW_W      *(volatile ioport Uint16*)(0x280C)
#define I2S0_W1_MSW_W      *(volatile ioport Uint16*)(0x280D)
#define I2S0_IR            *(volatile ioport Uint16*)(0x2810)
#define I2S0_ICMR          *(volatile ioport Uint16*)(0x2814)
#define I2S0_W0_LSW_R      *(volatile ioport Uint16*)(0x2828)
#define I2S0_W0_MSW_R      *(volatile ioport Uint16*)(0x2829)
#define I2S0_W1_LSW_R      *(volatile ioport Uint16*)(0x282C)
#define I2S0_W1_MSW_R      *(volatile ioport Uint16*)(0x282D)
*/

#define I2S2_CR            *(volatile ioport Uint16*)(0x2A00)
#define I2S2_SRGR          *(volatile ioport Uint16*)(0x2A04)
#define I2S2_W0_LSW_W      *(volatile ioport Uint16*)(0x2A08)
#define I2S2_W0_MSW_W      *(volatile ioport Uint16*)(0x2A09)
#define I2S2_W1_LSW_W      *(volatile ioport Uint16*)(0x2A0C)
#define I2S2_W1_MSW_W      *(volatile ioport Uint16*)(0x2A0D)
#define I2S2_IR            *(volatile ioport Uint16*)(0x2A10)
#define I2S2_ICMR          *(volatile ioport Uint16*)(0x2A14)
#define I2S2_W0_LSW_R      *(volatile ioport Uint16*)(0x2A28)
#define I2S2_W0_MSW_R      *(volatile ioport Uint16*)(0x2A29)
#define I2S2_W1_LSW_R      *(volatile ioport Uint16*)(0x2A2C)
#define I2S2_W1_MSW_R      *(volatile ioport Uint16*)(0x2A2D)

/* UART Registers */
#define UART_RBR	 	   *(volatile ioport Uint16*)(0x1B00) 
#define UART_THR	 	   *(volatile ioport Uint16*)(0x1B00) 
#define UART_IER	 	   *(volatile ioport Uint16*)(0x1B02) 
#define UART_IIR	 	   *(volatile ioport Uint16*)(0x1B04) 
#define UART_FCR	 	   *(volatile ioport Uint16*)(0x1B04) 
#define UART_LCR	 	   *(volatile ioport Uint16*)(0x1B06) 
#define UART_MCR	 	   *(volatile ioport Uint16*)(0x1B08) 
#define UART_LSR	 	   *(volatile ioport Uint16*)(0x1B0A) 
#define UART_SCR	 	   *(volatile ioport Uint16*)(0x1B0E) 
#define UART_DLL	 	   *(volatile ioport Uint16*)(0x1B10) 
#define UART_DLH	 	   *(volatile ioport Uint16*)(0x1B12) 
#define UART_PWREMU_MGMT   *(volatile ioport Uint16*)(0x1B18) 

#endif /*APPLICATION_1_MODIFIED_REGISTERS_H_*/
