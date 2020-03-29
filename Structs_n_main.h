#ifndef STRUCTS_N_MAIN_H_INCLUDED
#define STRUCTS_N_MAIN_H_INCLUDED

//#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define IP_SIZE 128
#define VETOR_SIZE 100
#define max(A,B) ((A)>=(B)?(A):(B))

struct info
{
	bool inRing;
	int key;
	int succ_key;
	//COMPLETAR COM RESTO DAS INFORMAÇ~OES
	//struct server 2nd succ ip and port
}sv_info;


#endif // STRUCTS_N_MAIN_H_INCLUDED
