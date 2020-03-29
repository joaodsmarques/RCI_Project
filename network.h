#ifndef NETWORK
#define NETWORK

#include "Structs_n_main.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

void startup(int, char**);
int init_UDPsv(char**);

void TCP_Server_Connect(char*, char*);


#endif
