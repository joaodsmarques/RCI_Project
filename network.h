#ifndef NETWORK
#define NETWORK

#include "structs_n_main.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

void startup(int, char**, all_info*, ringfd*);
int add_read_fd(fd_set*, ringfd);
//ATENCAO A ESTE
int init_UDPsv(all_info*);
int init_TCP_Listen(all_info*);
int init_TCP_connect(all_info*);
void send_message(int, const char*);
int get_incoming(int);
int get_message(int,char*);
int isAlive(int, fd_set*);



#endif
