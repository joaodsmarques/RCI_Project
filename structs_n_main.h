#ifndef STRUCTS_N_MAIN_H_INCLUDED
#define STRUCTS_N_MAIN_H_INCLUDED

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdbool.h>
#include <stdio.h>

#define IP_SIZE 20
#define VETOR_SIZE 100
#define PORT_SIZE 10
#define RING_SIZE 16
#define MSG_SIZE 50
#define max(A,B) ((A)>=(B)?(A):(B))

//Single Server info struct
typedef struct
{
  //User chosen port
  char port[PORT_SIZE];
  //User introduced IP
  char IP[IP_SIZE];
}server_info;

//File descriptors struct
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
  //temporary fd for the accepting calls
  int temp;
}ringfd;
//Server and its actual connections struct
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
  //successor key
  int succ_key;
  //2º successor key
  int second_succ_key;
  //Flag that indicates if we are in the ring
  bool inRing;
}all_info;


#endif // STRUCTS_N_MAIN_H_INCLUDED
