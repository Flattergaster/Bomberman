IDIR = include
LDIR = lib
SDIR = src
LFLAGS = -lpthread -lncurses -lform -lmenu -lpanel
CFLAGS = -std=c99 -O2 -Wall -pedantic -ansi -g
CC = gcc

SUBDIRS = $(LDIR) $(SDIR)
OBJCLIENT = $(SDIR)/client.o $(LDIR)/core.o $(LDIR)/gui.o

vpath %.c $(SDIR) : $(LDIR)
vpath %.o $(SDIR) : $(LDIR)
vpath %.h $(IDIR)

all: client

client: $(OBJCLIENT)
		$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS) 

$(OBJCLIENT): prepare

.PHONY: prepare $(SUBDIRS)
prepare: $(SUBDIRS)

$(SUBDIRS):
		make -C $@
		
$(SDIR): $(LDIR)

clean:
		clear
		cd src	&& $(MAKE) clean
		cd lib	&& $(MAKE) clean
		rm -rf client *.o
