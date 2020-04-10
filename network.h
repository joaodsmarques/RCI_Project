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
int add_write_fd(int, fd_set*, int, int);
void setnonblocking(int);
int init_UDPsv(all_info*);
void init_UDPcl(all_info*);
int init_TCPsv(all_info*);
int init_TCPcl(all_info*);
void send_message(int, const char*);
int get_incoming(int);
void get_message(int,char*);




#endif
