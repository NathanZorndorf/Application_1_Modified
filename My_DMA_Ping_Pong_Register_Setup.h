#ifndef MY_DMA_PING_PONG_REGISTER_SETUP_H_
#define MY_DMA_PING_PONG_REGISTER_SETUP_H_

	int My_DMA_Ping_Pong_Register_Setup(void);
  	interrupt void DMA_ISR(void);
  	Uint32 convert_address(Int16 *buffer);
  	
extern Int16 DMA_InpL[PING_PONG_SIZE];
extern Int16 DMA_InpR[PING_PONG_SIZE];
extern Int16 DMA_OutL[PING_PONG_SIZE];
extern Int16 DMA_OutR[PING_PONG_SIZE];

extern Int16 PingPongFlagInL;
extern Int16 PingPongFlagInR;
extern Int16 PingPongFlagOutL;
extern Int16 PingPongFlagOutR;

#endif /*MY_DMA_PING_PONG_REGISTER_SETUP_H_*/
