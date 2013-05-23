################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

EMPTY := 
SPACE := $(EMPTY) $(EMPTY)
CG_TOOL_ROOT := C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500

ORDERED_OBJS += \
$(GEN_CMDS_QUOTED) \
"./vector.obj" \
"./usbstk5505_led.obj" \
"./usbstk5505_i2c.obj" \
"./usbstk5505_gpio.obj" \
"./usbstk5505.obj" \
"./stereo.obj" \
"./sqrtv.obj" \
"./mul32.obj" \
"./maxvec.obj" \
"./main.obj" \
"./csl_intc.obj" \
"./cfft_scale.obj" \
"./cfft_noscale.obj" \
"./cfft32_scale.obj" \
"./cfft32_noscale.obj" \
"./cbrev32.obj" \
"./cbrev.obj" \
"./aic_i2c.obj" \
"./aic3204_init.obj" \
"./aic3204.obj" \
"./add.obj" \
"./PLL.obj" \
"./Output_MIDI.obj" \
"./My_UART_Register.obj" \
"./My_PLL.obj" \
"./My_I2S_Register.obj" \
"./My_I2S.obj" \
"./My_I2C.obj" \
"./My_DMA_Ping_Pong_Register_Setup.obj" \
"./My_AIC3204.obj" \
"./I2C_Test.obj" \
"./DMA_Ping_Pong.obj" \
"./Audio_To_MIDI_Using_DMA_and_HWAFFT.obj" \
"./Audio_To_MIDI_Using_DMA_and_CFFT.obj" \
"./Audio_To_MIDI_Using_DMA.obj" \
-l"C:\Program Files\Texas Instruments\ccsv4\tools\compiler\c5500\lib\55xdsp.lib" \
-l"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/lib/rts55.lib" \
"../55xdsph.lib" \
"../CSLc55x5h.lib" \
"../lnkx_Spectrum_Digital_UART_Ex.cmd" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include Debug_rts55h_32/subdir_vars.mk
-include subdir_rules.mk
-include Debug_rts55h_32/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Application_1_Modified.out

# Tool invocations
Application_1_Modified.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: Linker'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/bin/cl55" -v5515 -g --diag_warning=225 --sat_reassoc=off --ptrdiff_size=16 --fp_reassoc=off --no_bad_aliases --memory_model=small --asm_source=mnemonic -z -m"Application_1_Modified.map" --warn_sections -i"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/lib" -i"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c5500/include" --reread_libs --rom_model -o "Application_1_Modified.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C5500_EXECUTABLE_OUTPUTS__QTD) "Application_1_Modified.out"
	-$(RM) ".\Audio_To_MIDI_Using_DMA.obj" ".\Audio_To_MIDI_Using_DMA_and_CFFT.obj" ".\Audio_To_MIDI_Using_DMA_and_HWAFFT.obj" ".\DMA_Ping_Pong.obj" ".\I2C_Test.obj" ".\My_AIC3204.obj" ".\My_DMA_Ping_Pong_Register_Setup.obj" ".\My_I2C.obj" ".\My_I2S.obj" ".\My_I2S_Register.obj" ".\My_PLL.obj" ".\My_UART_Register.obj" ".\Output_MIDI.obj" ".\PLL.obj" ".\add.obj" ".\aic3204.obj" ".\aic3204_init.obj" ".\aic_i2c.obj" ".\cbrev.obj" ".\cbrev32.obj" ".\cfft32_noscale.obj" ".\cfft32_scale.obj" ".\cfft_noscale.obj" ".\cfft_scale.obj" ".\csl_intc.obj" ".\main.obj" ".\maxvec.obj" ".\mul32.obj" ".\sqrtv.obj" ".\stereo.obj" ".\usbstk5505.obj" ".\usbstk5505_gpio.obj" ".\usbstk5505_i2c.obj" ".\usbstk5505_led.obj" ".\vector.obj" 
	-$(RM) ".\add.pp" ".\cbrev.pp" ".\cbrev32.pp" ".\cfft32_noscale.pp" ".\cfft32_scale.pp" ".\cfft_noscale.pp" ".\cfft_scale.pp" ".\maxvec.pp" ".\mul32.pp" ".\sqrtv.pp" ".\vector.pp" 
	-$(RM) ".\Audio_To_MIDI_Using_DMA.pp" ".\Audio_To_MIDI_Using_DMA_and_CFFT.pp" ".\Audio_To_MIDI_Using_DMA_and_HWAFFT.pp" ".\DMA_Ping_Pong.pp" ".\I2C_Test.pp" ".\My_AIC3204.pp" ".\My_DMA_Ping_Pong_Register_Setup.pp" ".\My_I2C.pp" ".\My_I2S.pp" ".\My_I2S_Register.pp" ".\My_PLL.pp" ".\My_UART_Register.pp" ".\Output_MIDI.pp" ".\PLL.pp" ".\aic3204.pp" ".\aic3204_init.pp" ".\aic_i2c.pp" ".\csl_intc.pp" ".\main.pp" ".\stereo.pp" ".\usbstk5505.pp" ".\usbstk5505_gpio.pp" ".\usbstk5505_i2c.pp" ".\usbstk5505_led.pp" 
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
