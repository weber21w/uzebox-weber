###############################################################################
# Makefile for columns
###############################################################################

## General Flags
PROJECT = columns
MCU = atmega644
TARGET = columns.elf
CC = avr-gcc


## Kernel settings
KERNEL_DIR = ../kernel
MIXER_WAVES = "../../src/data/sound/sounds.inc"

KERNEL_OPTIONS  = -DVIDEO_MODE=3 -DINTRO_LOGO=0 -DSOUND_CHANNEL_4_ENABLE=1
KERNEL_OPTIONS += -DRAM_TILES_COUNT=35 -DMAX_SPRITES=0 -DSCREEN_TILES_V=26
KERNEL_OPTIONS += -DSOUND_MIXER=1 -DFIRST_RENDER_LINE=36
KERNEL_OPTIONS += -DINCLUDE_DEFAULT_WAVES=1 -DCONTROLLERS_VSYNC_READ=1


## Demo settings
DEMO_OPTIONS = -DFORMAT_EEPROM=0

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=28636360UL -Os -fsigned-char -ffunction-sections 
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 
CFLAGS += $(KERNEL_OPTIONS)
CFLAGS += $(DEMO_OPTIONS)
CFLAGS += -mcall-prologues -fdata-sections -ffreestanding -fomit-frame-pointer

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2 -nodefaultlibs -nostartfiles

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += -Wl,-Map=columns.map 
LDFLAGS += -Wl,-gc-sections 
LDFLAGS += -Wl,--relax


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = uzeboxVideoEngineCore.o uzeboxCore.o uzeboxSoundEngine.o uzeboxSoundEngineCore.o uzeboxVideoEngine.o main.o cpuasm.o

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Include Directories
INCLUDES = -I"$(KERNEL_DIR)" 

## Build
all: $(TARGET) columns.hex columns.eep columns.lss size
#../data/graphics/tiles.inc

## Regenerate the graphics include file
#../data/graphics/tiles.inc: ../data/graphics/tiles.png ../data/graphics/columnsgconfig.xml
#	gconvert ../data/my-gconvert-config.xml

## Compile Kernel files
uzeboxVideoEngineCore.o: $(KERNEL_DIR)/uzeboxVideoEngineCore.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

uzeboxSoundEngineCore.o: $(KERNEL_DIR)/uzeboxSoundEngineCore.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

uzeboxCore.o: $(KERNEL_DIR)/uzeboxCore.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uzeboxSoundEngine.o: $(KERNEL_DIR)/uzeboxSoundEngine.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uzeboxVideoEngine.o: $(KERNEL_DIR)/uzeboxVideoEngine.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

## Compile game sources
cpuasm.o: ../cpuasm.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

main.o: ../main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) columns.elf dep/* columns.hex columns.eep columns.lss columns.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)
