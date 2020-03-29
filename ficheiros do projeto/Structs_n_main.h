#ifndef STRUCTS_N_MAIN_H_INCLUDED
#define STRUCTS_N_MAIN_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define IP_SIZE 20
#define VETOR_SIZE 100
#define PORT_SIZE 10

//Macro for Maximum between 2 numbers
#define max(A,B) ((A)>=(B)?(A):(B))

//All the ladies have their ID in their Purses
typedef struct
{
  //User chosen port
  char port[PORT_SIZE];
  //User introduced IP
  char IP[IP_SIZE];
}server_info;

//All the most used purses can be organized in a closet
typedef struct
{
  //My ID
  server_info Myinfo;
  //Next server ID
  server_info Next_info;
  //Previous server ID
  server_info Prev_info;
  //Next server next's ID (2º Next server ID)
  server_info SecondNext_info;
  //Server number
  int key;
  //Next Server key
  int succ_key;
  //Flag that indicates if we are in the ring
  bool inRing;
}all_info;

#endif // STRUCTS_N_MAIN_H_INCLUDED
