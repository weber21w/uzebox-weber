###############################################################################
# Makefile for the project Dig Dug
###############################################################################

## General Flags
PROJECT = DIGDUG
GAME= DIGDUG
MCU = atmega644
TARGET = $(GAME).elf
CC = avr-gcc
INFO=../gameinfo.properties

## Escape spaces in mixer path (due to custom sounds.inc)
EMPTY:=
SPACE:= $(EMPTY) $(EMPTY)
SPACE_ESC:= \\$(SPACE)
MIX_PATH:= $(realpath ../data/sound/sounds.inc)
MIX_PATH_ESC:= $(subst $(SPACE),$(SPACE_ESC),$(MIX_PATH))
KERNEL_OPTIONS += -DMIXER_WAVES=\"$(MIX_PATH_ESC)\"

## Kernel settings
KERNEL_DIR = ../../../kernel
UZEBIN_DIR = ../../../bin/
KERNEL_OPTIONS += -DVIDEO_MODE=13 -DEXTENDED_PALETTE=1 -DSCROLLING=1 -DINTRO_LOGO=0
KERNEL_OPTIONS += -DMAX_SPRITES=64 -DRAM_TILES_COUNT=56 -DSCREEN_TILES_V=24 -DFIRST_RENDER_LINE=34
KERNEL_OPTIONS += -DSOUND_MIXER=1 -DSOUND_CHANNEL_5_ENABLE=0 -DSOUND_CHANNEL_5_ENABLE=0 -DSONG_SPEED=1 
##KERNEL_OPTIONS += -DUART=1 -DUART_TX_BUFFER_SIZE=2 -DUART_RX_BUFFER_SIZE=4

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=28636360UL -Os -fsigned-char -ffunction-sections -fno-toplevel-reorder
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 
CFLAGS += $(KERNEL_OPTIONS)
CFLAGS += -mcall-prologues -fdata-sections -ffreestanding -fomit-frame-pointer -fmerge-all-constants


## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += -Wl,-Map=$(GAME).map 
LDFLAGS += -Wl,-gc-sections
LDFLAGS += -T $(KERNEL_DIR)/uzebox_linkscript.x

## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = uzeboxVideoEngineCore.o uzeboxCore.o uzeboxSoundEngine.o uzeboxSoundEngineCore.o uzeboxVideoEngine.o $(GAME).o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Include Directories
INCLUDES = -I"$(KERNEL_DIR)" 

## Build
all: ../data/graphics/ddulogo.inc ../data/graphics/ddtitle.inc ../data/graphics/ddenemypanel.inc ../data/graphics/ddgametiles.inc ../data/graphics/ddsprites.inc $(TARGET) $(GAME).hex $(GAME).eep $(GAME).lss $(GAME).uze size

## Rebuild graphical resources
../data/graphics/ddulogo.inc: ../data/graphics/ddulogo.png ../data/graphics/ddulogo.xml
	$(UZEBIN_DIR)gconvert ../data/graphics/ddulogo.xml

../data/graphics/ddtitle.inc: ../data/graphics/ddtitle.png ../data/graphics/ddtitle.xml
	$(UZEBIN_DIR)gconvert ../data/graphics/ddtitle.xml

../data/graphics/ddenemypanel.inc: ../data/graphics/ddenemypanel.png ../data/graphics/ddenemypanel.xml
	$(UZEBIN_DIR)gconvert ../data/graphics/ddenemypanel.xml

../data/graphics/ddgametiles.inc: ../data/graphics/ddgametiles.png ../data/graphics/ddgametiles.xml
	$(UZEBIN_DIR)gconvert ../data/graphics/ddgametiles.xml

../data/graphics/ddsprites.inc: ../data/graphics/ddsprites.png ../data/graphics/ddsprites.xml
	$(UZEBIN_DIR)gconvert ../data/graphics/ddsprites.xml


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
$(GAME).o: ../digdug.c
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

%.uze: $(TARGET)
	-$(UZEBIN_DIR)packrom $(GAME).hex $@ $(INFO)

UNAME := $(shell sh -c 'uname -s 2>/dev/null || echo not')
AVRSIZEFLAGS := -A ${TARGET}
ifneq (,$(findstring MINGW,$(UNAME)))
AVRSIZEFLAGS := -C --mcu=${MCU} ${TARGET}
endif

size: ${TARGET}
	@echo
	@avr-size ${AVRSIZEFLAGS}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) $(GAME).* dep/* *.uze


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

