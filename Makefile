
#  Compiler
CC = gcc

#  Compiler Flags
CPPFLAGS=-D_POSIX_C_SOURCE=201112L -D_DEFAULT_SOURCE
CFLAGS = -W -Wall -std=c99 -pedantic -O3 -g $(CPPFLAGS)

#  Sources
SOURCES = main.c interface.c network.c

#  Objects
OBJECTS = main.o interface.o network.o

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

dkt: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

main.o:  main.c structs_n_main.h interface.h network.h
network.o: network.c network.h structs_n_main.h 
interface.o: interface.c interface.h #network.h


clean::
	rm -f *.o core a.out dkt *~

