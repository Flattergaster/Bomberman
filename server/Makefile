IDIR = include
LDIR = lib
SDIR = src
LFLAGS = -lpthread 
CFLAGS = -g -O2 -Wall -pedantic -ansi
CC = gcc

SUBDIRS = $(LDIR) $(SDIR)
OBJSERV = $(SDIR)/main.o $(LDIR)/server.o

vpath %.c $(SDIR) : $(LDIR)
vpath %.o $(SDIR) : $(LDIR)
vpath %.h $(IDIR)

all: server

server: $(OBJSERV)
		$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS) 

$(OBJSERV): prepare

.PHONY: prepare $(SUBDIRS)
prepare: $(SUBDIRS)

$(SUBDIRS):
		make -C $@
		
$(SDIR): $(LDIR)

clean:
		cd src	&& $(MAKE) clean
		cd lib	&& $(MAKE) clean
		rm -rf server client   *.o
