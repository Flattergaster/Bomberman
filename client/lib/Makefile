CFLAGS = -g -O2 -Wall -pedantic -ansi
LFLAGS =

vpath %.h ../include/

core.o: core.c core.h
		gcc -c $< $(CFLAGS) $(LFLAGS)

clean:
		rm -rf *.o
