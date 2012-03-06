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
physicsengine.elf.size \

ELFCHECK += \
physicsengine.elf.elfcheck \


# All Target
all: physicsengine.elf secondary-outputs

# Tool invocations
physicsengine.elf: $(OBJS) ../src/lscript.ld $(USER_OBJS)
	@echo Building target: $@
	@echo Invoking: MicroBlaze gcc linker
	mb-gcc -Wl,-T -Wl,../src/lscript.ld -L../../xilkernel_bsp_1/microblaze_1/lib -mxl-barrel-shift -mxl-pattern-compare -mcpu=v8.10.a -mno-xl-soft-mul -mhard-float -o"physicsengine.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo Finished building target: $@
	@echo ' '

physicsengine.elf.size: physicsengine.elf
	@echo Invoking: MicroBlaze Print Size
	mb-size physicsengine.elf  |tee "physicsengine.elf.size"
	@echo Finished building: $@
	@echo ' '

physicsengine.elf.elfcheck: physicsengine.elf
	@echo Invoking: Xilinx ELF Check
	elfcheck physicsengine.elf -hw ../../Server_Final_hw_platform/system.xml -pe microblaze_1  |tee "physicsengine.elf.elfcheck"
	@echo Finished building: $@
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES)$(ELFSIZE)$(ELFCHECK)$(S_UPPER_DEPS) physicsengine.elf
	-@echo ' '

secondary-outputs: $(ELFSIZE) $(ELFCHECK)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
