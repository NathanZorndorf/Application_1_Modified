################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LIB_SRCS += \
../55xdsph.lib \
../CSLc55x5h.lib 

C_SRCS += \
../Audio_To_MIDI_Using_DMA.c \
../Audio_To_MIDI_Using_DMA_and_CFFT.c \
../Audio_To_MIDI_Using_DMA_and_HWAFFT.c \
../DMA_Ping_Pong.c \
../I2C_Test.c \
../My_AIC3204.c \
../My_DMA_Ping_Pong_Register_Setup.c \
../My_I2C.c \
../My_I2S.c \
../My_I2S_Register.c \
../My_PLL.c \
../My_UART_Register.c \
../Output_MIDI.c \
../PLL.c \
../aic3204.c \
../aic3204_init.c \
../aic_i2c.c \
../csl_intc.c \
../main.c \
../stereo.c \
../usbstk5505.c \
../usbstk5505_gpio.c \
../usbstk5505_i2c.c \
../usbstk5505_led.c 

ASM_SRCS += \
../add.asm \
../cbrev.asm \
../cbrev32.asm \
../cfft32_noscale.asm \
../cfft32_scale.asm \
../cfft_noscale.asm \
../cfft_scale.asm \
../maxvec.asm \
../mul32.asm \
../sqrtv.asm \
../vector.asm 

CMD_SRCS += \
../lnkx_Spectrum_Digital_UART_Ex.cmd 

ASM_DEPS += \
./add.pp \
./cbrev.pp \
./cbrev32.pp \
./cfft32_noscale.pp \
./cfft32_scale.pp \
./cfft_noscale.pp \
./cfft_scale.pp \
./maxvec.pp \
./mul32.pp \
./sqrtv.pp \
./vector.pp 

OBJS += \
./Audio_To_MIDI_Using_DMA.obj \
./Audio_To_MIDI_Using_DMA_and_CFFT.obj \
./Audio_To_MIDI_Using_DMA_and_HWAFFT.obj \
./DMA_Ping_Pong.obj \
./I2C_Test.obj \
./My_AIC3204.obj \
./My_DMA_Ping_Pong_Register_Setup.obj \
./My_I2C.obj \
./My_I2S.obj \
./My_I2S_Register.obj \
./My_PLL.obj \
./My_UART_Register.obj \
./Output_MIDI.obj \
./PLL.obj \
./add.obj \
./aic3204.obj \
./aic3204_init.obj \
./aic_i2c.obj \
./cbrev.obj \
./cbrev32.obj \
./cfft32_noscale.obj \
./cfft32_scale.obj \
./cfft_noscale.obj \
./cfft_scale.obj \
./csl_intc.obj \
./main.obj \
./maxvec.obj \
./mul32.obj \
./sqrtv.obj \
./stereo.obj \
./usbstk5505.obj \
./usbstk5505_gpio.obj \
./usbstk5505_i2c.obj \
./usbstk5505_led.obj \
./vector.obj 

C_DEPS += \
./Audio_To_MIDI_Using_DMA.pp \
./Audio_To_MIDI_Using_DMA_and_CFFT.pp \
./Audio_To_MIDI_Using_DMA_and_HWAFFT.pp \
./DMA_Ping_Pong.pp \
./I2C_Test.pp \
./My_AIC3204.pp \
./My_DMA_Ping_Pong_Register_Setup.pp \
./My_I2C.pp \
./My_I2S.pp \
./My_I2S_Register.pp \
./My_PLL.pp \
./My_UART_Register.pp \
./Output_MIDI.pp \
./PLL.pp \
./aic3204.pp \
./aic3204_init.pp \
./aic_i2c.pp \
./csl_intc.pp \
./main.pp \
./stereo.pp \
./usbstk5505.pp \
./usbstk5505_gpio.pp \
./usbstk5505_i2c.pp \
./usbstk5505_led.pp 

