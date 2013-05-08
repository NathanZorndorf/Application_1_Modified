//////////////////////////////////////////////////////////////////////////////
// * File name: main_bypass1.c
// *                                                                          
// * Description: This file includes main() and system initialization funcitons.
// *                                                                          
// * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/ 
// *                                                                          
// *                                                                          
// *  Redistribution and use in source and binary forms, with or without      
// *  modification, are permitted provided that the following conditions      
// *  are met:                                                                
// *                                                                          
// *    Redistributions of source code must retain the above copyright        
// *    notice, this list of conditions and the following disclaimer.         
// *                                                                          
// *    Redistributions in binary form must reproduce the above copyright     
// *    notice, this list of conditions and the following disclaimer in the   
// *    documentation and/or other materials provided with the                
// *    distribution.                                                         
// *                                                                          
// *    Neither the name of Texas Instruments Incorporated nor the names of   
// *    its contributors may be used to endorse or promote products derived   
// *    from this software without specific prior written permission.         
// *                                                                          
// *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS     
// *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT       
// *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR   
// *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT    
// *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,   
// *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT        
// *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,   
// *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY   
// *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT     
// *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE   
// *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.    
// *                                                                          
//////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "data_types.h"
#include "register_system.h"
#include "register_cpu.h"
#include "rtc.h"
#include "control.h"
#include "i2s_bypass1.h"
#include "dma_bypass1.h"
#include "ref_data_bypass.h"


void InitSystem(void);
void ConfigPort(void);
void SYS_GlobalIntEnable(void);
void SYS_GlobalIntDisable(void);
void PLL_98MHz(void);
void buff_copy(Int16 *input, Int16 *output, Int16 size);
void turnOnLED(void);
void turnOffLED(void);
//void BlinkLED(void);

Uint16 fFilterOn = 0;
Uint16 fBypassOn = 1;

extern Uint16 Flag_RTC;
extern void AIC3254_init(void);
extern void DSP_fir(void);
extern Uint16 CurrentRxL_DMAChannel;
extern Uint16 CurrentRxR_DMAChannel;
extern Uint16 CurrentTxL_DMAChannel;
extern Uint16 CurrentTxR_DMAChannel;
extern Uint16 RunFilterForL;
extern Uint16 RunFilterForR;


void main(void)
{
    Int16 i;
    
    InitSystem();
    ConfigPort();

    SYS_GlobalIntEnable();
	reset_RTC();    
    
    IER0 = 0x0110;      // enable dma, timer int      
    IER1 = 0x0004;      // enable RTC int

	// clean up delay filter out buffer
	for(i=0; i <COEF_48+2; i++)
	{
		DelayFilterOutL[i] =0;
		DelayFilterOutR[i] =0;
	}

    setDMA_address();
    //set_i2s0_master();
    set_i2s0_slave();
    AIC3254_init();

    PLL_98MHz();

	enable_i2s0();
    enable_dma_int();
    set_dma0_ch0_i2s0_Lout();
    set_dma0_ch1_i2s0_Rout();
    set_dma0_ch2_i2s0_Lin();
	set_dma0_ch3_i2s0_Rin();
	enable_rtc_second_int();

    while(1)
    {
        while(fFilterOn)
        {
            turnOnLED();
            if(RunFilterForL ==1)
            {
                RunFilterForL =0;
                if (CurrentRxL_DMAChannel ==2)
                {
                    fir(&RcvL1[0],&coeff_fir_48[0],&FilterOutL1[0], &DelayFilterOutL[0], XMIT_BUFF_SIZE, COEF_48);
               }
                else
                {
                    fir(&RcvL2[0],&coeff_fir_48[0],&FilterOutL2[0], &DelayFilterOutL[0], XMIT_BUFF_SIZE, COEF_48);                
                }
                
            }
            if(RunFilterForR ==1)
            {
                RunFilterForR=0;
                if (CurrentRxR_DMAChannel ==2)
                {
                    fir(&RcvR1[0],&coeff_fir_48[0],&FilterOutR1[0], &DelayFilterOutR[0], XMIT_BUFF_SIZE, COEF_48);
                }
                else
                {
                    fir(&RcvR2[0],&coeff_fir_48[0],&FilterOutR2[0], &DelayFilterOutR[0], XMIT_BUFF_SIZE, COEF_48);                
                }
            }
        }
        
        while(fBypassOn)
        {
            turnOffLED();
            if(RunFilterForL ==1)
            {
                RunFilterForL =0;
                if (CurrentRxL_DMAChannel ==2)
                {
                    buff_copy(&RcvL1[0],&FilterOutL1[0], XMIT_BUFF_SIZE);
               }
                else
                {
                    buff_copy(&RcvL2[0],&FilterOutL2[0], XMIT_BUFF_SIZE);                
                }
                
            }
            if(RunFilterForR ==1)
            {
                RunFilterForR=0;
                if (CurrentRxR_DMAChannel ==2)
                {
                    buff_copy(&RcvR1[0],&FilterOutR1[0], XMIT_BUFF_SIZE);
                }
                else
                {
                    buff_copy(&RcvR2[0],&FilterOutR2[0], XMIT_BUFF_SIZE);                
                }
            }
        }
    }

}

