
#  Compiler
CC = gcc

#  Compiler Flags
CPPFLAGS=-D_POSIX_C_SOURCE=201112L -D_DEFAULT_SOURCE
CFLAGS = -W -Wall -std=c99 -pedantic -O3 -g $(CPPFLAGS)

#  Sources
SOURCES = main.c interface_n_aux.c Network.c

#  Objects
OBJECTS = main.o interface_n_aux.o Network.o

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

dkt: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

main.o:  main.c Structs_n_main.h interface_n_aux.h Network.h
Network.o: Network.c Structs_n_main.h Network.h interface_n_aux.h
interface_n_aux.o: interface_n_aux.c interface_n_aux.h

clean::
	rm -f *.o core a.out dkt *~
