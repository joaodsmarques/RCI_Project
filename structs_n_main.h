#ifndef STRUCTS_N_MAIN_H_INCLUDED
#define STRUCTS_N_MAIN_H_INCLUDED

//#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>
#include <signal.h>
#include <stdio.h>

#define IP_SIZE 20
#define VETOR_SIZE 100
#define PORT_SIZE 10
#define RING_SIZE 16
#define max(A,B) ((A)>=(B)?(A):(B))

typedef struct
{
  //User chosen port
  char port[PORT_SIZE];
  //User introduced IP
  char IP[IP_SIZE];
}server_info;

typedef struct ringfd
{
  //Fd relative to the predecessor TCP
  int prev;
  //Fd for the successor TCP
  int next;
  //Fd for UDP connection
  int udp;
  //Fd listening for TCP
  int listen;
  //Fd
  int temp;
}ringfd;

//All the most used purses can be organized in a closet
typedef struct
{
  //My ID
  server_info Myinfo;
  //Next server ID
  server_info Next_info;
  //Next server next's ID (2º Next server ID)
  server_info SecondNext_info;
  //Server number
  int key;
  //Next Server key
  int succ_key;

  int second_succ_key;
  //Flag that indicates if we are in the ring
  bool inRing;
}all_info;


#endif // STRUCTS_N_MAIN_H_INCLUDED
