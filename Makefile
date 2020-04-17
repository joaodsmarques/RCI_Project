
#  Compiler
CC = gcc

#  Compiler Flags
CPPFLAGS=-D_POSIX_C_SOURCE=201112L -D_DEFAULT_SOURCE
CFLAGS = -W -Wall -std=c99 -pedantic -O3 -g $(CPPFLAGS)

#  Sources
SOURCES = main.c interface_n_aux.c network.c

#  Objects
OBJECTS = main.o interface_n_aux.o network.o

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

dkt: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

main.o:  main.c structs_n_main.h interface_n_aux.h network.h
network.o: network.c network.h interface_n_aux.h structs_n_main.h 
interface_n_aux.o: interface_n_aux.c interface_n_aux.h #network.h


clean::
	rm -f *.o core a.out dkt *~
