PROP_SYS_ARCH_S = startup_stm32f301xe.s

### Compiler definitions
GCC      = 1
CC       = arm-none-eabi-gcc
LD       = arm-none-eabi-gcc
SIZE     = arm-none-eabi-size
AS       = arm-none-eabi-as
AR       = arm-none-eabi-ar
NM       = arm-none-eabi-nm
OBJCOPY  = arm-none-eabi-objcopy
STRIP    = arm-none-eabi-strip

%.ihex: %.$(TARGET)
	$(OBJCOPY) -O ihex $^ $@
	$(OBJCOPY) -O ihex $^ "contikiSTM32.hex"
	$(OBJCOPY) -O ihex $^ "contikiSTM32.hex"
	$(OBJCOPY) $^ "contikiSTM32.elf"
	
	
%.bin: %.$(TARGET)
	$(OBJCOPY) -O binary $^ $@
%.co: %.c
	$(CC) $(CFLAGS) -c -DAUTOSTART_ENABLE $< -o $@


ifndef LDSCRIPT
LDSCRIPT = $(CONTIKI)/platform/$(TARGET)/stm32cube-lib/stm32cube-prj/linker/gcc/stm32f301RETx_FLASH.ld
endif

#ASFLAGS += -mcpu=cortex-m4 -mthumb

CFLAGS+=\
	    -I$(CONTIKI)/cpu/arm/common/CMSIS   \
            -Wall -g -O0                        \
            -DWITH_UIP -DWITH_ASCII             \
            -mcpu=cortex-m4                     \
	    -mthumb				\
	    -munaligned-access		\
            -std=gnu99                          \
            -Wl,-cref                           \
	    -D __SOURCEFILE__=\"$*.c\"


LDFLAGS += -Wl,-Map=contiki-$(TARGET).map,--cref,--no-warn-mismatch


LDLIBS += $(CONTIKI_CPU)/lib/smallprintf_thumb2.a



LDFLAGS += -mcpu=cortex-m4 -mthumb  -mfloat-abi=soft -T$(LDSCRIPT) -Wl,-Map=output.map -Wl,--gc-sections -lm 


LDFLAGS += $(LDLIBS)

SMALL=1
ifeq ($(SMALL),1)
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections

LDFLAGS += -Wl,--undefined=_reset_vector__,--undefined=InterruptVectors,--undefined=_copy_data_init__,--undefined=_clear_bss_init__,--undefined=_end_of_init__

endif   # /SMALL


