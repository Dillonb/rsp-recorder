PROJECT_ROOT = $(CURDIR)/../..
ROOTDIR = $(N64_INST)
GCCN64PREFIX = $(ROOTDIR)/bin/mips64-elf-
CHKSUM64PATH = $(ROOTDIR)/bin/chksum64
MKDFSPATH = $(ROOTDIR)/bin/mkdfs
HEADERPATH = $(ROOTDIR)/mips64-elf/lib
N64TOOL = $(ROOTDIR)/bin/n64tool
HEADERNAME = header
LINK_FLAGS = -L$(ROOTDIR)/mips64-elf/lib -ldragon -lc -lm -ldragonsys -Tn64.ld
CFLAGS = -std=gnu99 -march=vr4300 -mtune=vr4300 -O2 -Wall -Werror -I$(ROOTDIR)/mips64-elf/include
ASFLAGS = -mtune=vr4300 -march=vr4300
CC = $(GCCN64PREFIX)gcc
AS = $(GCCN64PREFIX)as
LD = $(GCCN64PREFIX)ld
OBJCOPY = $(GCCN64PREFIX)objcopy

ifeq ($(N64_BYTE_SWAP),true)
ROM_EXTENSION = .v64
N64_FLAGS = -b -l 2M -h $(HEADERPATH)/$(HEADERNAME) -o $(PROG_NAME)$(ROM_EXTENSION)
else
ROM_EXTENSION = .z64
N64_FLAGS = -l 2M -h $(HEADERPATH)/$(HEADERNAME) -o $(PROG_NAME)$(ROM_EXTENSION)
endif

PROG_NAME = rsp-recorder

all: $(PROG_NAME)$(ROM_EXTENSION)

emulator: all
	n64 $(PROG_NAME)$(ROM_EXTENSION)

console: all
	UNFLoader -r $(PROG_NAME)$(ROM_EXTENSION)

$(PROG_NAME)$(ROM_EXTENSION): $(PROG_NAME).elf
	$(OBJCOPY) $(PROG_NAME).elf $(PROG_NAME).bin -O binary
	rm -f $(PROG_NAME)$(ROM_EXTENSION)
	$(N64TOOL) $(N64_FLAGS) -t "RSP Recorder" $(PROG_NAME).bin
	$(CHKSUM64PATH) $(PROG_NAME)$(ROM_EXTENSION)

$(PROG_NAME).elf : $(PROG_NAME).o basic.o bios.o sys.o
	$(LD) -o $(PROG_NAME).elf $(PROG_NAME).o basic.o bios.o sys.o $(LINK_FLAGS)

bios.o: bios.c bios.h
	$(CC) -c $(CFLAGS) -o bios.o bios.c

sys.o: sys.c sys.h
	$(CC) -c $(CFLAGS) -o sys.o sys.c

text.section.bin data.section.bin: basic.S
	$(CC) -c -o tmp.o $^
	$(OBJCOPY) \
		--dump-section .text=text.section.bin \
		--dump-section .data=data.section.bin \
		tmp.o

basic.o: loader.S text.section.bin data.section.bin
	$(CC) -c -o basic.o loader.S

clean:
	rm -f *.v64 *.z64 *.elf *.o *.bin

.PHONY : clean
