#ifndef MY_AIC3204_H_
#define MY_AIC3204_H_

// Sampling rates
enum {AIC3204_FS_8KHZ,
	  AIC3204_FS_16KHZ,
	  AIC3204_FS_24KHZ,
	  AIC3204_FS_32KHZ,
	  AIC3204_FS_44_1KHZ,
	  AIC3204_FS_48KHZ,
	  AIC3204_FS_96KHZ};
	  
	int My_AIC3204(void);
	
#endif /*MY_AIC3204_H_*/
