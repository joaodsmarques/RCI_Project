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


int init_UDPsv(all_info* );

/*all_info* NewServer_Heart(all_info* );

all_info* SentryServer_Heart(all_info*);*/

int init_TCP_listen (char[]);

int TCP_InitnConect(char [], char []);

ringfd Sentry_Startup(all_info**);

all_info* Server_Heart(all_info*, int , ringfd);

#endif // TCP_H_INCLUDED
