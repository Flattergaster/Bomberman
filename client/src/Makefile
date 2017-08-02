CFLAGS = -O2 -Wall -pedantic -ansi -g
LFLAGS =

vpath %.h ../include/
vpath %.c ../lib/

client.o: client.c core.c gui.c
		gcc -c $< $(CFLAGS) $(LFLAGS)

clean:                                                                                 
		rm -rf *.o *.out
