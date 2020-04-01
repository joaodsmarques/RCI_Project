#ifndef TCP_H_INCLUDED
#define TCP_H_INCLUDED

#include "Structs_n_main.h"

#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


//void TCP_Server_Connect(Closet *);

int init_UDPsv(all_info* );

all_info* NewServer_Heart(all_info* );

all_info* SentryServer_Heart(all_info*);

int init_TCPsv (all_info* );

int TCP_InitnConect(char [], char []);


#endif // TCP_H_INCLUDED