void PLL_98MHz(void)
{
// PLL set up from RTC
    // bypass PLL
    CONFIG_MSW = 0x0;

#if (PLL_100M ==1)
    PLL_CNTL2 = 0x8000;
    PLL_CNTL4 = 0x0000;
    PLL_CNTL3 = 0x0806;
    PLL_CNTL1 = 0x8BE8; //PG1.4: 0x82FA;
    
#elif (PLL_12M ==1)
    PLL_CNTL2 = 0x8000;
    PLL_CNTL4 = 0x0200;
    PLL_CNTL3 = 0x0806;
    PLL_CNTL1 = 0x82ED;
#elif (PLL_98M ==1)    
    PLL_CNTL2 = 0x8000;
    PLL_CNTL4 = 0x0000;
    PLL_CNTL3 = 0x0806;
    PLL_CNTL1 = 0x82ED;
    
#endif
    while ( (PLL_CNTL3 & 0x0008) == 0);
    // Switch to PLL clk
    CONFIG_MSW = 0x1;
}

void InitSystem(void)
{
	Uint16 i;
// PLL set up from RTC
    // bypass PLL
    CONFIG_MSW = 0x0;

#if (PLL_100M ==1)
    PLL_CNTL2 = 0x8000;
    PLL_CNTL4 = 0x0000;
    PLL_CNTL3 = 0x0806;
    PLL_CNTL1 = 0x8BE8; //PG1.4: 0x82FA;
    
#elif (PLL_12M ==1)
    PLL_CNTL2 = 0x8000;
    PLL_CNTL4 = 0x0200;
    PLL_CNTL3 = 0x0806;
    PLL_CNTL1 = 0x82ED;
#elif (PLL_98M ==1)    
    PLL_CNTL2 = 0x8000;
    PLL_CNTL4 = 0x0000;
    PLL_CNTL3 = 0x0806;
    PLL_CNTL1 = 0x82ED;

#elif (PLL_40M ==1)        
    PLL_CNTL2 = 0x8000;
    PLL_CNTL4 = 0x0300;
    PLL_CNTL3 = 0x0806;
    PLL_CNTL1 = 0x8262;    
#endif

    while ( (PLL_CNTL3 & 0x0008) == 0);
    // Switch to PLL clk
    CONFIG_MSW = 0x1;

// clock gating
// enable all clocks
    IDLE_PCGCR = 0x0;
    IDLE_PCGCR_MSW = 0xFF84;
    

// reset peripherals
    PER_RSTCOUNT = 0x02;
    PER_RESET = 0x00fb;    
    for (i=0; i< 0xFFF; i++);
}

void ConfigPort(void)
{
    Int16 i;
    //  configure ports
    PERIPHSEL0 = 0x6900;        // parallel port: mode 6, serial port1: mode 2 

    for (i=0; i< 0xFFF; i++);
}


void SYS_GlobalIntEnable(void)
{
    asm(" BIT (ST1, #ST1_INTM) = #0");
}

void SYS_GlobalIntDisable(void)
{
    asm(" BIT (ST1, #ST1_INTM) = #1");
}

void turnOnLED(void)
{
    Uint16 temp;
    
    temp = ST1_55;
    if((temp&0x2000) == 0)
    {
        // turn on LED
        temp |= 0x2000;
        ST1_55 =temp;
    }
    
}


void turnOffLED(void)
{
    Uint16 temp;
    
    temp = ST1_55;
    if((temp&0x2000) != 0)
    {
        // turn off LED
        temp &=0xDFFF;
        ST1_55 =temp;
    }
}


#if 0
void BlinkLED(void)
{
    Uint16 temp;

    if(fLedBlinkOn ==0)
        return;
            
    if(Conunt_RTC > 0)
    {
        Conunt_RTC =0;
        
        temp = ST1_55;
        if((temp&0x2000) == 0)
        {
            // turn on LED
            temp |= 0x2000;
        }
        else
        {
            // turn off LED
            temp &=0xDFFF;
        }
        ST1_55 = temp; 
    }
    
}
#endif

void buff_copy(Int16 *input, Int16 *output, Int16 size)
{
	Int16 i;
	
	for(i =0; i<size; i++)
	{
		*(output + i) = *(input +i);
	}
	
}





