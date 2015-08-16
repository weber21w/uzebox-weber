###############################################################################
# Makefile for solitaire
###############################################################################

## General Flags
PROJECT = snowpunkz
MCU = atmega644
TARGET = snowpunkz.elf
CC = avr-gcc


## Kernel settings
KERNEL_DIR = ../kernel

## Escape spaces in mixer path (due to custom sounds.inc)
EMPTY:=
SPACE:= $(EMPTY) $(EMPTY)
SPACE_ESC:= \\$(SPACE)
MIX_PATH:= $(realpath ../data/sound/sounds.inc)
MIX_PATH_ESC:= $(subst $(SPACE),$(SPACE_ESC),$(MIX_PATH))

KERNEL_OPTIONS = -DVIDEO_MODE=3 -DINTRO_LOGO=0 -DSOUND_CHANNEL_5_ENABLE=0 -DSOUND_MIXER=1 -DUART=1 -DUART_RX_BUFFER=1 -DUART_RX_BUFFER_SIZE=128 -DUART_TX_BUFFER=1 -DUART_TX_BUFFER_SIZE=32
KERNEL_OPTIONS += -DRAM_TILES_COUNT=16 -DMAX_SPRITES=9 -DCONTROLLERS_VSYNC_READ=0 -DTRUE_RANDOM_GEN=1
KERNEL_OPTIONS += -DSCREEN_TILES_V=26 -DFIRST_RENDER_LINE=28
KERNEL_OPTIONS += -DMIXER_WAVES=\"$(MIX_PATH_ESC)\"

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
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += -Wl,-Map=snowpunkz.map 
LDFLAGS += -Wl,-gc-sections 
LDFLAGS += -Wl,--relax
#LDFLAGS += -Wl,--section-start,.noinit=0x800100 -Wl,--section-start,.data=0x800500


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = uzeboxVideoEngineCore.o uzeboxCore.o uzeboxSoundEngine.o uzeboxSoundEngineCore.o uzeboxVideoEngine.o main.o

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Include Directories
INCLUDES = -I"$(KERNEL_DIR)" 

## Build
all: $(TARGET) snowpunkz.hex snowpunkz.eep snowpunkz.lss size
#../data/graphics/tiles.inc

## Regenerate the graphics include file
#../data/graphics/tiles.inc: ../data/graphics/tiles.png ../data/graphics/snowpunkzgconfig.xml
#	gconvert ../data/my-gconvert-config.xml

## Compile Kernel files


uzeboxVideoEngineCore.o: $(KERNEL_DIR)/uzeboxVideoEngineCore.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

uzeboxCore.o: $(KERNEL_DIR)/uzeboxCore.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uzeboxSoundEngineCore.o: $(KERNEL_DIR)/uzeboxSoundEngineCore.s
	$(CC) $(INCLUDES) $(ASMFLAGS) -c  $<

uzeboxSoundEngine.o: $(KERNEL_DIR)/uzeboxSoundEngine.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uzeboxVideoEngine.o: $(KERNEL_DIR)/uzeboxVideoEngine.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

## Compile game sources

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
	-rm -rf $(OBJECTS) snowpunkz.elf dep/* snowpunkz.hex snowpunkz.eep snowpunkz.lss snowpunkz.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)
