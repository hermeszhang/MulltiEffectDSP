################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
Delay2/delay2.obj: ../Delay2/delay2.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"E:/Zainstalowane Programy/ti/ccsv6/tools/compiler/c5500_4.4.1/bin/cl55" -v5515 --memory_model=huge -g --include_path="E:/Zainstalowane Programy/ti/ccsv6/tools/compiler/c5500_4.4.1/include" --include_path="C:/Users/Rino Reyns/Desktop/DSP_Inzynierka/common5535" --define=c5535 --display_error_number --diag_warning=225 --ptrdiff_size=32 --preproc_with_compile --preproc_dependency="Delay2/delay2.d" --obj_directory="Delay2" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

