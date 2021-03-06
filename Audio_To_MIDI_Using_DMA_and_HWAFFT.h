#ifndef AUDIO_TO_MIDI_USING_DMA_AND_HWAFFT_H_
#define AUDIO_TO_MIDI_USING_DMA_AND_HWAFFT_H_

	int Audio_To_MIDI_Using_DMA_and_HWAFFT(void); 

	extern Int16 DMA_InpL[PING_PONG_SIZE];
	extern Int16 DMA_InpR[PING_PONG_SIZE];
	extern Int16 DMA_OutL[PING_PONG_SIZE];
	extern Int16 DMA_OutR[PING_PONG_SIZE];
	
	extern Int16 PingPongFlagInL;
	extern Int16 PingPongFlagInR;
	extern Int16 PingPongFlagOutL;
	extern Int16 PingPongFlagOutR;
	
	#define HWAFFT_SCALE	1
	
#endif /*AUDIO_TO_MIDI_USING_DMA_AND_HWAFFT_H_*/
