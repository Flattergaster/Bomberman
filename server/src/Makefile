CFLAGS = -g -O2 -Wall -pedantic -ansi
LFLAGS = -lpthread                                                               

vpath %.h ../include/                                                                   

main.o: main.c server.h                                                                
		gcc -c $< $(CFLAGS) $(LFLAGS) 

clean:                                                                                 
		rm -rf *.o *.out
