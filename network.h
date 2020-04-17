#ifndef NETWORK
#define NETWORK

#include "structs_n_main.h"

#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
//Initializ the structs and analyze the arguments of argv
void startup(int, char**, all_info*, ringfd*);

//Select initialization
int add_read_fd(fd_set*, ringfd);
//UDP and TCP sockets inits
int init_UDPsv(all_info*);
int init_UDPcl(all_info*, struct addrinfo**);
int init_TCP_Listen(all_info*);
int init_TCP_connect(char*, char*);
//Send and receive messages
void send_udp(int, char*, struct sockaddr*, socklen_t);
void recv_udp(int, char*, struct sockaddr*, socklen_t*);
void send_message(int, const char*);
int get_incoming(int);
int get_message(int,char*);
//Close descriptors and reset the node
void close_all(ringfd*, all_info*);
//The heart of the key search process
int Find_key(all_info,char*, ringfd);

#endif
