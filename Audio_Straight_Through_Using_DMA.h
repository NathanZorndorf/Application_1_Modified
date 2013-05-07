#ifndef AUDIO_STRAIGHT_THROUGH_USING_DMA_H_
#define AUDIO_STRAIGHT_THROUGH_USING_DMA_H_

	int Audio_Straight_Through_Using_DMA(void); 

	extern Int16 DMA_InpL[PING_PONG_SIZE];
	extern Int16 DMA_InpR[PING_PONG_SIZE];
	extern Int16 DMA_OutL[PING_PONG_SIZE];
	extern Int16 DMA_OutR[PING_PONG_SIZE];
	
	extern Int16 PingPongFlagInL;
	extern Int16 PingPongFlagInR;
	extern Int16 PingPongFlagOutL;
	extern Int16 PingPongFlagOutR;


#endif /*AUDIO_STRAIGHT_THROUGH_USING_DMA_H_*/
