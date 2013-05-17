
MEMORY
{
    MMR     (RW) : origin = 0000000h length = 0000c0h /* MMRs */
    /*DARAM (RW)    : origin = 00000c0h length = 00ff40h*/  /* on-chip DARAM */
    DARAM_0 (RW)  : origin = 00000c0h length = 001f40h
    DARAM_1 (RW)  : origin = 0002000h length = 004000h
    /*DARAM_2 (RW)  : origin = 0004000h length = 002000h */
    DARAM_3 (RW)  : origin = 0006000h length = 002000h
    DARAM   (RW)  : origin = 0008000h length = 008000h
    
    SARAM   (RW)  : origin = 0010000h length = 040000h /* on-chip SARAM */

    SAROM_0 (RX)  : origin = 0fe0000h length = 008000h 	/* on-chip ROM 0 */
    SAROM_1 (RX)  : origin = 0fe8000h length = 008000h 	/* on-chip ROM 1 */
    SAROM_2 (RX)  : origin = 0ff0000h length = 008000h 	/* on-chip ROM 2 */
    SAROM_3 (RX)  : origin = 0ff8000h length = 008000h 	/* on-chip ROM 3 */
    
    EMIF_CS0 (RW)  : origin = 0050000h  length = 07B0000h   /* mSDR */ 
	EMIF_CS2 (RW)  : origin = 0800000h  length = 0400000h   /* ASYNC1 : NAND */ 
	EMIF_CS3 (RW)  : origin = 0C00000h  length = 0200000h   /* ASYNC2 : NAND  */
	EMIF_CS4 (RW)  : origin = 0E00000h  length = 0100000h   /* ASYNC3 : NOR */
	EMIF_CS5 (RW)  : origin = 0F00000h  length = 00E0000h   /* ASYNC4 : SRAM */

}


SECTIONS
{
    vectors (NOLOAD)
    .bss        : > DARAM /*, fill = 0 */
    vector      : > DARAM      ALIGN = 256 
    .stack      : > DARAM  
    .sysstack   : > DARAM  
	.sysmem 	: > DARAM 
    .text       : > SARAM  
    .data       : > DARAM
	.cinit 		: > DARAM
	.const 		: > DARAM
	.cio		: > DARAM
	.usect   	: > DARAM
	.switch     : > DARAM 
	.emif_cs0   : > EMIF_CS0
	.emif_cs2   : > EMIF_CS2
	.emif_cs3   : > EMIF_CS3
	.emif_cs4   : > EMIF_CS4
	.emif_cs5   : > EMIF_CS5
	/* --- MY DEFINITIONS --- */
	/* For DSPLIB FFT */
	/* .data:twiddle    : > DARAM_0 ALIGN = 2048 */
	/* .fftcode         : > SARAM */
	
	cmplxBuf  : > DARAM_1
	BufL      : > DARAM_1
	BufR      : > DARAM_1
	PSD		  : > DARAM_1
	PSD_sqrt  : > DARAM_1
	
	tmpBuf	  : > DARAM_1
	
	brBuf	  : > DARAM_1
	
	wnd1	  : > DARAM_3
	wnd2	  : > DARAM_3
	
	rfftL     : > DARAM_1
	ifftL     : > DARAM_1
	rfftR     : > DARAM_1
	ifftR     : > DARAM_1
}


/* C5535 HWAFFT ROM table addresses */
_hwafft_br       = 0x00fefe9c;
_hwafft_8pts     = 0x00fefeb0;
_hwafft_16pts    = 0x00feff9f;
_hwafft_32pts    = 0x00ff00f5;
_hwafft_64pts    = 0x00ff03fe;
_hwafft_128pts   = 0x00ff0593;
_hwafft_256pts   = 0x00ff07a4;
_hwafft_512pts   = 0x00ff09a2;
_hwafft_1024pts  = 0x00ff0c1c;

