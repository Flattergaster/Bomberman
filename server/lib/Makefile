CFLAGS = -g -O2 -Wall -pedantic -ansi
LFLAGS = -lpthread                                                               

vpath %.h ../include/                                                                   

server.o: server.c server.h                                                                
		gcc -c $< $(CFLAGS) $(LFLAGS) 

clean:                                                                                 
		rm -rf *.o *.out
