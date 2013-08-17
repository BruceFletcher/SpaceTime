#
# This build environment requires a symbolic link to the AVR libraries and headers
# you are compiling against, roughly like so:
#
# ln -s /usr/local/CrossPack-AVR/avr-4 avr
#

PRG            = spacetime
OBJ            = main.o uart.o shift.o timer.o display.o keypad.o serial.o clock.o edit.o
PROGRAMMER     = avrisp2
PORT           = usb
MCU_TARGET     = atmega328p
AVRDUDE_TARGET = atmega328p
OPTIMIZE       = -Os
DEFS           = -Iavr/include
LIBS           = -Lavr/lib/avr5

HZ          = 14745600

CC             = /opt/local/bin/avr-gcc

override CFLAGS        = -g -DF_CPU=$(HZ) -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
override LDFLAGS       = -Wl,-Map,$(PRG).map

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all: crtm328p.o $(PRG).elf lst text #eeprom

$(PRG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

# No idea why this file isn't found automatically by ld, but linking it to ./ seems to work
crtm328p.o:
	/bin/ln -s avr/lib/avr5/crtm328p.o crtm328p.o

clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak *.hex *.bin *.srec
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)

lst:  $(PRG).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

# Rules for building the .text rom images

text: hex bin srec

hex:  $(PRG).hex
bin:  $(PRG).bin
srec: $(PRG).srec

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

# command to program chip (invoked by running "make install")
install:  crtm328p.o $(PRG).hex
	avrdude -p $(AVRDUDE_TARGET) -c $(PROGRAMMER) -P $(PORT) -v  \
         -U flash:w:$(PRG).hex

# Chip received as lfuse 62, hfuse d9, efuse 7
fuse:
	avrdude -p $(AVRDUDE_TARGET) -c $(PROGRAMMER) -P $(PORT) -v \
	-U lfuse:w:0xd6:m -U hfuse:w:0xdf:m 

