################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
ELFSIZE += \
controller_version_6_sdk.elf.size \

ELFCHECK += \
controller_version_6_sdk.elf.elfcheck \


# All Target
all: controller_version_6_sdk.elf secondary-outputs

# Tool invocations
controller_version_6_sdk.elf: $(OBJS) ../src/lscript.ld $(USER_OBJS)
	@echo Building target: $@
	@echo Invoking: MicroBlaze gcc linker
	mb-gcc -Wl,-T -Wl,../src/lscript.ld -L../../xilkernel_bsp_0/microblaze_0/lib -mxl-barrel-shift -mxl-pattern-compare -mcpu=v8.10.a -mno-xl-soft-mul -mhard-float -o"controller_version_6_sdk.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo Finished building target: $@
	@echo ' '

controller_version_6_sdk.elf.size: controller_version_6_sdk.elf
	@echo Invoking: MicroBlaze Print Size
	mb-size controller_version_6_sdk.elf  |tee "controller_version_6_sdk.elf.size"
	@echo Finished building: $@
	@echo ' '

controller_version_6_sdk.elf.elfcheck: controller_version_6_sdk.elf
	@echo Invoking: Xilinx ELF Check
	elfcheck controller_version_6_sdk.elf -hw ../../hw_platform_0/system.xml -pe microblaze_0  |tee "controller_version_6_sdk.elf.elfcheck"
	@echo Finished building: $@
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES)$(ELFSIZE)$(ELFCHECK)$(S_UPPER_DEPS) controller_version_6_sdk.elf
	-@echo ' '

secondary-outputs: $(ELFSIZE) $(ELFCHECK)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
