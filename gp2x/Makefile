
# Makefile for AlexKidd2x
#
# Copyright (C) Puck2099

NAME	  = alexkidd2x.gpe

GP2XPATH = /usr/local/gp2xdev/bin/gp2x-

CC = 	$(GP2XPATH)gcc
CPP =  	$(GP2XPATH)gcc -E
CXX =  	$(GP2XPATH)g++
STRIP =	$(GP2XPATH)strip
LD =  	$(GP2XPATH)ld
AR =  	$(GP2XPATH)ar
AS =  	$(GP2XPATH)as

CFLAGS    = -I/usr/local/gp2xdev/include
DEFINES   = -DLSB_FIRST -DGP2X -DALIGN_DWORD -DUSE_ZLIB -DVERSION=\"v0.5\"
INCLUDES  = -I. -I.. -I../cpu -I../sound -I../unzip -Iminimal -Iasm -Iimg_headers
LIBS	  = -L/usr/local/gp2xdev/lib -Wl,-rpath,/usr/local/gp2xdev/lib -lpthread -lm -lpng -lz -static

#CFLAGS    += -g
CFLAGS    += -O3 -fomit-frame-pointer -ffast-math

#CFLAGS    += 
#CFLAGS	  += -mcpu=arm920 -mtune=arm920t -O3 -fstrict-aliasing -fexpensive-optimizations -falign-functions -fweb -frename-registers -fomit-frame-pointer -ffast-math -finline -finline-functions -fno-builtin -fno-common -mstructure-size-boundary=8 -msoft-float -Wall
#CFLAGS	  += -mcpu=arm920 -mtune=arm920t -O3 -fstrict-aliasing -fexpensive-optimizations -falign-functions -fweb -frename-registers -fomit-frame-pointer -ffast-math -finline -finline-functions -fno-builtin -fno-common -mstructure-size-boundary=8 -msoft-float -Wall
#CFLAGS	  += -mcpu=arm920 -mtune=arm920t -O3 -fstrict-aliasing -fexpensive-optimizations -falign-functions -fweb -frename-registers -ffast-math -finline -finline-functions -fno-builtin -fno-common -mstructure-size-boundary=8 -msoft-float -Wall -pg

#CFLAGS	  += -mcpu=arm920 -mtune=arm920t -std=c99 -O3 -fstrict-aliasing -fexpensive-optimizations -falign-functions -fweb -frename-registers -fomit-frame-pointer -ffast-math -finline -finline-functions -fno-builtin -fno-common -mstructure-size-boundary=8 -msoft-float -pedantic

#DEFINES   += -DDEBUG

OBJECTS   = ../cpu/z80.o ../sms.o ../pio.o ../memz80.o ../render.o ../vdp.o ../system.o ../error.o \
	    ../fileio.o ../state.o ../loadrom.o \
	    ../unzip/ioapi.o ../unzip/unzip.o \
	    ../sound/sound.o ../sound/sn76489.o ../sound/emu2413.o ../sound/ym2413.o ../sound/fmintf.o ../sound/stream.o \
	    main.o config.o menu.o smsrom_good.o video.o input.o usbjoy.o messages.o \
	    minimal/minimal.o minimal/sprites.o minimal/minimal_image.o minimal/cpuctrl.o \
	    asm/memcpy.o asm/memset.o asm/strlen.o asm/strncmp.o asm/memcmp.o asm/strcmp.o asm/asmlib.o

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@
	$(STRIP) $(NAME)

.c.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

clean:
	rm -f $(OBJECTS) $(NAME)