OBJS__QTD += \
".\Audio_To_MIDI_Using_DMA.obj" \
".\Audio_To_MIDI_Using_DMA_and_CFFT.obj" \
".\Audio_To_MIDI_Using_DMA_and_HWAFFT.obj" \
".\DMA_Ping_Pong.obj" \
".\I2C_Test.obj" \
".\My_AIC3204.obj" \
".\My_DMA_Ping_Pong_Register_Setup.obj" \
".\My_I2C.obj" \
".\My_I2S.obj" \
".\My_I2S_Register.obj" \
".\My_PLL.obj" \
".\My_UART_Register.obj" \
".\Output_MIDI.obj" \
".\PLL.obj" \
".\add.obj" \
".\aic3204.obj" \
".\aic3204_init.obj" \
".\aic_i2c.obj" \
".\cbrev.obj" \
".\cbrev32.obj" \
".\cfft32_noscale.obj" \
".\cfft32_scale.obj" \
".\cfft_noscale.obj" \
".\cfft_scale.obj" \
".\csl_intc.obj" \
".\main.obj" \
".\maxvec.obj" \
".\mul32.obj" \
".\sqrtv.obj" \
".\stereo.obj" \
".\usbstk5505.obj" \
".\usbstk5505_gpio.obj" \
".\usbstk5505_i2c.obj" \
".\usbstk5505_led.obj" \
".\vector.obj" 

ASM_DEPS__QTD += \
".\add.pp" \
".\cbrev.pp" \
".\cbrev32.pp" \
".\cfft32_noscale.pp" \
".\cfft32_scale.pp" \
".\cfft_noscale.pp" \
".\cfft_scale.pp" \
".\maxvec.pp" \
".\mul32.pp" \
".\sqrtv.pp" \
".\vector.pp" 

C_DEPS__QTD += \
".\Audio_To_MIDI_Using_DMA.pp" \
".\Audio_To_MIDI_Using_DMA_and_CFFT.pp" \
".\Audio_To_MIDI_Using_DMA_and_HWAFFT.pp" \
".\DMA_Ping_Pong.pp" \
".\I2C_Test.pp" \
".\My_AIC3204.pp" \
".\My_DMA_Ping_Pong_Register_Setup.pp" \
".\My_I2C.pp" \
".\My_I2S.pp" \
".\My_I2S_Register.pp" \
".\My_PLL.pp" \
".\My_UART_Register.pp" \
".\Output_MIDI.pp" \
".\PLL.pp" \
".\aic3204.pp" \
".\aic3204_init.pp" \
".\aic_i2c.pp" \
".\csl_intc.pp" \
".\main.pp" \
".\stereo.pp" \
".\usbstk5505.pp" \
".\usbstk5505_gpio.pp" \
".\usbstk5505_i2c.pp" \
".\usbstk5505_led.pp" 

C_SRCS_QUOTED += \
"../Audio_To_MIDI_Using_DMA.c" \
"../Audio_To_MIDI_Using_DMA_and_CFFT.c" \
"../Audio_To_MIDI_Using_DMA_and_HWAFFT.c" \
"../DMA_Ping_Pong.c" \
"../I2C_Test.c" \
"../My_AIC3204.c" \
"../My_DMA_Ping_Pong_Register_Setup.c" \
"../My_I2C.c" \
"../My_I2S.c" \
"../My_I2S_Register.c" \
"../My_PLL.c" \
"../My_UART_Register.c" \
"../Output_MIDI.c" \
"../PLL.c" \
"../aic3204.c" \
"../aic3204_init.c" \
"../aic_i2c.c" \
"../csl_intc.c" \
"../main.c" \
"../stereo.c" \
"../usbstk5505.c" \
"../usbstk5505_gpio.c" \
"../usbstk5505_i2c.c" \
"../usbstk5505_led.c" 

ASM_SRCS_QUOTED += \
"../add.asm" \
"../cbrev.asm" \
"../cbrev32.asm" \
"../cfft32_noscale.asm" \
"../cfft32_scale.asm" \
"../cfft_noscale.asm" \
"../cfft_scale.asm" \
"../maxvec.asm" \
"../mul32.asm" \
"../sqrtv.asm" \
"../vector.asm" 


